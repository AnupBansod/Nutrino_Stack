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
 * This class gives an access to the ISO 14443 protocol part 4 type B.
 *
 * @since Open NFC 4.0
 */
public interface Iso14443Part3BConnection extends Connection {

    /**
     * Performs an exchange of data between the local device and the card.
     *
     * @param command the command to be send to the card.
     *
     * @return the answer received from the card.
     *
     * @throws IllegalArgumentException if <code>command</code> is null or empty.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException if a communication error occurs.
     */
    byte[] exchangeData(byte[] command) throws NfcException;

    /**
     * Sets the 14443-3B RF timeout.
     *
     * @param timeout the timeout value in milliseconds.
     *
     * @throws IllegalStateException the connection is closed.
     */
    void setTimeout(int timeout);

    /**
     * Returns the 12 bytes of the ATQB frame returned by the card.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public byte[] getAtqb();

    /**
     * Returns the AFI value. The Application Family Identifier is used to select a card
     * by family card. This value is included in the ATQB frame, in the
     * application data field.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public byte getAfi();

    /**
     * Returns the flag indicating if the CID is supported. true if the CID is supported,
     * false if the CID is not supported. The value of the CID is given by the
     * field nMBLI_CID. This value is returned by the card in the FO bits of the
     * Protocol Data in the ATQB frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public boolean isCidSupported();

    /**
     * Returns the flag indicating if the NAD is supported. true if the NAD is supported,
     * false if the NAD is not supported. This value is returned by the card in
     * the FO bits of the Protocol Data in the ATQB frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public boolean isNadSupported();

    /**
     * Returns the size in bytes of the card input buffer. This value is returned by the
     * card in the Protocol Data in the ATQB frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public int getCardInputBufferSize();

    /**
     * Returns the size in bytes of the Reader input buffer. This value is always 256.
     * The value is sent to the card in the ATTRIB frame.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public int getReaderInputBufferSize();

    /**
     * Returns the baud rate in kbits/s of the connection between the PICC and PCD in
     * both direction. This value is computed between the NFC Controller and the
     * card.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public int getBaudRate();

    /**
     * Returns the current timeout value used for the connection. This value is
     * initialized with the value included in the ATQB. The value may be changed
     * with {@link #setTimeout}.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public int getTimeout();

    /**
     * Returns the Higher Layer data sent to the card in the ATTRIB frame. This value is
     * optional, if present it contains ISOCEI14443-4 data.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public byte[] getHigherLayerData();

    /**
     * Returns the Higher Layer Response data is included in the ATTRIB response frame
     * returned by the card. This value is optional, if present it contains
     * ISOCEI14443-4 response.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public byte[] getHigherLayerResponse();

    /**
     * Returns the MBLI-CID byte included in the ATTRIB response frame returned by the
     * card.
     *
     * @throws IllegalStateException the connection is closed.
     */
    public byte getMbliCid();
}
