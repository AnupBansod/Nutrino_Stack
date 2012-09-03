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
import org.opennfc.NfcPriority;
import org.opennfc.NfcException;
import org.opennfc.ConnectionProperty;

/**
 * The <a href="{@docRoot}overview-summary.html#nfc_tag_manager">NFC Tag Manager</a>
 * implements the methods to handle the NFC Tags.
 *
 * @since Open NFC 4.0
 */
public interface NfcTagManager {

    /**
     * To request the NDEF formatting of the tag only if the tag is blank, i.e.
     * is not already formatted and does not contain any data.
     */
    static final int ACTION_BIT_FORMAT_BLANK_TAG = ConstantAutogen.W_NDEF_ACTION_BIT_FORMAT_BLANK_TAG;

    /**
     * To request the NDEF formatting of the tag only if it is not already
     * formatted.
     */
    static final int ACTION_BIT_FORMAT_NON_NDEF_TAG = ConstantAutogen.W_NDEF_ACTION_BIT_FORMAT_NON_NDEF_TAG;

    /**
     * To force the NDEF formatting of the tag even if it is already formatted.
     */
    static final int ACTION_BIT_FORMAT_ALL = ConstantAutogen.W_NDEF_ACTION_BIT_FORMAT_ALL;

    /**
     * To request erase the tag content prior writing the message.
     */
    static final int ACTION_BIT_ERASE = ConstantAutogen.W_NDEF_ACTION_BIT_ERASE;

    /**
     * To request check the contents of the tag after writing.
     */
    static final int ACTION_BIT_CHECK_WRITE = ConstantAutogen.W_NDEF_ACTION_BIT_CHECK_WRITE;

    /**
     * To request lock of the tag after message has been written.
     */
    static final int ACTION_BIT_LOCK = ConstantAutogen.W_NDEF_ACTION_BIT_LOCK;

    /**
     * Registers a new tag listener.
     * <p/>
     * After a successful registration, the handler is called each time a NFC tag is detected.
     * {@link #unregisterTagListener unregisterTagListener()} should be called to unregister the listener.
     * <p/>
     * The handler method may use the methods of the connection {@link NfcTagConnection}
     * to read or write in the NFC tag.
     *
     * @param priority the priority used to listen to the tag must be a value
     *            between {@link NfcPriority#MINIMUM} and {@link NfcPriority#MAXIMUM} included.
     *            The exclusive access is requested with the value {@link NfcPriority#EXCLUSIVE}.
     *            If an exclusive access is requested, the error {@link NfcErrorCode#EXCLUSIVE_REJECTED}
     *            is returned if there is already another registration.
     *            If a shared access is requested, the error {@link NfcErrorCode#SHARE_REJECTED}
     *            is returned if there is already an exclusive registration.
     *
     * @param handler the {@link NfcTagDetectionEventHandler} whose
     *            {@link NfcTagDetectionEventHandler#onTagDetected onTagDetected()} method will
     *            be called for each connection with a new card.
     *
     * @throws SecurityException if the calling application is not allowed to listen to cards.
     * @throws SecurityException if the priority is {@link NfcPriority#MAXIMUM} or {@link NfcPriority#EXCLUSIVE}.
     *         and the calling application is not allowed to use these priority values.
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is already registered.
     * @throws IllegalArgumentException if <code>priority</code> is not valid.
     * @throws NfcException  in case of priority error or a registration error occurs.
     *
     * @see #unregisterTagListener unregisterTagListener()
     * @see NfcTagDetectionEventHandler
     */
    void registerTagListener(int priority, NfcTagDetectionEventHandler handler) throws NfcException;

    /**
     * Unregisters a card listener registered with {@link #registerTagListener}.
     *
     * @param handler the card detection handler.
     *
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is not already registered.
     *
     * @see #registerTagListener registerTagListener()
     */
    void unregisterTagListener(NfcTagDetectionEventHandler handler);


