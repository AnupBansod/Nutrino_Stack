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

package org.opennfc.tests;

import org.opennfc.NfcException;
import org.opennfc.p2p.P2PConnection;
import org.opennfc.p2p.P2PLink;

import android.util.Log;

import java.io.IOException;

/**
 * Mock of P2P connection
 */
public class MockP2PConnection implements P2PConnection {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = MockP2PConnection.class.getSimpleName();
    /** Last writen data */
    private byte[] write;

    /**
     * Create the connection
     */
    public MockP2PConnection() {
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.p2p.P2PConnection#cancel()
     */
    @Override
    public void cancel() {
        // TODO org.opennfc.tests.MockP2PConnection.cancel ()
        if (MockP2PConnection.DEBUG) {
            Log.d(MockP2PConnection.TAG, "TODO : org.opennfc.tests.MockP2PConnection.cancel ()");
        }

        return;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PConnection#close()
     */
    @Override
    public void close() throws IOException {
        // TODO org.opennfc.tests.MockP2PConnection.close ()
        if (MockP2PConnection.DEBUG) {
            Log.d(MockP2PConnection.TAG, "TODO : org.opennfc.tests.MockP2PConnection.close ()");
        }

        return;
    }

    /**
     * Connect to a link <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param link Link to connect with
     * @throws IOException On reading issue
     * @throws NfcException On other issue
     * @see org.opennfc.p2p.P2PConnection#connect(org.opennfc.p2p.P2PLink)
     */
    @Override
    public void connect(final P2PLink link) throws IOException, NfcException {
    }

    /**
     * Obtain a parameter of the connection.<br>
     * Here we answer 1 only for {@link P2PConnectionParameter#IS_SERVER} and 0
     * for others <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param parameter Tested parameter
     * @return {@code 1} for {@link P2PConnectionParameter#IS_SERVER}. {@code 0}
     *         for others
     * @throws IOException On reading issue
     * @throws NfcException On other issue
     * @see org.opennfc.p2p.P2PConnection#getParameter(org.opennfc.p2p.P2PConnection.P2PConnectionParameter)
     */
    @Override
    public int getParameter(final P2PConnectionParameter parameter) throws IOException,
            NfcException {
        switch (parameter) {
            case IS_SERVER:
                return 1;
        }

        return 0;
    }

    /**
     * Last written data
     * 
     * @return Last written data
     */
    public byte[] getWrite() {
        return this.write;
    }

    /**
     * Read some data <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param data Where write the read data
     * @return number of read bytes
     * @throws IOException On reading issue
     * @see org.opennfc.p2p.P2PConnection#read(byte[])
     */
    @Override
    public int read(final byte[] data) throws IOException {
        if (this.write == null) {
            return -1;
        }

        final int length = Math.min(this.write.length, data.length);

        System.arraycopy(this.write, 0, data, 0, length);

        return length;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param data Unused
     * @param off Unused
     * @param len Unused
     * @return -1
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PConnection#read(byte[], int, int)
     */
    @Override
    public int read(final byte[] data, final int off, final int len) throws IOException {
        // TODO org.opennfc.tests.MockP2PConnection.read (data, off, len)
        if (MockP2PConnection.DEBUG) {
            Log.d(MockP2PConnection.TAG,
                    "TODO : org.opennfc.tests.MockP2PConnection.read (data, off, len)");
        }

        return -1;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param parameter Unused
     * @param value Unused
     * @throws IOException Not happen
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PConnection#setParameter(org.opennfc.p2p.P2PConnection.P2PConnectionParameter,
     *      int)
     */
    @Override
    public void setParameter(final P2PConnectionParameter parameter, final int value)
            throws IOException,
            NfcException {
        // TODO org.opennfc.tests.MockP2PConnection.setParameter (parameter,
        // value)
        if (MockP2PConnection.DEBUG) {
            Log.d(MockP2PConnection.TAG,
                    "TODO : org.opennfc.tests.MockP2PConnection.setParameter (parameter, value)");
        }

        return;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @throws IOException Not happen
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PConnection#shutdown()
     */
    @Override
    public void shutdown() throws IOException, NfcException {
        // TODO org.opennfc.tests.MockP2PConnection.shutdown ()
        if (MockP2PConnection.DEBUG) {
            Log.d(MockP2PConnection.TAG, "TODO : org.opennfc.tests.MockP2PConnection.shutdown ()");
        }

        return;
    }

    /**
     * Write some data <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param data Data to write
     * @throws IOException On writing issue
     * @see org.opennfc.p2p.P2PConnection#write(byte[])
     */
    @Override
    public void write(final byte[] data) throws IOException {
        this.write = data;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param data Unused
     * @param off Unused
     * @param len Unused
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PConnection#write(byte[], int, int)
     */
    @Override
    public void write(final byte[] data, final int off, final int len) throws IOException {
        // TODO org.opennfc.tests.MockP2PConnection.write (data, off, len)
        if (MockP2PConnection.DEBUG) {
            Log.d(MockP2PConnection.TAG,
                    "TODO : org.opennfc.tests.MockP2PConnection.write (data, off, len)");
        }

        return;
    }
}
