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

import org.opennfc.cardlistener.TopazConnection;

/**
 * package private
 *
 * @hide
 **/
class TopazConnectionImpl extends Iso14443Part3ConnectionImpl implements TopazConnection {

    private byte[] mUid = null;

    private int mBblockSize = 0;

    private int mBlockNumber = 0;

    private byte[] mHeaderRom = null;

    /* package private */TopazConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /* See TopazConnection */
    public boolean isChipWritable() {
        int error = MethodAutogen.WType1ChipIsWritable(getNonNullHandle());

        if(error == ConstantAutogen.W_ERROR_ITEM_LOCKED)
            return false;

        NfcManager.checkError(error);

        return true;
    }

    /* See TopazConnection */
    public byte[] read(int offset, int length) throws NfcException {

        if((offset < 0) || (length <= 0)) {
            throw new IllegalArgumentException("offset or length are not valid");
        }

        byte[] dataRead = new byte[length];

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(dataRead, false);

        MethodAutogen.WType1ChipRead(getNonNullHandle(), callback.getId(),
                pBuffer.getValue(), offset, null);

        callback.waitForCompletionOrException();

        pBuffer.release(true);

        return dataRead;
    }

    /* See TopazConnection */
    public void write(byte[] buffer, int offset, boolean lock) throws NfcException {

        if((buffer == null) || (buffer.length == 0)) {
            throw new IllegalArgumentException("buffer is null or empty");
        }
        // the maximum buffer length and the offset value are checked in the lower layer.

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(buffer, true);

        MethodAutogen.WType1ChipWrite(getNonNullHandle(), callback.getId(), pBuffer.getValue(),
                offset, lock, null);

        callback.waitForCompletionOrException();

        pBuffer.release(false);
    }

    /* See TopazConnection */
    public byte[] getUid() {
        getTagInfo();
        return NfcManager.copyArray(this.mUid);
    }

    /* See TopazConnection */
    public int getBlockSize() {
        getTagInfo();
        return this.mBblockSize;
    }

    /* See TopazConnection */
    public int getBlockNumber() {
        getTagInfo();
        return this.mBlockNumber;
    }

    /* See TopazConnection */
    public byte[] getHeaderRom() {
        getTagInfo();
        return NfcManager.copyArray(this.mHeaderRom);
    }

    private void getTagInfo() {

        if (this.mUid == null) {
            byte[] pInfoBuffer = new byte[17];
            NfcManager.checkError(MethodAutogen.WType1ChipGetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));

            // byte[] uid
            this.mUid = new byte[7];
            System.arraycopy(pInfoBuffer, 0, this.mUid, 0, 7);
            // int blockSize
            this.mBblockSize = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 7);
            // int blockNumber
            this.mBlockNumber = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 11);
            // byte[] headerRom
            this.mHeaderRom = new byte[2];
            System.arraycopy(pInfoBuffer, 15, this.mHeaderRom, 0, 2);
        }
    }
}
