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
 * The FeliCa API provides access to the tags compliant with the FeliCa Specification.
 *
 * @since Open NFC 4.0
 */
public interface FeliCaConnection extends Connection {

    /**
     * Reads a specified area of the card.
     *
     * @param length the length of the area to be read.
     * @param serviceCodeList the service code list of the tag used for read operation
     * @param blockList the block list used for read operation.
     *
     * @return The data read.
     *
     * @throws IllegalArgumentException if <code>length</code> is not valid.
     * @throws IllegalArgumentException if <code>serviceCodeList</code> is null or empty.
     * @throws IllegalArgumentException if <code>blockList</code> is null or empty.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    byte[] read(int length, int[] serviceCodeList, byte[] blockList) throws NfcException;

    /**
     * Writes data into a specified area of the card.
     *
     * @param buffer the data to be written in the tag.
     * @param serviceCodeList the service code list of the tag used for write operation.
     * @param blockList the block list used for write operation.
     *
     * @throws IllegalArgumentException if <code>buffer</code> is null or empty.
     * @throws IllegalArgumentException if <code>serviceCodeList</code> is null or empty.
     * @throws IllegalArgumentException if <code>blockList</code> is null or empty.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException a NFC error occurred.
     */
    void write(byte[] buffer, int[] serviceCodeList, byte[] blockList) throws NfcException;

    /**
     * Returns the manufacturer ID (IDm), used by the NFC Device to address the card.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte[] getManufacturerId();

    /**
     * Returns the manufacturer Parameter (PMm).
     * Contains the Maximum Response Time parameter.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte[] getManufacturerParameter();

    /**
     * Returns the system code of the target.
     *
     * @throws IllegalStateException the connection is closed.
     */
    short getSystemCode();
}
