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

import org.opennfc.cardlistener.PicopassConnection;

/**
 * package private
 *
 * @hide
 **/
class PicopassConnectionImpl extends Iso14443Part3ConnectionImpl implements PicopassConnection {

    private byte[] mUid = null;

    private int mSectorSize = 0;

    private int mSectorNumber = 0;

    PicopassConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /* See PicopassConnection */
    public boolean isCardWritable() {
        int error = MethodAutogen.WPicoIsWritable(getNonNullHandle());

        if(error == ConstantAutogen.W_ERROR_ITEM_LOCKED)
            return false;

        NfcManager.checkError(error);

        return true;
    }

    /* See PicopassConnection */
    public byte[] read(int offset, int length) throws NfcException {

        if((offset < 0) || (length <= 0)) {
            throw new IllegalArgumentException("offset or length are not valid");
        }

        byte[] dataRead = new byte[length];

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(dataRead, false);

        MethodAutogen.WPicoRead(getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset, null);

        callback.waitForCompletionOrException();

        pBuffer.release(true);

        return dataRead;
    }

    /* See PicopassConnection */
    public void write(byte[] buffer, int offset, boolean lock) throws NfcException {

        if((buffer == null) || (buffer.length == 0)) {
            throw new IllegalArgumentException("buffer is null or empty");
        }
        // the maximum buffer length and the offset value are checked in the lower layer.

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(buffer, true);

        MethodAutogen.WPicoWrite(getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset,
                lock, null);

        callback.waitForCompletionOrException();

        pBuffer.release(false);
    }

    /* See PicopassConnection */
    public byte[] getUid() {
        getTagInfo();
        return NfcManager.copyArray(this.mUid);
    }

    /* See PicopassConnection */
    public int getSectorSize() {
        getTagInfo();
        return this.mSectorSize;
    }

    /* See PicopassConnection */
    public int getSectorNumber() {
        getTagInfo();
        return this.mSectorNumber;
    }

    private void getTagInfo() {

        if (this.mUid == null) {
            byte[] pInfoBuffer = new byte[16];
            NfcManager.checkError(MethodAutogen.WPicoGetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));

            // byte[] uid
            this.mUid = new byte[8];
            System.arraycopy(pInfoBuffer, 0, this.mUid, 0, 8);
            // int sectorSize
            this.mSectorSize = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 8);
            // int sectorNumber
            this.mSectorNumber = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 12);
        }
    }
}
