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
 * The specific constants of the priority values used with the NFC methods.
 *
 * @since Open NFC 4.0
 */
public final class NfcPriority {

    /* private constructor */
    private NfcPriority() {
    }

    /**
     * Exclusive access. When exclusivity is requested, the registration is
     * accepted only if no other client is already registered for this access.
     */
    public static final int EXCLUSIVE = ConstantAutogen.W_PRIORITY_EXCLUSIVE;

    /**
     * This value is used to restrict the access of the UICC to a protocol. This
     * priority level means "no access". It should not be used for the
     * registration methods.
     */
    public static final int NO_ACCESS = ConstantAutogen.W_PRIORITY_NO_ACCESS;

    /**
     * The minimum priority level for a shared access.
     */
    public static final int MINIMUM = ConstantAutogen.W_PRIORITY_MINIMUM;

    /**
     * The maximum priority level for a shared access.
     */
    public static final int MAXIMUM = ConstantAutogen.W_PRIORITY_MAXIMUM;
}
