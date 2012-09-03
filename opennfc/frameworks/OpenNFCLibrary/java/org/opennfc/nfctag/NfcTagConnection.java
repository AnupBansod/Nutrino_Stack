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
import org.opennfc.NfcException;
import org.opennfc.cardlistener.Connection;

/**
 * This interface defines the basic functionality for exchanging NFC Forum
 * formatted data with NFC tags. The actual data is stored in the
 * {@link NdefMessage} object containing the data in NDEF records. This
 * interface provides the means to read data from and write data to a target.
 *
 * @since Open NFC 4.0
 */
public interface NfcTagConnection extends Connection {

    /**
     * Returns the value of the tag identifier.
     *
     * @return the identifier of the tag or null if there is no identifier.
     **/
    byte[] getIdentifier() throws NfcException;

    /**
     * Returns the read-only status of the tag.
     *
     * @return true if the tag cannot be written,
     * false if the tag is read-only or if the status is unknown.
     **/
    boolean isReadOnly() throws NfcException;

    /**
     * Returns the capability of the tag to be set in read-only mode.
     *
     * @return true if the tag can be set is read-only mode,
     * false is the tag cannot be set in read-only or if this information is unknown.
     **/
    boolean isLockable() throws NfcException;

    /**
     * Returns the free size available in the tag.
     *
     * @return the free size in bytes of the space available for new records.
     *         The method returns zero if the free size is unknown.
     **/
    int getFreeSpaceSize() throws NfcException;

    /**
     * Reads a NDEF message from the tag.
     *
     * @return the NDEF message read in the tag.
     * @throws NfcException in case of error.
     */
    NdefMessage readMessage() throws NfcException;

    /**
     * Writes a NDEF message into the tag.
     *
     * <p/>
     * {@link #writeMessage writeMessage()} writes a message
     * on any type of tag (Type 1, 2, 3, 4, 5, 6), including prior formatting if
     * required (depending on the value of the <code>actionFlags</code> passed).
     * <p/>
     * This method may be used to format the tag, see
     * <a href="{@docRoot}overview-summary.html#nfc_formatting_support">Formatting Support</a>.
     * The format operations {@link NfcTagManager#ACTION_BIT_FORMAT_BLANK_TAG},
     * {@link NfcTagManager#ACTION_BIT_FORMAT_NON_NDEF_TAG} and {@link NfcTagManager#ACTION_BIT_FORMAT_ALL}
     * are mutually exclusive.
     * <p/>
     * If the {@link NfcTagManager#ACTION_BIT_CHECK_WRITE} bit is set, the implementation reads
     * the content of the tag after writing. If the content is different from
     * the expected data, the error {@link NfcErrorCode#TAG_WRITE} is returned.
     * If not set, the content of the tag is not checked after writing.
     * <p/>
     * If the {@link NfcTagManager#ACTION_BIT_LOCK} bit is set, the implementation locks the
     * content of the tag after writing If not set, there is no attempt to lock the tag.
     *
     * @param message the NDEF message to write.
     * @param actionFlags the write operations to perform. This is a bitmask with the following values:
     *     <ul>
     *       <li>{@link NfcTagManager#ACTION_BIT_FORMAT_BLANK_TAG} to request the NDEF formatting of the tag only if the tag is blank, i.e. is not already formatted and does not contain any data,</li>
     *       <li>{@link NfcTagManager#ACTION_BIT_FORMAT_NON_NDEF_TAG} to request the NDEF formatting of the tag only if it is not already formatted,</li>
     *       <li>{@link NfcTagManager#ACTION_BIT_FORMAT_ALL} to force the NDEF formatting of the tag even if it is already formatted,</li>
     *       <li>{@link NfcTagManager#ACTION_BIT_ERASE} to request erase the tag content prior writing the message,</li>
     *       <li>{@link NfcTagManager#ACTION_BIT_CHECK_WRITE} to request check the contents of the tag after writing, and</li>
     *       <li>{@link NfcTagManager#ACTION_BIT_LOCK} to request lock of the tag after message has been written.</li>
     *     </ul>
     * @throws IllegalArgumentException if <code>message</code> is null
     *         or if <code>actionFlags</code> is invalid.
     * @throws NfcException in case of error.
     */
    void writeMessage(NdefMessage message, int actionFlags) throws NfcException;

}
