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

import org.opennfc.p2p.P2PConnection;
import org.opennfc.p2p.P2PConnectionLess;
import org.opennfc.p2p.P2PLinkEventHandler;
import org.opennfc.p2p.P2PManager;

class P2PManagerImpl implements P2PManager {

    /** Operation link safe handle  */
    private static SafeHandle mOperationLink = new SafeHandle();

    /** hLink safe handle  */
    private static SafeHandle mLinkSafeHandle = null;

    /** the Local Link Max Information Unit */
    private short mLocalMiu = 0;

    /** the Local Link Timeout expressed in ms */
    private short mLocalLto = 0;

    /** the flag indicating if the Initiator mode is used for the P2P connection */
    private boolean mActivateInitiator = false;

    public void cancelLink() {
        if (!mOperationLink.isNull()) {
            MethodAutogen.WBasicCancelOperation(mOperationLink.getValue());
            mOperationLink.release();
        }
    }

    public P2PConnection createP2PConnection(Mode mode, String serviceUri, byte sap)
            throws NfcException {
        return new P2PConnectionImpl(mode, serviceUri, sap);
    }

    public P2PConnectionLess createP2PConnectionLess(String serviceUri, byte sap)
            throws NfcException {
        return new P2PConnectionLessImpl(serviceUri, sap);
    }

    public void establishLink(P2PLinkEventHandler listener) throws NfcException {

        if (listener == null) {
            throw new IllegalArgumentException("listener==null");
        }

        synchronized (this) {
            if (!mOperationLink.isNull()) {
                throw new IllegalStateException("Other link operation is ongoing");
            }

            GenericCallBackFunction pReleaseCallback = new GenericCallBackFunction(listener);
            GenericHandleCallBackFunction pEstablishmentCallback = new GenericHandleCallBackFunction(
                    listener);

            MethodAutogen.WP2PEstablishLink(pEstablishmentCallback.getId(), pReleaseCallback
                    .getId(), mOperationLink.getReference());
        }
    }

    public boolean getActivateInitiator() throws NfcException {
        getP2PParameters();
        return this.mActivateInitiator;
    }

    public short getLocalLto() throws NfcException {
        getP2PParameters();
        return this.mLocalLto;
    }

    public short getLocalMiu() throws NfcException {
        getP2PParameters();
        return this.mLocalMiu;
    }

    public void setActivateInitiator(boolean flag) throws NfcException {
        byte[] pConfigurationBuffer = getP2PParameters();
        pConfigurationBuffer[4] = (byte) ((flag) ? 1 : 0);
        NfcManager.checkError(MethodAutogen.WJavaP2PSetConfigurationBuffer(pConfigurationBuffer));
    }

    public void setLocalLto(short localLto) throws NfcException {
        byte[] pConfigurationBuffer = getP2PParameters();
        System.arraycopy(NfcTagManagerImpl.shortToByteArray(localLto), 0, pConfigurationBuffer, 0,
                2);
        NfcManager.checkError(MethodAutogen.WJavaP2PSetConfigurationBuffer(pConfigurationBuffer));
    }

    public void setLocalMiu(short localMiu) throws NfcException {

        byte[] pConfigurationBuffer = getP2PParameters();

        System.arraycopy(NfcTagManagerImpl.shortToByteArray(localMiu), 0, pConfigurationBuffer, 2,
                2);
        NfcManager.checkError(MethodAutogen.WJavaP2PSetConfigurationBuffer(pConfigurationBuffer));
    }

    private synchronized byte[] getP2PParameters() {

        byte[] pConfigurationBuffer = new byte[5];

        NfcManager.checkError(MethodAutogen.WJavaP2PGetConfigurationBuffer(pConfigurationBuffer));
        int index = 0;
        // short mLocalLto
        this.mLocalLto = NfcTagManagerImpl.byteArrayToShort(pConfigurationBuffer, index);
        index += 2;
        // short mLocalMiu
        this.mLocalMiu = NfcTagManagerImpl.byteArrayToShort(pConfigurationBuffer, index);
        index += 2;
        // boolean mActivateInitiator
        this.mActivateInitiator = (pConfigurationBuffer[index] == 1);

        return pConfigurationBuffer;
    }

    private class GenericHandleCallBackFunction extends
            Callback_tWBasicGenericHandleCallbackFunction {

        private P2PLinkEventHandler listener;

        GenericHandleCallBackFunction(P2PLinkEventHandler listener) {
            super(false);
            this.listener = listener;
        }

        protected void tWBasicGenericHandleCallbackFunction(int handle, int result) {
            mOperationLink.release();
            if (result == ConstantAutogen.W_SUCCESS) {
                mLinkSafeHandle = new SafeHandle(handle);
                this.listener.onLinkDetected(new P2PLinkImpl(mLinkSafeHandle));
            } else {
                listener.onLinkError();
            }
        }
    }

    private class GenericCallBackFunction extends Callback_tWBasicGenericCallbackFunction {

        private P2PLinkEventHandler listener;

        GenericCallBackFunction(P2PLinkEventHandler listener) {
            super(false);
            this.listener = listener;
        }

        @Override
        protected void tWBasicGenericCallbackFunction(int result) {
            /* If the Link has been broken */
            if (!mOperationLink.isNull()) {
                mOperationLink.release();
            }
            if (mLinkSafeHandle != null) {
                mLinkSafeHandle.release();
                mLinkSafeHandle = null;
            }

            this.listener.onLinkReleased();
        }
    }

}
