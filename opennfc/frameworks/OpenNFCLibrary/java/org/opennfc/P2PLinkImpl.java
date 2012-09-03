
package org.opennfc;

import java.io.IOException;

import org.opennfc.p2p.P2PLink;

class P2PLinkImpl implements P2PLink {

    /** hLink safe handle  */
    private SafeHandle mLinkSafeHandle = null;

    /** */
    private byte mAgreedLlcpVersion = 0;

    /**  */
    private int mBaudeRate = 0;

    /**  */
    private byte mRemoteLlcpVersion = 0;

    /**  */
    private int mRemoteLto = 0;

    /**  */
    private int mRemoteMiu = 0;

    /**  */
    private int mRemoteWks = 0;

    /**  */
    private boolean mIsInitiator = true;

    /* package private */P2PLinkImpl(SafeHandle mLinkSafeHandle) {
        this.mLinkSafeHandle = mLinkSafeHandle;
        getLinkProperties();
    }

    /* package private */int getSafeHandle(){
        return mLinkSafeHandle.getValue();
    }

    public byte UriLookup(String serviceUri) throws IOException, NfcException {

        if (serviceUri == null) {
            throw new IllegalArgumentException("serviceUri==null");
        }

        byte sap = 0;
        synchronized (this) {

            if (mLinkSafeHandle.isNull()) {
                throw new IOException("The link is broken");
            }

            CallbackP2PURILookupCompleted UriLookupCallback = new CallbackP2PURILookupCompleted();
            MethodAutogen.WP2PURILookup(this.mLinkSafeHandle.getValue(),
                    UriLookupCallback.getId(), serviceUri);
            sap = (byte) UriLookupCallback.waitForResultOrException();
        }

        return sap;
    }

    public void close() throws IOException {
        if (mLinkSafeHandle != null) {
            mLinkSafeHandle.release();
            mLinkSafeHandle = null;
        }
    }

    public byte getAgreeLlcpVersion() throws IOException {

        if (mLinkSafeHandle.isNull()) {
            throw new IOException("The link is broken");
        }
        return this.mAgreedLlcpVersion;
    }

    public int getBaudeRate() throws IOException {

        if (mLinkSafeHandle.isNull()) {
            throw new IOException("The link is broken");
        }
        return this.mBaudeRate;
    }

    public byte getRemoteLlcpVersion() throws IOException {

        if (mLinkSafeHandle.isNull()) {
            throw new IOException("The link is broken");
        }
        return this.mRemoteLlcpVersion;
    }

    public int getRemoteLto() throws IOException {

        if (mLinkSafeHandle.isNull()) {
            throw new IOException("The link is broken");
        }
        return this.mRemoteLto;
    }

    public int getRemoteMiu() throws IOException {

        if (mLinkSafeHandle.isNull()) {
            throw new IOException("The link is broken");
        }
        return this.mRemoteMiu;
    }

    public int getRemoteWks() throws IOException {

        if (mLinkSafeHandle.isNull()) {
            throw new IOException("The link is broken");
        }
        return this.mRemoteWks;
    }

    public boolean isInitiator() throws IOException {

        if (mLinkSafeHandle.isNull()) {
            throw new IOException("The link is broken");
        }
        return this.mIsInitiator;
    }

    private void getLinkProperties() {

        byte[] mLinkProperties = new byte[19];
        NfcManager.checkError(MethodAutogen.WJavaP2PGetLinkPropertiesBuffer(this.mLinkSafeHandle
                .getValue(), mLinkProperties));

        int index = 0;

        this.mAgreedLlcpVersion = mLinkProperties[index++];
        this.mRemoteLlcpVersion = mLinkProperties[index++];
        this.mRemoteWks = NfcTagManagerImpl.byteArrayToShort(mLinkProperties, index);
        index += 2;
        this.mRemoteMiu = NfcTagManagerImpl.byteArrayToInt(mLinkProperties, index);
        index += 4;
        this.mRemoteLto = NfcTagManagerImpl.byteArrayToInt(mLinkProperties, index);
        index += 4;
        this.mBaudeRate = NfcTagManagerImpl.byteArrayToInt(mLinkProperties, index);
        index += 4;
        this.mIsInitiator = (mLinkProperties[index] == 1);
    }

    private class CallbackP2PURILookupCompleted extends Callback_tWP2PURILookupCompleted {

        public CallbackP2PURILookupCompleted() {
            super(false);
        }

        @Override
        protected void tWP2PURILookupCompleted(int ndsap, int error) {
            if(error == ConstantAutogen.W_SUCCESS) {
                signalResult(ndsap);
            } else {
                signalException(new NfcException(NfcErrorCode.getCode(error)));
            }
        }
    }
}
