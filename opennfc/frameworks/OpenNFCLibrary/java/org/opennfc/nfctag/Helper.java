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

import java.io.UnsupportedEncodingException;

/**
 * package private
 *
 * @hide
 **/
final class Helper {

    private Helper() {
        // private constructor in order to prevent to be extended
    }

    /** Semaphore used into NdefMessage Constructor**/
    /* package protected */static final Object MUTEX = new Object();

    /** Semaphore used into NdefMessage.getContent function**/
    /* package protected */static final Object MUTEX_BIS = new Object();

    /* package protected */static int sNdefRecordSize = 0;

    /* package protected */static final int MB_FLAG = 0x80;

    /* package protected */static final int ME_FLAG = 0x40;

    /* package protected */static final int CF_FLAG = 0x20;

    /* package protected */static final int SR_FLAG = 0x10;

    /* package protected */static final int IL_FLAG = 0x08;

    /* package protected */static final int TNF_MASK = 0x07;

    //    /* package protected */static int mFlags;

    /* package protected */static boolean getFlag(int flags, int f) {
        return (flags & f) != 0;
    }

    /* package protected */static byte setFlag(int flags, int f) {
        return (byte) (flags |= f);
    }

    /* package protected */static byte clrFlag(int flags, int f) {
        return (byte) (flags &= ~f);
    }

    /* package protected */static int uriIdentifyLength[] = {
            0, 11, 12, 7, 8, 4, 7, 26, 10, 7, 7, 6, 6, 6, 6, 5, 9, 5, 7, 4, 4, 4, 5, 5, 8, 10, 9,
            10, 11, 7, 11, 12, 12, 12, 8, 8, 3
    };

    /* package protected */static String uriIdentifyCode[] = {
            "", "http://www.", "https://www.", "http://", "https://", "tel:", "mailto:",
            "ftp://anonymous:anonymous@", "ftp://ftp.", "ftps://", "sftp://", "smb://", "nfs://",
            "ftp://", "dav://", "news:", "telnet://", "imap:", "rtsp://", "urn:", "pop:", "sip:",
            "sips:", "tftp:", "btspp://", "btl2cap://", "btgoep://", "tcpobex://", "irdaobex://",
            "file://", "urn:epc:id:", "urn:epc:tag:", "urn:epc:pat:", "urn:epc:raw:", "urn:epc:",
            "urn:nfc:", "RFU"
    };

    /* package protected */static boolean isWellKnownCompatible(String type) {

        if (type == null)
            return false;

        /* skip the 'urn:nfc:wkt: if present */
        if (type.startsWith("urn:nfc:wkt:")) {
            type = type.substring(12);
        }

        /* The WKT-type must have at least one local or global char */
        if (type.length() == 0)
            return false;

        char nCar;
        /* If the fisrt character is other or reserved return FALSE */
        nCar = type.charAt(0);
        if (!(((nCar >= 'A') && (nCar <= 'Z')) || ((nCar >= 'a') && (nCar <= 'z')) || ((nCar >= '0') && (nCar <= '9')))) {
            return false;
        }

        for (int i = 1; i < type.length(); i++) {
            nCar = type.charAt(i);
            if (!isPrintableURI(nCar)) {
                return false;
            }
            if (nCar == '%' || nCar == '/' || nCar == '?' || nCar == '#' || nCar == '~') {
                return false;
            }
        }

        return true;
    }

    /* package protected */static boolean isExternalCompatible(String type) {

        if (type == null)
            return false;

        /* skip the 'urn:nfc:ext: if present */
        if (type.startsWith("urn:nfc:ext:")) {
            type = type.substring(12);
        }

        /* check the DNS part */
        int indexNamePart = type.indexOf(':');
        if (indexNamePart == -1 || indexNamePart == 0) {
            /* empty DNS part */
            return false;
        }
        char nCar;
        for (int i = 0; i < indexNamePart; i++) {
            nCar = type.charAt(i);
            if (!(((nCar >= 'A') && (nCar <= 'Z')) || ((nCar >= 'a') && (nCar <= 'z'))
                    || ((nCar >= '0') && (nCar <= '9')) || (nCar == '.') || (nCar == '-'))) {
                /* invalid character in DNS part") */
                return false;
            }
        }

        type = type.substring(indexNamePart + 1);
        /* here, the DNS part has been processed, and the : has been skipped */
        if (type.length() == 0) {
            /* empty name part is forbidden */
            return false;
        }

        /* Check the name part */
        for (int i = 0; i < type.length(); i++) {
            nCar = type.charAt(i);
            if (!isPrintableURI(nCar)) {
                return false;
            }
            if (nCar == '%' || nCar == '/' || nCar == '?' || nCar == '#' || nCar == '~') {
                return false;
            }
        }

        return true;
    }

    /* package protected */static boolean is8BitsCompatible(String type) {

        if (type == null)
            return false;

        char nCar;
        for (int i = 0; i < type.length(); i++) {
            nCar = type.charAt(i);
            if (nCar > 255) {
                return false;
            }
        }

        return true;
    }

    /* package protected */static final char[] mPrintableURIChars = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
            'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
            'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
            'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', '/', '?', '#', '[', ']',
            '@', '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=', '-', '.', '_', '~', ' '
    };

    /* package protected */static boolean isPrintableURI(char car) {
        for (int i = 0; i < mPrintableURIChars.length; i++) {
            if (mPrintableURIChars[i] == car) {
                return true;
            }
        }
        return false;
    }

    /* package protected */static boolean isPrintableURI(String type) {

        if (type == null)
            return false;

        boolean bInEscapement = false;
        int nEscapementLength = 0;
        char nCar;
        for (int i = 0; i < type.length(); i++) {
            nCar = type.charAt(i);
            if (bInEscapement == false) {
                if (nCar != '%') {
                    if (isPrintableURI(nCar) == false)
                        return false;
                } else {
                    bInEscapement = true;
                    nEscapementLength = (type.charAt(i + 1) == 'u') ? 4 : 2;
                }
            } else {
                if ((nEscapementLength == 4) && (nCar == 'u'))
                    continue;

                /* Checks if the character is an hexadecimal digit */
                if (!(((nCar >= 'A') && (nCar <= 'F')) || ((nCar >= '0') && (nCar <= '9')))) {
                    return false;
                }

                if (--nEscapementLength == 0) {
                    bInEscapement = false;
                }
            }
        }

        return (bInEscapement == false) ? true : false;
    }

    /**
    * Compares two Well-known TNF records
    *
    * @param pString1 The string corresponding to the first TNF record
    * @param pString2 The string corresponding to the second TNF record
    * @return true if the TNF records are equal
    */
    /* package protected */static boolean compareWellKnownURI(String pString1, String pString2) {

        if (pString1 == null) {
            if (pString2 == null) {
                return true;
            }
            return false;
        }

        /* Before performing the comparison, the prefix urn:nfc:wkt: is removed from the type names, if present */
        if (pString1.startsWith("urn:nfc:wkt:")) {
            pString1 = pString1.substring(12);
        }
        if (pString2.startsWith("urn:nfc:wkt:")) {
            pString2 = pString2.substring(12);
        }

        /* Then perform the comparison (case sensitive) */
        return (pString1.compareTo(pString2) == 0) ? true : false;
    }

    /**
    * Compares two external TNF records
    *
    * @param[in] pString1 The string corresponding to the first TNF record
    * @param[in] pString2 The string corresponding to the second TNF record
    * @return true if the TNF records are equal
    */
    /* package protected */static boolean compareExternalURI(String pString1, String pString2) {

        if (pString1 == null) {
            if (pString2 == null) {
                return (true);
            }
            return (false);
        }

        /* Before performing the comparison, the prefix urn:nfc:ext: is removed from the type names, if present */
        if (pString1.startsWith("urn:nfc:ext:")) {
            pString1 = pString1.substring(12);
        }
        if (pString2.startsWith("urn:nfc:ext:")) {
            pString2 = pString2.substring(12);
        }

        /* Then perform the comparison (case sensitive) */
        return (pString1.compareToIgnoreCase(pString2) == 0) ? true : false;
    }

    /**
    * Compares two Media TNF records
    *
    * @param pString1 The string corresponding to the first TNF record
    * @param pString2 The string corresponding to the second TNF record
    * @return true if the TNF records are equal
    */
    /* package protected */static boolean compareMediaURICompare(String pString1, String pString2) {

        /* The comparison must be done till the first occurence of '; */
        int endIndex = pString1.indexOf(';');
        if ((endIndex = pString1.indexOf(';')) != -1) {
            pString1 = pString1.substring(0, endIndex);
        }
        if ((endIndex = pString2.indexOf(';')) != -1) {
            pString2 = pString2.substring(0, endIndex);
        }
        /* Perform the comparison until the ';' (case insensitive) */
        if (pString1.length() == pString2.length()) {
            return pString1.equalsIgnoreCase(pString2);
        }
        /* the two sub-strings to compare do not have the same length, they differ ! */
        return false;
    }

    /* package protected */static final char sHexaChars[] = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    /**
     * Converts a string into a printable URI UTF-16 string
     *
     * @param[in] pInBuffer  The source buffer
     * return  The printable URI of the string
     */
    /* package protected */static String convertStringToPrintableString(final String pSourceString) {

        if (pSourceString == null) {
            return null;
        }

        char nCar;
        StringBuffer pDestString = new StringBuffer();

        for (int i = 0; i < pSourceString.length(); i++) {
            nCar = pSourceString.charAt(i);
            if (nCar > 255) {
                /* must be encoded using %uXXXX format */
                pDestString.append('%');
                pDestString.append('u');
                pDestString.append(sHexaChars[(nCar >> 12) & 0x0F]);
                pDestString.append(sHexaChars[(nCar >> 8) & 0x0F]);
                pDestString.append(sHexaChars[(nCar >> 4) & 0x0F]);
                pDestString.append(sHexaChars[(nCar) & 0x0F]);
            } else {
                if (isPrintableURI(nCar)) {
                    pDestString.append(nCar);
                } else {
                    /* must be encoded using %XX format */
                    pDestString.append('%');
                    pDestString.append(sHexaChars[(nCar >> 4) & 0x0F]);
                    pDestString.append(sHexaChars[(nCar) & 0x0F]);
                }
            }
        }
        return pDestString.toString();
    }

    /* package protected */static byte[] convertPrintableUTF16ToUTF8(final String pSourceString) {

        if (pSourceString == null) {
            return null;
        }

        boolean bInEscapement = false;
        int nEscapementLength = 0;
        char nCar, nOutCar = 0;
        StringBuffer pDestString = new StringBuffer();

        for (int i = 0; i < pSourceString.length(); i++) {
            nCar = pSourceString.charAt(i);
            if (bInEscapement == false) {
                if (nCar != '%') {
                    pDestString.append(nCar);
                } else {
                    bInEscapement = true;
                    nEscapementLength = (pSourceString.charAt(i + 1) == 'u') ? 4 : 2;
                    nOutCar = 0;
                }
            } else {
                nOutCar = (char) (nOutCar * 16);

                if ((nEscapementLength == 4) && (nCar == 'u'))
                    continue;

                if ((nCar >= '0') && (nCar <= '9')) {
                    nOutCar += nCar - '0';
                } else {
                    nOutCar += nCar - 'A' + 10;
                }

                if (--nEscapementLength == 0) {
                    if (pDestString != null) {
                        pDestString.append(nOutCar);
                    }
                    bInEscapement = false;
                }
            }
        }
        return pDestString.toString().getBytes();
    }

    /**
     * Convert byte array to String.
     * @param buf
     * @param offset
     * @param length
     * @return
     */
    /* package protected */static String bytesToString(byte[] buf, int offset, int length) {
        StringBuffer sb = new StringBuffer();

        for (int i = 0; i < length; i++) {
            char val = (char) (buf[offset + i]);
            sb.append(val);
        }
        return new String(sb).trim();
    }

    /**
     * Convert UTF-8 byte array to String.
     * @param buf
     * @return
     */
    /* package protected */static String convertUTF8ToUTF16(byte[] buf) {
        try {
            return new String(buf, "UTF-8");
        } catch (UnsupportedEncodingException ex) {
            ex.printStackTrace();
            return bytesToString(buf, 0, buf.length);
        }
    }

    /**
     * Convert this String into a sequence of bytes using the UTF-8 charset.
     * @param str
     * @return
     */
    /* package protected */static byte[] convertUTF16ToUTF8(String str) {
        try {
            return str.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            return str.getBytes();
        }
    }

    /**
     * Normalizes an absolute URI (lower scheme and host part)
     *
     * @param pString The absolute URI to be normalized
     */
    /* package protected */static String normalizeAbsoluteURI(String pString) {

        int index = -1;

        /* lower the scheme part */
        if ((index = pString.indexOf(':')) == -1) {
            return pString;
        }
        String scheme = pString.substring(0, index);
        pString = pString.replaceFirst(scheme, scheme.toLowerCase());

        /* check the presence of "//" */
        if ((index = pString.indexOf("//", index)) == -1) {
            /* no // found, return */
            return pString;
        }

        /* lower the host part ( authority : [user@]host[:port] )*/
        index += 2;
        int indexStart = pString.indexOf("@", index);
        indexStart = (indexStart == -1) ? index : indexStart + 1;
        int indexEnd = pString.indexOf(indexStart, ':');
        if (indexEnd == -1) {
            if ((indexEnd = pString.indexOf("/", indexStart)) == -1) {
                indexEnd = pString.length();
            }
        }
        String host = pString.substring(indexStart, indexEnd);

        return pString.replaceFirst(host, host.toLowerCase());
    }

    /**
    * Compares two absolute URI TNF records
    *
    * @param[in] pString1 The string corresponding to the first TNF record
    * @param[in] pString2 The string corresponding to the second TNF record
    * @return true if the TNF records are equal
    */
    /* package protected */static boolean compareAbsoluteURI(String pString1, String pString2) {

        if (pString1 == null) {
            if (pString2 == null) {
                return (true);
            }
            return (false);
        }

        String pTempString1 = convertStringToPrintableString(pString1);
        String pTempString2 = convertStringToPrintableString(pString2);

        if (pTempString1 != null && pTempString2 != null) {
            if (pTempString1.length() != pTempString2.length())
                return false;
        } else {
            return false;
        }

        if (pTempString1.length() == 0) {
            return true;
        }

        pTempString1 = normalizeAbsoluteURI(pTempString1);
        pTempString2 = normalizeAbsoluteURI(pTempString2);

        return (pTempString1.compareTo(pTempString2) == 0) ? true : false;
    }
}
