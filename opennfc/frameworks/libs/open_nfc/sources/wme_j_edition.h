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

#ifndef __WME_J_EDITION_H
#define __WME_J_EDITION_H

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#ifdef P_INCLUDE_J_EDITION_HELPER

/* The number of handles in each delta chunk */
#define PJED_HANDLE_LIST_DELTA  50

struct __tPJedHandleList;

typedef struct __tPJedHandleList
{
   W_HANDLE aHandleArray[PJED_HANDLE_LIST_DELTA];
   uint32_t aIsolateArray[PJED_HANDLE_LIST_DELTA];

   struct __tPJedHandleList* pNext;

} tPJedHandleList;

struct __tPJedRegistryEntry;

/* Type of the generic registry structure */
typedef struct __tPJedRegistryEntry
{
   tDFCCallbackContext sEventHandler;
   uint32_t nApplicationIdentifier;
   uint32_t nListenerId;
   uint32_t nRegistryType;
   uint8_t nTNF;
   const tchar* pTypeString;
   W_HANDLE hP2POpenOperation;

   struct __tPJedRegistryEntry* pNext;

} tPJedRegistryEntry;

typedef struct __tPJedP2PRegistryEntry
{
   tDFCCallbackContext sPushHandler;
   uint32_t nApplicationIdentifier;
   uint32_t nIsolate;
   uint32_t nP2PType;
   const tchar* pURIString;
   uint8_t nSAP;
   W_HANDLE hSocket;
   uint32_t nConnectionTimeReference;

   struct __tPJedP2PRegistryEntry* pNext;

} tPJedP2PRegistryEntry;

#define WJED_MAX_UICC_CONNECTIVITY_DATA_SIZE  255


/** Describes the J-Edition helper instance - Do not use directly */
typedef struct __tPJedInstance
{
   /* The event callback context */
   tDFCCallbackContext sCallbackContext;

   tPJedHandleList* pHandleListHead;

   tPJedRegistryEntry* pRegistryListHead;

   tPJedP2PRegistryEntry* pP2PRegistryListHead;

   W_HANDLE hRegistryListenNDEFMessage;
   W_HANDLE hRegistryListenUICCEOTMessage;
   W_HANDLE hRegistryListenSEEOTMessage;

   /* The keep alive connection timeout configuration */
   uint32_t nKeepAliveTimeoutConfiguration;

   /* The push registry timeout configuration */
   uint32_t nPushRegistryTimeoutConfiguration;

   /* The reader priority */
   uint8_t nReaderPriority;

   /* Current timer value */
   uint32_t nCurrentTimerValue;

   /* The keep alive connection */
   W_HANDLE hKeepAliveConnection;
   uint32_t nKeepAliveConnectionTimeReference;

   /* The last NDEF message received */
   W_HANDLE hLastNDEFMessage;
   uint32_t nLastNDEFMessageTimeReference;

   /* The length in bytes of the last connectivity message */
   uint32_t nLastConnectivityMessageLength;
   uint32_t nLastConnectivityMessageTimeReference;
   uint8_t aLastConnectivityData[WJED_MAX_UICC_CONNECTIVITY_DATA_SIZE];

   /* the P2P link establishment */
   W_HANDLE hLinkOperation;
   W_HANDLE hLink;

} tPJedInstance;

/**
 * @brief Creates a J-Edition helper instance.
 *
 * @param[out]  pWJedInstance  The J-Edition helper instance to initialize.
 **/
void PJedCreate(
         tPJedInstance* pWJedInstance);

/**
 * @brief Destroyes a J-Edition helper instance.
 *
 * @param[in]  pWJedInstance  The J-Edition helper instance to destroy.
 **/
void PJedDestroy(
         tPJedInstance* pWJedInstance );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* __WME_J_EDITION_H */
