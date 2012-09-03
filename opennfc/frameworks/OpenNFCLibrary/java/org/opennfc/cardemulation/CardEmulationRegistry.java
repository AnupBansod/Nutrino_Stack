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

package org.opennfc.cardemulation;

import org.opennfc.ConnectionProperty;

/**
 * The card emulation registry is used to register a card emulation.
 *
 * @since Open NFC 4.1
 */
public interface CardEmulationRegistry {

    /**
     * Creates a new instance of a card emulation.
     * <p/>
     * The emulation will start only when {@link CardEmulation#start CardEmulation.start()} is called.
     *
     * @param cardType the type of the card to be created:
     *         <ul>
     *           <li>{@link ConnectionProperty#ISO_14443_4_A} for a card of type ISO 14443-4 A, or</li>
     *           <li>{@link ConnectionProperty#ISO_14443_4_B} for a card of type ISO 14443-4 B.</li>
     *         </ul>
     * @param identifier the card identifier. For a tag of type A, the length of the identifier may be 4, 7 or 10 bytes.
     *        For a tag of type B, the length of the identifier shall be 4.
     *
     * @return the card emulation instance.
     *
     * @throws SecurityException if the calling application is not allowed to perform card emulation.
     * @throws IllegalArgumentException if <code>cardType</code> or <code>identifier</code> is null.
     * @throws IllegalArgumentException if <code>cardType</code> is unknown
     *         or the <code>identifier</code> length is not compliant with the card type.
     */
    CardEmulation createCardEmulation(ConnectionProperty cardType, byte[] identifier);

    /**
     * Creates a new instance of a card emulation.
     * <p/>
     * The emulation will start only when {@link CardEmulation#start CardEmulation.start()} is called.
     *
     * @param cardType the type of the card to be created:
     *         <ul>
     *           <li>{@link ConnectionProperty#ISO_14443_4_A} for a card of type ISO 14443-4 A, or</li>
     *           <li>{@link ConnectionProperty#ISO_14443_4_B} for a card of type ISO 14443-4 B.</li>
     *         </ul>
     * @param randomIdentifierLength the length in byte of the card identifier randomly generated for each reader selection.
     *        For a tag of type A, the length of the identifier may be 4, 7 or 10 bytes.
     *        For a tag of type B, the length of the identifier shall be 4.
     *
     * @return the card emulation instance.
     *
     * @throws SecurityException if the calling application is not allowed to perform card emulation.
     * @throws IllegalArgumentException if <code>cardType</code> is null.
     * @throws IllegalArgumentException if <code>cardType</code> is unknown or
     *         <code>randomIdentifierLength</code> is not compliant with the card type.
     */
    CardEmulation createCardEmulation(ConnectionProperty cardType, int randomIdentifierLength);

    /**
     * Checks if the NFC Controller supports a connection property for the card emulation function.
     *
     * @param  property  the connection property to check.
     *
     * @throws IllegalArgumentException if property in null.
     *
     * @return true if the type of connection is supported by the NFC Controller,
     *         false if this type of connection is not supported.
     */
    boolean checkConnectionProperty(ConnectionProperty property);
}
