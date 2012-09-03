/*
 * Copyright (c) 2010 Inside Secure, All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.opennfc;

import java.io.IOException;

import org.opennfc.p2p.P2PConnection;
import org.opennfc.p2p.P2PLink;
import org.opennfc.p2p.P2PManager.Mode;

class P2PConnectionImpl implements P2PConnection {

    /** */
    private SafeHandle mSocketSafeHandle = null;

    /**
     * Operation safe handles
     */
    private SafeHandle mOperationWrite = new SafeHandle();

    private SafeHandle mOperationRead = new SafeHandle();

    /* package private */P2PConnectionImpl(Mode mode, String serviceUri, byte sap) throws NfcException {

        if (mode == null) {
            throw new NullPointerException("mode==null");
        }

        mSocketSafeHandle = new SafeHandle();

        checkError(MethodAutogen.WP2PCreateSocket(mode.getValue(), serviceUri, sap, mSocketSafeHandle.getReference()));
    }

    public void cancel() {

        if (!mOperationRead.isNull()) {
            MethodAutogen.WBasicCancelOperation(mOperationRead.getValue());
        }
        if (!mOperationWrite.isNull()) {
            MethodAutogen.WBasicCancelOperation(mOperationWrite.getValue());
        }
    }

    public void close() throws IOException {

        if (!mSocketSafeHandle.isNull()) {
            mSocketSafeHandle.release();
        } else {
            throw new IOException("The connection is broken");
        }
    }

    public void connect(P2PLink link) throws IOException, NfcException {

        if (link == null) {
            throw new IllegalArgumentException("link==null");
        }

        synchronized (this) {
            if (mSocketSafeHandle.isNull()) {
                throw new IOException("The socket is broken");
            }

            P2PLinkImpl linkImpl = (P2PLinkImpl) link;
            CallbackGenericCompletion establishmentCallback = new CallbackGenericCompletion();
            MethodAutogen.WP2PConnect(mSocketSafeHandle.getValue(), linkImpl.getSafeHandle(), establishmentCallback.getId());

            checkError(establishmentCallback.waitForResult());
        }
    }

    public int getParameter(P2PConnectionParameter parameter) throws IOException, NfcException {

        if (mSocketSafeHandle.isNull())
            throw new IOException("The connection is not established");

        int value[] = new int[1];

        checkError(MethodAutogen.WP2PGetSocketParameter(mSocketSafeHandle.getValue(), parameter.getValue(), value));

        return value[0];
    }

    public void setParameter(P2PConnectionParameter parameter, int value) throws IOException, NfcException {

        if (mSocketSafeHandle.isNull())
            throw new IOException("The connection is not established");

        if ((parameter != P2PConnectionParameter.LOCAL_MIU) && (parameter != P2PConnectionParameter.LOCAL_RW))
            throw new IllegalArgumentException("The parameter is not valid");

        checkError(MethodAutogen.WP2PSetSocketParameter(mSocketSafeHandle.getValue(), parameter.getValue(), value));
    }

    public int read(byte[] data) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        }
        return read(data, 0, data.length);
    }

    public int read(byte[] data, int off, int len) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        } else if (len == 0) {
            return 0;
        } else if (off < 0 || len < 0) {
            throw new IllegalArgumentException("offset or length are negative");
        }
        /* IndexOutOfBoundsException is handled by ByteBufferReference */

        int receivedLength = 0;

        CallbackP2PReadCompleted mReadCallBack = new CallbackP2PReadCompleted();

        ByteBufferReference pReceptionBuffer = new ByteBufferReference(data, off, len, false);
        MethodAutogen.WP2PRead(mSocketSafeHandle.getValue(), mReadCallBack.getId(), pReceptionBuffer.getValue(),
                mOperationRead.getReference());
        try {
            receivedLength = mReadCallBack.waitForResultOrException();
        } catch (NfcException e) {
            if (e.getCode() == NfcErrorCode.BAD_STATE)
                throw new IllegalStateException(e.getMessage());
            else if (e.getCode() == NfcErrorCode.RETRY)
                throw new IllegalStateException(e.getMessage());
            throw new IOException(e.getMessage());
        } finally {
            pReceptionBuffer.release(true);
            if (!mOperationRead.isNull()) {
                MethodAutogen.WBasicCloseHandle(mOperationRead.getValue());
            }
        }

        return receivedLength;
    }

    public void write(byte[] data) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        }
        write(data, 0, data.length);
    }

    public void write(byte[] data, int off, int len) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        } else if (len == 0) {
            return;
        } else if (off < 0 || len < 0) {
            throw new IllegalArgumentException("offset or length are negative");
        }

        /* IndexOutOfBoundsException is handled by ByteBufferReference */

        CallbackGenericCompletion mWriteCallBack = new CallbackGenericCompletion();

        ByteBufferReference pSendBuffer = new ByteBufferReference(data, off, len, true);

        MethodAutogen.WP2PWrite(mSocketSafeHandle.getValue(), mWriteCallBack.getId(), pSendBuffer.getValue(),
                mOperationWrite.getReference());
        try {
            mWriteCallBack.waitForCompletionOrException();
        } catch (NfcException e) {
            if (e.getCode() == NfcErrorCode.BAD_STATE)
                throw new IllegalStateException(e.getMessage());
            else if (e.getCode() == NfcErrorCode.RETRY)
                throw new IllegalStateException(e.getMessage());
            throw new IOException(e.getMessage());
        } finally {
            pSendBuffer.release(false);
            if (!mOperationWrite.isNull()) {
                MethodAutogen.WBasicCloseHandle(mOperationWrite.getValue());
            }
        }
    }

    public void shutdown() throws IOException, NfcException {

        synchronized (this) {

            if (mSocketSafeHandle.isNull()) {
                throw new IOException("The socket is broken");
            }

            CallbackGenericCompletion releaseCallback = new CallbackGenericCompletion();

            MethodAutogen.WP2PShutdown(mSocketSafeHandle.getValue(), releaseCallback.getId());

            int nError = releaseCallback.waitForResult();
            if (nError == ConstantAutogen.W_ERROR_BAD_STATE) {
                throw new IOException("The connection is not established");
            }
            checkError(nError);
        }
    }

    private static void checkError(int error) throws NfcException {

        if (error != ConstantAutogen.W_SUCCESS) {
            switch (error) {
                case ConstantAutogen.W_ERROR_BAD_STATE:
                    throw new IllegalStateException("the connection is already established or the P2P "
                            + "link is no longer established.");
                case ConstantAutogen.W_ERROR_OUT_OF_RESOURCE:
                    throw new OutOfMemoryError("No enough resource available to "
                            + "complete the operation, the quota of sockets have " + "been reached.");
                case ConstantAutogen.W_ERROR_BAD_PARAMETER:
                    throw new IllegalArgumentException("the combination of SAP and " + "sericeUri is not a valid configuration");
                case ConstantAutogen.W_ERROR_EXCLUSIVE_REJECTED:
                    throw new IllegalStateException("the specified SAP or sericeUri " + "value is already in use");
                default:
                    throw new NfcException("An NFC error occurred: ", NfcErrorCode.getCode(error));
            }
        }
    }

    private class CallbackP2PReadCompleted extends Callback_tWBasicGenericDataCallbackFunction {

        protected CallbackP2PReadCompleted() {
            super(false);
        }

        @Override
        protected void tWBasicGenericDataCallbackFunction(int dataLength, int result) {
            if (result != ConstantAutogen.W_SUCCESS)
                signalException(new NfcException(NfcErrorCode.getCode(result)));
            else
                signalResult(dataLength);
        }
    }
}
