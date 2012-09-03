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

package org.opennfc;

import org.opennfc.nfctag.NfcTagConnection;
import org.opennfc.service.ThreadedTask;
import org.opennfc.service.ThreadedTaskManager;
import org.opennfc.tests.MockNfcTagConnection;

import android.net.Uri;
import android.nfc.ErrorCodes;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charsets;
import java.util.Arrays;

/**
 * Helpers/Utilities
 */
public class HelperForNfc {
    /**
     * Task do in separate thread for make exchange with the tag
     */
    final static class ExchangeTask implements ThreadedTask<Object> {
        /** Last exception append during the exchange */
        NfcException mNfcException;
        /** Answer of the tag */
        byte[] result;

        /**
         * Cancel the exchange
         * 
         * @see org.opennfc.service.ThreadedTask#cancel()
         */
        @Override
        public void cancel() {
            Log.w(HelperForNfc.TAG, "Exchange time out ! ");
        }

        /**
         * Do the exchange itself <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param taskID Task ID
         * @param parameters Task parameters. (NfcTagConnection : link with tag
         *            for communicate | byte[] : message data send to the tag)
         * @see org.opennfc.service.ThreadedTask#excuteTask(int, PARAMETER[])
         */
        @Override
        public void excuteTask(final int taskID, final Object... parameters) {
            final NfcTagConnection connection = (NfcTagConnection) parameters[0];
            final byte[] data = (byte[]) parameters[1];
            this.result = null;
            this.mNfcException = null;

            // Exchange data with the good instance

            try {
                if (connection instanceof NfcType1ConnectionImpl) {
                    this.result = ((NfcType1ConnectionImpl) connection).exchangeData(data);

                    return;
                }

                if (connection instanceof NfcType2ConnectionImpl) {
                    this.result = ((NfcType2ConnectionImpl) connection).exchangeData(data);

                    return;
                }

                if (connection instanceof NfcType3ConnectionImpl) {
                    Log.w(HelperForNfc.TAG, "No exchange in Type 3");

                    // this.result=
                    // ((NfcType3ConnectionImpl)connection).exchangeData(data);

                    return;
                }

                if (connection instanceof NfcType4AConnectionImpl) {
                    this.result = ((NfcType4AConnectionImpl) connection).exchangeData(data);

                    return;
                }

                if (connection instanceof NfcType4BConnectionImpl) {
                    this.result = ((NfcType4BConnectionImpl) connection).exchangeData(data);

                    return;
                }

                if (connection instanceof MockNfcTagConnection) {
                    this.result = ((MockNfcTagConnection) connection).exchangeData(data);

                    return;
                }

                if (HelperForNfc.DEBUG) {
                    Log.w(HelperForNfc.TAG, "We not actually manage : "
                            + connection.getClass().getName());
                }
            } catch (final NfcException nfcException) {
                this.mNfcException = nfcException;
            }
        }
    }

    /** Enable/disable debug */
    private static final boolean DEBUG = true;

    /** Threaded task for exchange with tag */
    final static ExchangeTask exchangeTask = new ExchangeTask();
    /** TAG use in debug */
    private static final String TAG = HelperForNfc.class.getSimpleName();

