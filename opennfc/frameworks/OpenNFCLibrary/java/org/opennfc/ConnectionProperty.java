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
 * The connection properties are constant values describing the features of a connection.
 *
 * <p>
 * A connection property may be:
 * <ul>
 * <li>a protocol supported,</li>
 * <li>the format of the tag, or</li>
 * <li>the physical type of the tag.</li>
 * </ul></p>
 *
 * <p>
 * The card properties are used to register a card detection event handler with
 * {@link org.opennfc.cardlistener.CardListenerRegistry#registerCardListener CardListenerRegistry.registerCardListener()}.
 * When a card connection is received,
 * {@link org.opennfc.cardlistener.Connection#getProperties} or
 * {@link org.opennfc.cardlistener.Connection#checkProperty Connection.checkProperty()} returns or checks the properties of the card.</p>
 * <p>
 * The method {@link org.opennfc.cardlistener.CardListenerRegistry#checkConnectionProperty CardListenerRegistry.checkConnectionProperty()} checks if a property
 * is supported by the NFC Controller for the card detection functions.</p>
 * <p>
 * The connection properties are also used in the card emulation API and in the Virtual Tag API.
 * They are needed to specify the type of card emulation requested.
 * The method {@link org.opennfc.cardemulation.CardEmulationRegistry#checkConnectionProperty CardEmulationRegistry.checkConnectionProperty()}
 * checks if a property is supported by the NFC Controller for the card emulation functions.</p>
 *
 * @since Open NFC 4.0
 */
public enum ConnectionProperty {
    /** Communicate with protocol ISO 14443 part 3 type A. */
    ISO_14443_3_A ( ConstantAutogen.W_PROP_ISO_14443_3_A ),

    /** Communicate with protocol ISO 14443 part 4 type A. */
    ISO_14443_4_A ( ConstantAutogen.W_PROP_ISO_14443_4_A ),

    /** Communicate with protocol ISO 14443 part 3 type B. */
    ISO_14443_3_B ( ConstantAutogen.W_PROP_ISO_14443_3_B ),

    /** Communicate with protocol ISO 14443 part 4 type B */
    ISO_14443_4_B ( ConstantAutogen.W_PROP_ISO_14443_4_B ),

    /** Communicate with protocol ISO 15693 part 3. */
    ISO_15693_3 ( ConstantAutogen.W_PROP_ISO_15693_3 ),

    /** Communicate with protocol ISO 15693 part 2. */
    ISO_15693_2 ( ConstantAutogen.W_PROP_ISO_15693_2 ),

    /** Communicate with protocol ISO 7816 part 4. */
    ISO_7816_4 ( ConstantAutogen.W_PROP_ISO_7816_4 ),

    /** Communicate with protocol B Prime. */
    BPRIME ( ConstantAutogen.W_PROP_BPRIME ),
    
    /** Communicate with protocol My D Move */
    MY_D_MOVE ( ConstantAutogen.W_PROP_MY_D_MOVE ),
    /** Communicate with protocol My D NFC */
    MY_D_NFC ( ConstantAutogen.W_PROP_MY_D_NFC ),

    /** Identify a NFC Type 1 tag. */
    NFC_TAG_TYPE_1 ( ConstantAutogen.W_PROP_NFC_TAG_TYPE_1 ),

    /** Identify a NFC Type 2 tag. */
    NFC_TAG_TYPE_2 ( ConstantAutogen.W_PROP_NFC_TAG_TYPE_2 ),

    /** Identify a NFC Type 3 tag. */
    NFC_TAG_TYPE_3 ( ConstantAutogen.W_PROP_NFC_TAG_TYPE_3 ),

    /** Identify a NFC Type 4-A tag. */
    NFC_TAG_TYPE_4_A ( ConstantAutogen.W_PROP_NFC_TAG_TYPE_4_A ),

    /** Identify a NFC Type 4-B tag. */
    NFC_TAG_TYPE_4_B ( ConstantAutogen.W_PROP_NFC_TAG_TYPE_4_B ),

    /** Identify a NFC Type 5 tag. */
    NFC_TAG_TYPE_5 ( ConstantAutogen.W_PROP_NFC_TAG_TYPE_5 ),

    /** Identify a NFC Type 6 tag. */
    NFC_TAG_TYPE_6 ( ConstantAutogen.W_PROP_NFC_TAG_TYPE_6 ),

    /** Identify a Topaz 120 tag. May be formatted as a NFC Type 1 tag. */
    TYPE_TOPAZ_120 ( ConstantAutogen.W_PROP_TOPAZ ),

    /** Identify a Topaz 512 tag. May be formatted as a NFC Type 1 tag. */
    TYPE_TOPAZ_512 ( ConstantAutogen.W_PROP_TOPAZ_512 ),

    /** Identify a Jewel tag. */
    TYPE_JEWEL ( ConstantAutogen.W_PROP_JEWEL ),

    /** Identify a FeliCa card. May be formatted as a NFC Type 3 tag. */
    TYPE_FELICA ( ConstantAutogen.W_PROP_FELICA ),

    /** Identify a picopass 2K tag. */
    PICOPASS_2K ( ConstantAutogen.W_PROP_PICOPASS_2K ),

    /** Identify a picopass 32K tag. */
    PICOPASS_32K ( ConstantAutogen.W_PROP_PICOPASS_32K ),

    /** Identify a iClass 2K tag. */
    ICLASS_2K ( ConstantAutogen.W_PROP_ICLASS_2K ),

    /** Identify a iClass 16K tag. */
    ICLASS_16K ( ConstantAutogen.W_PROP_ICLASS_16K ),

    /** Identify a MIFARE UL tag. */
    MIFARE_UL ( ConstantAutogen.W_PROP_MIFARE_UL ),

    /** Identify a MIFARE 1K tag. */
    MIFARE_1K ( ConstantAutogen.W_PROP_MIFARE_1K ),

    /** Identify a MIFARE 4K tag. */
    MIFARE_4K ( ConstantAutogen.W_PROP_MIFARE_4K ),

    /** Identify a Texas Instruments Tag It. */
    TI_TAGIT ( ConstantAutogen.W_PROP_TI_TAGIT ),

    /** Identify a ST LRI 512 tag. */
    ST_LRI_512 ( ConstantAutogen.W_PROP_ST_LRI_512 ),

    /** Identify a ST LRI 2K tag. */
    ST_LRI_2K ( ConstantAutogen.W_PROP_ST_LRI_2K ),

    /** Identify a NXP ICode tag. */
    NXP_ICODE ( ConstantAutogen.W_PROP_NXP_ICODE ),

    /** Identify a MIFARE UL C tag */
    MIFARE_UL_C ( ConstantAutogen.W_PROP_MIFARE_UL_C ),

    /** Identify a MIFARE DESFire D40. */
    MIFARE_DESFIRE_D40 ( ConstantAutogen.W_PROP_MIFARE_DESFIRE_D40 ),

    /** Identify a MIFARE DESFire EV1 2K. */
    MIFARE_DESFIRE_EV1_2K ( ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_2K ),

    /** Identify a MIFARE DESFire EV1 4K. */
    MIFARE_DESFIRE_EV1_4K ( ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_4K ),

    /** Identify a MIFARE DESFire EV1 8K. */
    MIFARE_DESFIRE_EV1_8K ( ConstantAutogen.W_PROP_MIFARE_DESFIRE_EV1_8K ),

    /** Identify a MIFARE Plus X 2K. */
    MIFARE_PLUS_X_2K ( ConstantAutogen.W_PROP_MIFARE_PLUS_X_2K ),

    /** Identify a MIFARE Plus X 4K. */
    MIFARE_PLUS_X_4K ( ConstantAutogen.W_PROP_MIFARE_PLUS_X_4K ),

    /** Identify a MIFARE Plus S 2K. */
    MIFARE_PLUS_S_2K ( ConstantAutogen.W_PROP_MIFARE_PLUS_S_2K ),

    /** Identify a MIFARE Plus S 4K. */
    MIFARE_PLUS_S_4K ( ConstantAutogen.W_PROP_MIFARE_PLUS_S_4K ),

    /** Identify a MIFARE Mini tag. */
    MIFARE_MINI ( ConstantAutogen.W_PROP_MIFARE_MINI );

    /**
     * The identifier of the property
     *
     * @hide
     */
    private int identifier;

    /**
     * private constructor to prevent instantiation.
     *
     * @hide
     */
    private ConnectionProperty(int identifier) {
      this.identifier = identifier;
    }

    /**
     * Returns the property value.
     *
     * package private.
     *
     * @return the property value.
     *
     * @hide
     */
    int getValue() {
       return this.identifier;
    }
}
