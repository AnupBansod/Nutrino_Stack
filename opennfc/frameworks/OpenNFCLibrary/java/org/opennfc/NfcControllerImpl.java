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

import org.opennfc.hardware.CollisionEventHandler;
import org.opennfc.hardware.NfcController;
import org.opennfc.hardware.NfcControllerExceptionEventHandler;
import org.opennfc.hardware.UnknownCardEventHandler;

/**
 * See NfcController interface package private
 *
 * @hide
 **/
final class NfcControllerImpl implements NfcController {

    private CardCollisionHandler mCardCollisionHandler = null;

    private UnknownCardHandler mUnknownCardHandler = null;

    private ExceptionEventHandler mExceptionEventHandler = null;

    /* package private */NfcControllerImpl() {
    }

    /** See NfcController */
    public void reset(NfcController.Mode mode) throws NfcException {
       if(mode == null) {
         throw new IllegalArgumentException("mode = null");
       }

        CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.WNFCControllerReset(callback.getId(), mode.getValue());
        callback.waitForCompletionOrException();
    }

    /** See NfcController */
    public String getFirmwareInfo(byte[] firmware) throws NfcException {
        int[] actualLength = new int[1];
        char[] buffer = new char[64];

        if (firmware == null) {
            throw new IllegalArgumentException("firmware or property=null");
        }

        int error = MethodAutogen.WNFCControllerGetFirmwareProperty(firmware,
                ConstantAutogen.W_NFCC_PROP_FIRMWARE_VERSION,
                buffer, actualLength);

        if (error != ConstantAutogen.W_SUCCESS)
            throw new NfcException(NfcErrorCode.getCode(error));

        return new String(buffer, 0, actualLength[0]);
    }

    /** See NfcController */
    public void firmwareUpdate(byte[] buffer, NfcController.Mode mode) throws NfcException {

        if ((buffer == null) || (mode == null)) {
            throw new IllegalArgumentException("buffer=null or mode=null");
        }

        CallbackGenericCompletion callback = new CallbackGenericCompletion();
        ByteBufferReference pUpdateBuffer = new ByteBufferReference(buffer, true);

        MethodAutogen.WNFCControllerFirmwareUpdate(callback.getId(), pUpdateBuffer.getValue(), mode.getValue());

        callback.waitForCompletionOrException();

        pUpdateBuffer.release(false);
    }

    /** See NfcController */
    public int getFirmwareUpdateState() {
        return MethodAutogen.WNFCControllerFirmwareUpdateState();
    }

    /** See NfcController */
    public NfcController.Mode getMode() {
        int value = MethodAutogen.WNFCControllerGetMode();

         switch (value) {
             case ConstantAutogen.W_NFCC_MODE_BOOT_PENDING:
                 return NfcController.Mode.BOOT_PENDING;
             case ConstantAutogen.W_NFCC_MODE_MAINTENANCE:
                 return NfcController.Mode.MAINTENANCE;
             case ConstantAutogen.W_NFCC_MODE_NO_FIRMWARE:
                 return NfcController.Mode.NO_FIRMWARE;
             case ConstantAutogen.W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED:
                 return NfcController.Mode.FIRMWARE_NOT_SUPPORTED;
             case ConstantAutogen.W_NFCC_MODE_NOT_RESPONDING:
                 return NfcController.Mode.NOT_RESPONDING;
             case ConstantAutogen.W_NFCC_MODE_LOADER_NOT_SUPPORTED:
                 return NfcController.Mode.LOADER_NOT_SUPPORTED;
             case ConstantAutogen.W_NFCC_MODE_ACTIVE:
                 return NfcController.Mode.ACTIVE;
             case ConstantAutogen.W_NFCC_MODE_SWITCH_TO_STANDBY:
                 return NfcController.Mode.SWITCH_TO_STANDBY;
             case ConstantAutogen.W_NFCC_MODE_STANDBY:
                 return NfcController.Mode.STANDBY;
             case ConstantAutogen.W_NFCC_MODE_SWITCH_TO_ACTIVE:
                 return NfcController.Mode.SWITCH_TO_ACTIVE;
         }
         throw new InternalError("Wrong mode code");
    }

    /** See NfcController */
    public void registerExceptionEventHandler(NfcControllerExceptionEventHandler handler)
            throws NfcException {

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }
        if (mExceptionEventHandler != null && mExceptionEventHandler.handler == handler) {
            throw new IllegalArgumentException("Event handler already registered");
        }

        mExceptionEventHandler = new ExceptionEventHandler(handler);

        int error = MethodAutogen.WNFCControllerMonitorException(mExceptionEventHandler.getId(),
                mExceptionEventHandler.registryHandle.getReference());

