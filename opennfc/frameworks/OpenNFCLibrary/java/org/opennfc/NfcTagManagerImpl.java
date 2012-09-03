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

import org.opennfc.nfctag.NdefMessage;
import org.opennfc.nfctag.NdefRecord;
import org.opennfc.nfctag.NdefTypeNameFormat;
import org.opennfc.nfctag.NfcTagManager;
import org.opennfc.nfctag.ReadCompletionEventHandler;
import org.opennfc.nfctag.WriteCompletionCallback;
import org.opennfc.nfctag.NfcTagDetectionEventHandler;
import org.opennfc.nfctag.NfcTagConnection;
import org.opennfc.nfctag.VirtualTag;
import org.opennfc.cardlistener.CardDetectionEventHandler;
import org.opennfc.cardlistener.Connection;
import org.opennfc.cardlistener.CardListenerRegistry;

/**
 * package private
 *
 * @hide
 **/
final class NfcTagManagerImpl implements NfcTagManager {

    /** The NDEF Reader callback registry */
    private Vector<WNDEFReadMessageCompletion> mTagReaderEventHandlerList = new Vector<WNDEFReadMessageCompletion>();

    /** The NDEF Reader callback registry */
    private Vector<WNDEFWriteMessageCompletion> mTagWriterCallbackList = new Vector<WNDEFWriteMessageCompletion>();

    private Vector<TagListener> tagListenerList = new Vector<TagListener>();

    private class TagListener implements CardDetectionEventHandler {

        private NfcTagDetectionEventHandler handler;

        public TagListener(NfcTagDetectionEventHandler handler) {
            this.handler = handler;
        }

        public void onCardDetected(Connection connection) {
            this.handler.onTagDetected((NfcTagConnection) connection);
        }

        public void onCardDetectedError(NfcErrorCode what) {
            this.handler.onTagDetectedError(what);
        }
    }

    protected static byte[] shortToByteArray(short s) {
        return new byte[] {
                (byte) ((s & 0xFF00) >> 8), (byte) (s & 0x00FF)
        };
    }

    protected static short byteArrayToShort(byte[] b, int offset) {
        return (short) (((b[offset] & 0xFF) << 8) + (b[offset + 1] & 0xFF));
    }

    protected static int byteArrayToInt(byte[] b, int offset) {
        return (b[offset] << 24) + ((b[offset + 1] & 0xFF) << 16) + ((b[offset + 2] & 0xFF) << 8)
                + (b[offset + 3] & 0xFF);
    }

    private static ConnectionProperty[] allTagProperties = {
            ConnectionProperty.NFC_TAG_TYPE_1, ConnectionProperty.NFC_TAG_TYPE_2,
            ConnectionProperty.NFC_TAG_TYPE_3, ConnectionProperty.NFC_TAG_TYPE_4_A,
            ConnectionProperty.NFC_TAG_TYPE_4_B, ConnectionProperty.NFC_TAG_TYPE_5,
            ConnectionProperty.NFC_TAG_TYPE_6
    };

    public void registerTagListener(int priority, NfcTagDetectionEventHandler handler)
            throws NfcException {

        NfcSecurityManager.check(NfcSecurityManager.CARD_LISTEN);

        if ((priority == NfcPriority.MAXIMUM) || (priority == NfcPriority.EXCLUSIVE)) {
            NfcSecurityManager.check(NfcSecurityManager.HARDWARE);
        }

        if (handler == null) {
            throw new IllegalArgumentException("handler = null");
        }

        CardListenerRegistry mgt = NfcManager.getInstance().getCardListenerRegistry();

        TagListener element = new TagListener(handler);
        mgt.registerCardListener(priority, allTagProperties, element);

        /* No need to synchronize here */
        this.tagListenerList.add(element);
    }

    public void unregisterTagListener(NfcTagDetectionEventHandler handler) {

        if (handler == null) {
            throw new IllegalArgumentException("handler = null");
        }

        Vector<TagListener> list = this.tagListenerList;
        synchronized (list) {
            for (int i = 0; i < list.size(); i++) {
                TagListener element = list.get(i);
                if (element.handler == handler) {
                    list.remove(element);

                    CardListenerRegistry mgt = NfcManager.getInstance().getCardListenerRegistry();
                    mgt.unregisterCardListener(element);

                    return;
                }
            }
        }

        throw new IllegalArgumentException("Handler is not registered");
    }

    public void registerMessageReader(NdefTypeNameFormat tnf, String type, int priority,
            ReadCompletionEventHandler eventHandler) {

        NfcSecurityManager.check(NfcSecurityManager.CARD_LISTEN);

        if (priority == NfcPriority.MAXIMUM) {
            NfcSecurityManager.check(NfcSecurityManager.HARDWARE);
        }

        if ((tnf == null) || (eventHandler == null)) {
            throw new IllegalArgumentException("tnf or eventHandler = null");
        }
        if (NdefRecord.checkTypeConsistency(tnf, type) == false) {
            throw new IllegalArgumentException("tnf and type are not consistent");
        }

        Vector<WNDEFReadMessageCompletion> list = this.mTagReaderEventHandlerList;

        /* Protection against reentrancy problems */
        synchronized (list) {

            int registrySize = list.size();
            for (int i = 0; i < registrySize; i++) {
                if (list.get(i).mEventHandler == eventHandler) {
                    throw new IllegalArgumentException("Event handler already registered");
                }
            }

            WNDEFReadMessageCompletion handler = new WNDEFReadMessageCompletion(eventHandler);

            SafeHandle readerRegistryHandle = new SafeHandle();

            MethodAutogen.WNDEFReadMessageOnAnyTag(handler.getId(), priority, tnf.getValue(), type,
                    readerRegistryHandle.getReference());

            if (readerRegistryHandle.isNull() == false) {
                handler.registryHandle = readerRegistryHandle;
                list.addElement(handler);
            } else {
                throw new OutOfMemoryError("Cannot allocate read resources");
            }
        }
    }

    public void unregisterMessageReader(ReadCompletionEventHandler eventHandler) {

        if (eventHandler == null) {
            throw new IllegalArgumentException("eventHandler==null");
        }

        Vector<WNDEFReadMessageCompletion> list = this.mTagReaderEventHandlerList;

        /* Protection against reentrancy problems */
        synchronized (list) {
            int registrySize = list.size();
            for (int i = 0; i < registrySize; i++) {
                WNDEFReadMessageCompletion handler = list.get(i);
                if (handler.mEventHandler == eventHandler) {
                    /* Registered */
                    handler.registryHandle.release();
                    handler.release();
                    list.removeElementAt(i);
                    return;
                }
            }
        }

        throw new IllegalArgumentException("The event handler is not registered");
    }

    public void writeMessageOnAnyTag(NdefMessage message, int priority,
            WriteCompletionCallback callback, int actionFlags) {

        NfcSecurityManager.check(NfcSecurityManager.CARD_LISTEN);

        if (priority == NfcPriority.MAXIMUM) {
            NfcSecurityManager.check(NfcSecurityManager.HARDWARE);
        }

        if (message == null) {
            throw new IllegalArgumentException("message==null");
        }
        if (callback == null) {
            throw new IllegalArgumentException("callback==null");
        }

        WNDEFWriteMessageCompletion handler = new WNDEFWriteMessageCompletion(callback);

        Vector<WNDEFWriteMessageCompletion> list = this.mTagWriterCallbackList;

        /* Protection against reentrancy problems */
        synchronized (list) {
            byte[] data = message.getContent();
            ByteBufferReference messageBuffer = new ByteBufferReference(data, 0, data.length, true);
            SafeHandle operationHandle = new SafeHandle();

            MethodAutogen.WJavaNDEFWriteMessageOnAnyTag(handler.getId(), priority, messageBuffer
                    .getValue(), actionFlags, operationHandle.getReference());

            messageBuffer.release(false);

            if (operationHandle.isNull() == false) {
                handler.operationSafeHandle = operationHandle;
                list.addElement(handler);
            } else {
                throw new OutOfMemoryError("Cannot allocate write resources");
            }
        }
    }

    /* package private */static SafeHandle getNewMessageHandle(NdefMessage message) {
        if (message != null) {
            try {
                message.equals(org.opennfc.nfctag.NdefTypeNameFormat.UNKNOWN);
            } catch (RuntimeException e) {
                return new SafeHandle(e.hashCode());
            }
        }
        return null;
    }

    public void cancelWriteMessageOnAnyTag(WriteCompletionCallback callback) {

        if (callback == null) {
            throw new IllegalArgumentException("callback = null");
        }

        Vector<WNDEFWriteMessageCompletion> list = this.mTagWriterCallbackList;

        /* Protection against reentrancy problems */
        synchronized (list) {
            int registrySize = list.size();
            for (int i = 0; i < registrySize; i++) {
                WNDEFWriteMessageCompletion handler = list.get(i);
                if (handler.mCallback == callback) {
                    /* Registered */
                    handler.operationSafeHandle.release();
                    list.removeElementAt(i);
                    break;
                }
            }
        }
    }

    private class WNDEFReadMessageCompletion extends Callback_tWNDEFReadMessageCompleted {

        private ReadCompletionEventHandler mEventHandler;

        SafeHandle registryHandle;

        public WNDEFReadMessageCompletion(ReadCompletionEventHandler eventHandler) {
            super(true);
            this.mEventHandler = eventHandler;
        }

        @Override
        public void tWNDEFReadMessageCompleted(int hMessage, int error) {
            if (error == ConstantAutogen.W_SUCCESS) {
                try {
                    this.mEventHandler.onTagRead(new NdefMessage(null, -1, hMessage));
                } catch (NfcException e) {
                    this.mEventHandler.onReadError(e.getCode());
                }
            } else {
                this.mEventHandler.onReadError(NfcErrorCode.getCode(error));
            }
        }
    }

    private class WNDEFWriteMessageCompletion extends Callback_tWBasicGenericCallbackFunction {

        private WriteCompletionCallback mCallback;

        SafeHandle operationSafeHandle;

        public WNDEFWriteMessageCompletion(WriteCompletionCallback callback) {
            super(false);
            this.mCallback = callback;
        }

        @Override
        public void tWBasicGenericCallbackFunction(int error) {

            Vector<WNDEFWriteMessageCompletion> list = mTagWriterCallbackList;

            /* Protection against reentrancy problems */
            synchronized (list) {
                this.operationSafeHandle.release();
                list.removeElement(this);
            }

            if (error == ConstantAutogen.W_SUCCESS) {
                this.mCallback.onTagWritten();
            } else {
                this.mCallback.onWriteError(NfcErrorCode.getCode(error));
            }
        }
    }

    public VirtualTag createVirtualTag(ConnectionProperty tagType, byte[] identifier,
            int tagCapacity) throws NfcException {
        return new VirtualTagImpl(tagType, identifier, tagCapacity);
    }
}
