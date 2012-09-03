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
 * The specific constants of the error code values used with the NFC methods.
 *
 * @since Open NFC 4.0
 */
public enum NfcErrorCode {
    /** The implementation does not support the requested version of the APIs. */
    VERSION_NOT_SUPPORTED (ConstantAutogen.W_ERROR_VERSION_NOT_SUPPORTED,
     "The implementation does not support the requested version of the APIs."),

    /** The specified item is not found. */
    ITEM_NOT_FOUND (ConstantAutogen.W_ERROR_ITEM_NOT_FOUND,
     "The specified item is not found."),

    /** The buffer specified in output to receive a result is too short. */
    BUFFER_TOO_SHORT (ConstantAutogen.W_ERROR_BUFFER_TOO_SHORT,
    "The buffer specified in output to receive a result is too short."),

    /** An error is detected when writing data in persistent memory. */
    PERSISTENT_DATA (ConstantAutogen.W_ERROR_PERSISTENT_DATA,
    "An error is detected when writing data in persistent memory."),

    /** No event is available in the event queue. */
    NO_EVENT (ConstantAutogen.W_ERROR_NO_EVENT,
    "No event is available in the event queue."),

    /** A wait operation is canceled. */
    WAIT_CANCELLED (ConstantAutogen.W_ERROR_WAIT_CANCELLED,
    "A wait operation is cancelled."),

    /**
     * The UICC is not present in the slot or an error occurred during the
     * communication with the UICC.
     */
    UICC_COMMUNICATION (ConstantAutogen.W_ERROR_UICC_COMMUNICATION,
    "The UICC is not present in the slot or an error occurred during the communication with the UICC."),

    /** A bad handle value is detected. */
    BAD_HANDLE (ConstantAutogen.W_ERROR_BAD_HANDLE, "A bad handle value is detected."),

    /**
     * The exclusive access is rejected because there is already another
     * registration for this type.
     */
    EXCLUSIVE_REJECTED (ConstantAutogen.W_ERROR_EXCLUSIVE_REJECTED,
    "The exclusive access is rejected because there is already another registration for this type."),

    /**
     * The shared access is rejected because there is already an exclusive
     * registration for this type.
     */
    SHARE_REJECTED (ConstantAutogen.W_ERROR_SHARE_REJECTED,
    "The shared access is rejected because there is already an exclusive registration for this type."),

    /** A wrong value is detected for a parameter. */
    BAD_PARAMETER (ConstantAutogen.W_ERROR_BAD_PARAMETER,
    "A wrong value is detected for a parameter."),

    /** The specified protocol is not supported. */
    RF_PROTOCOL_NOT_SUPPORTED (ConstantAutogen.W_ERROR_RF_PROTOCOL_NOT_SUPPORTED,
    "The specified protocol is not supported."),

    /**
     * The connection is not compliant with a protocol requested to execute a
     * function.
     */
    CONNECTION_COMPATIBILITY (ConstantAutogen.W_ERROR_CONNECTION_COMPATIBILITY,
    "The connection is not compliant with a protocol requested to execute a method."),

    /** The buffer specified in input is too large. */
    BUFFER_TOO_LARGE (ConstantAutogen.W_ERROR_BUFFER_TOO_LARGE,
    "The buffer specified in input is too large."),

    /** An index value is out of the valid range for this type of index. */
    INDEX_OUT_OF_RANGE (ConstantAutogen.W_ERROR_INDEX_OUT_OF_RANGE,
    "An index value is out of the valid range for this type of index."),

    /** Resources are missing to perform the operation. */
    OUT_OF_RESOURCE (ConstantAutogen.W_ERROR_OUT_OF_RESOURCE,
    "Resources are missing to perform the operation."),

    /** A tag is ill-formatted. */
    BAD_TAG_FORMAT (ConstantAutogen.W_ERROR_BAD_TAG_FORMAT,
    "A tag is ill-formatted."),

    /** A NDEF message is ill-formatted. */
    BAD_NDEF_FORMAT (ConstantAutogen.W_ERROR_BAD_NDEF_FORMAT,
    "A NDEF message is ill-formatted."),

    /** The NDEF message type is unknown. */
    NDEF_UNKNOWN (ConstantAutogen.W_ERROR_NDEF_UNKNOWN,
    "The NDEF message type is unknown."),

    /**
     * A write operation failed because the tag is locked or protected against
     * writing operation.
     */
    LOCKED_TAG (ConstantAutogen.W_ERROR_LOCKED_TAG,
    "A write operation failed because the tag is locked or protected against writing operation."),

