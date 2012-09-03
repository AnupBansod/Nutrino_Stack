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

import org.opennfc.HelperForNfc;
import org.opennfc.nfctag.NdefMessage;
import org.opennfc.nfctag.NdefRecord;
import org.opennfc.service.communication.ClientServer;

import android.util.Log;

import java.io.ByteArrayInputStream;
import java.util.Vector;

/**
 * Launch and drive test P2P.<br>
 * It creates as singleton, but there are 2 instances in different context, one
 * in test context, other in Activity context.<br>
 * So we have to transfer collected test information from Activity context to
 * test context to be able to print them<br>
 * NOTE : Event if, you not see "direct" method called on Activity context, some
 * parent method are called in Activity context
 */
public class TestP2P extends TestScenario {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = TestP2P.class.getSimpleName();
    /**
     * Test of P2P singleton. (NOTE : There are 2 instances in different
     * context, one in test context, other in Activity context)
     */
    public static final TestP2P TEST = new TestP2P();

    /**
     * Create the singleton instance
     */
    private TestP2P() {
    }

    /**
     * Call when test is finish.<br>
     * It merge the Activity context information (Stored in the tag connection)
     * with test context information.<br>
     * NOTE : here we are in test context
     * 
     * @param mockNfcTagConnection Tag connection with Activity information
     *            inside
     */
    public void finish(final MockNfcTagConnection mockNfcTagConnection) {
        // Try to go back to normal for P2P
        ClientServer.inject(null);

        try {
            final NdefMessage message = mockNfcTagConnection.getWriteMessage();
            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG, "message=" + message);
            }
            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG, "    hex=" + HelperForNfc.toHexadecimal(message.getContent()));
            }

            final Vector<NdefRecord> records = message.getRecords();
            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG, "records=" + records);
            }
            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG, "records.size()=" + records.size());
            }

            final NdefRecord ndefRecord = records.elementAt(0);
            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG, "ndefRecord=" + ndefRecord);
            }
            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG,
                        "       hex=" + HelperForNfc.toHexadecimal(ndefRecord.getContent()));
            }

            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG, "PAYLOAD=" + HelperForNfc.toHexadecimal(ndefRecord.getPayload()));
            }

            final ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(
                    ndefRecord.getPayload());

            this.deserialize(byteArrayInputStream);
        } catch (final Exception exception) {
            if (TestP2P.DEBUG) {
                Log.d(TestP2P.TAG, "Get info failed", exception);
            }
        }

        this.stopTest();
    }

    /**
     * Call to launch the test.<br>
     * NOTE : Here we are in test context <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.tests.TestScenario#start()
     */
    @Override
    protected void start() {
        // Inject a mock P2P manager for testing and follow what happen
        ClientServer.inject(MockP2PManager.MANAGER);

        TestUtil.launchActivity(ActivityForInternalTestsDispach.class);

        HelperForNfc.sleep(4096);

        final MockNfcTagConnection mockNfcTagConnection = new MockNfcTagConnection(1, null,
                MockNfcTagConnection.MODE_P2P);

        TestUtil.mCardDetectionEventHandler.onCardDetected(mockNfcTagConnection);
    }
}
