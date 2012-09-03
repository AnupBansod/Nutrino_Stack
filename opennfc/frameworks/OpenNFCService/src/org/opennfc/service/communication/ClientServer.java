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

import org.opennfc.HelperForNfc;
import org.opennfc.NfcException;
import org.opennfc.NfcManager;
import org.opennfc.p2p.P2PConnection;
import org.opennfc.p2p.P2PConnection.P2PConnectionParameter;
import org.opennfc.p2p.P2PLink;
import org.opennfc.p2p.P2PLinkEventHandler;
import org.opennfc.p2p.P2PManager;
import org.opennfc.service.OpenNFCBackgoundService;
import org.opennfc.service.OpenNFCService;
import org.opennfc.service.ThreadedTask;
import org.opennfc.service.ThreadedTaskManager;

import android.nfc.ErrorCodes;
import android.util.Log;

import java.io.IOException;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Do client/server communication.<br>
 * It plays the role of client or server depends on witch hardware initiate the
 * communication.<br>
 * For exchange data with other phone, the steps are :
 * <ol>
 * <li>Create a socket (Also called connection)</li>
 * <li>Establish a link with other phone. In other words, it wait until it see
 * other phone, a disconnection occurs or an exception append</li>
 * <li>Plug the connection to the link</li>
 * <li>Communicate with other board. Automatically in P2P mode; manually on
 * called {@link #send(byte[])} and/or {@link #receive(byte[])} method.</li>
 * <li>At the end, disconnect the connection</li>
 * </ol>
 * 
 * @hide
 */
public class ClientServer implements P2PLinkEventHandler, ThreadedTask<Integer> {
    /** Status indicates to associate the connection with the link */
    public static final int CONNECT_WITH_LINK = 3;
    /** Status indicates to create the connection */
    public static final int CREATE_CONNECTION = 1;
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Status indicates to disconnect the client/server */
    public static final int DISCONNECT = 0;
    /** Status indicates to do exchange with other phone */
    public static final int DO_EXCHANGE = 4;
    /** Status indicates to establish a link */
    public static final int ESTABLISH_LINK = 2;
    /** P2P manager instance */
    private static P2PManager p2pManager;
    /** Tag use in debug */
    private static final String TAG = ClientServer.class.getSimpleName();

    /**
     * Obtain the P2P manager instance
     * 
     * @return P2P manager instance
     */
    private static P2PManager getP2PManager() {
        if (ClientServer.p2pManager == null) {
            try {
                ClientServer.p2pManager = NfcManager.getInstance().getP2PManager();
            } catch (final NfcException exception) {
                if (ClientServer.DEBUG) {
                    Log.d(ClientServer.TAG, "Can't have connection to P2P manager", exception);
                }
            }
        }

        return ClientServer.p2pManager;
    }

    /**
     * Inject P2P manager for be able test the P2P
     * 
     * @param p2pManager P2P to inject. {@code null} to back to normal
     */
    public static void inject(final P2PManager p2pManager) {
        final ClientServer clientServer = ClientServerCache.CACHE.obtain(
                OpenNFCService.SERVER_COM_ANDROID_NPP_NAME, OpenNFCService.SERVER_SAP);

        if (clientServer != null) {
            clientServer.disconnect();

            HelperForNfc.sleep(256);
        }

        ClientServer.p2pManager = p2pManager;

        if (clientServer != null) {
            clientServer.connect();
        }
    }

    /** Client/server ID */
    private final int id;
    /** Actual link client/server is attach */
    private P2PLink link;
    /** Use for synchronized the P2P automatic exchange */
    public final Object LOCK = new Object();
    /** Messages queue for store messages to send in P2P mode */
    private final ConcurrentLinkedQueue<byte[]> messages;
    /** Socket/connection created */
    private P2PConnection mP2pConnection;
    /** URI to connect to */
    private String name;
    /** Indicates if it is P2P mode */
    private final boolean p2pMode;
    /** SAP to connect */
    private int sap;
    /** Actual doing task */
    private int status;

    /**
     * Create a client/server
     * 
     * @param id Client/server id
     * @param name URI to connect
     * @param sap SAP to use
     * @param p2pMode Enable/disable the P2P mode
     */
    ClientServer(final int id, final String name, final int sap, final boolean p2pMode) {
        this.id = id;
        this.sap = sap;
        this.name = name;
        this.p2pMode = p2pMode;
        this.messages = new ConcurrentLinkedQueue<byte[]>();
        this.status = ClientServer.DISCONNECT;
    }

    /**
     * Do nothing here <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.service.ThreadedTask#cancel()
     */
    @Override
    public void cancel() {
    }

    /**
     * Connect (if need) the client/server with its actual settings
     */
    public void connect() {
        if (this.status == ClientServer.DISCONNECT) {
            ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.CREATE_CONNECTION);
        }
    }

    /**
     * Disconnect (if need) the client/server
     */
    public void disconnect() {
        if (this.status != ClientServer.DISCONNECT) {
            ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.DISCONNECT);
        }
    }

    /**
     * Do tasks in separated thread.<br>
     * Tasks can be done : {@link #CONNECT_WITH_LINK},
     * {@link #CREATE_CONNECTION}, {@link #DISCONNECT}, {@link #DO_EXCHANGE} or
     * {@link #ESTABLISH_LINK} <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param taskID Thread ID (Unused)
     * @param parameters Take care of first parameter that indicates the task to
     *            do
     * @see org.opennfc.service.ThreadedTask#excuteTask(int, PARAMETER[])
     */
    @Override
    public void excuteTask(final int taskID, final Integer... parameters) {
        this.status = parameters[0];

        synchronized (this.LOCK) {
            this.LOCK.notifyAll();
        }

        if (ClientServer.DEBUG) {
            Log.d(ClientServer.TAG, "excuteTask : " + this.status);
        }

        switch (this.status) {
            case CREATE_CONNECTION:
                try {
                    this.mP2pConnection = ClientServer.getP2PManager().createP2PConnection(
                                P2PManager.Mode.CLIENT_SERVER, this.name, (byte) (this.sap & 0xFF));

                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "Connection created : " + this.mP2pConnection);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                            ClientServer.ESTABLISH_LINK);

                } catch (final NfcException exception) {
                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "Create connection failed ! ", exception);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.DISCONNECT);
                }
                break;

            case ESTABLISH_LINK:
                try {
                    ClientServer.getP2PManager().establishLink(this);

                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "establishLink launched !");
                    }
                } catch (final NfcException exception) {
                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "establishLink failed", exception);
                    }

                    switch (exception.getCode()) {
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.DISCONNECT);
                }

                break;

            case CONNECT_WITH_LINK:
                try {
                    this.mP2pConnection.connect(this.link);

                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "Connect to link succed !");
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.DO_EXCHANGE);
                } catch (final IOException exception) {
                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "Connection link I/O try to create a new link",
                                exception);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                            ClientServer.ESTABLISH_LINK);
                } catch (final NfcException exception) {
                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "Connect to link failed !", exception);
                    }

                    switch (exception.getCode()) {
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.DISCONNECT);
                }
                break;

            case DO_EXCHANGE:
                if (ClientServer.DEBUG) {
                    Log.d(ClientServer.TAG, "Will do excehange : " + this.p2pMode);
                }

                if (this.p2pMode == false) {
                    return;
                }

                try {
                    final byte[] temp = new byte[512];
                    int read;

                    // We only know now if it is a server or a client (Can
                    // change between 2 launch, because it is a hardware
                    // decision [first win])
                    boolean isServer = true;

                    try {
                        isServer = this.mP2pConnection
                                .getParameter(P2PConnectionParameter.IS_SERVER) == 1;
                    } catch (final NfcException exception) {
                        if (ClientServer.DEBUG) {
                            Log.d(ClientServer.TAG, "Is server failed ! ", exception);
                        }

                        ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                                ClientServer.ESTABLISH_LINK);
                        return;
                    }

                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "isServer=" + isServer);
                    }

                    byte[] message;

                    while (this.status == ClientServer.DO_EXCHANGE) {
                        // In server mode : write then read
                        // In client mode : read then write
                        if (isServer == true) {
                            while (this.messages.isEmpty() == true) {
                                if (ClientServer.DEBUG) {
                                    Log.d(ClientServer.TAG, "I waiting for someting to write ...");
                                }

                                synchronized (this.LOCK) {
                                    try {
                                        this.LOCK.wait(16384);
                                    } catch (final Exception exception) {
                                    }
                                }

                                if (this.status != ClientServer.DO_EXCHANGE) {
                                    if (ClientServer.DEBUG) {
                                        Log.d(ClientServer.TAG, "Someone as just disconnect me.");
                                    }

                                    return;
                                }
                            }

                            message = this.messages.poll();
                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG,
                                        "meassage to send : "
                                                + HelperForNfc.byteArrayToString(message));
                            }

                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG, "Server mode");
                            }

                            this.mP2pConnection.write(message);

                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG,
                                        ">WRITE> " + HelperForNfc.byteArrayToString(message));
                            }

                            if (this.status != ClientServer.DO_EXCHANGE) {
                                if (ClientServer.DEBUG) {
                                    Log.d(ClientServer.TAG, "Someone as just disconnect me.");
                                }

                                return;
                            }

                            read = this.mP2pConnection.read(temp);

                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG,
                                            "Read = " + read + " : "
                                                    + HelperForNfc.byteArrayToString(temp, 0, read));
                            }

                            OpenNFCBackgoundService.sOpenNFCBackgoundService
                                        .sendMessage(temp, read);
                        } else {
                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG, "Client mode");
                            }

                            read = this.mP2pConnection.read(temp);

                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG,
                                            "Read = " + read + " : "
                                                    + HelperForNfc.byteArrayToString(temp, 0, read));
                            }

                            OpenNFCBackgoundService.sOpenNFCBackgoundService
                                        .sendMessage(temp, read);

                            if (this.status != ClientServer.DO_EXCHANGE) {
                                if (ClientServer.DEBUG) {
                                    Log.d(ClientServer.TAG, "Someone as just disconnect me.");
                                }

                                return;
                            }

                            while (this.messages.isEmpty() == true) {
                                if (ClientServer.DEBUG) {
                                    Log.d(ClientServer.TAG, "I waiting for someting to write ...");
                                }

                                synchronized (this.LOCK) {
                                    try {
                                        this.LOCK.wait(16384);
                                    } catch (final Exception exception) {
                                    }
                                }

                                if (this.status != ClientServer.DO_EXCHANGE) {
                                    if (ClientServer.DEBUG) {
                                        Log.d(ClientServer.TAG, "Someone as just disconnect me.");
                                    }

                                    return;
                                }
                            }

                            message = this.messages.poll();
                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG,
                                        "meassage to send : "
                                                + HelperForNfc.byteArrayToString(message));
                            }

                            this.mP2pConnection.write(message);

                            if (ClientServer.DEBUG) {
                                Log.d(ClientServer.TAG,
                                        ">WRITE> " + HelperForNfc.byteArrayToString(message));
                            }
                        }
                    }

                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "***   END of fun   ***");
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.DISCONNECT);
                } catch (final IOException exception) {
                    if (ClientServer.DEBUG) {
                        Log.d(ClientServer.TAG, "Communication break, try again ...", exception);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                            ClientServer.ESTABLISH_LINK);
                }
                break;

            case DISCONNECT:
                this.messages.clear();

                if (this.link != null) {
                    try {
                        this.link.close();
                    } catch (final Exception exception) {
                        if (ClientServer.DEBUG) {
                            Log.d(ClientServer.TAG, "link close failed", exception);
                        }
                    }
                }
                this.link = null;

                if (this.mP2pConnection != null) {
                    try {
                        this.mP2pConnection.cancel();
                    } catch (final Exception exception) {
                        if (ClientServer.DEBUG) {
                            Log.d(ClientServer.TAG, "Connection cancel failed !", exception);
                        }
                    }

                    try {
                        this.mP2pConnection.close();
                    } catch (final Exception exception) {
                        if (ClientServer.DEBUG) {
                            Log.d(ClientServer.TAG, "Connection close failed", exception);
                        }
                    }
                }
                this.mP2pConnection = null;

                if (ClientServer.DEBUG) {
                    Log.d(ClientServer.TAG, "Disocnected !");
                }

                if (ClientServer.DEBUG) {
                    Log.v(ClientServer.TAG, "End of folk !");
                }
                
                OpenNFCBackgoundService.sOpenNFCBackgoundService.sendTagLost();
                
                break;
        }
    }

    /**
     * SAP used
     * 
     * @return SAP used
     */
    int geInternalSap() {
        return this.sap;
    }

    /**
     * Client/server ID
     * 
     * @return Client/server ID
     */
    public int getID() {
        return this.id;
    }

    /**
     * URI connect
     * 
     * @return URI connect
     */
    String getInternalName() {
        return this.name;
    }

    /**
     * Ask to Open NFC stack the local SAP
     * 
     * @return Local SAP or {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     * @throws IOException If exchange failed
     * @throws NfcException On other error
     */
    public int getLocalSap() throws IOException, NfcException {
        if (this.status == ClientServer.DISCONNECT) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        if (this.mP2pConnection != null) {
            return this.mP2pConnection.getParameter(P2PConnectionParameter.LOCAL_SAP);
        }

        return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
    }

    /**
     * Ask to Open NFC stack the local socket MIU
     * 
     * @return Local socket MIU or {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     * @throws IOException If exchange failed
     * @throws NfcException On other error
     */
    public int getLocalSocketMiu() throws IOException, NfcException {
        if (this.status == ClientServer.DISCONNECT) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        if (this.mP2pConnection != null) {
            return this.mP2pConnection.getParameter(P2PConnectionParameter.LOCAL_MIU);
        }

        return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
    }

    /**
     * Ask to Open NFC stack the local socket RW
     * 
     * @return Local socket RW or {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     * @throws IOException If exchange failed
     * @throws NfcException On other error
     */
    public int getLocalSocketRw() throws IOException, NfcException {
        if (this.status == ClientServer.DISCONNECT) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        if (this.mP2pConnection != null) {
            return this.mP2pConnection.getParameter(P2PConnectionParameter.LOCAL_RW);
        }

        return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
    }

    /**
     * Ask to Open NFC stack the remote socket MIU
     * 
     * @return Remote socket MIU or
     *         {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     * @throws IOException If exchange failed
     * @throws NfcException On other error
     */
    public int getRemoteSocketMiu() throws IOException, NfcException {
        if (this.status == ClientServer.DISCONNECT) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        if (this.link != null) {
            return this.link.getRemoteMiu();
        }

        if (this.mP2pConnection != null) {
            return this.mP2pConnection.getParameter(P2PConnectionParameter.REMOTE_MIU);
        }

        return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
    }

    /**
     * Ask to Open NFC stack the remote socket RW
     * 
     * @return Remote socket RW or {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     * @throws IOException If exchange failed
     * @throws NfcException On other error
     */
    public int getRemoteSocketRw() throws IOException, NfcException {
        if (this.status == ClientServer.DISCONNECT) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        if (this.mP2pConnection != null) {
            return this.mP2pConnection.getParameter(P2PConnectionParameter.REMOTE_RW);
        }

        return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
    }

    /**
     * Actual doing task
     * 
     * @return Actual doing task
     */
    public int getStatus() {
        return this.status;
    }

    /**
     * Enqueue a message to deliver in P2P mode
     * 
     * @param message Message to send
     */
    public void inQueue(final byte[] message) {
        if (ClientServer.DEBUG) {
            Log.d(ClientServer.TAG, "Message inqueued : " + HelperForNfc.byteArrayToString(message));
        }

        this.messages.add(message);

        synchronized (this.LOCK) {
            this.LOCK.notifyAll();
        }
    }

    /**
     * Indicates if it is a server
     * 
     * @return {@code true} if it is a server
     * @throws Exception On exchange issue (Probably due a lost connection)
     */
    public boolean isServer() throws Exception {
        try {
            return this.mP2pConnection.getParameter(P2PConnectionParameter.IS_SERVER) == 1;
        } catch (final Exception exception) {
            if (ClientServer.DEBUG) {
                Log.e(ClientServer.TAG, "Is server failed ! ", exception);
            }

            throw exception;
        }
    }

    /**
     * Indicates if the client/server match to a pair of uri/sap.<br>
     * If name is {@code null} and sap is 0 in same time the result will be
     * {@code false} because not enough precise
     * 
     * @param name URI test (Can be {@code null} to mean all URI, valid only if
     *            sap is not 0)
     * @param sap SAP to test (Can be 0 to mean all SAP, valid only if name !=
     *            {@code null})
     * @return {@code true} if the client/server match
     */
    public boolean itsMe(final String name, final int sap) {
        if (this.name != null && name != null && this.name.equals(name) == true
                && (this.sap == 0 || sap == 0 || this.sap == sap)) {
            return true;
        }

        if (this.name == null || name == null) {
            return sap != 0 && this.sap == sap;
        }

        return this.name.equals(name) == true && (this.sap == 0 || sap == 0 || this.sap == sap);
    }

    /**
     * Call when the link is establish <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param link Established link
     * @see org.opennfc.p2p.P2PLinkEventHandler#onLinkDetected(org.opennfc.p2p.P2PLink)
     */
    @Override
    public void onLinkDetected(final P2PLink link) {
        if (ClientServer.DEBUG) {
            Log.d(ClientServer.TAG, "onLinkDetected");
        }

        this.link = link;

        ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.CONNECT_WITH_LINK);
    }

    /**
     * Call when link have an error <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.p2p.P2PLinkEventHandler#onLinkError()
     */
    @Override
    public void onLinkError() {
        if (ClientServer.DEBUG) {
            Log.e(ClientServer.TAG, "onLinkError");
        }

        ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.ESTABLISH_LINK);
    }

    /**
     * Call when link is lost <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.p2p.P2PLinkEventHandler#onLinkReleased()
     */
    @Override
    public void onLinkReleased() {
        if (ClientServer.DEBUG) {
            Log.w(ClientServer.TAG, "onLinkReleased");
        }

        ThreadedTaskManager.MANAGER.doInSeparateThread(this, ClientServer.ESTABLISH_LINK);
    }

    /**
     * Read (in manual mode) data from other phone
     * 
     * @param receiveBuffer Buffer to fill
     * @return Number of bytes read from phone and write in the buffer
     * @throws IOException On read issue
     */
    public int receive(final byte[] receiveBuffer) throws IOException {
        if (this.status != ClientServer.DO_EXCHANGE) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        if (this.mP2pConnection != null) {
            return this.mP2pConnection.read(receiveBuffer);
        }

        return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
    }

    /**
     * Write (in manual mode) data to other phone
     * 
     * @param data Data to write
     * @return {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED} or
     *         {@link ErrorCodes#SUCCESS}
     * @throws IOException On writing issue
     */
    public int send(final byte[] data) throws IOException {
        if (ClientServer.DEBUG) {
            Log.d(ClientServer.TAG, "Want send something : " + HelperForNfc.byteArrayToString(data));
        }
        if (ClientServer.DEBUG) {
            Log.v(ClientServer.TAG, "status=" + this.status);
        }

        if (this.status != ClientServer.DO_EXCHANGE) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        if (ClientServer.DEBUG) {
            Log.d(ClientServer.TAG, "So its connected");
        }

        if (this.mP2pConnection != null) {
            if (ClientServer.DEBUG) {
                Log.d(ClientServer.TAG, "Theire a valid connection");
            }

            this.mP2pConnection.write(data);

            if (ClientServer.DEBUG) {
                Log.d(ClientServer.TAG, "Write REALY done !");
            }

            return ErrorCodes.SUCCESS;
        }

        if (ClientServer.DEBUG) {
            Log.d(ClientServer.TAG, "No connection !");
        }

        return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
    }

    /**
     * Change the URI and/or SAP.<br>
     * The {@code null} for URI or 0 for SAP are ignored, so you can use this
     * values to let URI or SAP as they are.<br>
     * This operation is only possible if the client/server is not connected
     * 
     * @param name New URI or {@code null} to not change the current URI
     * @param sap New SAP or 0 to not change the current SAP
     */
    public void setNameSap(final String name, final int sap) {
        if (this.status != ClientServer.DISCONNECT) {
            throw new IllegalStateException(
                    "Can't change the name and/or sap while client/server is connected");
        }

        if (name != null) {
            this.name = name;
        }

        if (sap != 0) {
            this.sap = sap;
        }
    }
}
