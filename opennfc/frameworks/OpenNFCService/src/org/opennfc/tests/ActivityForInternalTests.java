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

import java.io.IOException;

/**
 * Activity for test reading/writing
 */
public class ActivityForInternalTests extends Activity {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = ActivityForInternalTests.class.getSimpleName();

    /**
     * Call when Activity created <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param savedInstanceState Last instance stored information
     * @see android.app.Activity#onCreate(android.os.Bundle)
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Get the tag that launched the Activity
        final Intent intent = this.getIntent();

        final Tag tag = (Tag) intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);

        // Get the way for communicate (As application developer will do)
        final Ndef ndef = Ndef.get(tag);
        try {
            // First have to connect
            ndef.connect();
        } catch (final IOException exception) {
            if (ActivityForInternalTests.DEBUG) {
                Log.d(ActivityForInternalTests.TAG, "Connect failed", exception);
            }

            TestTagReadingWriting.TEST.assertFail("Connect failed : " + exception);
        }

        // Do the rest in separate thread to avoid ANR while testing
        (new Thread() {
            @Override
            public void run() {
                final Parcelable[] parcelables = intent.getParcelableArrayExtra(
                        NfcAdapter.EXTRA_NDEF_MESSAGES);

                if (parcelables != null) {
                    final NdefMessage[] messages = new NdefMessage[parcelables.length];

                    for (int i = parcelables.length - 1; i >= 0; i--) {
                        messages[i] = (NdefMessage) parcelables[i];
                    }

                    for (final NdefMessage message : messages) {
                        for (final NdefRecord ndefRecord : message.getRecords()) {
                            // Check if received palyload is correct
                            TestTagReadingWriting.TEST.receiveTag(tag, ndefRecord.getPayload());
                        }
                    }
                } else {
                    TestTagReadingWriting.TEST
                            .assertFail("Must have at least one message to read !");
                }

                // Pass to next part of the test
                TestTagReadingWriting.TEST.finishReading();

                ActivityForInternalTests.this.finish();
            }
        }).start();
    }
}
