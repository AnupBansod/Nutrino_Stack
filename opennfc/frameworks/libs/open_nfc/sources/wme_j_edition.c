/*
 * Copyright (c) 2009-2010 Inside Secure, All Rights Reserved.
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

#define P_MODULE  P_MODULE_DEC( WJED )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#ifdef P_INCLUDE_J_EDITION_HELPER

/* Registry type */
#define WJED_TYPE_LISTEN_NDEF_MESSAGE                0x00
#define WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE         0x01
#define WJED_TYPE_PUSH_REGISTRY_CONNECTIVITY_MESSAGE 0x02
#define WJED_TYPE_LISTEN_UICC_EOT_MESSAGE            0x03
#define WJED_TYPE_LISTEN_SE_EOT_MESSAGE              0x04
#define WJED_TYPE_P2P_REGISTRY                       0x05

/* Forward declarations */
static void static_PJedTimeoutElapsed(
               tContext* pContext,
               void* pCallbackParameter);

static void static_PJedNDEFMessageHandler(
               tContext* pContext,
               void* pHandlerParameter,
               W_HANDLE hMessage,
               W_ERROR nError );

static void static_PEstablishmentCallback( tContext* pContext,
               void* pCallbackParameter,
               W_HANDLE hLink,
               W_ERROR nError );

static void static_PReleaseCallback( tContext* pContext,
               void * pCallbackParameter,
               W_ERROR nResult );

void static static_PShutdownCompleted(tContext * pContext,
               void * pCallbackParameter,
               W_ERROR nError);

/* See header file */
void PJedCreate(
         tPJedInstance* pWJedInstance)
{
   CMemoryFill(pWJedInstance, 0, sizeof(tPJedInstance));
}

/* See header file */
void PJedDestroy(
         tPJedInstance* pWJedInstance )
{
   CMemoryFill(pWJedInstance, 0, sizeof(tPJedInstance));
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedInitialize(
         tContext* pContext,
         uint32_t nKeepAliveConnectionTimeout,
         uint32_t nPushRegistryTimeout,
         uint8_t nReaderPriority,
         tPJedSendEvent* pSendEventFunction,
         void* pCallbackParameter )
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pSendEventFunction,
      pCallbackParameter,
      &pInstance->sCallbackContext );

   pInstance->pHandleListHead = (tPJedHandleList*)CMemoryAlloc(sizeof(tPJedHandleList));
   if(pInstance->pHandleListHead != null)
   {
      CMemoryFill(pInstance->pHandleListHead, 0, sizeof(tPJedHandleList));
   }
   else
   {
      PDebugError("PJedInitialize: cannot allocate the handle list");
      return W_ERROR_OUT_OF_RESOURCE;
   }

   pInstance->nKeepAliveTimeoutConfiguration = nKeepAliveConnectionTimeout;
   pInstance->nPushRegistryTimeoutConfiguration = nPushRegistryTimeout;
   pInstance->nReaderPriority = nReaderPriority;

   pInstance->hKeepAliveConnection = W_NULL_HANDLE;

   return W_SUCCESS;
}

/**
 * Destroyes a registry entry.
 *
 * @param[in]  pEntry  The entry to destroy. May be null.
 **/
static void static_PJedDestroyRegistryEntry(
               tContext* pContext,
               tPJedRegistryEntry* pEntry )
{
   if(pEntry != null)
   {
      CMemoryFree((void*)pEntry->pTypeString);
      CMemoryFree(pEntry);
   }
}

/**
 * Destroyes a P2P registry entry.
 *
 * @param[in]  pP2PEntry  The entry to destroy. May be null.
 **/
static void static_PJedDestroyP2PRegistryEntry(
               tContext* pContext,
               tPJedP2PRegistryEntry* pP2PEntry )
{
   PDebugTrace("static_PJedDestroyP2PRegistryEntry()");

   if(pP2PEntry != null)
   {
      tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
      PDebugWarning("static_PJedDestroyP2PRegistryEntry(): close handles");
      if(pInstance->hLinkOperation != W_NULL_HANDLE) {
         PDebugWarning("static_PJedDestroyP2PRegistryEntry(): cancel link operation");
         PBasicCancelOperation(pContext, pInstance->hLinkOperation);
      }
      if(pInstance->hLink != W_NULL_HANDLE) {
         PDebugWarning("static_PJedDestroyP2PRegistryEntry(): close link handle");
         PHandleClose(pContext, pInstance->hLink);
      }
      PHandleClose(pContext, pP2PEntry->hSocket);
      CMemoryFree((void*)pP2PEntry->pURIString);
      CMemoryFree(pP2PEntry);
   }
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedAddHandle(
            tContext* pContext,
            W_HANDLE hHandle,
            uint32_t nIsolate)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedHandleList* pList = pInstance->pHandleListHead;

   PDebugTrace("PJedAddHandle: 0x%08X", hHandle);

   if(hHandle == W_NULL_HANDLE)
   {
      return W_SUCCESS;
   }

   if(pList == null)
   {
      pList = (tPJedHandleList*)CMemoryAlloc(sizeof(tPJedHandleList));
      if(pList != null)
      {
         CMemoryFill(pList, 0, sizeof(tPJedHandleList));
         pInstance->pHandleListHead = pList;
      }
      else
      {
         PDebugError("PJedAddHandle: cannot allocate the handle list");
         return W_ERROR_OUT_OF_RESOURCE;
      }
   }

   for(;;)
   {
      uint32_t nIndex;

      for(nIndex = 0; nIndex < PJED_HANDLE_LIST_DELTA; nIndex++)
      {
         if(pList->aHandleArray[nIndex] == W_NULL_HANDLE)
         {
            pList->aHandleArray[nIndex] = hHandle;
            pList->aIsolateArray[nIndex] = nIsolate;
            return W_SUCCESS;
         }
      }

      if(pList->pNext == null)
      {
         break;
      }
      else
      {
         pList = pList->pNext;
      }
   }

   pList->pNext = (tPJedHandleList*)CMemoryAlloc(sizeof(tPJedHandleList));
   pList = pList->pNext;
   if(pList != null)
   {
      CMemoryFill(pList, 0, sizeof(tPJedHandleList));
   }
   else
   {
      PDebugError("PJedAddHandle: cannot allocate the handle list");
      return W_ERROR_OUT_OF_RESOURCE;
   }

   pList->aHandleArray[0] = hHandle;
   pList->aIsolateArray[0] = nIsolate;

   return W_SUCCESS;
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedCheckHandle(
               tContext* pContext,
               W_HANDLE hHandle,
               uint32_t* pnIsolate)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedHandleList* pList = pInstance->pHandleListHead;

   PDebugTrace("PJedCheckHandle: 0x%08X", hHandle);

   if(pnIsolate != null)
   {
      *pnIsolate = 0;
   }

   if(hHandle == W_NULL_HANDLE)
   {
      return W_ERROR_BAD_HANDLE;
   }

   if(pList == null)
   {
      PDebugError("PJedCheckHandle: handle not found");
      return W_ERROR_BAD_HANDLE;
   }

   for(;;)
   {
      uint32_t nIndex;

      for(nIndex = 0; nIndex < PJED_HANDLE_LIST_DELTA; nIndex++)
      {
         if(pList->aHandleArray[nIndex] == hHandle)
         {
            if(pnIsolate != null)
            {
               *pnIsolate = pList->aIsolateArray[nIndex];
            }
            return W_SUCCESS;
         }
      }

      if(pList->pNext == null)
      {
         break;
      }
      else
      {
         pList = pList->pNext;
      }
   }

   PDebugError("PJedCheckHandle: handle not found");
   return W_ERROR_BAD_HANDLE;
}

/**
 * Update the timer.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pInstance  The current instance.
 */
static void static_PJedTimerUpdate(
               tContext* pContext,
               tPJedInstance* pInstance)
{
   uint32_t nMinimalAsoluteTime = 0xFFFFFFFF;
   tPJedP2PRegistryEntry* pP2PEntry;

   if(pInstance->nKeepAliveConnectionTimeReference != 0)
   {
      if(pInstance->nKeepAliveConnectionTimeReference < nMinimalAsoluteTime)
      {
         nMinimalAsoluteTime = pInstance->nKeepAliveConnectionTimeReference;
      }
   }

   if(pInstance->nLastNDEFMessageTimeReference != 0)
   {
      if(pInstance->nLastNDEFMessageTimeReference < nMinimalAsoluteTime)
      {
         nMinimalAsoluteTime = pInstance->nLastNDEFMessageTimeReference;
      }
   }

   if(pInstance->nLastConnectivityMessageTimeReference != 0)
   {
      if(pInstance->nLastConnectivityMessageTimeReference < nMinimalAsoluteTime)
      {
         nMinimalAsoluteTime = pInstance->nLastConnectivityMessageTimeReference;
      }
   }

   for(pP2PEntry = pInstance->pP2PRegistryListHead;
       pP2PEntry != null;
       pP2PEntry = pP2PEntry->pNext)
   {
      if(pP2PEntry->nConnectionTimeReference != 0)
      {
         if(pP2PEntry->nConnectionTimeReference < nMinimalAsoluteTime)
         {
            nMinimalAsoluteTime = pP2PEntry->nConnectionTimeReference;
         }
      }
   }

   if(nMinimalAsoluteTime == 0xFFFFFFFF)
   {
      if(pInstance->nCurrentTimerValue != 0)
      {
         pInstance->nCurrentTimerValue = 0;
         PMultiTimerCancelDriver(pContext, TIMER_T12_USER_WJED);
         /* @todo what can we do here if the IOCTL call failed */
      }
   }
   else
   {
      if(pInstance->nCurrentTimerValue != nMinimalAsoluteTime)
      {
         pInstance->nCurrentTimerValue = nMinimalAsoluteTime;
         PMultiTimerSetDriver(pContext, TIMER_T12_USER_WJED,
                  nMinimalAsoluteTime,
                  static_PJedTimeoutElapsed, pInstance );
         /* @todo what can we do here if the IOCTL call failed */
      }
   }
}

/**
 * Removes and destroyes a P2P registry.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pInstance  The current instance.
 *
 * @param[in]  pP2PEntry  The P2P entry to remove.
 **/
static void static_PJedRemoveAndDestroyP2PRegistryEntry(
               tContext* pContext,
               tPJedInstance* pInstance,
               tPJedP2PRegistryEntry* pP2PEntry)
{
   tPJedP2PRegistryEntry** ppPrevP2PEntry;

   PDebugTrace("static_PJedRemoveAndDestroyP2PRegistryEntry()");

   for(ppPrevP2PEntry = &pInstance->pP2PRegistryListHead;
       *ppPrevP2PEntry != pP2PEntry;
       ppPrevP2PEntry = &((*ppPrevP2PEntry)->pNext)) {}

   *ppPrevP2PEntry = pP2PEntry->pNext;

   static_PJedDestroyP2PRegistryEntry(pContext, pP2PEntry);
}

/**
 * Removes a handle from the handle list.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pInstance  The current instance.
 *
 * @param[in]  hHandle  The handle to remove.
 **/
static void static_PJedRemoveHandle(
               tContext* pContext,
               tPJedInstance* pInstance,
               W_HANDLE hHandle)
{
   tPJedHandleList** ppList;

   for(ppList = &(pInstance->pHandleListHead); *ppList != null; )
   {
      uint32_t nIndex;
      uint32_t nEmptyCount = 0;
      tPJedHandleList* pList = *ppList;

      for(nIndex = 0; nIndex < PJED_HANDLE_LIST_DELTA; nIndex++)
      {
         if(pList->aHandleArray[nIndex] == hHandle)
         {
            pList->aHandleArray[nIndex] = W_NULL_HANDLE;
            /* continue */
         }

         if(pList->aHandleArray[nIndex] == W_NULL_HANDLE)
         {
            nEmptyCount++;
         }
      }

      if(nEmptyCount == PJED_HANDLE_LIST_DELTA)
      {
         *ppList = pList->pNext;
         CMemoryFree(pList);
      }
      else
      {
         ppList = &(pList->pNext);
      }
   }
}

/**
 * Closes a handle.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pInstance  The current instance.
 *
 * @param[in]  hHandle  The handle to close.
 *
 * @param[in]  bGiveToNextListener The polling flag for a connection.
 *
 * @param[in]  bCardApplicationMatch The card match flag for a connection.
 *
 * @param[in]  bRemoveHandle  Flag to remove handle.
 **/
