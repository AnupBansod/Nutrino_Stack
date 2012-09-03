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

import org.opennfc.cardlistener.FeliCaConnection;

/**
 * package private
 *
 * @hide
 **/
class FeliCaConnectionImpl extends ConnectionImpl implements FeliCaConnection {

    private byte[] mManufacturerId = null;

    private short mSystemCode = 0;

    private byte[] mManufacturerParameter = null;

    FeliCaConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /* See FeliCaConnection */
    public byte[] read(int length, int[] serviceCodeList, byte[] blockList) throws NfcException {

        if(length <= 0) {
            throw new IllegalArgumentException("length is not valid");
        }

        if((serviceCodeList == null) || (serviceCodeList.length == 0)) {
            throw new IllegalArgumentException("serviceCodeList is null or empty");
        }
        // The maximum size and the content of serviceCodeList is checked by the lower layer

        if((blockList == null) || (blockList.length == 0)) {
            throw new IllegalArgumentException("blockList is null or empty");
        }
        // The maximum size and the content of blockList is checked by the lower layer

        byte[] dataRead = new byte[length];

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(dataRead, false);

        MethodAutogen.WFeliCaReadSimple(getNonNullHandle(), callback.getId(), pBuffer.getValue(), serviceCodeList, blockList);

        callback.waitForResultOrException();

        pBuffer.release(true);

        return dataRead;
    }

    /* See FeliCaConnection */
    public void write(byte[] buffer, int[] serviceCodeList, byte[] blockList) throws NfcException {

        if((buffer == null) || (buffer.length == 0)) {
            throw new IllegalArgumentException("buffer is null or empty");
        }
        // the maximum buffer length and the offset value are checked in the lower layer.

        if((serviceCodeList == null) || (serviceCodeList.length == 0)) {
            throw new IllegalArgumentException("serviceCodeList is null or empty");
        }
        // The maximum size and the content of serviceCodeList is checked by the lower layer

        if((blockList == null) || (blockList.length == 0)) {
            throw new IllegalArgumentException("blockList is null or empty");
        }
        // The maximum size and the content of blockList is checked by the lower layer

        CallbackGenericCompletion callback = new CallbackGenericCompletion();

        ByteBufferReference pBuffer = new ByteBufferReference(buffer, true);

        MethodAutogen.WFeliCaWriteSimple(getNonNullHandle(), callback.getId(), pBuffer.getValue(), serviceCodeList, blockList);

        callback.waitForResultOrException();

        pBuffer.release(false);
    }

    /* See FeliCaConnection */
    public byte[] getManufacturerId() {
        getTagInfo();
        return NfcManager.copyArray(this.mManufacturerId);
    }

    /* See FeliCaConnection */
    public byte[] getManufacturerParameter() {
        getTagInfo();
        return NfcManager.copyArray(this.mManufacturerParameter);
    }

    /* See FeliCaConnection */
    public short getSystemCode() {
        getTagInfo();
        return this.mSystemCode;
    }

    private void getTagInfo() {

        if (this.mManufacturerId == null) {
            byte[] pInfoBuffer = new byte[18];

            NfcManager.checkError(MethodAutogen.WFeliCaGetConnectionInfoBuffer(getNonNullHandle(), pInfoBuffer));

            // byte[] mManufacturerId
            this.mManufacturerId = new byte[8];
            System.arraycopy(pInfoBuffer, 0, this.mManufacturerId, 0, 8);
            // short mSystemCode
            this.mSystemCode = NfcTagManagerImpl.byteArrayToShort(pInfoBuffer, 8);
            // byte[] mManufacturerParameter
            this.mManufacturerParameter = new byte[8];
            System.arraycopy(pInfoBuffer, 10, this.mManufacturerParameter, 0, 8);
        }
    }
}
