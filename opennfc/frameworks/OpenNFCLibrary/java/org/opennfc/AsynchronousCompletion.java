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

/**
 * Utility class used to handle the asynchronous completion of some methods.
 * package private
 *
 * @hide
 **/
abstract class AsynchronousCompletion {

    /** The result value */
    private int mResult;

    /** The error value */
    private int mErrorValue;

    /** A possible error instance to throw */
    private Error mErrorException;

    /** A possible exception instance to throw */
    private NfcException mException;

    /** A possible runtime exception instance to throw */
    private RuntimeException mRuntimeException;

    /* The event flag */
    private boolean mIsEvent;

    /* The callback list */
    private static Vector<AsynchronousCompletion> sCallbackList = new Vector<AsynchronousCompletion>(
            10);

    /* The pump thread identifier */
    private static long sPumpThreadId;

    /**
     * Creates an new asynchronous completion object.
     *
     * @param isEvent set to true for an event completion.
     **/
    protected AsynchronousCompletion(boolean isEvent) {
        this.mIsEvent = isEvent;
        Vector<AsynchronousCompletion> list = AsynchronousCompletion.sCallbackList;

        synchronized (list) {
            list.add(this);
        }
    }

    static private int[] staticArgs = new int[8];

    /**
     * Aborts the waiting threads if any.
     **/
    /* package private */static void abortAll() {
        Vector<AsynchronousCompletion> list = AsynchronousCompletion.sCallbackList;
        synchronized (list) {
            for (int i = 0; i < list.size(); i++) {
                list.get(i).abortWait();
            }
            list.clear();
        }
    }

    /**
     * Dispatches a callback call.
     **/
    /* package private */static void dispatchEvent() {
        AsynchronousCompletion ac = null;
        Vector<AsynchronousCompletion> list = AsynchronousCompletion.sCallbackList;

        if (AsynchronousCompletion.sPumpThreadId == 0) {
            AsynchronousCompletion.sPumpThreadId = Thread.currentThread().getId();
        }

        while (MethodAutogen.WDFCPumpJNICallback(staticArgs)) {
            synchronized (list) {
                int hashCode = staticArgs[0];
                int num = list.size();
                for (int i = 0; i < num; i++) {
                    ac = list.get(i);
                    if (ac.hashCode() == hashCode) {
                        if (ac.mIsEvent == false) {
                            list.remove(ac);
                        }
                        break;
                    }
                    ac = null;
                }

                if (ac == null) {
                    throw new InternalError("AsynchronousCompletion: dispatch id error");
                }
            }
            ac.invokeFunction(staticArgs);
        }
    }

    /**
     * Returns the callback identifier.
     *
     * @return the identifier.
     **/
    public int getId() {
        return hashCode();
    }

    /**
     * Releases an event callback.
     **/
    public void release() {
        if (this.mIsEvent == false) {
            throw new InternalError("AsynchronousCompletion: release a normal callback");
        }

        Vector<AsynchronousCompletion> list = AsynchronousCompletion.sCallbackList;

        synchronized (list) {
            list.remove(this);
        }
    }

    /**
     * Invokes the callback method.
     *
     * @param args The callback arguments.
     **/
    protected abstract void invokeFunction(int[] args);

    /* The notification flag */
    private boolean isNotified = false;

    /**
     * Waits for the method completion.
     **/
    public void waitForCompletion() {

        if (Thread.currentThread().getId() == AsynchronousCompletion.sPumpThreadId) {
            throw new IllegalThreadStateException(
                    "Programming error: blocking method called from an event handler");
        }

        try {
            synchronized (this) {
                if (this.isNotified == false) {
                    wait();
                }
                this.isNotified = false;
            }
        } catch (InterruptedException e) {
            throw new IllegalStateException("Thread was interrupted", e);
        }

        if (this.mErrorException != null) {
            throw this.mErrorException;
        }

        if (this.mRuntimeException != null) {
            throw this.mRuntimeException;
        }
    }

    /**
     * Waits for the method completion with a result.
     *
     * @return The result value.
     **/
    public int waitForResult() {
        waitForCompletion();
        return this.mResult;
    }

    /**
     * Returns the error set with signalResult().
     *
     * @return The error value.
     **/
    public int getErrorValue() {
        return this.mErrorValue;
    }

    /**
     * Waits for the method completion or for an Exception.
     *
     * @throws NfcException in case of error.
     **/
    public void waitForCompletionOrException() throws NfcException {
        waitForCompletion();
        if (this.mException != null) {
            throw this.mException;
        }
    }

    /**
     * Waits for the method completion with a result or for an Exception.
     *
     * @return The result value.
     * @throws NfcException in case of error.
     **/
    public int waitForResultOrException() throws NfcException {
        waitForCompletion();
        if (this.mException != null) {
            throw this.mException;
        }
        return this.mResult;
    }

    /**
     * Aborts the wait if any
     **/
    private void abortWait() {
        this.mRuntimeException = new IllegalStateException("Stack operation aborted");
        signalCompletion();
    }

    /**
     * Signals the completion of the method.
     **/
    protected void signalCompletion() {
        synchronized (this) {
            isNotified = true;
            notify();
        }
    }

    /**
     * Signals the completion of the method with a result.
     *
     * @param result The result of the method.
     **/
    protected void signalResult(int result) {
        this.mResult = result;
        signalCompletion();
    }

    /**
     * Signals the completion of the method with a result and an error value.
     *
     * @param result The result of the method.
     * @param errorValue The error value.
     **/
    protected void signalResult(int result, int errorValue) {
        this.mResult = result;
        this.mErrorValue = errorValue;
        signalCompletion();
    }

    /**
     * Signals an Error.
     *
     * @param errorException The error instance.
     **/
    protected void signalException(Error errorException) {
        this.mErrorException = errorException;
        signalCompletion();
    }

    /**
     * Signals a Runtime Exception.
     *
     * @param runtimeException The runtime exception instance.
     **/
    protected void signalException(RuntimeException runtimeException) {
        this.mRuntimeException = runtimeException;
        signalCompletion();
    }

    /**
     * Signals an NFC Exception.
     *
     * @param exception The exception instance.
     **/
    protected void signalException(NfcException exception) {
        this.mException = exception;
        signalCompletion();
    }

}
