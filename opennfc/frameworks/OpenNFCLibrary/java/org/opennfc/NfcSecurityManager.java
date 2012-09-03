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
 * The NFC Manager class is the single-instance class used to access the Open
 * NFC library.
 *
 * @since Open NFC 4.0
 **/
/* package private */ final class NfcSecurityManager {

    /* package private */ static final String NFC = "NFC";
    /* package private */ static final String CARD_EMUL = "CARD_EMUL";
    /* package private */ static final String CARD_LISTEN = "CARD_LISTEN";
    /* package private */ static final String P2P = "P2P";
    /* package private */ static final String HARDWARE = "HARDWARE";

    /* Private constructor */
    private NfcSecurityManager() {
    }

    /**
     *  Initializes the Security Manager, used for permission checks
	  *  for some custom implementation.
     *
     * @param object The security manager "context", used for checking
     */
    /* package private */  static void setContext(Object object) {

		 /* no "context" in this implementation */
    }

    /**
     * Checks if the requested security level is granted to the application.
     *
     * @throws SecurityException in case of security exception.
     */
    /* package private */ static void check(String token) throws SecurityException {

       if(token == NFC) {
         /* No security exception in this implementation */
       } else if(token == CARD_EMUL) {
         /* No security exception in this implementation */
       } else if(token == CARD_LISTEN) {
         /* No security exception in this implementation */
       } else if(token == P2P) {
         /* No security exception in this implementation */
       } else if(token == HARDWARE) {
         /* No security exception in this implementation */
       }
    }
}
