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
 * The specification ISO 7816-4 defines a protocol to communicate with a smart
 * card. Originally this protocol was designed for contact smart cards but it is
 * also used on top of ISO 14443-4 A/B to communicate with contactless cards.
 *
 * @since Open NFC 4.0
 */
public interface Iso7816Part4Connection extends Connection {

    /**
     * Performs an APDU exchange between the card listener and the card.
     * <p>
     * The implementation of exchangeApdu() includes an automatic chaining of
     * the APDUs. For T=0 protocol, for case 4 and case 2 command APDUs the card
     * may respond with "61 XX" or "6C XX". If the response is "61 XX",
     * the implementation sends GET RESPONSE to the card to get the response
     * data before any other command is sent. If the response is "6C XX",
     * the implementation resends the command after setting Le equal to XX
     * received from the card before any other command is sent.</p>
     * <p>
     * In both the cases discussed above, the Open NFC implementation makes sure
     * that between sending the command APDU, receiving status word "61 XX"
     * or "6C XX", and sending GET RESPONSE or resending the command APDU
     * with Le set to XX respectively, there is not any other APDU exchange
     * on any logical channel with the card. In case the status word "61 XX"
     * is received multiple times successively from the card, the implementation
     * accumulates all the response data received from the card before returning
     * it to the calling application.</p>
     * <p>
     * The calling application remains oblivious of the exchanges mentioned above
     * and should only get the response received as a result of the above operations.</p>
     * <p>
     * The connection handle represents the default logical channel
     * (with the identifier zero) or a logical channel open with openLogicalChannel().
     * The implementation sets automatically the logical channel identifier
     * in the class byte (CLA) of the command APDU.</p>
     * <p>
     * Use the method openLogicalChannel() to open a new logical channel and to
     * select an application.</p>
     * <p>
     * There may be several logical channels open at the same time with
     * the same card. However, since the APDU protocol is synchronous,
     * there can be no interleaving of command and their response APDUs across
     * logical channels. Between the receipt of the command APDU and the sending
     * of the response APDU for that command, only one logical channel is active.</p>
     * <p>
     * The method exchangeApdu() rejects the command APDU of type MANAGE_CHANNEL
     * with an exception NfcException.</p>
     * <p>
     * On the logical channels with an identifier different from zero,
     * the method exchangeApdu() rejects the command APDU of type SELECT_BY_AID
     * with an exception NfcException.</p>
     *
     * @param commandApdu the command APDU to be sent to the card.
     *
     * @return the response APDU sent by the card.
     *
     * @throws IllegalArgumentException if <code>commandApdu</code> is null or empty.
     * @throws IllegalStateException the connection is closed.
     * @throws SecurityException a security error is detected.
     * @throws NfcException if a communication error occurs.
     */
    byte[] exchangeApdu(byte[] commandApdu) throws NfcException;

    /**
     * Opens a logical channel to communicate with an application.
     * <p>
     * This method opens a logical channel with the card, selecting the
     * application represented by the given AID. It's up to the card to choose
     * which logical channel will be used.</p>
     * <p>
     * openLogicalChannel() returns a connection representing the new logical
     * channel. The method exchangeApdu() of the new channel connection shall be
     * used to communicate with the selected application.</p>
     * <p>
     * After usage, the logical channel should be closed.</p>
     *
     * @param  aid  the target application identifier (AID).
     *
     * @return the connection on the new logical channel.
     *
     * @throws IllegalArgumentException if <code>aid</code> is null or empty.
     * @throws IllegalStateException the connection is closed.
     * @throws SecurityException a security error is detected.
     * @throws NfcException with the error code
     *         {@link org.opennfc.NfcErrorCode#FEATURE_NOT_SUPPORTED FEATURE_NOT_SUPPORTED}
     *         if the logical channels are not supported by the card.
     * @throws NfcException with the error code
     *         {@link org.opennfc.NfcErrorCode#ITEM_NOT_FOUND ITEM_NOT_FOUND}
     *         if the application cannot be selected or the AID does not match an application.
     * @throws NfcException with the error code
     *         {@link org.opennfc.NfcErrorCode#RF_COMMUNICATION RF_COMMUNICATION}
     *         if another error occured during the creation of the logical channel.
     */
    Iso7816Part4Connection openLogicalChannel(byte[] aid) throws NfcException;
}

