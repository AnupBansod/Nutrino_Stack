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
import org.opennfc.NfcPriority;
import org.opennfc.NfcErrorCode;
import org.opennfc.ConnectionProperty;

/**
 * Interface to listen to the card detection events.
 *
 * @since Open NFC 4.0
 */
public interface CardListenerRegistry {

    /**
     * Registers a new card listener for every type of card.
     *
     * <p>Once a detection handler is registered, the NFC Controller performs
     * automatically the detection sequence for every types of card.
     * The NFC Controller regularly scans the RF field with a low power detection sequence.
     * If a card is detected, the event handler is called.
     * The event handlers are called again each time a new card is detected.
     * </p>
     * <p>
     * {@link #unregisterCardListener unregisterCardListener()} should be called to unregister the listener.
     * </p>
     * <p>
     * The handler method may use the methods {@link Connection#checkProperty Connection.checkProperty()} or
     * {@link Connection#getProperties Connection.getProperties()} to get or check the properties of the connection.
     * </p>
     *
     * @param priority the priority used to listen to the tag must be a value
     *            between {@link NfcPriority#MINIMUM} and {@link NfcPriority#MAXIMUM} included.
     *            The exclusive access is requested with the value {@link NfcPriority#EXCLUSIVE}.
     *            If an exclusive access is requested, the error {@link NfcErrorCode#EXCLUSIVE_REJECTED}
     *            is returned if there is already another registration for this type or a sub-type.
     *            If a shared access is requested, the error {@link NfcErrorCode#SHARE_REJECTED}
     *            is returned if there is already an exclusive registration for this type or a sub-type.
     *
     * @param handler the {@link CardDetectionEventHandler} whose
     *            {@link CardDetectionEventHandler#onCardDetected onCardDetected()} method will
     *            be called for each connection with a new card.
     *
     * @throws SecurityException if the priority is {@link NfcPriority#MAXIMUM} or {@link NfcPriority#EXCLUSIVE}.
     *         and the calling application is not allowed to use these priority values.
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is already registered.
     * @throws IllegalArgumentException if <code>priority</code> is not valid.
     * @throws NfcException  in case of priority error or a registration error occurs.
     *
     * @see #unregisterCardListener unregisterCardListener()
     * @see CardDetectionEventHandler
     */
    void registerCardListener(int priority, CardDetectionEventHandler handler) throws NfcException;

    /**
     * Registers a new card listener for specific cards.
     *
     * <p>Once a detection handler is registered, the NFC Controller performs
     * automatically the detection sequence for the specified type(s) of cards.
     * The NFC Controller regularly scans the RF field with a low power detection sequence.
     * If a card is detected, the event handler is called.
     * The event handlers are called again each time a new card is detected.
     * </p>
     * <p>
     * {@link #unregisterCardListener unregisterCardListener()} should be called to unregister the listener.
     * </p>
     * <p>
     * The parameter <code>properties</code> defines the type of card to detect.
     * The connection returned to the handler should have at least one of the connection property defined in <code>properties</code>.
     * It may also have other properties.
     * </p>
     * <p>
     * If one of the properties specified by <code>properties</code> is not supported, the property is ignored.
     * If all the property values are unknown or not supported, the event handler is never called.
     * Use {@link #checkConnectionProperty checkConnectionProperty()} to check is a property is supported by the NFC Controller.
     * </p>
     * <p>
     * The event handler method may use the methods {@link Connection#checkProperty Connection.checkProperty()} or
     * {@link Connection#getProperties Connection.getProperties()} to get or check the properties of the connection.
     * Then the event handler should perform any necessary operations to decide if the card should be managed by this event handler.
     * <ul>
     *   <li>If the card is of the right type, the event handler should perform the communication with the card.
     *       Then the function should call {@link Connection#close(boolean,boolean)} with the parameter cardApplicationMatch
     *       set to true to prevent the unknown card detection handler from being called.</li>
     *   <li>If the card is not of the right type, the function should call {@link Connection#close(boolean,boolean)}
     *       with the parameter cardApplicationMatch set to false and giveToNextListener set to true.
     *       This will cause the other handlers to be called.</li>
     * </ul>
     * </p>
     * <p>
     * The handler function should call {@link Connection#close(boolean,boolean)}
     * to inform the framework that the work with the card is terminated.
     * {@link Connection#close(boolean,boolean)} closes the connection with the card.
     * </p>
     * <p>
     * If the card connection should be transferred to other eveny handlers,
     * the function should call {@link Connection#close(boolean,boolean)} with giveToNextListener set to true.
     * Otherwise, the function should set giveToNextListener to false to prevent any further communication with the card.
     * </p>
     * <p>
     * The following figure depicts the global card detection sequence:<br/>
     * <br/><img src="./doc-files/CardListenerRegistry-1.gif"/><br/>
     * </p>
     *
     * @param priority the priority used to listen to the tag must be a value
     *            between {@link NfcPriority#MINIMUM} and {@link NfcPriority#MAXIMUM} included.
     *            The exclusive access is requested with the value {@link NfcPriority#EXCLUSIVE}.
     *            If an exclusive access is requested, the error {@link NfcErrorCode#EXCLUSIVE_REJECTED}
     *            is returned if there is already another registration for this type or a sub-type.
     *            If a shared access is requested, the error {@link NfcErrorCode#SHARE_REJECTED}
     *            is returned if there is already an exclusive registration for this type or a sub-type.
     *
     * @param properties the array of connection properties defining which type
     *            of card should be detected. This array may contain one or
     *            several properties defined in {@link ConnectionProperty}.
     *
     * @param handler the {@link CardDetectionEventHandler} whose
     *            {@link CardDetectionEventHandler#onCardDetected onCardDetected()} method will
     *            be called for each connection with a new card.
     *
     * @throws SecurityException if the priority is {@link NfcPriority#MAXIMUM} or {@link NfcPriority#EXCLUSIVE}.
     *         and the calling application is not allowed to use these priority values.
     * @throws IllegalArgumentException if <code>properties</code> or <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is already registered.
     * @throws IllegalArgumentException if <code>priority</code> is not valid.
     * @throws NfcException  in case of priority error or a registration error occurs.
     *
     * @see #unregisterCardListener unregisterCardListener()
     * @see CardDetectionEventHandler
     * @see ConnectionProperty
     * @see #checkConnectionProperty checkConnectionProperty()
     */
    void registerCardListener(int priority, ConnectionProperty[] properties,
            CardDetectionEventHandler handler) throws NfcException;

    /**
     * Unregisters a card listener registered with {@link #registerCardListener registerCardListener()}.
     *
     * @param handler the card detection handler.
     *
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is not already registered.
     *
     * @see #registerCardListener registerCardListener()
     */
    void unregisterCardListener(CardDetectionEventHandler handler);

    /**
     * Checks if the NFC Controller supports a connection property for the card listener function.
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
