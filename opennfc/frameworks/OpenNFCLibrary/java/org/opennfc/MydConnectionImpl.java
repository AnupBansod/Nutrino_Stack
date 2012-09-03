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

import org.opennfc.cardlistener.MydConnection;

/**
 * My-D connection implementation.<br>
 * For read My-D card
 * 
 * @hide
 */
class MydConnectionImpl extends Iso14443Part3ConnectionImpl implements MydConnection {
    /** Configuration of the card */
    private int configurationByte = -1;
    /** Number of password authentication attempts prior the card deselects */
    private int passwordRetryCounter = -1;
    /** Sector number */
    private int sectorNumber;
    /** Sector size */
    private int sectorSize;
    /** Card Unique IDentifier */
    private byte[] uid;

    /**
     * Create the connection
     * 
     * @param connectionHandle Handle
     */
    MydConnectionImpl(final SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /**
     * Performs authentication with a My-D
     * 
     * @param password the password to be used for authentication.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException the authentication failed or a NFC error occurred.
     */
    @Override
    public void authenticate(final int password) throws NfcException {

        final CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.WMyDMoveAuthenticate(this.getNonNullHandle(), callback.getId(), password);
        callback.waitForCompletionOrException();
    }

    /**
     * Freezes the current lock bit configuration of a My-D card.
     * 
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    @Override
    public void freezeDataLockConfiguration() throws NfcException {

        final CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.WMyDMoveFreezeDataLockConfiguration(this.getNonNullHandle(), callback.getId());
        callback.waitForCompletionOrException();
    }

    /**
     * Collect card configuration
     * 
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException If card connection lost or problem while getting configuration
     */
    private void getConfiguration() throws NfcException {
        if (this.configurationByte < 0 || this.passwordRetryCounter < 0) {
            final CallbackMyDMoveGetConfigurationCompleted callback = new CallbackMyDMoveGetConfigurationCompleted();

            MethodAutogen.WMyDMoveGetConfiguration(this.getNonNullHandle(), callback.getId());

            callback.waitForCompletionOrException();

            this.configurationByte = callback.nStatusByte;
            this.passwordRetryCounter = callback.nPasswordRetryCounter;
        }
    }

    /**
     * Returns the configuration of the card.<br>
     * This is a bitwise of the {@link #MY_D_MOVE_CONFIG_LOCKED}, {@link #MY_D_MOVE_WRITE_PASSWORD_ENABLED},
     * {@link #MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED} and {@link #MY_D_MOVE_16BITS_COUNTER_ENABLED} bits.
     * 
     * @return The configuration of the card.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException If card connection lost or problem while getting configuration byte
     */
    @Override
    public int getConfigurationByte() throws NfcException {
        this.getConfiguration();

        return this.configurationByte;
    }

    /**
     * Collect card connection information
     */
    private void getConnectionInfo() {
        if (this.uid == null) {
            final byte[] pInfoBuffer = new byte[11];
            NfcManager.checkError(MethodAutogen.WMyDGetConnectionInfoBuffer(this.getNonNullHandle(), pInfoBuffer));

            this.uid = new byte[7];
            System.arraycopy(pInfoBuffer, 0, this.uid, 0, 7);

            this.sectorSize = pInfoBuffer[7] & 0xFF << 8 | pInfoBuffer[8] & 0xFF;
            this.sectorNumber = pInfoBuffer[9] & 0xFF << 8 | pInfoBuffer[10] & 0xFF;
        }
    }

    /**
     * Returns the value of the tag identifier.
     * 
     * @return the identifier of the tag or null if there is no identifier.
     **/
    @Override
    public byte[] getIdentifier() throws NfcException {
        return this.getUid();
    }

    /**
     * The number of password authentication attempts prior the card deselects (0-7). When set to zero, the retry counter is not
     * enabled.
     * 
     * @return The number of password authentication attempts prior the card deselects (0-7)
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException If card connection lost or problem while getting password retry counter
     */
    @Override
    public int getPasswordRetryCounter() throws NfcException {
        this.getConfiguration();

        return this.passwordRetryCounter;
    }

    /**
     * Returns the number of sector in the tag.
     * 
     * @return the sector number. Set to zero if the sector number is unknown.
     */
    @Override
    public int getSectorNumber() {
        this.getConnectionInfo();

        return this.sectorNumber;
    }

    /**
     * Returns the size of the tag sectors.
     * 
     * @return the sector size in bytes. Set to zero if the sector size in unknown.
     **/
    @Override
    public int getSectorSize() {
        this.getConnectionInfo();

        return this.sectorSize;
    }

    /**
     * Returns the UID value.
     * 
     * @return the UID value stored in a byte array of the same length.
     */
    @Override
    public byte[] getUid() {
        this.getConnectionInfo();

        return this.uid;
    }

    /**
     * Returns the capability of the tag to be set in read-only mode.
     * 
     * @return true if the tag can be set is read-only mode, false is the tag cannot be set in read-only or if this information
     *         is unknown.
     * @throws NfcException On connection issue
     */
    @Override
    public boolean isLockable() throws NfcException {
        return (this.getConfigurationByte() & MydConnection.MY_D_MOVE_CONFIG_LOCKED) == 0;
    }

    /**
     * Returns the read-only status of the tag.
     * 
     * @return true if the tag cannot be written, false if the tag is read-only or if the status is unknown.
     * @throws NfcException On connection issue
     */
    @Override
    public boolean isReadOnly() throws NfcException {
        return (this.getConfigurationByte() & MydConnection.MY_D_MOVE_CONFIG_LOCKED) != 0;
    }

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
    @Override
    public byte[] read(final int offset, final int length) throws NfcException {

        if ((offset < 0) || (length <= 0)) {
            throw new IllegalArgumentException("offset or length are not valid");
        }

        final byte[] dataRead = new byte[length];

        final CallbackGenericCompletion callback = new CallbackGenericCompletion();

        final ByteBufferReference pBuffer = new ByteBufferReference(dataRead, false);

        MethodAutogen.WMyDRead(this.getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset, null);

        callback.waitForCompletionOrException();

        pBuffer.release(true);

        return dataRead;
    }

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
    @Override
    public void setAccessRights(final int statusByte, final int passwordRetryCounter, final int password, final boolean lock)
            throws NfcException {
        if (statusByte != 0 //
                || statusByte != (MydConnection.MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED
                        | MydConnection.MY_D_MOVE_WRITE_PASSWORD_ENABLED | MydConnection.MY_D_MOVE_16BITS_COUNTER_ENABLED) //
                || statusByte != (MydConnection.MY_D_MOVE_WRITE_PASSWORD_ENABLED | MydConnection.MY_D_MOVE_16BITS_COUNTER_ENABLED) //
                || statusByte != (MydConnection.MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED | MydConnection.MY_D_MOVE_16BITS_COUNTER_ENABLED) //
                || statusByte != (MydConnection.MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED | MydConnection.MY_D_MOVE_WRITE_PASSWORD_ENABLED) //
                || statusByte != (MydConnection.MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED) //
                || statusByte != (MydConnection.MY_D_MOVE_WRITE_PASSWORD_ENABLED) //
                || statusByte != (MydConnection.MY_D_MOVE_16BITS_COUNTER_ENABLED)) {
            throw new IllegalArgumentException("Invalid statusByte : " + Integer.toHexString(statusByte));
        }

        if (passwordRetryCounter < 0 || passwordRetryCounter > 7) {
            throw new IllegalArgumentException("passwordRetryCounter must be in (0-7) not " + passwordRetryCounter);
        }

        final CallbackGenericCompletion callback = new CallbackGenericCompletion();

        MethodAutogen.WMyDMoveSetConfiguration(this.getNonNullHandle(), callback.getId(), statusByte, passwordRetryCounter,
                password, lock);

        callback.waitForCompletionOrException();
    }

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
    @Override
    public void write(final byte[] buffer, final int offset, final boolean lock) throws NfcException {

        if ((buffer == null) || (buffer.length == 0)) {
            throw new IllegalArgumentException("buffer is null or empty");
        }

        if (offset < 0 || offset >= buffer.length) {
            throw new IllegalArgumentException("offset is not valid");
        }

        final CallbackGenericCompletion callback = new CallbackGenericCompletion();

        final ByteBufferReference pBuffer = new ByteBufferReference(buffer, true);

        MethodAutogen.WMyDWrite(this.getNonNullHandle(), callback.getId(), pBuffer.getValue(), offset, lock, null);

        callback.waitForCompletionOrException();

        pBuffer.release(false);
    }
}
