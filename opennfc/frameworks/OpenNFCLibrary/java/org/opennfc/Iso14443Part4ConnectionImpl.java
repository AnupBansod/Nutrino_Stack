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

import org.opennfc.cardlistener.Iso14443Part4AConnection;
import org.opennfc.cardlistener.Iso14443Part4BConnection;
import org.opennfc.cardlistener.Iso7816Part4Connection;

/**
 * package private
 *
 * @hide
 **/
class Iso14443Part4ConnectionImpl extends ConnectionImpl {

    private byte[] mReceivedFrame = new byte[280];

    /* package private */Iso14443Part4ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    public void setNad(byte nad) {

        NfcManager.checkError(MethodAutogen.W14Part4SetNAD(getNonNullHandle(), nad));
    }

    public byte[] exchangeData(byte[] command) throws NfcException {

        CallbackW14ExchangeDataCompleted callback = new CallbackW14ExchangeDataCompleted();

        ByteBufferReference pReaderToCardBuffer = new ByteBufferReference(command, true);
        ByteBufferReference pCardToReaderBuffer = new ByteBufferReference(mReceivedFrame, false);

        MethodAutogen.W14Part4ExchangeData(getNonNullHandle(), callback.getId(),
                pReaderToCardBuffer.getValue(), pCardToReaderBuffer.getValue(), null);

        int dataLength = callback.waitForResultOrException();

        pReaderToCardBuffer.release(false);
        pCardToReaderBuffer.release(true);

        byte[] response = new byte[dataLength];
        if(dataLength != 0) {
            System.arraycopy(mReceivedFrame, 0, response, 0, dataLength);
        }

        return response;
    }

    /**********************************************************
     * This part is the implementation of ISO 7816 interface It will become
     * visible in ISO 7816 sub-classes implementing the actual interface.
     **********************************************************/

    private byte[] mResponseApduBuffer = new byte[280];

    public byte[] exchangeApdu(byte[] commandApdu) throws NfcException {

        CallbackGenericDataCompletion callback = new CallbackGenericDataCompletion();

        ByteBufferReference sendAPDUBufferRef = new ByteBufferReference(commandApdu, true);
        ByteBufferReference receivedAPDUBufferRef = new ByteBufferReference(mResponseApduBuffer,
                false);

        MethodAutogen.W7816ExchangeAPDU(getNonNullHandle(), callback.getId(), sendAPDUBufferRef
                .getValue(), receivedAPDUBufferRef.getValue(), null);

        int responseApduLength = callback.waitForResultOrException();

        sendAPDUBufferRef.release(false);
        receivedAPDUBufferRef.release(true);

        byte[] response = new byte[responseApduLength];
        System.arraycopy(mResponseApduBuffer, 0, response, 0, responseApduLength);

        return response;
    }

    public Iso7816Part4Connection openLogicalChannel(byte[] aid) throws NfcException {

      CallbackGenericDataCompletion callback = new CallbackGenericDataCompletion();

      ByteBufferReference aidBufferRef = new ByteBufferReference(aid, true);

      MethodAutogen.W7816OpenLogicalChannel(getNonNullHandle(), callback.getId(),
         aidBufferRef.getValue(), null);

      int logicalChannelHandle = callback.waitForResultOrException();

      aidBufferRef.release(false);

      return new LogicalChannelConnectionImpl(new SafeHandle(logicalChannelHandle, SafeHandle.LOGICAL_CHANNEL));
    }
}

/**
 * package private
 *
 * @hide
 **/
class LogicalChannelConnectionImpl extends Iso14443Part4ConnectionImpl implements
        Iso7816Part4Connection {

    /* package private */LogicalChannelConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}

/**
 * package private
 *
 * @hide
 **/
class Iso14443Part4AConnectionImpl extends Iso14443Part4ConnectionImpl implements
        Iso14443Part4AConnection {

    private boolean mIsCidSupported = false;

    private byte mCid = 0;

    private boolean mIsNadSupported = false;

    private byte mNad = 0;

    private byte[] mApplicationData = null;

    private int mCardInputBufferSize = 0;

    private int mReaderInputBufferSize = 0;

    private byte mFwiSfgi = 0;

    private int mTimeout = 0;

    private byte mDataRateMaxDiv = 0;

    private int mBaudRate = 0;

    /* package private */Iso14443Part4AConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /* See Iso14443Part4AConnection */
    public boolean isCidSupported() {
        getTagInfo();
        return this.mIsCidSupported;
    }

    /* See Iso14443Part4AConnection */
    public byte getCid() {
        getTagInfo();
        return this.mCid;
    }

    /* See Iso14443Part4AConnection */
    public boolean isNadSupported() {
        getTagInfo();
        return this.mIsNadSupported;
    }

    /* See Iso14443Part4AConnection */
    public byte getNad() {
        getTagInfo();
        return this.mNad;
    }

    /* See Iso14443Part4AConnection */
    public byte[] getApplicationData() {
        getTagInfo();
        return NfcManager.copyArray(this.mApplicationData);
    }

    /* See Iso14443Part4AConnection */
    public int getCardInputBufferSize() {
        getTagInfo();
        return this.mCardInputBufferSize;
    }

    /* See Iso14443Part4AConnection */
    public int getReaderInputBufferSize() {
        getTagInfo();
        return this.mReaderInputBufferSize;
    }

    /* See Iso14443Part4AConnection */
    public byte getFwiSfgi() {
        getTagInfo();
        return this.mFwiSfgi;
    }

    /* See Iso14443Part4AConnection */
    public int getTimeout() {
        getTagInfo();
        return this.mTimeout;
    }

    /* See Iso14443Part4AConnection */
    public byte getDataRateMaxDiv() {
        getTagInfo();
        return this.mDataRateMaxDiv;
    }

    /* See Iso14443Part4AConnection */
    public int getBaudRate() {
        getTagInfo();
        return this.mBaudRate;
    }

    private void getTagInfo() {

        if (this.mApplicationData == null) {
            byte[] pInfoBuffer = new byte[38];
            NfcManager.checkError(MethodAutogen.W14Part4GetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));

             // boolean isCidSupported
             this.mIsCidSupported = (pInfoBuffer[0] == 1);
             // byte cid
             this.mCid = pInfoBuffer[1];
             // boolean isNadSupported
             this.mIsNadSupported = (pInfoBuffer[2] == 1);
             // byte nad
             this.mNad = pInfoBuffer[3];
             // int cardInputBufferSize
             this.mCardInputBufferSize = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 4);
             // int readerInputBufferSize
             this.mReaderInputBufferSize = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 8);
             // byte fwiSfgi
             this.mFwiSfgi = pInfoBuffer[12];
             // int timeout
             this.mTimeout = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 13);
             // byte dataRateMaxDiv
             this.mDataRateMaxDiv = pInfoBuffer[17];
             // int baudRate
             this.mBaudRate = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 18);
             // int applicationDatalength
             int applicationDatalength = pInfoBuffer[22];
             // byte applicationData[15]
             this.mApplicationData = new byte[applicationDatalength];
             System.arraycopy(pInfoBuffer, 23, this.mApplicationData, 0, applicationDatalength);
        }
    }
}

/**
 * package private
 *
 * @hide
 **/
class Iso14443Part4BConnectionImpl extends Iso14443Part4ConnectionImpl implements
        Iso14443Part4BConnection {

    /* package private */Iso14443Part4BConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /* See Iso14443Part4BConnection */
    public byte getNad() {

        byte[] pInfoBuffer = new byte[1];
        NfcManager.checkError(MethodAutogen.W14Part4GetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));

        return pInfoBuffer[0];
    }

}