static void static_PJedCloseHandle(
               tContext* pContext,
               tPJedInstance* pInstance,
               W_HANDLE hHandle,
               bool bGiveToNextListener,
               bool bCardApplicationMatch,
               bool bRemoveHandle)
{
   uint32_t nPropertyNumber = 0;

   PDebugTrace("static_PJedCloseHandle(handle = 0x%08X, bGiveToNextListener=%d, bCardApplicationMatch=%d, bRemoveHandle=%d)",
      hHandle, bGiveToNextListener, bCardApplicationMatch, bRemoveHandle);

   if(hHandle == W_NULL_HANDLE)
   {
      return;
   }

   if(pInstance->hKeepAliveConnection == hHandle)
   {
      PDebugTrace("static_PJedCloseHandle: Closing the keep alive connection");

      /* Reset the keep alive connection */
      pInstance->hKeepAliveConnection = W_NULL_HANDLE;
      pInstance->nKeepAliveConnectionTimeReference = 0;
      static_PJedTimerUpdate( pContext, pInstance);

      PReaderHandlerWorkPerformed(pContext, hHandle, bGiveToNextListener, bCardApplicationMatch);

      nPropertyNumber = 1;
   }
   else if(PBasicGetConnectionPropertyNumber(pContext, hHandle, &nPropertyNumber) == W_SUCCESS)
   {
      if(PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_VIRTUAL_TAG) == W_SUCCESS)
      {
         nPropertyNumber = 0;
      }

      if(nPropertyNumber != 0)
      {
         tPJedP2PRegistryEntry* pP2PEntry = pInstance->pP2PRegistryListHead;
         while(pP2PEntry != null)
         {
            if(pP2PEntry->hSocket == hHandle)
            {
               PHandleClose(pContext, hHandle);
               pP2PEntry->hSocket = W_NULL_HANDLE;
               if(pP2PEntry->nConnectionTimeReference != 0)
               {
                  pP2PEntry->nConnectionTimeReference = 0;
                  static_PJedTimerUpdate( pContext, pInstance);
               }
               break;
            }
            pP2PEntry = pP2PEntry->pNext;
         }

         /* If the connection is not a P2P connection */
         if(pP2PEntry == null)
         {
            PDebugTrace("static_PJedCloseHandle: not a P2P connection, calling PReaderHandlerWorkPerformed()");
            PReaderHandlerWorkPerformed(pContext, hHandle, bGiveToNextListener, bCardApplicationMatch);
         }
      }
   }

   if(nPropertyNumber == 0)
   {
      if(hHandle == pInstance->hLastNDEFMessage)
      {
         PDebugTrace("static_PJedCloseHandle: last NDEF message destroyed");
         pInstance->hLastNDEFMessage = W_NULL_HANDLE;
         pInstance->nLastNDEFMessageTimeReference = 0;
         static_PJedTimerUpdate(pContext, pInstance);
      }
      PHandleClose(pContext, hHandle);
   }

   if(bRemoveHandle != false)
   {
      static_PJedRemoveHandle(pContext, pInstance, hHandle);
   }
}

/**
 * Closes the handles from the handle list.
 *
 * @param[in] pContext  The current context.
 *
 * @param[in] nIsolate  The isolate of the handle to remove.
 **/
static void static_PJedCloseHandleByIsolate(
            tContext* pContext,
            tPJedInstance* pInstance,
            uint32_t nIsolate)
{
   tPJedHandleList* pList = pInstance->pHandleListHead;
   W_HANDLE hHandle;

   PDebugTrace("static_PJedCloseHandleByIsolate: 0x%08X", nIsolate);

   if(pList == null)
   {
      PDebugTrace("static_PJedCloseHandleByIsolate: no handle list allocated");
      return;
   }

   for(;;)
   {
      uint32_t nIndex;

      for(nIndex = 0; nIndex < PJED_HANDLE_LIST_DELTA; nIndex++)
      {
         hHandle = pList->aHandleArray[nIndex];
         if(hHandle != W_NULL_HANDLE)
         {
            if(pList->aIsolateArray[nIndex] == nIsolate)
            {
               pList->aHandleArray[nIndex] = W_NULL_HANDLE;

               static_PJedCloseHandle(
                     pContext, pInstance, hHandle, true, false, false);
            }
         }
      }

      if(pList->pNext == null)
      {
         break;
      }
      else
      {
         pList = pList->pNext;
      }
   }
}

/* See the porting guide of Open NFC J-Edition  */
void PJedTerminate(
         tContext* pContext)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedHandleList* pList;
   tPJedRegistryEntry* pEntry;
   tPJedRegistryEntry* pNextEntry;
   tPJedP2PRegistryEntry* pP2PEntry;
   tPJedP2PRegistryEntry* pNextP2PEntry;

   pList = pInstance->pHandleListHead;
   pInstance->pHandleListHead = null;

   /* Remove the remaining handles */
   while(pList != null)
   {
      tPJedHandleList* pNextList = pList->pNext;
      uint32_t nIndex;

      for(nIndex = 0; nIndex < PJED_HANDLE_LIST_DELTA; nIndex++)
      {
         if(pList->aHandleArray[nIndex] != W_NULL_HANDLE)
         {
            PDebugWarning("PJedTerminate: Handle not closed");
            PHandleClose(pContext, pList->aHandleArray[nIndex]);
         }
      }

      CMemoryFree(pList);

      pList = pNextList;
   }

   /* Destroy the registry list */
   pEntry = pInstance->pRegistryListHead;
   pInstance->pRegistryListHead = null;
   while(pEntry != null)
   {
      pNextEntry = pEntry->pNext;
      static_PJedDestroyRegistryEntry(pContext, pEntry);
      pEntry = pNextEntry;
   }

   /* Destroy the P2P registry list */
   pP2PEntry = pInstance->pP2PRegistryListHead;
   pInstance->pP2PRegistryListHead = null;
   while(pP2PEntry != null)
   {
      pNextP2PEntry = pP2PEntry->pNext;
      static_PJedDestroyP2PRegistryEntry(pContext, pP2PEntry);
      pP2PEntry = pNextP2PEntry;
   }

   if(pInstance->hRegistryListenUICCEOTMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pInstance->hRegistryListenUICCEOTMessage);
      pInstance->hRegistryListenUICCEOTMessage = W_NULL_HANDLE;
   }

   if(pInstance->hRegistryListenSEEOTMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pInstance->hRegistryListenSEEOTMessage);
      pInstance->hRegistryListenSEEOTMessage = W_NULL_HANDLE;
   }

   if(pInstance->hRegistryListenNDEFMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pInstance->hRegistryListenNDEFMessage);
      pInstance->hRegistryListenNDEFMessage = W_NULL_HANDLE;
   }

   if(pInstance->hLastNDEFMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pInstance->hLastNDEFMessage);
      pInstance->hLastNDEFMessage = W_NULL_HANDLE;
      pInstance->nLastNDEFMessageTimeReference = 0;
   }

   pInstance->nLastConnectivityMessageLength = 0;
   pInstance->nLastConnectivityMessageTimeReference = 0;

   /* reset the keep alive connection */
   static_PJedCloseHandle(
      pContext, pInstance, pInstance->hKeepAliveConnection, true, false, true);

   static_PJedTimerUpdate(pContext, pInstance);
}

/* List of the callback identifiers */
#define WJED_EVENT_ID_LISTEN_TO_CARD_DETECTION  0x01
#define WJED_EVENT_ID_LISTEN_TO_TRANSACTION  0x02
#define WJED_EVENT_ID_LISTEN_TO_NDEF_RECORD 0x03

/* The structure associating a name and a value */
typedef const struct __tPJedNameValue
{
   uint8_t nValue;
   const char* pName;
} tPJedNameValue;

#define P_VALUE_STRING(X) \
   { X, #X },

#define PJED_SPECIAL_MODE_CARD      0xFD
#define PJED_SPECIAL_MODE_READER    0xFE

/* The NFCC property array */
static tPJedNameValue g_aPJedNFCCPropertyArray[] = {
   P_VALUE_STRING(W_NFCC_PROP_LIBRARY_VERSION)
   P_VALUE_STRING(W_NFCC_PROP_LIBRARY_IMPL)
   P_VALUE_STRING(W_NFCC_PROP_LOADER_VERSION)
   P_VALUE_STRING(W_NFCC_PROP_FIRMWARE_VERSION)
   P_VALUE_STRING(W_NFCC_PROP_HARDWARE_VERSION)
   P_VALUE_STRING(W_NFCC_PROP_HARDWARE_SN)

   P_VALUE_STRING(W_NFCC_PROP_NFC_HAL_VERSION)
   P_VALUE_STRING(W_NFCC_PROP_SE_NUMBER)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_A_MAX_RATE)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_A_INPUT_SIZE)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_B_MAX_RATE)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_B_INPUT_SIZE)
   P_VALUE_STRING(W_NFCC_PROP_CARD_ISO_14443_A_MAX_RATE)
   P_VALUE_STRING(W_NFCC_PROP_CARD_ISO_14443_B_MAX_RATE)

   P_VALUE_STRING(W_NFCC_PROP_STANDBY_TIMEOUT)
   P_VALUE_STRING(W_NFCC_PROP_BATTERY_LOW_SUPPORTED)
   P_VALUE_STRING(W_NFCC_PROP_BATTERY_OFF_SUPPORTED)
   P_VALUE_STRING(W_NFCC_PROP_STANDBY_SUPPORTED)
   P_VALUE_STRING(W_NFCC_PROP_CARD_ISO_14443_A_CID)
   P_VALUE_STRING(W_NFCC_PROP_CARD_ISO_14443_A_NAD)
   P_VALUE_STRING(W_NFCC_PROP_CARD_ISO_14443_B_CID)
   P_VALUE_STRING(W_NFCC_PROP_CARD_ISO_14443_B_NAD)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_A_CID)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_A_NAD)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_B_CID)
   P_VALUE_STRING(W_NFCC_PROP_READER_ISO_14443_B_NAD)

   P_VALUE_STRING(W_NFCC_PROP_P2P)

   /* Special values */
   { PJED_SPECIAL_MODE_READER, "mode.reader" },
   { PJED_SPECIAL_MODE_CARD, "mode.card" },
};

#undef P_VALUE_STRING

/**
 * @brief Returns the NFCC property code corresponding to the property name.
 *
 * @param[in]  pPropertyName  The property name.
 *
 * @param[in]  nNameLength  The length of the name in characters.
 *
 * @return  The property code or zero if the property is not found.
 **/
static uint8_t static_PJedGetNFCCPropertyByName(
               const tchar* pPropertyName,
               uint32_t nNameLength )
{
   uint32_t nIndex;

   if(pPropertyName != null)
   {
      for(nIndex = 0;
         nIndex < (sizeof(g_aPJedNFCCPropertyArray)/sizeof(tPJedNameValue));
         nIndex++)
      {
         if(PUtilMixedStringCompare(
            pPropertyName, nNameLength,
            g_aPJedNFCCPropertyArray[nIndex].pName) == 0)
         {
            return g_aPJedNFCCPropertyArray[nIndex].nValue;
         }
      }
   }

   PDebugError("static_PJedGetNFCCPropertyByName: property not found");

   return 0;
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedGetNFCCProperty(
               tContext* pContext,
               const tchar* pPropertyName,
               tchar* pPropertyValueBuffer,
               uint32_t nPropertyValueBufferLength,
               uint32_t* pnPropertyValueLength)
{
   uint32_t nNameLength = PUtilStringLength(pPropertyName);
   uint8_t nPropertyIdentifier = static_PJedGetNFCCPropertyByName(
      pPropertyName, nNameLength);

   if(nPropertyIdentifier == 0)
   {
      PDebugError("PJedGetNFCCProperty: property not found");
      return W_ERROR_ITEM_NOT_FOUND;
   }

   if((nPropertyIdentifier == PJED_SPECIAL_MODE_READER)
   || (nPropertyIdentifier == PJED_SPECIAL_MODE_CARD))
   {
      bool bReaderLock, bCardLock, bResult;

      PNFCControllerGetRFLock(pContext, W_NFCC_STORAGE_VOLATILE,
         &bReaderLock, &bCardLock);

      if(nPropertyIdentifier == PJED_SPECIAL_MODE_READER)
      {
         bResult = bReaderLock;
      }
      else
      {
         bResult = bCardLock;
      }

      if(bResult == false)
      {
         if(nPropertyValueBufferLength < 3)
         {
            PDebugError("PJedGetNFCCProperty: buffer too short");
            return W_ERROR_BUFFER_TOO_SHORT;
         }

         pPropertyValueBuffer[0] = (tchar)'o';
         pPropertyValueBuffer[1] = (tchar)'n';
         pPropertyValueBuffer[2] = (tchar)0;
         *pnPropertyValueLength = 2;
      }
      else
      {
         if(nPropertyValueBufferLength < 4)
         {
            PDebugError("PJedGetNFCCProperty: buffer too short");
            return W_ERROR_BUFFER_TOO_SHORT;
         }

         pPropertyValueBuffer[0] = (tchar)'o';
         pPropertyValueBuffer[1] = (tchar)'f';
         pPropertyValueBuffer[2] = (tchar)'f';
         pPropertyValueBuffer[3] = (tchar)0;
         *pnPropertyValueLength = 3;
      }

      return W_SUCCESS;
   }

   return PNFCControllerInternalGetProperty(
         pContext,
         nPropertyIdentifier,
         pPropertyValueBuffer,
         nPropertyValueBufferLength,
         pnPropertyValueLength );
}

/** See tWBasicGenericCallbackFunction */
static void static_PConnectCallback(
               tContext* pContext,
               void * pCallbackParameter,
               W_ERROR nResult )
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedP2PRegistryEntry* pP2PEntry = (tPJedP2PRegistryEntry*)pCallbackParameter;

   PDebugTrace("static_PConnectCallback(): nResult = %s ", PUtilTraceError(nResult));

   if (nResult == W_SUCCESS)
   {
      /* Setup the timeout for the push connection */
      pP2PEntry->nConnectionTimeReference =
         PNALServiceDriverGetCurrentTime(pContext) + pInstance->nPushRegistryTimeoutConfiguration;
      /* @todo what can we do here if the driver call failed */
      static_PJedTimerUpdate(pContext, pInstance);

      /* Call the push registry callback */
      PDFCPostContext2(&pP2PEntry->sPushHandler, P_DFC_TYPE_WJED,
         pP2PEntry->nApplicationIdentifier );
   }
   else
   {
      PDebugWarning("static_PConnectCallback(): connection FAILED nothing to do...");
   }
}

