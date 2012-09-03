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

package org.opennfc.nfctag;

import org.opennfc.NfcException;

/**
 * The Virtual Tag is a feature used to simulate a NFC tag with the local device.
 * <p/>
 * The following figure depicts the life-cycle of a virtual tag:<br/>
 * <br/><img src="./doc-files/VirtualTag-1.gif" width=70% height=70%/><br/>
 * <p/>
 * To create an instance of Virtual Tag, use {@link NfcTagManager#createVirtualTag NfcTagManager.createVirtualTag()}.
 * <p/>
 * The virtual tag is initially empty.
 * The application may fill the tag with the method {@link #writeMessage writeMessage()}.
 * The method {@link #readMessage readMessage()} is used by the application to read the content of the tag.
 * <p/>
 * The application calls the method {@link #start start()} to start the tag simulation.
 * Only one virtual tag of a given type can be simulated at the same time.
 * If several applications try to simultaneously simulate the same tag type,
 * the first application starting the simulation has the exclusive use of the service.
 * The other applications are rejected.
 * <strong>When the simulation is active, the application cannot read or write the content of the tag.</strong>
 * The application is notified of the following events occuring during the simulation:
 * <ul>
 *   <li>A remote reader selects the virtual tag.</li>
 *   <li>A remote reader stops communicating with the tag without reading or writing the tag content.</li>
 *   <li>A remote reader stops communicating with the tag after reading the tag content.</li>
 *   <li>A remote reader stops communicating with the tag after writing the tag content.</li>
 * </ul>
 * The application may subscribe to these events to decide to stop the tag simulation.
 * <p/>
 * During the simulation, a remote reader may read or write the tag. The write operations of a remote reader
 * are allowed only if <code>isReadOnly</code> is set to false when the simulation is started.
 * Several remote readers may read or write the tag during one simulation.
 * <p/>
 * The simulation is stopped with the {@link #stop} method. Then the virtual tag is no longer visible from the remote readers.
 * After stopping the tag, the application may read new the content of the virtual tag using the method {@link #readMessage readMessage()}.
 * The application may also write a new content or restart the simulation.
 *
 * @since Open NFC 4.0
 */
public interface VirtualTag extends NfcTagConnection {

    /**
     * Starts the simulation of the virtual tag.
     *
     * @param handler an instance of {@link VirtualTagEventHandler} whose
     *    {@link VirtualTagEventHandler#onTagEventDetected onTagEventDetected()}
     *    method will be called to notify the virtual tag events.
     * @param isReadOnly set to true to set the virtual tag in read only for the remote device.
     *        If this value is false, the remote device can write in the virtual tag.
     *
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalStateException if the simulation is already started.
     * @throws NfcException if the card emulation for this type is already used.
     */
    void start(VirtualTagEventHandler handler, boolean isReadOnly) throws NfcException;

    /**
     * Stops the simulation of the virtual tag.
     *
     * If the simulation is already stopped, this method does nothing.
     */
    void stop();
}