        if (error != ConstantAutogen.W_SUCCESS) {
            NfcException e = new NfcException("Registration error - ", NfcErrorCode.getCode(error));
            if (error == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
                throw new IllegalArgumentException(e);
            }
            throw e;
        }
    }

    /** See NfcController */
    public void unregisterExceptionEventHandler(NfcControllerExceptionEventHandler handler) {

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }
        if (mExceptionEventHandler == null || mExceptionEventHandler.handler != handler) {
            throw new IllegalArgumentException("Handler is not already registered.");
        }
        mExceptionEventHandler.registryHandle.release();
        mExceptionEventHandler = null;
    }

    /** See NfcController */
    public void selfTest() throws NfcException {
        CallbackControllerSelfTest callback = new CallbackControllerSelfTest();
        MethodAutogen.WNFCControllerSelfTest(callback.getId());
        callback.waitForCompletionOrException();
    }

    /** See NfcController */
    public byte[] productionTest(byte[] testCommand) throws NfcException {
        byte[] resultBuffer = new byte[280];
        CallbackGenericDataCompletion callback = new CallbackGenericDataCompletion();

        ByteBufferReference pParameterBuffer = new ByteBufferReference(testCommand, true);
        ByteBufferReference pResultBuffer = new ByteBufferReference(resultBuffer, false);

        MethodAutogen.WNFCControllerProductionTest(pParameterBuffer.getValue(), pResultBuffer
                .getValue(), callback.getId());

        int resultLength = callback.waitForResultOrException();
        pParameterBuffer.release(false);
        pResultBuffer.release(true);

        byte[] response = new byte[resultLength];
        System.arraycopy(resultBuffer, 0, response, 0, resultLength);

        return response;
    }

    /** See NfcController */
    public void switchStandbyMode(boolean standbyOn) throws NfcException {
        int error = MethodAutogen.WNFCControllerSwitchStandbyMode(standbyOn);
        if (error == ConstantAutogen.W_ERROR_BAD_NFCC_MODE) {
            throw new IllegalStateException("the NFC Controller is not in a valid mode to call this method.");
        } else if (error != ConstantAutogen.W_SUCCESS) {
            throw new NfcException(NfcErrorCode.getCode(error));
        }
    }

    public void registerCardCollisionHandler(CollisionEventHandler handler) throws NfcException {

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }
        if (mCardCollisionHandler != null && mCardCollisionHandler.handler == handler) {
            throw new IllegalArgumentException("Event handler already registered");
        }

        mCardCollisionHandler = new CardCollisionHandler(handler);

        int error = MethodAutogen.WReaderErrorEventRegister(mCardCollisionHandler.getId(),
                ConstantAutogen.W_READER_ERROR_COLLISION, true,
                mCardCollisionHandler.registryHandle.getReference());

        if (error != ConstantAutogen.W_SUCCESS) {
            NfcException e = new NfcException("Registration error - ", NfcErrorCode.getCode(error));
            if (error == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
                throw new IllegalArgumentException(e);
            }
            throw e;
        }
    }

    public void unregisterCardCollisionHandler(CollisionEventHandler handler) {

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }
        if (mCardCollisionHandler == null || mCardCollisionHandler.handler != handler) {
            throw new IllegalArgumentException("Handler is not already registered.");
        }
        mCardCollisionHandler.registryHandle.release();
        mCardCollisionHandler = null;
    }

    public void registerUnknownCardHandler(boolean startDetection, UnknownCardEventHandler handler)
            throws NfcException {

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }
        if (mCardCollisionHandler != null && mCardCollisionHandler.handler == handler) {
            throw new IllegalArgumentException("Event handler already registered");
        }

        mUnknownCardHandler = new UnknownCardHandler(handler);

        int error = MethodAutogen.WReaderErrorEventRegister(mUnknownCardHandler.getId(),
                ConstantAutogen.W_READER_ERROR_UNKNOWN, startDetection,
                mUnknownCardHandler.registryHandle.getReference());

        if (error != ConstantAutogen.W_SUCCESS) {
            NfcException e = new NfcException("Registration error - ", NfcErrorCode.getCode(error));
            if (error == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
                throw new IllegalArgumentException(e);
            }
            throw e;
        }
    }

    public void unregisterUnknownCardHandler(UnknownCardEventHandler handler) {

        if (handler == null) {
            throw new IllegalArgumentException("handler==null");
        }
        if (mUnknownCardHandler == null || mUnknownCardHandler.handler != handler) {
            throw new IllegalArgumentException("Handler is not already registered.");
        }
        mUnknownCardHandler.registryHandle.release();
        mUnknownCardHandler = null;
    }

    private class CallbackControllerSelfTest extends Callback_tWNFCControllerSelfTestCompleted {

        protected CallbackControllerSelfTest() {
            super(false);
        }

        @Override
        protected void tWNFCControllerSelfTestCompleted(int error, int result) {
            if (error != ConstantAutogen.W_SUCCESS)
                signalException(new NfcException(NfcErrorCode.getCode(error)));
            else
                signalCompletion();
        }
    }

    private class CardCollisionHandler extends Callback_tWBasicGenericEventHandler {

        CollisionEventHandler handler;

        SafeHandle registryHandle;

        CardCollisionHandler(CollisionEventHandler handler) {
            super(true);
            this.handler = handler;
            this.registryHandle = new SafeHandle();
        }

        @Override
        protected void tWBasicGenericEventHandler(int nEventCode) {
            if(nEventCode == ConstantAutogen.W_READER_ERROR_COLLISION) {
                handler.onCollisionDetected();
            }
        }

    }

    private class UnknownCardHandler extends Callback_tWBasicGenericEventHandler {

        UnknownCardEventHandler handler;

        SafeHandle registryHandle;

        UnknownCardHandler(UnknownCardEventHandler handler) {
            super(true);
            this.handler = handler;
            this.registryHandle = new SafeHandle();
        }

        @Override
        protected void tWBasicGenericEventHandler(int nEventCode) {
            if(nEventCode == ConstantAutogen.W_READER_ERROR_UNKNOWN) {
                handler.onUnknownCardDetected();
            }
        }

    }

    private class ExceptionEventHandler extends Callback_tWBasicGenericEventHandler {

        private NfcControllerExceptionEventHandler handler;

        SafeHandle registryHandle;

        ExceptionEventHandler(NfcControllerExceptionEventHandler handler) {
            super(true);
            this.handler = handler;
            this.registryHandle = new SafeHandle();
        }

        @Override
        protected void tWBasicGenericEventHandler(int nEventCode) {
            handler.onExceptionOccured();
        }

    }

}