/** See tWBasicGenericHandleCallbackFunction */
static void static_PEstablishmentCallback(
               tContext* pContext,
               void* pCallbackParameter,
               W_HANDLE hLink,
               W_ERROR nError )
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedP2PRegistryEntry* pP2PEntry = (tPJedP2PRegistryEntry*)pCallbackParameter;
   tPJedP2PRegistryEntry* pFoundP2PEntry;

   PDebugTrace("static_PEstablishmentCallback(): nError = %s ", PUtilTraceError(nError));

   /* Check if the entry is still present */
   CDebugAssert(pP2PEntry != null);
   pFoundP2PEntry = pInstance->pP2PRegistryListHead;
   if (pFoundP2PEntry != null)
   {
      while(pFoundP2PEntry != pP2PEntry)
      {
         pFoundP2PEntry = pFoundP2PEntry->pNext;
      }
   }

   if(pFoundP2PEntry != null)
   {
      /* Close the establish link() operation handle */
      if(pInstance->hLinkOperation != W_NULL_HANDLE)
      {
         PDebugWarning("static_PEstablishmentCallback(): Close the link operation handle = %d", pInstance->hLinkOperation);
         PHandleClose(pContext, pInstance->hLinkOperation);
         pInstance->hLinkOperation = W_NULL_HANDLE;
      }
   }

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PEstablishmentCallback: returns %s", PUtilTraceError(nError));
      CDebugAssert(hLink == W_NULL_HANDLE);

      if(pFoundP2PEntry != null)
      {
         /* Ignore the Push callback in case of error */
         PDebugWarning("static_PEstablishmentCallback(): Ignore the Push callback in case of error");
      }
      else
      {
         PDebugError("static_PEstablishmentCallback(): No P2P Entry found");
      }
      return;
   }

   /* If the entry is not in the registry, ignore the connection */
   if(pFoundP2PEntry == null)
   {
      PDebugWarning("static_PEstablishmentCallback: No registry found, close link");
      PHandleClose(pContext, hLink);
      return;
   }

   /* Store the link */
   pInstance->hLink = hLink;

   /* Setup the push timeout */
   pP2PEntry->nConnectionTimeReference = 0;
   static_PJedTimerUpdate(pContext, pInstance);

   /* Request connection establishment */
   PDebugTrace("static_PEstablishmentCallback: Request connection establishment. hSocket = %d | hLink = %d", pP2PEntry->hSocket, hLink);
   PP2PConnectDriver(pContext, pP2PEntry->hSocket, hLink, static_PConnectCallback, pP2PEntry);

   nError = PContextGetLastIoctlError(pContext);
   if (nError != W_SUCCESS)
   {
      PDFCPost2(pContext, P_DFC_TYPE_WJED, static_PConnectCallback, pP2PEntry, nError);
   }
}

/** See tWBasicGenericCallbackFunction */
static void static_PReleaseCallback(
               tContext* pContext,
               void * pCallbackParameter,
               W_ERROR nResult )
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedP2PRegistryEntry* pP2PEntry = (tPJedP2PRegistryEntry*)pCallbackParameter;
   tPJedP2PRegistryEntry* pFoundP2PEntry;

   PDebugTrace("static_PReleaseCallback(): nResult = %s", PUtilTraceError(nResult));

   /* Close the establish link() handle */
   if(pInstance->hLink != W_NULL_HANDLE)
   {
      PDebugWarning("static_PEstablishmentCallback(): Close the establish link() handle: pInstance->hLink = %d", pInstance->hLink);
      PHandleClose(pContext, pInstance->hLink);
      pInstance->hLink = W_NULL_HANDLE;
   }

   /* Check if the entry is still present */
   CDebugAssert(pP2PEntry != null);
   pFoundP2PEntry = pInstance->pP2PRegistryListHead;
   while(pFoundP2PEntry != pP2PEntry)
   {
      pFoundP2PEntry = pFoundP2PEntry->pNext;
   }

   if(pFoundP2PEntry != null)
   {
      pFoundP2PEntry->nConnectionTimeReference = 0;
      static_PJedTimerUpdate(pContext, pInstance);

      /* Restart the establishment */
      PDebugTrace("static_PReleaseCallback(): Restart the establishment link");
      PP2PEstablishLinkWrapper(  pContext,
                                 static_PEstablishmentCallback, pP2PEntry,
                                 static_PReleaseCallback, pP2PEntry,
                                 &pInstance->hLinkOperation);
      PDebugTrace("static_PReleaseCallback(): hLinkOperation = %d = 0x%x",
         pInstance->hLinkOperation, pInstance->hLinkOperation);

   }
   else
   {
      PDebugError("static_PReleaseCallback(): No P2P entry");
   }
}

static void static_PShutdownCompleted(
               tContext * pContext,
               void * pCallbackParameter,
               W_ERROR nError)
{
   PDebugTrace("static_PShutdownCompleted(): nError= %s", PUtilTraceError(nError));
   /* @todo There are something to do here ? */
}

static void static_PJedTimeoutElapsed(
               tContext* pContext,
               void* pCallbackParameter)
{
   tPJedInstance* pInstance = (tPJedInstance*)pCallbackParameter;
   uint32_t nCurrentTime = PNALServiceDriverGetCurrentTime(pContext);
   /* @todo what can we do here if the driver call failed */
   uint32_t nMinimalAsoluteTime = 0xFFFFFFFF;
   tPJedP2PRegistryEntry* pP2PEntry;
   W_ERROR nError;

   PDebugTrace("static_PJedTimeoutElapsed()");

   if(pInstance->nLastNDEFMessageTimeReference != 0)
   {
      if(pInstance->nLastNDEFMessageTimeReference <= nCurrentTime)
      {
         PDebugTrace("static_PJedTimeoutElapsed(): delete NDEF message for push");
         CDebugAssert(pInstance->hLastNDEFMessage != W_NULL_HANDLE);
         static_PJedRemoveHandle(pContext, pInstance, pInstance->hLastNDEFMessage);
         PHandleClose(pContext, pInstance->hLastNDEFMessage);
         pInstance->hLastNDEFMessage = W_NULL_HANDLE;
         pInstance->nLastNDEFMessageTimeReference = 0;
      }

      if(pInstance->nLastNDEFMessageTimeReference < nMinimalAsoluteTime)
      {
         nMinimalAsoluteTime = pInstance->nLastNDEFMessageTimeReference;
      }
   }

   if(pInstance->nKeepAliveConnectionTimeReference != 0)
   {
      if(pInstance->nKeepAliveConnectionTimeReference <= nCurrentTime)
      {
         PDebugTrace("static_PJedTimeoutElapsed(): delete keep alive connection");
         CDebugAssert(pInstance->hKeepAliveConnection != W_NULL_HANDLE);
         static_PJedRemoveHandle(pContext, pInstance, pInstance->hKeepAliveConnection);
         PReaderHandlerWorkPerformed(pContext, pInstance->hKeepAliveConnection, true, false);
         pInstance->hKeepAliveConnection = W_NULL_HANDLE;
         pInstance->nKeepAliveConnectionTimeReference = 0;
      }

      if(pInstance->nKeepAliveConnectionTimeReference < nMinimalAsoluteTime)
      {
         nMinimalAsoluteTime = pInstance->nKeepAliveConnectionTimeReference;
      }
   }

   if(pInstance->nLastConnectivityMessageTimeReference != 0)
   {
      if(pInstance->nLastConnectivityMessageTimeReference <= nCurrentTime)
      {
         PDebugTrace("static_PJedTimeoutElapsed(): delete connectivity message for push");
         pInstance->nLastConnectivityMessageLength = 0;
         pInstance->nLastConnectivityMessageTimeReference = 0;
      }

      if(pInstance->nLastConnectivityMessageTimeReference < nMinimalAsoluteTime)
      {
         nMinimalAsoluteTime = pInstance->nLastConnectivityMessageTimeReference;
      }
   }

   for(  pP2PEntry = pInstance->pP2PRegistryListHead;
         pP2PEntry != null;
         pP2PEntry = pP2PEntry->pNext)
   {
      if(pP2PEntry->nConnectionTimeReference != 0)
      {
         if(pP2PEntry->nConnectionTimeReference <= nCurrentTime)
         {
            PDebugWarning("static_PJedTimeoutElapsed(): Timeout elapsed for P2P socket connection");
            pP2PEntry->nConnectionTimeReference = 0;

            /* shutdwon the socket connection handle */
            if(pP2PEntry->hSocket != W_NULL_HANDLE)
            {
               PDebugWarning("static_PJedTimeoutElapsed(): shutdwon the socket handle = %dd = 0x%xx",
                  pP2PEntry->hSocket, pP2PEntry->hSocket);

               PP2PShutdownDriver(pContext,pP2PEntry->hSocket,static_PShutdownCompleted, pP2PEntry);

               nError = PContextGetLastIoctlError(pContext);
               if (nError != W_SUCCESS)
               {
                  PDFCPost2(pContext, P_DFC_TYPE_WJED, static_PShutdownCompleted, pP2PEntry, nError);
               }
            }
         }

         if(pP2PEntry->nConnectionTimeReference < nMinimalAsoluteTime)
         {
            nMinimalAsoluteTime = pP2PEntry->nConnectionTimeReference;
         }
      }
   }

   /* The timer is already stopped, restart it only if needed */
   if(nMinimalAsoluteTime != 0xFFFFFFFF)
   {
      if(pInstance->nCurrentTimerValue != nMinimalAsoluteTime)
      {
         pInstance->nCurrentTimerValue = nMinimalAsoluteTime;
         PMultiTimerSetDriver(pContext, TIMER_T12_USER_WJED,
                  nMinimalAsoluteTime,
                  static_PJedTimeoutElapsed, pInstance );
         /* @todo what can we do here if the driver call failed */
      }
   }
}

/** See tWReaderCardDetectionHandler */
static void static_PJedCardDetectionHandler(
               tContext* pContext,
               void* pHandlerParameter,
               W_HANDLE hConnection,
               W_ERROR nError )
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   uint32_t nCallbackParameter = PUtilConvertPointerToUint32(pHandlerParameter);
   uint32_t nListenerId = (nCallbackParameter & 0xFF);
   uint32_t nIsolate = (nCallbackParameter >> 8);

   PDebugTrace("static_PJedCardDetectionHandler() hConnection=0x%08X", hConnection);

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PJedCardDetectionHandler: received %s", PUtilTraceError(nError));
   }
   else
   {
      /* reset the keep alive connection */
      static_PJedCloseHandle(
         pContext, pInstance, pInstance->hKeepAliveConnection, true, false, true);

      if(hConnection != W_NULL_HANDLE)
      {
         if(PJedAddHandle(pContext, hConnection, nIsolate) != W_SUCCESS)
         {
            PDebugError("static_PJedCardDetectionHandler: PJedAddHandle() return in error");
         }

         pInstance->hKeepAliveConnection = hConnection;
         pInstance->nKeepAliveConnectionTimeReference =
            PNALServiceDriverGetCurrentTime(pContext) + pInstance->nKeepAliveTimeoutConfiguration;
         /* @todo what can we do here if the driver call failed */
         static_PJedTimerUpdate(pContext, pInstance);
      }
   }

   PDFCPostContext6(
      &pInstance->sCallbackContext, P_DFC_TYPE_WJED,
      nIsolate, WJED_EVENT_ID_LISTEN_TO_CARD_DETECTION,
      nListenerId, hConnection, nError );
}

/* See the porting guide of Open NFC J-Edition */
W_HANDLE PJedListenToCardDetection(
               tContext* pContext,
               uint32_t nIsolate,
               uint32_t nListenerId,
               const uint8_t* pProperties,
               uint32_t nPropertyNumber)
{
   W_HANDLE hRegistry = W_NULL_HANDLE;
   W_ERROR nError;
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   uint32_t nCallbackParameter = (nListenerId | (nIsolate << 8));

   PDebugTrace("PJedListenToCardDetection()");

   nError = PReaderListenToCardDetection (
      pContext,
      static_PJedCardDetectionHandler,
      PUtilConvertUint32ToPointer(nCallbackParameter),
      pInstance->nReaderPriority,
      pProperties, nPropertyNumber,
      &hRegistry);

   if(nError != W_SUCCESS)
   {
      PDebugError("PJedListenToCardDetection: PReaderListenToCardDetection() returned %s",
         PUtilTraceError(nError));
   }
   else
   {
      (void)PJedAddHandle(pContext, hRegistry, nIsolate);
   }

   return hRegistry;
}

/**
 * Compares 2 AID strings with masks.
 *
 * @param[in]  pAID1  The first AID string.
 *
 * @param[in]  pAID2  The second AID string.
 *
 * @return  true if the AIDs match.
 **/
static bool static_PJedCompareAID(
               const tchar* pAID1,
               const tchar* pAID2)
{
   uint32_t nLength = (pAID1 == null)?0:PUtilStringLength(pAID1);
   uint32_t nPos;

   PDebugTrace("static_PJedCompareAID AID1:");
   PDebugTraceBuffer((const uint8_t*)pAID1, nLength * sizeof(tchar));
   PDebugTrace("static_PJedCompareAID AID2:");
   PDebugTraceBuffer((const uint8_t*)pAID1, PUtilStringLength(pAID2) * sizeof(tchar));

   if(((pAID2 == null)?0:PUtilStringLength(pAID2)) != nLength)
   {
      PDebugTrace("static_PJedCompareAID: lengths are different");
      return false;
   }

   if((nLength & 1) != 0)
   {
      PDebugTrace("static_PJedCompareAID: odd value fot the length");
      return false;
   }

   for(nPos = 0; nPos < nLength; nPos++)
   {
      tchar c1, c2;

      c1 = pAID1[nPos];
      c2 = pAID2[nPos];

      if((c1 != ((tchar)'?')) && (c2 != ((tchar)'?')))
      {
         if(c1 >= 'a') { c1 = c1 - 'a' + 'A'; }
         if(c2 >= 'a') { c2 = c2 - 'a' + 'A'; }
         if(c1 != c2)
         {
            return false;
         }
      }
   }

   return true;
}

