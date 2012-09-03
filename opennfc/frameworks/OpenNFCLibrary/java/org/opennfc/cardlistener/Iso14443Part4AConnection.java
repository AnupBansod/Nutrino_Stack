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
 * This class gives an access to the ISO 14443 protocol part 4 type A.
 *
 * @since Open NFC 4.0
 */
public interface Iso14443Part4AConnection extends Connection {

    /**
     * Performs an exchange of data between the local device and the card
     *
     * @param command the command to be send to the card. The command may be null or empty.
     *
     * @return the answer received from the card. The answer may be empty.
     *
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException if a communication error occurs.
     */
    byte[] exchangeData(byte[] command) throws NfcException;

    /**
     * Sets the value of the Node Identifier (NAD) for the connection.
     *
     * @param nad the NAD value to be set.
     *
     * @throws IllegalStateException the connection is closed.
     */
    void setNad(byte nad);

    /**
     * Returns the flag indicating if the CID is supported. true if the CID is supported,
     * false if the CID is not supported. The value of the CID is given by {@link #getCid}.
     * This value is returned by the card in the ATS frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    boolean isCidSupported();

    /**
     * Returns the value of the card identifier (CID). If the CID is not supported, the
     * value is set to zero. This value is sent to the card in the RATS frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getCid();

    /**
     * Returns the flag indicating if the NAD is supported. true if the NAD is supported,
     * false if the NAD is not supported. The current value of the NAD is given by {@link #getNad}.
     * This value is returned by the card in the ATS frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    boolean isNadSupported();

    /**
     * Returns the current value of the node identifier (NAD). If the NAD is not
     * supported, the value is set to zero.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getNad();

    /**
     * Returns the application data array contains the historical bytes. This value is
     * returned by the card in the ATS frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte[] getApplicationData();

    /**
     * Returns the size in bytes of the card input buffer This value is returned by the
     * card in the ATS frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    int getCardInputBufferSize();

    /**
     * Returns the size in bytes of the Reader input buffer. This value is sent to the
     * card in the RATS frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    int getReaderInputBufferSize();

    /**
     * Returns the FWI and SFGI value. The Frame Waiting time Integer (most significant
     * half byte) and Start-up Frame Guard time Integer (least significant half
     * byte) are used to define respectively the FWT and SFGT This value is
     * returned by the card in the ATS frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getFwiSfgi();

    /**
     * Returns the current timeout value used for the connection. This value is
     * initialized with the value included in the ATS.
     *
     * @throws IllegalStateException the connection is closed.
     */
    int getTimeout();

    /**
     * Returns the Data Rate Max value. The Data Rate Divisor codes the bit rate
     * capability for both direction : PICC to PCD and PPCD to PICC
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getDataRateMaxDiv();

    /**
     * Returns the baud rate in kbits/s of the connection between the PICC and PCD in
     * both direction.
     *
     * @throws IllegalStateException the connection is closed.
     */
    int getBaudRate();
}
