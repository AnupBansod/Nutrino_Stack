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
 * This NfcException is thrown when an error is detected in the NFC API.
 *
 * @since Open NFC 4.0
 */
public final class NfcException extends Exception {

    /**
     * The serialization identifier.
     */
    private static final long serialVersionUID = -96339034562339311L;

    /** The internal code */
    private transient NfcErrorCode mCode;

    /**
     * Creates a new NFC exception.
     *
     * @param  message  the exception message.
     **/
    public NfcException(String message) {
        super(message);
        this.mCode = null;
    }

    /**
     * Creates a new NFC exception.
     *
     * @param  message  the exception message.
     *
     * @param  code  the code of the error causing the exception.
     **/
    public NfcException(String message, NfcErrorCode code) {
        super(message);
        this.mCode = code;
    }

    /**
     * Creates a new NFC exception.
     *
     * @param  code  the code of the error causing the exception.
     **/
    public NfcException(NfcErrorCode code) {
        this.mCode = code;
    }

    /**
     * Returns the code of the error causing the exception.
     *
     * @return the error code.
     **/
    public NfcErrorCode getCode() {
        return this.mCode;
    }

    /**
     * Returns the string value of the exception.
     *
     * @return the string representing the exception.
     **/
    @Override
    public String toString() {
      return "NfcException : " + getMessage();
    }

    /**
     * Returns the message of the exception.
     *
     * @return the message of the exception, null if there is no message.
     **/
    @Override
    public String getMessage() {
      String message = super.getMessage();

      if(this.mCode != null) {
         if(message == null) {
            message = this.mCode.message;
         }
         message = message + " - Caused by NFC error " + this.mCode;
      }

      return message;
    }
}
