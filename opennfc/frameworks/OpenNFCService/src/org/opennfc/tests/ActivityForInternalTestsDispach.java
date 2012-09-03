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
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.util.Log;

/**
 * Activity for dispach tag on first detection
 */
public class ActivityForInternalTestsDispach extends Activity {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = ActivityForInternalTestsDispach.class.getSimpleName();
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
            ActivityForInternalTestsDispach.this.doFinishNow();
        }
    };
    /** Intent to send when tag first detect */
    private PendingIntent pendingIntent;

    /**
     * Finish Activity now
     */
    void doFinishNow() {
        if (ActivityForInternalTestsDispach.DEBUG) {
            Log.d(ActivityForInternalTestsDispach.TAG,
                    "Good bye : ActivityForInternalTestsDispach !");
        }

        this.finish();
    }

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

        final Intent intent = new Intent(this, ActivityForInternalTestsPush.class);
        intent.putExtra(TestUtil.EXTRA_FINISH_LISTENER, this.finishListener);

        this.pendingIntent = PendingIntent.getActivity(this, 0, intent,
                Intent.FLAG_ACTIVITY_NEW_TASK);
    }

    /**
     * Call when Activity pause or destroy <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see android.app.Activity#onPause()
     */
    @Override
    protected void onPause() {
        NfcAdapter.getDefaultAdapter(this).disableForegroundDispatch(this);

        super.onPause();
    }

    /**
     * Call when Activity resume or create <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see android.app.Activity#onResume()
     */
    @Override
    protected void onResume() {
        super.onResume();

        NfcAdapter.getDefaultAdapter(this)
                .enableForegroundDispatch(this, this.pendingIntent, null, null);
    }
}
