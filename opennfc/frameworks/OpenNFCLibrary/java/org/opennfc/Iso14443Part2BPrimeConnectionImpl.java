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

import org.opennfc.cardlistener.Iso14443Part2BPrimeConnection;

/**
 * B-Prime connection implementation.<br>
 * For exchange with B-Prime cards
 * 
 * @hide
 */
class Iso14443Part2BPrimeConnectionImpl extends ConnectionImpl implements Iso14443Part2BPrimeConnection {
    /** Buffer for read data */
    private final byte[] mReceivedFrame = new byte[280];
    /** B-Prime repgen */
    private byte[] repgen;

    /**
     * Create the connection
     * 
     * @param connectionHandle Handle
     */
    Iso14443Part2BPrimeConnectionImpl(final SafeHandle connectionHandle) {
        super(connectionHandle);
    }

    /**
     * Performs an exchange of data between the local device and the card.
     * 
     * @param command the command to be send to the card.
     * @return the answer received from the card.
     * @throws IllegalArgumentException if <code>command</code> is null or empty.
     * @throws IllegalStateException the connection is closed.
     * @throws NfcException if a communication error occurs.
     */
    @Override
    public byte[] exchangeData(final byte[] command) throws NfcException {
        if ((command == null) || (command.length == 0)) {
            throw new IllegalArgumentException("command is null or empty");
        }

        final CallbackW14ExchangeDataCompleted callback = new CallbackW14ExchangeDataCompleted();

        final ByteBufferReference pReaderToCardBuffer = new ByteBufferReference(command, true);
        final ByteBufferReference pCardToReaderBuffer = new ByteBufferReference(this.mReceivedFrame, false);

        MethodAutogen.WBPrimeExchangeData(this.getNonNullHandle(), callback.getId(), pReaderToCardBuffer.getValue(),
                pCardToReaderBuffer.getValue(), null);

        final int dataLength = callback.waitForResultOrException();

        pReaderToCardBuffer.release(false);
        pCardToReaderBuffer.release(true);

        final byte[] response = new byte[dataLength];
        if (dataLength != 0) {
            System.arraycopy(this.mReceivedFrame, 0, response, 0, dataLength);
        }

        return response;
    }

    /**
     * Collect card information
     */
    private void getConnectionInfo() {
        if (this.repgen == null) {
            final byte[] pInfoBuffer = new byte[17];
            NfcManager.checkError(MethodAutogen.WBPrimeGetConnectionInfoBuffer(this.getNonNullHandle(), pInfoBuffer));

            final int size = pInfoBuffer[0] & 0xFF;
            this.repgen = new byte[size];

            if (size > 0) {
                System.arraycopy(pInfoBuffer, 1, this.repgen, 0, size);
            }
        }
    }

    /**
     * Returns the REPGEN sent by the card during the card detection.
     * 
     * @return the REPGEN value stored in a byte array of the same length.
     */
    @Override
    public byte[] getRepgen() {
        this.getConnectionInfo();

        return this.repgen;
    }
}
