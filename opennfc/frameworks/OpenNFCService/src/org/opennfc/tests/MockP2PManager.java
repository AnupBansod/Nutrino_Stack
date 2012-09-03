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
import org.opennfc.p2p.P2PConnectionLess;
import org.opennfc.p2p.P2PLinkEventHandler;
import org.opennfc.p2p.P2PManager;

import android.util.Log;

/**
 * Mock of P2P manager, use for test P2P only
 */
public class MockP2PManager implements P2PManager {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Mock of P2P manager singleton */
    public static final MockP2PManager MANAGER = new MockP2PManager();
    /** Tag use in debug */
    private static final String TAG = MockP2PManager.class.getSimpleName();

    /**
     * Create the mock instance
     */
    private MockP2PManager() {
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.p2p.P2PManager#cancelLink()
     */
    @Override
    public void cancelLink() {
        // TODO org.opennfc.tests.MockP2PManager.cancelLink ()
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG, "TODO : org.opennfc.tests.MockP2PManager.cancelLink ()");
        }

        return;
    }

    /**
     * Create a P2P connection.<br>
     * Here it creates a mock connection <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param mode Connection mode
     * @param serviceUri URI to contact
     * @param sap SAP to use
     * @return Created connection
     * @throws NfcException On creation issue
     * @see org.opennfc.p2p.P2PManager#createP2PConnection(org.opennfc.p2p.P2PManager.Mode,
     *      java.lang.String, byte)
     */
    @Override
    public P2PConnection createP2PConnection(final Mode mode, final String serviceUri,
            final byte sap)
            throws NfcException {
        return new MockP2PConnection();
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param serviceUri Unused
     * @param sap Unused
     * @return null
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PManager#createP2PConnectionLess(java.lang.String,
     *      byte)
     */
    @Override
    public P2PConnectionLess createP2PConnectionLess(final String serviceUri, final byte sap)
            throws NfcException {
        // TODO org.opennfc.tests.MockP2PManager.createP2PConnectionLess
        // (serviceUri, sap)
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG,
                    "TODO : org.opennfc.tests.MockP2PManager.createP2PConnectionLess (serviceUri, sap)");
        }

        return null;
    }

    /**
     * Create a link for P2P.<br>
     * Here we create a mock link <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param listener Listener to alert when link is created
     * @throws NfcException On creation issue
     * @see org.opennfc.p2p.P2PManager#establishLink(org.opennfc.p2p.P2PLinkEventHandler)
     */
    @Override
    public void establishLink(final P2PLinkEventHandler listener) throws NfcException {
        listener.onLinkDetected(new MockP2PLink());
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return false
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PManager#getActivateInitiator()
     */
    @Override
    public boolean getActivateInitiator() throws NfcException {
        // TODO org.opennfc.tests.MockP2PManager.getActivateInitiator ()
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG,
                    "TODO : org.opennfc.tests.MockP2PManager.getActivateInitiator ()");
        }

        return false;
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PManager#getLocalLto()
     */
    @Override
    public short getLocalLto() throws NfcException {
        // TODO org.opennfc.tests.MockP2PManager.getLocalLto ()
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG, "TODO : org.opennfc.tests.MockP2PManager.getLocalLto ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return 0
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PManager#getLocalMiu()
     */
    @Override
    public short getLocalMiu() throws NfcException {
        // TODO org.opennfc.tests.MockP2PManager.getLocalMiu ()
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG, "TODO : org.opennfc.tests.MockP2PManager.getLocalMiu ()");
        }

        return 0;
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param flag Unused
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PManager#setActivateInitiator(boolean)
     */
    @Override
    public void setActivateInitiator(final boolean flag) throws NfcException {
        // TODO org.opennfc.tests.MockP2PManager.setActivateInitiator (flag)
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG,
                    "TODO : org.opennfc.tests.MockP2PManager.setActivateInitiator (flag)");
        }

        return;
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param localLto Unused
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PManager#setLocalLto(short)
     */
    @Override
    public void setLocalLto(final short localLto) throws NfcException {
        // TODO org.opennfc.tests.MockP2PManager.setLocalLto (localLto)
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG,
                    "TODO : org.opennfc.tests.MockP2PManager.setLocalLto (localLto)");
        }

        return;
    }

    /**
     * TODO Implements it if need in future tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param localMiu Unused
     * @throws NfcException Not happen
     * @see org.opennfc.p2p.P2PManager#setLocalMiu(short)
     */
    @Override
    public void setLocalMiu(final short localMiu) throws NfcException {
        // TODO org.opennfc.tests.MockP2PManager.setLocalMiu (localMiu)
        if (MockP2PManager.DEBUG) {
            Log.d(MockP2PManager.TAG,
                    "TODO : org.opennfc.tests.MockP2PManager.setLocalMiu (localMiu)");
        }

        return;
    }
}
