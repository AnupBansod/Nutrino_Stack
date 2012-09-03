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

import java.io.ByteArrayOutputStream;

import org.opennfc.NfcErrorCode;
import org.opennfc.NfcException;

/**
 * The NdefRecord class represents a NDEF Record.
 *
 * @since Open NFC 4.0
 */
public class NdefRecord {

    private NdefTypeNameFormat mTnf = NdefTypeNameFormat.UNKNOWN;

    private int mTypeLength = 0;

    private byte[] mType = null;

    private String mTypeS = null;

    private byte[] mIdentifier = null;

    private byte[] mPayload = null;

    /**
     * Sets the record.
     *
     * package private
     *
     * @param handle the record handle.
     * @hide
     */
    void setNdefRecord(NdefRecord record) {
        this.mTnf = record.getTNF();
        this.mType = record.getAsciiType();
        this.mTypeS = record.getType();
        this.mIdentifier = record.getBinaryIdentifier();
        this.mPayload = record.getPayload();
    }

    /**
     * package private
     *
     * @hide
     */
    NdefRecord() {
    }

    /**
     * Creates a new record.
     *
     * @param tnf  the record <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     * @param type  the type string encoded in printable URI format as
     *              defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *              This value may be null.
     * @param payload  the record payload.
     * @throws IllegalArgumentException if <code>tnf</code> is null.
     * @throws NfcException if parameters <code>tnf</code>, <code>type</code>
     *         or <code>payload</code> are not consistent.
     **/
    public NdefRecord(NdefTypeNameFormat tnf, String type, byte[] payload) throws NfcException {
        this(tnf, type, payload, 0, (payload == null) ? 0 : payload.length);
    }

