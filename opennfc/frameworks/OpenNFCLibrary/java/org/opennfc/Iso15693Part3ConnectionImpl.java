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

import org.opennfc.cardlistener.Iso15693Part3Connection;

/**
 * package private
 *
 * @hide
 **/
class Iso15693Part3ConnectionImpl extends ConnectionImpl implements Iso15693Part3Connection {

    private byte[] mUid = null;

    private boolean mIsAfiSupported = false;

    private boolean mIsAfiLocked = false;

    private boolean mIsDsfidSupported = false;

    private boolean mIsDsfidLocked = false;

    private byte mAfi = 0;

    private byte mDsfid = 0;

    private int mSectorSize = 0;

    private int mSectorNumber = 0;

    /* package private */Iso15693Part3ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /** See Iso15693Part3Connection */
    public boolean isSectorWritable(byte sectorIndex) {
        int error = MethodAutogen.W15IsWritable(getNonNullHandle(), sectorIndex);

        if(error == ConstantAutogen.W_ERROR_ITEM_LOCKED)
            return false;

        NfcManager.checkError(error);

        return true;
    }

    /** See Iso15693Part3Connection */
    public byte[] read(int offset, int length) throws NfcException {

        if((offset < 0) || (length <= 0)) {
            throw new IllegalArgumentException("offset or length are not valid");
        }

        byte[] dataRead = new byte[length];

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(dataRead, false);

        MethodAutogen.W15Read(getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset, null);

        callback.waitForCompletionOrException();

        pBuffer.release(true);

        return dataRead;
    }

    /** See Iso15693Part3Connection */
    public void write(byte[] buffer, int offset, boolean lock) throws NfcException {

        if((buffer == null) || (buffer.length == 0)) {
            throw new IllegalArgumentException("buffer is null or empty");
        }
        // the maximum buffer length and the offset value are checked in the lower layer.

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(buffer, true);

        MethodAutogen.W15Write(getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset,
                lock, null);

        callback.waitForCompletionOrException();

        pBuffer.release(false);
    }

    /** See Iso15693Part3Connection */
    public void setAttribute(int actions, byte afi, byte dsfid) throws NfcException {

        CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.W15SetAttribute(getNonNullHandle(), callback.getId(), actions, afi, dsfid, null);
        callback.waitForCompletionOrException();

        // to force the update of the tag info
        this.mUid = null;
    }

    /** See Iso15693Part3Connection */
    public void setTagSize(int sectorNumber, int sectorSize) {

        NfcManager.checkError(MethodAutogen.W15SetTagSize(getNonNullHandle(), sectorNumber, sectorSize));

        // update the internal values
        this.mSectorSize = sectorSize;
        this.mSectorNumber = sectorNumber;
    }

    /** See Iso15693Part3Connection */
    public byte[] getUid() {
        getTagInfo();
        return NfcManager.copyArray(this.mUid);
    }

    /** See Iso15693Part3Connection */
    public boolean isAfiSupported() {
        getTagInfo();
        return this.mIsAfiSupported;
    }

    /** See Iso15693Part3Connection */
    public boolean isAfiLocked() {
        getTagInfo();
        return this.mIsAfiLocked;
    }

    /** See Iso15693Part3Connection */
    public boolean isDsfidSupported() {
        getTagInfo();
        return this.mIsDsfidSupported;
    }

    /** See Iso15693Part3Connection */
    public boolean isDsfidLocked() {
        getTagInfo();
        return this.mIsDsfidLocked;
    }

    /** See Iso15693Part3Connection */
    public byte getAfi() {
        getTagInfo();
        return this.mAfi;
    }

    /** See Iso15693Part3Connection */
    public byte getDsfid() {
        getTagInfo();
        return this.mDsfid;
    }

    /** See Iso15693Part3Connection */
    public int getSectorSize() {
        getTagInfo();
        return this.mSectorSize;
    }

    /** See Iso15693Part3Connection */
    public int getSectorNumber() {
        getTagInfo();
        return this.mSectorNumber;
    }

    private void getTagInfo() {

        if (this.mUid == null) {
            byte[] pInfoBuffer = new byte[22];
            NfcManager.checkError(MethodAutogen.W15GetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));

            // byte[] uid
            this.mUid = new byte[8];
            System.arraycopy(pInfoBuffer, 0, this.mUid, 0, 8);
            // boolean isAfiSupported
            this.mIsAfiSupported = (pInfoBuffer[8] == 1);
            // boolean isAfiLocked
            this.mIsAfiLocked = (pInfoBuffer[9] == 1);
            // boolean isDsfidSupported
            this.mIsDsfidSupported = (pInfoBuffer[10] == 1);
            // boolean isDsfidLocked
            this.mIsDsfidLocked = (pInfoBuffer[11] == 1);
            // byte afi
            this.mAfi = pInfoBuffer[12];
            // byte dsfid
            this.mDsfid = pInfoBuffer[13];
            // int sectorSize
            int sectorSize = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 14);
            if(sectorSize != 0) {
                this.mSectorSize = sectorSize;
            }
            // int sectorNumber
            int sectorNumber = NfcTagManagerImpl.byteArrayToInt(pInfoBuffer, 18);
            if(sectorNumber != 0) {
                this.mSectorNumber = sectorNumber;
            }
        }
    }
}
