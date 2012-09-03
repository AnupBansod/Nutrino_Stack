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

import java.io.IOException;

import org.opennfc.NfcException;

/**
 * This interface exposes the methods for the Peer to Peer communication (P2P).
 * Use {@link P2PManager#createP2PConnection} to create an new instance of P2P
 * connection.
 *
 * @since Open NFC 4.1
 */
public interface P2PConnection {

    /**
     * Retrieves the value of a connection parameter.
     *
     * @param parameter The parameter to be retrieved.
     *
     * @return The parameter value.
     *
     * @throws IOException if the connection is not established.
     * @throws NfcException if other error occurred.
     */
    public int getParameter(P2PConnectionParameter parameter) throws IOException, NfcException;

    /**
     * Sets the value of a connection parameter.
     *
     * @param value The value of the parameter to be set.
     * @param parameter The value of the parameter to be set <b>must</b> be one of
     *                  the following values:
     *            <ul>
     *            <li>{@link P2PConnectionParameter#LOCAL_MIU} is sent during the connection establishment, </li>
     *            <li>{@link P2PConnectionParameter#LOCAL_RW} is sent during the connection establishment.</li>
     *            </ul>
     *
     * @throws IOException if the connection is not established.
     * @throws IllegalArgumentException If the parameter is not valid.
     * @throws NfcException if a NFC error occurred.
     */
    public void setParameter(P2PConnectionParameter parameter, int value) throws IOException,
            NfcException;

    /**
     * Requests a connection establishment.
     * <p>
     * This method must be called prior to be able to send/receive data on the
     * established P2P connection.
     * <p>
     * A P2P connection remains established as long as the specified P2P link
     * remains active. When the P2P link is destroyed, the connection is
     * automatically closed. A new call to connect() must be done once the P2P
     * link has been reestablished.
     * <p>
     * Shutdown of an established connection can be done using the
     * {@link #shutdown shutdown()} method.
     * Once the connection has been closed, it can be reestablished using
     * connect(). Several connection/disconnection cycles can be performed
     * during the same P2P link establishment.
     * <p>
     * This method is <b>blocking</b> until the connection has been established
     * or in case of error. If needed, the connect operation can be canceled
     * using {@link #shutdown shutdown()} method.
     *
     * @param link The established link.
     *
     * @throws IllegalArgumentException if <code>link</code> is null.
     * @throws IOException if connection to the target is lost or it has been closed.
     * @throws IllegalStateException if the connection is already established or
     *          the P2P link is no longer established.
     * @throws NfcException if a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public void connect(P2PLink link) throws IOException, NfcException;

    /**
     * Sends data to the peer device. If data is of length 0, an empty frame will
     * be sent. The method <b>block</b> until the full buffer content is sent.
     * The general contract for <code>write(data)</code> is that it should have
     * exactly the same effect as the call <code>write(data, 0, data.length)</code>.
     *
     * @param data the buffer containing the data to be sent.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             write operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null.
     * @throws IOException if an error occurs during the communication with the
     *             peer device, or if {@link #shutdown()} is called before the
     *             write operation is completed, or if the peer device closed
     *             the connection before receiving the data.
     *
     * @since Open NFC 4.1
     */
    public void write(byte[] data) throws IOException;

    /**
     * Sends data to the peer device. If data is of length 0, an empty I frame will
     * be sent. The method <b>block</b> until the full buffer content is sent.
     *
     * @param data the buffer containing the data to be sent.
     * @param off the start offset in byte of the data in the buffer.
     * @param len the number of bytes to write.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             write operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null
     *             or if offset or length are negative.
     * @throws IndexOutOfBoundsException if the <code>offset</code> or
     *             <code>length</code> are outside of the data buffer.
     * @throws IOException if an error occurs during the communication with the
     *             peer device, or if {@link #shutdown()} is called before the
     *             write operation is completed, or if the peer device closed
     *             the connection before receiving the data.
     *
     * @since Open NFC 4.1
     */
    public void write(byte[] data, int off, int len) throws IOException;

    /**
     * Reads some number of bytes from the P2P connection and stores them into
     * the buffer array <code>data</code>. The amount of data received in this
     * operation is related to the value of the LocalMIU. The number of bytes
     * actually read is returned as an integer.
     *
     * <p> This method <b>block</b> until input data is available, or an
     * exception is thrown.
     *
     * @param data the buffer to store the received data.
     *
     * @return the actual number of bytes read; 0 if data length is zero.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             read operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null.
     * @throws IOException if an error occurs during the communication with the
     *             peer device or if {@link #shutdown()} or {@link #cancel()} is
     *             called before the read operation is completed.
     *
     * @since Open NFC 4.1
     */
    public int read(byte[] data) throws IOException;

