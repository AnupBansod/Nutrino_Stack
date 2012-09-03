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

/**
 * Utility class managing the handle values. Used only internally for the
 * implementation of the framework. package private
 *
 * @hide
 **/
final class SafeHandle extends RuntimeException {

    private static final long serialVersionUID = 5357827968277168621L;

    /** The handle value */
    private int[] mHandle = new int[1];

    /**
     * Creates an new insatnce of safe handle.
     *
     * @param handle the handle value.
     **/
    public SafeHandle(int handle) {
        this.mHandle[0] = handle;
    }

    /**
     * Creates an new insatnce of safe handle.
     **/
    public SafeHandle() {
        this.mHandle[0] = ConstantAutogen.W_NULL_HANDLE;
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
     * Releases the handle.
     **/
    public synchronized void release() {
        if (this.mHandle[0] != ConstantAutogen.W_NULL_HANDLE) {
            MethodAutogen.WBasicCloseHandle(this.mHandle[0]);
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
