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

#ifndef __WME_B_PRIME_H
#define __WME_B_PRIME_H

/* Default timeout value used for B PRIME          */
#define P_B_PRIME_DEFAULT_TIMEOUT          0x0B    /* 618 ms */

/**
 * Detection Configuration structure for B Prime
 */
/*typedef struct __tBPrimeDetectionConfiguration
{
} tBPrimeDetectionConfiguration;*/


#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/**
 * @brief   Create the connection at B-Prime level.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The user connection handle.
 *
 * @param[in]  hDriverConnection  The driver connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameterThe callback parameter.
 *
 * @param[in]  nProtocol  The protocol type.
 *
 * @param[in]  pBuffer  The buffer containing the activate result.
 *
 * @param[in]  nLength  The length of the buffer.
 **/
void PBPrimeUserCreateConnection(
            tContext* pContext,
            W_HANDLE hUserConnection,
            W_HANDLE hDriverConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProtocol,
            const uint8_t* pBuffer,
            uint32_t nLength );

/* Same as P14Part3ExchangeData()
   with an flag to acivate the CRC checking in the response */
void PBPrimeUserExchangeData(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericDataCallbackFunction* pCallback,
            void* pCallbackParameter,
            const uint8_t* pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength,
            uint8_t* pCardToReaderBuffer,
            uint32_t nCardToReaderBufferMaxLength,
            W_HANDLE* phOperation,
            bool bCheckResponseCRC );

W_ERROR PBPrimeGetConnectionInfo(
            tContext* pContext,
            W_HANDLE hConnection,
            tWBPrimeConnectionInfo* pBPrimeConnectionInfo );

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* ifndef */
/* EOF */