    /**
     * Registers a read event handler to detect a message on every tag detected.
     * <p/>
     * The event handler methods are called whether the read operation is successfully completed
     * or if a reading error occurred. The event handler is called for each new tag until
     * {@link #unregisterMessageReader unregisterMessageReader()} is used to unregister the handler.
     * <p/>
     * If no message of the specified type is found on the tag, the event handler
     * is called with the error code {@link NfcErrorCode#ITEM_NOT_FOUND}.
     * <p/>
     * {@link #registerMessageReader registerMessageReader()} does not return any error.
     * The errors are returned in the event handler.
     * <p/>
     * The parameters <code>tnf</code> and <code>type</code> define the type of NDEF message to detect.
     * The comparison algorithm is described in <a href="{@docRoot}overview-summary.html#ndef_type_comparison">Record Comparison</a>.
     * The message returned to the event handler is of the specified type.
     * The TNF values are defined in <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     * The special TNF value {@link NdefTypeNameFormat#ANY_TYPE} may be use to read every message regardless of the type.
     *
     * @param tnf the message TNF value as defined in <a href="{@docRoot}overview-summary.html#ndef_type_name_format">Type Name Format</a>.
     * @param type the type string.
     * @param priority the priority used to listen to the tag must be a value
     *            between {@link NfcPriority#MINIMUM} and
     *            {@link NfcPriority#MAXIMUM} included. The exclusive access
     *            {@link NfcPriority#EXCLUSIVE} cannot be requested.
     * @param eventHandler an instance of {@link ReadCompletionEventHandler}
     *            whose {@link ReadCompletionEventHandler#onTagRead onTagRead()}
     *            method will be called for each tag read.
     *
     * @throws SecurityException if the calling application is not allowed to listen to cards.
     * @throws SecurityException if the priority is {@link NfcPriority#MAXIMUM}.
     *         and the calling application is not allowed to use this priority value.
     * @throws IllegalArgumentException if <code>tnf</code> or <code>eventHandler</code> is null.
     * @throws IllegalArgumentException if <code>tnf</code> and <code>type</code> are not consistent.
     * @throws IllegalArgumentException if <code>eventHandler</code> is already registered.
     *
     * @see #unregisterMessageReader unregisterMessageReader()
     */
    void registerMessageReader(NdefTypeNameFormat tnf, String type, int priority,
            ReadCompletionEventHandler eventHandler);

    /**
     * Removes a tag reader registered with {@link #registerMessageReader registerMessageReader()}.
     *
     * @param eventHandler the read event handler to remove.
     *
     * @throws IllegalArgumentException if <code>eventHandler</code> is null.
     * @throws IllegalArgumentException if <code>eventHandler</code> was not previously registered.
     *
     * @see #registerMessageReader registerMessageReader()
     */
    void unregisterMessageReader(ReadCompletionEventHandler eventHandler);

