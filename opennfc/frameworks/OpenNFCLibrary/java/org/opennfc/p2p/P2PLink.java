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
 * This interface exposes the methods to retrieves the parameters values of the
 * P2P protocol negotiated during the link establishment.
 *
 * <p>These functions can be used only when a P2P link is established.
 *
 * @since Open NFC 4.1
 */
public interface P2PLink {

    /**
     * Retrieves the version of the LLC Protocol agreed between both devices.
     *
     * @return The version of the LLC Protocol.
     *
     * @throws IOException if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public byte getAgreeLlcpVersion() throws IOException;

    /**
     * Retrieves the remote LLC Protocol version.
     *
     * @return The remote LLC Protocol version.
     *
     * @throws IOException if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public byte getRemoteLlcpVersion() throws IOException;

    /**
     * Retrieves the remote Maximum Information Unit : maximum information field
     * size that the peer is able to receive. This value is truncated to the
     * maximum information field size we are able to send.
     *
     * @return The remote Maximum Information Unit.
     *
     * @throws IOException if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public int getRemoteMiu() throws IOException;

    /**
     * Retrieves the remote link timeout, expressed in ms.
     *
     * @return The remote link timeout.
     *
     * @throws IOException if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public int getRemoteLto() throws IOException;

    /**
     * Retrieves the remote Well-Known service list. Each bit set to 1 indicates
     * the corresponding Well-Known service is available on the peer device.
     *
     * @return The remote Well-Known service list.
     *
     * @throws IOException if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public int getRemoteWks() throws IOException;

    /**
     * Retrieves the link speed, in bits per seconds.
     *
     * @return The link speed.
     *
     * @throws IOException if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public int getBaudeRate() throws IOException;

    /**
     * Informs if the device is P2P initiator.
     *
     * @return True, if the device is the P2P initiator.
     *
     * @throws IOException if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public boolean isInitiator() throws IOException;

    /**
     * Retrieves on which SAP the specified <code>serviceUri</code> is running
     * on the peer device. The SAP is only valid during the duration of
     * establishment of the P2P link. If the P2P link is broken, the association
     * is no longer valid and a new URI lookup must be done.
     *
     * <p> This method <b>block</b> until the peer device respond, or an
     * exception is thrown.
     *
     * @param serviceUri The service URI. This value shall not be null and should
     *      contain a well-formed URI.
     *
     * @return The Destination SAP value associated to the URI. If no SAP is
     *      found a zero value is returned.
     *
     * @throws IllegalArgumentException if <code>serviceUri</code> is null.
     * @throws IllegalStateException if other lookup operation is already in progress.
     * @throws IOException if the link is broken.
     * @throws NfcException if a NFC error occurred.
     *
     * @since Open NFC 4.1
     */
    public byte UriLookup(String serviceUri) throws IOException, NfcException;

    /**
     * Closes a pending link.
     *
     * @throws IOException  if the link is broken.
     *
     * @since Open NFC 4.1
     */
    public void close() throws IOException;
}
