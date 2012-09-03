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
 * Utility class managing buffers between the java environment and the native
 * environment. Used only internally for the implementation of the framework.
 * package private
 *
 * @hide
 **/
final class ByteBufferReference {

    /** The java buffer */
    private byte[] mBuffer;

    /** The handle of the native buffer */
    private int mHandle;

    /**
     * Creates an new byte buffer to share with the native functions.
     *
     * @param buffer the byte buffer to share. This value may be null or empty.
     * @param copyToNative if set to true, the content of the java buffer is
     *            copied to the native buffer.
     **/
    public ByteBufferReference(byte[] buffer, boolean copyToNative) {
        this(buffer, 0, (buffer != null) ? buffer.length : 0, copyToNative);
    }

    /**
     * Creates an new byte buffer to share with the native functions.
     *
     * @param buffer the byte buffer to share. This value may be null or empty.
     * @param offset the zero-based offset in bytes of the first byte to share.
     * @param length the number of bytes to share.
     * @param copyToNative if set to true, the content of the java buffer is
     *            copied to the native buffer.
     **/
    public ByteBufferReference(byte[] buffer, int offset, int length, boolean copyToNative) {
        this.mBuffer = buffer;

        if ((offset < 0) || (length < 0)) {
            throw new IndexOutOfBoundsException();
        }

        if (length == 0) {
            if (offset != 0) {
                throw new IndexOutOfBoundsException();
            }

            this.mHandle = ConstantAutogen.W_NULL_HANDLE;
        } else {
            if (buffer != null) {
                if ((offset + length) > buffer.length) {
                    throw new IndexOutOfBoundsException();
                }
            }

            if (copyToNative == true) {
                if (buffer == null) {
                    throw new IndexOutOfBoundsException();
                }
            } else {
                buffer = null;
            }

            this.mHandle = MethodAutogen.WJavaCreateByteBuffer(buffer, offset, length);

            if (this.mHandle == ConstantAutogen.W_NULL_HANDLE) {
                throw new OutOfMemoryError("Cannot allocate a buffer");
            }
        }
    }

    /**
     * Returns the handle of the handle of the buffer
     *
     * @return the handle value.
     **/
    public int getValue() {
        return this.mHandle;
    }

    /**
     * Releases the native buffer.
     *
     * @param copyFromNative if set to true, the content of the native buffer is
     *            copied to the java buffer before being release.
     **/
    public void release(boolean copyFromNative) {
        if (this.mHandle != ConstantAutogen.W_NULL_HANDLE) {
            MethodAutogen.WJavaReleaseByteBuffer(this.mHandle, copyFromNative ? this.mBuffer : null);
            this.mHandle = ConstantAutogen.W_NULL_HANDLE;
            this.mBuffer = null;
        }
    }

    /**
     * Finalizer
     **/
    @Override
    protected void finalize() {
        release(false);
    }
}
