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

import org.opennfc.cardlistener.MifareULConnection;

/**
 * package private
 *
 * @hide
 **/
class MifareULConnectionImpl extends Iso14443Part3ConnectionImpl implements MifareULConnection {

    /* flag for the get access right method */
    private boolean mGetAccessRightsCalled;

    /* the card UID */
    private byte[] mUid;

    /* The sector size in bytes */
    private int mSectorSize;

    /* The sector number */
    private int mSectorNumber;

    /**
     * package private
     *
     * @hide
     **/
    MifareULConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);

        mGetAccessRightsCalled = false;
    }

    /** See MifareULConnection */
    public void authenticate(byte[] key) throws NfcException {

        CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.WMifareULCAuthenticate(getNonNullHandle(), callback.getId(), key);
        callback.waitForCompletionOrException();
    }

    /** See MifareULConnection */
    public void forceULC() {

        NfcManager.checkError(MethodAutogen.WMifareULForceULC(getNonNullHandle()));
    }

    /** See MifareULConnection */
    public void freezeDataLockConfiguration() throws NfcException {

        CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.WMifareULFreezeDataLockConfiguration(getNonNullHandle(), callback.getId());
        callback.waitForCompletionOrException();
    }

    /** See MifareULConnection */
    public int getAccessRights(int offset, int length) throws NfcException {

        if(mGetAccessRightsCalled == false) {

            CallbackGenericCompletion callback = new CallbackGenericCompletion();
            MethodAutogen.WMifareULRetrieveAccessRights(getNonNullHandle(), callback.getId());
            callback.waitForCompletionOrException();

            mGetAccessRightsCalled = true;
        }

        int[] rights = new int[1];

        NfcManager.checkError(MethodAutogen.WMifareULGetAccessRights(
            getNonNullHandle(), offset, length, rights));

        return rights[0];
    }

    /** See MifareULConnection */
    public void setAccessRights(byte[] key, int threshold, int rights, boolean lock)
            throws NfcException {

        CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.WMifareULCSetAccessRights(getNonNullHandle(), callback.getId(),
            key, threshold, rights, lock);
        callback.waitForCompletionOrException();
    }

    private void checkGetInfo() {
        if(this.mUid == null) {
            byte[] buffer = new byte[15];

            NfcManager.checkError(MethodAutogen.WMifareGetConnectionInfoBuffer(getNonNullHandle(), buffer));

            // byte[] uid
            this.mUid = new byte[7];
            System.arraycopy(buffer, 0, this.mUid, 0, 7);
            // int sectorSize
            this.mSectorSize = NfcTagManagerImpl.byteArrayToInt(buffer, 7);
            // int sectorNumber
            this.mSectorNumber = NfcTagManagerImpl.byteArrayToInt(buffer, 11);
        }
    }

    /** See MifareULConnection */
    public byte[] getUid() {

        checkGetInfo();
        return mUid;
    }

    /** See MifareULConnection */
    public int getSectorSize() {

        checkGetInfo();
        return mSectorSize;
    }

    /** See MifareULConnection */
    public int getSectorNumber() {

        checkGetInfo();
        return mSectorNumber;
    }

    /** See MifareULConnection */
    public byte[] read(int offset, int length) throws NfcException {

        if((offset < 0) || (length <= 0)) {
            throw new IllegalArgumentException("offset or length are not valid");
        }

        byte[] dataRead = new byte[length];

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(dataRead, false);

        MethodAutogen.WMifareRead(getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset, null);

        callback.waitForCompletionOrException();

        pBuffer.release(true);

        return dataRead;
    }

    /** See MifareULConnection */
    public void write(byte[] buffer, int offset, boolean lock) throws NfcException {

        if((buffer == null) || (buffer.length == 0)) {
            throw new IllegalArgumentException("buffer is null or empty");
        }
        // the maximum buffer length and the offset value are checked in the lower layer.

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(buffer, true);

        MethodAutogen.WMifareWrite(getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset, lock, null);

        callback.waitForCompletionOrException();

        pBuffer.release(false);

    }

}