    /**
     * NFC Forum "URI Record Type Definition" This is a mapping of
     * "URI Identifier Codes" to URI string prefixes, per section 3.2.2 of the
     * NFC Forum URI Record Type Definition document.
     */
    public static final String[] URI_PREFIX_MAP = new String[] {
            "", // 0x00
            "http://www.", // 0x01
            "https://www.", // 0x02
            "http://", // 0x03
            "https://", // 0x04
            "tel:", // 0x05
            "mailto:", // 0x06
            "ftp://anonymous:anonymous@", // 0x07
            "ftp://ftp.", // 0x08
            "ftps://", // 0x09
            "sftp://", // 0x0A
            "smb://", // 0x0B
            "nfs://", // 0x0C
            "ftp://", // 0x0D
            "dav://", // 0x0E
            "news:", // 0x0F
            "telnet://", // 0x10
            "imap:", // 0x11
            "rtsp://", // 0x12
            "urn:", // 0x13
            "pop:", // 0x14
            "sip:", // 0x15
            "sips:", // 0x16
            "tftp:", // 0x17
            "btspp://", // 0x18
            "btl2cap://", // 0x19
            "btgoep://", // 0x1A
            "tcpobex://", // 0x1B
            "irdaobex://", // 0x1C
            "file://", // 0x1D
            "urn:epc:id:", // 0x1E
            "urn:epc:tag:", // 0x1F
            "urn:epc:pat:", // 0x20
            "urn:epc:raw:", // 0x21
            "urn:epc:", // 0x22
    };

    /**
     * Convert a byte array to String.<br>
     * It assumes that characters are encode with one byte (ASCCI), not with two
     * bytes (Unicode)
     * 
     * @param array Byte array to convert
     * @return Converted String
     */
    public static String byteArrayToString(final byte[] array) {
        final int length = array.length;
        final char[] charArray = new char[length];

        for (int i = 0; i < length; i++) {
            charArray[i] = (char) (array[i] & 0xFF);
        }

        return new String(charArray);
    }

    /**
     * Convert a byte array to String.<br>
     * It assumes that characters are encode with one byte (ASCCI), not with two
     * bytes (Unicode)
     * 
     * @param array Array to convert
     * @param offset Offset to start reading the array
     * @param length Number of characters to read
     * @return Converted String
     */
    public static String byteArrayToString(final byte[] array, final int offset, final int length) {
        if (offset < 0 || offset + length > array.length) {
            throw new IllegalArgumentException("offset=" + offset + " | length=" + length
                    + " an array length is " + array.length);
        }

        final char[] charArray = new char[length];

        for (int i = 0; i < length; i++) {
            charArray[i] = (char) (array[i + offset] & 0xFF);
        }

        return new String(charArray);
    }

    /**
     * Concatenate several arrays in one
     * 
     * @param arrays Arrays to concatenate
     * @return The full array
     */
    public static byte[] concat(final byte[]... arrays) {
        int length = 0;

        for (final byte[] array : arrays) {
            length += array.length;
        }

        final byte[] result = new byte[length];
        int pos = 0;

        for (final byte[] array : arrays) {
            System.arraycopy(array, 0, result, pos, array.length);
            pos += array.length;
        }

        return result;
    }

    /**
     * Deserialize an integer from a stream.<br>
     * In other words, it reads an integer from a stream.<br>
     * See {@link #serialize(int, OutputStream)} for serialize
     * 
     * @param inputStream Stream to read
     * @return Read integer
     * @throws IOException On reading issue
     */
    public static int deserializeInteger(final InputStream inputStream) throws IOException {
        return inputStream.read() << 24 | inputStream.read() << 16 | inputStream.read() << 8
                | inputStream.read();
    }

    /**
     * Deserialize an integer from a stream.<br>
     * In other words, it reads an integer from a stream.<br>
     * See {@link #serialize(String, OutputStream)} for serialize
     * 
     * @param inputStream Stream to read
     * @return Read string
     * @throws IOException On reading issue
     */
    public static String deserializeString(final InputStream inputStream) throws IOException {
        final int length = HelperForNfc.deserializeInteger(inputStream);

        if (length < 0) {
            return null;
        }

        final byte[] data = new byte[length];

        int read = inputStream.read(data);
        int index = read;

        while (read >= 0 && index < length) {
            read = inputStream.read(data, index, length - index);

            if (read >= 0) {
                index += read;
            }
        }

        if (index < 0) {
            index = 0;
        }

        return new String(data, 0, index, Charsets.UTF_8);
    }

