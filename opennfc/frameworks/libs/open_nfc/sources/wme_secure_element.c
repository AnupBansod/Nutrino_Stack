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

/*******************************************************************************
   Contains the secure element implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( SE )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* See API Specification */
void PSEOpenConnection(
                  tContext* pContext,
                  uint32_t nSlotIdentifier,
                  bool bForce,
                  tPBasicGenericHandleCallbackFunction* pCallback,
                  void* pCallbackParameter )
{
   W_ERROR nError;
   tDFCCallbackContext sErrorCallback;

   PDebugTrace("PSEOpenConnection()");

   PSEDriverOpenConnection(
            pContext, nSlotIdentifier, bForce, pCallback, pCallbackParameter);

   if ((nError = PContextGetLastIoctlError(pContext)) == W_SUCCESS)
   {
      return;
   }

   PDebugError("PSEOpenConnection: return the error %s", PUtilTraceError(nError));

   PDFCFillCallbackContext( pContext,
      (tDFCCallback*)pCallback, pCallbackParameter,
      &sErrorCallback );

   PDFCPostContext3( &sErrorCallback, P_DFC_TYPE_SECURE_ELEMENT,
      W_NULL_HANDLE, nError );
}

/* See API Specification */
W_ERROR PSEGetInfo(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            tSEInfo* pSEInfo )
{
   PDebugTrace("PSEGetInfo()");

   if (pSEInfo)
   {
      return PSEDriverGetInfo(pContext, nSlotIdentifier, pSEInfo, sizeof(tSEInfo));
   }
   else
   {
      return W_ERROR_BAD_PARAMETER;
   }
}

/* See API Specifications */
W_ERROR WSEOpenConnectionSync(
            uint32_t nSlotIdentifier,
            bool bForce,
            W_HANDLE* phConnection)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WSEOpenConnection( nSlotIdentifier, bForce, PBasicGenericSyncCompletionHandle, &param );
   }

   return PBasicGenericSyncWaitForResultHandle(&param, phConnection);
}

W_ERROR WSESetPolicySync(
            uint32_t nSlotIdentifier,
            uint32_t nStorageType,
            uint32_t nProtocols)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WSESetPolicy( nSlotIdentifier, nStorageType, nProtocols, PBasicGenericSyncCompletion, &param );
   }

   return PBasicGenericSyncWaitForResult(&param);
}


static void static_PSESetPolicyDriverCompleted(
   tContext * pContext,
   void * pCallbackParameter,
   W_ERROR nError)
{
   PDFCPostContext2(pCallbackParameter, P_DFC_TYPE_READER, nError);
   CMemoryFree(pCallbackParameter);
}


void PSESetPolicy(
      tContext* pContext,
      uint32_t nSlotIdentifier,
      uint32_t nStorageType,
      uint32_t nProtocols,
      tPBasicGenericCallbackFunction * pCallback,
      void * pCallbackParameter )
{
   tDFCCallbackContext * pCallbackContext = (tDFCCallbackContext *)CMemoryAlloc(sizeof(tDFCCallbackContext));
   W_ERROR nError;

   if (pCallbackContext != null)
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);

      PSESetPolicyDriver(pContext, nSlotIdentifier, nStorageType, nProtocols, static_PSESetPolicyDriverCompleted, pCallbackContext);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_SECURE_ELEMENT, static_PSESetPolicyDriverCompleted, pCallbackContext, nError);
      }
   }
   else
   {
      tDFCCallbackContext sCallbackContext;

      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &sCallbackContext);
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_SECURE_ELEMENT, W_ERROR_OUT_OF_RESOURCE);
   }

}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* The watchdog timeout in ms */
#define P_SE_WATCHDOG_TIMEOUT   1000

static P_INLINE uint32_t static_PSESwitchGetPolicy(
            const tNFCControllerSEPolicy* pPolicy,
            uint32_t nSlotIdentifier)
{
   if(pPolicy->nSlotIdentifier == nSlotIdentifier)
   {
      return pPolicy->nSEProtocolPolicy;
   }

   return 0;
}

static void static_PSEMonitorEndOfTransactionEventReceived(
         tContext* pContext,
         void* pCallbackParameter,
         uint8_t nEventIdentifier,
         const uint8_t* pBuffer,
         uint32_t nLength,
         uint32_t nNALMessageReceptionCounter);

static void static_PSEMonitorHotPlugEventReceived(
         tContext* pContext,
         void* pCallbackParameter,
         uint8_t nEventIdentifier,
         const uint8_t* pBuffer,
         uint32_t nLength,
         uint32_t nNALMessageReceptionCounter);

/** See the specification tPBasicExchangeData */
static void static_PSEDriverExchangeApdu(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericDataCallbackFunction* pCallback,
            void* pCallbackParameter,
            const uint8_t* pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength,
            uint8_t* pCardToReaderBuffer,
            uint32_t nCardToReaderBufferMaxLength,
            W_HANDLE* phOperation )
{
   W_HANDLE hOperation = P14P4DriverExchangeDataInternal(
            pContext,
            hConnection,
            pCallback, pCallbackParameter,
            pReaderToCardBuffer, nReaderToCardBufferLength,
            pCardToReaderBuffer, nCardToReaderBufferMaxLength,
            false, 0,
            false);

   if(phOperation != null)
   {
      *phOperation = hOperation;
   }
   else
   {
      PHandleClose(pContext, hOperation);
   }
}

#ifdef P_SE_DEFAULT_PRINCIPAL_0
static const uint8_t g_aPSEDefaultPrincipal0[] = { P_SE_DEFAULT_PRINCIPAL_0 };
#define P_SE_DEFAULT_PRINCIPAL_0_BUFFER g_aPSEDefaultPrincipal0
#define P_SE_DEFAULT_PRINCIPAL_0_LENGTH sizeof(g_aPSEDefaultPrincipal0)
#else
#define P_SE_DEFAULT_PRINCIPAL_0_BUFFER null
#define P_SE_DEFAULT_PRINCIPAL_0_LENGTH 0
#endif /* P_SE_DEFAULT_PRINCIPAL_0 */

#ifdef P_SE_DEFAULT_PRINCIPAL_1
static const uint8_t g_aPSEDefaultPrincipal1[] = { P_SE_DEFAULT_PRINCIPAL_1 };
#define P_SE_DEFAULT_PRINCIPAL_1_BUFFER g_aPSEDefaultPrincipal1
#define P_SE_DEFAULT_PRINCIPAL_1_LENGTH sizeof(g_aPSEDefaultPrincipal1)
#else
#define P_SE_DEFAULT_PRINCIPAL_1_BUFFER null
#define P_SE_DEFAULT_PRINCIPAL_1_LENGTH 0
#endif /* P_SE_DEFAULT_PRINCIPAL_1 */

#ifdef P_SE_DEFAULT_PRINCIPAL_2
static const uint8_t g_aPSEDefaultPrincipal2[] = { P_SE_DEFAULT_PRINCIPAL_2 };
#define P_SE_DEFAULT_PRINCIPAL_2_BUFFER g_aPSEDefaultPrincipal2
#define P_SE_DEFAULT_PRINCIPAL_2_LENGTH sizeof(g_aPSEDefaultPrincipal2)
#else
#define P_SE_DEFAULT_PRINCIPAL_2_BUFFER null
#define P_SE_DEFAULT_PRINCIPAL_2_LENGTH 0
#endif /* P_SE_DEFAULT_PRINCIPAL_2 */

