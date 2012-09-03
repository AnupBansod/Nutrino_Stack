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

import android.app.Activity;
import android.content.Intent;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.tech.Ndef;
import android.os.Bundle;
import android.os.Parcelable;
import android.util.Log;

import java.io.ByteArrayOutputStream;

/**
 * Activity for treat the result of P2P
 */
public class ActivityForInternalTestsResultP2P extends Activity implements Runnable {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Extra to use for transfer the initiator tag */
    public static final String EXTRA_TRANSFER_TAG = "org.opennfc.tests.ActivityForInteranlTestsResultP2P.EXTRA.TRANSFER_TAG";
    /** Tag use in debug */
    private static final String TAG = ActivityForInternalTestsResultP2P.class.getSimpleName();
    /** Listener to call when Activity finish */
    private IFinishListener finishWithMe;
    /** Intent that launched this Activity */
    private Intent intent;

    /**
     * Finish the activity now
     */
    void doFinishNow() {
        if (ActivityForInternalTestsResultP2P.DEBUG) {
            Log.d(ActivityForInternalTestsResultP2P.TAG,
                    "Good bye : ActivityForInteranlTestsResultP2P !");
        }

        try {
            this.finishWithMe.finishNow();
        } catch (final Exception exception) {
            if (ActivityForInternalTestsResultP2P.DEBUG) {
                Log.d(ActivityForInternalTestsResultP2P.TAG, "Finish other failed !", exception);
            }
        }

        this.finish();
    }

    /**
     * Call when activity created <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param savedInstanceState Last instance stored information
     * @see android.app.Activity#onCreate(android.os.Bundle)
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.intent = this.getIntent();
        this.finishWithMe = IFinishListener.Stub.asInterface(this.intent
                .getIBinderExtra(TestUtil.EXTRA_FINISH_LISTENER));

        (new Thread(this)).start();
    }

    /**
     * In separate thread :
     * <ol>
     * <li>Check if message is correct</li>
     * <li>Send test result to test context</li>
     * </ol>
     * <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see java.lang.Runnable#run()
     */
    @Override
    public void run() {
        // Wait a little to be sure to be ready
        try {
            Thread.sleep(1024);
        } catch (final Exception exception) {
        }

        // Check if message is correct
        final Parcelable[] parcelables = this.intent
                .getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES);

        if (parcelables == null || parcelables.length == 0) {
            TestP2P.TEST.assertFail("No message found");
        } else {
            final NdefMessage ndefMessage = (NdefMessage) parcelables[0];
            final NdefRecord ndefRecord = ndefMessage.getRecords()[0];
            final byte[] payload = ndefRecord.getPayload();

            TestP2P.TEST.assertEquals("Not receive same as send !", payload,
                    ActivityForInternalTestsPush.PAYLOAD);
        }

        // Transfer test result to test context
        final Tag tag = (Tag) this.intent
                .getParcelableExtra(ActivityForInternalTestsResultP2P.EXTRA_TRANSFER_TAG);
        try {
            final ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream(4096);
            TestP2P.TEST.serialize(byteArrayOutputStream);

            final Ndef ndef = Ndef.get(tag);
            ndef.connect();

            ndef.writeNdefMessage(TestUtil.createAndroidMessage(byteArrayOutputStream.toByteArray()));
        } catch (final Exception exception) {
            if (ActivityForInternalTestsResultP2P.DEBUG) {
                Log.d(ActivityForInternalTestsResultP2P.TAG, "Save info failed !", exception);
            }
        }

        this.doFinishNow();
    }
}
