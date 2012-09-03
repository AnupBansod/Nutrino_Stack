/*
 * Copyright (c) 2010 Inside Secure, All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.opennfc.service.communication;

import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.util.Log;
import android.util.SparseArray;

/**
 * {@link ClientServer Client/server} cache.<br>
 * It stores {@link ClientServer client/server} instances
 * 
 * @hide
 */
public class ClientServerCache {
    /** Cache single instance */
    public final static ClientServerCache CACHE;
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = ClientServerCache.class.getSimpleName();
    static {
        CACHE = new ClientServerCache();
    }

    /** The cache itself */
    private final SparseArray<ClientServer> clientServerCache;
    /** Next ID to give */
    private int nextID;

    /**
     * Create the empty cache
     */
    private ClientServerCache() {
        this.clientServerCache = new SparseArray<ClientServer>(16);
        this.nextID = 1;
    }

    /**
     * Create a {@link ClientServer client/server} instance.
     * 
     * @param name Connection URI
     * @param sap SAP to use
     * @param p2pMode Indicates if we want use P2P mode ({@code true}) or manual
     *            communication ({@code false})
     * @return Handle on the created {@link ClientServer client/server} to able
     *         manipulate it later
     */
    public int createClientServer(final String name, final int sap, final boolean p2pMode) {
        if (ClientServerCache.DEBUG) {
            Log.d(ClientServerCache.TAG, "createClientServer name=" + name + " sap=" + sap);
        }

        int newID = -1;

        synchronized (this) {
            newID = this.nextID++;
        }

        final ClientServer clientServer = new ClientServer(newID, name, sap, p2pMode);
        this.clientServerCache.put(newID, clientServer);

        return newID;
    }

    /**
     * Obtain a {@link ClientServer client/server} for a specific handle
     * 
     * @param handle {@link ClientServer Client/server} handle
     * @return The {@link ClientServer client/server} associate to the handle
     */
    public ClientServer get(final int handle) {
        return this.clientServerCache.get(handle);
    }

    /**
     * Obtain a client server that match with a specified name and sap.<br>
     * The name can be {@code null} to indicates any URI.<br>
     * The SAP can be 0 to say any SAP.<br>
     * But if name is {@code null} and sap 0 in same time, then no client server
     * will match
     * 
     * @param name URI to connect with (May be {@code null}, if SAP > 0)
     * @param sap SAP to connect (May be 0, if name not {@code null})
     * @return Found client server or {@code null} if not found
     */
    ClientServer obtain(final String name, final int sap) {
        final int length = this.clientServerCache.size();
        ClientServer clientServer;

        for (int i = 0; i < length; i++) {
            clientServer = this.clientServerCache.get(this.clientServerCache.keyAt(i));

            if (clientServer.itsMe(name, sap) == true) {

                if (ClientServerCache.DEBUG) {
                    Log.d(ClientServerCache.TAG, "Found client server !");
                }

                return clientServer;
            }
        }

        return null;
    }

    /**
     * Post a message for a specific {@link ClientServer client/server} (In P2P
     * mode)
     * 
     * @param name URI to contact
     * @param sap SAP to use
     * @param ndefMessage Message to send
     */
    public void post(final String name, final int sap, final NdefMessage ndefMessage) {
        if (ClientServerCache.DEBUG) {
            Log.d(ClientServerCache.TAG, "post");
        }

        ClientServer clientServer = null;
        final int length = this.clientServerCache.size();
        ClientServer cs;

        for (int i = 0; i < length; i++) {
            cs = this.clientServerCache.get(this.clientServerCache.keyAt(i));

            if (cs.itsMe(name, sap) == true) {
                clientServer = cs;

                if (ClientServerCache.DEBUG) {
                    Log.d(ClientServerCache.TAG, "Found client server !");
                }

                break;
            }
        }

        if (ClientServerCache.DEBUG) {
            Log.d(ClientServerCache.TAG, "clientServer=" + clientServer);
        }

        if (clientServer != null) {
            for (final NdefRecord ndefRecord : ndefMessage.getRecords()) {
                if (ClientServerCache.DEBUG) {
                    Log.d(ClientServerCache.TAG, "Enqueing records ...");
                }

                clientServer.inQueue(ndefRecord.getPayload());
            }
        }
    }

    /**
     * Remove a {@link ClientServer client/server}
     * 
     * @param clientServer {@link ClientServer client/server} to remove
     */
    public void remove(final ClientServer clientServer) {
        this.remove(clientServer.getID());
    }

    /**
     * Remove a {@link ClientServer client/server}
     * 
     * @param handle Handle of the {@link ClientServer client/server} to remove
     */
    public void remove(final int handle) {
        final ClientServer clientServer = this.clientServerCache.get(handle);

        if (clientServer != null) {
            this.clientServerCache.remove(handle);
            clientServer.disconnect();
        }
    }
}
