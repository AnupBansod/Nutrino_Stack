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

import org.opennfc.cardlistener.Iso14443Part3AConnection;
import org.opennfc.cardlistener.Iso14443Part3BConnection;

/**
 * package private
 *
 * @hide
 **/
class Iso14443Part3ConnectionImpl extends ConnectionImpl {

    private byte[] mReceivedFrame = new byte[280];

    /* package private */Iso14443Part3ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    public byte[] exchangeData(byte[] command) throws NfcException {

        if ((command == null) || (command.length == 0)){
            throw new IllegalArgumentException("command is null or empty");
        }

        CallbackW14ExchangeDataCompleted callback = new CallbackW14ExchangeDataCompleted();

        ByteBufferReference pReaderToCardBuffer = new ByteBufferReference(command, true);
        ByteBufferReference pCardToReaderBuffer = new ByteBufferReference(mReceivedFrame, false);

        MethodAutogen.W14Part3ExchangeData(getNonNullHandle(), callback.getId(),
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
}

/**
 * package private
 *
 * @hide
 **/
class CallbackW14ExchangeDataCompleted extends Callback_tWBasicGenericDataCallbackFunction {

  public CallbackW14ExchangeDataCompleted() {
      super(false);
  }

  @Override
  protected void tWBasicGenericDataCallbackFunction(int dataLength, int error) {

      if(error == ConstantAutogen.W_SUCCESS) {
          signalResult(dataLength);
      } else if (error == ConstantAutogen.W_ERROR_BAD_NFCC_MODE ) {
          signalException(new IllegalStateException("The requested operation is not valid with the current mode of the NFC Controller"));
      } else if (error == ConstantAutogen.W_ERROR_BAD_STATE) {
          signalException(new IllegalStateException("Bad state for this operation"));
      } else if ((error == ConstantAutogen.W_ERROR_BAD_HANDLE) || (error == ConstantAutogen.W_ERROR_CANCEL)) {
          signalException(new IllegalStateException("The connection is closed"));
      } else if (error == ConstantAutogen.W_ERROR_OUT_OF_RESOURCE) {
          signalException(new OutOfMemoryError("Error allocating resources"));
      } else if (error == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
          signalException(new IllegalArgumentException("Error in the argument value"));
      } else {
          signalException(new NfcException(NfcErrorCode.getCode(error)));
      }
  }
}

/**
 * package private
 *
 * @hide
 **/
class Iso14443Part3AConnectionImpl extends Iso14443Part3ConnectionImpl implements
        Iso14443Part3AConnection {

    private byte[] mUid = null;

    private short mAtqa = 0;

    private byte mSak = 0;

    /* package private */Iso14443Part3AConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    public byte[] getUid() {
        getTagInfo();
        return NfcManager.copyArray(this.mUid);
    }

    public short getAtqa() {
        getTagInfo();
        return this.mAtqa;
    }

    public byte getSak() {
        getTagInfo();
        return this.mSak;
    }

    private void getTagInfo() {

        if (this.mUid == null) {

            byte[] pInfoBuffer = new byte[14];
            NfcManager.checkError(MethodAutogen.W14Part3GetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));

            // short atqa
            this.mAtqa = NfcTagManagerImpl.byteArrayToShort(pInfoBuffer, 0);
            // byte sak
            this.mSak = pInfoBuffer[2];
            // int uidLength
            int uidLength = pInfoBuffer[3];
            // byte uid[4,7 or 10]
            this.mUid = new byte[uidLength];
            System.arraycopy(pInfoBuffer, 4, this.mUid, 0, uidLength);
        }
    }
}

/**
 * package private
 *
 * @hide
 **/
class Iso14443Part3BConnectionImpl extends Iso14443Part3ConnectionImpl implements
        Iso14443Part3BConnection {

    private byte[] mAtqb = null;

    private byte mAfi = 0;

    private boolean mIsCidSupported = false;

    private boolean mIsNadSupported = false;

    private int mCardInputBufferSize = 0;

    private int mReaderInputBufferSize = 0;

    private int mBaudRate = 0;

    private int mTimeout = 0;

    private byte[] mHigherLayerData = null;

    private byte[] mHigherLayerResponse = null;

    private byte mMbliCid = 0;

    /* package private */Iso14443Part3BConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /* See Iso14443Part3BConnection */
    public void setTimeout(int timeout) {
        NfcManager.checkError(MethodAutogen.W14Part3SetTimeout (getNonNullHandle(), timeout));
    }

    /* See Iso14443Part3BConnection */
    public byte[] getAtqb() {
        getTagInfo();
        return NfcManager.copyArray(this.mAtqb);
    }

    /* See Iso14443Part3BConnection */
    public byte getAfi() {
        getTagInfo();
        return this.mAfi;
    }

    /* See Iso14443Part3BConnection */
    public boolean isCidSupported() {
        getTagInfo();
        return this.mIsCidSupported;
    }

    /* See Iso14443Part3BConnection */
    public boolean isNadSupported() {
        getTagInfo();
        return this.mIsNadSupported;
    }

    /* See Iso14443Part3BConnection */
    public int getCardInputBufferSize() {
        getTagInfo();
        return this.mCardInputBufferSize;
    }

    /* See Iso14443Part3BConnection */
    public int getReaderInputBufferSize() {
        getTagInfo();
        return this.mReaderInputBufferSize;
    }

    /* See Iso14443Part3BConnection */
    public int getBaudRate() {
        getTagInfo();
        return this.mBaudRate;
    }

    /* See Iso14443Part3BConnection */
    public int getTimeout() {
        getTagInfo();
        return this.mTimeout;
    }

    /* See Iso14443Part3BConnection */
    public byte[] getHigherLayerData() {
        getTagInfo();
        return NfcManager.copyArray(this.mHigherLayerData);
    }

    /* See Iso14443Part3BConnection */
    public byte[] getHigherLayerResponse() {
        getTagInfo();
        return NfcManager.copyArray(this.mHigherLayerResponse);
    }

    /* See Iso14443Part3BConnection */
    public byte getMbliCid() {
        getTagInfo();
        return this.mMbliCid;
    }

    private void getTagInfo() {

        if (this.mAtqb == null) {
            byte[] pInfoBuffer = new byte[532];
            NfcManager.checkError(MethodAutogen.W14Part3GetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));
            int index = 0;

            // byte[] atqb
            this.mAtqb = new byte[12];
            System.arraycopy(pInfoBuffer, index, this.mAtqb, 0, 12);
            index += 12;
            // byte afi
            this.mAfi = pInfoBuffer[index++];
            // boolean isCidSupported
            this.mIsCidSupported = (pInfoBuffer[index++] == 1);
            // boolean isNadSupported
            this.mIsNadSupported = (pInfoBuffer[index++] == 1);
            // int cardInputBufferSize
            this.mCardInputBufferSize = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, index);
            index += 4;
            // int readerInputBufferSize
            this.mReaderInputBufferSize = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, index);
            index += 4;
            // int baudRate
            this.mBaudRate = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, index);
            index += 4;
            // int timeout
            this.mTimeout = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, index);
            index += 4;
            // byte mbliCid
            this.mMbliCid = pInfoBuffer[index++];
            // int higherLayerDataLength
            int higherLayerDataLength = pInfoBuffer[index++];
            // byte[] higherLayerData
            this.mHigherLayerData = new byte[higherLayerDataLength];
            if (higherLayerDataLength > 0)
                System.arraycopy(pInfoBuffer, index, this.mHigherLayerData, 0,
                        higherLayerDataLength);
            index += higherLayerDataLength;

            // int higherLayerResponseLength
            int higherLayerResponseLength = pInfoBuffer[index++];
            // byte[] higherLayerResponse
            this.mHigherLayerResponse = new byte[higherLayerDataLength];
            if (higherLayerResponseLength > 0)
                System.arraycopy(pInfoBuffer, index, this.mHigherLayerResponse, 0,
                        higherLayerResponseLength);
        }
    }
}
