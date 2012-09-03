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

import java.io.ByteArrayOutputStream;
import java.util.Vector;

import org.opennfc.NfcErrorCode;
import org.opennfc.NfcException;

/**
 * NDEF message user must instantiate a new instance of this class to creates a
 * new NDEF message
 *
 * @since Open NFC 4.0
 */
public final class NdefMessage {

    /* Maximum NDEF file size */
    private final int NDEF_MAX_LENGTH = 10240;

    /* The next message */
    private NdefMessage mNextMessage;

    /* The record list */
    private Vector<NdefRecord> mRecordList;

    /**
     * Creates an empty NDEF message.
     */
    public NdefMessage() {
        this.mRecordList = new Vector<NdefRecord>();
    }

    /**
     * Builds a NDEF message from the content of a buffer
     *
     * @param buffer the buffer containing a binary representation of the message.
     * @throws IllegalArgumentException if <code>buffer</code> is null.
     * @throws NfcException if the format of the buffer is not a valid NDEF message.
     */
    public NdefMessage(byte[] buffer) throws NfcException {
        this(buffer, 0, (buffer == null) ? 0 : buffer.length);
    }

    /**
     * Builds a NDEF message from the content of a buffer
     *
     * @param buffer the buffer containing a binary representation of the message.
     * @param offset the index of the first byte of the message in the buffer.
     * @param length  the length in bytes of the buffer.
     * @throws IllegalArgumentException if <code>buffer</code> is null.
     * @throws IllegalArgumentException if <code>offset</code> or <code>length</code> are negative.
     * @throws IndexOutOfBoundsException if <code>offset</code> and <code>length</code> are invalid.
     * @throws NfcException if the format of the buffer is not a valid NDEF message.
     */
    public NdefMessage(byte[] buffer, int offset, int length) throws NfcException {

        /* Special case when an NDEF tag is read we can have more than one Ndef Message to be treated */
        if ((buffer == null) && (offset < 0)) {
            offset = 0;
            Vector<NdefMessage> ndefMessageList = new Vector<NdefMessage>();

            byte[] messageBufferTemp = new byte[NDEF_MAX_LENGTH];
            int bufferlength = MethodAutogen.WJavaNDEFGetMessageContent(length, messageBufferTemp);
            byte[] messageBuffer = new byte[bufferlength];
            System.arraycopy(messageBufferTemp, 0, messageBuffer, 0, bufferlength);

            offset += createMessage(messageBuffer, offset, bufferlength);
            int startIndex = 0;
            NdefMessage previous = null;
            NdefMessage next = null;
            while (offset < bufferlength) {
                next = new NdefMessage();
                offset = next.createMessage(messageBuffer, offset, bufferlength - offset);
                ndefMessageList.add(next);
                if (startIndex > 0) {
                    previous = ndefMessageList.elementAt((startIndex - 1));
                    previous.setNextMessage(next);
                }
                startIndex++;
            }
            if (ndefMessageList.size() > 0)
                this.setNextMessage(ndefMessageList.firstElement());
            return;
        }

        if (buffer == null) {
            throw new IllegalArgumentException("buffer==null");
        }

        if ((offset < 0) || (length < 0)) {
            throw new IllegalArgumentException("negative offset or length");
        }

        if ((offset + length) > buffer.length) {
            throw new IndexOutOfBoundsException();
        }

        createMessage(buffer, 0, buffer.length);
    }

    /** Creates a message from a message buffer */
    private int createMessage(byte[] buffer, int offset, int length) throws NfcException {

        this.mNextMessage = null;

        synchronized (Helper.MUTEX) {

            this.mRecordList = new Vector<NdefRecord>();
            /* first record */
            NdefRecord rec = new NdefRecord(buffer, offset, length);
            byte flags = buffer[offset];

            if (!Helper.getFlag(flags, Helper.MB_FLAG)) {
                throw new NfcException("MB flag was not set in the first record");
            }

            mRecordList.addElement(rec);
            offset += Helper.sNdefRecordSize;
            /*
             * Check if we've processed the last record of the message ignore
             * the bytes in the buffer after the end of the message
             */
            if (!Helper.getFlag(flags, Helper.ME_FLAG)) {

                while (offset < buffer.length) {
                    flags = buffer[offset];

                    if (Helper.getFlag(flags, Helper.MB_FLAG)) {
                        throw new NfcException("message have MB bit set");
                    }

                    rec = new NdefRecord(buffer, offset, length - offset);
                    mRecordList.addElement(rec);
                    if (offset >= buffer.length) {
                        break;
                    }
                    offset = Helper.sNdefRecordSize;
                }

                if (!Helper.getFlag(flags, Helper.ME_FLAG)) {
                    throw new NfcException("ME flag was not set in last record");
                }
            }
            mRecordList.setElementAt(rec, mRecordList.size() - 1);
        }

        return Helper.sNdefRecordSize;
    }

    /**
     * Returns the list of the this.mRecordList included in this message.
     *
     * @return The list of the this.mRecordList.
     **/
    public Vector<NdefRecord> getRecords() {

        return this.mRecordList;
    }

    /**
     * Compares a message with another object.
     *
     * @param  object the object to compare.
     * @return true if the specified object is a message with the same content, false otherwise.
     **/
    @Override
    public synchronized boolean equals(Object object) {

        if (object == this)
            return true;
        if ((object != null) && (object instanceof NdefMessage)) {
            NdefMessage message = (NdefMessage) object;

            int size = this.mRecordList.size();
            if (size == message.mRecordList.size()) {
                for (int i = 0; i < size; i++) {
                    NdefRecord record = this.mRecordList.get(i);
                    if (record == null) {
                        if (message.mRecordList.get(i) != null) {
                            return false;
                        }
                    } else {
                        if (record.equals(message.mRecordList.get(i)) == false) {
                            return false;
                        }
                    }
                }
                return true;
            }
        }
        return false;
    }

    /**
     * Returns the binary representation of the message.
     *
     * @return The binary representation of the message.
     */
    public byte[] getContent() {

        if (this.mRecordList.size() == 0) {
            return null;
        }

        ByteArrayOutputStream baos = new ByteArrayOutputStream();

        if (this.mRecordList.size() == 1) {
            /* only one record */
            NdefRecord rec = this.mRecordList.elementAt(0);
            byte[] data = rec.getContent();
            data[0] = Helper.setFlag(data[0], Helper.MB_FLAG);
            data[0] = Helper.setFlag(data[0], Helper.ME_FLAG);
            baos.write(data, 0, data.length);
            return baos.toByteArray();
        }

        /* the first one */
        NdefRecord rec = this.mRecordList.elementAt(0);
        byte[] data = rec.getContent();
        data[0] = Helper.setFlag(data[0], Helper.MB_FLAG);
        data[0] = Helper.clrFlag(data[0], Helper.ME_FLAG);
        baos.write(data, 0, data.length);

        /* middle ones */
        for (int i = 1; i < this.mRecordList.size() - 1; i++) {
            rec = this.mRecordList.elementAt(i);
            data = rec.getContent();
            data[0] = Helper.clrFlag(data[0], Helper.MB_FLAG);
            data[0] = Helper.clrFlag(data[0], Helper.ME_FLAG);
            baos.write(data, 0, data.length);
        }

        /* last one */
        rec = this.mRecordList.lastElement();
        data = rec.getContent();
        data[0] = Helper.setFlag(data[0], Helper.ME_FLAG);
        data[0] = Helper.clrFlag(data[0], Helper.MB_FLAG);
        baos.write(data, 0, data.length);
        byte[] buffer = baos.toByteArray();
        return buffer;
    }

    /**
     * Gets the next message if any.
     *
     * @return the next message or null if there is no next message.
     */
    public NdefMessage getNextMessage() {
        return this.mNextMessage;
    }

    /**
     * Sets the next message.
     *
     * @param message  the next message. This value may be null.
     *
     * @throws NfcException in case of circularity error.
     */
    public void setNextMessage(NdefMessage message) throws NfcException {

        /* Check the circularity error */
        NdefMessage otherMessage = message;
        while (otherMessage != null) {
            if (otherMessage == this) {
                throw new NfcException("Circularity error", NfcErrorCode.BAD_PARAMETER);
            }
            otherMessage = otherMessage.mNextMessage;
        }

        this.mNextMessage = message;
    }
}
