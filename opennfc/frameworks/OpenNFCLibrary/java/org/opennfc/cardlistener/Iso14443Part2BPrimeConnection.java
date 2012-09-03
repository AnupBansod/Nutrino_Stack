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
 * Connection with B-Prime card
 */
public interface Iso14443Part2BPrimeConnection extends Connection {
    /**
     * Performs an exchange of data between the local device and the card.
     * 
     * @param command the command to be send to the card.
     * @return the answer received from the card.
     * @throws IllegalArgumentException if <code>command</code> is null or empty.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException if a communication error occurs.
     */
    public byte[] exchangeData(byte[] command) throws NfcException;

    /**
     * Returns the REPGEN sent by the card during the card detection.
     * 
     * @return the REPGEN value stored in a byte array of the same length.
     */
    public byte[] getRepgen();
}
