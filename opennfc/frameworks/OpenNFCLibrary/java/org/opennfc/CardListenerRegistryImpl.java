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

package org.opennfc;

import java.util.Vector;

import org.opennfc.cardlistener.CardDetectionEventHandler;
import org.opennfc.cardlistener.CardListenerRegistry;

import android.util.Log;

/**
 * package private
 * 
 * @hide
 **/
final class CardListenerRegistryImpl implements CardListenerRegistry {

    /** The Reader listener registry */
    private Vector<ReaderCardDetectionHandler> cardListenerList = new Vector<ReaderCardDetectionHandler>();

    public void registerCardListener(int priority, CardDetectionEventHandler handler) throws NfcException {

        privateRegisterCardListener(priority, null, handler);
    }

    public void registerCardListener(int priority, ConnectionProperty[] properties, CardDetectionEventHandler handler)
            throws NfcException {

        if ((properties == null) || (properties.length == 0)) {
            throw new IllegalArgumentException("properties==null or empty");
        }

        privateRegisterCardListener(priority, properties, handler);
    }

    private void privateRegisterCardListener(int priority, ConnectionProperty[] properties, CardDetectionEventHandler handler)
            throws NfcException {

        if ((priority == NfcPriority.MAXIMUM) || (priority == NfcPriority.EXCLUSIVE)) {
            NfcSecurityManager.check(NfcSecurityManager.HARDWARE);
        }

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }

        ReaderCardDetectionHandler cardHandler = new ReaderCardDetectionHandler(handler);

        Vector<ReaderCardDetectionHandler> list = this.cardListenerList;