    /** A write operation failed because the tag has insufficient free space. */
    TAG_FULL (ConstantAutogen.W_ERROR_TAG_FULL,
    "A write operation failed because the tag has insufficient free space."),

    /** An asynchronous operation is not performed because it was canceled. */
    CANCEL (ConstantAutogen.W_ERROR_CANCEL,
    "An asynchronous operation is not performed because it was canceled."),

    /** The communication failed because a timeout expired. */
    TIMEOUT (ConstantAutogen.W_ERROR_TIMEOUT,
    "The communication failed because a timeout expired."),

    /**
     * The communication failed because a fault in the data integrity is
     * detected.
     */
    TAG_DATA_INTEGRITY (ConstantAutogen.W_ERROR_TAG_DATA_INTEGRITY,
    "The communication failed because a fault in the data integrity is detected."),

    /** An error in the NFC HAL protocol is detected. */
    NFC_HAL_COMMUNICATION (ConstantAutogen.W_ERROR_NFC_HAL_COMMUNICATION,
    "An error in the NFC HAL protocol is detected."),

    /** A record does not have the expected RTD. */
    WRONG_RTD (ConstantAutogen.W_ERROR_WRONG_RTD,
    "A record does not have the expected RTD."),

    /** An error is detected in the data written into a tag. */
    TAG_WRITE (ConstantAutogen.W_ERROR_TAG_WRITE,
    "An error is detected in the data written into a tag."),

    /**
     * The method cannot be executed in the current mode of the NFC
     * Controller.
     */
    BAD_NFCC_MODE (ConstantAutogen.W_ERROR_BAD_NFCC_MODE,
    "The method cannot be executed in the current mode of the NFC Controller."),

    /**
     * This error is returned by a registering method when the maximum number
     * of handlers is reached.
     */
    TOO_MANY_HANDLERS (ConstantAutogen.W_ERROR_TOO_MANY_HANDLERS,
    "This error is returned by a registering method when the maximum number of handlers is reached."),

    /**
     * A method cannot be executed because the current sate is no valid for
     * this execution.
     */
    BAD_STATE (ConstantAutogen.W_ERROR_BAD_STATE,
    "A method cannot be executed because the current sate is no valid for this execution."),

    /** An error is detected in the format of the firmware update buffer. */
    BAD_FIRMWARE_FORMAT (ConstantAutogen.W_ERROR_BAD_FIRMWARE_FORMAT,
    "An error is detected in the format of the firmware update buffer."),

    /** The signature of the firmware update buffer is not valid. */
    BAD_FIRMWARE_SIGNATURE (ConstantAutogen.W_ERROR_BAD_FIRMWARE_SIGNATURE,
    "The signature of the firmware update buffer is not valid."),

    /** The hardware does not boot correctly. */
    DURING_HARDWARE_BOOT (ConstantAutogen.W_ERROR_DURING_HARDWARE_BOOT,
    "The hardware does not boot correctly."),

    /** The firmware does not boot correctly. */
    DURING_FIRMWARE_BOOT (ConstantAutogen.W_ERROR_DURING_FIRMWARE_BOOT,
    "The firmware does not boot correctly."),

    /** The requested feature is not supported. */
    FEATURE_NOT_SUPPORTED (ConstantAutogen.W_ERROR_FEATURE_NOT_SUPPORTED,
    "The requested feature is not supported."),

    /** An error is detected in the client-server protocol. */
    CLIENT_SERVER_PROTOCOL (ConstantAutogen.W_ERROR_CLIENT_SERVER_PROTOCOL,
    "An error is detected in the client-server protocol."),

    /**
     * The method is not supported by the current version of the
     * implementation.
     */
    FUNCTION_NOT_SUPPORTED (ConstantAutogen.W_ERROR_FUNCTION_NOT_SUPPORTED,
    "The method is not supported by the current version of the implementation."),

    /** The lock operation failed because the tag is not lockable. */
    TAG_NOT_LOCKABLE (ConstantAutogen.W_ERROR_TAG_NOT_LOCKABLE,
    "The lock operation failed because the tag is not lockable."),

    /** The lock or write operation failed because the specified item is locked. */
    ITEM_LOCKED (ConstantAutogen.W_ERROR_ITEM_LOCKED,
    "The lock or write operation failed because the specified item is locked."),

    /** An error is returned by a synchronization function of the OS. */
    SYNC_OBJECT (ConstantAutogen.W_ERROR_SYNC_OBJECT,
    "An error is returned by a synchronization function of the OS."),

    /** Temporary error code, the caller should retry the call. */
    RETRY (ConstantAutogen.W_ERROR_RETRY,
    "Temporary error code, the caller should retry the call."),

    /** Error returned by the driver. */
    DRIVER (ConstantAutogen.W_ERROR_DRIVER,
    "Error returned by the driver."),

    /** Error caused by a missing information. */
    MISSING_INFO (ConstantAutogen.W_ERROR_MISSING_INFO,
    "Error caused by a missing information."),

    /**
     * The remote device rejects the connection attempt from the local Peer to
     * Peer client.
     */
    P2P_CLIENT_REJECTED (ConstantAutogen.W_ERROR_P2P_CLIENT_REJECTED,
    "The remote device rejects the connection attempt from the local Peer to Peer client."),

    /**
     * Error in the communication with the NFC Controller in the NFC HAL
     * implementation.
     */
    NFCC_COMMUNICATION (ConstantAutogen.W_ERROR_NFCC_COMMUNICATION,
    "Error in the communication with the NFC Controller in the NFC HAL implementation."),

    /**
     * Error in the communication on the RF interface. At any level of the
     * protocol.
     */
    RF_COMMUNICATION (ConstantAutogen.W_ERROR_RF_COMMUNICATION,
    "Error in the communication on the RF interface. At any level of the protocol."),

    /**
     * The firmware is not compliant with the NFC Controller type or with the
     * NFC HAL implementation.
     */
    BAD_FIRMWARE_VERSION (ConstantAutogen.W_ERROR_BAD_FIRMWARE_VERSION,
    "The firmware is not compliant with the NFC Controller type or with the NFC HAL implementation."),

    /**
     * Unable to provide the requested information, typically due to non
     * existence of a unique value valid for all the specified range.
     */
    HETEROGENEOUS_DATA (ConstantAutogen.W_ERROR_HETEROGENEOUS_DATA,
    "Unable to provide the requested information, typically due to non existence of a unique value valid for all the specified range."),

    /** An error is detected in the client-server communication. */
    CLIENT_SERVER_COMMUNICATION (ConstantAutogen.W_ERROR_CLIENT_SERVER_COMMUNICATION,
    "An error is detected in the client-server communication."),

    /** A security error is detected. */
    W_ERROR_SECURITY (ConstantAutogen.W_ERROR_SECURITY,
    "A security error is detected.");

     /** The error code */
     private int code;

     /**
      * The error message.
      *
      * package private
      *
      * @hide
      **/
     String message;

     /* private enum constructor */
     private NfcErrorCode(int code, String message) {
        this.code = code;
        this.message = message;
     }

     /**
      * Returns the error code corresponding to a value.
      *
      * @param value  the error value.
      *
      * @return  the error code.
      *
      * @hide
      **/
     public static NfcErrorCode getCode(int value) {

        switch(value) {
          case ConstantAutogen.W_ERROR_VERSION_NOT_SUPPORTED: return VERSION_NOT_SUPPORTED;
          case ConstantAutogen.W_ERROR_ITEM_NOT_FOUND: return ITEM_NOT_FOUND;
          case ConstantAutogen.W_ERROR_BUFFER_TOO_SHORT: return BUFFER_TOO_SHORT;
          case ConstantAutogen.W_ERROR_PERSISTENT_DATA: return PERSISTENT_DATA;
          case ConstantAutogen.W_ERROR_NO_EVENT: return NO_EVENT;
          case ConstantAutogen.W_ERROR_WAIT_CANCELLED: return WAIT_CANCELLED;
          case ConstantAutogen.W_ERROR_UICC_COMMUNICATION: return UICC_COMMUNICATION;
          case ConstantAutogen.W_ERROR_BAD_HANDLE: return BAD_HANDLE;
          case ConstantAutogen.W_ERROR_EXCLUSIVE_REJECTED: return EXCLUSIVE_REJECTED;
          case ConstantAutogen.W_ERROR_SHARE_REJECTED: return SHARE_REJECTED;
          case ConstantAutogen.W_ERROR_BAD_PARAMETER: return BAD_PARAMETER;
          case ConstantAutogen.W_ERROR_RF_PROTOCOL_NOT_SUPPORTED: return RF_PROTOCOL_NOT_SUPPORTED;
          case ConstantAutogen.W_ERROR_CONNECTION_COMPATIBILITY: return CONNECTION_COMPATIBILITY;
          case ConstantAutogen.W_ERROR_BUFFER_TOO_LARGE: return BUFFER_TOO_LARGE;
          case ConstantAutogen.W_ERROR_INDEX_OUT_OF_RANGE: return INDEX_OUT_OF_RANGE;
          case ConstantAutogen.W_ERROR_OUT_OF_RESOURCE: return OUT_OF_RESOURCE;
          case ConstantAutogen.W_ERROR_BAD_TAG_FORMAT: return BAD_TAG_FORMAT;
          case ConstantAutogen.W_ERROR_BAD_NDEF_FORMAT: return BAD_NDEF_FORMAT;
          case ConstantAutogen.W_ERROR_NDEF_UNKNOWN: return NDEF_UNKNOWN;
          case ConstantAutogen.W_ERROR_LOCKED_TAG: return LOCKED_TAG;
          case ConstantAutogen.W_ERROR_TAG_FULL: return TAG_FULL;
          case ConstantAutogen.W_ERROR_CANCEL: return CANCEL;
          case ConstantAutogen.W_ERROR_TIMEOUT: return TIMEOUT;
          case ConstantAutogen.W_ERROR_TAG_DATA_INTEGRITY: return TAG_DATA_INTEGRITY;
          case ConstantAutogen.W_ERROR_NFC_HAL_COMMUNICATION: return NFC_HAL_COMMUNICATION;
          case ConstantAutogen.W_ERROR_WRONG_RTD: return WRONG_RTD;
          case ConstantAutogen.W_ERROR_TAG_WRITE: return TAG_WRITE;
          case ConstantAutogen.W_ERROR_BAD_NFCC_MODE: return BAD_NFCC_MODE;
          case ConstantAutogen.W_ERROR_TOO_MANY_HANDLERS: return TOO_MANY_HANDLERS;
          case ConstantAutogen.W_ERROR_BAD_STATE: return BAD_STATE;
          case ConstantAutogen.W_ERROR_BAD_FIRMWARE_FORMAT: return BAD_FIRMWARE_FORMAT;
          case ConstantAutogen.W_ERROR_BAD_FIRMWARE_SIGNATURE: return BAD_FIRMWARE_SIGNATURE;
          case ConstantAutogen.W_ERROR_DURING_HARDWARE_BOOT: return DURING_HARDWARE_BOOT;
          case ConstantAutogen.W_ERROR_DURING_FIRMWARE_BOOT: return DURING_FIRMWARE_BOOT;
          case ConstantAutogen.W_ERROR_FEATURE_NOT_SUPPORTED: return FEATURE_NOT_SUPPORTED;
          case ConstantAutogen.W_ERROR_CLIENT_SERVER_PROTOCOL: return CLIENT_SERVER_PROTOCOL;
          case ConstantAutogen.W_ERROR_FUNCTION_NOT_SUPPORTED: return FUNCTION_NOT_SUPPORTED;
          case ConstantAutogen.W_ERROR_TAG_NOT_LOCKABLE: return TAG_NOT_LOCKABLE;
          case ConstantAutogen.W_ERROR_ITEM_LOCKED: return ITEM_LOCKED;
          case ConstantAutogen.W_ERROR_SYNC_OBJECT: return SYNC_OBJECT;
          case ConstantAutogen.W_ERROR_RETRY: return RETRY;
          case ConstantAutogen.W_ERROR_DRIVER: return DRIVER;
          case ConstantAutogen.W_ERROR_MISSING_INFO: return MISSING_INFO;
          case ConstantAutogen.W_ERROR_P2P_CLIENT_REJECTED: return P2P_CLIENT_REJECTED;
          case ConstantAutogen.W_ERROR_NFCC_COMMUNICATION: return NFCC_COMMUNICATION;
          case ConstantAutogen.W_ERROR_RF_COMMUNICATION: return RF_COMMUNICATION;
          case ConstantAutogen.W_ERROR_BAD_FIRMWARE_VERSION: return BAD_FIRMWARE_VERSION;
          case ConstantAutogen.W_ERROR_HETEROGENEOUS_DATA: return HETEROGENEOUS_DATA;
          case ConstantAutogen.W_ERROR_CLIENT_SERVER_COMMUNICATION: return CLIENT_SERVER_COMMUNICATION;
        }

        throw new InternalError("Wrong error code");
     }
}