    /**
     * Indicates if 2 arrays are equivalent.<br>
     * In other words, if tow arrays have the same content but not necessary the
     * same order, for example : [1, 2, 3 , 4, 5] , [2, 4, 1, 3, 5] , [5, 4, 3,
     * 2, 1] and [1, 3, 5, 2, 4] are consider equivalent. But [1, 2, 3, 4] is
     * different.<br>
     * Moreover {@code null} and empty array are consider equivalent
     * 
     * @param array1 First array to compare
     * @param array2 Second array to compare
     * @return {@code true} if they are equivalent
     */
    public static boolean doesArrayEquivalent(final int[] array1, final int[] array2) {
        return HelperForNfc.doesArrayInclude(array1, array2) == true
                && HelperForNfc.doesArrayInclude(array2, array1) == true;
    }

    /**
     * Indicates if an array is include inside an other.<br>
     * In other word, if all elements of an array are contains inside an other
     * one
     * 
     * @param container Array container
     * @param include Array tested if inside the container
     * @return {@code true} if the array is include in the container
     */
    public static boolean doesArrayInclude(final int[] container, final int[] include) {
        if (container == null || container.length == 0) {
            return include == null || include.length == 0;
        }

        if (include == null || include.length == 0) {
            return true;
        }

        Arrays.sort(container);

        for (final int test : include) {
            if (Arrays.binarySearch(container, test) < 0) {
                return false;
            }
        }

        return true;
    }

    /**
     * Indicates if an array is include inside an other.<br>
     * In other word, if all elements of an array are contains inside an other
     * one
     * 
     * @param container Array container
     * @param include Array tested if inside the container
     * @return {@code true} if the array is include in the container
     */
    public static boolean doesArrayInclude(final String[] container, final String[] include) {
        if (container == null || container.length == 0) {
            return include == null || include.length == 0;
        }

        if (include == null || include.length == 0) {
            return true;
        }

        Arrays.sort(container);

        for (final String test : include) {
            if (Arrays.binarySearch(container, test) < 0) {
                return false;
            }
        }

        return true;
    }

    /**
     * Compare to byte array.<br>
     * They are consider equals if :
     * <ul>
     * <li>They have the same size with the same content in same order</li>
     * <li><b>OR </b>They are {@code null} together</li>
     * <li><b>OR </b>One is {@code null}, other is empty</li>
     * </ul>
     * 
     * @param array1 First array to compare
     * @param array2 Second array to compare
     * @return Comparison result
     */
    public static boolean equalsEmptyNull(final byte[] array1, final byte[] array2) {
        if (array1 == null || array1.length == 0) {
            return array2 == null || array2.length == 0;
        }

        if (array2 == null || array1.length != array2.length) {
            return false;
        }

        for (int i = array1.length - 1; i >= 0; i--) {
            if (array1[i] != array2[i]) {
                return false;
            }
        }

        return true;
    }

    /**
     * Indicates if 2 strings are equals.<br>{@code null} string is consider equal
     * to empty string
     * 
     * @param string1 First string to compare
     * @param string2 Second string to compare
     * @return {@code true} if strings are equals
     */
    public static boolean equalsEmptyNull(final String string1, final String string2) {
        if (string1 == null) {
            return string2 == null || string2.length() == 0;
        }

        if (string2 == null) {
            return string1.length() == 0;
        }

        return string1.equals(string2);
    }

    /**
     * Indicates if 2 strings are equals in ignoring case.<br>{@code null} string is
     * consider equal to empty string
     * 
     * @param string1 First string to compare
     * @param string2 Second string to compare
     * @return {@code true} if strings are equals in ignoring case.
     */
    public static boolean equalsEmptyNullIgnoreCase(final String string1, final String string2) {
        if (string1 == null) {
            return string2 == null || string2.length() == 0;
        }

        if (string2 == null) {
            return string1.length() == 0;
        }

        return string1.equalsIgnoreCase(string2);
    }

