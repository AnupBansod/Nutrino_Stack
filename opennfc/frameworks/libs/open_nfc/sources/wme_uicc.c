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
   Contains the UICC implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( UICC )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)


/* See header file */
void PUICCUserCreate(
            tUICCUserInstance* pUICCUserInstance )
{
   CMemoryFill( pUICCUserInstance, 0, sizeof(tUICCUserInstance) );

   pUICCUserInstance->bInUse = false;
}

/* See header file */
void PUICCUserDestroy(
            tUICCUserInstance* pUICCUserInstance )
{
   if ( pUICCUserInstance != null )
   {
      CMemoryFill( pUICCUserInstance, 0, sizeof(tUICCUserInstance) );
   }
}


/* Declare a synchronous data structure */
typedef struct __tPUICCGetSlotInfoSync_Parameters
{
   P_SYNC_WAIT_OBJECT hWaitObject;

   uint32_t nSWPLinkStatus;

   W_ERROR nError;

} tPUICCGetSlotInfoSync_Parameters;


static void static_PUICCGetSlotInfoCompletedSync(
            void* pCallbackParameter,
            uint32_t nSWPLinkStatus,
            W_ERROR nError)
{
   tPUICCGetSlotInfoSync_Parameters* pParam = (tPUICCGetSlotInfoSync_Parameters*)pCallbackParameter;

   pParam->nSWPLinkStatus = nSWPLinkStatus;
   pParam->nError = nError;

   CSyncSignalWaitObject(&pParam->hWaitObject);
}

W_ERROR WUICCGetSlotInfoSync(
            uint32_t* pnSWPLinkStatus)
{
   tPUICCGetSlotInfoSync_Parameters param;

   if(pnSWPLinkStatus == null)
   {
      PDebugError("WUICCGetSlotInfoSync: bad parameter");
      return W_ERROR_BAD_PARAMETER;
   }

   if(CSyncCreateWaitObject(&param.hWaitObject) == false)
   {
      PDebugError("WUICCGetSlotInfoSync: cannot create the sync object");
      *pnSWPLinkStatus = W_UICC_LINK_STATUS_NO_CONNECTION;
      return W_ERROR_SYNC_OBJECT;
   }

   /* Send the command asynchronously */
   WUICCGetSlotInfo( static_PUICCGetSlotInfoCompletedSync, &param );

   CSyncWaitForObject(&param.hWaitObject);

   CSyncDestroyWaitObject(&param.hWaitObject);

   *pnSWPLinkStatus = param.nSWPLinkStatus;

   return param.nError;
}

W_ERROR WUICCActivateSWPLineSync( void )
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WUICCActivateSWPLine( PBasicGenericSyncCompletion, &param );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

static void static_PUICCActivateSWPLineDriverCompleted(
   tContext* pContext,
   void * pCallbackParameter,
   W_ERROR nError)
{
   PDFCPostContext2(pCallbackParameter, P_DFC_TYPE_UICC, nError);
   CMemoryFree(pCallbackParameter);
}

/* See client API */
void PUICCActivateSWPLine(
   tContext* pContext,
   tPBasicGenericCallbackFunction * pCallback,
   void * pCallbackParameter )
{
   tDFCCallbackContext * pCallbackContext = CMemoryAlloc(sizeof(tDFCCallbackContext));
   W_ERROR nError;

   if (pCallbackContext != null)
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);

      PUICCActivateSWPLineDriver(pContext, static_PUICCActivateSWPLineDriverCompleted, pCallbackContext);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_NFCC, static_PUICCActivateSWPLineDriverCompleted, pCallbackContext, nError);
      }
   }
   else
   {
      tDFCCallbackContext sCallbackContext;

      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &sCallbackContext);
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NFCC, W_ERROR_OUT_OF_RESOURCE);
   }
}



W_ERROR WUICCSetAccessPolicySync(
               uint32_t nStorageType,
               const tWUICCAccessPolicy* pAccessPolicy)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WUICCSetAccessPolicy( nStorageType, pAccessPolicy, PBasicGenericSyncCompletion, &param );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

void PUICCGetAccessPolicy(
      tContext* pContext,
      uint32_t nStorageType,
      tWUICCAccessPolicy* pAccessPolicy)
{
   if (pAccessPolicy)
   {
      PUICCDriverGetAccessPolicy( pContext, nStorageType, pAccessPolicy, sizeof(tWUICCAccessPolicy));
   }
}


/* see PUICCDriverSetAccessPolicy */
static void static_PUICCSetAccessPolicy_completed(
   tContext* pContext,
   void * pCallbackParameter,
   W_ERROR nError)
{
   tUICCUserInstance* pUICCUserInstance = PContextGetUICCUserInstance( pContext );

   PDFCPostContext2(&pUICCUserInstance->sCallbackContext, P_DFC_TYPE_UICC, nError);
   pUICCUserInstance->bInUse = false;
}

