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

import org.opennfc.HelperForNfc;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.XmlResourceParser;
import android.util.Log;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.locks.ReentrantLock;

/**
 * List of registered activities
 * 
 * @hide
 */
class RegisteredPackageList extends ReentrantLock {
    /**
     * Filter information
     * 
     * @hide
     */
    static class FilterInfo implements Comparable<FilterInfo> {
        /** Action name */
        final String mAction;
        /** Meta data name linked to action */
        final String mMetaData;

        /**
         * Create filter
         * 
         * @param action Action name
         * @param metaData Meta data name linked to action
         */
        FilterInfo(final String action, final String metaData) {
            this.mAction = action;
            this.mMetaData = metaData;
        }

        /**
         * Compare with an other filter.It returns :
         * <table>
         * <tr>
         * <th>0</th>
         * <td>If an other is the same filter</td>
         * </tr>
         * <tr>
         * <th>&lt; 0</th>
         * <td>If the other is after this filter</td>
         * </tr>
         * <tr>
         * <th>&gt; 0</th>
         * <td>If the other is before this filter</td>
         * </tr>
         * </table>
         * <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param another Filter to compare
         * @return the comparison result
         * @see Comparable#compareTo(Object)
         */
        @Override
        public int compareTo(final FilterInfo another) {
            final int comp = this.mAction.compareTo(another.mAction);
            if (comp == 0) {
                return this.mMetaData.compareTo(another.mMetaData);
            }
            return comp;
        }

        /**
         * Indicates if an object is equivalent to this filter<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param obj Object to compare
         * @return {@code true} if object is equivalent
         * @see Object#equals(Object)
         */
        @Override
        public boolean equals(final Object obj) {
            if (this == obj) {
                return true;
            }

            if (obj == null) {
                return false;
            }

            if (this.getClass() != obj.getClass()) {
                return false;
            }

            return this.compareTo((FilterInfo) obj) == 0;
        }

        /**
         * Compute filter hash code
         * 
         * @return Filter hash code
         */
        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((this.mAction == null)
                    ? 0
                    : this.mAction.hashCode());
            result = prime * result + ((this.mMetaData == null)
                    ? 0
                    : this.mMetaData.hashCode());
            return result;
        }