    /**
     * Indicates if 2 object of same types are equals or {@code null} together
     * 
     * @param <T> Objects type
     * @param t1 First object
     * @param t2 Second object
     * @return {@code true} if equals
     */
    public static <T> boolean equalsNull(final T t1, final T t2) {
        if (t1 == null) {
            return t2 == null;
        }

        if (t2 == null) {
            return false;
        }

        return t1.equals(t2);
    }

    /**
     * Exchange data with tag connection
     * 
     * @param connection Tag connection to communicate with
     * @param data Data send to tag
     * @param timeout Limit time on exchange in milliseconds
     * @return The tag answer
     * @throws NfcException On timeout or exchange issue
     */
    public static byte[] exchange(final NfcTagConnection connection, final byte[] data,
            final int timeout)
            throws NfcException {
        if (ThreadedTaskManager.MANAGER.executeWithTimeOut(timeout,
                HelperForNfc.exchangeTask, connection, data) == false) {
            throw new NfcException(NfcErrorCode.TIMEOUT);
        }

        if (HelperForNfc.exchangeTask.mNfcException != null) {
            throw HelperForNfc.exchangeTask.mNfcException;
        }

        return HelperForNfc.exchangeTask.result;
    }

    /**
     * Compute error code associated to exception
     * 
     * @param nfcException Exception to find its corresponding error code
     * @param errorCode Error code by default if not found
     * @return Result error code
     */
    public static int obtainErrorCode(final NfcException nfcException, final int errorCode) {
        if (nfcException == null) {
            return errorCode;
        }

        switch (nfcException.getCode()) {
            case BAD_TAG_FORMAT:
            case BAD_HANDLE:
            case BAD_FIRMWARE_SIGNATURE:
            case BAD_PARAMETER:
            case BAD_NDEF_FORMAT:
            case BAD_FIRMWARE_VERSION:
            case BAD_FIRMWARE_FORMAT:
            case FEATURE_NOT_SUPPORTED:
            case FUNCTION_NOT_SUPPORTED:
            case HETEROGENEOUS_DATA:
            case INDEX_OUT_OF_RANGE:
            case MISSING_INFO:
            case NDEF_UNKNOWN:
            case RF_PROTOCOL_NOT_SUPPORTED:
            case TAG_DATA_INTEGRITY:
            case VERSION_NOT_SUPPORTED:
            case WRONG_RTD:
                return ErrorCodes.ERROR_INVALID_PARAM;
            case BAD_NFCC_MODE:
                return ErrorCodes.ERROR_NFC_ON;
            case CLIENT_SERVER_COMMUNICATION:
            case BAD_STATE:
            case CLIENT_SERVER_PROTOCOL:
            case DRIVER:
            case EXCLUSIVE_REJECTED:
            case ITEM_NOT_FOUND:
            case NFC_HAL_COMMUNICATION:
            case NFCC_COMMUNICATION:
            case PERSISTENT_DATA:
            case RF_COMMUNICATION:
            case SHARE_REJECTED:
            case SYNC_OBJECT:
            case TAG_NOT_LOCKABLE:
            case UICC_COMMUNICATION:
                return ErrorCodes.ERROR_IO;
            case BUFFER_TOO_LARGE:
            case OUT_OF_RESOURCE:
            case TOO_MANY_HANDLERS:
                return ErrorCodes.ERROR_INSUFFICIENT_RESOURCES;
            case TAG_FULL:
            case BUFFER_TOO_SHORT:
                return ErrorCodes.ERROR_BUFFER_TO_SMALL;
            case RETRY:
            case CANCEL:
            case WAIT_CANCELLED:
                return ErrorCodes.ERROR_CANCELLED;
            case CONNECTION_COMPATIBILITY:
                return ErrorCodes.ERROR_CONNECT;
            case DURING_FIRMWARE_BOOT:
            case DURING_HARDWARE_BOOT:
                return ErrorCodes.ERROR_BUSY;
            case ITEM_LOCKED:
            case LOCKED_TAG:
            case TAG_WRITE:
                return ErrorCodes.ERROR_WRITE;
            case NO_EVENT:
            case TIMEOUT:
                return ErrorCodes.ERROR_TIMEOUT;
            case P2P_CLIENT_REJECTED:
                return ErrorCodes.ERROR_SOCKET_CREATION;
        }

        return errorCode;
    }