/* See client API */
void PUICCSetAccessPolicy(
      tContext* pContext,
      uint32_t nStorageType,
      const tWUICCAccessPolicy * pAccessPolicy,
      tPBasicGenericCallbackFunction * pCallback,
      void * pCallbackParameter )
{
   tUICCUserInstance* pUICCUserInstance = PContextGetUICCUserInstance( pContext );
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   if (pAccessPolicy == null)
   {
      PDebugError("PUICCSetAccessPolicy : pAccessPolicy is null");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if (pUICCUserInstance->bInUse == true)
   {
      PDebugError("PUICCSetAccessPolicy : operation already in progress");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* all is fine */

   PDFCFillCallbackContext(pContext, (tDFCCallback*) pCallback, pCallbackParameter, &pUICCUserInstance->sCallbackContext);
   pUICCUserInstance->bInUse = true;
   PUICCDriverSetAccessPolicy(pContext, nStorageType, pAccessPolicy, sizeof(tWUICCAccessPolicy), static_PUICCSetAccessPolicy_completed, null);

   nError = PContextGetLastIoctlError(pContext);
   if (nError != W_SUCCESS)
   {
      PDFCPost2(pContext, P_DFC_TYPE_UICC, static_PUICCSetAccessPolicy_completed, null, nError);
   }

   return;

return_error:

   PDFCFillCallbackContext(pContext, (tDFCCallback*) pCallback, pCallbackParameter, &sCallbackContext);
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_UICC, nError);
}

static void static_PUICCGetSlotInfoDriverCompleted(
   tContext * pContext,
   void * pCallbackParameter,
   uint32_t nSWPLinkStatus,
   W_ERROR nError )
{
   PDFCPostContext3(pCallbackParameter, P_DFC_TYPE_UICC, nSWPLinkStatus, nError);
   CMemoryFree(pCallbackParameter);
}

void PUICCGetSlotInfo(
      tContext* pContext,
      tPUICCGetSlotInfoCompleted * pCallback,
      void * pCallbackParameter )
{
   tDFCCallbackContext * pCallbackContext = CMemoryAlloc(sizeof(tDFCCallbackContext));
   W_ERROR nError;

   if (pCallbackContext != null)
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);

      PUICCGetSlotInfoDriver(pContext, static_PUICCGetSlotInfoDriverCompleted, pCallbackContext);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost3(pContext, P_DFC_TYPE_NFCC, static_PUICCGetSlotInfoDriverCompleted, pCallbackContext, 0, nError);
      }
   }
   else
   {
      tDFCCallbackContext sCallbackContext;

      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &sCallbackContext);
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NFCC, W_ERROR_OUT_OF_RESOURCE);
   }
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

static void static_PUICCMonitorConnectivityEventReceived(
         tContext* pContext,
         void* pCallbackParameter,
         uint8_t nEventIdentifier,
         const uint8_t* pBuffer,
         uint32_t nLength,
         uint32_t nNALMessageReceptionCounter);

/* See header file */
void PUICCCreate(
            tUICCInstance* pUICCInstance )
{
   CMemoryFill( pUICCInstance, 0, sizeof(tUICCInstance) );

   pUICCInstance->bUICCInfoIsBuilt = false;
}

/* See header file */
void PUICCDestroy(
            tUICCInstance* pUICCInstance )
{
   if ( pUICCInstance != null )
   {
      CMemoryFill( pUICCInstance, 0, sizeof(tUICCInstance) );
   }
}

/**
 * Sets the card access policy from a protocol bit-field.
 *
 * @param[in]  nCardProtocol  The card protocol bit-field.
 *
 * @param[out] pAccessPolicy  The access policy.
 **/
static void static_PUICCSetCardPolicy(
                  uint32_t nCardProtocol,
                  tWUICCAccessPolicy* pAccessPolicy)
{
   CDebugAssert((nCardProtocol | P_UICC_PROTOCOL_CARD_ALL) == P_UICC_PROTOCOL_CARD_ALL);

   pAccessPolicy->nCardISO14443_4_A_Priority =
      ((nCardProtocol & W_NFCC_PROTOCOL_CARD_ISO_14443_4_A) != 0)?
      W_PRIORITY_EXCLUSIVE:W_PRIORITY_NO_ACCESS;

   pAccessPolicy->nCardISO14443_4_B_Priority =
      ((nCardProtocol & W_NFCC_PROTOCOL_CARD_ISO_14443_4_B) != 0)?
      W_PRIORITY_EXCLUSIVE:W_PRIORITY_NO_ACCESS;

   pAccessPolicy->nCardBPrime_Priority =
      ((nCardProtocol & W_NFCC_PROTOCOL_CARD_BPRIME) != 0)?
      W_PRIORITY_EXCLUSIVE:W_PRIORITY_NO_ACCESS;

   pAccessPolicy->nCardFeliCa_Priority =
      ((nCardProtocol & W_NFCC_PROTOCOL_CARD_FELICA) != 0)?
      W_PRIORITY_EXCLUSIVE:W_PRIORITY_NO_ACCESS;
}

/**
 * Builds the UICC information.
 *
 * @param[in]  pContext  The current context
 *
 * @param[in]  pUICCInstance  The UICC instance
 *
 * @return  The result error code.
 **/
