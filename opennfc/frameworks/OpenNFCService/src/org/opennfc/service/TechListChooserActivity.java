/*
 * Copyright (C) 2008 The Android Open Source Project
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

import com.android.internal.app.ResolverActivity;

import android.content.Intent;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.os.Parcelable;
import android.util.Log;

import java.util.ArrayList;

/**
 * Activity for choose the activity to launch
 */
public class TechListChooserActivity extends ResolverActivity {
    /** Extra to carry the activity list to launch */
    public static final String EXTRA_RESOLVE_INFOS = "rlist";

    /**
     * Call when activity is created. It parse the extra
     * {@link #EXTRA_RESOLVE_INFOS} to know the list of activities to show. It
     * parse the extra {@link Intent#EXTRA_INTENT} to know the intent to send to
     * the chosen activity. <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param savedInstanceState Unused
     * @see com.android.internal.app.ResolverActivity#onCreate(android.os.Bundle)
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        // Get the intent that launch this activity
        final Intent intent = this.getIntent();

        // Extract the intent to send
        final Parcelable targetParcelable = intent.getParcelableExtra(Intent.EXTRA_INTENT);
        if (!(targetParcelable instanceof Intent)) {
            Log.w("TechListChooserActivity", "Target is not an intent: " + targetParcelable);
            this.finish();
            return;
        }

        // Extract the activities list
        final Intent target = (Intent) targetParcelable;
        final ArrayList<ResolveInfo> rList = intent
                .getParcelableArrayListExtra(TechListChooserActivity.EXTRA_RESOLVE_INFOS);

        // Activity title
        final CharSequence title = this.getResources().getText(
                com.android.internal.R.string.chooseActivity);

        // Create Activity with the extracted information
        super.onCreate(savedInstanceState, target, title, null, rList, false);
    }
}