    /**
     * Creates a new record.
     *
     * @param tnf  the record <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     * @param type  the type string encoded in printable URI format as
     *              defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *              This value may be null.
     * @param payload  the record payload.
     * @param offset  the offset of the first byte of the payload in the buffer.
     * @param length  the length of the payload in bytes.
     * @throws IllegalArgumentException if <code>tnf</code> is null.
     * @throws IllegalArgumentException if <code>offset</code> or <code>length</code> is negative.
     * @throws IndexOutOfBoundsException  if <code>offset</code> or <code>length</code> are out of the array.
     * @throws NfcException if parameters <code>tnf</code>, <code>type</code>
     *         or <code>payload</code> are not consistent.
     **/
    public NdefRecord(NdefTypeNameFormat tnf, String type, byte[] payload, int offset, int length)
            throws NfcException {

        /* Check Parameters */
        if (tnf == null) {
            throw new IllegalArgumentException("tnf = null");
        }

        if ((offset < 0) || (length < 0)) {
            throw new IllegalArgumentException("negative offset or length");
        }

        if (((payload == null) && (length != 0))) {
            throw new NfcException("The tnf, type and payload are not consistent",
                    NfcErrorCode.BAD_NDEF_FORMAT);
        }

        if (((tnf == NdefTypeNameFormat.EMPTY) || (tnf == NdefTypeNameFormat.UNKNOWN) || (tnf == NdefTypeNameFormat.UNCHANGED)
        /*|| (tnf == NdefTypeNameFormat.ANY_TYPE)*/)
                && (type != null && type.length() == 0)) {
            throw new NfcException("The tnf, type and payload are not consistent",
                    NfcErrorCode.BAD_NDEF_FORMAT);
        }

        int maxLength = (payload == null) ? 0 : payload.length;

        if ((offset + length) > maxLength) {
            throw new IndexOutOfBoundsException();
        }

        /* Check TYPE */
        if (type != null) {

            if (tnf == NdefTypeNameFormat.WELL_KNOWN) {
                if (!Helper.isWellKnownCompatible(type)) {
                    throw new NfcException(
                            "The type must used only characters in the range [0x20-0x7E]",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            } else if (tnf == NdefTypeNameFormat.EXTERNAL) {
                if (!Helper.isExternalCompatible(type)) {
                    throw new NfcException(
                            "The type must used only characters in the range [0x20-0x7E]",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            } else {
                /* For other types, the character range is limited to [0x0000-0x00FF] (8 bit compatible) */
                if (!Helper.is8BitsCompatible(type)) {
                    throw new NfcException(
                            "The type must used only characters in the range [0x00-0xFF]",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            }

            /* More processing according to the NDEF type */
            if (tnf == NdefTypeNameFormat.WELL_KNOWN) {
                /* remove the "urn:nfc:wkt:" prefix if present */
                if (type.startsWith("urn:nfc:wkt:")) {
                    type = type.substring(12);
                }
            } else if (tnf == NdefTypeNameFormat.EXTERNAL) {
                /* remove the "urn:nfc:ext:" prefix if present */
                if (type.startsWith("urn:nfc:ext:")) {
                    type = type.substring(12);
                }
            } else if (tnf == NdefTypeNameFormat.ABSOLUTE_URI) {
                if (!Helper.isPrintableURI(type)) {
                    throw new NfcException("The type must contain a printable URI",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            }

            /* Ensure the length of the type string fits in a byte */
            mTypeLength = type.length();
            if (mTypeLength > 255) {
                throw new NfcException("The type length must be <= 255",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        } else {
            mTypeLength = 0;
        }

        /* Check Type Name Format (TNF) */
        if (tnf == NdefTypeNameFormat.EMPTY) {
            if ((mTypeLength != 0) || (length != 0) || (payload != null && payload.length == 0)) {
                throw new NfcException("The tnf, type and payload are not consistent",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        } else if ((tnf == NdefTypeNameFormat.WELL_KNOWN) || (tnf == NdefTypeNameFormat.MEDIA)
                || (tnf == NdefTypeNameFormat.ABSOLUTE_URI) || (tnf == NdefTypeNameFormat.EXTERNAL)) {
            if (mTypeLength == 0) {
                throw new NfcException("The tnf, type and payload are not consistent",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        } else if ((tnf == NdefTypeNameFormat.UNKNOWN) || (tnf == NdefTypeNameFormat.UNCHANGED)) {
            if (mTypeLength != 0) {
                throw new NfcException("The tnf, type and payload are not consistent",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        }

        /* Every thing is OK */
        if (type != null) {
            this.mTypeS = type;
            this.mType = type.getBytes();
        } else {
            this.mTypeS = null;
            this.mType = null;
        }
        if (payload != null && length > 0) {
            /* The constructor must make a copy from the original data and payload. */
            this.mPayload = new byte[length];
            System.arraycopy(payload, offset, this.mPayload, 0, this.mPayload.length);
        } else {
            this.mPayload = null;
        }
        this.mTnf = tnf;
    }

    /**
     * Creates a new record with a nested message.
     *
     * @param tnf  the record <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     * @param type  the type string encoded in printable URI format as
     *              defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *              This value may be null.
     * @param nestedMessage  the message enclosed in the payload.
     * @throws IllegalArgumentException if <code>tnf</code> or <code>nestedMessage</code> is null.
     * @throws NfcException if parameters <code>tnf</code>, <code>type</code>
     *         or <code>nestedMessage</code> are not consistent.
     **/
    public NdefRecord(NdefTypeNameFormat tnf, String type, NdefMessage nestedMessage)
            throws NfcException {

        if (nestedMessage == null) {
            throw new IllegalArgumentException("nestedMessage = null");
        }
        if (tnf == null) {
            throw new IllegalArgumentException("tnf = null");
        }

        if (((tnf == NdefTypeNameFormat.EMPTY) || (tnf == NdefTypeNameFormat.UNKNOWN) || (tnf == NdefTypeNameFormat.UNCHANGED)
        /*|| (tnf == NdefTypeNameFormat.ANY_TYPE)*/)
                && (type != null && type.length() == 0)) {
            throw new NfcException("The tnf, type or nestedMessage are not consistent",
                    NfcErrorCode.BAD_NDEF_FORMAT);
        }

        /* Check TYPE */
        if (type != null) {

            if (tnf == NdefTypeNameFormat.WELL_KNOWN) {
                if (!Helper.isWellKnownCompatible(type)) {
                    throw new NfcException(
                            "The type must used only characters in the range [0x20-0x7E]",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            } else if (tnf == NdefTypeNameFormat.EXTERNAL) {
                if (!Helper.isExternalCompatible(type)) {
                    throw new NfcException(
                            "The type must used only characters in the range [0x20-0x7E]",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            } else {
                /* For other types, the character range is limited to [0x0000-0x00FF] (8 bit compatible) */
                if (!Helper.is8BitsCompatible(type)) {
                    throw new NfcException(
                            "The type must used only characters in the range [0x00-0xFF]",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            }

            /* More processing according to the NDEF type */
            if (tnf == NdefTypeNameFormat.WELL_KNOWN) {
                /* remove the "urn:nfc:wkt:" prefix if present */
                if (type.startsWith("urn:nfc:wkt:")) {
                    type = type.substring(12);
                }
            } else if (tnf == NdefTypeNameFormat.EXTERNAL) {
                /* remove the "urn:nfc:ext:" prefix if present */
                if (type.startsWith("urn:nfc:ext:")) {
                    type = type.substring(12);
                }
            } else if (tnf == NdefTypeNameFormat.ABSOLUTE_URI) {
                if (!Helper.isPrintableURI(type)) {
                    throw new NfcException("The type must contain a printable URI",
                            NfcErrorCode.BAD_NDEF_FORMAT);
                }
            }

            /* Ensure the length of the type string fits in a byte */
            mTypeLength = type.length();
            if (mTypeLength > 255) {
                throw new NfcException("The type length must be <= 255",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        } else {
            mTypeLength = 0;
        }

        /* Check Type Name Format (TNF) */
        if (tnf == NdefTypeNameFormat.EMPTY) {
            if ((mTypeLength != 0 || nestedMessage != null)) {
                throw new NfcException("The tnf, type or nestedMessage are not consistent",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        } else if ((tnf == NdefTypeNameFormat.WELL_KNOWN) || (tnf == NdefTypeNameFormat.MEDIA)
                || (tnf == NdefTypeNameFormat.ABSOLUTE_URI) || (tnf == NdefTypeNameFormat.EXTERNAL)) {
            if (mTypeLength == 0) {
                throw new NfcException("The tnf, type or nestedMessageare not consistent",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        } else if ((tnf == NdefTypeNameFormat.UNKNOWN) || (tnf == NdefTypeNameFormat.UNCHANGED)) {
            if (mTypeLength != 0) {
                throw new NfcException("The tnf, type or nestedMessage are not consistent",
                        NfcErrorCode.BAD_NDEF_FORMAT);
            }
        }

        /* Every thing is OK */
        if (type != null) {
            this.mTypeS = type;
            this.mType = type.getBytes();
        } else {
            this.mTypeS = null;
            this.mType = null;
        }
        /* The constructor must make a copy from the original data and payload. */
        byte[] payload = nestedMessage.getContent();
        if (payload != null) {
            this.mPayload = new byte[payload.length];
            System.arraycopy(payload, 0, mPayload, 0, mPayload.length);
        } else {
            this.mPayload = null;
        }
        this.mTnf = tnf;
    }

    /**
     * Creates a new record from a buffer.
     *
     * @param buffer  the buffer.
     * @throws IllegalArgumentException if <code>buffer</code> is null.
     * @throws NfcException if the format of the buffer does not match a NDEF format.
     **/
    public NdefRecord(byte[] buffer) throws NfcException {
        this(buffer, 0, (buffer == null) ? 0 : buffer.length);
    }

    /**
     * Creates a new record from a buffer.
     *
     * @param buffer  the buffer.
     * @param offset  the offset of the first byte of the record in the buffer.
     * @param length  the length of the record in bytes.
     * @throws IllegalArgumentException if <code>buffer</code> is null.
     * @throws IllegalArgumentException if <code>offset</code> or <code>length</code> is negative.
     * @throws IndexOutOfBoundsException  if <code>offset</code> or <code>length</code> are out of the array.
     * @throws NfcException if the format of the buffer does not match a NDEF format.
     **/
    public NdefRecord(byte[] buffer, int offset, int length) throws NfcException {

        if (buffer == null) {
            throw new IllegalArgumentException("buffer==null");
        }

        if ((offset < 0) || (length < 0)) {
            throw new IllegalArgumentException("negative offset or length");
        }

        if ((offset > buffer.length) || ((offset + length) > buffer.length)
                || ((offset + length) < 0)) {
            throw new IndexOutOfBoundsException();
        }

        if (length < 3) {
            throw new NfcException("Buffer size does not match a NDEF format.");
        }

        /* read mFlags */
        int flags = 0;
        flags = buffer[offset++];
        System.out.println("flags: " + flags);

        if ((Helper.getFlag(flags, Helper.CF_FLAG)) && Helper.getFlag(flags, Helper.ME_FLAG)) {
            throw new NfcException("Data can not be formed to NDEF record");
        }

        int tnf = flags & Helper.TNF_MASK;
        switch (tnf) {
            case ConstantAutogen.W_NDEF_TNF_EMPTY:
                mTnf = NdefTypeNameFormat.EMPTY;
                break;
            case ConstantAutogen.W_NDEF_TNF_WELL_KNOWN:
                mTnf = NdefTypeNameFormat.WELL_KNOWN;
                break;
            case ConstantAutogen.W_NDEF_TNF_MEDIA:
                mTnf = NdefTypeNameFormat.MEDIA;
                break;
            case ConstantAutogen.W_NDEF_TNF_ABSOLUTE_URI:
                mTnf = NdefTypeNameFormat.ABSOLUTE_URI;
                break;
            case ConstantAutogen.W_NDEF_TNF_EXTERNAL:
                mTnf = NdefTypeNameFormat.EXTERNAL;
                break;
            case ConstantAutogen.W_NDEF_TNF_UNCHANGED:
                mTnf = NdefTypeNameFormat.UNCHANGED;
                break;
            case ConstantAutogen.W_NDEF_TNF_UNKNOWN:
                mTnf = NdefTypeNameFormat.UNKNOWN;
                break;
        }
        /* read type length */
        int type_length = buffer[offset++];
        System.out.println("type length: " + type_length);

        /* read payload length */
        int payload_length = 0;
        if (Helper.getFlag(flags, Helper.SR_FLAG)) {
            payload_length = buffer[offset++];
        } else {
            if (buffer.length < (offset + 4)) {
                throw new NfcException("Invalid buffer payload length in NDEF format");
            }
            payload_length = (((buffer[offset++] & 0xFF) << 24) + ((buffer[offset++] & 0xFF) << 16)
                    + ((buffer[offset++] & 0xFF) << 8) + (buffer[offset++] & 0xFF));
        }
        if (payload_length < 0) {
            payload_length += 256;
        }
        System.out.println("payload length: " + payload_length);

        /* read identifier length */
        int id_length = 0;
        if (Helper.getFlag(flags, Helper.IL_FLAG)) {
            if (buffer.length < (offset + 1)) {
                throw new NfcException("Invalid buffer identifier in NDEF format");
            }
            id_length = buffer[offset++];
        } else {
            id_length = 0;
        }
        System.out.println("id length: " + id_length);

        if (((offset - 1) + payload_length) > buffer.length) {
            throw new NfcException("Invalid buffer length in NDEF format");
        }

        /* Check TNF */
        switch (tnf) {
            case ConstantAutogen.W_NDEF_TNF_EMPTY:
                if (type_length != 0 || id_length != 0 || payload_length != 0) {
                    throw new NfcException("Invalid empty NDEF record");
                }
                break;

            case ConstantAutogen.W_NDEF_TNF_WELL_KNOWN:
            case ConstantAutogen.W_NDEF_TNF_MEDIA:
            case ConstantAutogen.W_NDEF_TNF_ABSOLUTE_URI:
            case ConstantAutogen.W_NDEF_TNF_EXTERNAL:
                if (type_length == 0) {
                    throw new NfcException("Invalid type NDEF record");
                }
                break;

            case ConstantAutogen.W_NDEF_TNF_UNCHANGED:
            case ConstantAutogen.W_NDEF_TNF_UNKNOWN:
                if (type_length != 0) {
                    throw new NfcException("Invalid type NDEF record");
                }
                break;
            default:
                throw new NfcException("Invalid unknown-type NDEF record");
        }

        /* read type */
        if (type_length == 0) {
            mType = null;
            System.out.println("type is null");
        } else {
            mType = new byte[type_length];
            System.arraycopy(buffer, offset, mType, 0, type_length);
            offset += type_length;

            for (int i = 0; i < type_length; i++) {
                if (mType[i] < 0) {
                    mType[i] += (mType[i] + 256);
                }
            }

            mTypeS = new String(mType);
            System.out.println("type: " + mTypeS);
        }

        /* read identifier */
        System.out.println("reading identifier...  " + id_length);
        if (id_length == 0) {
            mIdentifier = null;
            System.out.println("identifier is null");
        } else {
            mIdentifier = new byte[id_length];
            System.arraycopy(buffer, offset, mIdentifier, 0, id_length);
            offset += id_length;
            System.out.println("identifier = " + new String(mIdentifier));
        }

        /* read payload */
        System.out.println("reading payload...   " + payload_length);
        if (payload_length == 0) {
            mPayload = null;
            System.out.println("payload is null");
        } else {
            if ((offset + payload_length) > buffer.length) {
                throw new NfcException("Missing payload bytes in NDEF format");
            }
            mPayload = new byte[payload_length];
            System.arraycopy(buffer, offset, mPayload, 0, payload_length);
            offset += payload_length;
            for (int i = 0; i < payload_length; i++) {
                if (mPayload[i] < 0) {
                    mPayload[i] += (mPayload[i] + 256);
                }
            }
        }

        Helper.sNdefRecordSize = offset;
    }

    /**
     * Returns the <a href="{@docRoot}overview-summary.html#ndef_id_format">Record Identifier</a>.
     * <p/>
     * The string is encoded in printable URI format as
     * defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *
     * @return the identifier of the record or null if the record
     *         has no identifier.
     */
    public String getIdentifier() {

        if (this.mIdentifier == null)
            return null;

        return Helper.convertStringToPrintableString(new String(this.mIdentifier));
    }

    /**
     * Sets the <a href="{@docRoot}overview-summary.html#ndef_id_format">Record Identifier</a>.
     *
     * Set <code>identifier</code> to null to delete the identifier of the record.
     * The identifier string is encoded in printable URI format as
     * defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *
     * @param identifier the identifier.
     * @throws IllegalArgumentException if record type format is EMPTY and this method is called.
     */
    public void setIdentifier(String identifier) {

        if (getTNF() == NdefTypeNameFormat.EMPTY) {
            throw new IllegalArgumentException("Cannot be invoked with an EMPTY type format record");
        }

        if ((identifier != null) && (identifier.length() > 0)) {
            if (!Helper.isPrintableURI(identifier)) {
                /* identifier be use printable URI format */
                return;
            }
            this.mIdentifier = Helper.convertPrintableUTF16ToUTF8(identifier);
        } else {
            /*
             * An ID_LENGTH of zero octets is allowed and, in such cases, the ID
             * field is omitted from the NDEF record.
             */
            this.mIdentifier = null;
        }
    }

    /**
     * Checks the <a href="{@docRoot}overview-summary.html#ndef_id_format">Record Identifier</a>.
     *
     * The identifier string is encoded in printable URI format as
     * defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *
     * @param identifier the identifier.
     * @return true if the NDEF record has an identifier and
     *       it matches <code>identifier</code>. false if
     *       <code>identifier</code> is null or has an invalid format or
     *       if the identifiers does not match.
     */
    public boolean checkIdentifier(String identifier) {

        if (identifier == null) {
            return false;
        }
        if (!Helper.isPrintableURI(identifier)) {
            return false;
        }

        return ((getIdentifier() != null) && (getIdentifier().compareTo(identifier) == 0)) ? true
                : false;
    }

    /**
     * Returns the <a href="{@docRoot}overview-summary.html#ndef_id_format">Record Identifier</a> encoded in Utf8.
     *
     * The buffer is encoded in Utf8 URI format as
     * defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *
     * @return the identifier of the record or null if the record
     *         has no identifier.
     */
    public byte[] getBinaryIdentifier() {

        return this.mIdentifier;
    }

    /**
     * Sets the identifier of a record encoded in Utf8.
     *
     * Set <code>identifier</code> to null to delete the identifier of the record.
     * The identifier is encoded in Utf8 URI format as
     * defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *
     * @param identifier the identifier.
     */
    public void setBinaryIdentifier(byte[] identifier) {

        if (getTNF() == NdefTypeNameFormat.EMPTY) {
            throw new IllegalArgumentException("Cannot be invoked with an EMPTY type format record");
        }
        if (identifier != null && identifier.length == 0) {
            /*
             * An ID_LENGTH of zero octets is allowed and, in such cases, the ID
             * field is omitted from the NDEF record.
             */
            this.mIdentifier = null;
        } else {
            if (identifier != null) {
                this.mIdentifier = new byte[identifier.length];
                System.arraycopy(identifier, 0, this.mIdentifier, 0, this.mIdentifier.length);
            } else {
                this.mIdentifier = null;
            }
        }
    }

    /**
     * Returns the <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>. of a record.
     *
     * @return the Type Name Format.
     */
    public NdefTypeNameFormat getTNF() {
        return mTnf;
    }

    /**
     * Returns the type string of a record.
     *
     * See <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     *
     * The type string is encoded in printable format as
     * defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *
     * @return the type string. null if there is no type string.
     */
    public String getType() {

        if (mTnf == NdefTypeNameFormat.EMPTY || mTnf == NdefTypeNameFormat.UNKNOWN
                || mTnf == NdefTypeNameFormat.UNCHANGED)
            return null;

        return mTypeS;
    }

    /**
     * Returns the type string of a record encoded in ASCII.
     *
     * See <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     *
     * The type string is encoded in ASCII format as
     * defined in <a href="{@docRoot}overview-summary.html#ndef_uri_format">URI Format</a>.
     *
     * @return the type string. null if there is no type string.
     */
    public byte[] getAsciiType() {

        if (mTnf == NdefTypeNameFormat.EMPTY || mTnf == NdefTypeNameFormat.UNKNOWN
                || mTnf == NdefTypeNameFormat.UNCHANGED)
            return null;
        return mType;
    }

    /**
     * Checks the type of a record.
     *
     * The comparison algorithm is described in
     * <a href="{@docRoot}overview-summary.html#ndef_type_comparison">Record Comparison</a>.
     *
     * @param tnf the <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a> to be checked.
     * @param type the NDEF type string to be checked.
     * @return true if the NDEF record matches the TNF and the type string.
     * @throws IllegalArgumentException if <code>tnf</code> is null.
     * @throws IllegalArgumentException if <code>tnf</code> and <code>type</code> are not consistent.
     */
    public boolean checkType(NdefTypeNameFormat tnf, String type) {

        if (tnf == null) {
            throw new IllegalArgumentException("tnf = null");
        }
        if (checkTypeConsistency(tnf, type) == false) {
            throw new IllegalArgumentException("tnf and type are not consistent");
        }

        /* comparison with ANY_TYPE is always successfully */
        if ((tnf == NdefTypeNameFormat.ANY_TYPE) || (getTNF() == NdefTypeNameFormat.ANY_TYPE)) {
            return true;
        }

        /* comparison between different TNF values is always unsuccessfully */
        if (tnf != getTNF()) {
            return false;
        }

        switch (tnf.getValue()) {
            case ConstantAutogen.W_NDEF_TNF_EMPTY:
            case ConstantAutogen.W_NDEF_TNF_UNCHANGED:
            case ConstantAutogen.W_NDEF_TNF_UNKNOWN:
                /* For this records, the Type strings must be null */
                return ((type == null) && (getType() == null)) ? true : false;

            case ConstantAutogen.W_NDEF_TNF_WELL_KNOWN:
                return Helper.compareWellKnownURI(getType(), type);
            case ConstantAutogen.W_NDEF_TNF_MEDIA:
                return Helper.compareMediaURICompare(getType(), type);
            case ConstantAutogen.W_NDEF_TNF_ABSOLUTE_URI:
                return Helper.compareAbsoluteURI(getType(), type);
            case ConstantAutogen.W_NDEF_TNF_EXTERNAL:
                return Helper.compareExternalURI(getType(), type);
            default:
                return false;
        }
    }

    /**
     * Checks the consistency of a NDEF record type as defined in
     * <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     *
     * @param tnf the Type Name Format to be checked.
     * @param type the type string to be checked.
     * @return true if the record type is consistent, false otherwise.
     * @throws IllegalArgumentException if <code>tnf</code> is null.
     */
    public static boolean checkTypeConsistency(NdefTypeNameFormat tnf, String type) {

        if (tnf == null) {
            throw new IllegalArgumentException("tnf = null");
        }

        if (tnf == NdefTypeNameFormat.ANY_TYPE) {
            if (type != null)
                return false;
        } else {
            if (tnf == NdefTypeNameFormat.EMPTY || tnf == NdefTypeNameFormat.UNKNOWN
                    || tnf == NdefTypeNameFormat.UNCHANGED) {
                /* for these TNF values, the type must be null */
                return (type == null) ? true : false;
            } else if (tnf == NdefTypeNameFormat.WELL_KNOWN || tnf == NdefTypeNameFormat.MEDIA
                    || tnf == NdefTypeNameFormat.ABSOLUTE_URI || tnf == NdefTypeNameFormat.EXTERNAL) {
                if (type == null) {
                    return false;
                }
            } else {
                return false;
            }

            if (tnf == NdefTypeNameFormat.MEDIA || tnf == NdefTypeNameFormat.ABSOLUTE_URI) {
                return Helper.isPrintableURI(type);
            } else if (tnf == NdefTypeNameFormat.WELL_KNOWN) {
                return Helper.isWellKnownCompatible(type);
            } else if (tnf == NdefTypeNameFormat.EXTERNAL) {
                return Helper.isExternalCompatible(type);
            }
        }

        return true;
    }

    /**
     * Returns the payload of a record.
     *
     * @return the payload of the record or null if there is no payload.
     */
    public byte[] getPayload() {
        return this.mPayload;
    }

    /**
     * Returns the binary representation of a record.
     *
     * @return the binary value of the record, encoded as a message of one record.
     */
    public byte[] getContent() {

        boolean SR = false;
        boolean IL = false;
        byte header = 0;

        /* set SR if data < 255 */
        if ((this.mPayload != null) && (this.mPayload.length < 255)) {
            header |= (1 << 4);
            SR = true;
        }
        if (mPayload == null) {
            header |= (1 << 4);
            SR = true;
        }

        /* set IL if id exist */
        if (this.mIdentifier != null && this.mIdentifier.length != 0) {
            header |= (1 << 3);
            IL = true;
        }

        /* set TNF */
        header += this.mTnf.getValue();

        /* set MB and ME */
        header = Helper.setFlag(header, Helper.MB_FLAG);
        header = Helper.setFlag(header, Helper.ME_FLAG);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        baos.write(header);

        int payload_length = 0;
        int type_length = 0;
        int identifier_length = 0;
        if (this.mPayload != null)
            payload_length = this.mPayload.length;
        if (this.mType != null)
            type_length = this.mType.length;
        if (this.mIdentifier != null)
            identifier_length = this.mIdentifier.length;

        /* write type length */
        if (this.mTnf != NdefTypeNameFormat.EMPTY && this.mTnf != NdefTypeNameFormat.UNKNOWN
                && this.mTnf != NdefTypeNameFormat.UNCHANGED) {
            baos.write(type_length);
        } else {
            /* make sure type is omitted */
            type_length = 0;
            baos.write(type_length);
        }

        /* write payload length */
        if (!SR) {
            baos.write(payload_length >> 24);
            baos.write(payload_length >> 16);
            baos.write(payload_length >> 8);
        }
        baos.write(payload_length);

        /* write id length */
        if (IL) {
            baos.write(identifier_length);
        } else {
            /* make sure identifier is omitted */
            identifier_length = 0;
        }

        /* write type */
        if (this.mType != null)
            baos.write(mType, 0, type_length);

        /* write id */
        if (this.mIdentifier != null)
            baos.write(this.mIdentifier, 0, this.mIdentifier.length);

        /* write payload */
        if (this.mPayload != null)
            baos.write(this.mPayload, 0, payload_length);

        return baos.toByteArray();
    }

    /**
     * Extracts a NDEF message enclosed in the payload of a record.
     *
     * @return  the NDEF message enclosed in the record payload.
     *
     * @throws  NfcException if the payload does not include a well-formed message.
     */
    public NdefMessage getEnclosedMessage() throws NfcException {

        if (this.mPayload == null)
            throw new NfcException("payload==null");

        return new NdefMessage(this.mPayload);
    }

    /**
     * Compares a record with another object.
     *
     * The comparison algorithm is described in
     * <a href="{@docRoot}overview-summary.html#ndef_type_comparison">Record Comparison</a>.
     *
     * @param  object the object to compare.
     * @return true if the specified object is a record with the same content, false otherwise.
     **/
    @Override
    public synchronized boolean equals(Object object) {

        if (object == this)
            return true;

        if ((object != null) && (object instanceof NdefRecord)) {

            NdefRecord record = (NdefRecord) object;
            if (checkType(record.getTNF(), record.getType())) {
                byte[] payload = record.getPayload();
                byte[] thisPayload = getPayload();
                if (payload == null) {
                    return (thisPayload == null);
                }
                if (thisPayload != null) {
                    int length = thisPayload.length;
                    if (payload.length == length) {
                        for (int i = 0; i < length; i++) {
                            if (thisPayload[i] != payload[i]) {
                                return false;
                            }
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
