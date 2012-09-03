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

package org.opennfc.service;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * Manage broadcast intent.<br>
 * It is called when phone boot completed or when action adapter state change
 */
public class OpenNFCBroadcastReceiver extends BroadcastReceiver {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = OpenNFCBroadcastReceiver.class.getSimpleName();

    /**
     * constructor.
     */
    public OpenNFCBroadcastReceiver() {
    }

    /**
     * Call when event received, we listen here the booting and the packages
     * install, modify and remove.<br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param context Android context
     * @param intent Intent that wakeup the receiver
     * @see android.content.BroadcastReceiver#onReceive(android.content.Context,
     *      android.content.Intent)
     */
    @Override
    public void onReceive(final Context context, final Intent intent) {
        final String intentAction = intent.getAction();
        if (OpenNFCBroadcastReceiver.DEBUG) {
            Log.d(OpenNFCBroadcastReceiver.TAG,
                    "onReceive: a=" + intentAction + " " + intent.toString());
        }

        if (Intent.ACTION_BOOT_COMPLETED.equals(intentAction) == true) {
            if (OpenNFCBroadcastReceiver.DEBUG) {
                Log.v(OpenNFCBroadcastReceiver.TAG, "OpenNFCServiceConnection start Service");
            }

            // On boot, we start the Open NFC service
            final Intent serviceLauncher = new Intent(context, OpenNFCService.class);
            context.startService(serviceLauncher);
        } else if (Intent.ACTION_PACKAGE_ADDED.equals(intentAction) == true
                || Intent.ACTION_PACKAGE_CHANGED.equals(intentAction) == true
                || Intent.ACTION_PACKAGE_REMOVED.equals(intentAction) == true) {
            final Intent serviceLauncher = new Intent(context, OpenNFCService.class);
            serviceLauncher.putExtra(OpenNFCService.REFRESH_PACKAGE_LIST, true);

            context.startService(serviceLauncher);
        } else if (Intent.ACTION_SCREEN_ON.equals(intentAction) == true) {
            if (OpenNFCBroadcastReceiver.DEBUG) {
                Log.d(OpenNFCBroadcastReceiver.TAG, "#########################");
                Log.d(OpenNFCBroadcastReceiver.TAG, "### ***************** ###");
                Log.d(OpenNFCBroadcastReceiver.TAG, "### *** SCREEN ON *** ###");
                Log.d(OpenNFCBroadcastReceiver.TAG, "### ***************** ###");
                Log.d(OpenNFCBroadcastReceiver.TAG, "#########################");
            }
        } else if (Intent.ACTION_SCREEN_OFF.equals(intentAction) == true) {
            if (OpenNFCBroadcastReceiver.DEBUG) {
                Log.d(OpenNFCBroadcastReceiver.TAG, "##########################");
                Log.d(OpenNFCBroadcastReceiver.TAG, "### ****************** ###");
                Log.d(OpenNFCBroadcastReceiver.TAG, "### *** SCREEN OFF *** ###");
                Log.d(OpenNFCBroadcastReceiver.TAG, "### ****************** ###");
                Log.d(OpenNFCBroadcastReceiver.TAG, "##########################");
            }
        }
    }
}
