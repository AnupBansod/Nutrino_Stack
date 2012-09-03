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

import org.opennfc.nfctag.NfcTagConnection;

/**
 * package private
 *
 * @hide
 **/
final class NfcTagConnectionImpl {}

/**
 * package private
 *
 * @hide
 **/
final class NfcType1ConnectionImpl extends TopazConnectionImpl implements NfcTagConnection {

    /* package private */NfcType1ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}

/**
 * package private
 *
 * @hide
 **/
final class NfcType2ConnectionImpl extends MifareULConnectionImpl implements NfcTagConnection {

    /* package private */NfcType2ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}

/**
 * package private
 *
 * @hide
 **/
final class NfcType3ConnectionImpl extends FeliCaConnectionImpl implements NfcTagConnection {

    /* package private */NfcType3ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}
/**
 * package private
 *
 * @hide
 **/
final class NfcType4AConnectionImpl extends Iso7816Part4AConnectionImpl implements NfcTagConnection {

    /* package private */NfcType4AConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}

/**
 * package private
 *
 * @hide
 **/
final class NfcType4BConnectionImpl extends Iso7816Part4BConnectionImpl implements NfcTagConnection {

    /* package private */NfcType4BConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}

/**
 * package private
 *
 * @hide
 **/
final class NfcType5ConnectionImpl extends PicopassConnectionImpl implements NfcTagConnection {

    /* package private */NfcType5ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}

/**
 * package private
 *
 * @hide
 **/
final class NfcType6ConnectionImpl extends Iso15693Part3ConnectionImpl implements NfcTagConnection {

    /* package private */NfcType6ConnectionImpl(SafeHandle connectionHandle) {
        super(connectionHandle);
    }
}
