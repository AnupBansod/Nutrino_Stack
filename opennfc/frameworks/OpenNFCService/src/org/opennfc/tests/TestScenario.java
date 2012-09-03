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
import org.opennfc.service.ThreadedTask;
import org.opennfc.service.ThreadedTaskManager;

import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

/**
 * Generic test scenario
 */
public abstract class TestScenario {
    /**
     * Assertion information
     */
    static class AssertInfo {
        /** Class and method name */
        String classMethodName;
        /** Line number */
        int lineNumber;
        /** Generic message */
        String message;
        /** Specific message */
        String userMessage;

        /**
         * Create an empty assert information
         */
        AssertInfo() {
        }

        /**
         * Deserialize assert information from a stream.<br>
         * In other words, it read the stream and fill its content.<br>
         * See {@link #serialize(OutputStream)} for serialize
         * 
         * @param inputStream Strem to read
         * @throws IOException On reading issue
         */
        public void deserialize(final InputStream inputStream) throws IOException {
            this.classMethodName = HelperForNfc.deserializeString(inputStream);
            this.lineNumber = HelperForNfc.deserializeInteger(inputStream);
            this.message = HelperForNfc.deserializeString(inputStream);
            this.userMessage = HelperForNfc.deserializeString(inputStream);
        }

        /**
         * Serialize assert information to a stream.<br>
         * In other words, it write its content in a stream.<br>
         * See {@link #deserialize(InputStream)} for deserialize
         * 
         * @param outputStream Stream where write
         * @throws IOException On writing issue
         */
        public void serialize(final OutputStream outputStream) throws IOException {
            HelperForNfc.serialize(this.classMethodName, outputStream);
            HelperForNfc.serialize(this.lineNumber, outputStream);
            HelperForNfc.serialize(this.message, outputStream);
            HelperForNfc.serialize(this.userMessage, outputStream);

            outputStream.flush();
        }
    }

    /** Tag use in debug */
    private static final String TAG = TestScenario.class.getSimpleName();
    /** List of failed tests */
    private final ArrayList<AssertInfo> failedList;
    /** Actual number of succeed tests */
    private int successCount;
    /** Thread for start the test */
    private final ThreadedTask<Void> threadedStart = new ThreadedTask<Void>()
    {
        /**
         * Call when thread cancel <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @see org.opennfc.service.ThreadedTask#cancel()
         */
        @Override
        public void cancel() {
        }

        /**
         * Start the test
         * 
         * @param taskID Tasks ID
         * @param parameters Unused
         */
        @Override
        public void excuteTask(final int taskID, final Void... parameters) {
            HelperForNfc.sleep(128);

            TestScenario.this.start();
        }
    };

    /** Lock for synchronized */
    private final Object WAIT = new Object();

    /**
     * Create scenario
     */
    public TestScenario() {
        this.failedList = new ArrayList<TestScenario.AssertInfo>();
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final boolean value,
            final boolean expected) {
        if (value == expected) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final byte value,
            final byte expected) {
        if (value == expected) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final byte[] value,
            final byte[] expected) {
        if ((value == null && expected == null)) {
            this.successCount++;

            return;
        }

        if (value != null && expected != null && value.length == expected.length) {
            for (int i = value.length - 1; i >= 0; i--) {
                if (value[i] != expected[i]) {
                    this.failedList.add(this.createAssertInfo(HelperForNfc.toHexadecimal(value)
                            + " found but expected : " + HelperForNfc.toHexadecimal(expected),
                            userMessage));

                    return;
                }
            }

            this.successCount++;

            return;
        }

        this.failedList.add(this.createAssertInfo("Size for value " + (value == null
                ? -1
                : value.length) + " but expected size " + (expected == null
                ? -1
                : expected.length) + " (Note -1 means that the array is null)", userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final double value,
            final double expected) {
        if (value == expected) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final float value,
            final float expected) {
        if (value == expected) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final int value, final int expected) {
        if (value == expected) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final long value,
            final long expected) {
        if (value == expected) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final void assertEquals(final String userMessage, final short value,
            final short expected) {
        if (value == expected) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Assume that a value is what we expect
     * 
     * @param <T> Value type
     * @param userMessage Specific message
     * @param value Value to test
     * @param expected Expected value
     */
    protected final <T> void assertEquals(final String userMessage, final T value, final T expected) {
        if ((value == null && expected == null)
                || (expected != null && expected.equals(value) == true)) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo(value + " found but expected : " + expected,
                userMessage));
    }

    /**
     * Call inside test to say abnormal state
     * 
     * @param userMessage Message for explain why its wrong to go there
     */
    protected final void assertFail(final String userMessage) {
        this.failedList.add(this.createAssertInfo("Fail condition", userMessage));
    }

    /**
     * Assume that a condition is false
     * 
     * @param userMessage Explain message or {@code null} if test enough self
     *            explain
     * @param value Value to test
     */
    protected final void assertFalse(final String userMessage, final boolean value) {
        if (value == false) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo("Value must be false", userMessage));
    }

    /**
     * Assume that an object is not {@code null}
     * 
     * @param userMessage Specific message
     * @param value Value to test
     */
    protected final void assertNotNull(final String userMessage, final Object value) {
        if (value != null) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo("Value musn't be null", userMessage));
    }

    /**
     * Assume that an object is {@code null}
     * 
     * @param userMessage Specific message
     * @param value Value to test
     */
    protected final void assertNull(final String userMessage, final Object value) {
        if (value == null) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo("Value must be null", userMessage));
    }

