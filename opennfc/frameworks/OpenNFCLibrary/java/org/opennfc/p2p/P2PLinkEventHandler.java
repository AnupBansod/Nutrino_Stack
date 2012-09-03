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

/**
 * Interface definition for a callback to be invoked each time a P2P Link is
 * established or an error occurred or when the connection is broken. These
 * callbacks methods are called if the P2PLinkEventHandler has been registered
 * with the P2P Link Manager using the {@link P2PManager#establishLink establishLink()}
 * method.
 *
 * @since Open NFC 4.1
 */
public interface P2PLinkEventHandler {

    /**
     * Method called when a new P2P link is established.
     *
     * @param link the P2P link.
     *
     * @since Open NFC 4.1
     */
    public void onLinkDetected(P2PLink link);

    /**
     * Method called when an error or a call to
     * {@link P2PManager#cancelLink cancelLink()} function occurs
     * during a P2P establishment procedure.
     *
     * @since Open NFC 4.1
     */
    public void onLinkError();

    /**
     * Method called when the link is broken.
     *
     * @since Open NFC 4.1
     */
    public void onLinkReleased();

}