static W_ERROR static_PUICCDriverBuildInfo(
            tContext* pContext,
            tUICCInstance* pUICCInstance)
{
   if(pUICCInstance->bUICCInfoIsBuilt == false)
   {
      tWUICCAccessPolicy* pPersistentAP = &pUICCInstance->sPersistentAccessPolicy;
      uint32_t nCardProtocol;

      W_ERROR nError = PNFCControllerGetUICCAccessPolicy(pContext,
         W_NFCC_STORAGE_PERSISTENT,
         &pPersistentAP->nReaderISO14443_4_A_Priority,
         &pPersistentAP->nReaderISO14443_4_B_Priority,
         &nCardProtocol);

      if(nError != W_SUCCESS)
      {
         PDebugError("static_PUICCDriverBuildInfo: Error %s returned by PNFCControllerGetUICCAccessPolicy()",
            PUtilTraceError(nError));
         return nError;
      }

      if((pPersistentAP->nReaderISO14443_4_A_Priority != W_PRIORITY_NO_ACCESS)
      && ((pPersistentAP->nReaderISO14443_4_A_Priority < W_PRIORITY_MINIMUM)
      || (pPersistentAP->nReaderISO14443_4_A_Priority > W_PRIORITY_MAXIMUM)))
      {
         PDebugError("static_PUICCDriverBuildInfo: bad reader A priority");
         return W_ERROR_PERSISTENT_DATA;
      }

      if((pPersistentAP->nReaderISO14443_4_B_Priority != W_PRIORITY_NO_ACCESS)
      && ((pPersistentAP->nReaderISO14443_4_B_Priority < W_PRIORITY_MINIMUM)
      || (pPersistentAP->nReaderISO14443_4_B_Priority > W_PRIORITY_MAXIMUM)))
      {
         PDebugError("static_PUICCDriverBuildInfo: bad reader B priority");
         return W_ERROR_PERSISTENT_DATA;
      }

      if((nCardProtocol | P_UICC_PROTOCOL_CARD_ALL) != P_UICC_PROTOCOL_CARD_ALL)
      {
         PDebugError("static_PUICCDriverBuildInfo: bad card priority");
         return W_ERROR_PERSISTENT_DATA;
      }

      static_PUICCSetCardPolicy(nCardProtocol, pPersistentAP);

      PDebugTrace("Card Access Policy:");
      PDebugTrace("   - Reader ISO14443-4 A =%s", PUtilTracePriority(pPersistentAP->nReaderISO14443_4_A_Priority));
      PDebugTrace("   - Reader ISO14443-4 B =%s", PUtilTracePriority(pPersistentAP->nReaderISO14443_4_B_Priority));
      PDebugTrace("   - Card ISO14443-4 A & CLT =%s", PUtilTracePriority(pPersistentAP->nCardISO14443_4_A_Priority));
      PDebugTrace("   - Card ISO14443-4 B =%s", PUtilTracePriority(pPersistentAP->nCardISO14443_4_B_Priority));
      PDebugTrace("   - Card B Prime =%s", PUtilTracePriority(pPersistentAP->nCardBPrime_Priority));
      PDebugTrace("   - Card Felica =%s", PUtilTracePriority(pPersistentAP->nCardFeliCa_Priority));

      /* The volatile policy is set from the persistent policy */
      CMemoryCopy(&pUICCInstance->sVolatileAccessPolicy, pPersistentAP, sizeof(tWUICCAccessPolicy));

      pUICCInstance->bUICCInfoIsBuilt = true;
   }

   return W_SUCCESS;
}

/* The pending operation flags */
#define P_UICC_FLAG_PENDING_GET_SLOT_INFO             0x0001
#define P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION  0x0004
#define P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION           0x0008
#define P_UICC_FLAG_PENDING_SET_POLICY_PERSISTENT     0x0010
#define P_UICC_FLAG_PENDING_SET_POLICY_VOLATILE       0x0020

/**
 * Returns the card and reader protocols from the access policy structure.
 *
 * @param[in]  pAccessPolicy  The access policy structure.
 *
 * @return  The access policy bit field.
 **/
static uint32_t static_PUICCGetProtocols(
            const tWUICCAccessPolicy* pAccessPolicy)
{
   uint32_t nProtocols = 0;

   /* card protocols */

   if(pAccessPolicy->nCardISO14443_4_A_Priority == W_PRIORITY_EXCLUSIVE)
   {
      nProtocols |= W_NFCC_PROTOCOL_CARD_ISO_14443_4_A;
   }
   if(pAccessPolicy->nCardISO14443_4_B_Priority == W_PRIORITY_EXCLUSIVE)
   {
      nProtocols |= W_NFCC_PROTOCOL_CARD_ISO_14443_4_B;
   }
   if(pAccessPolicy->nCardBPrime_Priority == W_PRIORITY_EXCLUSIVE)
   {
      nProtocols |= W_NFCC_PROTOCOL_CARD_BPRIME;
   }
   if(pAccessPolicy->nCardFeliCa_Priority == W_PRIORITY_EXCLUSIVE)
   {
      nProtocols |= W_NFCC_PROTOCOL_CARD_FELICA;
   }

   /* reader protocols */
   if(pAccessPolicy->nReaderISO14443_4_A_Priority != W_PRIORITY_NO_ACCESS)
   {
      nProtocols |= W_NFCC_PROTOCOL_READER_ISO_14443_4_A;
   }
   if(pAccessPolicy->nReaderISO14443_4_B_Priority != W_PRIORITY_NO_ACCESS)
   {
      nProtocols |= W_NFCC_PROTOCOL_READER_ISO_14443_4_B;
   }
   return nProtocols;
}


/* Callback for the function PNFCControllerSetUICCAccessPolicy() */
static void static_PUICCSetUICCAccessPolicyCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tUICCInstance* pUICCInstance = (tUICCInstance*)pCallbackParameter;

   CDebugAssert((pUICCInstance->nPendingFlags &
      (P_UICC_FLAG_PENDING_SET_POLICY_PERSISTENT | P_UICC_FLAG_PENDING_SET_POLICY_VOLATILE)) != 0);

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PUICCSetUICCAccessPolicyCompleted: return the error %s", PUtilTraceError(nError));
   }
   else
   {
      if((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_SET_POLICY_PERSISTENT) != 0)
      {
         CMemoryCopy(&pUICCInstance->sPersistentAccessPolicy,
            &pUICCInstance->sNewAccessPolicy, sizeof(tWUICCAccessPolicy));
      }
      if((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_SET_POLICY_VOLATILE) != 0)
      {
         CMemoryCopy(&pUICCInstance->sVolatileAccessPolicy,
            &pUICCInstance->sNewAccessPolicy, sizeof(tWUICCAccessPolicy));
      }
   }

   pUICCInstance->nPendingFlags &= ~(P_UICC_FLAG_PENDING_SET_POLICY_PERSISTENT | P_UICC_FLAG_PENDING_SET_POLICY_VOLATILE);

   PDFCDriverPostCC2( pUICCInstance->pSetAccessPolicyDriverCC, P_DFC_TYPE_UICC, nError );
}


