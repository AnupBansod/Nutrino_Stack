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

package org.opennfc.tests;

import org.opennfc.ConnectionProperty;
import org.opennfc.HelperForNfc;
import org.opennfc.NfcException;
import org.opennfc.nfctag.NdefMessage;
import org.opennfc.nfctag.NfcTagConnection;

import android.util.Log;

/**
 * Mock of Nfc tag connection to use in tests
 */
public class MockNfcTagConnection implements NfcTagConnection {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Mode use in P2P test scenario */
    public static final int MODE_P2P = 1;
    /** Mode use in reading/writing test scenario */
    public static final int MODE_READING_WRITING = 0;

    /** Next virtual handle */
    private static int NEXT_HANDLE = 1;
    /** Not in specific mode (Ordinary one) */
    public static final int NO_MODE = -1;
    /** Tag use in debug */
    private static final String TAG = MockNfcTagConnection.class.getSimpleName();
    /** Exception to throw if call exchange method */
    private final NfcException exchangeExecptionToThrow;
    /** Simulated time for doing an exchange in millisecond */
    private final int exchangeSleepTime;
    /** Connection handle */
    private final int handle;
    /** Last command receive */
    private byte[] lastCommand;
    /** Indicates if connection is lockable */
    private boolean lockable;
    /** Mode to use */
    private final int mode;
    /** Message to answer when {@link #readMessage()} is called */
    private NdefMessage readMessage;
    /** Indicates if connection is on read only */
    private boolean readOnly;
    /** Last writne massage by {@link #writeMessage(NdefMessage, int)} */
    private NdefMessage writeMessage;

    /**
     * Create the tag connection mock
     * 
     * @param exchangeSleepTime Simulated time for doing an exchange in
     *            millisecond
     * @param exchangeExecptionToThrow Exception to throw if call exchange
     *            method
     * @param mode Mode to use
     */
    public MockNfcTagConnection(final int exchangeSleepTime,
            final NfcException exchangeExecptionToThrow, final int mode) {
        this.handle = MockNfcTagConnection.NEXT_HANDLE++;

        this.exchangeSleepTime = exchangeSleepTime;
        this.exchangeExecptionToThrow = exchangeExecptionToThrow;

        this.mode = mode;
    }

