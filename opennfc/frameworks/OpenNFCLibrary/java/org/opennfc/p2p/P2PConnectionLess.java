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
 * This interface exposes the methods for the P2P connectionless. Use
 * {@link P2PManager#createP2PConnectionLess} to create a new instance of P2P
 * connectionless.
 *
 * @since Open NFC 4.1
 */
public interface P2PConnectionLess {

    /**
     * Retrieves the local Service Access Point value.
     *
     * @throws IOException if the connection is not connected.
     * @throws NfcException if a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public byte getLocalSap() throws IOException, NfcException;

    /**
     * Sends <code>data.length</code> bytes from the specified byte array through
     * a connectionless transport. The general contract for <code>sendTo(data)</code>
     * is that it should have exactly the same effect as the call
     * <code>sendTo(data, 0, data.length)</code>.
     *
     * @param sap the remote SAP value. Valid range is 2..63.
     * @param data the buffer containing the data to be sent.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             write operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null.
     * @throws IOException if an error occurs during the communication with the
     *             peer device, or if {@link #cancel()} is called before the
     *             write operation is completed, or if the peer device closed
     *             the connection before receiving the data.
     *
     * @since Open NFC 4.1
     */
    public void sendTo(byte sap, byte[] data) throws IOException;

    /**
     * Sends <code>len</code> bytes from the specified byte array
     * starting at offset <code>off</code> through a connectionless transport.
     * <p>
     * If <code>data</code> is <code>null</code>, a
     * <code>NullPointerException</code> is thrown.
     * <p>
     * If <code>off</code> is negative, or <code>len</code> is negative, or
     * <code>off+len</code> is greater than the length of the array
     * <code>b</code>, then an <tt>IndexOutOfBoundsException</tt> is thrown.
     *
     * @param sap the remote SAP value. Valid range is 2..63.
     * @param data the buffer containing the data to be sent.
     * @param off the start offset in byte of the data in the buffer.
     * @param len the number of bytes to write.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             write operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null
     *             or if offset or length are negative.
     * @throws IndexOutOfBoundsException if the <code>off</code> or
     *             <code>len</code> are outside of the data buffer.
     * @throws IOException if an error occurs during the communication with the
     *             peer device, or if {@link #cancel()} is called before the
     *             write operation is completed, or if the peer device closed
     *             the connection before receiving the data.
     *
     * @since Open NFC 4.1
     */
    public void sendTo(byte sap, byte[] data, int off, int len) throws IOException;

    /**
     * Reads some number of bytes from the P2P connectionless and stores them into
     * the buffer array <code>data</code>. The number of bytes actually read is
     * returned as an integer.
     *
     * <p>The remote Service Access Point (SAP) is be stored in a byte at the end
     * of the data buffer array. The length returned by recvFrom() does not include
     * this byte.
     *
     * <p>This method <b>blocks</b> until input data is available, or an exception
     * is thrown.
     *
     * @return the total number of bytes read into the buffer, or
     *             <code>-1</code> if there is no more data.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             read operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null.
     * @throws IOException if an error occurs during the communication with the
     *             peer device or if {@link #cancel()} is called before the read
     *             operation is completed.
     *
     * @since Open NFC 4.1
     */
    public int recvFrom(byte[] data) throws IOException;

    /**
     * Reads up to <code>len</code> bytes of data from the P2P connection
     * into an array of bytes.  An attempt is made to read as many as
     * <code>len</code> bytes, but a smaller number may be read, possibly
     * zero. The number of bytes actually read is returned as an integer.
     *
     * <p>The remote Service Access Point (SAP) is be stored in a byte at the end
     * of the data buffer array. The length returned by recvFrom() does not include
     * this byte.
     *
     * <p>This method <b>blocks</b> until input data is available, or an exception
     * is thrown.
     *
     * <p> If <code>data</code> is <code>null</code>, a
     * <code>NullPointerException</code> is thrown.
     *
     * <p> If <code>off</code> is negative, or <code>len</code> is negative, or
     * <code>off+len</code> is greater than the length of the array
     * <code>b</code>, then an <code>IndexOutOfBoundsException</code> is
     * thrown.
     *
     * <p> If <code>len</code> is zero, then no bytes are read and
     * <code>0</code> is returned; otherwise, there is an attempt to read at
     * least one byte. If no byte is available , the value <code>-1</code> is returned;
     * otherwise, at least one byte is read and stored into <code>data</code>.
     *
     * @return the total number of bytes read into the buffer, or
     *             <code>-1</code> if there is no more data.
     *
     * @throws IllegalStateException if the connection is not open or if another
     *             read operation is pending.
     * @throws IllegalArgumentException if the <code>data</code> buffer is null
     *             or if offset or length are negative.
     * @throws IndexOutOfBoundsException if the <code>off</code> or
     *             <code>len</code> are outside of the data buffer.
     * @throws IOException if an error occurs during the communication with the
     *             peer device or if {@link #cancel()} is called before the read
     *             operation is completed.
     *
     * @since Open NFC 4.1
     */
    public int recvFrom(byte[] data, int off, int len) throws IOException;

    /**
     * Determines if there is a data that can be receive via a call to {@link #recvFrom recvFrom()}.
     * If true, a call to {@link #recvFrom recvFrom()} will not block the application.
     *
     * @return true if there is data to read; false if there is no data to read
     *
     * @throws IOException if connection to the target is lost or it has been closed.
     *
     * @since Open NFC 4.1
     */
    public boolean ready() throws IOException;

    /**
     * Cancels every pending operation.
     *
     * @since Open NFC 4.1
     */
    public void cancel();

    /**
     * Closes a pending connectionLess.
     *
     * @throws IOException if the connectionLess is broken.
     *
     * @since Open NFC 4.1
     */
    public void close() throws IOException;
}