/**
 * Retrieves the next entry in the registry.
 *
 * @param[in]  pInstance  The current instance.
 *
 * @param[in]  nRegistryType  The registry type.
 *
 * @param[in]  nApplicationIdentifier  The application identifier.
 *
 * @param[in]  bUseIdentifier  Flag set to use the application identifier.
 *
 * @param[in]  nTNF  The TNF.
 *
 * @param[in]  pTypeString  The type string.
 *
 * @param[in]  pLastEntry  The last entry to exclude or null to start from the beginning.
 *
 * @return  The next corrsponding entry or null if no entry is found.
 **/
static tPJedRegistryEntry* static_PJedFindNextRegistryEntry(
               tPJedInstance* pInstance,
               uint32_t nRegistryType,
               uint32_t nApplicationIdentifier,
               bool bUseIdentifier,
               uint8_t nTNF,
               const tchar* pTypeString,
               tPJedRegistryEntry* pLastEntry)
{
   PDebugTrace("static_PJedFindNextRegistryEntry()");

   if(pLastEntry == null)
   {
      pLastEntry = pInstance->pRegistryListHead;
   }
   else
   {
      pLastEntry = pLastEntry->pNext;
   }

   while(pLastEntry != null)
   {
      if(pLastEntry->nRegistryType == nRegistryType)
      {
         if((pLastEntry->nApplicationIdentifier != nApplicationIdentifier)
         && (bUseIdentifier != false))
         {
            pLastEntry = pLastEntry->pNext;
            continue;
         }

         if(nRegistryType == WJED_TYPE_PUSH_REGISTRY_CONNECTIVITY_MESSAGE)
         {
            CDebugAssert(pTypeString != null);
            CDebugAssert(pLastEntry->pTypeString != null);
            if(static_PJedCompareAID(pLastEntry->pTypeString, pTypeString) != false)
            {
              return pLastEntry;
            }
         }
         else if((nRegistryType == WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE)
         || (nRegistryType == WJED_TYPE_LISTEN_NDEF_MESSAGE))
         {
            if(WNDEFCompareRecordType(
               pLastEntry->nTNF, pLastEntry->pTypeString,
               nTNF, pTypeString) != false)
            {
               return pLastEntry;
            }
         }
         else if((nRegistryType == WJED_TYPE_LISTEN_UICC_EOT_MESSAGE)
         || (nRegistryType == WJED_TYPE_LISTEN_SE_EOT_MESSAGE))
         {
            return pLastEntry;
         }
         else
         {
            PDebugError("static_PJedFindNextRegistryEntry: Wrong registry type");
            CDebugAssert(false);
         }
      }

      pLastEntry = pLastEntry->pNext;
   }

   return null;
}

/** See PSEMonitorEndOfTransaction */
static void static_PJedSEEndOfTransactionHandler(
               tContext* pContext,
               void *pHandlerParameter,
               uint32_t nSlotIdentifier,
               uint32_t nProtocol)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedRegistryEntry* pEntry;

   PDebugTrace("static_PJedSEEndOfTransactionHandler()");

   pEntry = null;
   while((pEntry = static_PJedFindNextRegistryEntry(
               pInstance,
               WJED_TYPE_LISTEN_SE_EOT_MESSAGE,
               0, false,
               0, null,
               pEntry)) != null)
   {
      PDFCPostContext6(
         &pInstance->sCallbackContext, P_DFC_TYPE_WJED,
         pEntry->nApplicationIdentifier, WJED_EVENT_ID_LISTEN_TO_TRANSACTION,
         pEntry->nListenerId, 0, 0 );
   }
}

/** See tWUICCMonitorTransactionEventHandler */
static void static_PJedConnectivityMessageHandler(
               tContext* pContext,
               void *pHandlerParameter)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedRegistryEntry* pEntry;
   uint8_t aAID[16];
   uint32_t nAIDLength;
   W_ERROR nError;
   tchar aAIDString[33];
   uint32_t nPos;

   PDebugTrace("static_PJedConnectivityMessageHandler()");

   pInstance->nLastConnectivityMessageLength = 0;
   pInstance->nLastConnectivityMessageTimeReference = 0;

   nAIDLength = PUICCGetTransactionEventAID(pContext, aAID, 16);
   /* @todo If the IOCTL failed, the nAIDLength is set to zero */

   for(nPos = 0; nPos < nAIDLength; nPos++)
   {
      uint8_t v = aAID[nPos];
      if((v & 0x0F) <= 9)
      {
         aAIDString[(nPos * 2) + 1] = (v & 0x0F) + '0';
      }
      else
      {
         aAIDString[(nPos * 2) + 1] = (v & 0x0F) - 10 + 'A';
      }
      v = (v >> 4) & 0x0F;
      if(v <= 9)
      {
         aAIDString[nPos * 2] = v + '0';
      }
      else
      {
         aAIDString[nPos * 2] = v - 10 + 'A';
      }
   }
   aAIDString[nAIDLength * 2] = 0;

   nError = PUICCGetConnectivityEventParameter(pContext,
      pInstance->aLastConnectivityData, WJED_MAX_UICC_CONNECTIVITY_DATA_SIZE,
      &pInstance->nLastConnectivityMessageLength);
   if(nError != W_SUCCESS)
   {
      PDebugError("static_PJedConnectivityMessageHandler: error returned by PUICCGetConnectivityEventParameter()");
      static_PJedTimerUpdate(pContext, pInstance);
      return;
   }
   pInstance->nLastConnectivityMessageTimeReference =
      PNALServiceDriverGetCurrentTime(pContext) + pInstance->nPushRegistryTimeoutConfiguration;
   /* @todo what can we do here if the driver call failed */
   static_PJedTimerUpdate(pContext, pInstance);

   pEntry = null;
   while((pEntry = static_PJedFindNextRegistryEntry(
               pInstance,
               WJED_TYPE_PUSH_REGISTRY_CONNECTIVITY_MESSAGE,
               0, false,
               0, aAIDString,
               pEntry)) != null)
   {
      PDFCPostContext2(
         &pEntry->sEventHandler, P_DFC_TYPE_WJED,
         pEntry->nApplicationIdentifier );
   }

   pEntry = null;
   while((pEntry = static_PJedFindNextRegistryEntry(
               pInstance,
               WJED_TYPE_LISTEN_UICC_EOT_MESSAGE,
               0, false,
               0, null,
               pEntry)) != null)
   {
      PDFCPostContext6(
         &pInstance->sCallbackContext, P_DFC_TYPE_WJED,
         pEntry->nApplicationIdentifier, WJED_EVENT_ID_LISTEN_TO_TRANSACTION,
         pEntry->nListenerId, 0, 0 );
   }
}

static W_ERROR static_PJedGetMessageType(
               tContext* pContext,
               W_HANDLE hMessage,
               uint8_t* pnTNF,
               tchar** ppTypeString)
{
   W_HANDLE hRecord = PNDEFGetRecord(pContext, hMessage, 0);
   W_ERROR nError;
   uint32_t nTypeLength;
   tchar* pTypeString = null;
   uint32_t nTNF;

   PDebugTrace("static_PJedGetMessageType()");

   if(hRecord == W_NULL_HANDLE)
   {
      PDebugError("static_PJedGetMessageType: error returned by PNDEFGetRecord()");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   nError = PNDEFGetRecordInfo(pContext, hRecord, W_NDEF_INFO_TNF, &nTNF);
   if(nError != W_SUCCESS)
   {
      PDebugError("static_PJedGetMessageType: error returned by PNDEFGetRecordInfo(TNF)");
      goto return_error;
   }
   *pnTNF = (uint8_t)nTNF;

   nError = PNDEFGetRecordInfo(pContext, hRecord, W_NDEF_INFO_TYPE_STRING_PRINT, &nTypeLength);
   if(nError != W_SUCCESS)
   {
      PDebugError("static_PJedGetMessageType: error returned by PNDEFGetRecordInfo(TYPE)");
      goto return_error;
   }

   if(nTypeLength != 0)
   {
      pTypeString = (tchar*)CMemoryAlloc((nTypeLength + 1) * sizeof(tchar));
      if(pTypeString == null)
      {
         PDebugError("static_PJedGetMessageType: Error returned by CMemoryAlloc()");
         nError = W_ERROR_OUT_OF_RESOURCE;
      }

      nError = PNDEFGetTypeString(pContext, hRecord,
         pTypeString, nTypeLength + 1, &nTypeLength);
      if(nError != W_SUCCESS)
      {
         PDebugError("static_PJedGetMessageType: error returned by PNDEFGetTypeString()");
         goto return_error;
      }
   }

   *ppTypeString = pTypeString;

   PHandleClose(pContext, hRecord);

   return W_SUCCESS;

return_error:

   CMemoryFree(pTypeString);
   PHandleClose(pContext, hRecord);
   *pnTNF = 0;
   *ppTypeString = null;

   return nError;
}

/**
 * Duplicates a NDEF message.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  hMessage   The message to duplicate.
 *
 * @return  The new message handle or W_NULL_HANDLE in case of error.
 **/
static W_HANDLE static_PJedDuplicateNDEFMessage(
               tContext* pContext,
               W_HANDLE hMessage)
{
   uint32_t nBufferLength = PNDEFGetMessageLength(pContext, hMessage);
   uint8_t* pBuffer = null;
   W_HANDLE hNewMessage = W_NULL_HANDLE;

   PDebugTrace("static_PJedDuplicateNDEFMessage()");

   if(nBufferLength == 0)
   {
      PDebugError("static_PJedDuplicateNDEFMessage: Error returned by PNDEFGetMessageLength()");
      goto return_function;
   }

   pBuffer = (uint8_t*)CMemoryAlloc(nBufferLength);
   if(pBuffer == null)
   {
      PDebugError("static_PJedDuplicateNDEFMessage: Error allocating memory");
      goto return_function;
   }

   if(PNDEFGetMessageContent(pContext, hMessage, pBuffer, nBufferLength, &nBufferLength) != W_SUCCESS)
   {
      PDebugError("static_PJedDuplicateNDEFMessage: Error returned by PNDEFGetMessageLength()");
      goto return_function;
   }

   if(PNDEFBuildMessage(pContext, pBuffer, nBufferLength, &hNewMessage) != W_SUCCESS)
   {
      PDebugError("static_PJedDuplicateNDEFMessage: Error returned by PNDEFBuildMessage()");
      goto return_function;
   }

return_function:

   CMemoryFree(pBuffer);

   return hNewMessage;
}

/**
 * Sends a NDEF message.
 *
 * @param[in]  pContext  The current context.
 *
 * @parma[in]  pInstance  The current instance.
 *
 * @param[in]  pEntry  The entry used to send the message.
 *
 * @param[in]  hMessage  The message to send.
 **/
static void static_PJedSendNDEFMessage(
                  tContext* pContext,
                  tPJedInstance* pInstance,
                  tPJedRegistryEntry* pEntry,
                  W_HANDLE hMessage)
{
   /* Duplicate the message */
   W_HANDLE hNewMessage = static_PJedDuplicateNDEFMessage(pContext, hMessage);

   PDebugTrace("static_PJedSendNDEFMessage()");

   if(hNewMessage != W_NULL_HANDLE)
   {
      if(PJedAddHandle(pContext, hNewMessage, pEntry->nApplicationIdentifier) != W_SUCCESS)
      {
         PDebugError("static_PJedSendNDEFMessage: PJedAddHandle() return in error");
      }

      PDFCPostContext6(
         &pInstance->sCallbackContext, P_DFC_TYPE_WJED,
         pEntry->nApplicationIdentifier, WJED_EVENT_ID_LISTEN_TO_NDEF_RECORD,
         pEntry->nListenerId, hNewMessage, 0 );
   }
   else
   {
      PDebugError("static_PJedSendNDEFMessage: Error returnd by static_PJedDuplicateNDEFMessage()");
   }
}

/**
 * @brief  Udpate the registration for the connectivity message and the NDEF message.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pInstance  The current registry.
 *
 * @return The error code.
 **/
static W_ERROR static_PJedUpdateRegistration(
               tContext* pContext,
               tPJedInstance* pInstance)
{
   uint32_t nListenNDEFMessageCount = 0;
   uint32_t nPushRegistryNDEFMessageCount = 0;
   uint32_t nPushRegistryConnectivityMessageCount = 0;
   uint32_t nPushRegistrySEEOTCount = 0;
   uint32_t nPushRegistryUICCEOTCount = 0;
   tPJedRegistryEntry* pEntry = pInstance->pRegistryListHead;
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("static_PJedUpdateRegistration()");

   while(pEntry != null)
   {
      switch(pEntry->nRegistryType)
      {
         case WJED_TYPE_LISTEN_NDEF_MESSAGE:
            nListenNDEFMessageCount++;
            break;
         case WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE:
            nPushRegistryNDEFMessageCount++;
            break;
         case WJED_TYPE_PUSH_REGISTRY_CONNECTIVITY_MESSAGE:
            nPushRegistryConnectivityMessageCount++;
            break;
         case WJED_TYPE_LISTEN_SE_EOT_MESSAGE:
            nPushRegistrySEEOTCount++;
            break;
         case WJED_TYPE_LISTEN_UICC_EOT_MESSAGE:
            nPushRegistryUICCEOTCount++;
            break;
         default:
            PDebugError("static_PJedUpdateRegistration: %p wrong type 0x%08X", pEntry, pEntry->nRegistryType);
            CDebugAssert(false);
            break;
      }

      pEntry = pEntry->pNext;
   }

   if((nPushRegistryConnectivityMessageCount + nPushRegistryUICCEOTCount) == 0)
   {
      if(pInstance->hRegistryListenUICCEOTMessage != W_NULL_HANDLE)
      {
         PDebugTrace("static_PJedUpdateRegistration: closing registration for UICC EVT_TRANSACTION message");
         PHandleClose(pContext, pInstance->hRegistryListenUICCEOTMessage);
         pInstance->hRegistryListenUICCEOTMessage = W_NULL_HANDLE;
      }
   }
   else
   {
      if(pInstance->hRegistryListenUICCEOTMessage == W_NULL_HANDLE)
      {
         PDebugTrace("static_PJedUpdateRegistration: registering for UICC EVT_TRANSACTION message");
         nError = PUICCMonitorTransactionEvent(
            pContext,
            static_PJedConnectivityMessageHandler, null,
            &pInstance->hRegistryListenUICCEOTMessage);

         if(nError != W_SUCCESS)
         {
            PDebugError("static_PJedUpdateRegistration: WUICCMonitorConnectivityEvent() returned %s",
               PUtilTraceError(nError));
         }
      }
   }

   if(nPushRegistrySEEOTCount == 0)
   {
      if(pInstance->hRegistryListenSEEOTMessage != W_NULL_HANDLE)
      {
         PHandleClose(pContext, pInstance->hRegistryListenSEEOTMessage);
         pInstance->hRegistryListenSEEOTMessage = W_NULL_HANDLE;
      }
   }
   else
   {
      if(pInstance->hRegistryListenSEEOTMessage == W_NULL_HANDLE)
      {
         nError = PSEMonitorEndOfTransaction(
               pContext, 0,
               static_PJedSEEndOfTransactionHandler, null,
               &pInstance->hRegistryListenSEEOTMessage);

         if(nError != W_SUCCESS)
         {
            PDebugError("static_PJedUpdateRegistration: WUICCMonitorConnectivityEvent() returned %s",
               PUtilTraceError(nError));
         }
      }
   }
   if((nListenNDEFMessageCount + nPushRegistryNDEFMessageCount) == 0)
   {
      if(pInstance->hRegistryListenNDEFMessage != W_NULL_HANDLE)
      {
         PHandleClose(pContext, pInstance->hRegistryListenNDEFMessage);
         pInstance->hRegistryListenNDEFMessage = W_NULL_HANDLE;
      }
   }
   else
   {
      if(pInstance->hRegistryListenNDEFMessage == W_NULL_HANDLE)
      {
         PNDEFReadMessageOnAnyTag (
            pContext,
            static_PJedNDEFMessageHandler, null,
            /* -1 is necessary to call the direct connection first and only
               after the NDEF listeners and the Push registrations.
               If a Midlet using a direct connection is writting in a tag,
               the new content will be used for the NDEF listeners and
               the Push registry. */
            (pInstance->nReaderPriority -1),
            W_NDEF_TNF_ANY_TYPE, null,
            &pInstance->hRegistryListenNDEFMessage);
      }
   }

   return nError;
}

/** See tWNDEFReadMessageCompleted */
static void static_PJedNDEFMessageHandler(
               tContext* pContext,
               void* pHandlerParameter,
               W_HANDLE hMessage,
               W_ERROR nError )
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   uint8_t nTNF;
   tchar* pTypeString = null;
   tPJedRegistryEntry* pEntry;

   PDebugTrace("static_PJedNDEFMessageHandler()");

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PJedNDEFMessageHandler: returns %s",
         PUtilTraceError(nError));
      CDebugAssert(hMessage == W_NULL_HANDLE);
      return;
   }

   if(hMessage == W_NULL_HANDLE)
   {
      PDebugError("static_PJedNDEFMessageHandler: null handle");
      return;
   }

   if(pInstance->hLastNDEFMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pInstance->hLastNDEFMessage);
      pInstance->hLastNDEFMessage = W_NULL_HANDLE;
      pInstance->nLastNDEFMessageTimeReference = 0;
   }

   if(PNDEFGetRecordNumber(pContext, hMessage) == 0)
   {
      PDebugWarning("static_PJedNDEFMessageHandler: empty message, ignore");
      goto return_error;
   }

   nError = static_PJedGetMessageType(pContext, hMessage, &nTNF, &pTypeString);
   if(nError != W_SUCCESS)
   {
      PDebugError("static_PJedNDEFMessageHandler: error returned by static_PJedGetMessageType()");
      goto return_error;
   }

   pInstance->hLastNDEFMessage = hMessage;
   pInstance->nLastNDEFMessageTimeReference =
      PNALServiceDriverGetCurrentTime(pContext) + pInstance->nPushRegistryTimeoutConfiguration;
   /* @todo what can we do here if the driver call failed */
   static_PJedTimerUpdate(pContext, pInstance);

   pEntry = null;
   while((pEntry = static_PJedFindNextRegistryEntry(
               pInstance,
               WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE,
               0, false,
               nTNF, pTypeString,
               pEntry)) != null)
   {
      PDFCPostContext2(
         &pEntry->sEventHandler, P_DFC_TYPE_WJED,
         pEntry->nApplicationIdentifier );
   }

   pEntry = null;
   while((pEntry = static_PJedFindNextRegistryEntry(
               pInstance,
               WJED_TYPE_LISTEN_NDEF_MESSAGE,
               0, false,
               nTNF, pTypeString,
               pEntry)) != null)
   {
      static_PJedSendNDEFMessage(pContext, pInstance, pEntry, hMessage);
   }

   CMemoryFree(pTypeString);

   return;

return_error:

   CMemoryFree(pTypeString);
   PHandleClose(pContext, hMessage);

   PHandleClose(pContext, pInstance->hLastNDEFMessage);
   pInstance->hLastNDEFMessage = W_NULL_HANDLE;
   pInstance->nLastNDEFMessageTimeReference = 0;
   static_PJedTimerUpdate(pContext, pInstance);
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedReadOnAnyTag(
               tContext* pContext,
               uint32_t nIsolate,
               bool bRegister,
               uint32_t nListenerId,
               uint8_t nTNF,
               const tchar* pTypeString)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   W_ERROR nError;
   tPJedRegistryEntry* pNewEntry;
   tPJedRegistryEntry** ppPrevEntry;
   tchar* pTypeStringCopy = null;

   PDebugTrace("PJedReadOnAnyTag()");

   if(bRegister == false)
   {
      pNewEntry = null;
      while((pNewEntry = static_PJedFindNextRegistryEntry(
               pInstance,
               WJED_TYPE_LISTEN_NDEF_MESSAGE,
               nIsolate, true,
               nTNF, pTypeString,
               pNewEntry)) != null)
      {
         if(pNewEntry->nListenerId == nListenerId)
         {
            break;
         }
      }

      if(pNewEntry == null)
      {
         PDebugError("PJedReadOnAnyTag: Entry not found");
         return W_ERROR_BAD_PARAMETER;
      }

      for( ppPrevEntry = &pInstance->pRegistryListHead;
           *ppPrevEntry != pNewEntry;
           ppPrevEntry = &((*ppPrevEntry)->pNext) ) {}
      *ppPrevEntry = pNewEntry->pNext;

      static_PJedDestroyRegistryEntry(pContext, pNewEntry);

      nError = static_PJedUpdateRegistration(pContext, pInstance);
      if(nError != W_SUCCESS)
      {
         PDebugError("PJedReadOnAnyTag: Error returned by static_PJedUpdateRegistration");
      }

      return W_SUCCESS;
   }

   pNewEntry = (tPJedRegistryEntry*)CMemoryAlloc(sizeof(tPJedRegistryEntry));
   if(pNewEntry == null)
   {
      PDebugError("PJedReadOnAnyTag: Cannot allocate the entry()");
      return W_ERROR_OUT_OF_RESOURCE;
   }
   CMemoryFill(pNewEntry, 0, sizeof(tPJedRegistryEntry));

   pNewEntry->nApplicationIdentifier = nIsolate;
   pNewEntry->nListenerId = nListenerId;
   pNewEntry->nTNF = nTNF;
   pNewEntry->nRegistryType = WJED_TYPE_LISTEN_NDEF_MESSAGE;
   pNewEntry->pNext = null;
   pNewEntry->hP2POpenOperation = W_NULL_HANDLE;

   /* Duplicate the type string */
   if(pTypeString != null)
   {
      uint32_t nLength = PUtilStringLength(pTypeString);
      if(nLength != 0)
      {
         nLength = (nLength + 1)*sizeof(tchar);
         pTypeStringCopy = (tchar*)CMemoryAlloc(nLength);
         if(pTypeStringCopy == null)
         {
            PDebugError("PJedReadOnAnyTag: Cannot allocate the type string");
            static_PJedDestroyRegistryEntry(pContext, pNewEntry);
            return W_ERROR_OUT_OF_RESOURCE;
         }
         CMemoryCopy(pTypeStringCopy, pTypeString, nLength);
      }
   }
   pNewEntry->pTypeString = pTypeStringCopy;

   /* Check if the last NDEF message is not available */
   if(pInstance->hLastNDEFMessage != W_NULL_HANDLE)
   {
      tchar* pMessageTypeString;
      uint8_t nMessageTNF;

      /* Check if the message is Ok for the new listener */
      nError = static_PJedGetMessageType(pContext, pInstance->hLastNDEFMessage,
         &nMessageTNF, &pMessageTypeString);
      if(nError != W_SUCCESS)
      {
         PDebugError("PJedReadOnAnyTag: error returned by static_PJedGetMessageType()");
         static_PJedDestroyRegistryEntry(pContext, pNewEntry);
         return W_ERROR_BAD_NDEF_FORMAT;
      }

      if(WNDEFCompareRecordType(
         nMessageTNF, pMessageTypeString,
         pNewEntry->nTNF, pNewEntry->pTypeString) != false)
      {
         static_PJedSendNDEFMessage(pContext, pInstance,
            pNewEntry, pInstance->hLastNDEFMessage);
      }

      CMemoryFree(pMessageTypeString);
   }

   for( ppPrevEntry = &pInstance->pRegistryListHead;
        *ppPrevEntry != null;
        ppPrevEntry = &((*ppPrevEntry)->pNext) ) {}
   *ppPrevEntry = pNewEntry;

   nError = static_PJedUpdateRegistration(pContext, pInstance);
   if(nError != W_SUCCESS)
   {
      PDebugError("PJedReadOnAnyTag: Error returned by static_PJedUpdateRegistration");
      *ppPrevEntry = null;
   }

   if(nError != W_SUCCESS)
   {
      static_PJedDestroyRegistryEntry(pContext, pNewEntry);
   }

   return nError;
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedMonitorEndOfTransaction(
               tContext* pContext,
               uint32_t nIsolate,
               bool bRegister,
               uint32_t nSlotId)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   W_ERROR nError;
   tPJedRegistryEntry* pNewEntry;
   tPJedRegistryEntry** ppPrevEntry;
   uint32_t nRegistryType;

   PDebugTrace("PJedMonitorEndOfTransaction(bRegister=%d, nSlotId=%d)", bRegister, nSlotId);

   switch(nSlotId)
   {
      case WJED_UICC_SLOT:
         nRegistryType = WJED_TYPE_LISTEN_UICC_EOT_MESSAGE;
         break;
     case WJED_SE_SLOT:
         nRegistryType = WJED_TYPE_LISTEN_SE_EOT_MESSAGE;
         break;
     default:
         PDebugError("PJedMonitorEndOfTransaction: bad slot identifier");
         return W_ERROR_BAD_PARAMETER;
   }

   if(bRegister == false)
   {
      pNewEntry = null;
      while((pNewEntry = static_PJedFindNextRegistryEntry(
               pInstance,
               nRegistryType,
               nIsolate, true,
               0, null,
               pNewEntry)) != null)
      {
         if(pNewEntry->nListenerId == nSlotId)
         {
            break;
         }
      }

      if(pNewEntry == null)
      {
         PDebugError("PJedMonitorEndOfTransaction: Entry not found");
         return W_ERROR_BAD_PARAMETER;
      }

      for( ppPrevEntry = &pInstance->pRegistryListHead;
           *ppPrevEntry != pNewEntry;
           ppPrevEntry = &((*ppPrevEntry)->pNext) ) {}
      *ppPrevEntry = pNewEntry->pNext;

      static_PJedDestroyRegistryEntry(pContext, pNewEntry);

      nError = static_PJedUpdateRegistration(pContext, pInstance);
      if(nError != W_SUCCESS)
      {
         PDebugError("PJedMonitorEndOfTransaction: Error returned by static_PJedUpdateRegistration");
      }

      return W_SUCCESS;
   }

   pNewEntry = (tPJedRegistryEntry*)CMemoryAlloc(sizeof(tPJedRegistryEntry));
   if(pNewEntry == null)
   {
      PDebugError("PJedMonitorEndOfTransaction: Cannot allocate the entry()");
      return W_ERROR_OUT_OF_RESOURCE;
   }
   CMemoryFill(pNewEntry, 0, sizeof(tPJedRegistryEntry));

   pNewEntry->nApplicationIdentifier = nIsolate;
   pNewEntry->nListenerId = nSlotId;
   pNewEntry->nTNF = 0;
   pNewEntry->pTypeString = null;
   pNewEntry->nRegistryType = nRegistryType;
   pNewEntry->pNext = null;
   pNewEntry->hP2POpenOperation = W_NULL_HANDLE;

   /* Check if the last connectivity message is available */
   if(pInstance->nLastConnectivityMessageTimeReference != 0)
   {
      PDFCPostContext6(
         &pInstance->sCallbackContext, P_DFC_TYPE_WJED,
         pNewEntry->nApplicationIdentifier, WJED_EVENT_ID_LISTEN_TO_TRANSACTION,
         pNewEntry->nListenerId, 0, 0 );
   }

   for( ppPrevEntry = &pInstance->pRegistryListHead;
        *ppPrevEntry != null;
        ppPrevEntry = &((*ppPrevEntry)->pNext) ) {}
   *ppPrevEntry = pNewEntry;

   nError = static_PJedUpdateRegistration(pContext, pInstance);
   if(nError != W_SUCCESS)
   {
      PDebugError("PJedMonitorEndOfTransaction: Error returned by static_PJedUpdateRegistration");
      *ppPrevEntry = null;
   }

   if(nError != W_SUCCESS)
   {
      static_PJedDestroyRegistryEntry(pContext, pNewEntry);
   }

   return nError;
}