    /**
     * Writes a message on the first tag present in the field, including prior formatting if requested.
     *
     * The callback method is always called whether the write operation is completed or if an error occurred.
     * The write operation may be canceled with the method {@link #cancelWriteMessageOnAnyTag cancelWriteMessageOnAnyTag()}.
     * If {@link #cancelWriteMessageOnAnyTag cancelWriteMessageOnAnyTag()} is called when the operation is not yet completed,
     * the callback method is called with the error code {@link NfcErrorCode#CANCEL}.
     * <p/>
     * {@link #writeMessageOnAnyTag writeMessageOnAnyTag()} does not return any error.
     * The error is returned in the callback method.
     * <p/>
     * {@link #writeMessageOnAnyTag writeMessageOnAnyTag()} writes a message
     * on any type of tag (Type 1, 2, 3, 4, 5, 6), including prior formatting if
     * required (depending on the value of the <code>actionFlags</code> passed).
     * <p/>
     * This method may be used to format the tag, see
     * <a href="{@docRoot}overview-summary.html#nfc_formatting_support">Formatting Support</a>.
     * The format operations {@link #ACTION_BIT_FORMAT_BLANK_TAG},
     * {@link #ACTION_BIT_FORMAT_NON_NDEF_TAG} and {@link #ACTION_BIT_FORMAT_ALL}
     * are mutually exclusive.
     * <p/>
     * If the {@link #ACTION_BIT_CHECK_WRITE} bit is set, the implementation reads
     * the content of the tag after writing. If the content is different from
     * the expected data, the error {@link NfcErrorCode#TAG_WRITE} is returned.
     * If not set, the content of the tag is not checked after writing.
     * <p/>
     * If the {@link #ACTION_BIT_LOCK} bit is set, the implementation locks the
     * content of the tag after writing If not set, there is no attempt to lock the tag.
     *
     * @param message the message to be written.
     *
     * @param priority the priority used to listen to the tag must be a value
     *            between {@link NfcPriority#MINIMUM} and
     *            {@link NfcPriority#MAXIMUM} included. The exclusive access
     *            {@link NfcPriority#EXCLUSIVE} cannot be requested.
     *
     * @param callback an instance of {@link WriteCompletionCallback} whose
     *            {@link WriteCompletionCallback#onTagWritten} method will be
     *            called when the message is written in a tag.
     *
     * @param actionFlags the write operations to perform. This is a bitmask
     *            with the following values:
     *            <ul>
     *            <li>{@link #ACTION_BIT_FORMAT_BLANK_TAG} to request the NDEF formatting of the tag only if the tag is blank, i.e. is not already formatted and does not contain any data,</li>
     *            <li>{@link #ACTION_BIT_FORMAT_NON_NDEF_TAG} to request the NDEF formatting of the tag only if it is not already formatted,</li>
     *            <li>{@link #ACTION_BIT_FORMAT_ALL} to force the NDEF formatting of the tag even if it is already formatted,</li>
     *            <li>{@link #ACTION_BIT_ERASE} to request erase the tag content prior writing the message,</li>
     *            <li>{@link #ACTION_BIT_CHECK_WRITE} to request check the contents of the tag after writing, and</li>
     *            <li>{@link #ACTION_BIT_LOCK} to request lock of the tag after message has been written.</li>
     *            </ul>
     *
     * @see #cancelWriteMessageOnAnyTag cancelWriteMessageOnAnyTag()
     *
     * @throws SecurityException if the calling application is not allowed to listen to cards.
     * @throws SecurityException if the priority is {@link NfcPriority#MAXIMUM}.
     *         and the calling application is not allowed to use this priority value.
     * @throws IllegalArgumentException if <code>message</code> or <code>callback</code> is null.
     */
    void writeMessageOnAnyTag(NdefMessage message, int priority, WriteCompletionCallback callback,
            int actionFlags);

    /**
     * Cancels a pending write operation initiated with
     * {@link #writeMessageOnAnyTag writeMessageOnAnyTag()}.
     * <p/>
     * If the operation is already completed or if <code>callback</code> is not
     * registered, this method has not effect.
     *
     * @param callback the callback instance used for the write operation.
     * @throws IllegalArgumentException if <code>callback</code> is null.
     * @see #writeMessageOnAnyTag writeMessageOnAnyTag()
     */
    void cancelWriteMessageOnAnyTag(WriteCompletionCallback callback);

    /**
     * Creates a new virtual tag.
     * <p/>
     * The emulation will start only when {@link VirtualTag#start VirtualTag.start()} is called.
     * <p/>
     * The virtual tag is a tag Type 4 A or B.
     * The Tag 4 specification gives a limit of 0x80FE bytes for the NDEF message.
     * The maximum size usable for the messages stored in the virtual tag is 0x80FC bytes (2 bytes are used for the file length).
     *
     * @param tagType the type of the tag to be created:
     *         <ul>
     *           <li>{@link ConnectionProperty#ISO_14443_4_A} for a NFC tag type 4 using ISO 14443-4 A, or</li>
     *           <li>{@link ConnectionProperty#ISO_14443_4_B} for a NFC tag type 4 using ISO 14443-4 B.</li>
     *         </ul>
     * @param identifier the tag identifier. For a tag of type A, the length of the identifier may be 4, 7 or 10 bytes.
     *        For a tag of type B, the length of the identifier shall be 4.
     * @param tagCapacity the capacity of the tag in bytes.
     * This value is automatically truncated depending on the maximum capacity of the tag type.
     *
     * @throws SecurityException if the calling application is not allowed to perform card emulation.
     * @throws IllegalArgumentException if <code>tagType</code> or <code>identifier</code> is null.
     * @throws IllegalArgumentException if <code>tagType</code> is unknown or the <code>identifier</code> length is not compliant with the tag type.
     * @throws IllegalArgumentException if <code>tagCapacity</code> if zero or negative.
     * @throws NfcException if the tag type is not supported.
     */
    VirtualTag createVirtualTag(ConnectionProperty tagType, byte[] identifier, int tagCapacity) throws NfcException;
}
