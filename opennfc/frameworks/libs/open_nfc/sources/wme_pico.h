/*
 * Copyright (c) 2007-2010 Inside Secure, All Rights Reserved.
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

#ifndef __WME_PICO_H
#define __WME_PICO_H

/*******************************************************************************
   Contains the declaration of the Picopass functions
*******************************************************************************/

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#define P_PICO_BLOCK_SIZE                    0x08

/**
 * @brief   Create the Picopass connection at level 15693-2 or 14443-3.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameter   The callback parameter.
 *
 * @param[in]  nProperty  The connection property.
 **/
void PPicoCreateConnection(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProperty );

/** See tPReaderUserRemoveSecondaryConnection */
void PPicoRemoveConnection(
            tContext* pContext,
            W_HANDLE hUserConnection );

/**
 * @brief   Checks if a card is a Type 5 Tag.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pnMaximumTagSize  The maximum tag size.
 *
 * @param[in]  pbIsLocked  The card is locked or not.
 *
 * @param[in]  pbIsLockable  The card is lockable or not.
 *
 * @param[in]  pSerialNumber  The card serial number.
 *
 * @param[in]  pnSerialNumberLength  The card serial number length.
 **/
W_ERROR PPicoCheckType5(
            tContext* pContext,
            W_HANDLE hConnection,
            uint32_t* pnMaximumTagSize,
            bool* pbIsLocked,
            bool* pbIsLockable,
            bool* pbIsFormattable,
            uint8_t* pSerialNumber,
            uint8_t* pnSerialNumberLength );


/**
 * @brief   Invalidates the cache associated to the current connection
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The connection handle.
 */
W_ERROR PPicoInvalidateCache(
            tContext* pContext,
            W_HANDLE hConnection,
            uint32_t nOffset,
            uint32_t nLength);


#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* __WME_PICO_H */