#ifdef P_SE_DEFAULT_PRINCIPAL_3
static const uint8_t g_aPSEDefaultPrincipal3[] = { P_SE_DEFAULT_PRINCIPAL_3 };
#define P_SE_DEFAULT_PRINCIPAL_3_BUFFER g_aPSEDefaultPrincipal3
#define P_SE_DEFAULT_PRINCIPAL_3_LENGTH sizeof(g_aPSEDefaultPrincipal3)
#else
#define P_SE_DEFAULT_PRINCIPAL_3_BUFFER null
#define P_SE_DEFAULT_PRINCIPAL_3_LENGTH 0
#endif /* P_SE_DEFAULT_PRINCIPAL_3 */

struct __tPSEDefaultPrincipal
{
   const uint8_t* pBuffer;
   uint32_t nLength;
};

static struct __tPSEDefaultPrincipal g_aPSEDefaultPrincipalArray[NAL_MAXIMUM_SE_NUMBER] = {
   { P_SE_DEFAULT_PRINCIPAL_0_BUFFER, P_SE_DEFAULT_PRINCIPAL_0_LENGTH },
   { P_SE_DEFAULT_PRINCIPAL_1_BUFFER, P_SE_DEFAULT_PRINCIPAL_1_LENGTH },
   { P_SE_DEFAULT_PRINCIPAL_2_BUFFER, P_SE_DEFAULT_PRINCIPAL_2_LENGTH },
   { P_SE_DEFAULT_PRINCIPAL_3_BUFFER, P_SE_DEFAULT_PRINCIPAL_3_LENGTH },
};

/**
 * Builds the SE information.
 *
 * @param[in]  pContext  The current context
 *
 * @param[in]  pSEInstance  The SE instance
 *
 * @return  The result error code.
 **/
static W_ERROR static_PSEDriverBuildInfo(
            tContext* pContext,
            tSEInstance* pSEInstance)
{
   W_ERROR nError = W_SUCCESS;

   if (PNFCControllerIsActive(pContext) == false)
   {
      PDebugError("static_PSEDriverBuildInfo: bad NFC Controller mode");
      nError = W_ERROR_BAD_NFCC_MODE;
      goto return_function;
   }

   if(pSEInstance->bSEInfoIsBuilt == false)
   {
      uint32_t nSENumber = PNFCControllerGetSecureElementNumber(pContext);
      uint32_t nSlotIdentifier;
      tSESlot* pSlot;
      tNFCControllerSEPolicy sPersistentPolicy;
      tNFCControllerSEPolicy sVolatilePolicy;

      pSEInstance->nSENumber = nSENumber;

      nError = PNFCControllerGetSESwitchPosition(pContext,
         &sPersistentPolicy, &sVolatilePolicy, null, null);
      if(nError != W_SUCCESS)
      {
         PDebugError("static_PSEDriverBuildInfo: Error %s returned by PNFCControllerGetSESwitchPosition()",
            PUtilTraceError(nError));
         goto return_function;
      }

      for(nSlotIdentifier = 0; nSlotIdentifier < nSENumber; nSlotIdentifier++)
      {
         tSEInfo* pConstantInfo;
         W_ERROR nError;
         uint32_t nConstantProtocols;

         pSlot = &pSEInstance->aSlotArray[nSlotIdentifier];

         pConstantInfo = &pSlot->sConstantInfo;
         nError = PNFCControllerGetSecureElementHardwareInfo(pContext, nSlotIdentifier,
            pConstantInfo->aDescription,
            &pConstantInfo->nCapabilities,
            &nConstantProtocols);

         if(nError != W_SUCCESS)
         {
            PDebugError("static_PSEDriverBuildInfo: Error %s returned by PNFCControllerGetSecureElementHardwareInfo()",
               PUtilTraceError(nError));
            goto return_function;
         }

         pSlot->nConstantProtocols = nConstantProtocols;

         pConstantInfo->nSlotIdentifier = nSlotIdentifier;

         if((pConstantInfo->nCapabilities & W_SE_FLAG_REMOVABLE) != 0)
         {
            /* @todo: set presence flag for removable SEs */
            pConstantInfo->bIsPresent = false;
         }
         else
         {
            /* The flag is always set to true for a non-removable SE. */
            pConstantInfo->bIsPresent = true;
         }

         pConstantInfo->nSupportedProtocols = nConstantProtocols & W_NFCC_PROTOCOL_CARD_ALL;

         pConstantInfo->nPersistentPolicy = static_PSESwitchGetPolicy(&sPersistentPolicy, nSlotIdentifier);

         pConstantInfo->nVolatilePolicy = static_PSESwitchGetPolicy(&sVolatilePolicy, nSlotIdentifier);

         /* Activate the security stack */
         if((pConstantInfo->nCapabilities & W_SE_FLAG_COMMUNICATION) != 0)
         {
            pSlot->pSecurityStackInstance = PSecurityStackCreateInstance(
               pContext,
               nSlotIdentifier,
               g_aPSEDefaultPrincipalArray[nSlotIdentifier].pBuffer,
               g_aPSEDefaultPrincipalArray[nSlotIdentifier].nLength);

            if(pSlot->pSecurityStackInstance == null)
            {
               PDebugError("static_PSEDriverBuildInfo: Error returned by PSecurityStackCreateInstance()");
               nError = W_ERROR_OUT_OF_RESOURCE;
               goto return_function;
            }
         }
      }

      PNALServiceRegisterForEvent(
         pContext,
         NAL_SERVICE_SECURE_ELEMENT,
         NAL_EVT_SE_CARD_EOT,
         &pSEInstance->sEndOfTransactionOperation,
         static_PSEMonitorEndOfTransactionEventReceived,
         pSEInstance );

      PNALServiceRegisterForEvent(
         pContext,
         NAL_SERVICE_SECURE_ELEMENT,
         NAL_EVT_SE_HOT_PLUG,
         &pSEInstance->sHotPlugOperation,
         static_PSEMonitorHotPlugEventReceived,
         pSEInstance );
   }

return_function:

   pSEInstance->bSEInfoIsBuilt = true;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverBuildInfo: returning error %s", PUtilTraceError(nError));

      PDebugWarning("static_PSEDriverBuildInfo: Deactivating the Secure Element(s)");
      pSEInstance->nSENumber = 0;
   }

   return nError;
}

/* See header file */
W_ERROR PSEDriverGetInfo(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            tSEInfo* pSEInfo,
            uint32_t nSize )
{
   W_ERROR nError = W_SUCCESS;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tNFCControllerSEPolicy sPersistentPolicy;
   tNFCControllerSEPolicy sVolatilePolicy;

   PDebugTrace("PSEDriverGetInfo()");

   if((pSEInfo == null) || (nSize != sizeof(tSEInfo)))
   {
      PDebugError("PSEDriverGetInfo: wrong buffer parameters");
      nError = W_ERROR_BAD_PARAMETER;
      pSEInfo = null;
      goto return_function;
   }

   CDebugAssert(pSEInstance->bSEInfoIsBuilt == true);

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("PSEDriverGetInfo: wrong slot identifier");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_function;
   }

   CMemoryCopy(pSEInfo, &pSEInstance->aSlotArray[nSlotIdentifier].sConstantInfo, sizeof(tSEInfo));

   nError = PNFCControllerGetSESwitchPosition(pContext,
      &sPersistentPolicy, &sVolatilePolicy, null, null);
   if(nError != W_SUCCESS)
   {
      PDebugError("PSEDriverGetInfo: error returned by PNFCControllerGetSESwitchPosition");
      goto return_function;
   }

   pSEInfo->nPersistentPolicy = static_PSESwitchGetPolicy(&sPersistentPolicy, nSlotIdentifier);
   pSEInfo->nVolatilePolicy = static_PSESwitchGetPolicy(&sVolatilePolicy, nSlotIdentifier);

