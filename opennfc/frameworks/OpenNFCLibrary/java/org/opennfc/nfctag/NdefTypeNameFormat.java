/*
 * Copyright (c) 2010 Inside Secure, All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.opennfc.nfctag;

/**
 * The different NDEF record types as defined in
 * <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
 *
 * @since Open NFC 4.0
 */
public enum NdefTypeNameFormat {
    /**
     * Empty Record with no type or payload associated.
     */
    EMPTY(ConstantAutogen.W_NDEF_TNF_EMPTY),

    /**
     * NFC Forum well-known type.
     */
    WELL_KNOWN(ConstantAutogen.W_NDEF_TNF_WELL_KNOWN),

    /**
     * Media-type as defined in RFC 2046.
     */
    MEDIA(ConstantAutogen.W_NDEF_TNF_MEDIA),

    /**
     * Absolute URI as defined in RFC 3986.
     */
    ABSOLUTE_URI(ConstantAutogen.W_NDEF_TNF_ABSOLUTE_URI),

    /**
     * NFC Forum external type.
     */
    EXTERNAL(ConstantAutogen.W_NDEF_TNF_EXTERNAL),

    /**
     * This value is used to indicate that the type of the payload unknown.
     */
    UNKNOWN(ConstantAutogen.W_NDEF_TNF_UNKNOWN),

    /**
     * This value is used in all middle record chunks and the terminating record
     * chunk used in chunked payloads.
     */
    UNCHANGED(ConstantAutogen.W_NDEF_TNF_UNCHANGED),

    /**
     * This value is used with the card listener method to read every message
     * regardless of their type.
     */
    ANY_TYPE(ConstantAutogen.W_NDEF_TNF_ANY_TYPE);

    private int mIdentifier;

    /**
     * private constructor to prevent instantiation
     */
    private NdefTypeNameFormat(int identifier) {
        this.mIdentifier = identifier;
    }

    /**
     * Returns the type value defined in the NDEF specification.
     *
     * @return the type value.
     **/
    public int getValue() {
        return this.mIdentifier;
    }
}