        /**
         * String representation of the filter <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return String representation
         * @see java.lang.Object#toString()
         */
        @Override
        public String toString() {
            return new StringBuilder("Filter(Action=").append(this.mAction).append(" | meta-data=")
                    .append(this.mMetaData).append(')').toString();
        }
    }

    /**
     * Registered information for launch activity
     */
    static class RegisteredInfo {
        /** Activity description */
        final ResolveInfo mResolveInfo;
        /** Targeted technologies */
        final String[] mTechnologies;

        /**
         * Create registered information
         * 
         * @param resolveInfo Describe activity
         * @param techonologies Targeted technologies
         */
        public RegisteredInfo(final ResolveInfo resolveInfo, final String[] techonologies) {
            this.mResolveInfo = resolveInfo;
            this.mTechnologies = techonologies;

            if (RegisteredPackageList.DEBUG) {
                Log.d(RegisteredPackageList.TAG, "Create " + this);
            }
        }

        /**
         * String representation <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return String representation
         * @see java.lang.Object#toString()
         */
        @Override
        public String toString() {
            final StringBuilder stringBuilder = new StringBuilder("Registered(Launch=");
            stringBuilder.append(this.mResolveInfo.activityInfo.packageName);
            stringBuilder.append(" | Tech=");

            for (final String tech : this.mTechnologies) {
                stringBuilder.append(tech);
                stringBuilder.append(" ; ");
            }

            stringBuilder.append(')');

            return stringBuilder.toString();
        }
    }

    /** Debug enable/disable */
    private static final boolean DEBUG = true;
    /** Markup "tech" in parsing the XML technologies list */
    private static final String MARKUP_TECH = "tech";

    /** Markup "tech-list" in parsing the XML technologies list */
    private static final String MARKUP_TECH_LIST = "tech-list";
    /** List of registered packages */
    static RegisteredPackageList registeredPacakgeList;

    /** TAG for debugging */
    private static final String TAG = RegisteredPackageList.class.getSimpleName();
    /** Android context to use */
    private final Context mContext;

    /** Map of registered information */
    private final HashMap<FilterInfo, ArrayList<RegisteredInfo>> mHashMapRegistered;

    /**
     * Create the package list
     * 
     * @param context Context to use
     */
    RegisteredPackageList(final Context context) {
        RegisteredPackageList.registeredPacakgeList = this;

        this.mContext = context;
        this.mHashMapRegistered = new HashMap<RegisteredPackageList.FilterInfo, ArrayList<RegisteredPackageList.RegisteredInfo>>();
    }

    /**
     * Collect registered action.<br>
     * To registered see {@link #registerFilter(String, String)}
     */
    public void collectPackages() {
        this.lock();
        try {
            ArrayList<RegisteredInfo> arrayList;
            final PackageManager packageManager = this.mContext.getPackageManager();
            final Intent intent = new Intent();

            for (final FilterInfo filterInfo : this.mHashMapRegistered.keySet()) {
                if (RegisteredPackageList.DEBUG) {
                    Log.d(RegisteredPackageList.TAG, "Filter to detect : " + filterInfo);
                }

                arrayList = this.mHashMapRegistered.get(filterInfo);
                arrayList.clear();

                intent.setAction(filterInfo.mAction);

                for (final ResolveInfo resolveInfo : packageManager.queryIntentActivities(intent,
                        PackageManager.GET_META_DATA)) {
                    if (RegisteredPackageList.DEBUG) {
                        Log.d(RegisteredPackageList.TAG, "Tested package : "
                                + resolveInfo.activityInfo.packageName);
                    }

                    try {
                        this.parseRegisteredInfo(packageManager, filterInfo, resolveInfo, arrayList);
                    } catch (final XmlPullParserException exception) {
                        if (RegisteredPackageList.DEBUG) {
                            Log.d(RegisteredPackageList.TAG, "Error in parsing XML", exception);
                        }
                    } catch (final IOException exception) {
                        if (RegisteredPackageList.DEBUG) {
                            Log.d(RegisteredPackageList.TAG, "Input/Output error", exception);
                        }
                    }
                }
            }
        } finally {
            this.unlock();
        }
    }

    /**
     * Give the list of activities registered for a specific action, meta data
     * and technologies
     * 
     * @param action Action name
     * @param metaData Meta data name
     * @param tagTechs Technologies received
     * @return List of activities that match
     */
    ArrayList<ResolveInfo> listOfActivities(final String action, final String metaData,
                final String[] tagTechs) {
        final FilterInfo filterInfo = new FilterInfo(action, metaData);
        final ArrayList<RegisteredInfo> arrayList = this.mHashMapRegistered.get(filterInfo);

        if (arrayList == null) {
            return null;
        }

        final ArrayList<ResolveInfo> list = new ArrayList<ResolveInfo>();

        for (final RegisteredInfo registeredInfo : arrayList) {
            if (list.contains(registeredInfo.mResolveInfo) == false
                        && HelperForNfc.doesArrayInclude(tagTechs, registeredInfo.mTechnologies) == true) {
                list.add(registeredInfo.mResolveInfo);
            }
        }

        return list;
    }

    /**
     * Parse registered information inside activity information
     * 
     * @param packageManager Package manager to use
     * @param filterInfo Filter to associate
     * @param info Information to parse
     * @param registeredInfos List to fill
     * @throws XmlPullParserException If XML is illegal
     * @throws IOException On reading issue
     */
    void parseRegisteredInfo(final PackageManager packageManager, final FilterInfo filterInfo,
            final ResolveInfo info, final ArrayList<RegisteredInfo> registeredInfos)
            throws XmlPullParserException, IOException {
        final ActivityInfo activityInfo = info.activityInfo;

        final XmlResourceParser parser = activityInfo.loadXmlMetaData(packageManager,
                filterInfo.mMetaData);
        if (parser == null) {
            throw new XmlPullParserException("No " + filterInfo.mMetaData + " meta-data");
        }

        this.parseTechLists(parser, info, registeredInfos);

        parser.close();
    }

    /**
     * Parse XML file that describe a technologies list
     * 
     * @param parser XML parser
     * @param resolveInfo Activity description
     * @param components List to fill
     * @throws XmlPullParserException On XML parsing issue
     * @throws IOException On reading issue
     */
    void parseTechLists(final XmlPullParser parser, final ResolveInfo resolveInfo,
            final ArrayList<RegisteredInfo> components) throws XmlPullParserException, IOException {
        int eventType = parser.getEventType();
        while (eventType != XmlPullParser.START_TAG) {
            eventType = parser.next();
        }

        final ArrayList<String> items = new ArrayList<String>();
        String tagName;
        eventType = parser.next();
        while (eventType != XmlPullParser.END_DOCUMENT) {
            tagName = parser.getName();

            if (eventType == XmlPullParser.START_TAG
                    && RegisteredPackageList.MARKUP_TECH.equals(tagName) == true) {
                items.add(parser.nextText());
            } else if (eventType == XmlPullParser.END_TAG
                    && RegisteredPackageList.MARKUP_TECH_LIST.equals(tagName) == true) {
                final int size = items.size();
                if (size > 0) {
                    components
                            .add(new RegisteredInfo(resolveInfo, items.toArray(new String[size])));
                    items.clear();
                }
            }

            eventType = parser.next();
        }

        if (RegisteredPackageList.DEBUG) {
            Log.d(RegisteredPackageList.TAG, "ADDED !");
        }
    }

    /**
     * Register an action associated with a meta data to collect
     * 
     * @param action Action name
     * @param metaData Meta data name
     */
    public void registerFilter(final String action, final String metaData) {
        this.lock();
        try {
            final FilterInfo filterInfo = new FilterInfo(action, metaData);
            final ArrayList<RegisteredInfo> arrayList = this.mHashMapRegistered.get(filterInfo);

            if (arrayList == null) {
                this.mHashMapRegistered.put(filterInfo,
                        new ArrayList<RegisteredPackageList.RegisteredInfo>());
            }
        } finally {
            this.unlock();
        }
    }
}
