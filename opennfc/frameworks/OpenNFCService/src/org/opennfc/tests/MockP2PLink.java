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
import org.opennfc.p2p.P2PLink;

import android.util.Log;

import java.io.IOException;

/**
 * Mock a P2P link
 */
public class MockP2PLink implements P2PLink {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = MockP2PLink.class.getSimpleName();

    /**
     * Create the link
     */
    public MockP2PLink() {
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#close()
     */
    @Override
    public void close() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.close ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.close ()");
        }

        return;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#getAgreeLlcpVersion()
     */
    @Override
    public byte getAgreeLlcpVersion() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.getAgreeLlcpVersion ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.getAgreeLlcpVersion ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#getBaudeRate()
     */
    @Override
    public int getBaudeRate() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.getBaudeRate ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.getBaudeRate ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#getRemoteLlcpVersion()
     */
    @Override
    public byte getRemoteLlcpVersion() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.getRemoteLlcpVersion ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.getRemoteLlcpVersion ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#getRemoteLto()
     */
    @Override
    public int getRemoteLto() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.getRemoteLto ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.getRemoteLto ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#getRemoteMiu()
     */
    @Override
    public int getRemoteMiu() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.getRemoteMiu ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.getRemoteMiu ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#getRemoteWks()
     */
    @Override
    public int getRemoteWks() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.getRemoteWks ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.getRemoteWks ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return false
     * @throws IOException Not happen
     * @see org.opennfc.p2p.P2PLink#isInitiator()
     */
    @Override
    public boolean isInitiator() throws IOException {
        // TODO org.opennfc.tests.MockP2PLink.isInitiator ()
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.isInitiator ()");
        }

        return false;
    }

    /**
     * TODO Implements it if used in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param serviceUri Unused
     * @return 0
     * @throws IOException Not happen
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PLink#UriLookup(java.lang.String)
     */
    @Override
    public byte UriLookup(final String serviceUri) throws IOException, NfcException {
        // TODO org.opennfc.tests.MockP2PLink.UriLookup (serviceUri)
        if (MockP2PLink.DEBUG) {
            Log.d(MockP2PLink.TAG, "TODO : org.opennfc.tests.MockP2PLink.UriLookup (serviceUri)");
        }

        return 0;
    }
}
