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

package org.opennfc.tests;

import org.opennfc.HelperForNfc;
import org.opennfc.nfctag.NdefMessage;
import org.opennfc.nfctag.NdefRecord;
import org.opennfc.service.ThreadedTask;
import org.opennfc.service.ThreadedTaskManager;

import android.nfc.Tag;
import android.nfc.tech.Ndef;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.util.Vector;

/**
 * Launch and drive the test scenario for reading and writing.<br>
 * It build to be a singleton, but unfortunately, it is a singleton only on a
 * specific context. So we have to transfer collected information from one
 * context to other one :-S
 */
public class TestTagReadingWriting extends TestScenario implements
        ThreadedTask<MockNfcTagConnection> {
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Payload test size */
    private static final int SIZE = 12;

    /** Tag use in debug */
    private static final String TAG = TestTagReadingWriting.class.getSimpleName();
    /**
     * Test scenario of reading and writing singleton (Unfortunately, it is a
     * singleton only on a specific context. So we have to transfer collected
     * information from one context to other one :-S )
     */
    public static final TestTagReadingWriting TEST = new TestTagReadingWriting();
    /** Last read tag */
    private Tag tag;

    /**
     * Create the singleton instance
     */
    private TestTagReadingWriting() {
    }

    /**
     * Call when thread cancel <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.service.ThreadedTask#cancel()
     */
    @Override
    public void cancel() {
        // Nothing to do here
    }

    /**
     * Merge the Activity context information (Stored in the tag connection)
     * with the test context information.<br>
     * NOTE : Here we are in test context <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param taskID Ignored here
     * @param parameters First parameter : The connection with Activity context
     *            information inside
     * @see org.opennfc.service.ThreadedTask#excuteTask(int, PARAMETER[])
     */
    @Override
    public void excuteTask(final int taskID, final MockNfcTagConnection... parameters) {
        // Wait a little to be sure to have get back the Activity information
        HelperForNfc.sleep(1024);

        try {
            // Merge actual information with Activity information
            final ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(
                    parameters[0].getLastCommand());
            this.deserialize(byteArrayInputStream);
        } catch (final Exception exception) {
        }

        // Finish the test and print the result
        this.stopTest();
    }

    /**
     * Call when read is finish and we are ready for next step.<br>
     * NOTE : here we are in Activity context
     */
    public void finishReading() {
        final Ndef ndef = Ndef.get(this.tag);

        final byte[] payload = new byte[TestTagReadingWriting.SIZE];
        for (byte b = 0; b < TestTagReadingWriting.SIZE; b++) {
            payload[b & 0xFF] = (byte) ((TestTagReadingWriting.SIZE - b) & 0xFF);
        }

        try {
            // Write the message

            ndef.writeNdefMessage(TestUtil.createAndroidMessage(payload));
        } catch (final Exception exception) {
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG, "Write failed !", exception);
            }

            this.assertFail("Exception append while writing : " + exception);
        } finally {
            try {
                // The method is called from Activity context, not the test
                // context, so we have to transfer the collected information
                // from activity context to the test context

                final ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream(4096);
                this.serialize(byteArrayOutputStream);

                this.tag.getTagService().transceive(this.tag.getServiceHandle(),
                        byteArrayOutputStream.toByteArray(), true);
            } catch (final Exception exception) {
            }
        }
    }

    /**
     * Call when writing is finish and test if writing is correct.<br>
     * NOTE : here we are in test context
     * 
     * @param mockNfcTagConnection Tag connection where the writing happen
     */
    public void finishWriting(final MockNfcTagConnection mockNfcTagConnection) {
        // Check if writing message is correct
        final byte[] payload = new byte[TestTagReadingWriting.SIZE];
        for (byte b = 0; b < TestTagReadingWriting.SIZE; b++) {
            payload[b & 0xFF] = (byte) ((TestTagReadingWriting.SIZE - b) & 0xFF);
        }

        try {
            final NdefMessage message = mockNfcTagConnection.getWriteMessage();
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG, "message=" + message);
            }
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG,
                        "    hex=" + HelperForNfc.toHexadecimal(message.getContent()));
            }

            final Vector<NdefRecord> records = message.getRecords();
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG, "records=" + records);
            }
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG, "records.size()=" + records.size());
            }

            final NdefRecord ndefRecord = records.elementAt(0);
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG, "ndefRecord=" + ndefRecord);
            }
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG,
                        "       hex=" + HelperForNfc.toHexadecimal(ndefRecord.getContent()));
            }

            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG,
                        "PAYLOAD=" + HelperForNfc.toHexadecimal(ndefRecord.getPayload()));
            }

            this.assertEquals(null, ndefRecord.getPayload(), payload);
        } catch (final Exception exception) {
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG, "finishWriting failed !", exception);
            }

            this.assertFail("Exception append while writing : " + exception);
        }

        // Finish the test in separate thread to be sure not block the Activity
        // context information transfer
        ThreadedTaskManager.MANAGER.doInSeparateThread(this, mockNfcTagConnection);
    }

    /**
     * Treatment when tag is received.<br>
     * It test if the payload is the one we waited .<br>
     * NOTE : here we are in the Activity context
     * 
     * @param tag Tag received
     * @param payload Payload received to test
     */
    public void receiveTag(final Tag tag, final byte[] payload) {
        this.assertEquals("Payload size must be " + TestTagReadingWriting.SIZE, payload.length,
                TestTagReadingWriting.SIZE);

        for (byte b = 0; b < TestTagReadingWriting.SIZE; b++) {
            this.assertEquals("Index : " + (b & 0xFF), payload[b & 0xFF], b);
        }

        this.tag = tag;
    }

    /**
     * Call when test start.<br>
     * NOTE : Here we are in test context <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.tests.TestScenario#start()
     */
    @Override
    protected void start() {
        // Create a mock tag connection
        final MockNfcTagConnection mockNfcTagConnection = new MockNfcTagConnection(1, null,
                MockNfcTagConnection.MODE_READING_WRITING);

        try {
            // Create the message that the readMessage() of mock tag connection
            // will return
            final byte[] payload = new byte[TestTagReadingWriting.SIZE];
            for (byte b = 0; b < TestTagReadingWriting.SIZE; b++) {
                payload[b & 0xFF] = b;
            }

            mockNfcTagConnection.setReadMessage(TestUtil.createMessage(payload));
        } catch (final Exception exception) {
            if (TestTagReadingWriting.DEBUG) {
                Log.d(TestTagReadingWriting.TAG, "Assign message failed !", exception);
            }

            this.assertFail("Exception append while assign : " + exception);
        }

        // Simulate a tag connection detection
        TestUtil.mCardDetectionEventHandler.onCardDetected(mockNfcTagConnection);
    }
}