return_function:

   if(nError != W_SUCCESS)
   {
      PDebugError("PSEDriverGetInfo: error %s returned", PUtilTraceError(nError));
      if(pSEInfo != null)
      {
         CMemoryFill(pSEInfo, 0, sizeof(tSEInfo));
      }
   }

   return nError;
}

/* See header file */
void PSECreate(
            tSEInstance* pSEInstance )
{
   CMemoryFill( pSEInstance, 0, sizeof(tSEInstance) );

   pSEInstance->bSEInfoIsBuilt = false;
}

/* See header file */
void PSEDriverResetData(
            tContext* pContext )
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );

   if(pSEInstance->bSEInfoIsBuilt != false)
   {
      uint32_t nSlotIdentifier;

      for(nSlotIdentifier = 0; nSlotIdentifier < pSEInstance->nSENumber; nSlotIdentifier++)
      {
         if((pSEInstance->aSlotArray[nSlotIdentifier].sConstantInfo.nCapabilities & W_SE_FLAG_COMMUNICATION) != 0)
         {
            PSecurityStackDestroyInstance(
                  pContext,
                  pSEInstance->aSlotArray[nSlotIdentifier].pSecurityStackInstance );

            pSEInstance->aSlotArray[nSlotIdentifier].pSecurityStackInstance = null;
         }
      }
   }

   CMemoryFill( pSEInstance, 0, sizeof(tSEInstance) );

   pSEInstance->bSEInfoIsBuilt = false;
}

/* See header file */
void PSEDestroy(
            tSEInstance* pSEInstance )
{
   if ( pSEInstance != null )
   {
      CMemoryFill( pSEInstance, 0, sizeof(tSEInstance) );

      pSEInstance->bSEInfoIsBuilt = false;
   }
}

/* The pending operation flags */
#define  P_SE_FLAG_PENDING_SET_POLICY              0x0002
#define  P_SE_FLAG_PENDING_EOT_REGITRATION         0x0004
#define  P_SE_FLAG_PENDING_HOTPLUG_REGITRATION     0x0008
#define  P_SE_FLAG_PENDING_COMMUNICATION           0x0010
#define  P_SE_FLAG_PENDING_COMMUNICATION_FROM_USER 0x0020

/* Destroy the monitor end-of-transaction event registry */
static uint32_t static_PSEDestroyMonitorEndOfTransaction(
            tContext* pContext,
            void* pObject )
{
   tSESlot* pSlot = P_HANDLE_GET_STRUCTURE(tSESlot, pObject, sEndOfTransactionRegistry);

   CDebugAssert((pSlot->nPendingFlags & P_SE_FLAG_PENDING_EOT_REGITRATION) != 0);

   PDFCDriverFlushCall(pSlot->pEndOfTransactionHandlerDriverCC);

   pSlot->nPendingFlags &= ~P_SE_FLAG_PENDING_EOT_REGITRATION;

   return P_HANDLE_DESTROY_DONE;
}

/* monitor end-of-transaction registry type */
tHandleType g_sSEMonitorEndOfTransaction = { static_PSEDestroyMonitorEndOfTransaction, null, null, null, null };

#define P_HANDLE_TYPE_SE_MONITOR_EOT (&g_sSEMonitorEndOfTransaction)

static void static_PSEMonitorEndOfTransactionEventReceived(
         tContext* pContext,
         void* pCallbackParameter,
         uint8_t nEventIdentifier,
         const uint8_t* pBuffer,
         uint32_t nLength,
         uint32_t nNALMessageReceptionCounter)
{
   tSEInstance* pSEInstance = (tSEInstance*)pCallbackParameter;
   tSESlot* pSlot;
   uint32_t nCardProtocols, nSlotIdentifier;

   if((nEventIdentifier != NAL_EVT_SE_CARD_EOT)||(pBuffer == null)||(nLength < 3))
   {
      PDebugError("static_PSEMonitorEndOfTransactionEventReceived: Protocol error");
      /* Protocol error: can't do anything about it */
      return;
   }

   nSlotIdentifier = pBuffer[0];
   nCardProtocols = PNALReadCardProtocols(&pBuffer[1]);

   PDebugTrace("static_PSEMonitorEndOfTransactionEventReceived(slot #%d, %s)",
      nSlotIdentifier, PUtilTraceCardProtocol(pContext, nCardProtocols));

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("static_PSEMonitorEndOfTransactionEventReceived: Protocol error in the slot value");
      /* Protocol error: can't do anything about it */
      return;
   }

   pSlot = &pSEInstance->aSlotArray[nSlotIdentifier];

   /* Copy the AID value if any */
   if(nLength > 3)
   {
      nLength -= 3;
      if(nLength > P_SE_MAX_AID_LENGTH)
      {
         PDebugError("static_PSEMonitorEndOfTransactionEventReceived: AID length too large, truncated");
         nLength = P_SE_MAX_AID_LENGTH;
      }
      pSlot->nLastAIDLength = nLength;
      CMemoryCopy(pSlot->aLastAID, &pBuffer[3], nLength);
   }
   else
   {
      pSlot->nLastAIDLength = 0;
   }

   if((pSlot->nPendingFlags & P_SE_FLAG_PENDING_EOT_REGITRATION) != 0)
   {
      PDFCDriverPostCC3( pSlot->pEndOfTransactionHandlerDriverCC,
         P_DFC_TYPE_SECURE_ELEMENT, nSlotIdentifier, nCardProtocols );
   }
}

/* See client API specification */
W_ERROR PSEMonitorEndOfTransaction(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            tPBasicGenericEventHandler2* pHandler,
            void* pHandlerParameter,
            W_HANDLE* phEventRegistry)
{
   W_ERROR nError = W_SUCCESS;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tSESlot* pSlot;

   PDebugTrace("PSEMonitorEndOfTransaction()");

   if((phEventRegistry == null) /* || (pHandler == null) @todo: fix autogen */)
   {
      PDebugError("PSEMonitorEndOfTransaction: bad parameters");
      return W_ERROR_BAD_PARAMETER;
   }

   CDebugAssert(pSEInstance->bSEInfoIsBuilt == true);

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("PSEMonitorEndOfTransaction: Wrong SE slot identifier");
      return W_ERROR_BAD_PARAMETER;
      goto return_error;
   }
   pSlot = &pSEInstance->aSlotArray[nSlotIdentifier];

   /* Check the presence of SE and the EOT detection capability */
   if((pSlot->sConstantInfo.nCapabilities & W_SE_FLAG_END_OF_TRANSACTION_NOTIFICATION) == 0)
   {
      PDebugWarning("PSEMonitorEndOfTransaction: End of transaction event not supported");
      nError = W_ERROR_FEATURE_NOT_SUPPORTED;
      goto return_error;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if((pSlot->nPendingFlags & P_SE_FLAG_PENDING_EOT_REGITRATION) != 0)
   {
      PDebugError("PSEMonitorEndOfTransaction: service already used");
      nError = W_ERROR_EXCLUSIVE_REJECTED;
      goto return_error;
   }

   if((nError = PHandleRegister(pContext,
            &pSlot->sEndOfTransactionRegistry,
            P_HANDLE_TYPE_SE_MONITOR_EOT, phEventRegistry)) != W_SUCCESS)
   {
      PDebugError("PSEMonitorEndOfTransaction: error registering the monitor object");
      goto return_error;
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pHandler, pHandlerParameter,
      &pSlot->pEndOfTransactionHandlerDriverCC );

   pSlot->nPendingFlags |= P_SE_FLAG_PENDING_EOT_REGITRATION;

   return W_SUCCESS;

return_error:

   PDebugError("PSEMonitorEndOfTransaction: return the error %s", PUtilTraceError(nError));

   *phEventRegistry = W_NULL_HANDLE;
   return nError;
}

