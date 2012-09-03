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

import org.opennfc.NfcErrorCode;

/**
 * Interface definition for a callback to be invoked each time a card of the
 * specified type is detected.
 *
 * @since Open NFC 4.0
 */
public interface CardDetectionEventHandler {

    /**
     * Method called when a new card is detected. The method
     * implementation should call {@link Connection#getProperties Connection.getProperties()} or
     * {@link Connection#checkProperty Connection.checkProperty()}
     * to check the type of the card and perform any necessary operations.
     *
     * @param connection the connection with the card.
     */
    void onCardDetected(Connection connection);

    /**
     * Method called when an error occurs during a detection.
     *
     * @param what the type of error that has occurred.
     */
    void onCardDetectedError(NfcErrorCode what);
}