    /**
     * Parse record to URI
     * 
     * @param record Record to parse
     * @return URI parsed
     */
    public static Uri parseWellKnownUriRecord(final android.nfc.NdefRecord record) {
        final byte[] payload = record.getPayload();

        /*
         * payload[0] contains the URI Identifier Code, per the NFC Forum
         * "URI Record Type Definition" section 3.2.2.
         * payload[1]...payload[payload.length - 1] contains the rest of the
         * URI.
         */
        final String prefix = HelperForNfc.URI_PREFIX_MAP[(payload[0] & 0xff)];
        final byte[] fullUri = HelperForNfc.concat(prefix.getBytes(Charsets.UTF_8),
                Arrays.copyOfRange(payload, 1, payload.length));
        return Uri.parse(new String(fullUri, Charsets.UTF_8));
    }

    /**
     * Serialize an integer in a stream.<br>
     * In other words, its write integer in the stream.<br>
     * See {@link #deserializeInteger(InputStream)} for deserialize
     * 
     * @param integer Integer to write
     * @param outputStream Stream where write
     * @throws IOException On writing issue
     */
    public static void serialize(final int integer, final OutputStream outputStream)
            throws IOException {
        outputStream.write(integer >> 24 & 0xFF);
        outputStream.write(integer >> 16 & 0xFF);
        outputStream.write(integer >> 8 & 0xFF);
        outputStream.write(integer & 0xFF);

        outputStream.flush();
    }

    /**
     * Serialize a string in a stream.<br>
     * In other words, its write string in the stream.<br>
     * See {@link #deserializeString(InputStream)} for deserialize
     * 
     * @param string String to write (can be {@code null})
     * @param outputStream Stream where write
     * @throws IOException On writing issue
     */
    public static void serialize(final String string, final OutputStream outputStream)
            throws IOException {
        if (string == null) {
            HelperForNfc.serialize(-1, outputStream);
        }

        final byte[] data = string.getBytes(Charsets.UTF_8);

        HelperForNfc.serialize(data.length, outputStream);

        outputStream.write(data);

        outputStream.flush();
    }

    /**
     * Make thread that call this method sleep for a time
     * 
     * @param millisecond Time to sleep in millisecond
     */
    public static void sleep(final int millisecond) {
        try {
            Thread.sleep(millisecond);
        } catch (final Exception exception) {
        }
    }

    /**
     * Transform a string to byte array (in ASCII).<br>
     * Can be revert with {@link #byteArrayToString(byte[])} or
     * {@link #byteArrayToString(byte[], int, int)}
     * 
     * @param string String to convert
     * @return Array of byte
     */
    public static byte[] stringToByteArray(final String string) {
        final char[] chars = string.toCharArray();
        final int length = chars.length;
        final byte[] array = new byte[length];

        for (int i = 0; i < length; i++) {
            array[i] = (byte) (chars[i] & 0xFF);
        }

        return array;
    }

    /**
     * Transform a byte array to hexa-decimal representation
     * 
     * @param array Array to convert
     * @return String representation
     */
    public static final String toHexadecimal(final byte[] array) {
        if (array == null) {
            return "";
        }

        final StringBuilder builder = new StringBuilder(array.length << 1);

        for (final byte b : array) {
            builder.append(Integer.toHexString((b & 0xF0) >> 4));
            builder.append(Integer.toHexString(b & 0xF));
        }

        return builder.toString();
    }
}