/* See Client API Specification */
uint32_t PSEGetTransactionAID(
                  tContext* pContext,
                  uint32_t nSlotIdentifier,
                  uint8_t* pBuffer,
                  uint32_t nBufferLength)
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tSESlot* pSlot;
   uint32_t nLength;

   CDebugAssert(pSEInstance->bSEInfoIsBuilt == true);

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("PSEGetTransactionAID: Wrong SE slot identifier");
      return W_ERROR_BAD_PARAMETER;
   }
   pSlot = &pSEInstance->aSlotArray[nSlotIdentifier];

   nLength = pSlot->nLastAIDLength;

   if((pBuffer != null) && (nLength <= nBufferLength))
   {
      CMemoryCopy(pBuffer, pSlot->aLastAID, nLength);
   }

   return nLength;
}

/* Destroy the monitor hot-plug event registry */
static uint32_t static_PSEDestroyMonitorHotPlug(
            tContext* pContext,
            void* pObject )
{
   tSESlot* pSlot = P_HANDLE_GET_STRUCTURE(tSESlot, pObject, sHotPlugRegistry);

   CDebugAssert((pSlot->nPendingFlags & P_SE_FLAG_PENDING_HOTPLUG_REGITRATION) != 0);

   PDFCDriverFlushCall(pSlot->pHotPlugHandlerDriverCC);

   pSlot->nPendingFlags &= ~P_SE_FLAG_PENDING_HOTPLUG_REGITRATION;

   return P_HANDLE_DESTROY_DONE;
}

/* monitor hot-plug registry type */
tHandleType g_sSEMonitorHotPlug = { static_PSEDestroyMonitorHotPlug, null, null, null, null };

#define P_HANDLE_TYPE_SE_MONITOR_HOTPLUG (&g_sSEMonitorHotPlug)

static void static_PSEMonitorHotPlugEventReceived(
         tContext* pContext,
         void* pCallbackParameter,
         uint8_t nEventIdentifier,
         const uint8_t* pBuffer,
         uint32_t nLength,
         uint32_t nNALMessageReceptionCounter)
{
   tSEInstance* pSEInstance = (tSEInstance*)pCallbackParameter;
   tSESlot* pSlot;
   uint32_t nSlotIdentifier;
   uint8_t nEventCode;

   if((nEventIdentifier != NAL_EVT_SE_HOT_PLUG)||(pBuffer == null)||(nLength != 2))
   {
      PDebugError("static_PSEMonitorHotPlugEventReceived: Protocol error");
      /* Protocol error: can't do anything about it */
      return;
   }

   nSlotIdentifier = pBuffer[0];
   nEventCode = pBuffer[1];

   PDebugTrace("static_PSEMonitorHotPlugEventReceived(slot #%d, %d)",
      nSlotIdentifier, nEventCode);

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("static_PSEMonitorHotPlugEventReceived: Protocol error in the slot value");
      /* Protocol error: can't do anything about it */
      return;
   }

   pSlot = &pSEInstance->aSlotArray[nSlotIdentifier];

   if((pSlot->nPendingFlags & P_SE_FLAG_PENDING_HOTPLUG_REGITRATION) != 0)
   {
      PDFCDriverPostCC3( pSlot->pHotPlugHandlerDriverCC,
         P_DFC_TYPE_SECURE_ELEMENT, nSlotIdentifier,
         (nEventCode == 0)?W_SE_EVENT_REMOVED:W_SE_EVENT_INSERTED );
   }
}

/* See client API specification */
W_ERROR PSEMonitorHotPlugEvents(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            tPBasicGenericEventHandler2* pHandler,
            void* pHandlerParameter,
            W_HANDLE* phEventRegistry)
{
   W_ERROR nError = W_SUCCESS;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tSESlot* pSlot;

   PDebugTrace("PSEMonitorHotPlugEvents()");

   if((phEventRegistry == null) || (pHandler == null))
   {
      PDebugError("PSEMonitorHotPlugEvents: bad parameters");
      return W_ERROR_BAD_PARAMETER;
   }

   CDebugAssert(pSEInstance->bSEInfoIsBuilt == true);

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("PSEMonitorHotPlugEvents: Wrong SE slot identifier");
      return W_ERROR_BAD_PARAMETER;
      goto return_error;
   }
   pSlot = &pSEInstance->aSlotArray[nSlotIdentifier];

   /* Check the hot-plug detection capability */
   if((pSlot->sConstantInfo.nCapabilities & W_SE_FLAG_HOT_PLUG) == 0)
   {
      PDebugWarning("PSEMonitorHotPlugEvents: Hot-plug event not supported");
      nError = W_ERROR_FEATURE_NOT_SUPPORTED;
      goto return_error;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if((pSlot->nPendingFlags & P_SE_FLAG_PENDING_HOTPLUG_REGITRATION) != 0)
   {
      PDebugError("PSEMonitorHotPlugEvents: service already used");
      nError = W_ERROR_EXCLUSIVE_REJECTED;
      goto return_error;
   }

   if((nError = PHandleRegister(pContext,
            &pSlot->sHotPlugRegistry,
            P_HANDLE_TYPE_SE_MONITOR_HOTPLUG, phEventRegistry)) != W_SUCCESS)
   {
      PDebugError("PSEMonitorHotPlugEvents: error registering the monitor object");
      goto return_error;
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pHandler, pHandlerParameter,
      &pSlot->pHotPlugHandlerDriverCC );

   pSlot->nPendingFlags |= P_SE_FLAG_PENDING_HOTPLUG_REGITRATION;

   return W_SUCCESS;

return_error:

   PDebugError("PSEMonitorHotPlugEvents: return the error %s", PUtilTraceError(nError));

   *phEventRegistry = W_NULL_HANDLE;
   return nError;
}

static void static_PSESetSESwitchPositionCompleted(
         tContext* pContext,
         void* pCallbackParameter,
         W_ERROR nError)
{
   tSEInstance* pSEInstance = (tSEInstance*)pCallbackParameter;

   CDebugAssert((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_SET_POLICY) != 0);
   pSEInstance->nGlobalPendingFlags &= ~P_SE_FLAG_PENDING_SET_POLICY;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSESetSESwitchPositionCompleted: return the error %s", PUtilTraceError(nError));
   }

   PDFCDriverPostCC2( pSEInstance->pSetPolicyDriverCC,
      P_DFC_TYPE_SECURE_ELEMENT, nError );
}

