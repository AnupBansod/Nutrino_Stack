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

import org.opennfc.NfcErrorCode;
import org.opennfc.NfcException;

/**
 * The TextRecord class represents a record of type Text.
 *
 * See <a href="{@docRoot}overview-summary.html#ndef_text_record">Text Record</a>
 * for the details on the Text encoding in the NDEF record.
 *
 * @since Open NFC 4.0
 */
public final class TextRecord extends NdefRecord {

    /** Comparison result: The primary language matches exactly the language of the record. */
    public static final int TEXT_MATCH_1 = ConstantAutogen.W_RTD_TEXT_MATCH_1;

    /** Comparison result: The primary language matches partially the language of the record. */
    public static final int TEXT_PARTIAL_MATCH_1 = ConstantAutogen.W_RTD_TEXT_PARTIAL_MATCH_1;

    /** Comparison result: The secondary language matches exactly the language of the record. */
    public static final int TEXT_MATCH_2 = ConstantAutogen.W_RTD_TEXT_MATCH_2;

    /** Comparison result: The secondary language matches partially the language of the record. */
    public static final int TEXT_PARTIAL_MATCH_2 = ConstantAutogen.W_RTD_TEXT_PARTIAL_MATCH_2;

    /** Comparison result: The languages do not match the language of the record. */
    public static final int TEXT_NO_MATCH_FOUND = ConstantAutogen.W_RTD_TEXT_NO_MATCH_FOUND;

    /**
     * The Text type
     * @hide
     */
    private static final String TYPE_TEXT = "T";

    /**
     * Creates a new Text record.
     *
     * @param language the language of the text as defined in
     *   <a href="{@docRoot}overview-summary.html#rtd_text_language_format">Language Format</a>.
     * @param useUtf8 use UTF8 encoding for the text of the record
     * @param text the text of the record
     * @throws NfcException
     * @throws IllegalArgumentException if <code>language</code> or <code>text</code> is null.
     */
    public TextRecord(String language, boolean useUtf8, String text) throws NfcException {

        if ((language == null) || (text == null)) {
            throw new IllegalArgumentException("language or text = null");
        }

        int languageLength = language.length();
        if ((languageLength < 2) || (languageLength > 63)) {
            throw new RuntimeException(new NfcException("Error creating the record",
                    NfcErrorCode.BAD_PARAMETER));
        }

        int totalLengthInByte = 1 + languageLength;
        byte[] textUtf8Bytes = null;
        if (useUtf8) {
            textUtf8Bytes = Helper.convertUTF16ToUTF8(text);
            totalLengthInByte += textUtf8Bytes.length;
        } else {
            if ((languageLength & 1) == 0) {
                totalLengthInByte++;
            }
            totalLengthInByte += text.length();
        }

        byte[] buffer = new byte[totalLengthInByte];
        int index = 0;

        if ((!useUtf8) && ((languageLength & 1) == 0)) {
            index++;
        }
        buffer[index] = (byte) languageLength;
        if (!useUtf8) {
            buffer[index] |= 0x80;
        }
        index++;
        char[] l = language.toCharArray();
        for (int i = 0; i < languageLength; i++) {
            buffer[index++] = (byte) l[i];
        }
        if (!useUtf8) {
            System.arraycopy(text, 0, buffer, index, text.length());
        } else {
            System.arraycopy(textUtf8Bytes, 0, buffer, index, textUtf8Bytes.length);
        }
        /* set the Ndef Record attributes */
        setNdefRecord(new NdefRecord(NdefTypeNameFormat.WELL_KNOWN, TYPE_TEXT, buffer));
    }

    /**
     * Creates a new Text record from another record.
     *
     * @param record the record value.
     *
     * @throws IllegalArgumentException if <code>record</code> is null.
     * @throws NfcException if <code>record</code> is not of type Text.
     */
    public TextRecord(NdefRecord record) throws NfcException {

        if (record == null) {
            throw new IllegalArgumentException("record = null");
        }
        if (record.checkType(NdefTypeNameFormat.WELL_KNOWN, TYPE_TEXT) == false) {
            throw new NfcException("Record is not of type Text", NfcErrorCode.BAD_NDEF_FORMAT);
        }
        /* set the Ndef Record attributes */
        setNdefRecord(new NdefRecord(NdefTypeNameFormat.WELL_KNOWN, TYPE_TEXT, record.getPayload()));
        /* check the Text formating */
        privateGetText();
    }

