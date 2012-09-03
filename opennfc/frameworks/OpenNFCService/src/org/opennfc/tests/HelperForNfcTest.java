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

import org.opennfc.HelperForNfc;
import org.opennfc.NfcErrorCode;
import org.opennfc.NfcException;

import java.nio.charset.Charset;

/**
 * Test of {@link HelperForNfc}
 */
public class HelperForNfcTest extends TestScenario {
    /**
     * Create the test of {@link HelperForNfc}
     */
    public HelperForNfcTest() {
    }

    /**
     * Call when test is started.<br>
     * It launch each unit tests <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.tests.TestScenario#start()
     */
    @Override
    protected void start() {
        this.testByteArrayToString();
        this.testConcat();
        this.testDoesArrayEquivalent();
        this.testDoesArrayIncludeInt();
        this.testDoesArrayIncludeString();
        this.testEqualsEmptyNullByteArray();
        this.testEqualsEmptyNullString();
        this.testEqualsEmptyNullIgnoreCase();
        this.testEqualsNull();
        this.testExchange();
        this.testParseWellKnownUriRecord();
        this.testStringToByteArray();
        this.testToHexadecimal();

        this.stopTest();
    }

    /**
     * Test of {@link HelperForNfc#byteArrayToString(byte[])} and
     * {@link HelperForNfc#byteArrayToString(byte[], int, int)}
     */
    private void testByteArrayToString() {
        try {
            HelperForNfc.byteArrayToString(null);

            this.assertFail("Null must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }

        final byte[] array = {
                (byte) ('a' & 0xFF), (byte) ('b' & 0xFF), (byte) ('c' & 0xFF), (byte) ('d' & 0xFF),
                (byte) ('e' & 0xFF), (byte) ('f' & 0xFF)
        };
        this.assertEquals(null, HelperForNfc.byteArrayToString(array), "abcdef");
        this.assertEquals(null, HelperForNfc.byteArrayToString(array, 1, 3), "bcd");

        try {
            HelperForNfc.byteArrayToString(array, -1, 3);

            this.assertFail("Negative offset  must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }

        try {
            HelperForNfc.byteArrayToString(array, 50, 3);

            this.assertFail("To big offset must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }

        try {
            HelperForNfc.byteArrayToString(array, 0, 33);

            this.assertFail("To big length must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }

        try {
            HelperForNfc.byteArrayToString(array, 0, -1);

            this.assertFail("Negative length must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }
    }

    /**
     * Test of {@link HelperForNfc#concat(byte[]...)}
     */
    private void testConcat() {
        try {
            HelperForNfc.concat((byte[][]) null);

            this.assertFail("Null must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }

        final byte[] empty = new byte[0];
        this.assertEquals("Empty argument returns an empty array", HelperForNfc.concat(), empty);

        try {
            HelperForNfc.concat(empty, null, empty);

            this.assertFail("At least one null must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }

        final byte[] array1 = {
                (byte) 0, (byte) 1, (byte) 2, (byte) 3, (byte) 4, (byte) 5
        };
        final byte[] array2 = {
                (byte) 10, (byte) 11, (byte) 22, (byte) 33
        };
        final byte[] array3 = {
                (byte) 111, (byte) 222
        };

        final byte[] result = {
                (byte) 0, (byte) 1, (byte) 2, (byte) 3, (byte) 4, (byte) 5, (byte) 10, (byte) 11,
                (byte) 22, (byte) 33, (byte) 111, (byte) 222
        };

        this.assertEquals(null, HelperForNfc.concat(array1, empty, array2, empty, array3), result);
    }

    /**
     * Test of {@link HelperForNfc#doesArrayEquivalent(int[], int[])}
     */
    private void testDoesArrayEquivalent() {
        final int[] empty = new int[0];

        this.assertTrue("null must be equivalent to null",
                HelperForNfc.doesArrayEquivalent(null, null));
        this.assertTrue("empty must be equivalent to null",
                HelperForNfc.doesArrayEquivalent(empty, null));
        this.assertTrue("null must be equivalent to empty",
                HelperForNfc.doesArrayEquivalent(null, empty));
        this.assertTrue("empty must be equivalent to empty",
                HelperForNfc.doesArrayEquivalent(empty, empty));

        final int[] array1 = {
                1, 2, 3, 4, 5, 6, 7, 8, 9
        };
        final int[] array2 = {
                1, 5, 9, 8, 4, 2, 6, 3, 7
        };
        final int[] array3 = {
                1, 5, 8, 4, 2, 6, 3, 7
        };

        this.assertFalse("Array musn't be equivalent to null",
                HelperForNfc.doesArrayEquivalent(array1, null));
        this.assertFalse("Array musn't be equivalent to null",
                HelperForNfc.doesArrayEquivalent(null, array1));
        this.assertFalse("Array musn't be equivalent to empty",
                HelperForNfc.doesArrayEquivalent(array1, empty));
        this.assertFalse("Array musn't be equivalent to empty",
                HelperForNfc.doesArrayEquivalent(empty, array1));
        this.assertTrue("Array must be equivalent to himself",
                HelperForNfc.doesArrayEquivalent(array1, array1));
        this.assertTrue("Array must be equivalent to other disorder",
                HelperForNfc.doesArrayEquivalent(array1, array2));
        this.assertTrue("Array must be equivalent to other disorder",
                HelperForNfc.doesArrayEquivalent(array2, array1));
        this.assertFalse("Array musn't be equivalent to smaller",
                HelperForNfc.doesArrayEquivalent(array1, array3));
        this.assertFalse("Array musn't be equivalent to bigger",
                HelperForNfc.doesArrayEquivalent(array3, array1));
    }

    /**
     * Test of {@link HelperForNfc#doesArrayInclude(int[], int[])}
     */
    private void testDoesArrayIncludeInt() {
        final int[] empty = new int[0];

        this.assertTrue("null must include null",
                HelperForNfc.doesArrayInclude((int[]) null, (int[]) null));
        this.assertTrue("empty must include null", HelperForNfc.doesArrayInclude(empty, null));
        this.assertTrue("null must include empty", HelperForNfc.doesArrayInclude(null, empty));
        this.assertTrue("empty must include empty", HelperForNfc.doesArrayInclude(empty, empty));

        final int[] array1 = {
                1, 2, 3, 4, 5, 6, 7, 8, 9
        };
        final int[] array2 = {
                1, 5, 9, 8, 4, 2, 6, 3, 7
        };
        final int[] array3 = {
                1, 5, 8, 4, 2, 6, 3, 7
        };
        final int[] array4 = {
                1, 5, 18, 4, 2, 6, 3, 7
        };

        this.assertFalse("null musn't include something",
                HelperForNfc.doesArrayInclude(null, array1));
        this.assertTrue("array must include null", HelperForNfc.doesArrayInclude(array1, null));
        this.assertFalse("empty musn't include something",
                HelperForNfc.doesArrayInclude(empty, array1));
        this.assertTrue("array must include empty", HelperForNfc.doesArrayInclude(array1, empty));
        this.assertTrue("Array must include himself", HelperForNfc.doesArrayInclude(array1, array1));
        this.assertTrue("Array must include same but disorder",
                HelperForNfc.doesArrayInclude(array1, array2));
        this.assertTrue("Array must include same but disorder",
                HelperForNfc.doesArrayInclude(array2, array1));
        this.assertTrue("Array must include smaller", HelperForNfc.doesArrayInclude(array1, array3));
        this.assertFalse("Array musn't include bigger",
                HelperForNfc.doesArrayInclude(array3, array1));
        this.assertFalse("Array musn't include something with not commun",
                HelperForNfc.doesArrayInclude(array1, array4));
    }

    /**
     * Test of {@link HelperForNfc#doesArrayInclude(String[], String[])}
     */
    private void testDoesArrayIncludeString() {
        final String[] empty = new String[0];

        this.assertTrue("null must include null",
                HelperForNfc.doesArrayInclude((String[]) null, (String[]) null));
        this.assertTrue("empty must include null", HelperForNfc.doesArrayInclude(empty, null));
        this.assertTrue("null must include empty", HelperForNfc.doesArrayInclude(null, empty));
        this.assertTrue("empty must include empty", HelperForNfc.doesArrayInclude(empty, empty));

        final String[] array1 = {
                "1", "2", "3", "4", "5", "6", "7", "8", "9"
        };
        final String[] array2 = {
                "1", "5", "9", "8", "4", "2", "6", "3", "7"
        };
        final String[] array3 = {
                "1", "5", "8", "4", "2", "6", "3", "7"
        };
        final String[] array4 = {
                "1", "5", "18", "4", "2", "6", "3", "7"
        };

        this.assertFalse("null musn't include something",
                HelperForNfc.doesArrayInclude(null, array1));
        this.assertTrue("array must include null", HelperForNfc.doesArrayInclude(array1, null));
        this.assertFalse("empty musn't include something",
                HelperForNfc.doesArrayInclude(empty, array1));
        this.assertTrue("array must include empty", HelperForNfc.doesArrayInclude(array1, empty));
        this.assertTrue("Array must include himself", HelperForNfc.doesArrayInclude(array1, array1));
        this.assertTrue("Array must include same but disorder",
                HelperForNfc.doesArrayInclude(array1, array2));
        this.assertTrue("Array must include same but disorder",
                HelperForNfc.doesArrayInclude(array2, array1));
        this.assertTrue("Array must include smaller", HelperForNfc.doesArrayInclude(array1, array3));
        this.assertFalse("Array musn't include bigger",
                HelperForNfc.doesArrayInclude(array3, array1));
        this.assertFalse("Array musn't include something with not commun",
                HelperForNfc.doesArrayInclude(array1, array4));
    }

    /**
     * Test of {@link HelperForNfc#equalsEmptyNull(byte[], byte[])}
     */
    private void testEqualsEmptyNullByteArray() {
        final byte[] empty = new byte[0];

        this.assertTrue("null equals null",
                HelperForNfc.equalsEmptyNull((byte[]) null, (byte[]) null));
        this.assertTrue("null equals empty", HelperForNfc.equalsEmptyNull((byte[]) null, empty));
        this.assertTrue("empty equals null", HelperForNfc.equalsEmptyNull(empty, (byte[]) null));
        this.assertTrue("empty equals empty", HelperForNfc.equalsEmptyNull(empty, empty));

        final byte[] array1 = {
                (byte) 0, (byte) 1, (byte) 2, (byte) 3, (byte) 4, (byte) 5
        };
        final byte[] array2 = {
                (byte) 10, (byte) 11, (byte) 22, (byte) 33
        };
        final byte[] array3 = {
                (byte) 0, (byte) 1, (byte) 2, (byte) 3, (byte) 4, (byte) 5
        };

        this.assertFalse("array not equals to null", HelperForNfc.equalsEmptyNull(array1, null));
        this.assertFalse("array not equals to null", HelperForNfc.equalsEmptyNull(null, array1));
        this.assertFalse("array not equals to empty", HelperForNfc.equalsEmptyNull(array1, empty));
        this.assertFalse("array not equals to empty", HelperForNfc.equalsEmptyNull(empty, array1));
        this.assertTrue("array equals to himself", HelperForNfc.equalsEmptyNull(array1, array1));
        this.assertTrue(null, HelperForNfc.equalsEmptyNull(array1, array3));
        this.assertTrue(null, HelperForNfc.equalsEmptyNull(array3, array1));
        this.assertFalse(null, HelperForNfc.equalsEmptyNull(array1, array2));
        this.assertFalse(null, HelperForNfc.equalsEmptyNull(array2, array1));
    }

    /**
     * Test of {@link HelperForNfc#equalsEmptyNullIgnoreCase(String, String)}
     */
    private void testEqualsEmptyNullIgnoreCase() {
        final String empty = "";

        this.assertTrue("null equals null",
                HelperForNfc.equalsEmptyNullIgnoreCase((String) null, (String) null));
        this.assertTrue("null equals empty",
                HelperForNfc.equalsEmptyNullIgnoreCase((String) null, empty));
        this.assertTrue("empty equals null",
                HelperForNfc.equalsEmptyNullIgnoreCase(empty, (String) null));
        this.assertTrue("empty equals empty", HelperForNfc.equalsEmptyNullIgnoreCase(empty, empty));

        final String array1 = "test";
        final String array2 = "try";
        final String array3 = "test";
        final String array4 = "TesT";

        this.assertFalse("String not equals to null",
                HelperForNfc.equalsEmptyNullIgnoreCase(array1, null));
        this.assertFalse("String not equals to null",
                HelperForNfc.equalsEmptyNullIgnoreCase(null, array1));
        this.assertFalse("String not equals to empty",
                HelperForNfc.equalsEmptyNullIgnoreCase(array1, empty));
        this.assertFalse("String not equals to empty",
                HelperForNfc.equalsEmptyNullIgnoreCase(empty, array1));
        this.assertTrue("String equals to himself",
                HelperForNfc.equalsEmptyNullIgnoreCase(array1, array1));
        this.assertTrue(null, HelperForNfc.equalsEmptyNullIgnoreCase(array1, array3));
        this.assertTrue(null, HelperForNfc.equalsEmptyNullIgnoreCase(array3, array1));
        this.assertFalse(null, HelperForNfc.equalsEmptyNullIgnoreCase(array1, array2));
        this.assertFalse(null, HelperForNfc.equalsEmptyNullIgnoreCase(array2, array1));
        this.assertTrue(null, HelperForNfc.equalsEmptyNullIgnoreCase(array1, array4));
        this.assertTrue(null, HelperForNfc.equalsEmptyNullIgnoreCase(array4, array1));
    }

    /**
     * Test of {@link HelperForNfc#equalsEmptyNull(String, String)}
     */
    private void testEqualsEmptyNullString() {
        final String empty = "";

        this.assertTrue("null equals null",
                HelperForNfc.equalsEmptyNull((String) null, (String) null));
        this.assertTrue("null equals empty", HelperForNfc.equalsEmptyNull((String) null, empty));
        this.assertTrue("empty equals null", HelperForNfc.equalsEmptyNull(empty, (String) null));
        this.assertTrue("empty equals empty", HelperForNfc.equalsEmptyNull(empty, empty));

        final String array1 = "test";
        final String array2 = "try";
        final String array3 = "test";
        final String array4 = "TesT";

        this.assertFalse("String not equals to null", HelperForNfc.equalsEmptyNull(array1, null));
        this.assertFalse("String not equals to null", HelperForNfc.equalsEmptyNull(null, array1));
        this.assertFalse("String not equals to empty", HelperForNfc.equalsEmptyNull(array1, empty));
        this.assertFalse("String not equals to empty", HelperForNfc.equalsEmptyNull(empty, array1));
        this.assertTrue("String equals to himself", HelperForNfc.equalsEmptyNull(array1, array1));
        this.assertTrue(null, HelperForNfc.equalsEmptyNull(array1, array3));
        this.assertTrue(null, HelperForNfc.equalsEmptyNull(array3, array1));
        this.assertFalse(null, HelperForNfc.equalsEmptyNull(array1, array2));
        this.assertFalse(null, HelperForNfc.equalsEmptyNull(array2, array1));
        this.assertFalse(null, HelperForNfc.equalsEmptyNull(array1, array4));
        this.assertFalse(null, HelperForNfc.equalsEmptyNull(array4, array1));
    }

    /**
     * Test of {@link HelperForNfc#equalsNull(Object, Object)}
     */
    private void testEqualsNull() {
        this.assertTrue("null equals null",
                HelperForNfc.equalsNull(null, null));

        final String array1 = "test";
        final String array2 = "try";
        final String array3 = "test";

        this.assertFalse("object not equals null",
                HelperForNfc.equalsNull(array1, null));
        this.assertFalse("object not equals null",
                HelperForNfc.equalsNull(null, array1));
        this.assertFalse(null,
                HelperForNfc.equalsNull(array1, array2));
        this.assertFalse(null,
                HelperForNfc.equalsNull(array2, array1));
        this.assertTrue(null,
                HelperForNfc.equalsNull(array1, array3));
        this.assertTrue(null,
                HelperForNfc.equalsNull(array3, array1));
    }

    /**
     * Test of
     * {@link HelperForNfc#exchange(org.opennfc.nfctag.NfcTagConnection, byte[], int)}
     */
    private void testExchange() {
        final byte[] data = {
                (byte) 0, (byte) 1, (byte) 2, (byte) 3, (byte) 4, (byte) 5
        };

        MockNfcTagConnection mockNfcTagConnection = new MockNfcTagConnection(1000, null,
                MockNfcTagConnection.NO_MODE);

        try {
            HelperForNfc.exchange(mockNfcTagConnection, data, 250);

            this.assertFail("Must have a timeout exception here");
        } catch (final NfcException exception) {
            this.assertEquals("Must be a time out exception", exception.getCode(),
                    NfcErrorCode.TIMEOUT);
        }

        mockNfcTagConnection = new MockNfcTagConnection(1, new NfcException(
                NfcErrorCode.BAD_PARAMETER), MockNfcTagConnection.NO_MODE);
        try {
            HelperForNfc.exchange(mockNfcTagConnection, data, 250);

            this.assertFail("Must have a bad parameter exception here");
        } catch (final NfcException exception) {
            this.assertEquals("Must be a bad parameter exception", exception.getCode(),
                    NfcErrorCode.BAD_PARAMETER);
        }

        mockNfcTagConnection = new MockNfcTagConnection(1, null, MockNfcTagConnection.NO_MODE);
        try {
            this.assertEquals(null, HelperForNfc.exchange(mockNfcTagConnection, data, 250), data);

        } catch (final NfcException exception) {
            this.assertFail("Musn't have an exception here : " + exception + " | "
                    + exception.getCode());
        }
    }

    /**
     * Test of
     * {@link HelperForNfc#parseWellKnownUriRecord(android.nfc.NdefRecord)}
     */
    private void testParseWellKnownUriRecord() {
        final byte[] address = "google.fr".getBytes(Charset.forName("UTF-8"));
        final int length = address.length;
        final byte[] payload = new byte[length + 1];
        payload[0] = (byte) 1;
        System.arraycopy(address, 0, payload, 1, length);

        android.nfc.NdefRecord record = new android.nfc.NdefRecord(
                android.nfc.NdefRecord.TNF_WELL_KNOWN, android.nfc.NdefRecord.RTD_URI, new byte[0],
                payload);

        this.assertEquals(null, HelperForNfc.parseWellKnownUriRecord(record).toString(),
                "http://www.google.fr");

        payload[0] = (byte) 2;
        record = new android.nfc.NdefRecord(
                android.nfc.NdefRecord.TNF_WELL_KNOWN, android.nfc.NdefRecord.RTD_URI, new byte[0],
                payload);
        this.assertEquals(null, HelperForNfc.parseWellKnownUriRecord(record).toString(),
                "https://www.google.fr");

        for (int i = HelperForNfc.URI_PREFIX_MAP.length - 1; i >= 0; i--) {
            payload[0] = (byte) (i & 0xFF);
            record = new android.nfc.NdefRecord(
                    android.nfc.NdefRecord.TNF_WELL_KNOWN, android.nfc.NdefRecord.RTD_URI,
                    new byte[0],
                    payload);
            this.assertEquals("Prefix : " + HelperForNfc.URI_PREFIX_MAP[i],
                    HelperForNfc.parseWellKnownUriRecord(record).toString(),
                    HelperForNfc.URI_PREFIX_MAP[i] + "google.fr");
        }
    }

    /**
     * Test of {@link HelperForNfc#stringToByteArray(String)}
     */
    private void testStringToByteArray() {
        try {
            HelperForNfc.stringToByteArray(null);

            this.assertFail("Null must return an exception");
        } catch (final Exception exception) {
            // Thats what we want
        }

        final byte[] array = {
                (byte) ('a' & 0xFF), (byte) ('b' & 0xFF), (byte) ('c' & 0xFF), (byte) ('d' & 0xFF),
                (byte) ('e' & 0xFF), (byte) ('f' & 0xFF)
        };

        this.assertEquals(null, HelperForNfc.stringToByteArray("abcdef"), array);
    }

    /**
     * Test of {@link HelperForNfc#toHexadecimal(byte[])}
     */
    private void testToHexadecimal() {
        this.assertEquals("null gives empty string", HelperForNfc.toHexadecimal(null), "");
        this.assertEquals("empty array gives empty string",
                HelperForNfc.toHexadecimal(new byte[0]), "");
        this.assertEquals(
                null,
                HelperForNfc.toHexadecimal(new byte[] {
                        (byte) 0xca, (byte) 0xfe, (byte) 0x00, (byte) 0x0f, (byte) 0xac,
                        (byte) 0x0e, (byte) 0x1a, (byte) 0xa1, (byte) 0x15, (byte) 0x51
                }), "cafe000fac0e1aa11551");
    }
}
