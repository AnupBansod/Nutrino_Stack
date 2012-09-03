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
public interface Iso14443Part4BConnection extends Connection {

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
     * The current value of the node identifier (NAD). If the NAD is not
     * supported, the value is set to zero.
     *
     * @throws IllegalStateException the connection is closed.
     */
    byte getNad();

}
