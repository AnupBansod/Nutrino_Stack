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

#ifndef __WME_TYPE1_CHIP_H
#define __WME_TYPE1_CHIP_H

/*******************************************************************************
   Contains the declaration of the type1 functions
*******************************************************************************/

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/**
 * @brief   Create the Type 1 chip connection.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The user connection handle.
 *
 * @param[in]  hDriverConnection  The driver connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameter The callback parameter.
 *
 * @param[in]  nProtocol  The protocol property.
 *
 * @param[in]  pBuffer  The buffer containing the activate result.
 *
 * @param[in]  nLength  The length of the buffer.
 **/
void PType1ChipUserCreateConnection(
            tContext* pContext,
            W_HANDLE hUserConnection,
            W_HANDLE hDriverConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProtocol,
            const uint8_t* pBuffer,
            uint32_t nLength );

/**
 * @brief   Checks if a card is a Type 1 Tag.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The connection handle.
 *
 * @param[in]  pnMaximumTagSize  The maximum tag size.
 *
 * @param[in]  pnSectorSize  The sector size.
 *
 * @param[in]  pbIsLocked  The card is locked or not.
 *
 * @param[in]  pbIsLockable  The card is lockable or not.
 *
 * @param[in] pbIsFormattable The card is formattable or not
 *
 * @param[in]  pSerialNumber  The card serial number.
 *
 * @param[in]  pnSerialNumberLength  The card serial number length.
 *
 **/
W_ERROR PType1ChipUserCheckType1(
            tContext* pContext,
            W_HANDLE hUserConnection,
            uint32_t* pnMaximumTagSize,
            uint8_t *pnSectorSize,
            bool* pbIsLocked,
            bool* pbIsLockable,
            bool* pbIsFormattable,
            uint8_t* pSerialNumber,
            uint8_t* pnSerialNumberLength);

/**
 * @brief   Invalidates the cache associated to the current connection
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The connection handle.
 */
W_ERROR PType1ChipUserInvalidateCache(
            tContext* pContext,
            W_HANDLE  hUserConnection,
            uint32_t  nOffset,
            uint32_t  nLength);


/**
 * @brief   Create the connection for JEWEL, Topaz and Topaz 512
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameterThe callback parameter.
 *
 * @param[in]  nProtocol  The connection property.
 **/
void PType1UserCreateSecondaryConnection(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProperty );

/** See tPReaderUserRemoveSecondaryConnection */
void PType1UserRemoveSecondaryConnection(
            tContext* pContext,
            W_HANDLE hConnection );

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* __WME_TYPE1_CHIP_H */