static W_ERROR static_getP2PSAP( const uint8_t* pURLString,
                                 uint8_t* pnSAP,
                                 uint32_t nTypeStringPos)
{
   uint16_t v;
   uint32_t i;
   uint32_t nIndex = 0;
   uint32_t nSapLength = 0;
   uint32_t nSAPStringIndex = nTypeStringPos;
   tchar* pSapString = null;

   PDebugTrace("static_getP2PSAP() URL=%s", pURLString);

   /* Get the position of the last ':' character */
   while(pURLString[nSAPStringIndex] != 0)
   { nSAPStringIndex++; }
   while((nTypeStringPos != nSAPStringIndex) && (pURLString[nSAPStringIndex] != ':'))
   {  nSAPStringIndex--; }

   if(pURLString[nSAPStringIndex] != ':')
   {
      PDebugError("static_getP2PSAP: Wrong URL string \"%s\" SAP not found", pURLString);
      return W_ERROR_BAD_PARAMETER;
   }
   nSAPStringIndex++;

   /* Get the SAP string length */
   while( (pURLString[nSAPStringIndex + nSapLength] != ';')&&(pURLString[nSAPStringIndex + nSapLength] != 0))
   { nSapLength++; }

   /* Copy and convert to Utf-16 */
   pSapString = (tchar*)CMemoryAlloc((nSapLength + 1)*sizeof(tchar));
   if(pSapString == null)
   {
      PDebugError("static_PJedPushRegistryCheckURL: Cannot allocate the SAP string");
      return W_ERROR_OUT_OF_RESOURCE;
   }
   while( (pURLString[nSAPStringIndex + nIndex] != ';')&&(pURLString[nSAPStringIndex + nIndex] != 0) )
   {
      pSapString[nIndex] = pURLString[nSAPStringIndex + nIndex];
      nIndex++;
   }
   pSapString[nSapLength] = 0;

   /* Convert SAP to Utf-8 */
   for (i = 0; i < nSapLength; i++)
   {
      v = pSapString[i];
      if ((*pSapString >= '0') && (*pSapString <= '9'))
      {
         *pnSAP *= (uint8_t)(10*i);
         *pnSAP += (uint8_t)(v - '0');
      }
   }

   return W_SUCCESS;
}

/**
 * Checks the Push registry URL.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pURLString  The Push registry URL.
 *
 * @param[out] pnRegistryType  A pointer on a variable valued with the registry type.
 *
 * @param[out] pnTNF  A pointer on a variable valued with the TNF.
 *
 * @param[out] ppTypeString  A pointer on a variable valued with the type string
 *             of the URL. This value should be freed after use.
 *             null is returned in case of error.
 **/
static W_ERROR static_PJedPushRegistryCheckURL(
               tContext* pContext,
               const uint8_t* pURLString,
               uint32_t* pnRegistryType,
               uint8_t* pnTNF,
               tchar** ppTypeString,
               uint8_t* pnSAP)
{
   uint32_t nTypeStringPos = 0;
   uint32_t nLength = 0;
   tchar* pTypeString = null;
   uint32_t nPos = 0;
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("static_PJedPushRegistryCheckURL() URL = %s", pURLString);

   if(pURLString == null)
   {
      PDebugError("static_PJedPushRegistryCheckURL: null URL string");
      return W_ERROR_BAD_PARAMETER;
   }

   /* Get the position of the '=' character */
   while((pURLString[nTypeStringPos] != 0) && (pURLString[nTypeStringPos] != '='))
   { nTypeStringPos++; }

   if(pURLString[nTypeStringPos] != '=')
   {
      PDebugError("static_PJedPushRegistryCheckURL: Wrong URL string \"%s\"", pURLString);
      return W_ERROR_BAD_PARAMETER;
   }
   nTypeStringPos++;

   /* Get the type string length */
   while( (pURLString[nTypeStringPos + nLength] != 0) && (pURLString[nTypeStringPos + nLength] != ':'))
   { nLength++; }

   if((nTypeStringPos == 14) && (nLength > 12)
   && (CMemoryCompare(pURLString, "ndef:rtd?name=urn:nfc:wkt:", (nTypeStringPos+12)) == 0))
   {
      nLength-=12;
      nTypeStringPos+=12;

      *pnRegistryType = WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE;
      *pnTNF = W_NDEF_TNF_WELL_KNOWN;
   }
   else if((nTypeStringPos == 23) && (nLength > 12)
   && (CMemoryCompare(pURLString, "ndef:external_rtd?name=urn:nfc:ext:", (nTypeStringPos+12)) == 0))
   {
      nLength-=12;
      nTypeStringPos+=12;

      *pnRegistryType = WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE;
      *pnTNF = W_NDEF_TNF_EXTERNAL;
   }
   else if((nTypeStringPos == 15)
   && (CMemoryCompare(pURLString, "ndef:mime?name=", nTypeStringPos) == 0))
   {
      *pnRegistryType = WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE;
      *pnTNF = W_NDEF_TNF_MEDIA;
   }
   else if((nTypeStringPos == 14)
   && (CMemoryCompare(pURLString, "ndef:uri?name=", nTypeStringPos) == 0))
   {
      *pnRegistryType = WJED_TYPE_PUSH_REGISTRY_NDEF_MESSAGE;
      *pnTNF = W_NDEF_TNF_ABSOLUTE_URI;
   }
   else if((nTypeStringPos == 20)
   && (CMemoryCompare(pURLString, "secure-element:?aid=", nTypeStringPos) == 0))
   {
      *pnRegistryType = WJED_TYPE_PUSH_REGISTRY_CONNECTIVITY_MESSAGE;
      *pnTNF = 0; /* not used */
   }
   else if((nTypeStringPos == 19)
   && (CMemoryCompare(pURLString, "nfcp2p:server?name=", nTypeStringPos) == 0))
   {
      if ((nError = static_getP2PSAP(pURLString, pnSAP, nTypeStringPos)) != W_SUCCESS)
      {
         return nError;
      }
      *pnRegistryType = WJED_TYPE_P2P_REGISTRY;
      *pnTNF = W_P2P_TYPE_SERVER;
   }
   else if((nTypeStringPos == 19)
   && (CMemoryCompare(pURLString, "nfcp2p:client?name=", nTypeStringPos) == 0))
   {
      *pnSAP = 0;
      *pnRegistryType = WJED_TYPE_P2P_REGISTRY;
      *pnTNF = W_P2P_TYPE_CLIENT;
   }
   else if((nTypeStringPos == 26)
   && (CMemoryCompare(pURLString, "nfcp2p:client_server?name=", nTypeStringPos) == 0))
   {
      if ((nError = static_getP2PSAP(pURLString, pnSAP, nTypeStringPos)) != W_SUCCESS)
      {
         return nError;
      }
      *pnRegistryType = WJED_TYPE_P2P_REGISTRY;
      *pnTNF = W_P2P_TYPE_CLIENT_SERVER;
   }
   else
   {
      PDebugError("static_PJedPushRegistryCheckURL: Wrong type string \"%s\"", pURLString);
      return W_ERROR_BAD_PARAMETER;
   }

   pTypeString = (tchar*)CMemoryAlloc((nLength + 1)*sizeof(tchar));
   if(pTypeString == null)
   {
      PDebugError("static_PJedPushRegistryCheckURL: Cannot allocate the type string");
      return W_ERROR_OUT_OF_RESOURCE;
   }
   /* Copy and convert to Utf-16 */
   while((pURLString[nTypeStringPos + nPos] != 0) && (pURLString[nTypeStringPos + nPos] != ':'))
   {
      pTypeString[nPos] = pURLString[nTypeStringPos + nPos];
      nPos++;
   }
   pTypeString[nLength] = 0;

   if(ppTypeString != null)
   {
      *ppTypeString = pTypeString;
   }
   else
   {
      CMemoryFree(pTypeString);
   }

   return W_SUCCESS;
}

/* See the porting guide of Open NFC J-Edition  */
W_ERROR PJedPushRegistryCheckEntry(
               tContext* pContext,
               const uint8_t* pURLString)
{
   uint32_t nRegistryType;
   uint8_t nTNF;
   uint8_t nSAP;

   PDebugTrace("PJedPushRegistryCheckEntry()");

   return static_PJedPushRegistryCheckURL(
         pContext, pURLString, &nRegistryType, &nTNF, null, &nSAP);
}

/* Add a push registry entry  */
static W_ERROR static_PJedAddOtherPushRegistry(
               tContext* pContext,
               tPJedPushRegistryEventHandler* pEventHandler,
               void* pHandlerParameter,
               uint32_t nApplicationIdentifier,
               uint32_t nRegistryType,
               uint8_t nTNF,
               const tchar* pTypeString)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   W_ERROR nError;
   tPJedRegistryEntry* pNewEntry;
   tPJedRegistryEntry** ppPrevEntry;

   PDebugTrace("static_PJedAddOtherPushRegistry()");

   /* Check if the entry is not already in the list */
   if(static_PJedFindNextRegistryEntry(
               pInstance,
               nRegistryType,
               nApplicationIdentifier, true,
               nTNF,
               pTypeString,
               null) != null)
   {
      PDebugError("static_PJedAddOtherPushRegistry: Already registered");
      CMemoryFree((void*)pTypeString);
      return W_ERROR_EXCLUSIVE_REJECTED;
   }

   pNewEntry = (tPJedRegistryEntry*)CMemoryAlloc(sizeof(tPJedRegistryEntry));
   if(pNewEntry == null)
   {
      PDebugError("static_PJedAddOtherPushRegistry: Cannot allocate the entry()");
      CMemoryFree((void*)pTypeString);
      return W_ERROR_OUT_OF_RESOURCE;
   }
   CMemoryFill(pNewEntry, 0, sizeof(tPJedRegistryEntry));
   PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pEventHandler, pHandlerParameter,
         &pNewEntry->sEventHandler );
   pNewEntry->nApplicationIdentifier = nApplicationIdentifier;
   pNewEntry->nListenerId = 0;
   pNewEntry->pNext = null;
   pNewEntry->hP2POpenOperation = W_NULL_HANDLE;
   pNewEntry->nRegistryType = nRegistryType;
   pNewEntry->nTNF = nTNF;
   pNewEntry->pTypeString = pTypeString;

   for( ppPrevEntry = &pInstance->pRegistryListHead;
        *ppPrevEntry != null;
        ppPrevEntry = &((*ppPrevEntry)->pNext) ) {}
   *ppPrevEntry = pNewEntry;

   nError = static_PJedUpdateRegistration(pContext, pInstance);
   if(nError != W_SUCCESS)
   {
      PDebugError("static_PJedAddOtherPushRegistry: Error returned by static_PJedUpdateRegistration");
      static_PJedDestroyRegistryEntry(pContext, pNewEntry);
      *ppPrevEntry = null;
   }

   return nError;
}

/**
 * Finds a P2P registry entry wit the specified URI.
 *
 * @param[in]   pInstance The current instance.
 *
 * @param[in]   pURIString  The URI to find.
 *
 * @return  The entry if an entry is found or zero if no entry is found.
 **/
static tPJedP2PRegistryEntry* static_PJedFindP2PRegistryWithURI(
               tPJedInstance* pInstance,
               const tchar* pURIString)
{
   tPJedP2PRegistryEntry* pP2PEntry = pInstance->pP2PRegistryListHead;

   PDebugTrace("static_PJedFindP2PRegistryWithURI()");

   if (pP2PEntry == null)
      PDebugError("static_PJedFindP2PRegistryWithURI(): pP2PEntry == null");

   while(pP2PEntry != null)
   {
      PDebugTrace("static_PJedFindP2PRegistryWithURI(): pP2PEntry != null");
      if(PUtilStringCompare(pP2PEntry->pURIString, pURIString) == 0)
      {
         return pP2PEntry;
      }
      pP2PEntry = pP2PEntry->pNext;
   }

   return null;
}

