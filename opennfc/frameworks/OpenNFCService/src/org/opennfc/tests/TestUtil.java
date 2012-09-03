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
import org.opennfc.NfcManager;
import org.opennfc.cardlistener.CardDetectionEventHandler;
import org.opennfc.nfctag.NdefMessage;
import org.opennfc.p2p.P2PManager;
import org.opennfc.service.OpenNFCBackgoundService;
import org.opennfc.service.ThreadedTask;
import org.opennfc.service.ThreadedTaskManager;

import android.app.Activity;
import android.app.Service;
import android.content.Intent;
import android.util.Log;

/**
 * Utilities for launch the tests
 */
public class TestUtil implements ThreadedTask<Void> {
    /**
     * Describe a scenario
     */
    static class Scenario {
        /** Scenario test class to launch */
        TestScenario mTestScenario;
        /** Scenario name */
        String title;

        /**
         * Create a scenario
         * 
         * @param title Scenario name
         * @param testScenario Test to launch
         */
        public Scenario(final String title, final TestScenario testScenario) {
            this.title = title;
            this.mTestScenario = testScenario;
        }
    }

    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Extra for finish listener */
    public static final String EXTRA_FINISH_LISTENER = "FinishListener";
    /** Card detection listener for simulate a arrive card */
    static CardDetectionEventHandler mCardDetectionEventHandler;
    /** P2P manager instance */
    private static P2PManager p2pManager;
    /**
     * List of scenario to launch
     */
    static final Scenario[] scenarios = {
             // new Scenario("P2P manager", new TestP2PManager())
            new Scenario("Helper for NFC", new HelperForNfcTest()),//
            new Scenario("Tag reading", TestTagReadingWriting.TEST),//
            new Scenario("Peer to peer", TestP2P.TEST),//
    };
    /** Service that launch the test */
    private static Service service;
    /** Tag use in debug */
    private static final String TAG = TestUtil.class.getSimpleName();
    /** Indicates if tests are launch or not */
    public static final boolean TEST_MODE = false;

    /**
     * Create an Android NdefMessage : {@link android.nfc.NdefMessage} with a
     * given payload
     * 
     * @param payload Payload to put inside the message
     * @return Created message
     */
    public static android.nfc.NdefMessage createAndroidMessage(final byte[] payload) {
        final android.nfc.NdefRecord record = new android.nfc.NdefRecord(
                    android.nfc.NdefRecord.TNF_UNKNOWN, new byte[0], new byte[0], payload);

        return new android.nfc.NdefMessage(new android.nfc.NdefRecord[] {
                    record
            });
    }

    /**
     * Create an Open Nfc NdefMessage : {@link NdefMessage} with a given payload
     * 
     * @param payload Payload to put inside the message
     * @return Created message
     * @throws NfcException On creation issue
     */
    public static NdefMessage createMessage(final byte[] payload) throws NfcException {
        final android.nfc.NdefRecord record = new android.nfc.NdefRecord(
                    android.nfc.NdefRecord.TNF_UNKNOWN, new byte[0], new byte[0], payload);

        final android.nfc.NdefMessage message = new android.nfc.NdefMessage(
                new android.nfc.NdefRecord[] {
                    record
            });

        return new NdefMessage(message.toByteArray());
    }

    /**
     * Obtain the P2P manager instance
     * 
     * @return P2P manager instance
     * @throws NfcException On getting issue
     */
    public static P2PManager getP2PManager() throws NfcException {
        if (TestUtil.p2pManager == null) {
            final NfcManager mNfcManager = OpenNFCBackgoundService.getNfcManager();

            if (TestUtil.DEBUG) {
                Log.d(TestUtil.TAG, "$$$$$$$$$$$$$$$$$$$$$$$>>>>>>>>>>>>>> mNfcManager="
                        + mNfcManager);
            }

            TestUtil.p2pManager = mNfcManager.getP2PManager();
        }

        return TestUtil.p2pManager;
    }

    /**
     * Launch an activity
     * 
     * @param activityClass Activity class to launch
     */
    public static void launchActivity(final Class<? extends Activity> activityClass) {
        final Intent intent = new Intent(TestUtil.service, activityClass);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        TestUtil.service.startActivity(intent);
    }

    /**
     * Start the tests
     * 
     * @param service Service that launch the test
     * @param mCardDetectionEventHandler Listener of card detection
     */
    public static void startTests(final Service service,
            final CardDetectionEventHandler mCardDetectionEventHandler) {
        TestUtil.service = service;

        TestUtil.mCardDetectionEventHandler = mCardDetectionEventHandler;

        ThreadedTaskManager.MANAGER.doInSeparateThread(new TestUtil());
    }

    /**
     * Call when tests are canceled <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.service.ThreadedTask#cancel()
     */
    @Override
    public void cancel() {
    }

    /**
     * Dedicated thread task fo launch the tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param taskID Task ID
     * @param parameters Unused
     * @see org.opennfc.service.ThreadedTask#excuteTask(int, PARAMETER[])
     */
    @Override
    public void excuteTask(final int taskID, final Void... parameters) {
        Log.d(TestUtil.TAG, "Tests will start in 16s 384ms");

        try {
            Thread.sleep(16384);
        } catch (final Exception exception) {
        }

        Log.d(TestUtil.TAG, "Test Start !");

        for (final Scenario scenario : TestUtil.scenarios) {
            final int length = scenario.title.length() + 22;
            final char[] separators = new char[length];

            for (int i = 0; i < length; i++) {
                separators[i] = '-';
            }

            final String lineSeparator = new String(separators);

            Log.d(TestUtil.TAG, "");
            Log.d(TestUtil.TAG, lineSeparator);
            Log.d(TestUtil.TAG, "---   TESTING : " + scenario.title + "   ---");
            Log.d(TestUtil.TAG, lineSeparator);

            scenario.mTestScenario.startTest();

            Log.d(TestUtil.TAG, "");
            Log.d(TestUtil.TAG, lineSeparator);
            Log.d(TestUtil.TAG, "");
        }

        Log.d(TestUtil.TAG, "");
        Log.d(TestUtil.TAG, "Test End !");
    }
}