/* See header file */
void PSESetPolicyDriver(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            uint32_t nStorageType,
            uint32_t nProtocols,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter)
{
   tDFCDriverCCReference pErrorCC;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   W_ERROR nError = W_SUCCESS;
   tNFCControllerSEPolicy sNewPolicy;

   PDebugTrace("PSESetPolicy( nSlotIdentifier=%d, nStorageType=%d, nProtocols=%08x)",
      nSlotIdentifier, nStorageType, nProtocols);

   CDebugAssert(pSEInstance->bSEInfoIsBuilt == true);

   /* Check the arguments */
   if((nStorageType != W_NFCC_STORAGE_VOLATILE)
   && (nStorageType != W_NFCC_STORAGE_PERSISTENT)
   && (nStorageType != (W_NFCC_STORAGE_PERSISTENT | W_NFCC_STORAGE_VOLATILE)))
   {
      PDebugError("PSESetPolicy: Bad storage parameters");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("PSESetPolicy: Bad slot identifier");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   /* check the validity of the nProtocols parameter */
   if((nProtocols | W_NFCC_PROTOCOL_CARD_ALL) != W_NFCC_PROTOCOL_CARD_ALL)
   {
      PDebugError("Bad SE policy parameter");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   if((pSEInstance->aSlotArray[nSlotIdentifier].nConstantProtocols & nProtocols) != nProtocols)
   {
      PDebugError("Bad SE policy parameter");
      nError = W_ERROR_FEATURE_NOT_SUPPORTED;
      goto send_event;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_SET_POLICY) != 0)
   {
      PDebugError("PSESetPolicy: operation already active");
      nError = W_ERROR_BAD_STATE;
      goto send_event;
   }

   if((nStorageType & W_NFCC_STORAGE_VOLATILE) != 0)
   {
      /* Check if a communication is pending */
      if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION) != 0)
      {
         nStorageType &= ~W_NFCC_STORAGE_VOLATILE;

         if(nProtocols == 0)
         {
            if((pSEInstance->sAfterComPolicy.nSlotIdentifier != nSlotIdentifier)
            || (pSEInstance->sAfterComPolicy.nSESwitchPosition == P_SE_SWITCH_OFF))
            {
               PDebugTrace("The SE #%d is already OFF", nSlotIdentifier);
               goto check_next2;
            }

            pSEInstance->sAfterComPolicy.nSlotIdentifier = 0;
            pSEInstance->sAfterComPolicy.nSESwitchPosition = P_SE_SWITCH_OFF;
            pSEInstance->sAfterComPolicy.nSEProtocolPolicy = 0;
         }
         else
         {
            pSEInstance->sAfterComPolicy.nSlotIdentifier = nSlotIdentifier;
            pSEInstance->sAfterComPolicy.nSESwitchPosition = P_SE_SWITCH_RF_INTERFACE;
            pSEInstance->sAfterComPolicy.nSEProtocolPolicy = nProtocols;
         }

         /* Send a success event */
         goto check_next2;
      }
   }

check_next2:

   if(nStorageType != 0)
   {
      if(nProtocols == 0)
      {
         sNewPolicy.nSlotIdentifier = 0;
         sNewPolicy.nSESwitchPosition = P_SE_SWITCH_OFF;
         sNewPolicy.nSEProtocolPolicy = 0;
      }
      else
      {
         sNewPolicy.nSlotIdentifier = nSlotIdentifier;
         sNewPolicy.nSESwitchPosition = P_SE_SWITCH_RF_INTERFACE;
         sNewPolicy.nSEProtocolPolicy = nProtocols;
      }

      pSEInstance->nGlobalPendingFlags |= P_SE_FLAG_PENDING_SET_POLICY;

      PDFCDriverFillCallbackContext( pContext,
         (tDFCCallback*)pCallback, pCallbackParameter,
         &pSEInstance->pSetPolicyDriverCC );

      PNFCControllerSetSESwitchPosition(
            pContext, nStorageType, &sNewPolicy,
            static_PSESetSESwitchPositionCompleted, pSEInstance);

      return;
   }

send_event:

   if(nError != W_SUCCESS)
   {
      PDebugError("PSESetPolicy: Error %s returned", PUtilTraceError(nError));
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pCallback, pCallbackParameter,
      &pErrorCC );

   PDFCDriverPostCC2( pErrorCC, P_DFC_TYPE_SECURE_ELEMENT, nError );
}


/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

         OPEN HOST CONNECTION

   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */


/* Callback invoked when the SE switch is set back to the original position */
static void static_PSEDriverOpenConnectionSendError(
         tContext* pContext,
         tSESlot* pSlot)
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );

   PDebugTrace("static_PSEDriverOpenConnectionSendError()");

   CDebugAssert((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION) != 0);


   PReaderDriverEnableAllNonSEListeners(pContext);

   pSEInstance->nGlobalPendingFlags &= ~P_SE_FLAG_PENDING_COMMUNICATION;

   if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION_FROM_USER) != 0)
   {
      PDFCDriverPostCC3( pSlot->pOpenConnectionDriverCC,
         P_DFC_TYPE_SECURE_ELEMENT, W_NULL_HANDLE, pSlot->nOpenConnectionFirstError);
   }
   else
   {
      PDFCPostContext3(&(pSlot->sOpenConnectionCC),
         P_DFC_TYPE_SECURE_ELEMENT, W_NULL_HANDLE, pSlot->nOpenConnectionFirstError);
   }
}


/* Callback invoked when the SE switch is set back to the original position */
static void static_PSEDriverOpenConnectionSetBackSESwitchPositionCompleted(
         tContext* pContext,
         void* pCallbackParameter,
         W_ERROR nError)
{
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;

   PDebugTrace("static_PSEDriverOpenConnectionSetBackSESwitchPositionCompleted()");

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverOpenConnectionSetBackSESwitchPositionCompleted: Error %s returned",
         PUtilTraceError(nError));

      if(pSlot->nOpenConnectionFirstError == W_SUCCESS)
      {
         pSlot->nOpenConnectionFirstError = nError;
      }
   }

   static_PSEDriverOpenConnectionSendError(pContext, pSlot);
}

/* Set the switch back */
static void static_PSEDriverOpenConnectionSetBackSESwitchPosition(
            tContext* pContext,
            tSEInstance* pSEInstance,
            tSESlot* pSlot,
            W_ERROR nError )
{
   PDebugTrace("static_PSEDriverOpenConnectionSetBackSESwitchPosition()");

   if(pSlot->nOpenConnectionFirstError == W_SUCCESS)
   {
      pSlot->nOpenConnectionFirstError = nError;
   }

   PNFCControllerSetSESwitchPosition(pContext,
      W_NFCC_STORAGE_VOLATILE, &pSEInstance->sAfterComPolicy,
      static_PSEDriverOpenConnectionSetBackSESwitchPositionCompleted, pSlot);
}

/**
 * @brief   Destroy callback function.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The callback parameter.
 **/
