/*
 * Copyright (c) 2009-2010 Inside Secure, All Rights Reserved.
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

package org.opennfc.p2p;

import org.opennfc.NfcException;

/**
 * This interface exposes the methods for the Peer to Peer communication (P2P).
 * Use {@link org.opennfc.NfcManager#getP2PManager() NfcManager.getP2PManager()}
 * to create an new instance of P2P manager. Both <b>connection-oriented</b> and
 * <b>connectionless</b> transports defined in the LLCP specification are supported.
 * <p/>
 * The local P2P stack is configurable. The stack configuration is common to all
 * applications, meaning that only the last parameters set are taken into account.
 * A change of the P2P stack configuration does not impact the currently established
 * P2P link (if any), but only the future link establishments.
 * <p/>
 * Default configuration values should suit most application requirements, meaning
 * the configuration of the stack is optional.
 * <p/>
 * The P2P link establishment is requested by calling the {@link #establishLink
 * establishLink()} API. Once P2P link establishment has been requested, the NFC
 * controller starts hunting for the presence of a peer P2P device. When a P2P device
 * is found, the establishment {@link P2PLinkEventHandler#onLinkDetected(P2PLink)
 * P2PLinkEventHandler.onLinkDetected()} method is called.
 * <p/>
 * Both connection-oriented and connectionless object retrieved by a call to
 * {@link #createP2PConnection(Mode, String, byte) createP2PConnection()}
 * or {@link #createP2PConnectionLess(String, byte) createP2PConnectionLess()}
 * functions are not related to one P2P link session {@link P2PLink}, meaning the
 * same connection object can be used over several P2P link establishments. The
 * connection object are destroyed only when the corresponding handles are closed.
 *
 * @since Open NFC 4.1
 **/
public interface P2PManager {

    /**
     * Sets the Local Link Timeout expressed in ms.
     *
     * <p>Valid range is 10..2550 ms.It must be a multiple of 10 ms Recommended
     * value is 100 ms.
     *
     * <p>This value is sent to the peer device during P2P link establishment.
     *
     * @param localLto Local Link Timeout.
     *
     * @throws IllegalArgumentException if <code>localLto</code> are not valid.
     * @throws NfcException if a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public void setLocalLto(short localLto) throws NfcException;

    /**
     * Sets the Local Link Max Information Unit.
     *
     * <p>This value corresponds to the maximum size of a packet we can receive
     * from the peer device. Valid Range is 128..256.
     *
     * <p>Recommended value is 256 bytes.
     *
     * <p>This value is sent to the peer device during P2P link establishment.
     *
     * @param localMiu Local MIU.
     *
     * @throws IllegalArgumentException if <code>localMiu</code> are not valid.
     * @throws NfcException if a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public void setLocalMiu(short localMiu) throws NfcException;

    /**
     * Sets the flag indicating if the Initiator mode is used for the P2P connection.
     *
     * <p>Changing this flag has no effect on the current P2P detection. It is
     * only used when the P2P detection is restarted or when the device exits
     * from the standby mode.
     *
     * <p>Recommended value is true.
     *
     * <p>Note that the initiator mode shall be activated to be compliant
     * with the NFC Forum specification.
     *
     * <p>Note that two devices with the Initiator mode deactivated cannot
     * establish a P2P link.
     *
     * @param flag True to active the initiator mode. Otherwise false.
     *
     * @throws NfcException if a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public void setActivateInitiator(boolean flag) throws NfcException;

    /**
     * Retrieves the Local Link Timeout expressed in ms.
     *
     * @return The Local Link Timeout expressed in ms.
     *
     * @throws NfcException a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public short getLocalLto() throws NfcException;

    /**
     * Retrieves the Local Link Max Information Unit.
     *
     * @return The Local Link Max Information Unit.
     *
     * @throws NfcException a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public short getLocalMiu() throws NfcException;

    /**
     * Retrieves the flag indicating if the Initiator mode is used for the P2P
     * connection.
     *
     * @return True if the initiator mode is active. Otherwise false.
     *
     * @throws NfcException a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public boolean getActivateInitiator() throws NfcException;

    /**
     * Requests establishment of the P2P RF link.
     *
     * @param listener a {@link P2PLinkEventHandler} whose
     *            {@link P2PLinkEventHandler#onLinkDetected onLinkDetected()}
     *            method will be called when a new P2P link is established or
     *            {@link P2PLinkEventHandler#onLinkError onLinkError()}
     *            method will be called when an error occurs or
     *            {@link P2PLinkEventHandler#onLinkReleased onLinkReleased()}
     *            method will be called when the link is broken.
     *
     * @throws IllegalArgumentException if <code>listener</code> is null.
     * @throws IllegalStateException if a link operation is ongoing.
     * @throws NfcException a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public void establishLink(P2PLinkEventHandler listener) throws NfcException;

    /**
     * Cancels the pending establishment link procedure.
     * <p/>
     * If cancel is called when the link establishment is not yet completed, the
     * {@link P2PLinkEventHandler#onLinkDetected onLinkDetected()}
     * function is called.
     *
     * @since Open NFC 4.1
     */
    public void cancelLink();

