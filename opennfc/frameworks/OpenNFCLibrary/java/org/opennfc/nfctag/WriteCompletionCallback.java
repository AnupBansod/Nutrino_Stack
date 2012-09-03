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

import org.opennfc.NfcErrorCode;

/**
 * Interface definition used for receiving notifications from the
 * {@link NfcTagManager} when the operation has been completed. These callbacks
 * methods are called when a {@link WriteCompletionCallback} instance is registered with
 * the NFC tag manager using the {@link NfcTagManager#writeMessageOnAnyTag
 * writeMessageOnAnyTag()} method.
 *
 * @since Open NFC 4.0
 */
public interface WriteCompletionCallback {

    /**
     * Completion method called when the read operation has been completed
     * successfully.
     */
    void onTagWritten();

    /**
     * Called when an error occurs during the write operation.
     *
     * @param what the type of error that has occurred:
     *            <ul>
     *            <li>{@link NfcErrorCode#BAD_PARAMETER} Illegal value for the parameters of the write method.</li>
     *            <li>{@link NfcErrorCode#SHARE_REJECTED} The shared access is rejected.</li>
     *            <li>{@link NfcErrorCode#CANCEL} The operation is canceled using the {@link NfcTagManager#cancelWriteMessageOnAnyTag cancelWriteMessageOnAnyTag()} method.</li>
     *            <li>{@link NfcErrorCode#CONNECTION_COMPATIBILITY} The tag is not compliant with a tag type.</li>
     *            <li>{@link NfcErrorCode#TIMEOUT} A timeout occurred during the communication with the tag.</li>
     *            <li>{@link NfcErrorCode#LOCKED_TAG} The operation failed because the tag is locked.</li>
     *            <li>{@link NfcErrorCode#TAG_NOT_LOCKABLE} The operation failed because the tag is not lockable.</li>
     *            <li>{@link NfcErrorCode#TAG_FULL} The message is too large for the remaining free space in the tag.</li>
     *            <li>{@link NfcErrorCode#TAG_WRITE} The content read in the tag is different from the content written.</li>
     *            </ul>
     */
    void onWriteError(NfcErrorCode what);
}