    /**
     * Reads up to <code>len</code> bytes of data from the P2P connection
     * into an array of bytes. The amount of data received in this
     * operation is related to the value of the LocalMIU. An attempt is made to
     * read as many as <code>len</code> bytes, but a smaller number may be read,
     * possibly zero. The number of bytes actually read is returned as an integer.
     *
     * <p> This method <b>block</b> until input data is available, or an
     * exception is thrown.
     *
     * <p> If <code>len</code> is zero, then no bytes are read and
     * <code>0</code> is returned; otherwise, there is an attempt to read at
     * least one byte. If no byte is available , the value <code>-1</code> is returned;
     * otherwise, at least one byte is read and stored into <code>data</code>.
     *
     * @param data the buffer to store the received data.
     * @param off the start offset in byte of the data in the buffer.
     * @param len the number of bytes to read.
     *
     * @return the actual number of bytes read; 0 if a zero length frame is
     *          received; 0 if data length is zero.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             read operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null
     *             or if <code>off</code>  or <code>len</code> are negative.
     * @throws IndexOutOfBoundsException if the <code>offset</code> or length are
     *             outside of the data buffer.
     * @throws IOException if an error occurs during the communication with the
     *             peer device or if {@link #shutdown()} or {@link #cancel()} is
     *             called before the read operation is completed.
     *
     * @since Open NFC 4.1
     */
    public int read(byte[] data, int off, int len) throws IOException;

    /**
     * Cancels every pending operation.
     *
     * @since Open NFC 4.1
     */
    public void cancel();

    /**
     * Shutdowns the connection. shutdown() tries to perform a graceful shutdown,
     * meaning it ensures all pending data have been sent before closing the
     * connection.
     * <p/>
     * If the peer does not acknowledge pending data in a timely manner, the
     * connection will be closed even if there is remaining data to send.
     * <p/>
     * This is the safe manner to request termination of a P2P connection.
     *
     * @throws IOException if the connection is not established.
     * @throws NfcException if a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public void shutdown() throws IOException, NfcException;

    /**
     * Closes a pending connection.
     *
     * @throws IOException if the connection is broken.
     *
     * @since Open NFC 4.1
     */
    public void close() throws IOException;

    /**
     * <p>This class collects the P2P connection parameters.
     *
     * @since Open NFC 4.1
     */
    public enum P2PConnectionParameter {

        /**
         * Local MIU for connection oriented.
         *
         * <p>This parameter is sent during the establishment of the connection. It
         * allows to specify the maximum packet size in bytes we are able to receive.
         *
         * <p>Must be in the range [128-256]
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()} and set
         * using {@link P2PConnection#setParameter setParameter()}.
         *
         * <p>If the connection is already established, setting the value will only apply
         * on next connection.
         *
         * @since Open NFC 4.1
         */
        LOCAL_MIU(ConstantAutogen.W_P2P_LOCAL_MIU),

        /**
         * Local Receive Window for connection oriented.
         *
         * <p>This parameter is sent during the establishment of the connection.
         *
         * <p>Must be in the range [0-15]
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()} and set
         * using {@link P2PConnection#setParameter setParameter()}.
         *
         * <p>If the connection is already established, setting the value will only apply
         * on next connection.
         *
         * @since Open NFC 4.1
         */
        LOCAL_RW(ConstantAutogen.W_P2P_LOCAL_RW),

        /**
         * Connection oriented status.
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()}.
         *
         * <p>If not zero, the connection is established. If set to zero, the connection
         * is not established.
         *
         * @since Open NFC 4.1
         */
        CONNECTION_ESTABLISHED(ConstantAutogen.W_P2P_CONNECTION_ESTABLISHED),

        /**
         * Data availability status.
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()}.
         *
         * <p>If not zero, some data can be received on the connection.
         *
         * @since Open NFC 4.1
         */
        DATA_AVAILABLE(ConstantAutogen.W_P2P_DATA_AVAILABLE),

        /**
         * Local SAP associated to the connection.
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()}.
         *
         * @since Open NFC 4.1
         */
        LOCAL_SAP(ConstantAutogen.W_P2P_LOCAL_SAP),

        /**
         * Remote SAP associated to a established connection-oriented.
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()}.
         *
         * <p>This parameter indicates the maximum frame size we are able to send to
         * the peer.
         *
         * @since Open NFC 4.1
         */
        REMOTE_SAP(ConstantAutogen.W_P2P_REMOTE_SAP),

        /**
         * Remote MIU associated to a established connection-oriented.
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()}.
         *
         * <p>This parameter indicates the maximum packet size we are able to send to
         * the peer.
         *
         * @since Open NFC 4.1
         */
        REMOTE_MIU(ConstantAutogen.W_P2P_REMOTE_MIU),

        /**
         * Remote RW associated to a established connection-oriented.
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()}.
         *
         * <p>This parameter indicates the maximum number of I PDU that can be sent
         * to the peer without receiving acknowledge.
         *
         * @since Open NFC 4.1
         */
        REMOTE_RW(ConstantAutogen.W_P2P_REMOTE_RW),

        /**
         * If true, the socket is server, if false, the socket is client.
         *
         * <p>The current parameter value can be retrieved using
         * {@link P2PConnection#getParameter(P2PConnectionParameter) getParameter()}.
         *
         * <p>This parameter allows to know how a client/server socket has been established.
         *
         * @since Open NFC 4.1
         */
        IS_SERVER(ConstantAutogen.W_P2P_IS_SERVER);

        /**
         * The identifier of the property
         *
         * @hide
         */
        private int identifier;

        /**
         * private constructor to prevent instantiation.
         *
         * @hide
         */
        private P2PConnectionParameter(int identifier) {
            this.identifier = identifier;
        }

        /**
         * Returns the property value.
         *
         * @return the property value.
         *
         * @hide
         */
        public int getValue() {
            return this.identifier;
        }
    }
}
