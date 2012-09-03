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

import org.opennfc.cardlistener.Connection;
import org.opennfc.nfctag.NdefMessage;

/**
 * package private
 *
 * Warning: this class implements the methods of interface
 * NfcTagConnection but does not implement explicitly the interface
 * NfcTagConnection. When a sub-class implements NfcTagConnection, the methods
 * become visible.
 *
 * @hide
 **/
class ConnectionImpl implements Connection {

    private SafeHandle mConnectionHandle;

    /* package private */int getNonNullHandle() {
        if (this.mConnectionHandle.isNull()) {
            throw new IllegalStateException("Handle already disposed");
        }
        return this.mConnectionHandle.getValue();
    }

    /* Builds a connection from a record handle */
    /* package private */ConnectionImpl(SafeHandle connectionHandle) {

        this.mConnectionHandle = connectionHandle;
    }

    /* package private */ConnectionImpl() {

        this.mConnectionHandle = null;
    }


    /* package private */void setHandle(SafeHandle connectionHandle) {

        if ((this.mConnectionHandle != null) || (connectionHandle == null)) {
            throw new InternalError();
        }
        this.mConnectionHandle = connectionHandle;
    }


    /* The connection property values */
    private static final ConnectionProperty[] sPropertyValues = ConnectionProperty.values();

    public ConnectionProperty[] getProperties() {

        int[] nPropertyNumber = new int[1];
        checkError2(MethodAutogen.WBasicGetConnectionPropertyNumber(getNonNullHandle(),
                nPropertyNumber));
        int num = nPropertyNumber[0];
        byte[] propertyByteArray = new byte[num];
        ConnectionProperty[] propertyArray = new ConnectionProperty[num];

        checkError2(MethodAutogen.WBasicGetConnectionProperties(getNonNullHandle(),
                propertyByteArray));

        for (int i = 0; i < num; i++) {

            for (ConnectionProperty prop : sPropertyValues) {

                if (prop.getValue() == propertyByteArray[i]) {
                    propertyArray[i] = prop;
                    break;
                }
            }
        }

        return propertyArray;
    }

    public boolean checkProperty(ConnectionProperty property) {

        if (property == null) {
            throw new IllegalArgumentException("property == null");
        }

        if (MethodAutogen.WBasicCheckConnectionProperty(getNonNullHandle(), property.getValue()) == ConstantAutogen.W_SUCCESS)
            return true;

        return false;
    }

    public boolean previousApplicationMatch() {
        return MethodAutogen.WReaderPreviousApplicationMatch(getNonNullHandle());
    }

    public void close(boolean giveToNextListener, boolean cardApplicationMatch) {
        this.mConnectionHandle.close(giveToNextListener, cardApplicationMatch);
    }

    public void close() {
        this.mConnectionHandle.close(true, true);
    }

    /**
     * @param error
     * @throws NfcException
     */
    private static void checkError2(int error) {
        switch (error) {
            case ConstantAutogen.W_SUCCESS:
                break;
            default:
                throw new IllegalStateException(new NfcException(NfcErrorCode.getCode(error)));
        }
    }

    /*
     * This section is only used if the NfcTagConnection interface is
     * implemented in a sub-class
     */

    private byte[] serialNumber;

    private boolean isReadOnly;

    private boolean isLockable;

    private int freeSpaceSize;

    /* See NfcTagConnection */
    public byte[] getIdentifier() throws NfcException {
        getTagInfo();
        return this.serialNumber;
    }

    /* See NfcTagConnection */
    public boolean isReadOnly() throws NfcException {
        getTagInfo();
        return this.isReadOnly;
    }

    /* See NfcTagConnection */
    public boolean isLockable() throws NfcException {
        getTagInfo();
        return this.isLockable;
    }

    /* See NfcTagConnection */
    public int getFreeSpaceSize() throws NfcException {
        getTagInfo();
        return this.freeSpaceSize;
    }

    /* See NfcTagConnection */
    public NdefMessage readMessage() throws NfcException {

        HandlerReadMessageCompleted readCallback = new HandlerReadMessageCompleted();

        MethodAutogen.WNDEFReadMessage(getNonNullHandle(), readCallback.getId(),
                ConstantAutogen.W_NDEF_TNF_ANY_TYPE, null, null);

        int hMessage = readCallback.waitForResultOrException();
        if (readCallback.getErrorValue() != ConstantAutogen.W_SUCCESS) {
            throw new NfcException("No message is found or in error case.", NfcErrorCode
                    .getCode(readCallback.getErrorValue()));
        }

        return new NdefMessage(null, -1, hMessage);
    }

    /* See NfcTagConnection */
    public void writeMessage(NdefMessage message, int flags) throws NfcException {

        if (message == null) {
            throw new IllegalArgumentException("message == null");
        }
        CallbackGenericCompletion writeCallback = new CallbackGenericCompletion();

        byte[] data = message.getContent();
        ByteBufferReference messageBuffer = new ByteBufferReference(data, 0, data.length, true);

        MethodAutogen.WJavaNDEFWriteMessage(getNonNullHandle(), writeCallback.getId(),
                messageBuffer.getValue(), flags, null);

        try {
            writeCallback.waitForCompletionOrException();
        }catch (Exception e) {
           throw new NfcException(e.getMessage());
        } finally {
            messageBuffer.release(false);
        }
    }

    private void getTagInfo() throws NfcException {

        if (this.serialNumber == null) {

            byte[] pTagInfo = new byte[18];
            int error = MethodAutogen.WNDEFGetTagInfoBuffer(getNonNullHandle(), pTagInfo);

            if (error != ConstantAutogen.W_SUCCESS)
                throw new NfcException(NfcErrorCode.getCode(error));

            // bool bIsLocked
            this.isReadOnly = (pTagInfo[0] == 1);
            // bool bIsLockable
            this.isLockable = (pTagInfo[1] == 1);
            // bool nFreeSpaceSize
            this.freeSpaceSize = NfcTagManagerImpl.byteArrayToInt(pTagInfo, 2);
            // uint8_t nSerialNumberLength
            int serialNumberlength = pTagInfo[6];
            // uint8_t sSerialNumber []
            this.serialNumber = new byte[serialNumberlength];
            System.arraycopy(pTagInfo, 7, this.serialNumber, 0, serialNumberlength);
        }
    }

    private class HandlerReadMessageCompleted extends Callback_tWNDEFReadMessageCompleted {

        public HandlerReadMessageCompleted() {
            super(false);
        }

        @Override
        public void tWNDEFReadMessageCompleted(int message, int error) {

            if (error != ConstantAutogen.W_SUCCESS)
                signalException(new NfcException(NfcErrorCode.getCode(error)));
            else
                signalResult(message);
        }
    }
}
