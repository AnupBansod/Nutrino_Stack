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

import android.nfc.ErrorCodes;
import android.nfc.ILlcpSocket;
import android.os.RemoteException;
import android.util.Log;

import java.io.IOException;

/**
 * Client socket LLCP manager
 * 
 * @hide
 */
public class ILlcpSocketImpl extends ILlcpSocket.Stub {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Client socket LLCP unique instance */
    public static final ILlcpSocketImpl SOCKET;
    /** Tag use in debug */
    private static final String TAG = ILlcpSocketImpl.class.getSimpleName();
    static {
        SOCKET = new ILlcpSocketImpl();
    }

    /**
     * Create the manager
     */
    private ILlcpSocketImpl() {
    }

    /**
     * Close a client socket LLCP <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client socket LLCP handle to close
     * @return {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED} or
     *         {@link ErrorCodes#SUCCESS}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#close(int)
     */
    @Override
    public int close(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        clientServer.disconnect();

        return ErrorCodes.SUCCESS;
    }

    /**
     * Connect a client to a specific SAP.<br>
     * If the connection is already established, the SAP must be the same as
     * already connected <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle to connect
     * @param sap SAP where connect
     * @return {@link ErrorCodes#ERROR_SOCKET_OPTIONS},
     *         {@link ErrorCodes#ERROR_SOCKET_CREATION} or
     *         {@link ErrorCodes#SUCCESS}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#connect(int, int)
     */
    @Override
    public int connect(final int nativeHandle, final int sap) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_CREATION;
        }

        if (clientServer.getStatus() != ClientServer.DISCONNECT) {
            if (sap == clientServer.geInternalSap()) {
                return ErrorCodes.SUCCESS;
            }

            return ErrorCodes.ERROR_SOCKET_OPTIONS;
        }

        clientServer.setNameSap(null, sap);
        clientServer.connect();

        return ErrorCodes.SUCCESS;
    }

    /**
     * Connect a client to a specific URI.<br>
     * If the connection is already established, the URI must be the same as
     * already connected <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle to connect
     * @param uri URI to connect
     * @return {@link ErrorCodes#ERROR_SOCKET_OPTIONS},
     *         {@link ErrorCodes#ERROR_SOCKET_CREATION} or
     *         {@link ErrorCodes#SUCCESS}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#connectByName(int, java.lang.String)
     */
    @Override
    public int connectByName(final int nativeHandle, final String uri) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_CREATION;
        }

        if (clientServer.getStatus() != ClientServer.DISCONNECT) {
            if (uri.equals(clientServer.getInternalName()) == true) {
                return ErrorCodes.SUCCESS;
            }

            return ErrorCodes.ERROR_SOCKET_OPTIONS;
        }

        clientServer.setNameSap(uri, 0);
        clientServer.connect();

        return ErrorCodes.SUCCESS;
    }

    /**
     * Obtain a client local SAP <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle
     * @return Client local SAP, {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     *         or {@link ErrorCodes#ERROR_IO}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#getLocalSap(int)
     */
    @Override
    public int getLocalSap(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        try {
            return clientServer.getLocalSap();
        } catch (final IOException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getLocalSap", exception);
            }

            return ErrorCodes.ERROR_IO;
        } catch (final NfcException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getLocalSap", exception);
            }

            return HelperForNfc.obtainErrorCode(exception, ErrorCodes.ERROR_SOCKET_NOT_CONNECTED);
        }
    }

    /**
     * Obtain a client local MIU <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle
     * @return Client local MIU, {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     *         or {@link ErrorCodes#ERROR_IO}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#getLocalSocketMiu(int)
     */
    @Override
    public int getLocalSocketMiu(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        try {
            return clientServer.getLocalSocketMiu();
        } catch (final IOException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getLocalSocketMiu", exception);
            }

            return ErrorCodes.ERROR_IO;
        } catch (final NfcException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getLocalSocketMiu", exception);
            }

            return HelperForNfc.obtainErrorCode(exception, ErrorCodes.ERROR_SOCKET_NOT_CONNECTED);
        }
    }

    /**
     * Obtain a client local RW <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle
     * @return Client local RW, {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED} or
     *         {@link ErrorCodes#ERROR_IO}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#getLocalSocketRw(int)
     */
    @Override
    public int getLocalSocketRw(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        try {
            return clientServer.getLocalSocketRw();
        } catch (final IOException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getLocalSocketRw", exception);
            }

            return ErrorCodes.ERROR_IO;
        } catch (final NfcException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getLocalSocketRw", exception);
            }

            return HelperForNfc.obtainErrorCode(exception, ErrorCodes.ERROR_SOCKET_NOT_CONNECTED);
        }
    }

    /**
     * Obtain a client remote MIU <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle
     * @return Client remote MIU, {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     *         or {@link ErrorCodes#ERROR_IO}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#getRemoteSocketMiu(int)
     */
    @Override
    public int getRemoteSocketMiu(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        try {
            return clientServer.getRemoteSocketMiu();
        } catch (final IOException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getRemoteSocketMiu", exception);
            }

            return ErrorCodes.ERROR_IO;
        } catch (final NfcException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getRemoteSocketMiu", exception);
            }

            return HelperForNfc.obtainErrorCode(exception, ErrorCodes.ERROR_SOCKET_NOT_CONNECTED);
        }
    }

    /**
     * Obtain a client remote RW <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle
     * @return Client remote RW, {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED}
     *         or {@link ErrorCodes#ERROR_IO}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#getRemoteSocketRw(int)
     */
    @Override
    public int getRemoteSocketRw(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        try {
            return clientServer.getRemoteSocketRw();
        } catch (final IOException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getRemoteSocketRw", exception);
            }

            return ErrorCodes.ERROR_IO;
        } catch (final NfcException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "getRemoteSocketRw", exception);
            }

            return HelperForNfc.obtainErrorCode(exception, ErrorCodes.ERROR_SOCKET_NOT_CONNECTED);
        }
    }

    /**
     * Read data from client <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle
     * @param receiveBuffer Where write read data
     * @return Number of byte read,
     *         {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED} or
     *         {@link ErrorCodes#ERROR_IO}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#receive(int, byte[])
     */
    @Override
    public int receive(final int nativeHandle, final byte[] receiveBuffer) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        try {
            return clientServer.receive(receiveBuffer);
        } catch (final IOException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "receive", exception);
            }

            return ErrorCodes.ERROR_IO;
        }
    }

    /**
     * Write data to client <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Client handle
     * @param data Data to write
     * @return {@link ErrorCodes#SUCCESS},
     *         {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED} or
     *         {@link ErrorCodes#ERROR_IO}
     * @throws RemoteException On broken link
     * @see android.nfc.ILlcpSocket#send(int, byte[])
     */
    @Override
    public int send(final int nativeHandle, final byte[] data) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        try {
            return clientServer.send(data);
        } catch (final IOException exception) {
            if (ILlcpSocketImpl.DEBUG) {
                Log.d(ILlcpSocketImpl.TAG, "send", exception);
            }

            return ErrorCodes.ERROR_IO;
        }
    }
}
