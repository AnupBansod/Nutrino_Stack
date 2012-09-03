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

package org.opennfc.service;

import org.opennfc.NfcErrorCode;
import org.opennfc.nfctag.NfcTagConnection;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Describe an NFC information.<br>
 * It manages time out between connect and close.<br>
 * It manages tag connection
 */
public class NfcInformation {
    /**
     * Task for time out management
     */
    class TimeoutTask implements ThreadedTask<Void> {
        /**
         * Cancel the task <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @see org.opennfc.service.ThreadedTask#cancel()
         */
        @Override
        public void cancel() {
            // Nothing to do here
        }

        /**
         * Time out management <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param taskID Task ID. Unused
         * @param parameters Parameters. Unused
         * @see org.opennfc.service.ThreadedTask#excuteTask(int, PARAMETER[])
         */
        @Override
        public void excuteTask(final int taskID, final Void... parameters) {
            boolean timeNotOut;
            boolean closed;

            synchronized (NfcInformation.this) {
                timeNotOut = NfcInformation.this.timeInitialized.get();
                closed = NfcInformation.this.connectionClosed.get();
            }

            // While timeout not append AND the connection is not closed
            while (timeNotOut == true
                    && closed == false) {
                synchronized (NfcInformation.this) {
                    // Now time out may append
                    NfcInformation.this.timeInitialized.set(false);

                    // We wait for time out, normal close or initialize again
                    try {
                        NfcInformation.this.wait(NfcInformation.TIMEOUT);
                    } catch (final Exception exception) {
                    }

                    // Get the new timeout and connection status
                    timeNotOut = NfcInformation.this.timeInitialized.get();
                    closed = NfcInformation.this.connectionClosed.get();
                }
            }

            // Treatment for timeout or connection close
            NfcInformation.this.timeoutOrConnectionClose();
        }
    }

    /** State of connection to say connection is established */
    public static final int STATUS_CONNECTED = 1;
    /** State of connection to say connection disconnected */
    public static final int STATUS_DISCONNECTED = 2;
    /**
     * State of connection to say that it is not already connect (That is to say
     * the connect method never called)
     */
    public static final int STATUS_NOT_CONNECTED = 0;

    /** Time out connection in milliseconds, actually 16s 384ms */
    static final int TIMEOUT = 16384;
    /** Actual connection status */
    int connectedStatus;

    /** Indicates if connection properly closed */
    AtomicBoolean connectionClosed;
    /** Tag handle id */
    final int id;
    /** Tag connection */
    NfcTagConnection mNfcTagConnection;

    public NfcTagConnection getNfcTagConnection() {
        return this.mNfcTagConnection;
    }

    /** Task for timeout */
    private TimeoutTask mTimeoutTask;
    /** UID for override in case of Topaz (TODO check if really need) */
    byte[] overideID;
    /** Technologies list */
    int[] techList;
    /** Time start for perish */
    long time;
    /** Indicates that timeout is just refreshed to 0 */
    AtomicBoolean timeInitialized;
    /** Connection error code */
    NfcErrorCode what;

    /**
     * Create the information
     * 
     * @param id Tag handle id
     * @param nfcTagConnection Connection with the tag
     * @param techList Technologies list
     */
    NfcInformation(final int id, final NfcTagConnection nfcTagConnection, final int[] techList) {
        this.id = id;
        this.connectedStatus = NfcInformation.STATUS_NOT_CONNECTED;
        this.mNfcTagConnection = nfcTagConnection;
        this.techList = techList;
        this.timeInitialized = new AtomicBoolean(false);
        this.connectionClosed = new AtomicBoolean(true);
    }

    /**
     * Indicates that connection ends normally.
     */
    public void finishTime() {
        if (this.mTimeoutTask == null) {
            return;
        }

        synchronized (NfcInformation.this) {
            this.connectionClosed.set(true);
            this.timeInitialized.set(false);

            NfcInformation.this.notifyAll();
        }

        this.mTimeoutTask = null;
    }

    /**
     * Initialize or reinitialize the time of timeout
     */
    public void intializeTime() {
        synchronized (NfcInformation.this) {
            this.timeInitialized.set(true);
            this.connectionClosed.set(false);

            if (this.mTimeoutTask == null) {
                this.mTimeoutTask = new TimeoutTask();

                ThreadedTaskManager.MANAGER.doInSeparateThread(this.mTimeoutTask);

                return;
            }

            NfcInformation.this.notifyAll();
        }
    }

    /**
     * Call by system when timeout append or normal connection close
     */
    void timeoutOrConnectionClose() {
        NfcInformationCache.getCache().remove(this.id);

        this.mTimeoutTask = null;
    }
}
