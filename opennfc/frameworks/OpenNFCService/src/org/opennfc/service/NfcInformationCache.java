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

import org.opennfc.ConvertInformationNfc;
import org.opennfc.nfctag.NfcTagConnection;

import android.util.Log;

import java.util.concurrent.ConcurrentHashMap;

/**
 * Cache for {@link NfcInformation}.<br>
 * It manages also a "garbage collector" to clear blocked information
 */
public class NfcInformationCache {
    /** Cache single instance */
    private static final NfcInformationCache CACHE;
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Initial cache size */
    private static final int INITIAL_SIZE = 16;
    /** TAG used in debug */
    private static final String TAG = NfcInformationCache.class.getSimpleName();

    static {
        CACHE = new NfcInformationCache();
    }

    /**
     * Get cache single instance
     * 
     * @return Cache instance
     */
    public static NfcInformationCache getCache() {
        return NfcInformationCache.CACHE;
    }

    /**
     * Cache itself
     */
    final ConcurrentHashMap<Integer, NfcInformation> cache;

    /**
     * Create cache instance
     */
    private NfcInformationCache() {
        this.cache = new ConcurrentHashMap<Integer, NfcInformation>(
                NfcInformationCache.INITIAL_SIZE);
    }

    /**
     * Add connection to the cache (The information is automatically created)
     * 
     * @param nfcTagConnection Connection for communicate with the tag
     * @param convertInformationNfc Converted information from the tag
     */
    public void add(final NfcTagConnection nfcTagConnection,
            final ConvertInformationNfc convertInformationNfc) {
        final int id = convertInformationNfc.handle;

        if (NfcInformationCache.DEBUG) {
            Log.d(NfcInformationCache.TAG, "ADD  " + id);
        }

        // Create the information
        final NfcInformation nfcInformation = new NfcInformation(id, nfcTagConnection,
                convertInformationNfc.technologies);
        nfcInformation.overideID = convertInformationNfc.overrideID;

        // Add the created information to the cache
        this.cache.put(id, nfcInformation);

        nfcInformation.intializeTime();
    }

    /**
     * Clear the cache
     */
    public void clear() {
        this.cache.clear();
    }

    /**
     * Obtain an information
     * 
     * @param id Information ID
     * @return The information
     */
    public NfcInformation get(final int id) {
        return this.cache.get(id);
    }

    /**
     * Remove an information
     * 
     * @param id Information ID
     */
    public void remove(final int id) {
        Log.d(NfcInformationCache.TAG, "Will remove");
        final NfcInformation nfcInformation = this.cache.remove(id);
        Log.d(NfcInformationCache.TAG, "removed");

        if (nfcInformation != null) {
            Log.d(NfcInformationCache.TAG, "May close : nfcInformation.connectedStatus="
                    + nfcInformation.connectedStatus);
            if (nfcInformation.connectedStatus != NfcInformation.STATUS_DISCONNECTED) {
                Log.d(NfcInformationCache.TAG, "Will close");
                nfcInformation.mNfcTagConnection.close();
                Log.d(NfcInformationCache.TAG, "Closed");
            }

            nfcInformation.connectedStatus = NfcInformation.STATUS_DISCONNECTED;

            nfcInformation.mNfcTagConnection = null;
            nfcInformation.techList = null;
        }

        if (NfcInformationCache.DEBUG) {
            Log.d(NfcInformationCache.TAG, "CACHE SIZE = " + this.cache.size());
        }
    }
}