static void static_PSEDriverDestroyConnectionCallback(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );

   PDebugTrace("static_PSEDriverDestroyConnectionCallback()");

   CDebugAssert((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION) != 0);

   pSEInstance->nGlobalPendingFlags &= ~P_SE_FLAG_PENDING_COMMUNICATION;

   PReaderDriverEnableAllNonSEListeners(pContext);

   PDFCPostContext2(&pSlot->sDestroyCallback, P_DFC_TYPE_SECURE_ELEMENT, nError);
}

/**
 * @brief   Destroyes a SE connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PSEDriverDestroyConnectionAsync(
            tContext* pContext,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            void* pObject )
{
   tSESlot* pSlot = (tSESlot*)pObject;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );

   PDebugTrace("static_PSEDriverDestroyConnectionAsync");

   if(pSlot->bFirstDestroyCalled == false)
   {
      /* Postpone the execution of the destroy sequence after the closing of the reader mode */
      pSlot->bFirstDestroyCalled = true;

      return P_HANDLE_DESTROY_LATER;
   }

   if(pSlot->hDriverConnectionWeak != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pSlot->hDriverConnectionWeak);
      pSlot->hDriverConnectionWeak = W_NULL_HANDLE;
   }

   PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &pSlot->sDestroyCallback);

   PNFCControllerSetSESwitchPosition(pContext,
      W_NFCC_STORAGE_VOLATILE, &pSEInstance->sAfterComPolicy,
      static_PSEDriverDestroyConnectionCallback, pSlot);

   return P_HANDLE_DESTROY_PENDING;
}

/**
 * @brief   Gets the SE connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 **/
static uint32_t static_PSEDriverGetPropertyNumber(
            tContext* pContext,
            void* pObject
             )
{
   return 1;
}

/**
 * @brief   Gets the SE connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  pPropertyArray  The property array.
 **/
static bool static_PSEDriverGetProperties(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray )
{
   pPropertyArray[0] = W_PROP_SECURE_ELEMENT;
   return true;
}

/**
 * @brief   Checkes the SE connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  nPropertyValue  The property value.
 **/
static bool static_PSEDriverCheckProperties(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue )
{
   PDebugTrace(
      "static_PSEDriverCheckProperties: nPropertyValue=%s (0x%02X)",
      PUtilTraceConnectionProperty(nPropertyValue), nPropertyValue  );

   return ( nPropertyValue == W_PROP_SECURE_ELEMENT )?true:false;
}

/* Handle registry SE type */
tHandleType g_sSEConnection = {  null,
                                 static_PSEDriverDestroyConnectionAsync,
                                 static_PSEDriverGetPropertyNumber,
                                 static_PSEDriverGetProperties,
                                 static_PSEDriverCheckProperties };

#define P_HANDLE_TYPE_SE_CONNECTION (&g_sSEConnection)

/**
 * @brief   Destroyes a SE connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PSEDriverSecurityStackDestroyHookAsync(
            tContext* pContext,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            void* pObject )
{
   tSESlot* pSlot = P_HANDLE_GET_STRUCTURE(tSESlot, pObject, sObjectHeaderSecurityStackDestroyHook);

   PDebugTrace("static_PSEDriverSecurityStackDestroyHookAsync");

   CDebugAssert(pSlot->hDriverConnectionWeak != W_NULL_HANDLE);

   /* Artificially increment the ref count to avoid double call to destroy procedure
      when a decrement ref count function is called
   */
   PHandleIncrementReferenceCount(pSlot);

   PSecurityStackNotifyEndofConnection(
            pContext,
            pSlot->pSecurityStackInstance,
            static_PSEDriverExchangeApdu,
            pSlot->hDriverConnectionWeak,
            pCallback, pCallbackParameter );

   return P_HANDLE_DESTROY_PENDING;
}

/* Handle registry SE type */
tHandleType g_sSESecurityStackDestryHook = {  null,
                                 static_PSEDriverSecurityStackDestroyHookAsync,
                                 null,
                                 null,
                                 null };

#define P_HANDLE_TYPE_SECURITY_STACK_DESTROY_HOOK (&g_sSESecurityStackDestryHook)

static void static_PSEDriverOpenConnectionDetectionHandler(
            tContext* pContext,
            void* pCallbackParameter,
            uint32_t nDriverProtocol,
            W_HANDLE hDriverConnection,
            uint32_t nLength,
            bool bCardApplicationMatch )
{
   W_ERROR nError;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;

   PDebugTrace("static_PSEDriverOpenConnectionDetectionHandler()");

   PHandleClose(pContext, pSlot->hOpenConnectionListenner);
   pSlot->hOpenConnectionListenner = W_NULL_HANDLE;

   CDebugAssert(hDriverConnection != W_NULL_HANDLE);

   /* Deactivate timer here */
   PMultiTimerCancel(pContext, TIMER_T11_SE_WATCHDOG);

   /* Add a secure element component to the connection object */
   if ( ( nError = PHandleAddHeir(
                        pContext,
                        hDriverConnection,
                        pSlot,
                        P_HANDLE_TYPE_SE_CONNECTION ) ) != W_SUCCESS )
   {
      PDebugError("static_PSEDriverOpenConnectionDetectionHandler: cannot build the connection");
      goto send_error;
   }

   if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION_FROM_USER) != 0)
   {
      /* Add a second component to insert a hook in the destruction sequence */
      if ( ( nError = PHandleAddHeir(
                           pContext,
                           hDriverConnection,
                           &pSlot->sObjectHeaderSecurityStackDestroyHook,
                           P_HANDLE_TYPE_SECURITY_STACK_DESTROY_HOOK ) ) != W_SUCCESS )
      {
         PDebugError("static_PSEDriverOpenConnectionDetectionHandler: cannot a second destructor");
         goto send_error;
      }

      /* Create a weak handle to be used by the hook during the destruction sequence */
      if ( ( nError = PHandleDuplicateWeak(
                        pContext,
                        hDriverConnection,
                        &pSlot->hDriverConnectionWeak ) ) != W_SUCCESS )
      {
         PDebugError("static_PSEDriverOpenConnectionDetectionHandler: cannot duplicate the connection handle");
         goto send_error;
      }
   }
   else
   {
      pSlot->hDriverConnectionWeak = W_NULL_HANDLE;
   }

   /* Send event */
   if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION_FROM_USER) != 0)
   {
      PDFCDriverPostCC3( pSlot->pOpenConnectionDriverCC,
         P_DFC_TYPE_SECURE_ELEMENT, hDriverConnection, W_SUCCESS);
   }
   else
   {
      PDFCPostContext3(&(pSlot->sOpenConnectionCC),
         P_DFC_TYPE_SECURE_ELEMENT, hDriverConnection, W_SUCCESS);
   }

   return;

send_error:

   (void)PReaderDriverWorkPerformed(
         pContext, hDriverConnection, false, true );

   /* Send the error */
   static_PSEDriverOpenConnectionSetBackSESwitchPosition(pContext, pSEInstance, pSlot, nError);
}

/* Watchdog handler is a simple wrapper to send a timeout error */
static void static_PSEDriverOpenConnectionWatchdogHandler(
               tContext* pContext,
               void* pCallbackParameter)
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;

   PDebugTrace("static_PSEDriverOpenConnectionWatchdogHandler()");

   PHandleClose(pContext, pSlot->hOpenConnectionListenner);
   pSlot->hOpenConnectionListenner = W_NULL_HANDLE;

   static_PSEDriverOpenConnectionSetBackSESwitchPosition(pContext, pSEInstance, pSlot, W_ERROR_TIMEOUT);
}

