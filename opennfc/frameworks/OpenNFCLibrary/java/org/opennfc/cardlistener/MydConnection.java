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
import org.opennfc.nfctag.NfcTagConnection;

/**
 * Connection with My-D card
 */
public interface MydConnection extends NfcTagConnection {
    /** If set, indicates blocks 0x22 and 0x23 are configured as a 16 bits counter */
    public static final int MY_D_MOVE_16BITS_COUNTER_ENABLED = ConstantAutogen.W_MY_D_MOVE_16BITS_COUNTER_ENABLED;
    /** If set, indicates the my-d move card configuration is locked and can not been changed */
    public static final int MY_D_MOVE_CONFIG_LOCKED = ConstantAutogen.W_MY_D_MOVE_CONFIG_LOCKED;
    /**
     * If set, indicates the read and write password is active for read, write and decrement commands for all blocks above
     * address 0x0F
     */
    public static final int MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED = ConstantAutogen.W_MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED;
    /** If set, indicates the write password is active for write commands for all blocks starting from address 0x10 */
    public static final int MY_D_MOVE_WRITE_PASSWORD_ENABLED = ConstantAutogen.W_MY_D_MOVE_WRITE_PASSWORD_ENABLED;

    /**
     * Performs authentication with a My-D
     * 
     * @param password the password to be used for authentication.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException the authentication failed or a NFC error occurred.
     */
    public void authenticate(int password) throws NfcException;

    /**
     * Freezes the current lock bit configuration of a My-D card.
     * 
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    public void freezeDataLockConfiguration() throws NfcException;

    /**
     * Returns the configuration of the card.<br>
     * This is a bitwise of the {@link #MY_D_MOVE_CONFIG_LOCKED}, {@link #MY_D_MOVE_WRITE_PASSWORD_ENABLED},
     * {@link #MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED} and {@link #MY_D_MOVE_16BITS_COUNTER_ENABLED} bits.
     * 
     * @return The configuration of the card.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException If card connection lost or problem while getting configuration byte
     */
    public int getConfigurationByte() throws NfcException;

    /**
     * The number of password authentication attempts prior the card deselects (0-7). When set to zero, the retry counter is not
     * enabled.
     * 
     * @return The number of password authentication attempts prior the card deselects (0-7)
     * @throws NfcException If card connection lost or problem while getting password retry counter
     */
    public int getPasswordRetryCounter() throws NfcException;

    /**
     * Returns the number of sector in the tag.
     * 
     * @return the sector number. Set to zero if the sector number is unknown.
     */
    public int getSectorNumber();

    /**
     * Returns the size of the tag sectors.
     * 
     * @return the sector size in bytes. Set to zero if the sector size in unknown.
     **/
    public int getSectorSize();

    /**
     * Returns the UID value.
     * 
     * @return the UID value stored in a byte array of the same length.
     */
    public byte[] getUid();

    /**
     * Reads a specified area of My-D tag.
     * 
     * @param offset the position within the tag of area to be read.
     * @param length the length of the area of the tag to be read.
     * @return the data read.
     * @throws IllegalArgumentException if <code>offset</code> or <code>length</code> are not valid.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    public byte[] read(int offset, int length) throws NfcException;

    /**
     * Sets the access rights of a My-D
     * 
     * @param statusByte The configuration of the card. This is a bitwize of the {@link #MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED},
     *            {@link #MY_D_MOVE_WRITE_PASSWORD_ENABLED} and {@link #MY_D_MOVE_16BITS_COUNTER_ENABLED} bits.
     * @param passwordRetryCounter The number of password authentication attempts prior the card deselects (0-7). When set to
     *            zero, the retry counter is not enabled.
     * @param password The password to be used for authentication
     * @param lock If set to true, the status byte and password retry counter will be locked after the operation. Tag status
     *            will be {@link #MY_D_MOVE_CONFIG_LOCKED}
     * @throws IllegalStateException the connection is closed.
     * @throws IllegalArgumentException the statusByte is not zero or a correct bitwize of the
     *             {@link #MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED}, {@link #MY_D_MOVE_WRITE_PASSWORD_ENABLED} and
     *             {@link #MY_D_MOVE_16BITS_COUNTER_ENABLED} bits.
     * @throws IllegalArgumentException passwordRetryCounter not in (0-7)
     * @throws NfcException a NFC error occurred.
     */
    public void setAccessRights(int statusByte, int passwordRetryCounter, int password, boolean lock) throws NfcException;

    /**
     * Writes data into a specified area of a My-D tag.
     * 
     * @param buffer the data to be written in the tag.
     * @param offset the position of the tag of the area to be written.
     * @param lock locks the sector containing the area after write.
     * @throws IllegalArgumentException if <code>buffer</code> is null or empty.
     * @throws IllegalArgumentException if <code>offset</code> is not valid.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    public void write(byte[] buffer, int offset, boolean lock) throws NfcException;
}
