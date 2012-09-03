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
 * The MifareULConnection gives an access to the:
 * <ul>
 * <li>Mifare UL
 * <li>Mifare UL C
 * </ul>
 *
 * @since Open NFC 4.0
 */
public interface MifareULConnection extends Connection {

    /** Indicates unconditionnal read access */
    static final byte AR_READ_OK = ConstantAutogen.W_MIFARE_UL_READ_OK;

    /** Indicates authenticated-only read access (only for Mifare UL-C) */
    static final byte AR_READ_AUTHENTICATED = ConstantAutogen.W_MIFARE_UL_READ_AUTHENTICATED;

    /** Indicates unconditionnal write access */
    static final byte AR_WRITE_OK = ConstantAutogen.W_MIFARE_UL_WRITE_OK;

    /** Indicates authenticated-only write access (only for Mifare UL-C) */
    static final byte AR_WRITE_AUTHENTICATED = ConstantAutogen.W_MIFARE_UL_WRITE_AUTHENTICATED;

    /** Indicates no write access at all is granted, due to lock bits */
    static final byte AR_WRITE_LOCKED = ConstantAutogen.W_MIFARE_UL_WRITE_LOCKED;

    /**
     * Forces Open NFC to consider a Mifare UL card as a Mifare UL-C card.
     *
     * @throws IllegalStateException the connection is closed.
     */
    void forceULC();

    /**
     * Freezes the current lock bit configuration of a Mifare UL, Mifare UL-C card.
     *
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    void freezeDataLockConfiguration() throws NfcException;

    /**
     * Gets the access rights of an area of a Mifare UL / Mifare UL-C.
     *
     * @param offset the start address of the area to be checked.
     * @param length the length of the area to be checked.
     *
     * @return the access rights. Composed of two nibbles:<ul>
     *         <li>LSB nibble : the read access rights. Possible values AR_READ_OK, AR_READ_AUTHENTICATED.</li>
     *         <li>MSB nibble : the write access rights. Possible values are AR_WRITE_OK, AR_WRITE_AUTHENTICATED, AR_WRITE_LOCKED.</li>
     *         </ul>
     *
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    int getAccessRights(int offset, int length) throws NfcException;

    /**
     * Sets the access rights of a Mifare UL-C.
     *
     * @param key the authentication key (16 bytes).
     * @param threshold the sector number from which the rights are applied (0x03 - 0x30).
     * @param rights the rights to be set. Composed of two nibbles:<ul>
     *         <li>LSB nibble : the read access rights. Possible values AR_READ_OK, AR_READ_AUTHENTICATED.</li>
     *         <li>MSB nibble : the write access rights. Possible values are AR_WRITE_OK, AR_WRITE_AUTHENTICATED, AR_WRITE_LOCKED.</li>
     *         </ul>
     * @param lock locks the access rights after programming.
     *
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    void setAccessRights(byte[] key, int threshold, int rights, boolean lock) throws NfcException;

    /**
     * Performs authentication with a Mifare UL-C.
     *
     * @param key the key to be used for authentication. If set to null,
     *        the authentication is performed with the default transport key
     *        of the Mifare UL-C cards.
     *
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException the authentication failed or a NFC error occurred.
     */
    void authenticate(byte[] key) throws NfcException;

    /**
     * Returns the UID value.
     *
     * @return the UID value stored in a byte array of the same length.
     */
    byte[] getUid();

    /**
     * Returns the size of the tag sectors.
     *
     * @return the sector size in bytes. Set to zero if the sector size in unknown.
     **/
    int getSectorSize();

    /**
     * Retunrs the number of sector in the tag.
     *
     * @return the sector number. Set to zero if the sector number is unknown.
     */
    int getSectorNumber();

    /**
     * Reads a specified area of Mifare tag.
     *
     * @param offset the position within the tag of area to be read.
     *
     * @param length the length of the area of the tag to be read.
     *
     * @return the data read.
     *
     * @throws IllegalArgumentException if <code>offset</code> or <code>length</code> are not valid.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    byte[] read(int offset, int length) throws NfcException;

    /**
     * Writes data into a specified area of a Mifare tag.
     *
     * @param buffer the data to be written in the tag.
     * @param offset the position of the tag of the area to be written.
     * @param lock locks the sector containing the area after write.
     *
     * @throws IllegalArgumentException if <code>buffer</code> is null or empty.
     * @throws IllegalArgumentException if <code>offset</code> is not valid.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    void write(byte[] buffer, int offset, boolean lock) throws NfcException;
}