    /**
     * Creates a new P2P connection-oriented object.
     * <p/>
     * Prior being able to send/receive data through a connection-oriented
     * transport, a connection must be established, using
     * {@link P2PConnection#connect OpenNfcP2PConnection.connect()} function
     * call. We distinguish 3 types of connection-oriented transports : client,
     * server and client/server transports.
     * <p/>
     * When called with parameter <code>type</code> set to
     * {@link P2PManager.Mode#SERVER}, a connection-oriented server transport
     * will be created.
     * <p/>
     * If non-zero, the parameter <code>SAP</code> contains the SAP value on which
     * the server will listen (valid range is 2 - 63). <code>SAP</code> is typically
     * only set to a non-zero value when implementing a "well-known service" that
     * must listen on a specific SAP value.
     * <p/>
     * If parameter <code>SAP</code> is set to zero, a free SAP will be allocated
     * by the stack.
     * <p/>
     * If non null, the parameter <code>sericeUri</code> contains the service name
     * associated to the connection. This service name will be registered into the
     * SDP (Service Discovery Protocol) implemented in the P2P stack, allowing a
     * remote device to connect to this server using its name.
     * <p/>
     * The combination of <code>SAP</code> set to null and <code>sericeUri</code>
     * set to null is not a valid configuration for server transports.
     * <p/>
     * When called with <code>type</code> set to {@link P2PManager.Mode#CLIENT},
     * a connection-oriented client transport will be created.
     * <p/>
     * If non-zero, the parameter <code>SAP</code> contains the SAP value on which
     * the client will try to connect (2-63). In this case, the parameter
     * <code>sericeUri</code> must be null.
     * <p/>
     * If the parameter <code>serviceUri</code> is non null, it contains the service
     * name on which the client will try to connect. In this case, the parameter
     * <code>SAP</code> value must be zero.
     * <p/>
     * The combination of <code>SAP</code> set to a value different from zero and
     * <code>sericeUri</code> set to a non-null value is not a valid configuration
     * for connection oriented client transport.
     * <p/>
     * When called with <code>type</code> set to {@link P2PManager.Mode#CLIENT_SERVER},
     * a connection-oriented client/server will be created.
     * <p/>
     * If non-zero, the parameter <code>SAP</code> contains the SAP value on which
     * the client/server will listen. If <code>SAP</code> is set to zero, a free
     * SAP will be allocated to the connection by the P2P stack. If the parameter
     * <code>sericeUri</code> is null, the parameter <code>SAP</code> value is
     * also be used to establish a connection with the peer. Otherwise, the
     * <code>sericeUri</code> parameter will be used to perform the connection.
     * <p/>
     * If not null, the <code>sericeUri</code> contains the service name associated
     * to the socket. This service name will be registered into the SDP implemented
     * in the P2P stack. This service name will also be used to connect to the
     * remote service.
     * <p/>
     *
     * @param mode the requested mode for the connection:
     *            <ul>
     *            <li>{@link P2PManager.Mode#CLIENT Mode.CLIENT} for a client
     *            connection,</li>
     *            <li>{@link P2PManager.Mode#SERVER Mode.SERVER} for a server
     *            connection, or</li>
     *            <li>{@link P2PManager.Mode#CLIENT_SERVER Mode.CLIENT_SERVER}
     *            for aclient or server connection.</li>
     *            </ul>.
     * @param serviceUri the service URI to connect to.
     * @param sap The Service Access Point. Valid values are 0, and any value in
     *              the range 2-63.
     *
     * @return A new P2P connection instance.
     *
     * @throws IllegalStateException if the specified <code>sap</code> or
     *              <code>sericeUri</code> value is already in use.
     * @throws NullPointerException if <code>mode</code> is null.
     * @throws IllegalArgumentException if the combination of <code>SAP</code> and
     *              <code>sericeUri</code> is not a valid configuration.
     * @throws NfcException if NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public P2PConnection createP2PConnection(Mode mode, String serviceUri, byte sap)
            throws NfcException;

    /**
     * Creates a new P2P connectionless object.
     * <p/>
     * Connectionless transport do not require any connect operation prior being
     * able to send/receive data.
     * <p/>
     * If non-zero, the <code>SAP</code> parameter contains the SAP value on which
     * the connectionless will be bound. If set to zero, a free SAP will be
     * allocated to the connectionless by the P2P stack.
     * <p/>
     * If non null, <code>sericeUri</code> contains the service the service name
     * associated to the connectionless object. This service name will be registered
     * into the SDP implemented into the stack, allowing peer device to perform a
     * name lookup (LLCP 1.1 specific feature)
     * <p/>
     * The combination of <code>SAP</code> set to zero and <code>sericeUri</code>
     * set to null is not a valid configuration for connectionless transports.
     * <p/>
     *
     * @param serviceUri the service URI to connect to.
     * @param sap The Service Access Point. Valid values are 0, and any value in
     *              the range 2-63.
     *
     * @return A new P2P connectionless instance.
     *
     * @throws IllegalStateException if the specified <code>SAP</code> or
     *              <code>sericeUri</code> value is already in use.
     * @throws IllegalArgumentException if the combination of <code>SAP</code>
     *              and <code>sericeUri</code> is not a valid configuration.
     * @throws NfcException if NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public P2PConnectionLess createP2PConnectionLess(String serviceUri, byte sap)
            throws NfcException;

    /**
     * This class provides the Peer to Peer modes.
     *
     * @since Open NFC 4.1
     */
    enum Mode {
        /**
         * Type of P2P connection oriented socket: client.
         *
         * @since Open NFC 4.1
         */
        CLIENT(ConstantAutogen.W_P2P_TYPE_CLIENT),
        /**
         * Type of P2P connection oriented socket: server.
         *
         * @since Open NFC 4.1
         */
        SERVER(ConstantAutogen.W_P2P_TYPE_SERVER),
        /**
         * Type of P2P connection oriented socket: client/server.
         *
         * @since Open NFC 4.1
         */
        CLIENT_SERVER(ConstantAutogen.W_P2P_TYPE_CLIENT_SERVER);

        /* package protected */int identifier;

        /** private constructor */
        private Mode(int identifier) {
            this.identifier = identifier;
        }

        /**
         * Returns the type value.
         *
         * @return the type value.
         *
         * @hide
         **/
        public int getValue() {
            return this.identifier;
        }
    }
}
