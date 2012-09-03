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

import org.opennfc.ConnectionProperty;

/**
 * The connection qualify a given connection with a card.
 *
 * @since Open NFC 4.0
 */
public interface Connection {

    /**
     * Retrieves the different properties associated to the connection.
     *
     * @return the connection properties.
     *
     * @throws IllegalStateException if the connection is closed.
     */
    ConnectionProperty[] getProperties();

    /**
     * Checks if a property is present in the connection.
     *
     * @param property the property to be checked.
     *
     * @return true if the property is present.
     *
     * @throws  IllegalArgumentException if property is null.
     * @throws IllegalStateException if the connection is closed.
     */
    boolean checkProperty(ConnectionProperty property);

    /**
     * Checks if this connection was previously used by another listener and
     * closed with the cardApplicationMatch flag set to true.
     *
     * <p>This information is useful to implement a default application for the cards.
     * A default application listens for some card types by registering a listener with a low priority.
     * When the default application receives the card connection, it checks with previousApplicationMatch()
     * if the card was used by other listeners of higher priority. If the card was not used,
     * the application performs the default actions for this card.</p>
     *
     * @return true if the card was used by another listener, false otherwise.
     */
    boolean previousApplicationMatch();

    /**
     * Informs the framework that the work performed on a connection is
     * terminated. The connection will be closed and will be no longer
     * available.
     *
     * <p>If the connection is already closed, this call has no effect.</p>
     *
     * @param  giveToNextListener  informs the framework if the card connection
     *         may be given to the next card listeners registered for this type of card.
     *
     * @param  cardApplicationMatch  informs the framework that the current card listener
     *         found something useful in this card, so the unknown card listener
     *         will not be called.
     */
    void close(boolean giveToNextListener, boolean cardApplicationMatch);

    /**
     * Informs the framework that the work performed on a connection is
     * terminated. The connection will be closed and will be no longer
     * available.
     * <p>If the connection is already closed, this call has no effect.</p>
     * <p>Equivalent to <code>close( true, true )</code>. </p>
     */
    void close();

}
