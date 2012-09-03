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

import org.opennfc.cardlistener.Connection;
import org.opennfc.nfctag.NfcTagConnection;
import org.opennfc.tests.MockNfcTagConnection;

import android.nfc.NdefMessage;
import android.nfc.tech.IsoDep;
import android.nfc.tech.Ndef;
import android.nfc.tech.NfcA;
import android.nfc.tech.NfcB;
import android.nfc.tech.TagTechnology;
import android.os.Bundle;
import android.util.Log;

import java.util.Arrays;

/**
 * Convert Open NFC information to Google API information
 */
public class ConvertInformationNfc {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = ConvertInformationNfc.class.getSimpleName();

    /**
     * Create a converted information from a tag connection
     * 
     * @param connection Tag connection
     * @param ndefMessage Message read on tag
     * @param actualSize Actual message size
     * @return Converted information
     */
    public static ConvertInformationNfc create(final NfcTagConnection connection, final android.nfc.NdefMessage ndefMessage,
            final int actualSize) {
        if (ConvertInformationNfc.DEBUG) {
            Log.d(ConvertInformationNfc.TAG, "***************************************************");
            Log.d(ConvertInformationNfc.TAG, "***   " + connection.getClass().getName() + "   ***");
            Log.d(ConvertInformationNfc.TAG, "***************************************************");
        }

        if (connection instanceof NfcType1ConnectionImpl) {
            return ConvertInformationNfc.createNfcType1ConnectionImpl((NfcType1ConnectionImpl) connection, ndefMessage,
                    actualSize);
        }

        if (connection instanceof NfcType2ConnectionImpl) {
            return ConvertInformationNfc.createNfcType2ConnectionImpl((NfcType2ConnectionImpl) connection, ndefMessage,
                    actualSize);
        }

        if (connection instanceof NfcType3ConnectionImpl) {
            return ConvertInformationNfc.createNfcType3ConnectionImpl((NfcType3ConnectionImpl) connection, ndefMessage,
                    actualSize);
        }

        if (connection instanceof NfcType4AConnectionImpl) {
            return ConvertInformationNfc.createNfcType4ConnectionImpl((NfcType4AConnectionImpl) connection, ndefMessage,
                    actualSize);
        }

        if (connection instanceof NfcType4BConnectionImpl) {
            return ConvertInformationNfc.createNfcType4ConnectionImpl((NfcType4BConnectionImpl) connection, ndefMessage,
                    actualSize);
        }

        if (connection instanceof MydConnectionImpl) {
            return ConvertInformationNfc.createMydConnectionImpl((MydConnectionImpl) connection, ndefMessage, actualSize);
        }

        if (connection instanceof MockNfcTagConnection) {
            return ConvertInformationNfc.createMockNfcTagConnection((MockNfcTagConnection) connection, ndefMessage, actualSize);
        }

        // switch (connectionProperty) {
        // case ISO_14443_4_A:
        // bundle.putByteArray(IsoDep.EXTRA_HIST_BYTES,
        // ((Iso14443Part4AConnection) connection).getApplicationData());
        //
        // technologies[index] = TagTechnology.ISO_DEP;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case ISO_14443_4_B:
        // bundle.putByteArray(IsoDep.EXTRA_HI_LAYER_RESP, new byte[] {
        // ((Iso14443Part4BConnection) connection).getNad()
        // });
        //
        // technologies[index] = TagTechnology.ISO_DEP;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case ISO_14443_3_A:
        // s = ((Iso14443Part3AConnection) connection).getAtqa();
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
        // (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
        // });
        //
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (
        // ((Iso14443Part3AConnection) connection).getSak() & (short)
        // 0xFF));
        //
        // technologies[index] = TagTechnology.NFC_A;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case ISO_14443_3_B:
        // bytes = ((Iso14443Part3BConnection) connection).getAtqb();
        // bundle.putByteArray(NfcB.EXTRA_APPDATA, Arrays.copyOfRange(bytes,
        // 5, 9));
        // bundle.putByteArray(NfcB.EXTRA_PROTINFO,
        // Arrays.copyOfRange(bytes, 9, 12));
        //
        // technologies[index] = TagTechnology.NFC_B;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case ISO_15693_2:
        // case ISO_15693_3:
        // bundle.putByte(NfcV.EXTRA_DSFID,
        // ((Iso15693Part3Connection) connection).getDsfid());
        // bundle.putByte(NfcV.EXTRA_RESP_FLAGS, (byte) 0);
        // // 0 = b(00000000) = RRRRXRRE
        // // R : RFU (fixed)
        // // X : Extension_flag : 0 -> No protocol format extension.
        // // .................... 1 -> Protocol format is extended.
        // // Reserved for future use.
        // // E : Error_flag : 0 -> No error
        // // ................ 1 -> Error detected. Error code is in
        // // the "Error" field.
        //
        // technologies[index] = TagTechnology.NFC_V;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case MIFARE_1K:
        // // s = ((Iso14443Part3AConnection) connection).getAtqa();
        // // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
        // // (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
        // // });
        // //
        // // bundle.putShort(NfcA.EXTRA_SAK, (short) (
        // // ((Iso14443Part3AConnection) connection).getSak() &
        // // (short) 0xFF));
        //
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (0x08)); // See :
        // // android.nfc.tech.MifareClassic
        //
        // technologies[index] = TagTechnology.MIFARE_CLASSIC;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case MIFARE_4K:
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (0x38)); // See :
        // // android.nfc.tech.MifareClassic
        //
        // technologies[index] = TagTechnology.MIFARE_CLASSIC;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case MIFARE_PLUS_S_2K:
        // case MIFARE_PLUS_X_2K:
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (0x10)); // See :
        // // android.nfc.tech.MifareClassic
        //
        // technologies[index] = TagTechnology.MIFARE_CLASSIC;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case MIFARE_PLUS_S_4K:
        // case MIFARE_PLUS_X_4K:
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (0x11)); // See :
        // // android.nfc.tech.MifareClassic
        //
        // technologies[index] = TagTechnology.MIFARE_CLASSIC;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case MIFARE_MINI:
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (0x09)); // See :
        // // android.nfc.tech.MifareClassic
        //
        // technologies[index] = TagTechnology.MIFARE_CLASSIC;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case MIFARE_UL:
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
        // // android.nfc.tech.MifareUltralight
        //
        // technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case MIFARE_UL_C:
        // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
        // bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
        // // android.nfc.tech.MifareUltralight
        //
        // technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
        // bundles[index] = bundle;
        // index++;
        // break;
        // case NFC_TAG_TYPE_1:
        // case NFC_TAG_TYPE_2:
        // case NFC_TAG_TYPE_3:
        // case NFC_TAG_TYPE_4_A:
        // case NFC_TAG_TYPE_4_B:
        // try {
        // bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH,
        // ((NfcTagConnection) connection).getFreeSpaceSize() + actualSize);
        // } catch (Exception exception) {
        // if (DEBUG)
        // Log.d(TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
        // }
        //
        // try {
        // bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
        // ((NfcTagConnection) connection).isReadOnly() == true
        // ? Ndef.NDEF_MODE_READ_ONLY
        // : Ndef.NDEF_MODE_READ_WRITE);
        // } catch (Exception exception) {
        // if (DEBUG)
        // Log.d(TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
        // }
        //
        // bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
        //
        // switch (connectionProperty) {
        // case NFC_TAG_TYPE_1:
        // bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_1);
        // break;
        // case NFC_TAG_TYPE_2:
        // bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_2);
        // break;
        // case NFC_TAG_TYPE_3:
        // bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_3);
        // break;
        // case NFC_TAG_TYPE_4_A:
        // case NFC_TAG_TYPE_4_B:
        // bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_4);
        // break;
        // }
        //
        // technologies[index] = TagTechnology.NDEF;
        // bundles[index] = bundle;
        // index++;
        // break;
        // default:
        // if (DEBUG)
        // Log.v(TAG, "Sorry we can't manage the card type : " +
        // connectionProperty);
        // break;
        // }

        return null;
    }

    /**
     * Create a converted information from a connection
     * 
     * @param connection Connection to read
     * @return Extracted information
     */
    public static ConvertInformationNfc createConnection(final Connection connection) {
        if (ConvertInformationNfc.DEBUG) {
            Log.d(ConvertInformationNfc.TAG, "connection=" + connection + " | " + connection.getClass().getName());
        }
        final ConnectionProperty[] properties = connection.getProperties();

        final int length = properties.length;
        final int[] technologies = new int[length + 1];
        final Bundle[] bundles = new Bundle[length + 1];
        int index = 0;
        Bundle bundle;
        short s;
        int handle = -1;
        byte[] id = null;

        if (connection instanceof Iso14443Part3AConnectionImpl) {

            final Iso14443Part3AConnectionImpl iso14443Part3AConnectionImpl = (Iso14443Part3AConnectionImpl) connection;

            bundle = new Bundle();

            s = iso14443Part3AConnectionImpl.getAtqa();
            bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
                    (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
            });

            bundle.putShort(NfcA.EXTRA_SAK, (short) (iso14443Part3AConnectionImpl.getSak() & (short) 0xFF));

            technologies[index] = TagTechnology.NFC_A;
            bundles[index] = bundle;
            index++;

            handle = iso14443Part3AConnectionImpl.getNonNullHandle();
            id = iso14443Part3AConnectionImpl.getUid();
        }

        if (handle < 0) {
            return null;
        }

        final ConvertInformationNfc convertInformationNfc = new ConvertInformationNfc(
                Arrays.copyOfRange(technologies, 0, index), Arrays.copyOfRange(bundles, 0, index), handle);

        convertInformationNfc.overrideID = id;

        return convertInformationNfc;
    }

    /**
     * Extract information from a mock tag connection
     * 
     * @param connection Connection where extract information
     * @param ndefMessage Linked message
     * @param actualSize Actual message size
     * @return Converted information
     */
    private static ConvertInformationNfc createMockNfcTagConnection(final MockNfcTagConnection connection,
            final NdefMessage ndefMessage, final int actualSize) {

        final int[] technologies = {
                TagTechnology.ISO_DEP, TagTechnology.NDEF
        };
        final Bundle[] bundles = {
                new Bundle(), new Bundle()
        };

        return new ConvertInformationNfc(technologies, bundles, connection.getHandle());
    }

    static ConvertInformationNfc createMydConnectionImpl(MydConnectionImpl connection, NdefMessage ndefMessage, int actualSize) {
        if (ConvertInformationNfc.DEBUG) {
            Log.d(ConvertInformationNfc.TAG, "TYPE 2 !!!");
        }

        final ConnectionProperty[] properties = connection.getProperties();

        ConnectionProperty connectionProperty;

        final int length = properties.length;
        final int[] technologies = new int[length + 1];
        final Bundle[] bundles = new Bundle[length + 1];
        int index = 0;
        Bundle bundle;
        short s;
        byte[] bytes;
        Iso14443Part3AConnectionImpl iso14443Part3AConnectionImpl;
        Iso14443Part3BConnectionImpl iso14443Part3BConnectionImpl;

        for (int i = 0; i < length; i++) {
            connectionProperty = properties[i];
            if (ConvertInformationNfc.DEBUG) {
                Log.d(ConvertInformationNfc.TAG, "connectionProperty=" + connectionProperty + " | "
                        + connection.getClass().getName());
            }

            bundle = new Bundle();

            if (connectionProperty == null) {
                if (ConvertInformationNfc.DEBUG) {
                    Log.d(ConvertInformationNfc.TAG, "Type 2 : some property are null");
                }

                continue;
            }

            switch (connectionProperty) {
                case ISO_14443_3_A:
                    iso14443Part3AConnectionImpl = new Iso14443Part3AConnectionImpl(new SafeHandle(
                            connection.getNonNullHandle()));
                    s = iso14443Part3AConnectionImpl.getAtqa();
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
                            (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
                    });

                    bundle.putShort(NfcA.EXTRA_SAK, (short) (iso14443Part3AConnectionImpl.getSak() & (short) 0xFF));

                    technologies[index] = TagTechnology.NFC_A;
                    bundles[index] = bundle;
                    index++;
                    break;
                case ISO_14443_3_B:
                    iso14443Part3BConnectionImpl = new Iso14443Part3BConnectionImpl(new SafeHandle(
                            connection.getNonNullHandle()));
                    bytes = iso14443Part3BConnectionImpl.getAtqb();
                    bundle.putByteArray(NfcB.EXTRA_APPDATA, Arrays.copyOfRange(bytes, 5, 9));
                    bundle.putByteArray(NfcB.EXTRA_PROTINFO, Arrays.copyOfRange(bytes, 9, 12));

                    technologies[index] = TagTechnology.NFC_B;
                    bundles[index] = bundle;
                    index++;
                    break;
                case BPRIME:
                    // Iso14443Part2BPrimeConnectionImpl iso14443Part2BPrimeConnectionImpl = new
                    // Iso14443Part2BPrimeConnectionImpl(new SafeHandle(
                    // connection.getNonNullHandle()));
                    // bytes = iso14443Part3BConnectionImpl.getAtqb();
                    // bundle.putByteArray(NfcB.EXTRA_APPDATA, Arrays.copyOfRange(bytes,
                    // 5, 9));
                    // bundle.putByteArray(NfcB.EXTRA_PROTINFO,
                    // Arrays.copyOfRange(bytes, 9, 12));

                    technologies[index] = TagTechnology.NFC_B;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL_C:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MY_D_MOVE:
                    // TODO find what to do for MY_D_MOVE
                    if (DEBUG)
                        Log.d(TAG, "TODO | TODO | TODO find what to do for MY_D_MOVE");
                    break;
                case MY_D_NFC:
                    // TODO find what to do for MY_D_NFC
                    if (DEBUG)
                        Log.d(TAG, "TODO | TODO | TODO find what to do for MY_D_NFC");
                    break;
                case NFC_TAG_TYPE_2:
                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, connection.getFreeSpaceSize() + actualSize);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
                        }
                    }

                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
                                connection.isReadOnly() == true
                                        ? Ndef.NDEF_MODE_READ_ONLY
                                        : Ndef.NDEF_MODE_READ_WRITE);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
                        }
                    }

                    if (ndefMessage != null) {
                        bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
                    }
                    bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_2);

                    technologies[index] = TagTechnology.NDEF;
                    bundles[index] = bundle;
                    index++;
                    break;
                default:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createMydConnectionImpl can't manage : " + connectionProperty);
                    }
                    break;
            }
        }

        try {
            if (connection.isReadOnly() == false) {
                technologies[index] = TagTechnology.NDEF_FORMATABLE;
                bundles[index] = new Bundle();
                index++;
            }
        } catch (final Exception exception) {
            if (ConvertInformationNfc.DEBUG) {
                Log.w(ConvertInformationNfc.TAG, "Read only test failed !", exception);
            }
        }

        return new ConvertInformationNfc(Arrays.copyOfRange(technologies, 0, index), Arrays.copyOfRange(bundles, 0, index),
                connection.getNonNullHandle());
    }

    /**
     * Create a converted information from a tag connection type 1
     * 
     * @param connection Tag connection
     * @param ndefMessage Message read on tag
     * @param actualSize Actual message size
     * @return Converted information
     */
    static ConvertInformationNfc createNfcType1ConnectionImpl(final NfcType1ConnectionImpl connection,
            final android.nfc.NdefMessage ndefMessage, final int actualSize) {
        final ConnectionProperty[] properties = connection.getProperties();

        ConnectionProperty connectionProperty;

        final int length = properties.length;
        final int[] technologies = new int[length + 1];
        final Bundle[] bundles = new Bundle[length + 1];
        int index = 0;
        Bundle bundle;
        short s;
        byte[] bytes;
        Iso14443Part3AConnectionImpl iso14443Part3AConnectionImpl;
        Iso14443Part3BConnectionImpl iso14443Part3BConnectionImpl;
        byte[] overide = null;
        boolean already = false;

        for (int i = 0; i < length; i++) {
            connectionProperty = properties[i];

            bundle = new Bundle();

            if (connectionProperty == null) {
                if (already == true) {
                    if (ConvertInformationNfc.DEBUG) {
                        Log.d(ConvertInformationNfc.TAG, "Already done !");
                    }
                    continue;
                }
                already = true;

                try {
                    bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, connection.getFreeSpaceSize() + actualSize);
                } catch (final Exception exception) {
                    if (ConvertInformationNfc.DEBUG) {
                        Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
                    }
                }

                try {
                    bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
                            connection.isReadOnly() == true
                                    ? Ndef.NDEF_MODE_READ_ONLY
                                    : Ndef.NDEF_MODE_READ_WRITE);
                } catch (final Exception exception) {
                    if (ConvertInformationNfc.DEBUG) {
                        Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
                    }
                }

                if (ndefMessage != null) {
                    bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
                }
                bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_1);

                technologies[index] = TagTechnology.NDEF;
                bundles[index] = bundle;
                index++;
            } else {
                switch (connectionProperty) {
                    case ISO_14443_3_A:
                        iso14443Part3AConnectionImpl = new Iso14443Part3AConnectionImpl(new SafeHandle(
                                connection.getNonNullHandle()));
                        s = iso14443Part3AConnectionImpl.getAtqa();
                        bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
                                (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
                        });

                        bundle.putShort(NfcA.EXTRA_SAK, (short) (iso14443Part3AConnectionImpl.getSak() & (short) 0xFF));

                        technologies[index] = TagTechnology.NFC_A;
                        bundles[index] = bundle;
                        index++;
                        break;
                    case ISO_14443_3_B:
                        iso14443Part3BConnectionImpl = new Iso14443Part3BConnectionImpl(new SafeHandle(
                                connection.getNonNullHandle()));
                        bytes = iso14443Part3BConnectionImpl.getAtqb();
                        bundle.putByteArray(NfcB.EXTRA_APPDATA, Arrays.copyOfRange(bytes, 5, 9));
                        bundle.putByteArray(NfcB.EXTRA_PROTINFO, Arrays.copyOfRange(bytes, 9, 12));

                        technologies[index] = TagTechnology.NFC_B;
                        bundles[index] = bundle;
                        index++;
                        break;
                    case TYPE_TOPAZ_512:
                        if (ConvertInformationNfc.DEBUG) {
                            Log.v(ConvertInformationNfc.TAG, "createNfcType1ConnectionImpl don't know now how manage topaz");
                        }
                        final TopazConnectionImpl topazConnectionImpl = new TopazConnectionImpl(new SafeHandle(
                                connection.getNonNullHandle()));
                        overide = topazConnectionImpl.getUid();
                        break;
                    case NFC_TAG_TYPE_1:
                        if (already == true) {
                            if (ConvertInformationNfc.DEBUG) {
                                Log.d(ConvertInformationNfc.TAG, "Already done 2 !");
                            }
                            continue;
                        }
                        already = true;

                        try {
                            bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, connection.getFreeSpaceSize() + actualSize);
                        } catch (final Exception exception) {
                            if (ConvertInformationNfc.DEBUG) {
                                Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
                            }
                        }

                        try {
                            bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
                                    connection.isReadOnly() == true
                                            ? Ndef.NDEF_MODE_READ_ONLY
                                            : Ndef.NDEF_MODE_READ_WRITE);
                        } catch (final Exception exception) {
                            if (ConvertInformationNfc.DEBUG) {
                                Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
                            }
                        }

                        if (ndefMessage != null) {
                            bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
                        }
                        bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_1);

                        technologies[index] = TagTechnology.NDEF;
                        bundles[index] = bundle;
                        index++;
                        break;
                    default:
                        if (ConvertInformationNfc.DEBUG) {
                            Log.v(ConvertInformationNfc.TAG, "createNfcType1ConnectionImpl can't manage : "
                                    + connectionProperty);
                        }
                        break;
                }
            }
        }

        try {
            if (connection.isReadOnly() == false) {
                technologies[index] = TagTechnology.NDEF_FORMATABLE;
                bundles[index] = new Bundle();
                index++;
            }
        } catch (final Exception exception) {
            if (ConvertInformationNfc.DEBUG) {
                Log.w(ConvertInformationNfc.TAG, "Read only test failed !", exception);
            }
        }

        final ConvertInformationNfc convertInformationNfc = new ConvertInformationNfc(
                Arrays.copyOfRange(technologies, 0, index), Arrays.copyOfRange(bundles, 0, index),
                connection.getNonNullHandle());
        convertInformationNfc.overrideID = overide;

        return convertInformationNfc;
    }

    /**
     * Create a converted information from a tag connection type 2
     * 
     * @param connection Tag connection
     * @param ndefMessage Message read on tag
     * @param actualSize Actual message size
     * @return Converted information
     */
    static ConvertInformationNfc createNfcType2ConnectionImpl(final NfcType2ConnectionImpl connection,
            final android.nfc.NdefMessage ndefMessage, final int actualSize) {
        if (ConvertInformationNfc.DEBUG) {
            Log.d(ConvertInformationNfc.TAG, "TYPE 2 !!!");
        }

        final ConnectionProperty[] properties = connection.getProperties();

        ConnectionProperty connectionProperty;

        final int length = properties.length;
        final int[] technologies = new int[length + 1];
        final Bundle[] bundles = new Bundle[length + 1];
        int index = 0;
        Bundle bundle;
        short s;
        byte[] bytes;
        Iso14443Part3AConnectionImpl iso14443Part3AConnectionImpl;
        Iso14443Part3BConnectionImpl iso14443Part3BConnectionImpl;

        for (int i = 0; i < length; i++) {
            connectionProperty = properties[i];
            if (ConvertInformationNfc.DEBUG) {
                Log.d(ConvertInformationNfc.TAG, "connectionProperty=" + connectionProperty + " | "
                        + connection.getClass().getName());
            }

            bundle = new Bundle();

            if (connectionProperty == null) {
                if (ConvertInformationNfc.DEBUG) {
                    Log.d(ConvertInformationNfc.TAG, "Type 2 : some property are null");
                }

                continue;
            }

            switch (connectionProperty) {
                case ISO_14443_3_A:
                    iso14443Part3AConnectionImpl = new Iso14443Part3AConnectionImpl(new SafeHandle(
                            connection.getNonNullHandle()));
                    s = iso14443Part3AConnectionImpl.getAtqa();
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
                            (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
                    });

                    bundle.putShort(NfcA.EXTRA_SAK, (short) (iso14443Part3AConnectionImpl.getSak() & (short) 0xFF));

                    technologies[index] = TagTechnology.NFC_A;
                    bundles[index] = bundle;
                    index++;
                    break;
                case ISO_14443_3_B:
                    iso14443Part3BConnectionImpl = new Iso14443Part3BConnectionImpl(new SafeHandle(
                            connection.getNonNullHandle()));
                    bytes = iso14443Part3BConnectionImpl.getAtqb();
                    bundle.putByteArray(NfcB.EXTRA_APPDATA, Arrays.copyOfRange(bytes, 5, 9));
                    bundle.putByteArray(NfcB.EXTRA_PROTINFO, Arrays.copyOfRange(bytes, 9, 12));

                    technologies[index] = TagTechnology.NFC_B;
                    bundles[index] = bundle;
                    index++;
                    break;
                case BPRIME:
                    // Iso14443Part2BPrimeConnectionImpl iso14443Part2BPrimeConnectionImpl = new
                    // Iso14443Part2BPrimeConnectionImpl(new SafeHandle(
                    // connection.getNonNullHandle()));
                    // bytes = iso14443Part3BConnectionImpl.getAtqb();
                    // bundle.putByteArray(NfcB.EXTRA_APPDATA, Arrays.copyOfRange(bytes,
                    // 5, 9));
                    // bundle.putByteArray(NfcB.EXTRA_PROTINFO,
                    // Arrays.copyOfRange(bytes, 9, 12));

                    technologies[index] = TagTechnology.NFC_B;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL_C:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case NFC_TAG_TYPE_2:
                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, connection.getFreeSpaceSize() + actualSize);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
                        }
                    }

                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
                                connection.isReadOnly() == true
                                        ? Ndef.NDEF_MODE_READ_ONLY
                                        : Ndef.NDEF_MODE_READ_WRITE);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
                        }
                    }

                    if (ndefMessage != null) {
                        bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
                    }
                    bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_2);

                    technologies[index] = TagTechnology.NDEF;
                    bundles[index] = bundle;
                    index++;
                    break;
                default:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createNfcType2ConnectionImpl can't manage : " + connectionProperty);
                    }
                    break;
            }
        }

        try {
            if (connection.isReadOnly() == false) {
                technologies[index] = TagTechnology.NDEF_FORMATABLE;
                bundles[index] = new Bundle();
                index++;
            }
        } catch (final Exception exception) {
            if (ConvertInformationNfc.DEBUG) {
                Log.w(ConvertInformationNfc.TAG, "Read only test failed !", exception);
            }
        }

        return new ConvertInformationNfc(Arrays.copyOfRange(technologies, 0, index), Arrays.copyOfRange(bundles, 0, index),
                connection.getNonNullHandle());
    }

    /**
     * Create a converted information from a tag connection type 3
     * 
     * @param connection Tag connection
     * @param ndefMessage Message read on tag
     * @param actualSize Actual message size
     * @return Converted information
     */
    static ConvertInformationNfc createNfcType3ConnectionImpl(final NfcType3ConnectionImpl connection,
            final android.nfc.NdefMessage ndefMessage, final int actualSize) {
        final ConnectionProperty[] properties = connection.getProperties();

        ConnectionProperty connectionProperty;

        final int length = properties.length;
        final int[] technologies = new int[length + 1];
        final Bundle[] bundles = new Bundle[length + 1];
        int index = 0;
        Bundle bundle;
        for (int i = 0; i < length; i++) {
            connectionProperty = properties[i];

            bundle = new Bundle();

            if (connectionProperty == null) {
                if (ConvertInformationNfc.DEBUG) {
                    Log.d(ConvertInformationNfc.TAG, "Type 3 : some property are null");
                }

                continue;
            }

            switch (connectionProperty) {
                case TYPE_FELICA:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createNfcType3ConnectionImpl don't know now how manage felica");
                    }
                    break;
                case NFC_TAG_TYPE_3:
                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, connection.getFreeSpaceSize() + actualSize);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
                        }
                    }

                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
                                connection.isReadOnly() == true
                                        ? Ndef.NDEF_MODE_READ_ONLY
                                        : Ndef.NDEF_MODE_READ_WRITE);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
                        }
                    }

                    if (ndefMessage != null) {
                        bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
                    }
                    bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_3);

                    technologies[index] = TagTechnology.NDEF;
                    bundles[index] = bundle;
                    index++;
                    break;
                default:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createNfcType3ConnectionImpl can't manage : " + connectionProperty);
                    }
                    break;
            }
        }

        try {
            if (connection.isReadOnly() == false) {
                technologies[index] = TagTechnology.NDEF_FORMATABLE;
                bundles[index] = new Bundle();
                index++;
            }
        } catch (final Exception exception) {
            if (ConvertInformationNfc.DEBUG) {
                Log.w(ConvertInformationNfc.TAG, "Read only test failed !", exception);
            }
        }

        return new ConvertInformationNfc(Arrays.copyOfRange(technologies, 0, index), Arrays.copyOfRange(bundles, 0, index),
                connection.getNonNullHandle());
    }

    /**
     * Create a converted information from a tag connection type 4-A
     * 
     * @param connection Tag connection
     * @param ndefMessage Message read on tag
     * @param actualSize Actual message size
     * @return Converted information
     */
    static ConvertInformationNfc createNfcType4ConnectionImpl(final NfcType4AConnectionImpl connection,
            final android.nfc.NdefMessage ndefMessage, final int actualSize) {
        final ConnectionProperty[] properties = connection.getProperties();

        ConnectionProperty connectionProperty;

        final int length = properties.length;
        final int[] technologies = new int[length + 1];
        final Bundle[] bundles = new Bundle[length + 1];
        int index = 0;
        Bundle bundle;
        Iso14443Part4AConnectionImpl iso14443Part4AConnectionImpl;

        for (int i = 0; i < length; i++) {
            connectionProperty = properties[i];

            bundle = new Bundle();

            if (connectionProperty == null) {
                if (ConvertInformationNfc.DEBUG) {
                    Log.d(ConvertInformationNfc.TAG, "Type 4A : some property are null");
                }

                continue;
            }

            switch (connectionProperty) {
                case MIFARE_1K:
                    // s = connection.getAtqa();
                    // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
                    // (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
                    // });
                    //
                    // bundle.putShort(NfcA.EXTRA_SAK, (short) (
                    // connection.getSak() &
                    // (short) 0xFF));

                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x08)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_4K:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x38)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_PLUS_S_2K:
                case MIFARE_PLUS_X_2K:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x10)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_PLUS_S_4K:
                case MIFARE_PLUS_X_4K:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x11)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_MINI:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x09)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL_C:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_DESFIRE_D40:
                case MIFARE_DESFIRE_EV1_2K:
                case MIFARE_DESFIRE_EV1_4K:
                case MIFARE_DESFIRE_EV1_8K:
                case ISO_7816_4:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createNfcType4ConnectionImpl don't know now how manage : "
                                + connectionProperty);
                    }
                    break;
                case ISO_14443_4_A:
                    iso14443Part4AConnectionImpl = new Iso14443Part4AConnectionImpl(new SafeHandle(
                            connection.getNonNullHandle()));

                    bundle.putByteArray(IsoDep.EXTRA_HIST_BYTES, iso14443Part4AConnectionImpl.getApplicationData());

                    technologies[index] = TagTechnology.ISO_DEP;
                    bundles[index] = bundle;
                    index++;
                    break;
                case NFC_TAG_TYPE_4_A:
                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, connection.getFreeSpaceSize() + actualSize);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
                        }
                    }

                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
                                connection.isReadOnly() == true
                                        ? Ndef.NDEF_MODE_READ_ONLY
                                        : Ndef.NDEF_MODE_READ_WRITE);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
                        }
                    }

                    if (ndefMessage != null) {
                        bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
                    }
                    bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_4);

                    technologies[index] = TagTechnology.NDEF;
                    bundles[index] = bundle;
                    index++;
                    break;
                default:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createNfcType4ConnectionImpl can't manage : " + connectionProperty);
                    }
                    break;
            }
        }

        try {
            if (connection.isReadOnly() == false) {
                technologies[index] = TagTechnology.NDEF_FORMATABLE;
                bundles[index] = new Bundle();
                index++;
            }
        } catch (final Exception exception) {
            if (ConvertInformationNfc.DEBUG) {
                Log.w(ConvertInformationNfc.TAG, "Read only test failed !", exception);
            }
        }

        return new ConvertInformationNfc(Arrays.copyOfRange(technologies, 0, index), Arrays.copyOfRange(bundles, 0, index),
                connection.getNonNullHandle());
    }

    /**
     * Create a converted information from a tag connection type 4-B
     * 
     * @param connection Tag connection
     * @param ndefMessage Message read on tag
     * @param actualSize Actual message size
     * @return Converted information
     */
    static ConvertInformationNfc createNfcType4ConnectionImpl(final NfcType4BConnectionImpl connection,
            final android.nfc.NdefMessage ndefMessage, final int actualSize) {
        final ConnectionProperty[] properties = connection.getProperties();

        ConnectionProperty connectionProperty;

        final int length = properties.length;
        final int[] technologies = new int[length + 1];
        final Bundle[] bundles = new Bundle[length + 1];
        int index = 0;
        Bundle bundle;
        Iso14443Part4BConnectionImpl iso14443Part4BConnectionImpl;

        for (int i = 0; i < length; i++) {
            connectionProperty = properties[i];

            bundle = new Bundle();

            if (connectionProperty == null) {
                if (ConvertInformationNfc.DEBUG) {
                    Log.d(ConvertInformationNfc.TAG, "Type 4B : some property are null");
                }

                continue;
            }

            switch (connectionProperty) {
                case MIFARE_1K:
                    // s = connection.getAtqa();
                    // bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[] {
                    // (byte) (s >> 8 & 0xFF), (byte) (s & 0xFF)
                    // });
                    //
                    // bundle.putShort(NfcA.EXTRA_SAK, (short) (
                    // connection.getSak() &
                    // (short) 0xFF));

                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x08)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_4K:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x38)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_PLUS_S_2K:
                case MIFARE_PLUS_X_2K:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x10)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_PLUS_S_4K:
                case MIFARE_PLUS_X_4K:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x11)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_MINI:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x09)); // See :
                    // android.nfc.tech.MifareClassic

                    technologies[index] = TagTechnology.MIFARE_CLASSIC;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_UL_C:
                    bundle.putByteArray(NfcA.EXTRA_ATQA, new byte[0]);
                    bundle.putShort(NfcA.EXTRA_SAK, (short) (0x00)); // See :
                    // android.nfc.tech.MifareUltralight

                    technologies[index] = TagTechnology.MIFARE_ULTRALIGHT;
                    bundles[index] = bundle;
                    index++;
                    break;
                case MIFARE_DESFIRE_D40:
                case MIFARE_DESFIRE_EV1_2K:
                case MIFARE_DESFIRE_EV1_4K:
                case MIFARE_DESFIRE_EV1_8K:
                case ISO_7816_4:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createNfcType4ConnectionImpl don't know now how manage : "
                                + connectionProperty);
                    }
                    break;
                case ISO_14443_4_B:
                    iso14443Part4BConnectionImpl = new Iso14443Part4BConnectionImpl(new SafeHandle(
                            connection.getNonNullHandle()));

                    bundle.putByteArray(IsoDep.EXTRA_HI_LAYER_RESP, new byte[] {
                        iso14443Part4BConnectionImpl.getNad()
                    });

                    technologies[index] = TagTechnology.ISO_DEP;
                    bundles[index] = bundle;
                    index++;
                    break;
                case NFC_TAG_TYPE_4_B:
                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, connection.getFreeSpaceSize() + actualSize);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_MAXLENGTH get failed", exception);
                        }
                    }

                    try {
                        bundle.putInt(Ndef.EXTRA_NDEF_CARDSTATE,//
                                connection.isReadOnly() == true
                                        ? Ndef.NDEF_MODE_READ_ONLY
                                        : Ndef.NDEF_MODE_READ_WRITE);
                    } catch (final Exception exception) {
                        if (ConvertInformationNfc.DEBUG) {
                            Log.d(ConvertInformationNfc.TAG, "EXTRA_NDEF_CARDSTATE get failed", exception);
                        }
                    }

                    if (ndefMessage != null) {
                        bundle.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMessage);
                    }
                    bundle.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_4);

                    technologies[index] = TagTechnology.NDEF;
                    bundles[index] = bundle;
                    index++;
                    break;
                default:
                    if (ConvertInformationNfc.DEBUG) {
                        Log.v(ConvertInformationNfc.TAG, "createNfcType4ConnectionImpl can't manage : " + connectionProperty);
                    }
                    break;
            }
        }

        try {
            if (connection.isReadOnly() == false) {
                technologies[index] = TagTechnology.NDEF_FORMATABLE;
                bundles[index] = new Bundle();
                index++;
            }
        } catch (final Exception exception) {
            if (ConvertInformationNfc.DEBUG) {
                Log.w(ConvertInformationNfc.TAG, "Read only test failed !", exception);
            }
        }

        return new ConvertInformationNfc(Arrays.copyOfRange(technologies, 0, index), Arrays.copyOfRange(bundles, 0, index),
                connection.getNonNullHandle());
    }

    /** Information store for each technology */
    public final Bundle[] bundles;
    /** Handle to use */
    public int handle;
    /** ID for override */
    public byte[] overrideID;
    /** Technologies supported by the tag */
    public final int[] technologies;

    /**
     * Create converted information
     * 
     * @param technologies Technologies supported
     * @param bundles Information store for each technology
     * @param handle Handle to use
     */
    private ConvertInformationNfc(final int[] technologies, final Bundle[] bundles, final int handle) {
        this.technologies = technologies;
        this.bundles = bundles;
        this.handle = handle;
    }
}