        /* Protection against reentrancy problems */
        synchronized (list) {

            int registrySize = list.size();
            for (int i = 0; i < registrySize; i++) {
                if (list.get(i).handler == handler) {
                    throw new IllegalArgumentException("Event handler already registered");
                }
            }

            byte[] propertyArray = null;

            if (properties != null) {

                int size = properties.length;

                for (int i = 0; i < properties.length; i++) {
                    if (properties[i] == ConnectionProperty.ISO_7816_4) {
                        size += 2;
                        break;
                    }
                }

                propertyArray = new byte[size];
                for (int i = 0; i < properties.length; i++) {
                    propertyArray[i] = (byte) properties[i].getValue();
                }

                if (size > properties.length) {
                    propertyArray[size - 2] = (byte) ConnectionProperty.ISO_14443_4_A.getValue();
                    propertyArray[size - 1] = (byte) ConnectionProperty.ISO_14443_4_B.getValue();
                }
            }

            cardHandler.registryHandle = new SafeHandle();

            int error = MethodAutogen.WReaderListenToCardDetection(cardHandler.getId(), priority, propertyArray,
                    cardHandler.registryHandle.getReference());

            if (error == ConstantAutogen.W_SUCCESS) {
                if (cardHandler.registryHandle.isNull() == false) {
                    list.addElement(cardHandler);
                } else {
                    error = ConstantAutogen.W_ERROR_BAD_PARAMETER;
                }
            }

            if (error != ConstantAutogen.W_SUCCESS) {
                NfcException e = new NfcException("Card listener registration error", NfcErrorCode.getCode(error));

                if (error == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
                    throw new IllegalArgumentException(e);
                }
                throw e;
            }
        }
    }

    public void unregisterCardListener(CardDetectionEventHandler handler) {

        if (handler == null) {
            throw new IllegalArgumentException("handler = null");
        }

        ReaderCardDetectionHandler reg = null;
        Vector<ReaderCardDetectionHandler> list = this.cardListenerList;

        /* Protection against reentrancy problems */
        synchronized (list) {
            int registrySize = list.size();
            for (int i = 0; i < registrySize; i++) {
                reg = list.elementAt(i);
                if (reg.handler == handler) {
                    /* Registered */
                    reg.registryHandle.release();
                    list.removeElementAt(i);
                    return;
                }
            }

            throw new IllegalArgumentException("Event handler not registered");
        }
    }

    private static final int[] searchPorpertyList = {
            ConstantAutogen.W_PROP_BPRIME, ConstantAutogen.W_PROP_MY_D_MOVE, ConstantAutogen.W_PROP_MY_D_NFC,
            ConstantAutogen.W_PROP_NFC_TAG_TYPE_1, ConstantAutogen.W_PROP_NFC_TAG_TYPE_2,
            ConstantAutogen.W_PROP_NFC_TAG_TYPE_3, ConstantAutogen.W_PROP_NFC_TAG_TYPE_4_A,
            ConstantAutogen.W_PROP_NFC_TAG_TYPE_4_B, ConstantAutogen.W_PROP_NFC_TAG_TYPE_5,
            ConstantAutogen.W_PROP_NFC_TAG_TYPE_6, ConstantAutogen.W_PROP_TYPE1_CHIP, ConstantAutogen.W_PROP_JEWEL,
            ConstantAutogen.W_PROP_TOPAZ, ConstantAutogen.W_PROP_TOPAZ_512, ConstantAutogen.W_PROP_FELICA,
            ConstantAutogen.W_PROP_MIFARE_1K, ConstantAutogen.W_PROP_MIFARE_4K, ConstantAutogen.W_PROP_MIFARE_PLUS_S_2K,
            ConstantAutogen.W_PROP_MIFARE_PLUS_S_4K, ConstantAutogen.W_PROP_MIFARE_PLUS_X_2K,
            ConstantAutogen.W_PROP_MIFARE_PLUS_X_4K, ConstantAutogen.W_PROP_MIFARE_DESFIRE_D40,
            ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_2K, ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_4K,
            ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_8K, ConstantAutogen.W_PROP_MIFARE_UL, ConstantAutogen.W_PROP_MIFARE_UL_C,
            ConstantAutogen.W_PROP_MIFARE_MINI, ConstantAutogen.W_PROP_PICOPASS_2K, ConstantAutogen.W_PROP_PICOPASS_32K,
            ConstantAutogen.W_PROP_NXP_ICODE, ConstantAutogen.W_PROP_TI_TAGIT, ConstantAutogen.W_PROP_ST_LRI_512,
            ConstantAutogen.W_PROP_ST_LRI_2K, ConstantAutogen.W_PROP_ISO_7816_4, ConstantAutogen.W_PROP_ISO_14443_4_A,
            ConstantAutogen.W_PROP_ISO_14443_4_B, ConstantAutogen.W_PROP_ISO_14443_3_A, ConstantAutogen.W_PROP_ISO_14443_3_B,
            ConstantAutogen.W_PROP_ISO_15693_3,
    };

    public boolean checkConnectionProperty(ConnectionProperty property) {
        if (property == null) {
            throw new IllegalArgumentException("property = null");
        }

        return MethodAutogen.WReaderIsPropertySupported(property.getValue());
    }

    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Tag use in debug */
    private static final String TAG = CardListenerRegistryImpl.class.getSimpleName();

    private class ReaderCardDetectionHandler extends Callback_tWReaderCardDetectionHandler {

        CardDetectionEventHandler handler;

        SafeHandle registryHandle;

        ReaderCardDetectionHandler(CardDetectionEventHandler handler) {
            super(true);
            this.handler = handler;
        }

        @Override
        public void tWReaderCardDetectionHandler(int connection, int error) {

            if (error == ConstantAutogen.W_SUCCESS) {

                int size = searchPorpertyList.length;
                int index = 0;

                SafeHandle connectionHandle = new SafeHandle(connection);

                for (index = 0; index < size; index++) {
                    if (MethodAutogen.WBasicCheckConnectionProperty(connection, searchPorpertyList[index]) == ConstantAutogen.W_SUCCESS) {
                        size = 0;
                        break;
                    }
                }

                if (size != 0) {
                    connectionHandle.close(true, false);
                    handler.onCardDetectedError(NfcErrorCode.BAD_PARAMETER);
                }

                if (DEBUG)
                    Log.d(TAG, "searchPorpertyList[" + index + "]=" + searchPorpertyList[index]);
                switch (searchPorpertyList[index]) {
                    case ConstantAutogen.W_PROP_NFC_TAG_TYPE_1:
                        handler.onCardDetected(new NfcType1ConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_NFC_TAG_TYPE_2:
                        handler.onCardDetected(new NfcType2ConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_NFC_TAG_TYPE_3:
                        handler.onCardDetected(new NfcType3ConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_NFC_TAG_TYPE_4_A:
                        handler.onCardDetected(new NfcType4AConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_NFC_TAG_TYPE_4_B:
                        handler.onCardDetected(new NfcType4BConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_NFC_TAG_TYPE_5:
                        handler.onCardDetected(new NfcType5ConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_NFC_TAG_TYPE_6:
                        handler.onCardDetected(new NfcType6ConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_TYPE1_CHIP:
                    case ConstantAutogen.W_PROP_JEWEL:
                    case ConstantAutogen.W_PROP_TOPAZ:
                    case ConstantAutogen.W_PROP_TOPAZ_512:
                        handler.onCardDetected(new TopazConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_FELICA:
                        handler.onCardDetected(new FeliCaConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_MIFARE_MINI:
                    case ConstantAutogen.W_PROP_MIFARE_1K:
                    case ConstantAutogen.W_PROP_MIFARE_4K:
                        handler.onCardDetected(new Iso14443Part3AConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_MIFARE_PLUS_S_2K:
                    case ConstantAutogen.W_PROP_MIFARE_PLUS_S_4K:
                    case ConstantAutogen.W_PROP_MIFARE_PLUS_X_2K:
                    case ConstantAutogen.W_PROP_MIFARE_PLUS_X_4K:
                        handler.onCardDetected(new Iso14443Part3AConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_MIFARE_DESFIRE_D40:
                    case ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_2K:
                    case ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_4K:
                    case ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_8K:
                        handler.onCardDetected(new Iso7816Part4AConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_MIFARE_UL:
                    case ConstantAutogen.W_PROP_MIFARE_UL_C:
                        handler.onCardDetected(new MifareULConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_MY_D_MOVE:
                    case ConstantAutogen.W_PROP_MY_D_NFC:
                        if (DEBUG)
                            Log.d(TAG, "MY-D <NFC> My-D <NFC> My-D <NFC> My-D");
                        handler.onCardDetected(new MydConnectionImpl(connectionHandle));
                        if (DEBUG)
                            Log.d(TAG, "Ok Ok  <NFC> My-D");
                        break;
                    case ConstantAutogen.W_PROP_PICOPASS_2K:
                    case ConstantAutogen.W_PROP_PICOPASS_32K:
                        handler.onCardDetected(new PicopassConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_ISO_7816_4:
                        if (MethodAutogen.WBasicCheckConnectionProperty(connection, ConstantAutogen.W_PROP_ISO_14443_4_A) == ConstantAutogen.W_SUCCESS)
                            handler.onCardDetected(new Iso7816Part4AConnectionImpl(connectionHandle));
                        else
                            handler.onCardDetected(new Iso7816Part4BConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_ISO_14443_4_A:
                        handler.onCardDetected(new Iso14443Part4AConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_ISO_14443_4_B:
                        handler.onCardDetected(new Iso14443Part4BConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_ISO_14443_3_A:
                        handler.onCardDetected(new Iso14443Part3AConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_ISO_14443_3_B:
                        handler.onCardDetected(new Iso14443Part3BConnectionImpl(connectionHandle));
                        break;
                    case ConstantAutogen.W_PROP_BPRIME:
                        if (DEBUG)
                            Log.d(TAG, "B-PRIME <NFC> B-PRIME <NFC> B-PRIME <NFC> B-PRIME");
                        handler.onCardDetected(new Iso14443Part2BPrimeConnectionImpl(connectionHandle));
                        if (DEBUG)
                            Log.d(TAG, "Ok Ok  <NFC> B-PRIME");
                        break;
                    case ConstantAutogen.W_PROP_NXP_ICODE:
                    case ConstantAutogen.W_PROP_TI_TAGIT:
                    case ConstantAutogen.W_PROP_ST_LRI_512:
                    case ConstantAutogen.W_PROP_ST_LRI_2K:
                    case ConstantAutogen.W_PROP_ISO_15693_3:
                        handler.onCardDetected(new Iso15693Part3ConnectionImpl(connectionHandle));
                        break;
                    default:
                        connectionHandle.close(true, false);
                        handler.onCardDetectedError(NfcErrorCode.BAD_PARAMETER);
                        break;
                }
            } else {
                handler.onCardDetectedError(NfcErrorCode.getCode(error));
            }
        }
    }
}
