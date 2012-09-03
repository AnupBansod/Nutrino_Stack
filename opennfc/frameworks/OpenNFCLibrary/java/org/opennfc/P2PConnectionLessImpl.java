
package org.opennfc;

import java.io.IOException;

import org.opennfc.p2p.P2PConnectionLess;
import org.opennfc.p2p.P2PConnection.P2PConnectionParameter;

class P2PConnectionLessImpl implements P2PConnectionLess {

    /** */
    private SafeHandle mSocketSafeHandle = null;

    /**
     * Operation safe handles
     */
    private SafeHandle mOperationSendTo = new SafeHandle();

    private SafeHandle mOperationRecvFrom = new SafeHandle();

    /* package private */P2PConnectionLessImpl(String serviceUri, byte sap) throws NfcException {

        mSocketSafeHandle = new SafeHandle();

        checkError(MethodAutogen.WP2PCreateSocket(ConstantAutogen.W_P2P_TYPE_CONNECTIONLESS,
                serviceUri, sap, mSocketSafeHandle.getReference()));
    }

    public void cancel() {

        if (!mOperationSendTo.isNull()) {
            MethodAutogen.WBasicCancelOperation(mOperationSendTo.getValue());
        }
        if (!mOperationRecvFrom.isNull()) {
            MethodAutogen.WBasicCancelOperation(mOperationRecvFrom.getValue());
        }
    }

    public void close() throws IOException {

        if (!mSocketSafeHandle.isNull()) {
            mSocketSafeHandle.release();
        } else {
            throw new IOException("The connectionLess is broken");
        }
    }

    public byte getLocalSap() throws IOException, NfcException {

        if (mSocketSafeHandle.isNull())
            throw new IOException("The connection is not established");

        int value[] = new int[1];

        checkError(MethodAutogen.WP2PGetSocketParameter(mSocketSafeHandle.getValue(),
                P2PConnectionParameter.LOCAL_SAP.getValue(), value));

        return (byte) value[0];
    }

    public boolean ready() throws IOException {
        throw new IOException("Not implemeted");
    }

    public int recvFrom(byte[] data) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        }
        return recvFrom(data, 0, data.length);
    }

    public int recvFrom(byte[] data, int off, int len) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        } else if (len == 0) {
            return 0;
        } else if (off < 0 || len < 0) {
            throw new IllegalArgumentException("offset or length are negative");
        }

        /* IndexOutOfBoundsException is handled by ByteBufferReference */

        int receivedLength = 0;

        CallbackP2PRecvFromCompleted mP2PRecvFromCallBack = new CallbackP2PRecvFromCompleted();

        ByteBufferReference pReceptionBuffer = new ByteBufferReference(data, off, len, false);

        MethodAutogen.WP2PRecvFrom(mSocketSafeHandle.getValue(), mP2PRecvFromCallBack.getId(),
                pReceptionBuffer.getValue(), mOperationRecvFrom.getReference());

        try {
            receivedLength = mP2PRecvFromCallBack.waitForResultOrException();
            pReceptionBuffer.release(true);
            if (!mOperationRecvFrom.isNull()) {
                MethodAutogen.WBasicCloseHandle(mOperationRecvFrom.getValue());
            }
        } catch (NfcException e) {
            if (e.getCode() == NfcErrorCode.BAD_STATE)
                throw new IllegalStateException(e.getMessage());
            else if (e.getCode() == NfcErrorCode.RETRY)
                throw new IllegalStateException(e.getMessage());
            throw new IOException(e.getMessage());
        }

        return receivedLength;
    }

    public void sendTo(byte sap, byte[] data) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        }
        sendTo(sap, data, 0, data.length);
    }

    public void sendTo(byte sap, byte[] data, int off, int len) throws IOException {

        if (data == null) {
            throw new IllegalArgumentException("data==null");
        } else if (len == 0) {
            return;
        } else if (off < 0 || len < 0) {
            throw new IllegalArgumentException("offset or length are negative");
        }

        /* IndexOutOfBoundsException is handled by ByteBufferReference */

        CallbackGenericCompletion mSendToCallBack = new CallbackGenericCompletion();

        ByteBufferReference pSendBuffer = new ByteBufferReference(data, off, len, true);

        MethodAutogen.WP2PSendTo(mSocketSafeHandle.getValue(), mSendToCallBack.getId(), sap,
                pSendBuffer.getValue(), mOperationSendTo.getReference());

        try {
            mSendToCallBack.waitForCompletionOrException();
        } catch (NfcException e) {
            if (e.getCode() == NfcErrorCode.BAD_STATE)
                throw new IllegalStateException(e.getMessage());
            else if (e.getCode() == NfcErrorCode.RETRY)
                throw new IllegalStateException(e.getMessage());
            throw new IOException(e.getMessage());
        } finally {
            pSendBuffer.release(false);
            if (!mOperationSendTo.isNull()) {
                MethodAutogen.WBasicCloseHandle(mOperationSendTo.getValue());
            }
        }

    }

    /** */
    private static void checkError(int error) throws NfcException {

        if (error != ConstantAutogen.W_SUCCESS) {
            switch (error) {
                case ConstantAutogen.W_ERROR_OUT_OF_RESOURCE:
                    throw new OutOfMemoryError("No enough resource available to "
                            + "complete the operation, the quota of sockets have "
                            + "been reached.");
                case ConstantAutogen.W_ERROR_BAD_PARAMETER:
                    throw new IllegalArgumentException("the combination of SAP and "
                            + "sericeUri is not a valid configuration");
                case ConstantAutogen.W_ERROR_EXCLUSIVE_REJECTED:
                    throw new IllegalStateException("the specified SAP or sericeUri "
                            + "value is already in use");
                default:
                    throw new NfcException("An NFC error occurred: ", NfcErrorCode.getCode(error));
            }
        }
    }

    private class CallbackP2PRecvFromCompleted extends Callback_tWP2PRecvFromCompleted {

        protected CallbackP2PRecvFromCompleted() {
            super(false);
        }

        @Override
        protected void tWP2PRecvFromCompleted(int dataLength, int error, int nsap) {
            if (error != ConstantAutogen.W_SUCCESS)
                signalException(new NfcException(NfcErrorCode.getCode(error)));
            else
                signalResult(dataLength);
        }

    }
}
