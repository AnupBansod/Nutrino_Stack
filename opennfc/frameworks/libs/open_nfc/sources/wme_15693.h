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

#ifndef __WME_15693_H
#define __WME_15693_H

/*******************************************************************************
   Contains the declaration of the ISO 15693 functions
*******************************************************************************/

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/**
 * @brief   Creates the connection at ISO 15693 level.
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
void P15P3UserCreateConnection(
            tContext* pContext,
            W_HANDLE hUserConnection,
            W_HANDLE hDriverConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProtocol,
            const uint8_t* pBuffer,
            uint32_t nLength );

/**
 * @brief   Checks if a card is a Type 6 Tag.
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
W_ERROR P15P3UserCheckType6(
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
W_ERROR P15InvalidateCache(
            tContext* pContext,
            W_HANDLE hConnection,
            uint32_t nOffset,
            uint32_t nLength);


#if 0 /* @todo */
/***********  Keep for futur usage **************/

/* See tWBasicGenericDataCallbackFunction */
typedef void tP15P3UserExchangeDataCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            uint32_t nCardToReaderBufferLength,
            W_ERROR nError );
/**
 * @brief   Exchange data with the card at ISO 15693 level.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameterThe callback parameter.
 *
 * @param[in]  pReaderToCardBuffer  The command to the card.
 *
 * @param[in]  nReaderToCardBufferLength  The length of the command to the card.
 *
 * @param[in]  pCardToReaderBuffer  The answer from the card.
 *
 * @param[in]  nCardToReaderBufferMaxLength  The maximum length of the answer from the card.
 **/
void P15P3UserExchangeData(
            tContext* pContext,
            W_HANDLE hConnection,
            tP15P3UserExchangeDataCompleted *pCallback,
            void *pCallbackParameter,
            uint8_t *pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength,
            uint8_t *pCardToReaderBuffer,
            uint32_t nCardToReaderMaxBufferLength );

#endif /* #if 0 */


/**
 * @brief   Create the connection for ICODE, LRI and TAG IT chips
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
void P15P3UserCreateSecondaryConnection(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProperty );

/** See tPReaderUserRemoveSecondaryConnection */
void P15P3UserRemoveSecondaryConnection(
            tContext* pContext,
            W_HANDLE hUserConnection );



/*********** End Of Keep for futur usage **************/

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* __WME_15693_H */