/* See header file */
void PUICCDriverSetAccessPolicy(
            tContext* pContext,
            uint32_t nStorageType,
            const tWUICCAccessPolicy* pAccessPolicy,
            uint32_t nSize,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter)
{
   tDFCDriverCCReference pErrorCC;
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   W_ERROR nError;
   uint32_t nPendingFlags;
   uint32_t nProtocols;

   PDebugTrace("PUICCDriverSetAccessPolicy()");

   if (PNFCControllerIsActive(pContext) == false)
   {
      PDebugError("PUICCDriverSetAccessPolicy: bad NFC Controller mode");
      nError = W_ERROR_BAD_NFCC_MODE;
      goto send_event;
   }

   /* Check the argument */
   if(((nStorageType | W_NFCC_STORAGE_VOLATILE | W_NFCC_STORAGE_PERSISTENT) !=
      (W_NFCC_STORAGE_VOLATILE | W_NFCC_STORAGE_PERSISTENT))
   || (pAccessPolicy == null)
   || (nStorageType == 0))
   {
      PDebugError("PUICCDriverSetAccessPolicy: Bad parameters");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   if((pAccessPolicy->nReaderISO14443_4_A_Priority != W_PRIORITY_NO_ACCESS)
   && ((pAccessPolicy->nReaderISO14443_4_A_Priority < W_PRIORITY_MINIMUM)
   || (pAccessPolicy->nReaderISO14443_4_A_Priority > W_PRIORITY_MAXIMUM)))
   {
      PDebugError("PUICCDriverSetAccessPolicy: bad reader A priority");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   if((pAccessPolicy->nReaderISO14443_4_B_Priority != W_PRIORITY_NO_ACCESS)
   && ((pAccessPolicy->nReaderISO14443_4_B_Priority < W_PRIORITY_MINIMUM)
   || (pAccessPolicy->nReaderISO14443_4_B_Priority > W_PRIORITY_MAXIMUM)))
   {
      PDebugError("PUICCDriverSetAccessPolicy: bad reader B priority");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

  if((pAccessPolicy->nCardISO14443_4_A_Priority != W_PRIORITY_NO_ACCESS)
   && (pAccessPolicy->nCardISO14443_4_A_Priority != W_PRIORITY_EXCLUSIVE))
   {
      PDebugError("PUICCDriverSetAccessPolicy: bad card A priority");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   if((pAccessPolicy->nCardISO14443_4_B_Priority != W_PRIORITY_NO_ACCESS)
   && (pAccessPolicy->nCardISO14443_4_B_Priority != W_PRIORITY_EXCLUSIVE))
   {
      PDebugError("PUICCDriverSetAccessPolicy: bad card B priority");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   if((pAccessPolicy->nCardBPrime_Priority != W_PRIORITY_NO_ACCESS)
   && (pAccessPolicy->nCardBPrime_Priority != W_PRIORITY_EXCLUSIVE))
   {
      PDebugError("PUICCDriverSetAccessPolicy: bad card B Prime priority");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   if((pAccessPolicy->nCardFeliCa_Priority != W_PRIORITY_NO_ACCESS)
   && (pAccessPolicy->nCardFeliCa_Priority != W_PRIORITY_EXCLUSIVE))
   {
      PDebugError("PUICCDriverSetAccessPolicy: bad card FeliCa priority");
      nError = W_ERROR_BAD_PARAMETER;
      goto send_event;
   }

   nError = static_PUICCDriverBuildInfo(pContext, pUICCInstance);
   if(nError != W_SUCCESS)
   {
      PDebugError("PUICCDriverSetAccessPolicy: Error returned by static_PUICCDriverBuildInfo()");
      goto send_event;
   }

   nPendingFlags = 0;

   if (nStorageType & W_NFCC_STORAGE_PERSISTENT)
   {
      nPendingFlags |= P_UICC_FLAG_PENDING_SET_POLICY_PERSISTENT;
   }

   if (nStorageType & W_NFCC_STORAGE_VOLATILE)
   {
      nPendingFlags |= P_UICC_FLAG_PENDING_SET_POLICY_VOLATILE;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if ((pUICCInstance->nPendingFlags & (P_UICC_FLAG_PENDING_SET_POLICY_VOLATILE | P_UICC_FLAG_PENDING_SET_POLICY_PERSISTENT)) != 0)
   {
      PDebugError("PUICCDriverSetAccessPolicy: operation already active");
      nError = W_ERROR_BAD_STATE;
      goto send_event;
   }

   pUICCInstance->nPendingFlags |= nPendingFlags;

   /* Convert the access policy into card protocol bit-field */
   nProtocols = static_PUICCGetProtocols(pAccessPolicy);

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pCallback, pCallbackParameter,
      &pUICCInstance->pSetAccessPolicyDriverCC );

   CMemoryCopy(&pUICCInstance->sNewAccessPolicy,
      pAccessPolicy, sizeof(tWUICCAccessPolicy));


   PNFCControllerSetUICCAccessPolicy(
            pContext,
            nStorageType,
            pAccessPolicy->nReaderISO14443_4_A_Priority,
            pAccessPolicy->nReaderISO14443_4_B_Priority,
            nProtocols,
            static_PUICCSetUICCAccessPolicyCompleted,
            pUICCInstance);

   return;

send_event:

   if(nError != W_SUCCESS)
   {
      PDebugError("PUICCDriverSetAccessPolicy: Error %s returned", PUtilTraceError(nError));
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pCallback, pCallbackParameter,
      &pErrorCC );

   PDFCDriverPostCC2( pErrorCC, P_DFC_TYPE_UICC, nError );
}

/* See WUICCGetAccessPolicy() */
W_ERROR PUICCDriverGetAccessPolicy(
            tContext* pContext,
            uint32_t nStorageType,
            tWUICCAccessPolicy* pAccessPolicy,
            uint32_t nSize)
{
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   W_ERROR nError;

   PDebugTrace("PUICCDriverGetAccessPolicy()");

   if ((pAccessPolicy != null) && (nSize == sizeof(tWUICCAccessPolicy)))
   {
      nError = static_PUICCDriverBuildInfo(pContext, pUICCInstance);

      if (nError == W_SUCCESS)
      {
         if(nStorageType == W_NFCC_STORAGE_VOLATILE)
         {
            CMemoryCopy(pAccessPolicy, &pUICCInstance->sVolatileAccessPolicy, sizeof(tWUICCAccessPolicy));
            return W_SUCCESS;
         }
         else if(nStorageType == W_NFCC_STORAGE_PERSISTENT)
         {
            CMemoryCopy(pAccessPolicy, &pUICCInstance->sPersistentAccessPolicy, sizeof(tWUICCAccessPolicy));
            return W_SUCCESS;
         }
         else
         {
            PDebugError("PUICCDriverGetAccessPolicy: wrong storage value");
            return W_ERROR_BAD_PARAMETER;
         }
      }
      else
      {
         PDebugError("PUICCDriverGetAccessPolicy: error returned by static_PUICCDriverBuildInfo");

         pAccessPolicy->nReaderISO14443_4_A_Priority = W_PRIORITY_NO_ACCESS;
         pAccessPolicy->nReaderISO14443_4_B_Priority = W_PRIORITY_NO_ACCESS;
         pAccessPolicy->nCardISO14443_4_A_Priority = W_PRIORITY_NO_ACCESS;
         pAccessPolicy->nCardISO14443_4_B_Priority = W_PRIORITY_NO_ACCESS;
         pAccessPolicy->nCardBPrime_Priority = W_PRIORITY_NO_ACCESS;
         pAccessPolicy->nCardFeliCa_Priority = W_PRIORITY_NO_ACCESS;

         return nError;
      }
   }
   else
   {
      PDebugError("PUICCDriverGetAccessPolicy: wrong parameters");
      return W_ERROR_BAD_PARAMETER;
   }
}

/* Destroy the monitor transaction event registry */
static uint32_t static_PUICCDestroyMonitorTransactionEvent(
            tContext* pContext,
            void* pObject )
{
   tUICCInstance* pUICCInstance = P_HANDLE_GET_STRUCTURE(tUICCInstance, pObject, sMonitorTransactionEventRegistry);

   CDebugAssert((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION) != 0);

   PDFCDriverFlushCall(pUICCInstance->pMonitorTransactionEventDriverCC);

   pUICCInstance->nPendingFlags &= ~P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION;

   if((pUICCInstance->nPendingFlags &
   (P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION | P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION)) == 0)
   {
      PNALServiceCancelOperation(pContext, &pUICCInstance->sMonitorConnectivityOperation);
   }

   return P_HANDLE_DESTROY_DONE;
}

/* monitor transaction registry type */
tHandleType g_sUICCMonitorTransactionEvent = { static_PUICCDestroyMonitorTransactionEvent, null, null, null, null };

#define P_HANDLE_TYPE_UICC_MONITOR_TRANSACTION_EVENT (&g_sUICCMonitorTransactionEvent)

/* See Client API Specification */
W_ERROR PUICCMonitorTransactionEvent(
            tContext* pContext,
            tPUICCMonitorTransactionEventHandler* pHandler,
            void* pHandlerParameter,
            W_HANDLE* phEventRegistry)
{
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("PUICCMonitorTransactionEvent()");

   if(phEventRegistry == null)
   {
      PDebugError("PUICCMonitorTransactionEvent: bad parameters");
      return W_ERROR_BAD_PARAMETER;
   }

#if 0 /* @todo patch */

   if(pHandler == null)
   {
      PDebugError("PUICCMonitorTransactionEvent: bad parameters");
      return W_ERROR_BAD_PARAMETER;
   }

#endif

   if (PNFCControllerIsActive(pContext) == false)
   {
      PDebugError("PUICCMonitorTransactionEvent: bad NFC Controller mode");
      nError = W_ERROR_BAD_NFCC_MODE;
      goto return_error;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION) != 0)
   {
      PDebugError("PUICCMonitorTransactionEvent: service already used");
      nError = W_ERROR_EXCLUSIVE_REJECTED;
      goto return_error;
   }

   if((nError = PHandleRegister(pContext,
            &pUICCInstance->sMonitorTransactionEventRegistry,
            P_HANDLE_TYPE_UICC_MONITOR_TRANSACTION_EVENT, phEventRegistry)) != W_SUCCESS)
   {
      PDebugError("PUICCMonitorTransactionEvent: error registering the monitor object");
      goto return_error;
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pHandler, pHandlerParameter,
      &pUICCInstance->pMonitorTransactionEventDriverCC );

   if((pUICCInstance->nPendingFlags &
   (P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION | P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION)) == 0)
   {
      PNALServiceRegisterForEvent(
            pContext,
            NAL_SERVICE_UICC,
            NAL_EVT_UICC_CONNECTIVITY,
            &pUICCInstance->sMonitorConnectivityOperation,
            static_PUICCMonitorConnectivityEventReceived,
            pUICCInstance );
   }

   pUICCInstance->nPendingFlags |= P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION;

   return W_SUCCESS;

return_error:

   PDebugError("PUICCMonitorTransactionEvent: return the error %s", PUtilTraceError(nError));

   *phEventRegistry = W_NULL_HANDLE;
   return nError;
}

/* See Client API Specification */
uint32_t PUICCGetTransactionEventAID(
                  tContext* pContext,
                  uint8_t* pBuffer,
                  uint32_t nBufferLength)
{
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   uint32_t nLength = pUICCInstance->nLastAIDLength;

   if((pBuffer != null) && (nLength <= nBufferLength))
   {
      CMemoryCopy(pBuffer, pUICCInstance->aLastAID, nLength);
   }

   return nLength;
}

/* See Client API Specification */
W_ERROR PUICCGetConnectivityEventParameter(
            tContext* pContext,
            uint8_t* pDataBuffer,
            uint32_t nBufferLength,
            uint32_t* pnActualDataLength)
{
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   W_ERROR nError = W_ERROR_BAD_PARAMETER;

   if(pnActualDataLength == null)
   {
      PDebugError("PUICCGetConnectivityEventParameter: bad parameters");
      return W_ERROR_BAD_PARAMETER;
   }

   if((pDataBuffer == null)
   || (nBufferLength == 0))
   {
      PDebugError("PUICCGetConnectivityEventParameter: bad parameters");
      goto return_error;
   }

   if (PNFCControllerIsActive(pContext) == false)
   {
      PDebugError("PUICCGetConnectivityEventParameter: bad NFC Controller mode");
      nError = W_ERROR_BAD_NFCC_MODE;
      goto return_error;
   }

   /* Check access */
   if((pUICCInstance->nPendingFlags &
      (P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION | P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION)) == 0)
   {
      PDebugError("PUICCGetConnectivityEventParameter: no monitor active");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   if(nBufferLength < pUICCInstance->nConnectivityDataLength)
   {
      PDebugError("PUICCGetConnectivityEventParameter: buffer too short");
      nError = W_ERROR_BUFFER_TOO_SHORT;
      goto return_error;
   }

   CMemoryCopy(pDataBuffer, &pUICCInstance->aConnectivityDataBuffer, pUICCInstance->nConnectivityDataLength);

   *pnActualDataLength = pUICCInstance->nConnectivityDataLength;

   return W_SUCCESS;

return_error:

   *pnActualDataLength = 0;
   return nError;
}

/* Destroy the monitor connectivity event registry */
static uint32_t static_PUICCDestroyMonitorConnectivity(
            tContext* pContext,
            void* pObject )
{
   tUICCInstance* pUICCInstance = P_HANDLE_GET_STRUCTURE(tUICCInstance, pObject, sMonitorConnectivityRegistry);

   CDebugAssert((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION) != 0);

   PDFCDriverFlushCall(pUICCInstance->pMonitorConnectivityDriverCC);

   /* Reset the message length */
   pUICCInstance->nConnectivityDataLength = 0;

   pUICCInstance->nPendingFlags &= ~P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION;

   if((pUICCInstance->nPendingFlags &
   (P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION | P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION)) == 0)
   {
      PNALServiceCancelOperation(pContext, &pUICCInstance->sMonitorConnectivityOperation);
   }

   return P_HANDLE_DESTROY_DONE;
}

/* monitor transaction registry type */
tHandleType g_sUICCMonitorConnectivity = { static_PUICCDestroyMonitorConnectivity, null, null, null, null };

#define P_HANDLE_TYPE_UICC_MONITOR_CONNECTIVITY (&g_sUICCMonitorConnectivity)

static void static_PUICCMonitorConnectivityEventReceived(
         tContext* pContext,
         void* pCallbackParameter,
         uint8_t nEventIdentifier,
         const uint8_t* pBuffer,
         uint32_t nLength,
         uint32_t nNALMessageReceptionCounter)
{
   tUICCInstance* pUICCInstance = (tUICCInstance*)pCallbackParameter;
   uint32_t nMessageCode;

   PDebugTrace("static_PUICCMonitorConnectivityEventReceived()");

   CDebugAssert(nEventIdentifier == NAL_EVT_UICC_CONNECTIVITY);
   if((pBuffer == null) || (nLength == 0))
   {
      PDebugError("static_PUICCMonitorConnectivityEventReceived: Error of protocol");
      /* Can't do anything about it, just return */
      return;
   }

   nMessageCode = pBuffer[0];

   if(nMessageCode == 0x12 /* EVT_TRANSACTION */)
   {
      if((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION) != 0)
      {
         /* BER-TLV parser */

         uint32_t nRemaingBytes;

         /* skip Message Code */
         nRemaingBytes = nLength - 1;
         pBuffer++;

         while(nRemaingBytes >= 2)
         {
            uint8_t nType = *pBuffer++;  /* Parsing only 1-byte types */
            uint32_t nDataLength = *pBuffer++;
            nRemaingBytes -= 2;

            if(nDataLength > 127)
            {
               if((nDataLength != 0x81) || (nRemaingBytes == 0))
               {
                  /* Only length <= 255 are supported */
                  PDebugError("static_PUICCMonitorConnectivityEventReceived: Error in the BER-TLV format sent by the UICC");
                  break;
               }
               nDataLength = *pBuffer++;
               nRemaingBytes--;
            }

            if(nDataLength > nRemaingBytes)
            {
               PDebugError("static_PUICCMonitorConnectivityEventReceived: Error in the BER-TLV format sent by the UICC");
               break;
            }

            if(nType == 0x81) /* AID */
            {
               pUICCInstance->nLastAIDLength = nDataLength;

               if(pUICCInstance->nLastAIDLength > P_UICC_MAX_CONNECTIVITY_AID_LENGTH)
               {
                  PDebugWarning("static_PUICCMonitorConnectivityEventReceived: AID too long (%d bytes), truncated to %d bytes",
                     pUICCInstance->nLastAIDLength, P_UICC_MAX_CONNECTIVITY_AID_LENGTH);
                  pUICCInstance->nLastAIDLength = P_UICC_MAX_CONNECTIVITY_AID_LENGTH;
               }

               if(pUICCInstance->nLastAIDLength != 0)
               {
                  CMemoryCopy(pUICCInstance->aLastAID, pBuffer, pUICCInstance->nLastAIDLength);
               }
            }
            else if(nType == 0x82) /* Parameters */
            {
               pUICCInstance->nConnectivityDataLength = nDataLength;

               if(pUICCInstance->nConnectivityDataLength > P_UICC_MAX_CONNECTIVITY_DATA_LENGTH)
               {
                  PDebugWarning("static_PUICCMonitorConnectivityEventReceived: parameter too long (%d bytes), truncated to %d bytes",
                     pUICCInstance->nConnectivityDataLength, P_UICC_MAX_CONNECTIVITY_DATA_LENGTH);
                  pUICCInstance->nConnectivityDataLength = P_UICC_MAX_CONNECTIVITY_DATA_LENGTH;
               }

               if(pUICCInstance->nConnectivityDataLength != 0)
               {
                  CMemoryCopy(pUICCInstance->aConnectivityDataBuffer,
                     pBuffer, pUICCInstance->nConnectivityDataLength);
               }
            }
            else
            {
               PDebugWarning(
                  "static_PUICCMonitorConnectivityEventReceived: unknown data type %02X, ignored", nType);
            }

            pBuffer += nDataLength;
            nRemaingBytes -= nDataLength;
         }

         PDFCDriverPostCC1( pUICCInstance->pMonitorTransactionEventDriverCC,
            P_DFC_TYPE_UICC);
      }
   }
   else if((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION) != 0)
   {
      pUICCInstance->nConnectivityDataLength = nLength - 1;

      if(pUICCInstance->nConnectivityDataLength > P_UICC_MAX_CONNECTIVITY_DATA_LENGTH)
      {
         PDebugWarning("static_PUICCMonitorConnectivityEventReceived: Message 0x%02X too long (%d bytes), truncated to %d bytes",
            nMessageCode, pUICCInstance->nConnectivityDataLength, P_UICC_MAX_CONNECTIVITY_DATA_LENGTH);
         pUICCInstance->nConnectivityDataLength = P_UICC_MAX_CONNECTIVITY_DATA_LENGTH;
      }

      if(pUICCInstance->nConnectivityDataLength != 0)
      {
         CMemoryCopy(pUICCInstance->aConnectivityDataBuffer,
            &pBuffer[1], pUICCInstance->nConnectivityDataLength);
      }

      PDFCDriverPostCC3( pUICCInstance->pMonitorConnectivityDriverCC,
         P_DFC_TYPE_UICC, nMessageCode, pUICCInstance->nConnectivityDataLength );
   }
}

/* See Client API Specification */
W_ERROR PUICCMonitorConnectivityEvent(
            tContext* pContext,
            tPUICCMonitorConnectivityEventHandler* pHandler,
            void* pHandlerParameter,
            W_HANDLE* phEventRegistry)
{
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("PUICCMonitorConnectivityEvent()");

   if(phEventRegistry == null)
   {
      PDebugError("PUICCMonitorConnectivityEvent: bad parameters");
      return W_ERROR_BAD_PARAMETER;
   }

#if 0 /* @todo patch */

   if(pHandler == null)
   {
      PDebugError("PUICCMonitorConnectivityEvent: bad parameters");
      return W_ERROR_BAD_PARAMETER;
   }

#endif

   if (PNFCControllerIsActive(pContext) == false)
   {
      PDebugError("PUICCMonitorConnectivityEvent: bad NFC Controller mode");
      nError = W_ERROR_BAD_NFCC_MODE;
      goto return_error;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION) != 0)
   {
      PDebugError("PUICCMonitorConnectivityEvent: service already used");
      nError = W_ERROR_EXCLUSIVE_REJECTED;
      goto return_error;
   }

   if((nError = PHandleRegister(pContext,
            &pUICCInstance->sMonitorConnectivityRegistry,
            P_HANDLE_TYPE_UICC_MONITOR_CONNECTIVITY, phEventRegistry)) != W_SUCCESS)
   {
      PDebugError("PUICCMonitorConnectivityEvent: error registering the monitor object");
      goto return_error;
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pHandler, pHandlerParameter,
      &pUICCInstance->pMonitorConnectivityDriverCC );

   if((pUICCInstance->nPendingFlags &
   (P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION | P_UICC_FLAG_PENDING_TRANSACTION_REGITRATION)) == 0)
   {
      PNALServiceRegisterForEvent(
            pContext,
            NAL_SERVICE_UICC,
            NAL_EVT_UICC_CONNECTIVITY,
            &pUICCInstance->sMonitorConnectivityOperation,
            static_PUICCMonitorConnectivityEventReceived,
            pUICCInstance );
   }

   /* Reset the message length */
   pUICCInstance->nConnectivityDataLength = 0;

   pUICCInstance->nPendingFlags |= P_UICC_FLAG_PENDING_CONNECTIVITY_REGITRATION;

   return W_SUCCESS;

return_error:

   PDebugError("PUICCMonitorConnectivityEvent: return the error %s", PUtilTraceError(nError));

   *phEventRegistry = W_NULL_HANDLE;
   return nError;
}


/** @see tPNALServiceSendEventCompleted */

static void static_PUICCActivateSWPLineCompleted(
         tContext* pContext,
         void* pCallbackParameter,
         uint32_t nLength,
         W_ERROR nError,
         uint32_t nReceptionCounter)
{
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );

   CDebugAssert(pUICCInstance->bActivateSWPLineInProgress == true);

   pUICCInstance->bActivateSWPLineInProgress = false;

   if (nError != W_SUCCESS)
   {
      PDebugError("PUICCActivateSWPLineCompleted %s", PUtilTraceError(nError));
   }

   PDFCDriverPostCC2( pUICCInstance->pEndOfActivateSWPLineDriverCC, P_DFC_TYPE_UICC, nError);
}


/* See Client API Specification */
void PUICCActivateSWPLineDriver(
            tContext* pContext,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter)
{
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   tDFCDriverCCReference          pErrorCC;

   /* only one operation is allowed */

   if (pUICCInstance->bActivateSWPLineInProgress == false)
   {
      pUICCInstance->bActivateSWPLineInProgress = true;

      PDFCDriverFillCallbackContext( pContext, (tDFCCallback*)pCallback, pCallbackParameter, & pUICCInstance->pEndOfActivateSWPLineDriverCC);

      /* Request the establishment of the SWP communication */
      PNALServiceExecuteCommand(
         pContext,
         NAL_SERVICE_UICC,
         &pUICCInstance->sActivateSWPLineOperation,
         NAL_CMD_UICC_START_SWP,
         null, 0,
         null, 0,
         static_PUICCActivateSWPLineCompleted,
         null );
   }
   else
   {
      PDebugWarning("PUICCActivateSWPLine: operation in progress");

      PDFCDriverFillCallbackContext( pContext, (tDFCCallback*)pCallback, pCallbackParameter, &pErrorCC );

      PDFCDriverPostCC2( pErrorCC, P_DFC_TYPE_UICC, W_ERROR_BAD_STATE);
   }
}

/* Receive the result of the get parameter */
static void static_PUICCGetSlotInfoCompleted(
         tContext* pContext,
         void* pCallbackParameter,
         uint32_t nLength,
         W_ERROR nError)
{
   tUICCInstance* pUICCInstance = (tUICCInstance*)pCallbackParameter;
   uint32_t nSWPLinkStatus = 0;

   CDebugAssert((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_GET_SLOT_INFO) != 0);

   if(nError == W_SUCCESS)
   {
      if(nLength != 1)
      {
         PDebugError("static_PUICCGetSlotInfoCompleted: Error of length");
         nLength = W_ERROR_NFC_HAL_COMMUNICATION;
      }
      else
      {
         switch(pUICCInstance->aGetSlotInfoBuffer[0])
         {
            case NAL_UICC_SWP_NONE:
               nSWPLinkStatus = W_UICC_LINK_STATUS_NO_UICC;
               break;
            case NAL_UICC_SWP_BOOTING:
               nSWPLinkStatus = W_UICC_LINK_STATUS_INITIALIZATION;
               break;
            case NAL_UICC_SWP_ERROR:
               nSWPLinkStatus = W_UICC_LINK_STATUS_ERROR;
               break;
            case NAL_UICC_SWP_ACTIVE:
               nSWPLinkStatus = W_UICC_LINK_STATUS_ACTIVE;
               break;
            default:
               PDebugError("static_PUICCGetSlotInfoCompleted: Error of value");
               nLength = W_ERROR_NFC_HAL_COMMUNICATION;
               break;
         }
      }
   }

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PUICCGetSlotInfoCompleted: Error %s returned", PUtilTraceError(nError));
   }

   pUICCInstance->nPendingFlags &= ~P_UICC_FLAG_PENDING_GET_SLOT_INFO;

   /* Send the status of the SWP link */
   PDFCDriverPostCC3( pUICCInstance->pGetSlotInfoDriverCC,
      P_DFC_TYPE_UICC, nSWPLinkStatus, nError );
}

/* See Client API Specification */
void PUICCGetSlotInfoDriver(
            tContext* pContext,
            tPUICCGetSlotInfoCompleted* pCallback,
            void* pCallbackParameter )
{
   tDFCDriverCCReference pErrorCC;
   tUICCInstance* pUICCInstance = PContextGetUICCInstance( pContext );
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("PUICCGetSlotInfo()");

   if (PNFCControllerIsActive(pContext) == false)
   {
      PDebugError("PUICCGetSlotInfo: bad NFC Controller mode");
      nError = W_ERROR_BAD_NFCC_MODE;
      goto send_event;
   }

   /* This flag is needed to protect the operation against re-entrantcy */
   if((pUICCInstance->nPendingFlags & P_UICC_FLAG_PENDING_GET_SLOT_INFO) != 0)
   {
      PDebugError("PUICCGetSlotInfo: PUICCGetSlotInfo() already pending");
      nError = W_ERROR_EXCLUSIVE_REJECTED;
      goto send_event;
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pCallback, pCallbackParameter,
      &pUICCInstance->pGetSlotInfoDriverCC );

   PNALServiceGetParameter(
         pContext,
         NAL_SERVICE_UICC,
         &pUICCInstance->sGetSlotInfoOperation,
         NAL_PAR_UICC_SWP,
         pUICCInstance->aGetSlotInfoBuffer, 1,
         static_PUICCGetSlotInfoCompleted, pUICCInstance );

   pUICCInstance->nPendingFlags |= P_UICC_FLAG_PENDING_GET_SLOT_INFO;

   return;

send_event:

   if(nError != W_SUCCESS)
   {
      PDebugError("PUICCGetSlotInfo: Error %s returned", PUtilTraceError(nError));
   }

   PDFCDriverFillCallbackContext( pContext,
      (tDFCCallback*)pCallback, pCallbackParameter,
      &pErrorCC );

   PDFCDriverPostCC4( pErrorCC, P_DFC_TYPE_UICC, 0, 0, nError );
}

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */

