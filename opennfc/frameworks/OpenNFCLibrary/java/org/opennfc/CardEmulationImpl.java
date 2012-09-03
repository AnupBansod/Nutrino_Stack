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

import org.opennfc.cardemulation.CardEmulation;
import org.opennfc.cardemulation.CardEmulationEventHandler;

/**
 * ISO14443-4 A card emulation User must derive a class from this class to start
 * ISO14443-4A card emulation package private
 *
 * @hide
 **/
final class CardEmulationImpl implements CardEmulation {

    /**
     * Card emulation connection handle
     */
    private SafeHandle mConnectionSafeHandle = new SafeHandle();

    private byte[] mUID = null;
    private int mRandomIdentifierLength = 0;
    ConnectionProperty mCardType;

    /* package private */CardEmulationImpl(ConnectionProperty cardType, byte[] identifier) {

        if ((cardType == null) || (identifier == null)) {
            throw new IllegalArgumentException("cardType or identifier = null");
        }
        if (cardType == ConnectionProperty.ISO_14443_4_A) {
           if ((identifier.length != 4) && (identifier.length != 7) && (identifier.length != 10)) {
              throw new IllegalArgumentException(
                    "the identifier length is not compliant with the card type");
           }
        } else if (cardType == ConnectionProperty.ISO_14443_4_B) {
           if (identifier.length != 4) {
              throw new IllegalArgumentException(
                    "the identifier length is not compliant with the card type");
           }
        } else {
            throw new IllegalArgumentException("Unsupported cardType");
        }

        mRandomIdentifierLength = 0;
        mCardType = cardType;
        mUID = new byte[identifier.length];
        System.arraycopy(identifier, 0, mUID, 0, identifier.length);
    }

    /* package private */CardEmulationImpl(ConnectionProperty cardType, int randomIdentifierLength) {

        if (cardType == null) {
            throw new IllegalArgumentException("cardType or identifier = null");
        }
        if (cardType == ConnectionProperty.ISO_14443_4_A) {
           if ((randomIdentifierLength != 4) && (randomIdentifierLength != 7) && (randomIdentifierLength != 10)) {
              throw new IllegalArgumentException(
                    "the identifier length is not compliant with the card type");
           }
        } else if (cardType == ConnectionProperty.ISO_14443_4_B) {
           if (randomIdentifierLength != 4) {
              throw new IllegalArgumentException(
                    "the identifier length is not compliant with the card type");
           }
        } else {
            throw new IllegalArgumentException("Unsupported cardType");
        }

        mRandomIdentifierLength = randomIdentifierLength;
        mCardType = cardType;
        mUID = null;
    }

    public void start(CardEmulationEventHandler handler) throws NfcException {

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }

        synchronized(this) {
           if (!mConnectionSafeHandle.isNull()) {
               throw new IllegalStateException("The card emulation is already started");
           }

           GenericCallBackFunction openCallBack = new GenericCallBackFunction();
           EventCallBackHandler pEventHandler = new EventCallBackHandler(handler);
           CommandCallBackHandler pCommandHandler = new CommandCallBackHandler(handler);

           MethodAutogen.WEmulOpenConnectionBuffer(openCallBack.getId(), pEventHandler.getId(),
                   pCommandHandler.getId(),
                   mCardType.getValue(), mUID, mRandomIdentifierLength, mConnectionSafeHandle.getReference());

           checkError(openCallBack.waitForResult());
        }
    }

    public void stop() {
        GenericCallBackFunction callback = new GenericCallBackFunction();

        synchronized(this) {
           if (mConnectionSafeHandle.isNull() == false) {
              MethodAutogen.WEmulClose(mConnectionSafeHandle.getValue(), callback.getId());
              // Do not check the error, no exception is thrown
              callback.waitForCompletion();
              mConnectionSafeHandle.release();
              mCardType = null;
              mUID = null;
              mRandomIdentifierLength = 0;
           }
        }
    }

    public void sendResponse(byte[] response) throws NfcException {

        if (response == null) {
            throw new IllegalArgumentException("response = null");
        }
        synchronized(this) {
           if (mConnectionSafeHandle.isNull()) {
               throw new NfcException("Card emulation is stopped.");
           }

           checkError(MethodAutogen.WEmulSendAnswer(mConnectionSafeHandle.getValue(), response));
        }
    }

    private class GenericCallBackFunction extends Callback_tWBasicGenericCallbackFunction {

        GenericCallBackFunction() {
            super(false);
        }

        @Override
        protected void tWBasicGenericCallbackFunction(int result) {
            signalResult(result);
        }
    }

    private class EventCallBackHandler extends Callback_tWBasicGenericEventHandler {

        private CardEmulationEventHandler mHandler;

        EventCallBackHandler(CardEmulationEventHandler handler) {
            super(true);
            this.mHandler = handler;
        }

        @Override
        protected void tWBasicGenericEventHandler(int eventCode) {
            mHandler.onEventReceived(eventCode);
        }
    }

    private class CommandCallBackHandler extends Callback_tWEmulCommandReceived {

        private CardEmulationEventHandler mHandler;

        CommandCallBackHandler(CardEmulationEventHandler handler) {
            super(true);
            this.mHandler = handler;
        }

        @Override
        protected void tWEmulCommandReceived(int dataLength) {

            byte[] command = new byte[dataLength];
            int[] pActualDataLength = new int[1];
            if (MethodAutogen.WEmulGetMessageData(mConnectionSafeHandle.getValue(), command,
                    pActualDataLength) == ConstantAutogen.W_SUCCESS) {
                mHandler.onCommandReceived(command);
            }
        }

    }

    private static void checkError(int error) throws NfcException {

        if (error != ConstantAutogen.W_SUCCESS) {
            switch (error) {
                case ConstantAutogen.W_ERROR_OUT_OF_RESOURCE:
                    throw new OutOfMemoryError();
                case ConstantAutogen.W_ERROR_BAD_PARAMETER:
                    throw new IllegalArgumentException("parameter inconsistency");
                case ConstantAutogen.W_ERROR_BUFFER_TOO_LARGE:
                    throw new IllegalArgumentException("The Application Data buffer"
                            + " of the Type A card, or the Higher Layer Response buffer "
                            + "of the Type B card is too large.");
                case ConstantAutogen.W_ERROR_EXCLUSIVE_REJECTED:
                    throw new IllegalStateException("The card emulation is already "
                            + "opened by another client with the same protocol. This "
                            + "error is also returned if a virtual tag is open with "
                            + "the same protocol or if the access to the card protocol "
                            + "is granted to the UICC");
                case ConstantAutogen.W_ERROR_BAD_STATE:
                    throw new IllegalStateException();
                default:
                    throw new NfcException("Card emulation error", NfcErrorCode.getCode(error));
            }
        }
    }

}