/* Add a P2P push registry entry  */
static W_ERROR static_PJedAddP2PPushRegistry(
               tContext* pContext,
               tPJedPushRegistryEventHandler* pEventHandler,
               void* pHandlerParameter,
               uint32_t nApplicationIdentifier,
               uint32_t nP2P2Type,
               const tchar* pURIString,
               uint8_t nSAP)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedP2PRegistryEntry* pNewP2PEntry;

   PDebugTrace("static_PJedAddP2PPushRegistry(): nSAP = 0x%x = %d", nSAP, nSAP);

   /* Check if the entry is not already in the list */
   pNewP2PEntry = static_PJedFindP2PRegistryWithURI(pInstance, pURIString);

   if(pNewP2PEntry == null)
   {
      W_ERROR status = W_SUCCESS;
      tPJedP2PRegistryEntry** ppPrevP2PEntry;

      pNewP2PEntry = (tPJedP2PRegistryEntry*)CMemoryAlloc(sizeof(tPJedP2PRegistryEntry));
      if(pNewP2PEntry == null)
      {
         PDebugError("static_PJedAddP2PPushRegistry: Cannot allocate the entry()");
         CMemoryFree((void*)pURIString);
         return W_ERROR_OUT_OF_RESOURCE;
      }
      CMemoryFill(pNewP2PEntry, 0, sizeof(tPJedP2PRegistryEntry));
      PDFCFillCallbackContext(
            pContext, (tDFCCallback*)pEventHandler, pHandlerParameter,
            &pNewP2PEntry->sPushHandler );

      pNewP2PEntry->nApplicationIdentifier = nApplicationIdentifier;
      pNewP2PEntry->nIsolate = 0;
      pNewP2PEntry->nP2PType = nP2P2Type;
      pNewP2PEntry->pURIString = pURIString;
      pNewP2PEntry->hSocket= W_NULL_HANDLE;
      pNewP2PEntry->nSAP = nSAP;
      pNewP2PEntry->nConnectionTimeReference = 0;

      pNewP2PEntry->pNext = null;

      for( ppPrevP2PEntry = &pInstance->pP2PRegistryListHead;
           *ppPrevP2PEntry != null;
           ppPrevP2PEntry = &((*ppPrevP2PEntry)->pNext) ) {}
      *ppPrevP2PEntry = pNewP2PEntry;

      /* Creates a new P2P socket */
      PDebugTrace("static_PJedAddP2PPushRegistry(): Creates a P2P socket on SAP = %dd", nSAP);
      status = PP2PCreateSocketDriver( pContext, (uint8_t)nP2P2Type,
                   pURIString, (1+PUtilStringLength(pURIString))*sizeof(tchar),
                   nSAP, &pNewP2PEntry->hSocket);

      PDebugTrace("static_PJedAddP2PPushRegistry(): hSocket = %d", pNewP2PEntry->hSocket);

      if (status == W_SUCCESS)
      {
         pInstance->hLinkOperation = W_NULL_HANDLE;
         pInstance->hLink = W_NULL_HANDLE;

         /* Requests establishment of the P2P link */
         PDebugTrace("static_PJedAddP2PPushRegistry(): Requests establishment link");
         PP2PEstablishLinkWrapper(  pContext,
                                    static_PEstablishmentCallback, pNewP2PEntry,
                                    static_PReleaseCallback, pNewP2PEntry,
                                    &pInstance->hLinkOperation);
         PDebugTrace("static_PReleaseCallback(): hLinkOperation = %d = 0x%x",
            pInstance->hLinkOperation, pInstance->hLinkOperation);
      }
      else
      {
         PDebugError("static_PJedAddP2PPushRegistry(): Creates a P2P socket error = %s ", PUtilTraceError(status));
         CMemoryFree((void*)pURIString);
         return status;
      }
   }
   else
   {
      PDebugError("static_PJedAddP2PPushRegistry: The URI is already used in the push registry");
      CMemoryFree((void*)pURIString);
      return W_ERROR_EXCLUSIVE_REJECTED;
   }

   return W_SUCCESS;
}


/* See the porting guide of Open NFC J-Edition  */
W_ERROR PJedPushRegistryAddEntry(
               tContext* pContext,
               tPJedPushRegistryEventHandler* pEventHandler,
               void* pHandlerParameter,
               uint32_t nApplicationIdentifier,
               const uint8_t* pURLString)
{
   W_ERROR nError;
   uint32_t nRegistryType;
   uint8_t nTNF;
   uint8_t nSAP;
   tchar* pTypeString;

   PDebugTrace("PJedPushRegistryAddEntry() URL=%s", pURLString);

   /* Select the type of registry based on the URL */
   nError = static_PJedPushRegistryCheckURL(
         pContext,  pURLString,
         &nRegistryType, &nTNF, &pTypeString, &nSAP);

   PDebugTrace("PJedPushRegistryAddEntry: nSAP = 0x%xh = %dd", nSAP, nSAP);

   if(nError != W_SUCCESS)
   {
      PDebugError("PJedPushRegistryAddEntry: Error returned by static_PJedPushRegistryCheckURL()");
      return nError;
   }

   if(nRegistryType == WJED_TYPE_P2P_REGISTRY)
   {
      return static_PJedAddP2PPushRegistry(pContext,
         pEventHandler, pHandlerParameter, nApplicationIdentifier, nTNF, pTypeString, nSAP);
   }
   else
   {
      return static_PJedAddOtherPushRegistry(pContext,
         pEventHandler, pHandlerParameter, nApplicationIdentifier, nRegistryType, nTNF, pTypeString);
   }
}

/* Removes a P2P entry  */
static W_ERROR static_PJedRemoveP2PPushRegistry(
               tContext* pContext,
               tPJedInstance* pInstance,
               uint32_t nApplicationIdentifier,
               uint32_t nP2PType,
               const tchar* pURIString)
{
   tPJedP2PRegistryEntry* pP2PEntry;

   PDebugTrace("static_PJedRemoveP2PPushRegistry()");

   /* Check if the entry is not already in the list */
   pP2PEntry = static_PJedFindP2PRegistryWithURI(pInstance, pURIString);

   if((pP2PEntry == null)
   || (pP2PEntry->nP2PType != nP2PType))
   {
      PDebugError("static_PJedFindP2PRegistryWithURI: Cannot find the registry entry");
      return W_ERROR_BAD_PARAMETER;
   }

   if(pP2PEntry->nConnectionTimeReference != 0)
   {
      pP2PEntry->nConnectionTimeReference = 0;
      static_PJedTimerUpdate(pContext, pInstance);
   }

   if(pP2PEntry->hSocket!= W_NULL_HANDLE)
   {
      static_PJedRemoveHandle(pContext, pInstance, pP2PEntry->hSocket);
   }
   PDebugTrace("static_PJedRemoveP2PPushRegistry(): Remove And Destroy P2P Registry Entry...");
   static_PJedRemoveAndDestroyP2PRegistryEntry(pContext, pInstance, pP2PEntry);

   return W_SUCCESS;
}

/* See the porting guide of Open NFC J-Edition  */
W_ERROR PJedPushRegistryRemoveEntry(
               tContext* pContext,
               uint32_t nApplicationIdentifier,
               const uint8_t* pURLString)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   uint8_t nTNF;
   uint32_t nRegistryType;
   uint8_t nSAP;
   tchar* pTypeString;
   W_ERROR nError;

   PDebugTrace("PJedPushRegistryRemoveEntry() URL=%s", pURLString);

   /* Select the type of registry based on the URL */
   nError = static_PJedPushRegistryCheckURL(
         pContext, pURLString, &nRegistryType, &nTNF, &pTypeString, &nSAP);

   if(nError != W_SUCCESS)
   {
      PDebugError("PJedPushRegistryRemoveEntry: Error returned by static_PJedPushRegistryCheckURL()");
      return nError;
   }

   if(nRegistryType == WJED_TYPE_P2P_REGISTRY)
   {
      return static_PJedRemoveP2PPushRegistry(
               pContext,
               pInstance,
               nApplicationIdentifier,
               nTNF, pTypeString);
   }
   else
   {
      tPJedRegistryEntry* pEntry = null;
      tPJedRegistryEntry** ppPrevEntry;

      pEntry = static_PJedFindNextRegistryEntry(
                  pInstance,
                  nRegistryType,
                  nApplicationIdentifier, true,
                  nTNF,
                  pTypeString,
                  null);

      if(pEntry == null)
      {
         PDebugError("PJedPushRegistryRemoveEntry: Entry not found");
         return W_ERROR_BAD_PARAMETER;
      }

      /* Remove the entry */
      for(ppPrevEntry = &pInstance->pRegistryListHead;
         *ppPrevEntry != pEntry;
         ppPrevEntry = &((*ppPrevEntry)->pNext)) {}
      *ppPrevEntry = pEntry->pNext;

      static_PJedDestroyRegistryEntry(pContext, pEntry);

      return static_PJedUpdateRegistration(pContext, pInstance);
   }
}

#define WJED_INFO_TYPE_MESSAGE_BUFFER    0x10
#define WJED_INFO_TYPE_RECORD_NUMBER     0x11
#define WJED_INFO_TYPE_CONNECTIVITY_MESSAGE  0x12
#define WJED_INFO_TYPE_CONNECTION_PROPERTIES 0x13
#define WJED_INFO_TYPE_CONNECTION_UID 0x14

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedGetInfo(
               tContext* pContext,
               W_HANDLE hHandle,
               uint32_t nInfoType,
               uint32_t* pnValue)
{
   tPJedInstance* pInstance;

   PDebugTrace("PJedGetInfo() hHandle=%08X, nInfoType=%d", hHandle, nInfoType);

   switch(nInfoType)
   {
      case WJED_INFO_TYPE_MESSAGE_BUFFER:
         *pnValue = PNDEFGetMessageLength(pContext, hHandle);
         return W_SUCCESS;

      case WJED_INFO_TYPE_RECORD_NUMBER:
         *pnValue = PNDEFGetRecordNumber(pContext, hHandle);
         return W_SUCCESS;

      case WJED_INFO_TYPE_CONNECTION_PROPERTIES:
         return PBasicGetConnectionPropertyNumber(pContext, hHandle, pnValue);

      case WJED_INFO_TYPE_CONNECTION_UID:
         if((PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_3_A) == W_SUCCESS)
         || (PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_4_A) == W_SUCCESS))
         {
            tW14Part3ConnectionInfo sInfo;
            if(P14Part3GetConnectionInfo(
               pContext, hHandle, &sInfo) == W_SUCCESS)
            {
               /* sInfo.sW14TypeA.aUID[] */
               *pnValue = sInfo.sW14TypeA.nUIDLength;
            }
         }
         else if((PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_PICOPASS_2K) == W_SUCCESS)
         || (PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_PICOPASS_32K) == W_SUCCESS))
         {
            *pnValue = 8;
         }
         else if((PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_3_B) == W_SUCCESS)
         || (PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_4_B) == W_SUCCESS))
         {
            *pnValue = 4;
         }
         else if(PBasicCheckConnectionProperty(
            pContext, hHandle, W_PROP_TYPE1_CHIP) == W_SUCCESS)
         {
            *pnValue = 4;
         }
         else if(PBasicCheckConnectionProperty(
            pContext, hHandle, W_PROP_FELICA) == W_SUCCESS)
         {
            *pnValue = 8;
         }
         else if(PBasicCheckConnectionProperty(
            pContext, hHandle, W_PROP_ISO_15693_3) == W_SUCCESS)
         {
            *pnValue = 8;
         }
         else
         {
            *pnValue = 0;
         }
         return W_SUCCESS;

      case WJED_INFO_TYPE_CONNECTIVITY_MESSAGE:
         pInstance = PContextGetJEditionHelper(pContext);

         /* Return the length of the last message but do not erase the message */
         *pnValue = pInstance->nLastConnectivityMessageLength;
         return W_SUCCESS;

      default:
         return PNDEFGetRecordInfo(pContext, hHandle, nInfoType, pnValue);
   }
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedGetInfoBuffer(
               tContext* pContext,
               W_HANDLE hHandle,
               uint32_t nInfoType,
               uint8_t* pBuffer,
               uint32_t nBufferLength,
               uint32_t* pnActualLength)
{
   PDebugTrace("PJedGetInfoBuffer() hHandle=%08X, nInfoType=%d", hHandle, nInfoType);

   switch(nInfoType)
   {
      case WJED_INFO_TYPE_MESSAGE_BUFFER:
         return PNDEFGetMessageContent(pContext, hHandle,
            pBuffer, nBufferLength, pnActualLength);
      case WJED_INFO_TYPE_CONNECTION_PROPERTIES:
         *pnActualLength = nBufferLength;
         return PBasicGetConnectionProperties(pContext, hHandle,
            pBuffer, nBufferLength);

      case WJED_INFO_TYPE_CONNECTION_UID:
      {
         uint8_t* pInfo = null;

         *pnActualLength = 0;

         if((PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_3_A) == W_SUCCESS)
         || (PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_4_A) == W_SUCCESS))
         {
            tW14Part3ConnectionInfo sInfo14A;
            if(P14Part3GetConnectionInfo(
               pContext, hHandle, &sInfo14A) == W_SUCCESS)
            {
               *pnActualLength = sInfo14A.sW14TypeA.nUIDLength;
               pInfo = sInfo14A.sW14TypeA.aUID;
            }
         }
         else if((PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_PICOPASS_2K) == W_SUCCESS)
         || (PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_PICOPASS_32K) == W_SUCCESS))
         {
            tWPicoConnectionInfo sInfoPico;
            if(PPicoGetConnectionInfo(
               pContext, hHandle, &sInfoPico) == W_SUCCESS)
            {
               *pnActualLength = 8;
               pInfo = sInfoPico.UID;
            }
         }
         else if((PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_3_B) == W_SUCCESS)
         || (PBasicCheckConnectionProperty(pContext, hHandle, W_PROP_ISO_14443_4_B) == W_SUCCESS))
         {
            tW14Part3ConnectionInfo sInfo14B;
            if(P14Part3GetConnectionInfo(
               pContext, hHandle, &sInfo14B) == W_SUCCESS)
            {
               *pnActualLength = 4;
               pInfo = &(sInfo14B.sW14TypeB.aATQB[1]);
            }
         }
         else if(PBasicCheckConnectionProperty(
            pContext, hHandle, W_PROP_TYPE1_CHIP) == W_SUCCESS)
         {
            tWType1ChipConnectionInfo sInfoT1;
            if(PType1ChipGetConnectionInfo(
               pContext, hHandle, &sInfoT1) == W_SUCCESS)
            {
               *pnActualLength = 4;
               pInfo = sInfoT1.UID;
            }
         }
         else if(PBasicCheckConnectionProperty(
            pContext, hHandle, W_PROP_FELICA) == W_SUCCESS)
         {
            tWFeliCaConnectionInfo sInfoT3;
            if(PFeliCaGetConnectionInfo(
               pContext, hHandle, &sInfoT3) == W_SUCCESS)
            {
               *pnActualLength = 8;
               pInfo = sInfoT3.aManufacturerID;
            }
         }
         else if(PBasicCheckConnectionProperty(
            pContext, hHandle, W_PROP_ISO_15693_3) == W_SUCCESS)
         {
            tW15ConnectionInfo sInfo15;
            if(P15GetConnectionInfo(
               pContext, hHandle, &sInfo15) == W_SUCCESS)
            {
               *pnActualLength = 8;
               pInfo = sInfo15.UID;
            }
         }

         if(*pnActualLength != 0)
         {
            if(nBufferLength < *pnActualLength)
            {
               return W_ERROR_BUFFER_TOO_SHORT;
            }
            CMemoryCopy(pBuffer, pInfo, *pnActualLength);
         }
      }
      return W_SUCCESS;

      case WJED_INFO_TYPE_CONNECTIVITY_MESSAGE:
      {
         tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);

         if(pInstance->nLastConnectivityMessageTimeReference != 0)
         {
            *pnActualLength = pInstance->nLastConnectivityMessageLength;
            if(nBufferLength < *pnActualLength)
            {
               return W_ERROR_BUFFER_TOO_SHORT;
            }
            CMemoryCopy(pBuffer, pInstance->aLastConnectivityData, *pnActualLength);

            /* Then reset the message */
            pInstance->nLastConnectivityMessageLength = 0;
            pInstance->nLastConnectivityMessageTimeReference = 0;
            static_PJedTimerUpdate(pContext, pInstance);
         }
         else
         {
            *pnActualLength = 0;
         }
         return W_SUCCESS;
      }
      default:
         return PNDEFGetRecordInfoBuffer(pContext, hHandle, nInfoType,
            pBuffer, nBufferLength, pnActualLength);
   }
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedRetrieveConnection(
               tContext* pContext,
               W_HANDLE hConnection)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);

   PDebugTrace("PJedRetrieveConnection() hConnection=0x%08X", hConnection);

   if(hConnection == W_NULL_HANDLE)
   {
      hConnection = pInstance->hKeepAliveConnection;
      if(hConnection != W_NULL_HANDLE)
      {
         PDebugTrace("PJedRetrieveConnection() closing the keep alive connection");

         static_PJedCloseHandle(
               pContext, pInstance, hConnection, true, false, true);
      }
   }
   else if(pInstance->hKeepAliveConnection == hConnection)
   {
      pInstance->hKeepAliveConnection = W_NULL_HANDLE;
      pInstance->nKeepAliveConnectionTimeReference = 0;
      static_PJedTimerUpdate(pContext, pInstance);

      return W_SUCCESS;
   }

   return W_ERROR_BAD_HANDLE;
}

