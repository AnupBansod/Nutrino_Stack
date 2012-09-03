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

import android.nfc.ErrorCodes;
import android.nfc.ILlcpServiceSocket;
import android.os.RemoteException;

/**
 * Implements a LLCP server socket manager
 */
public class ILlcpServiceSocketImpl extends ILlcpServiceSocket.Stub {
    /** Singleton instance */
    public static final ILlcpServiceSocketImpl SOCKET;
    /** Tag use in debug */
    private static final String TAG = ILlcpServiceSocketImpl.class.getSimpleName();
    static {
        SOCKET = new ILlcpServiceSocketImpl();
    }

    /**
     * Create the instance
     */
    private ILlcpServiceSocketImpl() {
    }

    /**
     * Make a server wait for a client to connect.<br>
     * This method block until a client connect or an issue happen <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Server handle
     * @return connected client's handle OR
     *         {@link ErrorCodes#ERROR_SOCKET_NOT_CONNECTED} if server not
     *         connect OR {@link ErrorCodes#ERROR_SOCKET_CREATION} if link
     *         between server and client can't be establish
     * @throws RemoteException On losing AIDL link
     * @see android.nfc.ILlcpServiceSocket#accept(int)
     */
    @Override
    public int accept(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return ErrorCodes.ERROR_SOCKET_NOT_CONNECTED;
        }

        clientServer.connect();

        while (clientServer.getStatus() != ClientServer.DO_EXCHANGE) {
            synchronized (clientServer.LOCK) {
                try {
                    clientServer.LOCK.wait(16384);
                } catch (final Exception exception) {
                }
            }

            if (clientServer.getStatus() == ClientServer.DISCONNECT) {
                return ErrorCodes.ERROR_SOCKET_CREATION;
            }
        }

        return nativeHandle;
    }

    /**
     * Close a server <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param nativeHandle Server handle to close
     * @throws RemoteException On losing AIDL link
     * @see android.nfc.ILlcpServiceSocket#close(int)
     */
    @Override
    public void close(final int nativeHandle) throws RemoteException {
        final ClientServer clientServer = ClientServerCache.CACHE.get(nativeHandle);

        if (clientServer == null) {
            return;
        }

        clientServer.disconnect();
    }
}
