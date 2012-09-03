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

import org.opennfc.cardemulation.CardEmulationRegistry;
import org.opennfc.cardemulation.CardEmulation;

/**
 * package private
 *
 * @hide
 **/
final class CardEmulationRegistryImpl implements CardEmulationRegistry {

    public CardEmulation createCardEmulation(ConnectionProperty cardType, byte[] identifier) {
        return new CardEmulationImpl(cardType, identifier);
    }

    public CardEmulation createCardEmulation(ConnectionProperty cardType, int randomIdentifierLength) {
        return new CardEmulationImpl(cardType, randomIdentifierLength);
    }

    public boolean checkConnectionProperty(ConnectionProperty property) {
        if (property == null) {
            throw new IllegalArgumentException("property = null");
        }

        return MethodAutogen.WEmulIsPropertySupported(property.getValue());
    }
}