/* See the porting guide of Open NFC J-Edition */
void PJedCloseHandle(
               tContext* pContext,
               W_HANDLE hHandle)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);

   PDebugTrace("PJedCloseHandle() hHandle=0x%08X", hHandle);

   static_PJedCloseHandle(
               pContext, pInstance, hHandle, true, false, true);
}

/* See the porting guide of Open NFC J-Edition */
void PJedReaderHandlerWorkPerformed(
               tContext* pContext,
               W_HANDLE hConnection,
               bool bGiveToNextListener,
               bool bCardApplicationMatch )
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);

   PDebugTrace("PJedReaderHandlerWorkPerformed() hConnection=0x%08X", hConnection);

   static_PJedCloseHandle(
         pContext, pInstance, hConnection, bGiveToNextListener, bCardApplicationMatch, true);
}

/* See the porting guide of Open NFC J-Edition */
void PJedReleaseApplication(
               tContext* pContext,
               uint32_t nIsolate)
{
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedRegistryEntry** ppEntry;
   W_ERROR nError;

   PDebugTrace("PJedReleaseApplication() nIsolate=0x%08X", nIsolate);

   static_PJedCloseHandleByIsolate(pContext, pInstance, nIsolate);

   ppEntry = &pInstance->pRegistryListHead;

   while(*ppEntry != null)
   {
      if((((*ppEntry)->nRegistryType == WJED_TYPE_LISTEN_NDEF_MESSAGE)
      || ((*ppEntry)->nRegistryType == WJED_TYPE_LISTEN_UICC_EOT_MESSAGE)
      || ((*ppEntry)->nRegistryType == WJED_TYPE_LISTEN_SE_EOT_MESSAGE))
      && ((*ppEntry)->nApplicationIdentifier == nIsolate))
      {
         tPJedRegistryEntry* pEntry = *ppEntry;
         *ppEntry = pEntry->pNext;
         static_PJedDestroyRegistryEntry(pContext, pEntry);
      }
      else
      {
         ppEntry = &((*ppEntry)->pNext);
      }
   }

   nError = static_PJedUpdateRegistration(pContext, pInstance);

   if(nError != W_SUCCESS)
   {
      PDebugError("PJedReleaseApplication: static_PJedUpdateRegistration() returned %s",
      PUtilTraceError(nError));
   }
}

/* The structure of a buffer object */
typedef struct __tPJedBufferObject
{
   tHandleObjectHeader sHeader;
   uint32_t nParameter;
   uint32_t nLength;
   uint8_t sBuffer[1];
} tPJedBufferObject;

/**
 * @brief   Destroyes a buffer object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PJedDestroyBufferObject(
            tContext* pContext,
            void* pObject )
{
   PDebugTrace("static_PJedDestroyBufferObject");
   CMemoryFree( pObject );

   return P_HANDLE_DESTROY_DONE;
}

/* Buffer Object Type */
tHandleType g_sPJedBufferObjectType = { static_PJedDestroyBufferObject,
                                          null, null, null, null };

#define P_HANDLE_TYPE_WJED_BUFFER_OBJECT (&g_sPJedBufferObjectType)

/* See the porting guide of Open NFC J-Edition */
W_HANDLE PJedCreateBufferObject(
               tContext* pContext,
               uint32_t nIsolate,
               uint32_t nParameter,
               uint32_t nLength,
               uint8_t** ppContent)
{
   W_HANDLE hHandle = W_NULL_HANDLE;
   W_ERROR nError;
   tPJedBufferObject* pBufferObject;

   PDebugTrace("PJedCreateBufferObject: allocating buffer of %d bytes", nLength);

   if((pBufferObject = (tPJedBufferObject*)CMemoryAlloc(sizeof(tPJedBufferObject)- 1 + nLength)) == null)
   {
      PDebugError("PJedCreateBufferObject: Cannot allocate the buffer object");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_function;
   }

   pBufferObject->nParameter = nParameter;
   pBufferObject->nLength = nLength;

   nError = PHandleRegister(pContext,
            pBufferObject,
            P_HANDLE_TYPE_WJED_BUFFER_OBJECT,
            &hHandle);
   if(nError != W_SUCCESS)
   {
      PDebugError("PJedCreateBufferObject: cannot register the object");
      CMemoryFree(pBufferObject);
      goto return_function;
   }

   nError = PJedAddHandle(pContext, hHandle, nIsolate);
   if(nError != W_SUCCESS)
   {
      PDebugError("PJedCreateBufferObject: cannot register the handle");
      PHandleClose(pContext, hHandle);
      goto return_function;
   }

   if(ppContent != null)
   {
      *ppContent = pBufferObject->sBuffer;
   }

return_function:

   if(nError != W_SUCCESS)
   {
      PDebugError("PJedCreateBufferObject: Error %s", PUtilTraceError(nError));
      hHandle = W_NULL_HANDLE;
      if(ppContent != null)
      {
         *ppContent = null;
      }
   }

   return hHandle;
}

/* See the porting guide of Open NFC J-Edition */
W_ERROR PJedGetBufferContent(
               tContext* pContext,
               W_HANDLE hBufferObject,
               uint32_t* pnIsolate,
               uint32_t* pnParameter,
               uint8_t** ppContent,
               uint32_t* pnLength)
{
   W_ERROR nError;
   tPJedBufferObject* pBufferObject;

   PDebugTrace("PJedGetBufferContent()");

   nError = PHandleGetObject(pContext, hBufferObject,
            P_HANDLE_TYPE_WJED_BUFFER_OBJECT, (void**)&pBufferObject);

   if(nError != W_SUCCESS)
   {
      PDebugError("PJedGetBufferContent: Bad handle error %s", PUtilTraceError(nError));
      return nError;
   }

   nError = PJedCheckHandle(pContext, hBufferObject, pnIsolate);
   if(nError != W_SUCCESS)
   {
      PDebugError("PJedGetBufferContent: Non registered handle error %s", PUtilTraceError(nError));
      return nError;
   }

   if(ppContent != null)
   {
      *ppContent = pBufferObject->sBuffer;
   }

   if(pnParameter != null)
   {
      *pnParameter = pBufferObject->nParameter;
   }

   if(pnLength != null)
   {
      *pnLength = pBufferObject->nLength;
   }

   return W_SUCCESS;
}

void PJed14Part3ExchangeData(
               tContext* pContext,
               W_HANDLE hConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter,
               const uint8_t * pReaderToCardBuffer,
               uint32_t nReaderToCardBufferLength,
               uint8_t * pCardToReaderBuffer,
               uint32_t nCardToReaderBufferMaxLength,
               W_HANDLE * phOperation,
               bool bCheckResponseCRC )
{
   P14P3UserExchangeData(
      pContext,
      hConnection,
      pCallback, pCallbackParameter,
      pReaderToCardBuffer, nReaderToCardBufferLength,
      pCardToReaderBuffer, nCardToReaderBufferMaxLength,
      phOperation, bCheckResponseCRC );
}

W_ERROR PJedGetP2PPushSocket(
               tContext* pContext,
               const tchar* pPushEntry,
               W_HANDLE * phSocket)
{
   W_ERROR nError = W_SUCCESS;
   uint32_t nRegistryType;
   uint8_t nTNF;
   uint8_t nSAP;
   tchar* pTypeString;
   tPJedInstance* pInstance = PContextGetJEditionHelper(pContext);
   tPJedP2PRegistryEntry* pP2PEntry;

   uint8_t* pURLString = null;
   uint32_t URILength = 0;

   PDebugTrace("WJedGetP2PPushSocket()");

   URILength = PUtilConvertUTF16ToUTF8(null, pPushEntry, PUtilStringLength(pPushEntry));

   pURLString = CMemoryAlloc(URILength+1);
   if (pURLString == null)
   {
      PDebugError("WJedGetP2PPushSocket : unable to allocate UTF8 URI");
      return W_ERROR_OUT_OF_RESOURCE;
   }

   PUtilConvertUTF16ToUTF8(pURLString, pPushEntry, URILength);
   pURLString[URILength] = 0;

   /* Select the type of registry based on the URL */
   nError = static_PJedPushRegistryCheckURL(
         pContext,  pURLString,
         &nRegistryType, &nTNF, &pTypeString, &nSAP);

   CMemoryFree(pURLString);

   if(nError != W_SUCCESS)
   {
      PDebugError("WJedGetP2PPushSocket: Error returned during url check: %s", PUtilTraceError(nError));
      return nError;
   }

   /* Check if the entry is not already in the list */
   PDebugTrace("WJedGetP2PPushSocket(): Check if the entry is not already in the list");

   pP2PEntry = static_PJedFindP2PRegistryWithURI(pInstance, pTypeString);

   if (pP2PEntry != null)
   {
      /* the entry is in the list */
      CDebugAssert(pP2PEntry->hSocket != W_NULL_HANDLE);
      CDebugAssert(pP2PEntry->nConnectionTimeReference != 0);

      pP2PEntry->nConnectionTimeReference = 0;
      static_PJedTimerUpdate(pContext, pInstance);

      /* Duplicate the hSocket */
      PDebugTrace("WJedGetP2PPushSocket(): Duplicate the hSocket: %d = 0x%x",pP2PEntry->hSocket,pP2PEntry->hSocket);
      nError = PHandleDuplicate(pContext, pP2PEntry->hSocket, phSocket);
   }
   else
   {
      /* the entry is not in the list */
      PDebugWarning("WJedGetP2PPushSocket(): The entry is not in the list");
      nError = W_ERROR_BAD_STATE;
   }

   return nError;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
