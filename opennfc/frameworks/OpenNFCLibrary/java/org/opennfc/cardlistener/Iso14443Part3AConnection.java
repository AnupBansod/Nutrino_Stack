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
 * This class gives an access to the ISO 14443 protocol part 3 type A.
 *
 * @since Open NFC 4.0
 */
public interface Iso14443Part3AConnection extends Connection {

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
     * Returns the UID of the card.
     *
     * @return the UID value on 4,7 or 10 bytes.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte[] getUid();

    /**
     * Returns the ATQA value.
     * <p/>
     * The Answer to Request Type A code contains information about:
     * <ul>
     * <li>anti-colision frame,</li>
     * <li>UID size, and</li>
     * <li>the proprietary bits. </li>
     * </ul>
     *
     * @return the ATQA value.
     *
     * @throws IllegalStateException the connection is closed.
     */
    short getAtqa();

    /**
     * Return the SAK value.
     * <p/>
     * The Select Acknowledge Type A byte determines if the UID
     * is completed and card is compliant with ISO/IEC 14443-4 protocol.
     *
     * @return the SAK byte value.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getSak();
}
