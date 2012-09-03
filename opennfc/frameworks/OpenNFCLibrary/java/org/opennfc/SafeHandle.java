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

/**
 * Utility class managing the handle values. Used only internally for the
 * implementation of the framework. package private
 *
 * @hide
 **/
final class SafeHandle extends RuntimeException {

    private static final long serialVersionUID = -6000337901002516818L;

    /** Handle type: normal or connection handle */
    public static final int NORMAL = 0;

    /** Handle type: handle with async close */
    public static final int ASYNC = 1;

    /** Handle type: logical channel */
    public static final int LOGICAL_CHANNEL = 2;

    /** The handle type */
    private int _type;

    /** The handle value */
    private int[] mHandle = new int[1];

    /**
     * Creates an new instance of safe handle.
     **/
    public SafeHandle() {
        this.mHandle[0] = ConstantAutogen.W_NULL_HANDLE;
        _type = NORMAL;
    }

    /**
     * Creates an new instance of safe handle.
     *
     * @param handle The handle value.
     **/
    public SafeHandle(int handle) {
        this.mHandle[0] = handle;
        _type = NORMAL;
    }

    /**
     * Creates an new instance of safe handle.
     *
     * @param handle The handle value.
     *
     * @param type  The handle type.
     **/
    public SafeHandle(int handle, int type) {
        this.mHandle[0] = handle;
        _type = type;
    }

    /**
     * Returns the handle value.
     *
     * @return the handle value.
     **/
    public int getValue() {
        return this.mHandle[0];
    }

    /**
     * Returns a reference on the handle.
     *
     * @return the handle reference.
     **/
    public int[] getReference() {
        return this.mHandle;
    }

    @Override
    public int hashCode() {
        return this.mHandle[0];
    }

    /**
     * Checks if the handle is null.
     **/
    public boolean isNull() {
       return (this.mHandle[0] == ConstantAutogen.W_NULL_HANDLE);
    }

    /**
     * Calls work perfomed for connection handle only.
     **/
    public synchronized void close(boolean continuePolling, boolean cardApplicationMatch) {

        if (this.mHandle[0] != ConstantAutogen.W_NULL_HANDLE) {
            if(this._type == NORMAL) {
               MethodAutogen.WReaderHandlerWorkPerformed(this.mHandle[0], continuePolling, cardApplicationMatch);
               this.mHandle[0] = ConstantAutogen.W_NULL_HANDLE;
            }
            else {
               release();
            }
        }
    }

    /**
     * Releases the handle.
     **/
    public synchronized void release() {
        if (this.mHandle[0] != ConstantAutogen.W_NULL_HANDLE) {
            try {
               if(this._type == NORMAL) {
                  MethodAutogen.WBasicCloseHandle(this.mHandle[0]);
               }
               else if(this._type == ASYNC) {
                  CallbackGenericCompletion callback = new CallbackGenericCompletion();
                  MethodAutogen.WBasicCloseHandleSafe(this.mHandle[0], callback.getId());
                  callback.waitForResult();
               }
               else if(this._type == LOGICAL_CHANNEL) {
                  CallbackGenericCompletion callback = new CallbackGenericCompletion();
                  MethodAutogen.W7816CloseLogicalChannel(this.mHandle[0], callback.getId());
                  callback.waitForResult();
               }
            }
            catch( Exception e ) {}
            this.mHandle[0] = ConstantAutogen.W_NULL_HANDLE;
        }
    }

    /**
     * Finalizer
     **/
    @Override
    protected void finalize() {
        release();
    }
}
