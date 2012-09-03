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

package org.opennfc.cardlistener;

import org.opennfc.NfcException;

/**
 * The ISO 15693 API provides an access to the ISO 15693 reader methods
 * defined in the specification ISO 15693 part 3 "Anti-collision and
 * transmission protocol".
 *
 * @since Open NFC 4.0
 */
public interface Iso15693Part3Connection extends Connection {

    /** Action flag: Set the AFI byte */
    static final int ACTION_SET_AFI = ConstantAutogen.W_15_ATTRIBUTE_ACTION_SET_AFI;

    /** Action flag: Lock the AFI byte */
    static final int ACTION_LOCK_AFI = ConstantAutogen.W_15_ATTRIBUTE_ACTION_LOCK_AFI;

    /** Action flag: Set the DSFID byte */
    static final int ACTION_SET_DSFID = ConstantAutogen.W_15_ATTRIBUTE_ACTION_SET_DSFID;

    /** Action flag: Lock the DSFID byte */
    static final int ACTION_LOCK_DSFID = ConstantAutogen.W_15_ATTRIBUTE_ACTION_LOCK_DSFID;

    /** Action flag: Set the global lock of the tag */
    static final int ACTION_LOCK_TAG = ConstantAutogen.W_15_ATTRIBUTE_ACTION_LOCK_TAG;

    /**
     * Forces the sector number and the sector size for the connection.
     *
     * Many tags do no return this information, so the sector number and
     * the sector size returned by {@link #getSectorSize} and {@link #getSectorNumber}
     * are set to zero. In this case, the read or write function will return the exception NfcException.
     *
     * To avoid this, the calling application should set the sector size and number
     * with setTagSize(). This information should be deduced from the knowledge
     * of the tag model deduced from the tag UID.
     *
     * If the tag returns the size information with GET INFO as defined in ISO 15693-3,
     * setTagSize() throws the exception IllegalArgumentException if the sector size or
     * the sector number are not identical the values returned by the tag.
     *
     * @param sectorNumber the number of sector in the card.
     * @param sectorSize the sector size in bytes.
     *
     * @throws IllegalArgumentException  the value of the sector size or number are not valid.
     * @throws IllegalStateException the connection is closed.
     */
    void setTagSize(int sectorNumber, int sectorSize);

    /**
     * Checks if a sector is writable.
     *
     * @param sectorIndex the sector to be checked.
     *
     * @return true if the sector is writable.
     *
     * @throws IllegalArgumentException if <code>sectorIndex</code> is not valid.
     * @throws IllegalStateException the connection is closed.
     */
    boolean isSectorWritable(byte sectorIndex);

    /**
     * Sets the attributes of the tag
     *
     * @param actions the actions to be performed, a bit field value of the ACTION_XXX values.
     * @param afi the AFI value to set if {@link #ACTION_SET_AFI} is requested.
     * @param dsfid the DSFID value to set if {@link #ACTION_SET_DSFID} is requested.
     *
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    void setAttribute(int actions, byte afi, byte dsfid) throws NfcException;

    /**
     * Reads a contiguous area of the tag
     *
     * @param offset the start address in bytes of the area to be read.
     * @param length the length in bytes of the area to be read.
     *
     * @return the bytes read.
     *
     * @throws IllegalArgumentException if <code>offset</code> or <code>length</code> are not valid.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    byte[] read(int offset, int length) throws NfcException;

    /**
     * Writes data into a contiguous area of the tag
     *
     * @param buffer the data to be written in the tag.
     * @param offset the start address of the area of the tag to be written.
     * @param lock lock the data area after writing the data.
     *
     * @throws IllegalArgumentException if <code>buffer</code> is null or empty.
     * @throws IllegalArgumentException if <code>offset</code> is not valid.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    void write(byte[] buffer, int offset, boolean lock) throws NfcException;

    /**
     * Returns The UID value.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte[] getUid();

    /**
     * Returns the flag indicating if the AFI is supported. true if the AFI is supported,
     * false if the AFI is not supported.
     *
     * @throws IllegalStateException the connection is closed.
     */
    boolean isAfiSupported();

    /**
     * Returns the flag indicating if the AFI is locked. true if the AFI is locked, false if
     * the AFI is not locked. The value is true if the AFI is not supported.
     *
     * @throws IllegalStateException the connection is closed.
     */
    boolean isAfiLocked();

    /**
     * Returns the flag indicating if the DSFID is supported. true if the DSFID is
     * supported, false if the DSFID is not supported.
     *
     * @throws IllegalStateException the connection is closed.
     */
    boolean isDsfidSupported();

    /**
     * Returns the flag indicating if the DSFID is locked. true if the DSFID is locked,
     * false if the DSFID is not locked. The value is true if the AFI is not
     * supported.
     *
     * @throws IllegalStateException the connection is closed.
     */
    boolean isDsfidLocked();

    /**
     * Returns the AFI value. The value is zero if the AFI is not supported.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getAfi();

    /**
     * Returns the DSFID value. The value is zero if the DSFID is not supported.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getDsfid();

    /**
     * Returns the sector size in bytes. Set to zero if the sector size in unknown.
     *
     * @throws IllegalStateException the connection is closed.
     */
    int getSectorSize();

    /**
     * Returns the sector number. Set to zero if the sector number is unknown.
     *
     * @throws IllegalStateException the connection is closed.
     */
    int getSectorNumber();

}