/* Callback invoked when the SE switch is set to reader position */
static void static_PSEDriverOpenConnectionSetSESwitchPositionCompleted(
         tContext* pContext,
         void* pCallbackParameter,
         W_ERROR nError)
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;
   uint32_t nProtocol = 0;
   tNFCControllerSEPolicy sVolatilePolicy;

   PDebugTrace("static_PSEDriverOpenConnectionSetSESwitchPositionCompleted()");

   CDebugAssert((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION) != 0);

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverOpenConnectionSetSESwitchPositionCompleted: Error %s returned by PNFCControllerSetSESwitchPosition()",
         PUtilTraceError(nError));

      if(pSlot->nOpenConnectionFirstError == W_SUCCESS)
      {
         pSlot->nOpenConnectionFirstError = nError;
      }

      static_PSEDriverOpenConnectionSendError(pContext, pSlot);
      return;
   }

   nError = PNFCControllerGetSESwitchPosition(pContext, null, &sVolatilePolicy, null, null);

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverOpenConnectionSetSESwitchPositionCompleted: Error %s returned by PNFCControllerGetSESwitchPosition()",
         PUtilTraceError(nError));

      static_PSEDriverOpenConnectionSetBackSESwitchPosition(pContext, pSEInstance, pSlot, nError);
      return;
   }

   CDebugAssert( (sVolatilePolicy.nSESwitchPosition == P_SE_SWITCH_FORCED_HOST_INTERFACE)
                  || (sVolatilePolicy.nSESwitchPosition == P_SE_SWITCH_HOST_INTERFACE));

   /* Get the SE reader protocol */
   nProtocol = pSEInstance->aSlotArray[sVolatilePolicy.nSlotIdentifier].nConstantProtocols;

   pSlot->hOpenConnectionListenner = W_NULL_HANDLE;

   nError = PReaderDriverRegisterInternal(
            pContext,
            static_PSEDriverOpenConnectionDetectionHandler, pSlot,
            pSlot->bOpenConnectionForce ? W_PRIORITY_SE_FORCED : W_PRIORITY_SE,
            nProtocol, 0,
            pSlot->aOpenConnectionDetectionBuffer,
            sizeof (pSlot->aOpenConnectionDetectionBuffer),
            &pSlot->hOpenConnectionListenner,
            false);

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverOpenConnectionSetSESwitchPositionCompleted: Error %s returned by PReaderDriverRegisterInternal()",
         PUtilTraceError(nError));

      static_PSEDriverOpenConnectionSetBackSESwitchPosition(pContext, pSEInstance, pSlot, nError);
      return;
   }

   PMultiTimerSet(pContext, TIMER_T11_SE_WATCHDOG, P_SE_WATCHDOG_TIMEOUT,
         static_PSEDriverOpenConnectionWatchdogHandler, pSlot );
}

void static_PSEDriverOpenConnectionStopAllActiveDetectionCompleted(
            tContext* pContext,
            void * pCallbackParameter)
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;

   PDebugTrace("static_PSEDriverOpenConnectionStopAllActiveDetectionCompleted()");

   PNFCControllerSetSESwitchPosition(pContext,
      W_NFCC_STORAGE_VOLATILE, &pSEInstance->sNewPolicy,
      static_PSEDriverOpenConnectionSetSESwitchPositionCompleted, pSlot);
}

/** Initialize a Secure Element */
static void static_PSEDriverOpenConnection(
                  tContext* pContext,
                  uint32_t nSlotIdentifier,
                  bool bForce,
                  bool bFromUser,
                  tPBasicGenericHandleCallbackFunction* pCallback,
                  void* pCallbackParameter )
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   W_ERROR nError = W_SUCCESS;
   tSESlot* pSlot;
   tDFCCallbackContext sErrorCC;
   tDFCDriverCCReference pErrorDriverCC;

   PDebugTrace("static_PSEDriverOpenConnection()");

   CDebugAssert(pSEInstance->bSEInfoIsBuilt == true);

   if(nSlotIdentifier >= pSEInstance->nSENumber)
   {
      PDebugError("static_PSEDriverOpenConnection: Wrong slot identifier");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   pSlot = &pSEInstance->aSlotArray[nSlotIdentifier];

   /* Check the presence of SE and the communication capability */
   if((pSlot->sConstantInfo.bIsPresent == false) ||
      ((pSlot->sConstantInfo.nCapabilities & W_SE_FLAG_COMMUNICATION) == 0))
   {
      PDebugError("static_PSEDriverOpenConnection: No SE or communication not supported");
      nError = W_ERROR_FEATURE_NOT_SUPPORTED;
      goto send_event;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION) != 0)
   {
      PDebugError("static_PSEDriverOpenConnection: communication already active");
      nError = W_ERROR_EXCLUSIVE_REJECTED;
      goto send_event;
   }

   nError = PNFCControllerGetSESwitchPosition(pContext,
      null, null, null, &pSEInstance->sAfterComPolicy);
   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverOpenConnection: error returned by PNFCControllerGetSESwitchPosition()");
      goto send_event;
   }

   pSEInstance->nGlobalPendingFlags |= P_SE_FLAG_PENDING_COMMUNICATION;

   if(bFromUser != false)
   {
      PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pCallback, pCallbackParameter,
      &pSlot->pOpenConnectionDriverCC );

      pSEInstance->nGlobalPendingFlags |= P_SE_FLAG_PENDING_COMMUNICATION_FROM_USER;
   }
   else
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &(pSlot->sOpenConnectionCC));

      pSEInstance->nGlobalPendingFlags &= ~P_SE_FLAG_PENDING_COMMUNICATION_FROM_USER;
   }

   pSEInstance->sNewPolicy.nSESwitchPosition = (bForce != false)?P_SE_SWITCH_FORCED_HOST_INTERFACE:P_SE_SWITCH_HOST_INTERFACE;
   pSEInstance->sNewPolicy.nSlotIdentifier = nSlotIdentifier;
   pSEInstance->sNewPolicy.nSEProtocolPolicy = pSEInstance->sAfterComPolicy.nSEProtocolPolicy;

   pSlot->nOpenConnectionFirstError = W_SUCCESS;
   pSlot->bFirstDestroyCalled = false;
   pSlot->bOpenConnectionForce = bForce;

   PReaderDriverDisableAllNonSEListeners(pContext,
         static_PSEDriverOpenConnectionStopAllActiveDetectionCompleted,
         pSlot);

   return;

send_event:

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverOpenConnection: Sending error %s", PUtilTraceError(nError));
   }

   if(bFromUser != false)
   {
      PDFCDriverFillCallbackContext( pContext,
         (tDFCCallback*)pCallback, pCallbackParameter, &pErrorDriverCC );
      PDFCDriverPostCC3( pErrorDriverCC,
         P_DFC_TYPE_SECURE_ELEMENT, W_NULL_HANDLE, nError );
   }
   else
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &sErrorCC);
      PDFCPostContext3(&sErrorCC, P_DFC_TYPE_SECURE_ELEMENT, W_NULL_HANDLE, nError);
   }
}

