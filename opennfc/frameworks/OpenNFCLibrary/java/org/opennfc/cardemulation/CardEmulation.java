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

import org.opennfc.NfcException;
import org.opennfc.ConnectionProperty;

/**
 * The Card emulation is a feature of the NFC Controller used to emulate a card
 * with the NFC Device.
 * <p/>
 * The following figure depicts the life-cycle of the card emulation:<br/>
 * <br/><img src="./doc-files/CardEmulation-1.gif" width=70% height=70%/><br/>
 * <p/>
 * To create an instance of Card Emulation, use CardEmulationRegistry.createCardEmulation().
 * <p/>
 * <p><b>Starting the Card Emulation</b></p>
 * <p>
 * To emulate a card, a client application calls the method {@link #start start()}.
 * When this method returns successfully, an external reader may start to interact with the simulated card.</p>
 *
 * <p><b>Answering to the Reader</b></p>
 * <p>
 * When a command is received and analyzed, the answer can be sent back
 * to the reader using the method {@link #sendResponse sendResponse()}.</p>
 *
 * <p><b>Exclusivity</b></p>
 * <p>
 * Only one card of a given type can be emulated at the same time.
 * If several applications try to simultaneously emulate the same card type,
 * the first application obtaining the connection has the exclusive use of the service.
 * The other application are rejected. The card emulation can be open
 * when the owner application stops the card emulation.</p>
 *
 * <p><b>Stopping the Card Emulation</b></p>
 * <p>
 * The card emulation is stopped by calling the method {@link #stop stop()}.
 *
 * @since Open NFC 4.0
 */
public interface CardEmulation {

    /**
     * Starts the emulation of the card.
     *
     * @param handler a {@link CardEmulationEventHandler} whose
     *            {@link CardEmulationEventHandler#onEventReceived onEventReceived()} method will be
     *            called for each event received or
     *            {@link CardEmulationEventHandler#onCommandReceived onCommandReceived()} method will be
     *            called for each command received.
     *
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalStateException if the emulation is already started.
     * @throws NfcException in case of NFC error.
     */
    public void start(CardEmulationEventHandler handler) throws NfcException;

    /**
     * Stop the emulation of the card.
     *
     * If the emulation is already stopped, this method does nothing.
     */
    public void stop();

    /**
     * Sends the response of the emulated card to the reader.
     *
     * @param response the buffer with the response data to send to the reader.
     *
     * @throws IllegalArgumentException if <code>response</code> is null.
     * @throws NfcException if the card emulation is stopped or in case of NFC error.
     */
    public void sendResponse(byte[] response) throws NfcException;

}