    /**
     * Checks if a record is a Text record.
     *
     * @param record  the record to check.
     * @return true if the record is of type Text, false otherwise.
     * @throws IllegalArgumentException if <code>record</code> is null.
     **/
    public static boolean isTextRecord(NdefRecord record) {

        if (record == null) {
            throw new IllegalArgumentException("record = null");
        }

        return record.checkType(NdefTypeNameFormat.WELL_KNOWN, TYPE_TEXT);
    }

    /**
     * Returns the language of a Text record.
     *
     * @return the language string as defined in
     *   <a href="{@docRoot}overview-summary.html#rtd_text_language_format">Language Format</a>.
     */
    public String getLanguage() {

        byte[] payload = this.getPayload();
        int langLen = payload[0] & 0x1F;
        byte[] lang = new byte[langLen];
        System.arraycopy(payload, 1, lang, 0, langLen);

        return Helper.bytesToString(lang, 0, langLen);
    }

    /**
     * Returns the value of the text.
     *
     * @return the value of the text.
     */
    public String getText() {

        try {
            return privateGetText();
        } catch (NfcException e) {
        }

        return null;
    }

    /**
     * Gets the value of the text in a Text record.
     *
     * @hide
     * @return the value of the text.
     * @throws NfcException if <code>record</code> is not of type Text.
     */
    private String privateGetText() throws NfcException {

        byte[] payload = this.getPayload();
        int langLen = payload[0] & 0x1F;
        int textLen = payload.length - (1 + langLen);
        byte[] textTmp = new byte[textLen];
        System.arraycopy(payload, (1 + langLen), textTmp, 0, textLen);
        String text = null;
        if ((payload[0] & 0x80) == 0) {
            text = Helper.convertUTF8ToUTF16(textTmp);
        } else {
            text = Helper.bytesToString(textTmp, 0, textLen);
        }
        return text;
    }

    /**
     * Checks if a text record matches a language.
     *
     * The comparison algorithm is defined in
     * <a href="{@docRoot}overview-summary.html#rtd_text_language_match">Language Matching</a>.
     *
     * @param language the primary language as defined in
     *   <a href="{@docRoot}overview-summary.html#rtd_text_language_format">Language Format</a>.
     * @return the result code of the comparison (See <a href="{@docRoot}overview-summary.html#rtd_text_language_match">Language Matching</a>).
     * @throws IllegalArgumentException if <code>langage</code> is null.
     */
    public int checkLanguage(String language) {

        if (language == null) {
            throw new IllegalArgumentException("language = null");
        }

        String lang = this.getLanguage();
        /* Compare Language */
        switch (TextMatch(lang, language)) {
            case 0:
                return TEXT_MATCH_1;

            case 1:
                return TEXT_PARTIAL_MATCH_1;
        }

        return TEXT_NO_MATCH_FOUND;

    }

    /**
     * Checks if a text record matches a language.
     *
     * The comparison algorithm is defined in
     * <a href="{@docRoot}overview-summary.html#rtd_text_language_match">Language Matching</a>.
     *
     * @param language1 the primary language as defined in
     *   <a href="{@docRoot}overview-summary.html#rtd_text_language_format">Language Format</a>.
     * @param language2 the secondary language as defined in
     *   <a href="{@docRoot}overview-summary.html#rtd_text_language_format">Language Format</a>.
     * @return the result code of the comparison (See <a href="{@docRoot}overview-summary.html#rtd_text_language_match">Language Matching</a>).
     * @throws IllegalArgumentException if <code>langage1</code> or <code>langage2</code> is null.
     */
    public int checkLanguage(String language1, String language2) {

        if ((language1 == null) || (language2 == null)) {
            throw new IllegalArgumentException("language1 or language2 = null");
        }

        String lang = this.getLanguage();
        /* Compare Language 1 */
        switch (TextMatch(lang, language1)) {
            case 0:
                return TEXT_MATCH_1;

            case 1:
                return TEXT_PARTIAL_MATCH_1;
        }
        /* Compare Language 2 */
        switch (TextMatch(lang, language2)) {
            case 0:
                return TEXT_MATCH_2;

            case 1:
                return TEXT_PARTIAL_MATCH_2;
        }

        return TEXT_NO_MATCH_FOUND;
    }

    /**
     * Checks if two languages match
     * @param language
     * @param language1
     * @return     0 if totally match
     *             1 if partially match
     *             2 if no match
     */

    private int TextMatch(String language, String language1) {

        if (language.compareTo(language1) == 0)
            return 0;

        if (language.startsWith(language1))
            return 1;

        if (language1.startsWith(language))
            return 1;

        return 2;
    }
}
