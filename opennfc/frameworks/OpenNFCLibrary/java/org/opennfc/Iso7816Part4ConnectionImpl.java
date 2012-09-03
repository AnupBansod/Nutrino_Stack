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

import org.opennfc.cardlistener.Iso7816Part4Connection;

/**
 * package private
 *
 * @hide
 **/
final class Iso7816Part4ConnectionImpl {}

/**
 * package private
 *
 * @hide
 **/
class Iso7816Part4AConnectionImpl extends Iso14443Part4AConnectionImpl implements Iso7816Part4Connection {

    /* package private */Iso7816Part4AConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}

/**
 * package private
 *
 * @hide
 **/
class Iso7816Part4BConnectionImpl extends Iso14443Part4BConnectionImpl implements Iso7816Part4Connection {

    /* package private */Iso7816Part4BConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}