/* See header file */
void PSEDriverOpenConnection(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            bool bForce,
            tPBasicGenericHandleCallbackFunction* pCallback,
            void* pCallbackParameter)
{
   static_PSEDriverOpenConnection(
                  pContext, nSlotIdentifier, bForce, true,
                  pCallback, pCallbackParameter );
}

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

         INITIALIZATION

   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */


/* Forward declaration */
static void static_PSEDriverInitializeNext(
            tContext* pContext,
            W_ERROR nError);

/* Callback for the close safe function */
static void static_PSEDriverInitializeCloseSafeCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverInitializeCloseSafeCompleted: Error %s received", PUtilTraceError(nError));
   }

   if(pSlot->nInitializeHostConnectionError != W_SUCCESS)
   {
      nError = pSlot->nInitializeHostConnectionError;
   }

   pSlot->nInitializeHostConnectionError = W_SUCCESS;
   pSlot->hInitializeHostConnection = W_NULL_HANDLE;

   static_PSEDriverInitializeNext(pContext, nError);
}

/* Callback for the load ACF function */
static void static_PSEDriverInitializeLoadAcfCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;

   pSlot->nInitializeHostConnectionError = nError;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverInitializeLoadAcfCompleted: Error %s received", PUtilTraceError(nError));
   }

   PHandleCloseSafe(
      pContext,
      pSlot->hInitializeHostConnection,
      static_PSEDriverInitializeCloseSafeCompleted, pSlot);
}

/* Callback for the SE initalization */
static void static_PSEDriverInitializeOpenConnectionCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_HANDLE hConnection,
            W_ERROR nError)
{
   tSESlot* pSlot = (tSESlot*)pCallbackParameter;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverInitializeOpenConnectionCompleted: Error %s received", PUtilTraceError(nError));
      static_PSEDriverInitializeNext(pContext, nError);
      return;
   }

   pSlot->hInitializeHostConnection = hConnection;

   PSecurityStackLoadAcf(
            pContext,
            pSlot->pSecurityStackInstance,
            static_PSEDriverExchangeApdu,
            hConnection,
            static_PSEDriverInitializeLoadAcfCompleted, pSlot );
}

/* Callback for the next initalization */
static void static_PSEDriverInitializeNext(
            tContext* pContext,
            W_ERROR nError)
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PSEDriverInitializeNext: Error %s received for SE #%d",
         PUtilTraceError(nError), pSEInstance->nSEIndentifierBeingInitialized);
      PDebugWarning("static_PSEDriverInitializeNext: The error is ignored but the access to the SE #%d is locked.",
         pSEInstance->nSEIndentifierBeingInitialized);
   }

   pSEInstance->nSEIndentifierBeingInitialized++;

   while(pSEInstance->nSEIndentifierBeingInitialized < pSEInstance->nSENumber)
   {
      tSESlot* pSlot = &pSEInstance->aSlotArray[pSEInstance->nSEIndentifierBeingInitialized];

      if(((pSlot->sConstantInfo.nCapabilities & W_SE_FLAG_COMMUNICATION) != 0)
      && (pSlot->sConstantInfo.bIsPresent == true))
      {
         static_PSEDriverOpenConnection(
            pContext,
            pSEInstance->nSEIndentifierBeingInitialized, true, false,
            static_PSEDriverInitializeOpenConnectionCompleted, pSlot);
         return;
      }

      pSEInstance->nSEIndentifierBeingInitialized++;
   }

   PDFCPostContext2( &pSEInstance->sInitializeCallback, P_DFC_TYPE_SECURE_ELEMENT, nError );
}

/* See header file */
void PSEDriverInitialize(
            tContext* pContext,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter)
{
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   W_ERROR nError;

   PDebugTrace("PSEDriverInitialize()");

   PSEDriverResetData(pContext);

   PDFCFillCallbackContext(pContext, (tDFCCallback*) pCallback, pCallbackParameter, &pSEInstance->sInitializeCallback);

   pSEInstance->nSEIndentifierBeingInitialized = (uint32_t)-1;

   nError = static_PSEDriverBuildInfo(pContext, pSEInstance);
   if(nError != W_SUCCESS)
   {
      PDebugError("PSEDriverInitialize: Error %s returned by static_PSEDriverBuildInfo()", PUtilTraceError(nError));
   }

   static_PSEDriverInitializeNext(pContext, nError);
}

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

         FILTER FUNCTIONS

   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

/** See header file */
W_ERROR PSEFilterApdu(
            tContext* pContext,
            const uint8_t* pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength )
{
   tNFCControllerSEPolicy sVolatilePolicy;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   W_ERROR nError;

   PDebugTrace("PSEFilterApdu()");

   if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION) == 0)
   {
      PDebugError("PSEFilterApdu: communication is not active");
      return W_ERROR_BAD_STATE;
   }

   nError = PNFCControllerGetSESwitchPosition(pContext, null, &sVolatilePolicy, null, null);
   if(nError != W_SUCCESS)
   {
      PDebugError("PSEFilterApdu: error %s retruend by PNFCControllerGetSESwitchPosition",
         PUtilTraceError(nError));
      return nError;
   }

   if( (sVolatilePolicy.nSESwitchPosition != P_SE_SWITCH_FORCED_HOST_INTERFACE)
   && (sVolatilePolicy.nSESwitchPosition != P_SE_SWITCH_HOST_INTERFACE))
   {
      PDebugError("PSEFilterApdu: bad switch state");
      return W_ERROR_BAD_STATE;
   }

   return PSecurityStackFilterApdu(
            pContext,
            pSEInstance->aSlotArray[sVolatilePolicy.nSlotIdentifier].pSecurityStackInstance,
            pReaderToCardBuffer, nReaderToCardBufferLength );
}

/** See header file */
W_ERROR PSENotifyResponseApdu(
            tContext* pContext,
            const uint8_t* pCardToReaderBuffer,
            uint32_t nCardToReaderBufferLength,
            W_ERROR nError )
{
   tNFCControllerSEPolicy sVolatilePolicy;
   tSEInstance* pSEInstance = PContextGetSEInstance( pContext );
   W_ERROR nError2;

   PDebugTrace("PSENotifyResponseApdu()");

   if((pSEInstance->nGlobalPendingFlags & P_SE_FLAG_PENDING_COMMUNICATION) == 0)
   {
      PDebugError("PSENotifyResponseApdu: communication is not active");
      return W_ERROR_BAD_STATE;
   }

   nError2 = PNFCControllerGetSESwitchPosition(pContext, null, &sVolatilePolicy, null, null);
   if(nError2 != W_SUCCESS)
   {
      PDebugError("PSENotifyResponseApdu: error %s retruend by PNFCControllerGetSESwitchPosition",
         PUtilTraceError(nError2));
      return nError2;
   }

   if( (sVolatilePolicy.nSESwitchPosition != P_SE_SWITCH_FORCED_HOST_INTERFACE)
   && (sVolatilePolicy.nSESwitchPosition != P_SE_SWITCH_HOST_INTERFACE))
   {
      PDebugError("PSENotifyResponseApdu: bad switch state");
      return W_ERROR_BAD_STATE;
   }

   return PSecurityStackNotifyResponseApdu(
            pContext,
            pSEInstance->aSlotArray[sVolatilePolicy.nSlotIdentifier].pSecurityStackInstance,
            pCardToReaderBuffer, nCardToReaderBufferLength,
            nError );
}

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */
