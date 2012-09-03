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

import java.net.URI;
import java.net.URISyntaxException;

import org.opennfc.NfcException;

/**
 * The UriRecord represent a NDEF Record of type URI.
 *
 * See <a href="{@docRoot}overview-summary.html#ndef_uri_record">URI Record</a>
 * for the details on the URI encoding in the NDEF record.
 *
 * @since Open NFC 4.0
 */
public final class UriRecord extends NdefRecord {

    /**
     * The URI type
     * @hide
     */
    private static final String TYPE_URI = "U";

    /**
     * Creates a new URI record.
     *
     * @param uri the URI value.
     * @throws NfcException
     *
     * @throws IllegalArgumentException if <code>uri</code> is null.
     */
    public UriRecord(URI uri) throws NfcException {

        if (uri == null) {
            throw new IllegalArgumentException("uri = null");
        }

        String uriString = uri.toASCIIString();
        if (!Helper.isPrintableURI(uriString)) {
            throw new NfcException("The type must contain a printable URI");
        }

        int identifierCode = 0;
        int uriLength = uriString.length();
        /* Store the index of the greatest corresponding URI code length */
        for (int i = 0; i < Helper.uriIdentifyCode.length; i++) {
            if (uriLength >= Helper.uriIdentifyLength[i]) {
                if (uriString.startsWith(Helper.uriIdentifyCode[i])) {
                    if ((identifierCode == 0)
                            || (Helper.uriIdentifyLength[i] > Helper.uriIdentifyLength[identifierCode]))
                        identifierCode = i;
                }
            }
        }
        /* store the uri paylaod */
        byte[] bufferTemp = Helper.convertPrintableUTF16ToUTF8(uriString);
        byte[] buffer = new byte[1 + (uriLength - Helper.uriIdentifyLength[identifierCode])];
        buffer[0] = (byte) identifierCode;
        System.arraycopy(bufferTemp, Helper.uriIdentifyLength[identifierCode], buffer, 1,
                buffer.length - 1);
        /* set the Ndef Record attributes */
        setNdefRecord(new NdefRecord(NdefTypeNameFormat.WELL_KNOWN, TYPE_URI, buffer));
    }

    /**
     * Creates a new URI record from another record.
     *
     * @param record the record value.
     *
     * @throws IllegalArgumentException if <code>record</code> is null.
     * @throws URISyntaxException if <code>record</code> is not of type URI.
     * @throws NfcException
     */
    public UriRecord(NdefRecord record) throws URISyntaxException, NfcException {

        if (record == null) {
            throw new IllegalArgumentException("record = null");
        }
        if (record.checkType(NdefTypeNameFormat.WELL_KNOWN, TYPE_URI) == false) {
            throw new URISyntaxException("", "record is not of type URI");
        }
        /* set the Ndef Record attributes */
        setNdefRecord(new NdefRecord(NdefTypeNameFormat.WELL_KNOWN, TYPE_URI, record.getPayload()));
        /* check the URI value */
        privateGetUri();
    }

    /**
     * Checks if a record is a URI record.
     *
     * @param record  the record to check.
     * @return true if the record is of type URI, false otherwise.
     * @throws IllegalArgumentException if <code>record</code> is null.
     **/
    public static boolean isUriRecord(NdefRecord record) {

        if (record == null) {
            throw new IllegalArgumentException("record = null");
        }

        return record.checkType(NdefTypeNameFormat.WELL_KNOWN, TYPE_URI);
    }

    /**
     * Gets the value of the URI in a URI record.
     *
     * @return the value of the URI.
     */
    public URI getUri() {

        try {
            return privateGetUri();
        } catch (URISyntaxException e) {
        }

        return null;
    }

    /**
     * Gets the value of the URI in a URI record.
     *
     * @return the value of the URI.
     * @hide
     */
    private URI privateGetUri() throws URISyntaxException {

        byte[] payload = this.getPayload();
        int identifierCode = payload[0];
        if (identifierCode > 0x23) {
            identifierCode = 0;
        }
        byte[] tmp = new byte[payload.length - 1];
        System.arraycopy(payload, 1, tmp, 0, tmp.length);

        StringBuffer stringBuffer = new StringBuffer();
        stringBuffer.append(Helper.uriIdentifyCode[identifierCode]);
        stringBuffer.append(Helper.convertUTF8ToUTF16(tmp));

        return new URI(new String(stringBuffer));
    }

}
