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
import android.app.PendingIntent;
import android.content.Intent;
import android.nfc.NdefMessage;
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.util.Log;

/**
 * Activity for push P2P message
 */
public class ActivityForInternalTestsPush extends Activity {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Payload's test */
    public static final byte[] PAYLOAD = {
            (byte) 0x00, (byte) 0xCA, (byte) 0xFE, (byte) 0x00, (byte) 0xFA, (byte) 0xCE,
            (byte) 0x00, (byte) 0xBA, (byte) 0xFE
    };
    /** Tag use in debug */
    private static final String TAG = ActivityForInternalTestsPush.class.getSimpleName();
    /** Listener called when activity have to finish */
    private final IFinishListener.Stub finishListener = new IFinishListener.Stub() {
        /**
         * Called when activity have to finish <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @see org.opennfc.tests.IFinishListener#finishNow()
         */
        @Override
        public void finishNow() {
            ActivityForInternalTestsPush.this.doFinishNow();
        }
    };
    /** Listener to finish in same time that this activity */
    private IFinishListener finishWithMe;
    /** Message to push */
    private NdefMessage message;
    /** Intent to send when return from writing and get back as read */
    private PendingIntent pendingIntent;

    /**
     * Finish the activity now
     */
    void doFinishNow() {
        if (ActivityForInternalTestsPush.DEBUG) {
            Log.d(ActivityForInternalTestsPush.TAG, "Good bye : ActivityForInternalTestsPush !");
        }

        try {
            this.finishWithMe.finishNow();
        } catch (final Exception exception) {
            if (ActivityForInternalTestsPush.DEBUG) {
                Log.d(ActivityForInternalTestsPush.TAG, "Finish other failed !", exception);
            }
        }

        this.finish();
    }

    /**
     * Call when activity created <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param savedInstanceState
     * @see android.app.Activity#onCreate(android.os.Bundle)
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final Intent callIntent = this.getIntent();
        this.finishWithMe = IFinishListener.Stub.asInterface(callIntent
                .getIBinderExtra(TestUtil.EXTRA_FINISH_LISTENER));

        final Intent intent = new Intent(this, ActivityForInternalTestsResultP2P.class);
        intent.putExtra(TestUtil.EXTRA_FINISH_LISTENER, this.finishListener);
        intent.putExtra(ActivityForInternalTestsResultP2P.EXTRA_TRANSFER_TAG,
                callIntent.getParcelableExtra(NfcAdapter.EXTRA_TAG));

        this.pendingIntent = PendingIntent.getActivity(this, 0, intent,
                Intent.FLAG_ACTIVITY_NEW_TASK);
    }

    /**
     * Call when activity go on pause or destroy <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see android.app.Activity#onPause()
     */
    @Override
    protected void onPause() {
        NfcAdapter.getDefaultAdapter(this).disableForegroundNdefPush(this);
        NfcAdapter.getDefaultAdapter(this).disableForegroundDispatch(this);

        super.onPause();
    }

    /**
     * Call when activity resume or create <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see android.app.Activity#onResume()
     */
    @Override
    protected void onResume() {
        super.onResume();
        if (this.message == null) {
            try {
                this.message = TestUtil.createAndroidMessage(ActivityForInternalTestsPush.PAYLOAD);
            } catch (final Exception exception) {
                if (ActivityForInternalTestsPush.DEBUG) {
                    Log.d(ActivityForInternalTestsPush.TAG, "Create message failed !", exception);
                }
            }
        }

        NfcAdapter.getDefaultAdapter(this).enableForegroundNdefPush(this, this.message);
        NfcAdapter.getDefaultAdapter(this)
                .enableForegroundDispatch(this, this.pendingIntent, null, null);
    }
}