    /**
     * Assume that a condition is true
     * 
     * @param userMessage Explain message or {@code null} if test enough self
     *            explain
     * @param value Value to test
     */
    protected final void assertTrue(final String userMessage, final boolean value) {
        if (value == true) {
            this.successCount++;
            return;
        }

        this.failedList.add(this.createAssertInfo("Value must be true", userMessage));
    }

    /**
     * Create an assert information
     * 
     * @param message Generic message
     * @param userMessage Specific message
     * @return Created assert information
     */
    private final AssertInfo createAssertInfo(final String message, final String userMessage) {
        final AssertInfo assertInfo = new AssertInfo();

        final Throwable throwable = new Throwable();
        final StackTraceElement stackTraceElement = throwable.getStackTrace()[2];

        assertInfo.classMethodName = stackTraceElement.getClassName().concat(".")
                    .concat(stackTraceElement.getMethodName());
        assertInfo.lineNumber = stackTraceElement.getLineNumber();
        assertInfo.message = message;
        assertInfo.userMessage = userMessage;

        return assertInfo;
    }

    /**
     * Deserialize some test information from a stream, in order to merge with
     * current information.<br>
     * See {@link #serialize(OutputStream)} for serialize
     * 
     * @param inputStream Stream to read
     * @throws IOException On reading issue
     */
    public void deserialize(final InputStream inputStream) throws IOException {
        this.successCount += HelperForNfc.deserializeInteger(inputStream);

        final int count = HelperForNfc.deserializeInteger(inputStream);
        AssertInfo assertInfo;

        for (int i = 0; i < count; i++) {
            assertInfo = new AssertInfo();
            assertInfo.deserialize(inputStream);

            this.failedList.add(assertInfo);
        }
    }

    /**
     * Serialize the collected test information to a stream.<br>
     * See {@link #deserialize(InputStream)} for deserialize
     * 
     * @param outputStream Stream where write
     * @throws IOException On writing issue
     */
    public void serialize(final OutputStream outputStream) throws IOException {
        HelperForNfc.serialize(this.successCount, outputStream);

        HelperForNfc.serialize(this.failedList.size(), outputStream);
        for (final AssertInfo assertInfo : this.failedList) {
            assertInfo.serialize(outputStream);
        }

        outputStream.flush();
    }

    /**
     * Internal call when test start.<br>
     * Extended classes put their code here
     */
    protected abstract void start();

    /**
     * Call when scenario test and block until the test call {@link #stopTest()}
     */
    public final void startTest() {
        this.successCount = 0;
        this.failedList.clear();

        ThreadedTaskManager.MANAGER.doInSeparateThread(this.threadedStart);

        synchronized (this.WAIT) {
            try {
                this.WAIT.wait();
            } catch (final Exception exception) {
            }
        }
    }

    /**
     * Call by the test to say its finish, can pass to next test one
     */
    public final void stopTest() {
        final int failedCount = this.failedList.size();

        Log.d(TestScenario.TAG, "---****<<<{([@])}>>>***---");
        Log.d(TestScenario.TAG, "Test result : " + this.successCount + " OK : " + failedCount
                + " Failed : On "
                + (this.successCount + failedCount) + " tests");

        for (final AssertInfo assertInfo : this.failedList) {
            Log.d(TestScenario.TAG, "   Failed : " + assertInfo.classMethodName + " at line "
                    + assertInfo.lineNumber);
            Log.d(TestScenario.TAG, "          : " + assertInfo.message);

            if (assertInfo.userMessage != null) {
                Log.d(TestScenario.TAG, "          : " + assertInfo.userMessage);
            }
        }

        Log.d(TestScenario.TAG, "---****<<<{([@])}>>>***---");

        synchronized (this.WAIT) {
            this.WAIT.notify();
        }
    }
}