    /**
     * Check if a property is valid <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param property Property to check
     * @return {@code false}
     * @see org.opennfc.cardlistener.Connection#checkProperty(org.opennfc.ConnectionProperty)
     */
    @Override
    public boolean checkProperty(final ConnectionProperty property) {
        // TODO org.opennfc.tests.MockNfcTagConnection.checkProperty (property)
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "TODO : org.opennfc.tests.MockNfcTagConnection.checkProperty (property)");
        }

        return false;
    }

    /**
     * TODO <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.cardlistener.Connection#close()
     */
    @Override
    public void close() {
        // TODO org.opennfc.tests.MockNfcTagConnection.close ()
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "TODO : org.opennfc.tests.MockNfcTagConnection.close ()");
        }

        return;
    }

    /**
     * TODO <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param giveToNextListener
     * @param cardApplicationMatch
     * @see org.opennfc.cardlistener.Connection#close(boolean, boolean)
     */
    @Override
    public void close(final boolean giveToNextListener, final boolean cardApplicationMatch) {
        // TODO org.opennfc.tests.MockNfcTagConnection.close
        // (giveToNextListener, cardApplicationMatch)
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "TODO : org.opennfc.tests.MockNfcTagConnection.close (giveToNextListener, cardApplicationMatch)");
        }

        return;
    }

    /**
     * Simulate an exchange of data (it returns the same as send)
     * 
     * @param command Command send
     * @return Result of exchange (The same array as command)
     * @throws NfcException I specified an non {@code null} exception at
     *             construction, the specified exception is throw here
     */
    public byte[] exchangeData(final byte[] command) throws NfcException {
        HelperForNfc.sleep(this.exchangeSleepTime);

        if (this.exchangeExecptionToThrow != null) {
            throw this.exchangeExecptionToThrow;
        }

        this.lastCommand = command;

        return command;
    }

    /**
     * TODO <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return
     * @throws NfcException
     * @see org.opennfc.nfctag.NfcTagConnection#getFreeSpaceSize()
     */
    @Override
    public int getFreeSpaceSize() throws NfcException {
        // TODO org.opennfc.tests.MockNfcTagConnection.getFreeSpaceSize ()
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "TODO : org.opennfc.tests.MockNfcTagConnection.getFreeSpaceSize ()");
        }

        return 0;
    }

    /**
     * Tag connection handle
     * 
     * @return Tag connection handle
     */
    public int getHandle() {
        return this.handle;
    }

    /**
     * TODO <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return
     * @throws NfcException
     * @see org.opennfc.nfctag.NfcTagConnection#getIdentifier()
     */
    @Override
    public byte[] getIdentifier() throws NfcException {
        // TODO org.opennfc.tests.MockNfcTagConnection.getIdentifier ()
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "TODO : org.opennfc.tests.MockNfcTagConnection.getIdentifier ()");
        }

        return null;
    }

    /**
     * Last command
     * 
     * @return Last command
     */
    public byte[] getLastCommand() {
        return this.lastCommand;
    }

    /**
     * TODO Connection properties <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return {@code null}
     * @see org.opennfc.cardlistener.Connection#getProperties()
     */
    @Override
    public ConnectionProperty[] getProperties() {
        // TODO org.opennfc.tests.MockNfcTagConnection.getProperties ()
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "TODO : org.opennfc.tests.MockNfcTagConnection.getProperties ()");
        }

        return null;
    }

    /**
     * Last written message
     * 
     * @return Last writtten message
     */
    public NdefMessage getWriteMessage() {
        return this.writeMessage;
    }

    /**
     * Indicates if tag is lockable <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return {@code true} if tag is lockable
     * @throws NfcException On issue
     * @see org.opennfc.nfctag.NfcTagConnection#isLockable()
     */
    @Override
    public boolean isLockable() throws NfcException {
        return this.lockable;
    }

    /**
     * Indicates if tag is read only <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return {@code true} if tag is read only
     * @throws NfcException On issue
     * @see org.opennfc.nfctag.NfcTagConnection#isReadOnly()
     */
    @Override
    public boolean isReadOnly() throws NfcException {
        return this.readOnly;
    }

    /**
     * TODO <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return
     * @see org.opennfc.cardlistener.Connection#previousApplicationMatch()
     */
    @Override
    public boolean previousApplicationMatch() {
        // TODO org.opennfc.tests.MockNfcTagConnection.previousApplicationMatch
        // ()
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "TODO : org.opennfc.tests.MockNfcTagConnection.previousApplicationMatch ()");
        }

        return false;
    }

    /**
     * Read a message <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @return Message read
     * @throws NfcException On reading issue
     * @see org.opennfc.nfctag.NfcTagConnection#readMessage()
     */
    @Override
    public NdefMessage readMessage() throws NfcException {
        return this.readMessage;
    }

    /**
     * Change lockable status
     * 
     * @param lockable New lockable status
     */
    public void setLockable(final boolean lockable) {
        this.lockable = lockable;
    }

    /**
     * Change the read message to answer with {@link #readMessage()}
     * 
     * @param readMessage New message to answer
     */
    public void setReadMessage(final NdefMessage readMessage) {
        this.readMessage = readMessage;
    }

    /**
     * Chang read only status
     * 
     * @param readOnly New read only status
     */
    public void setReadOnly(final boolean readOnly) {
        this.readOnly = readOnly;
    }

    /**
     * Write a message and do special treatment for some mode <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param message Message to write
     * @param actionFlags Flags to use
     * @throws NfcException On writing issue
     * @see org.opennfc.nfctag.NfcTagConnection#writeMessage(org.opennfc.nfctag.NdefMessage,
     *      int)
     */
    @Override
    public void writeMessage(final NdefMessage message, final int actionFlags) throws NfcException {
        if (MockNfcTagConnection.DEBUG) {
            Log.d(MockNfcTagConnection.TAG,
                    "writeMessage writeMessage writeMessage writeMessage writeMessage writeMessage writeMessage writeMessage writeMessage writeMessage writeMessage writeMessage");
        }

        this.writeMessage = message;

        switch (this.mode) {
            case MODE_READING_WRITING:
                TestTagReadingWriting.TEST.finishWriting(this);
                break;
            case MODE_P2P:
                TestP2P.TEST.finish(this);
                break;
        }
    }
}
