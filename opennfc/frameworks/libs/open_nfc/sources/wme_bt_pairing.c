/*
 * Copyright (c) 2008-2010 Inside Secure, All Rights Reserved.
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
   Contains the Bluetooth pairing implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( BT_PAIR )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* The maximum length in Unicode character of a device name, not including the terminating zero */
#define BT_PAIR_MAXIMUM_NAME_LENGTH  58

#define P_BT_PAIR_MINIMUM_PAYLOAD_SIZE  27

/* Utf8 encoding may use up to 3 bytes per characters */
#define P_BT_PAIR_MAXIMUM_PAYLOAD_SIZE  (P_BT_PAIR_MINIMUM_PAYLOAD_SIZE + (3*BT_PAIR_MAXIMUM_NAME_LENGTH))

#define P_BT_PAIR_MAXIMUM_MESSAGE_SIZE (4 + P_BT_PAIR_MAXIMUM_PAYLOAD_SIZE)

#define P_BT_PAIR_PASSIVE_PAIRING   1
#define P_BT_PAIR_ACTIVE_PAIRING    3

/* The P2P service name for the Bluetooth pairing */
static const tchar g_aPBTPairingServiceName[] =
   { 'n', 'f', 'c', ':', 'b', 't', '-', 'p', 'a', 'i', 'r', 'i', 'n', 'g', 0 };

typedef struct __tPairingOperation
{
   /* Operation header */
   tHandleObjectHeader        sObjectHeader;

   /* BT_PAIR operation handle */
   W_HANDLE                   hVirtualTag;
   W_HANDLE                   hReaderTag;

   /* Command state */
   uint32_t                   nMode;
   bool                       bTagTreatment;

   tWBTPairingInfo            sLocalInfo;
   W_HANDLE                   hMessage;
   W_ERROR                    nResultError;
   bool                       bSetBTMasterMode;

   tWBTPairingInfo            sRemoteInfo;
   uint32_t                   nRemotePairingType;
   tchar                      aRemoteDeviceNameBuffer[BT_PAIR_MAXIMUM_NAME_LENGTH];
   uint32_t                   nRemoteDeviceNameLength;
   uint8_t                    aBuffer[P_BT_PAIR_MAXIMUM_MESSAGE_SIZE];

   /* Callback context */
   tDFCCallbackContext        sCallbackContext;

   W_HANDLE                   hPairingOperation;
   W_HANDLE                   hConnection;
   uint8_t                    nState;

   /* P2P BT pairing related stuff */
   W_HANDLE                   hSocket;
   W_HANDLE                   hLink;
   W_HANDLE                   hLinkOperation;
   bool                       bIsMaster;
   bool                       bIsSocketConnected;

} tPairingOperation;

static void static_PBTPairingEventDetected(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nEventCode);

void static_PBTPairingP2PReadCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nDataLength,
            W_ERROR nError);

static const tchar g_aTypeString[] = {0x6E, 0x6F, 0x6B, 0x69, 0x61, 0x2E, 0x63, 0x6F, 0x6D, 0x3A, 0x62, 0x74, 0x0};
static const uint8_t g_aTypeStringUTF8[] = {0x6E, 0x6F, 0x6B, 0x69, 0x61, 0x2E, 0x63, 0x6F, 0x6D, 0x3A, 0x62, 0x74, 0x0};
static const uint8_t aPUPI[] = { 0x01, 0x02, 0x03, 0x04 };

/* Destroy operation callback */
static uint32_t static_PBTPairingDestroyOperation(
            tContext* pContext,
            void* pObject );

/* Operation registry BT Pairing type */
tHandleType g_sBT_PAIROperation = { static_PBTPairingDestroyOperation, null, null, null, null };
#define P_HANDLE_TYPE_BT_PAIR_OPERATION (&g_sBT_PAIROperation)

/**
 * @brief   Destroyes a BT_PAIR operation object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PBTPairingDestroyOperation(
            tContext* pContext,
            void* pObject )
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pObject;

   PDebugTrace("static_PBTPairingDestroyOperation");

   /* the operation is completed, we can free everything */

   PHandleClose( pContext, pPairingOperation->hVirtualTag);
   PHandleClose( pContext, pPairingOperation->hReaderTag);
   PHandleClose( pContext, pPairingOperation->hMessage);
   PHandleClose( pContext, pPairingOperation->hConnection);

   /* Free the operation structure */
   CMemoryFree( pPairingOperation );

   return P_HANDLE_DESTROY_DONE;
}

void static static_BTPairingShutdownCompleted(tContext * pContext, void * pCallbackParameter, W_ERROR nError);

/* Finish the operation */
static void static_PBTPairingFinishOperation(
            tContext* pContext,
            tPairingOperation* pPairingOperation,
            bool bSetBTMasterMode,
            W_ERROR nResultError)
{
   uint32_t nRemoteDeviceNameLength;
   W_ERROR nError;

   pPairingOperation->bSetBTMasterMode = bSetBTMasterMode;

   PDebugTrace("static_PBTPairingFinishOperation : nResultError %s", PUtilTraceError(nResultError));

   if (pPairingOperation->nState == P_OPERATION_STATE_STARTED)
   {
      PBasicSetOperationCompleted(pContext, pPairingOperation->hPairingOperation);
   }

   pPairingOperation->nResultError = nResultError;
   pPairingOperation->nState = P_OPERATION_STATE_COMPLETED;

   if (pPairingOperation->bIsSocketConnected)
   {
      PDebugTrace("static_PBTPairingFinishOperation : PP2PShutdown");
      PP2PShutdownDriver(pContext, pPairingOperation->hSocket, static_BTPairingShutdownCompleted, pPairingOperation);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_BT_PAIR, static_BTPairingShutdownCompleted,  pPairingOperation, nError);
      }

      return;
   }

   if (pPairingOperation->hLink != W_NULL_HANDLE)
   {
      PDebugTrace("static_PBTPairingFinishOperation : Close link");
      PHandleClose(pContext, pPairingOperation->hLink);
      pPairingOperation->hLink = W_NULL_HANDLE;
   }

   PHandleClose( pContext, pPairingOperation->hVirtualTag);
   PHandleClose( pContext, pPairingOperation->hReaderTag);
   PHandleClose( pContext, pPairingOperation->hMessage);

   pPairingOperation->hVirtualTag = W_NULL_HANDLE;
   pPairingOperation->hReaderTag = W_NULL_HANDLE;
   pPairingOperation->hMessage = W_NULL_HANDLE;

   PHandleClose( pContext, pPairingOperation->hConnection);
   pPairingOperation->hConnection = W_NULL_HANDLE;


   if(nResultError != W_SUCCESS)
   {
      bSetBTMasterMode = false;
      nRemoteDeviceNameLength = 0;
      PDebugError("static_PBTPairingFinishOperation: return the error %s",
         PUtilTraceError(nResultError));
   }
   else
   {
      nRemoteDeviceNameLength = pPairingOperation->nRemoteDeviceNameLength;
   }

   PDFCPostContext4(
         &pPairingOperation->sCallbackContext,
         P_DFC_TYPE_BT_PAIR,
         nRemoteDeviceNameLength,
         bSetBTMasterMode,
         nResultError );

   PHandleClose(pContext, pPairingOperation->hPairingOperation);
}

/* W_BT_PAIRING_TYPE_READER */

static void static_PBTPairingWriteConfirmMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingWriteConfirmMessageCompleted");

   if (pPairingOperation->nState == P_OPERATION_STATE_STARTED)
   {
      if (nError != W_SUCCESS)
      {
         PReaderHandlerWorkPerformed(pContext, pPairingOperation->hConnection, true, false);
         pPairingOperation->hConnection = W_NULL_HANDLE;
      }
      else
      {
         PReaderHandlerWorkPerformed(pContext, pPairingOperation->hConnection, true, true);
         pPairingOperation->hConnection = W_NULL_HANDLE;
      }

      static_PBTPairingFinishOperation(pContext, pPairingOperation, true, nError);
   }
}


static W_ERROR static_PBTPairingGenericCheckRemoteMessage(uint8_t *pBuffer,
                                                          tWBTPairingInfo* pRemoteInfo,
                                                          uint32_t   nPayloadBufferLength,
                                                          tPairingOperation* pPairingOperation)
{
   uint32_t nRemoteDeviceNameLengthInBytes;

   CMemoryCopy(pRemoteInfo->aBD_ADDR,pBuffer,sizeof(pRemoteInfo->aBD_ADDR));
   pBuffer = pBuffer + sizeof(pRemoteInfo->aBD_ADDR);

   CMemoryCopy(pRemoteInfo->aBTDeviceClass,pBuffer,sizeof(pRemoteInfo->aBTDeviceClass));
   pBuffer = pBuffer + sizeof(pRemoteInfo->aBTDeviceClass);

   CMemoryCopy(pRemoteInfo->aAuthenticationData,pBuffer,sizeof(pRemoteInfo->aAuthenticationData));
   pBuffer = pBuffer + sizeof(pRemoteInfo->aAuthenticationData);

   pRemoteInfo->nAuthenticationDataType = W_BT_PAIRING_AUTH_PIN;
   pRemoteInfo->nAuthenticationDataSize = sizeof(pRemoteInfo->aAuthenticationData);

   nRemoteDeviceNameLengthInBytes = (*pBuffer);

   if (nRemoteDeviceNameLengthInBytes != (nPayloadBufferLength - P_BT_PAIR_MINIMUM_PAYLOAD_SIZE))
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: Wrong device name length");
      goto return_error_bad_format;
   }

   if (nRemoteDeviceNameLengthInBytes > 0)
   {
      uint32_t nIndex;

      if((pPairingOperation->nRemoteDeviceNameLength = PUtilConvertUTF8ToUTF16(
         pPairingOperation->aRemoteDeviceNameBuffer,
         pBuffer+1, nRemoteDeviceNameLengthInBytes)) == 0)
      {
         PDebugError("static_PBTPairingCheckRemoteMessage: Wrong Utf8 format");
         goto return_error_bad_format;
      }

      for(nIndex = 0; nIndex < pPairingOperation->nRemoteDeviceNameLength; nIndex++)
      {
         if(pPairingOperation->aRemoteDeviceNameBuffer[nIndex] < 0x20)
         {
            PDebugError("static_PBTPairingCheckRemoteMessage: Wrong character in the device name");
            goto return_error_bad_format;
         }
      }
   }
   else
   {
      pPairingOperation->nRemoteDeviceNameLength = 0;
   }

   if (CMemoryCompare(pRemoteInfo->aBD_ADDR,&pPairingOperation->sLocalInfo.aBD_ADDR,sizeof(pRemoteInfo->aBD_ADDR))== 0)
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: remote BT address is identical to local address");
      goto return_error_bad_format;
   }

   if ((pRemoteInfo->aBD_ADDR[2] == 0x9E)&&(pRemoteInfo->aBD_ADDR[1] == 0x8B)
      &&(pRemoteInfo->aBD_ADDR[0]<0x40))
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: Wrong remote BT address range");
      goto return_error_bad_format;
   }

   return W_SUCCESS;

   return_error_bad_format:
   return W_ERROR_BAD_NDEF_FORMAT;

}

static W_ERROR static_PBTPairingP2PCheckRemoteMessage(
            tContext* pContext,
            tPairingOperation* pPairingOperation,
            uint8_t *pReceptionBuffer,
            uint32_t nDataLength)
{
   tWBTPairingInfo* pRemoteInfo;
   uint8_t *  pPayloadBuffer = null;
   uint32_t   nPayloadBufferLength;
   uint32_t   nTypeStringLength;
   uint8_t *pBuffer = null;

   pRemoteInfo = &pPairingOperation->sRemoteInfo;

   pReceptionBuffer++;
   nTypeStringLength =  (*pReceptionBuffer);
   pReceptionBuffer++;
   nPayloadBufferLength = (*pReceptionBuffer);
   pReceptionBuffer++;

   if (CMemoryCompare(pReceptionBuffer,g_aTypeStringUTF8,nTypeStringLength) !=0)
   {
      PDebugError("static_PBTPairingP2PCheckRemoteMessage: not a bluetooth message");
      goto return_error_bad_format;
   }

   pPayloadBuffer = pReceptionBuffer+nTypeStringLength;

   if ((nPayloadBufferLength < P_BT_PAIR_MINIMUM_PAYLOAD_SIZE)
   || (nPayloadBufferLength > P_BT_PAIR_MAXIMUM_PAYLOAD_SIZE))
   {
      PDebugError("static_PBTPairingP2PCheckRemoteMessage: Out of range");
      goto return_error_bad_format;
   }

   pBuffer = (pPayloadBuffer) + 1;

   if (static_PBTPairingGenericCheckRemoteMessage(pBuffer,pRemoteInfo,nPayloadBufferLength,pPairingOperation) != W_SUCCESS)
   {
      PDebugError("static_PBTPairingP2PCheckRemoteMessage: static_PBTPairingGenericCheckRemoteMessage failed");
      goto return_error_bad_format;
   }

   return W_SUCCESS;

return_error_bad_format:

   PDebugWarning("static_PBTPairingP2PCheckRemoteMessage: Bad remote bluetooth parameters");

   pPairingOperation->nRemoteDeviceNameLength = 0;

   return W_ERROR_BAD_NDEF_FORMAT;
}


static W_ERROR static_PBTPairingCheckRemoteMessage(
            tContext* pContext,
            tPairingOperation* pPairingOperation,
            W_HANDLE hMessage)
{
   tWBTPairingInfo* pRemoteInfo;
   W_HANDLE hRecord;
   uint8_t *  pPayloadBuffer = null;
   uint32_t   nPayloadBufferLength;
   uint8_t *pBuffer = null;
   W_ERROR nError;

   pRemoteInfo = &pPairingOperation->sRemoteInfo;

   if ((hRecord = PNDEFGetRecord(pContext, hMessage, 0)) == W_NULL_HANDLE)
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: Error returned by PNDEFGetRecord");
      goto return_error_bad_format;
   }

   nError = PNDEFGetRecordInfo(pContext, hRecord, W_NDEF_INFO_PAYLOAD, &nPayloadBufferLength);

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: PNDEFGetRecordInfo failed");
      goto return_error_bad_format;
   }

   if ((nPayloadBufferLength < P_BT_PAIR_MINIMUM_PAYLOAD_SIZE)
   || (nPayloadBufferLength > P_BT_PAIR_MAXIMUM_PAYLOAD_SIZE))
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: Out of range");
      goto return_error_bad_format;
   }

   if (PNDEFGetPayloadPointer(pContext, hRecord,(uint8_t**)&pPayloadBuffer) != W_SUCCESS )
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: Cannot extract the payload");
      goto return_error_bad_format;
   }

   pPairingOperation->nRemotePairingType = (*pPayloadBuffer);
   if ((pPairingOperation->nRemotePairingType != P_BT_PAIR_PASSIVE_PAIRING)
    && (pPairingOperation->nRemotePairingType != P_BT_PAIR_ACTIVE_PAIRING))
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: Bad pairing type");
      goto return_error_bad_format;
   }

   pBuffer = (pPayloadBuffer) + 1;

   if (static_PBTPairingGenericCheckRemoteMessage(pBuffer,pRemoteInfo,nPayloadBufferLength,pPairingOperation) != W_SUCCESS)
   {
      PDebugError("static_PBTPairingCheckRemoteMessage: static_PBTPairingGenericCheckRemoteMessage failed");
      goto return_error_bad_format;
   }

   PHandleClose( pContext, hMessage);
   PHandleClose( pContext, hRecord);

   return W_SUCCESS;

return_error_bad_format:

   PDebugWarning("static_PBTPairingCheckRemoteMessage: Bad remote bluetooth parameters");

   PHandleClose( pContext, hMessage);
   PHandleClose( pContext, hRecord);

   pPairingOperation->nRemotePairingType = 0;
   pPairingOperation->nRemoteDeviceNameLength = 0;

   return W_ERROR_BAD_NDEF_FORMAT;
}

/* W_BT_PAIRING_TYPE_READER */

static void static_PBTPairingReadMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_HANDLE hMessage,
            W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingReadMessageCompleted");

   if (pPairingOperation->nState == P_OPERATION_STATE_COMPLETED)
   {
      PReaderHandlerWorkPerformed(pContext, pPairingOperation->hConnection, true, false);
      pPairingOperation->hConnection = W_NULL_HANDLE;
      return;
   }

   if (nError != W_SUCCESS)
   {
      PReaderHandlerWorkPerformed(pContext, pPairingOperation->hConnection, true, false);
      pPairingOperation->hConnection = W_NULL_HANDLE;

      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
   }

   nError = static_PBTPairingCheckRemoteMessage(pContext, pPairingOperation, hMessage);

   if (nError != W_SUCCESS)
   {
      PDebugWarning("static_PBTPairingReadMessageCompleted: Bad remote bluetooth parameters");

      PReaderHandlerWorkPerformed(pContext, pPairingOperation->hConnection, true, false);
      pPairingOperation->hConnection = W_NULL_HANDLE;

      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      return;
   }

   if (pPairingOperation->nRemotePairingType == P_BT_PAIR_ACTIVE_PAIRING)
   {
      PNDEFWriteMessage(pContext, pPairingOperation->hConnection, static_PBTPairingWriteConfirmMessageCompleted,
         pCallbackParameter, pPairingOperation->hMessage, W_NDEF_ACTION_BIT_ERASE | W_NDEF_ACTION_BIT_CHECK_WRITE, null);
   }
   else
   {
      PReaderHandlerWorkPerformed(pContext, pPairingOperation->hConnection, true, false);
      pPairingOperation->hConnection = W_NULL_HANDLE;

      /* Send the result */
      static_PBTPairingFinishOperation(pContext, pPairingOperation, true, W_SUCCESS);
   }
}

/* W_BT_PAIRING_TYPE_READER */

static void static_PBTPairingReaderCardDetectionHandler(
            tContext* pContext,
            void *pHandlerParameter,
            W_HANDLE hConnection,
            W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pHandlerParameter;

   PDebugTrace("static_PBTPairingReaderCardDetectionHandler");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError == W_SUCCESS)
      {
         if (pPairingOperation->bTagTreatment == false)
         {
            pPairingOperation->hConnection = hConnection;
            PNDEFReadMessage(pContext, hConnection, static_PBTPairingReadMessageCompleted,
               pPairingOperation, W_NDEF_TNF_EXTERNAL, g_aTypeString, null);
         }
         else
         {
            PReaderHandlerWorkPerformed(pContext, hConnection, true, false);
         }
      }
      else
      {
         /* Send the error */
         PDebugWarning("static_PBTPairingReaderCardDetectionHandler: card detection error");

         static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      }
   }
}

/* W_BT_PAIRING_TYPE_TAG */

static void static_PBTPairingStartCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingStartCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PBTPairingStartCompleted: tag activation error");

         static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      }
   }
}


/* W_BT_PAIRING_TYPE_TAG */

static void static_PBTPairingWriteMessageCompleted(
            tContext* pContext,
            tPairingOperation* pPairingOperation,
            W_ERROR nError)
{
   PDebugTrace("static_PBTPairingWriteMessageCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PBTPairingWriteMessageCompleted: tag write error");
         static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      }
      else
      {
         if (pPairingOperation->nMode == W_BT_PAIRING_TYPE_TAG)
         {
            PVirtualTagStart(pContext,pPairingOperation->hConnection, static_PBTPairingStartCompleted,
               pPairingOperation,static_PBTPairingEventDetected, pPairingOperation, true);
         }
         else
         {
            PVirtualTagStart(pContext,pPairingOperation->hConnection, static_PBTPairingStartCompleted,
               pPairingOperation,static_PBTPairingEventDetected, pPairingOperation, false);
         }
      }
   }
}

/* ! W_BT_PAIRING_TYPE_TAG */
static void static_PBTPairingWriteAgainMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingWriteAgainMessageCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      static_PBTPairingWriteMessageCompleted(pContext, pPairingOperation, nError);
   }
}

/* ! W_BT_PAIRING_TYPE_TAG */

static void static_PBTPairingReadMessageModifiedCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_HANDLE hMessage,
            W_ERROR nError
   )
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingReadMessageModifiedCompleted");

   if (pPairingOperation->nState == P_OPERATION_STATE_COMPLETED)
   {
      PHandleClose(pContext, hMessage);
      return;
   }

   pPairingOperation->bTagTreatment = false;

   if (nError == W_SUCCESS)
   {
      nError = static_PBTPairingCheckRemoteMessage( pContext, pPairingOperation, hMessage);

      if(nError != W_SUCCESS)
      {
         PDebugWarning("static_PBTPairingReadMessageModifiedCompleted: bluetooth parameter error");
      }

      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
   }
   else
   {
      if (pPairingOperation->hVirtualTag == W_NULL_HANDLE)
      {
         PDebugWarning("static_PBTPairingReadMessageModifiedCompleted: the virtual tag handle is null");

         PReaderHandlerWorkPerformed(pContext, pPairingOperation->hConnection, true, false);
         pPairingOperation->hConnection = W_NULL_HANDLE;
         static_PBTPairingFinishOperation(pContext, pPairingOperation, false, W_ERROR_BAD_STATE);
         return;
      }

      pPairingOperation->hConnection = pPairingOperation->hVirtualTag;
      PNDEFWriteMessage(pContext, pPairingOperation->hVirtualTag, static_PBTPairingWriteAgainMessageCompleted,
            pPairingOperation, pPairingOperation->hMessage, W_NDEF_ACTION_BIT_ERASE | W_NDEF_ACTION_BIT_CHECK_WRITE , null);
   }
}

/* ! W_BT_PAIRING_TYPE_TAG */

static void static_PBTPairingStopCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   W_HANDLE hOperation;
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingStopCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError == W_SUCCESS)
      {
         PNDEFReadMessage(pContext, pPairingOperation->hVirtualTag, static_PBTPairingReadMessageModifiedCompleted,
            pPairingOperation, W_NDEF_TNF_EXTERNAL, g_aTypeString, &hOperation);
         return;
      }

      PDebugWarning("static_PBTPairingStopCompleted: tag stop error");

      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
   }
}

/* W_BT_PAIRING_TYPE_TAG */

static void static_PBTPairingEventDetected(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nEventCode)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingEventDetected");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      switch(nEventCode)
      {
         case W_VIRTUAL_TAG_EVENT_READER_READ_ONLY :

            PDebugTrace("static_PBTPairingEventDetected: W_VIRTUAL_TAG_EVENT_READER_READ_ONLY");

            if (pPairingOperation->nMode == W_BT_PAIRING_TYPE_TAG)
            {
               static_PBTPairingFinishOperation(pContext, pPairingOperation, false, W_SUCCESS);
            }
            break;

         case W_VIRTUAL_TAG_EVENT_READER_WRITE :

            PDebugTrace("static_PBTPairingEventDetected: W_VIRTUAL_TAG_EVENT_READER_WRITE");

            CDebugAssert ( pPairingOperation->nMode != W_BT_PAIRING_TYPE_TAG );

            if (pPairingOperation->nMode != W_BT_PAIRING_TYPE_TAG)
            {
               pPairingOperation->bTagTreatment = true;
               PVirtualTagStop( pContext, pPairingOperation->hVirtualTag, static_PBTPairingStopCompleted,
                  pPairingOperation);
            }
            break;

         case W_VIRTUAL_TAG_EVENT_SELECTION :
         case W_VIRTUAL_TAG_EVENT_READER_LEFT :
         default :
            break;
      }
   }
}

/* W_BT_PAIRING_TYPE_TAG */

static void static_PBTPairingWriteFirstMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingWriteFirstMessageCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PBTPairingWriteFirstMessageCompleted: tag write error");

         static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      }
      else
      {
         static_PBTPairingWriteMessageCompleted(pContext, pPairingOperation, nError);
      }
   }

}

/**
 * Type of the cancel operation callback.
 *
 * @param[in] pContext The current context.
 *
 * @param[in]  pCancelParameter  The cancel callback parameter.
 *
 * @param[in]  bIsClosing  The operation is cancelled because of a close handle.
 **/
static void static_PBTPairingCancelOperation(
         tContext* pContext,
         void* pCancelParameter,
         bool bIsClosing)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCancelParameter;

   PDebugTrace("static_PBTPairingCancelOperation");
   PDebugTrace("static_PBTPairingCancelOperation : bIsClosing %d", (int) bIsClosing);

   if (bIsClosing == false)
   {
      /* Check the operation state */
      if ( PBasicGetOperationState(pContext, pPairingOperation->hPairingOperation)
         == P_OPERATION_STATE_CANCELLED )
      {

         /* Send the error */
         PHandleClose( pContext, pPairingOperation->hVirtualTag);
         pPairingOperation->hVirtualTag = W_NULL_HANDLE;

         PHandleClose( pContext, pPairingOperation->hReaderTag);
         pPairingOperation->hReaderTag = W_NULL_HANDLE;

         PHandleClose( pContext, pPairingOperation->hMessage);
         pPairingOperation->hMessage = W_NULL_HANDLE;

         if ((pPairingOperation->nMode == W_BT_PAIRING_TYPE_P2P)
         || (pPairingOperation->nMode == W_BT_PAIRING_TYPE_P2P_MASTER)
         || (pPairingOperation->nMode == W_BT_PAIRING_TYPE_P2P_SLAVE))
         {
            if (pPairingOperation->hLinkOperation != W_NULL_HANDLE)
            {
               PBasicCancelOperation(pContext, pPairingOperation->hLinkOperation);
               PHandleClose(pContext, pPairingOperation->hLinkOperation);
               pPairingOperation->hLinkOperation = W_NULL_HANDLE;
            }
         }
      }
   }
}

/* See Client API Specifications */
W_ERROR PBTPairingGetRemoteDeviceInfo(
         tContext* pContext,
         W_HANDLE hOperation,
         tWBTPairingInfo* pRemoteInfo,
         tchar* pRemoteDeviceNameBuffer,
         uint32_t nRemoteDeviceNameBufferLength)
{
   tPairingOperation* pPairingOperation;
   W_ERROR nError;

   if(pRemoteInfo == null)
   {
      return W_ERROR_BAD_PARAMETER;
   }

   if (hOperation == W_NULL_HANDLE)
   {
      return W_ERROR_BAD_HANDLE;
   }

   nError = PHandleGetObject(pContext, hOperation, P_HANDLE_TYPE_BT_PAIR_OPERATION, (void**)&pPairingOperation);
   if(( ( nError != W_SUCCESS ) || ( pPairingOperation == null ) )||
      (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   || (pPairingOperation->nResultError != W_SUCCESS))
   {
      return W_ERROR_BAD_STATE;
   }

   if(pRemoteDeviceNameBuffer != null)
   {
      uint32_t nLengthToCopy = pPairingOperation->nRemoteDeviceNameLength;

      if(nRemoteDeviceNameBufferLength < nLengthToCopy)
      {
         nLengthToCopy = nRemoteDeviceNameBufferLength;
      }

      if(nLengthToCopy != 0)
      {
         CMemoryCopy(pRemoteDeviceNameBuffer,
            pPairingOperation->aRemoteDeviceNameBuffer,
            nLengthToCopy << 1);
      }
      pRemoteDeviceNameBuffer[nLengthToCopy] = 0;
   }

   CMemoryCopy(pRemoteInfo, &pPairingOperation->sRemoteInfo, sizeof(tWBTPairingInfo));

   return W_SUCCESS;
}


static void static_BTPairingShutdownCompleted(tContext * pContext, void * pCallbackParameter, W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   pPairingOperation->bIsSocketConnected = false;
   static_PBTPairingFinishOperation(pContext, pPairingOperation, pPairingOperation->bSetBTMasterMode, pPairingOperation->nResultError);
}


void static_PBTPairingP2PWriteCompleted(tContext* pContext, void *pCallbackParameter, W_ERROR nError)
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingWriteCompleted %s", PUtilTraceError(nError));

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PBTPairingWriteCompleted: P2PWrite failed");

         static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      }
      else
      {
         if (pPairingOperation->bIsMaster == false)
         {
            PDebugTrace("static_PBTPairingP2PWriteCompleted: PP2PReadWrapper");
            PP2PReadDriver(pContext, pPairingOperation->hSocket, static_PBTPairingP2PReadCompleted, pPairingOperation, pPairingOperation->aBuffer, P_BT_PAIR_MAXIMUM_MESSAGE_SIZE, null);

            nError = PContextGetLastIoctlError(pContext);
            if (nError != W_SUCCESS)
            {
               PDFCPost3(pContext, P_DFC_TYPE_BT_PAIR, static_PBTPairingP2PReadCompleted, pPairingOperation, 0, nError);
            }
         }
         else
         {
            /* Send the result */
            static_PBTPairingFinishOperation(pContext, pPairingOperation, true, W_SUCCESS);
         }
      }
   }
}

void static_PBTPairingP2PReadCompleted(tContext* pContext, void *pCallbackParameter, uint32_t nDataLength, W_ERROR nError)
{
   uint32_t nSendBufferLength;
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingReadCompleted %s", PUtilTraceError(nError));

   if (pPairingOperation->nState == P_OPERATION_STATE_COMPLETED)
   {
      return;
   }

   if (nError != W_SUCCESS)
   {
      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      return;
   }

   nError = static_PBTPairingP2PCheckRemoteMessage(pContext, pPairingOperation,
      pPairingOperation->aBuffer, nDataLength);

   if (nError != W_SUCCESS)
   {
      PDebugWarning("static_PBTPairingReadCompleted: Bad remote bluetooth parameters");

      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
      return;
   }

   if (pPairingOperation->bIsMaster == false )
   {
      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, W_SUCCESS);
   }
   else
   {
      if ((nError = PNDEFGetMessageContent(pContext, pPairingOperation->hMessage, pPairingOperation->aBuffer, sizeof(pPairingOperation->aBuffer),&nSendBufferLength)) != W_SUCCESS)
      {
         static_PBTPairingFinishOperation(pContext, pPairingOperation, false, W_SUCCESS);
      }

      PDebugWarning("static_PBTPairingReadCompleted: PP2PWriteWrapper");

      PP2PWriteDriver(pContext, pPairingOperation->hSocket, static_PBTPairingP2PWriteCompleted, pPairingOperation, pPairingOperation->aBuffer, nSendBufferLength, null);
      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_WIFI_PAIR, static_PBTPairingP2PWriteCompleted, pPairingOperation, nError);
      }
   }
}

void static_PBTPairingP2PConnectCompleted(
         tContext* pContext,
         void *pCallbackParameter,
         W_ERROR nError)
{

   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;
   uint32_t nSendBufferLength;

   PDebugTrace("static_PBTPairingP2PConnectCompleted %s", PUtilTraceError(nError));

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError == W_SUCCESS)
      {
         uint32_t nValue;

         pPairingOperation->bIsSocketConnected = true;

         if ((nError = PP2PGetSocketParameterDriver(pContext, pPairingOperation->hSocket, W_P2P_IS_SERVER, &nValue)) != W_SUCCESS)
         {
            PDebugError("static_PBTPairingP2PConnectCompleted : PP2PGetSocketParameterWrapper failed");
            goto error;
         }

         pPairingOperation->bIsMaster = !nValue;

         PDebugTrace("static_PBTPairingP2PConnectCompleted : acting as %s", pPairingOperation->bIsMaster ? "master" : "slave" );

         if (pPairingOperation->bIsMaster == false )
         {
            if ((nError = PNDEFGetMessageContent(pContext, pPairingOperation->hMessage, pPairingOperation->aBuffer, sizeof(pPairingOperation->aBuffer),&nSendBufferLength)) != W_SUCCESS)
            {
               PDebugError("static_PBTPairingP2PConnectCompleted : PNDEFGetMessageContent failed");
               goto error;
            }

            PDebugTrace("static_PBTPairingP2PConnectCompleted : PP2PWriteWrapper");
            PP2PWriteDriver(pContext, pPairingOperation->hSocket, static_PBTPairingP2PWriteCompleted, pPairingOperation, pPairingOperation->aBuffer, nSendBufferLength, null);

            nError = PContextGetLastIoctlError(pContext);
            if (nError != W_SUCCESS)
            {
               PDFCPost2(pContext, P_DFC_TYPE_WIFI_PAIR, static_PBTPairingP2PWriteCompleted, pPairingOperation, nError);
            }
         }
         else
         {
            PDebugTrace("static_PBTPairingP2PConnectCompleted : PP2PReadWrapper");
            PP2PReadDriver(pContext, pPairingOperation->hSocket, static_PBTPairingP2PReadCompleted, pPairingOperation, pPairingOperation->aBuffer, P_BT_PAIR_MAXIMUM_MESSAGE_SIZE, null);

            nError = PContextGetLastIoctlError(pContext);
            if (nError != W_SUCCESS)
            {
               PDFCPost3(pContext, P_DFC_TYPE_BT_PAIR, static_PBTPairingP2PReadCompleted, pPairingOperation, 0, nError);
            }
         }
      }
      else
      {
         /* Send the error */
         PDebugWarning("static_PBTPairingP2PConnectCompleted: connection error");
         goto error;
      }
   }

   return;

error:
   static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);
}

static void static_PBTPairingP2PEstablishLinkCompleted(
            tContext * pContext,
            void* pCallbackParameter,
            W_HANDLE hHandle,
            W_ERROR nResult )
{
   tPairingOperation* pPairingOperation = (tPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PBTPairingP2PEstablishLinkCompleted : %s", PUtilTraceError(nResult));

   PHandleClose(pContext, pPairingOperation->hLinkOperation);
   pPairingOperation->hLinkOperation = W_NULL_HANDLE;

   if (nResult == W_SUCCESS)
   {
      pPairingOperation->hLink = hHandle;

      PP2PConnectDriver(pContext, pPairingOperation->hSocket, pPairingOperation->hLink, static_PBTPairingP2PConnectCompleted, pPairingOperation);
   }
   else
   {
      /* Send the error */
      PDebugWarning("static_PBTPairingEstablishLinkCompleted: P2P link establishment error");

      static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nResult);
   }
}


static void static_PBTPairingP2PReleaseLinkCompleted(
         tContext * pContext,
         void * pCallbackParameter,
         W_ERROR nError)
{
   PDebugTrace("static_PBTPairingP2PReleaseLinkCompleted: P2P link released");
}

/* See Client API Specifications */
void PBTPairingStart(
         tContext* pContext,
         uint32_t nMode,
         const tWBTPairingInfo* pLocalInfo,
         const tchar* pLocalDeviceName,
         tPBTPairingStartCompleted* pCallback,
         void* pCallbackParameter,
         W_HANDLE* phOperation)
{
   W_ERROR nError = W_SUCCESS;
   W_HANDLE hMessage, hRecord;
   uint8_t *  pPayloadBuffer = null;
   uint8_t *  pBuffer = null;
   uint32_t  nPayloadLength;
   W_HANDLE hEventRegistry;
   W_HANDLE hVirtualTag;
   uint8_t nReadAndCardProtocolSupported = 0;
   tPairingOperation* pPairingOperation;
   uint8_t  nTagType;
   bool   bValue;

   PDebugTrace("PBTPairingStart");

   if (((nMode != W_BT_PAIRING_TYPE_TAG)&&
      (nMode != W_BT_PAIRING_TYPE_P2P)&&
      (nMode != W_BT_PAIRING_TYPE_P2P_MASTER)&&
      (nMode != W_BT_PAIRING_TYPE_P2P_SLAVE)&&
      (nMode != W_BT_PAIRING_TYPE_READER))||
      ((pLocalInfo->nAuthenticationDataType != W_BT_PAIRING_AUTH_PIN)||
      (pLocalInfo->nAuthenticationDataSize != sizeof(pLocalInfo->aAuthenticationData))||
      ((pLocalInfo->aBD_ADDR[2] == 0x9E)&&(pLocalInfo->aBD_ADDR[1] == 0x8B)&&(pLocalInfo->aBD_ADDR[0]<0x40))))
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("PBTPairingStart: BAD_PARAMETER");


      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext4( &sCallbackContext, P_DFC_TYPE_BT_PAIR,
         0, false, W_ERROR_BAD_PARAMETER );

      return;
   }

   if(phOperation != null)
   {
      *phOperation = W_NULL_HANDLE;
   }

   /* Create the BT_PAIR structure */
   pPairingOperation = (tPairingOperation*)CMemoryAlloc( sizeof(tPairingOperation) );
   if ( pPairingOperation == null )
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("PBTPairingStart: Cannot allocate the operation structure");


      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext4( &sCallbackContext, P_DFC_TYPE_BT_PAIR,
         0, false, W_ERROR_OUT_OF_RESOURCE );

      return;
   }
   CMemoryFill(pPairingOperation, 0, sizeof(tPairingOperation));

   /* Store the callback information */
   PDFCFillCallbackContext(
      pContext, (tDFCCallback*)pCallback, pCallbackParameter, &pPairingOperation->sCallbackContext );

   pPairingOperation->nMode = nMode;
   pPairingOperation->hVirtualTag = W_NULL_HANDLE;
   pPairingOperation->hReaderTag = W_NULL_HANDLE;
   pPairingOperation->hMessage = W_NULL_HANDLE;
   pPairingOperation->bTagTreatment = false;

   if((pLocalInfo == null) || (phOperation == null))
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }
   CMemoryCopy(&pPairingOperation->sLocalInfo, pLocalInfo, sizeof(tWBTPairingInfo));

   if (PReaderIsPropertySupported(pContext, W_PROP_NFC_TAG_TYPE_4_A) != false )
   {
      nReadAndCardProtocolSupported |= W_NFCC_PROTOCOL_READER_ISO_14443_4_A;
   }

   if (PReaderIsPropertySupported(pContext, W_PROP_NFC_TAG_TYPE_4_B) != false )
   {
      nReadAndCardProtocolSupported |= W_NFCC_PROTOCOL_READER_ISO_14443_4_B;
   }

   if (PEmulIsPropertySupported(pContext, W_PROP_NFC_TAG_TYPE_4_A) != false )
   {
      nReadAndCardProtocolSupported |= W_NFCC_PROTOCOL_CARD_ISO_14443_4_A;
   }

   if (PEmulIsPropertySupported(pContext, W_PROP_NFC_TAG_TYPE_4_B) != false )
   {
      nReadAndCardProtocolSupported |= W_NFCC_PROTOCOL_CARD_ISO_14443_4_B;
   }

   if(((nError = PNFCControllerGetBooleanProperty(pContext, W_NFCC_PROP_P2P,  &bValue)) == W_SUCCESS )
   && (bValue == true))
   {
      nReadAndCardProtocolSupported |= (W_NFCC_PROTOCOL_CARD_P2P_TARGET|W_NFCC_PROTOCOL_READER_P2P_INITIATOR);
   }

   if ((pLocalInfo->nAuthenticationDataType != W_BT_PAIRING_AUTH_PIN)||
      (pLocalInfo->nAuthenticationDataSize != sizeof(pLocalInfo->aAuthenticationData))||
      ((pLocalInfo->aBD_ADDR[2] == 0x9E)&&(pLocalInfo->aBD_ADDR[1] == 0x8B)&&(pLocalInfo->aBD_ADDR[0]<0x40)))
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   pPairingOperation->hPairingOperation = PBasicCreateOperation(
            pContext,
            static_PBTPairingCancelOperation,
            pPairingOperation);

   if(pPairingOperation->hPairingOperation == W_NULL_HANDLE)
   {
      PDebugError("PBTPairingStart: error on PBasicCreateOperation()");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   /* Add the generic operation structure */
   if ( ( nError = PHandleAddHeir(
                     pContext,
                     pPairingOperation->hPairingOperation,
                     pPairingOperation,
                     P_HANDLE_TYPE_BT_PAIR_OPERATION ) ) != W_SUCCESS )
   {
      PDebugError("PBTPairingStart: could not add the generic operation structure");
      goto return_error;
   }

   /* use a separate handle for user to avoid problems if user prematurely closes it */

   if (phOperation != null)
   {
      nError = PHandleDuplicate(pContext, pPairingOperation->hPairingOperation, phOperation);

      if (nError != W_SUCCESS)
      {
         PDebugError("PBTPairingStart: error on PHandleDuplicate()");
         goto return_error;
      }
   }


   pPairingOperation->nState = P_OPERATION_STATE_STARTED;

   nError = PNDEFCreateNewMessage(pContext, &hMessage);
   if (nError != W_SUCCESS)
   {
      goto return_error;
   }
   else
   {
      uint32_t nLocalDeviceNameLength;
      uint32_t nUtf8Len = 0;

      if (pLocalDeviceName != null)
      {
         uint32_t nIndex;

         nLocalDeviceNameLength = PUtilStringLength(pLocalDeviceName);

         if(nLocalDeviceNameLength > BT_PAIR_MAXIMUM_NAME_LENGTH)
         {
            nError = W_ERROR_BAD_PARAMETER;
            goto return_error;
         }

         for(nIndex = 0; nIndex < nLocalDeviceNameLength; nIndex++)
         {
            if(pLocalDeviceName[nIndex] < 0x20)
            {
               nError = W_ERROR_BAD_PARAMETER;
               goto return_error;
            }
         }

         if(nLocalDeviceNameLength != 0)
         {
            nUtf8Len = PUtilConvertUTF16ToUTF8( null, pLocalDeviceName, nLocalDeviceNameLength);
         }
      }
      else
      {
         nLocalDeviceNameLength = 0;
      }

      nPayloadLength = P_BT_PAIR_MINIMUM_PAYLOAD_SIZE + nUtf8Len;
      pPayloadBuffer = (uint8_t *)CMemoryAlloc(nPayloadLength);
      pBuffer = pPayloadBuffer;
      if (nMode == W_BT_PAIRING_TYPE_TAG)
      {
         (*pBuffer) = P_BT_PAIR_PASSIVE_PAIRING;
      }
      else
      {
         (*pBuffer) = P_BT_PAIR_ACTIVE_PAIRING;
      }
      pBuffer++;

      CMemoryCopy(pBuffer,pLocalInfo->aBD_ADDR,sizeof(pLocalInfo->aBD_ADDR));
      pBuffer = pBuffer + sizeof(pLocalInfo->aBD_ADDR);

      CMemoryCopy(pBuffer,pLocalInfo->aBTDeviceClass,sizeof(pLocalInfo->aBTDeviceClass));
      pBuffer = pBuffer + sizeof(pLocalInfo->aBTDeviceClass);

      CMemoryCopy(pBuffer,pLocalInfo->aAuthenticationData,sizeof(pLocalInfo->aAuthenticationData));
      pBuffer = pBuffer + sizeof(pLocalInfo->aAuthenticationData);

      (*pBuffer) = (uint8_t)nUtf8Len;

      if (nUtf8Len != 0)
      {
         (void)PUtilConvertUTF16ToUTF8( pBuffer+1, pLocalDeviceName, nLocalDeviceNameLength);
      }

      nError = PNDEFCreateRecord(pContext, W_NDEF_TNF_EXTERNAL, g_aTypeString, pPayloadBuffer, nPayloadLength, &hRecord);

      CMemoryFree(pPayloadBuffer);

      if (nError!= W_SUCCESS)
      {
         goto return_error;
      }

      nError = PNDEFAppendRecord(pContext, hMessage, hRecord);

      PHandleClose(pContext, hRecord);

      if (nError != W_SUCCESS)
      {
         goto return_error;
      }
   }
   pPairingOperation->hMessage = hMessage;

   nError = W_ERROR_RF_PROTOCOL_NOT_SUPPORTED;

   switch(nMode)
   {
      case W_BT_PAIRING_TYPE_TAG :
         if ((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_CARD_ISO_14443_4_A) != 0)
         {
            nTagType = W_PROP_NFC_TAG_TYPE_4_B;
         }
         else if ((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_CARD_ISO_14443_4_B) != 0)
         {
            nTagType = W_PROP_NFC_TAG_TYPE_4_A;
         }
         else
         {
            goto return_error;
         }

         nError = PVirtualTagCreate(pContext,nTagType, aPUPI, sizeof(aPUPI), P_BT_PAIR_MAXIMUM_MESSAGE_SIZE, &hVirtualTag);
         if(nError != W_SUCCESS)
         {
            goto return_error;
         }
         else
         {
            pPairingOperation->hVirtualTag = hVirtualTag;
            pPairingOperation->hConnection = hVirtualTag;
            PNDEFWriteMessage(pContext, hVirtualTag, static_PBTPairingWriteFirstMessageCompleted,
                           pPairingOperation, hMessage, W_NDEF_ACTION_BIT_ERASE | W_NDEF_ACTION_BIT_CHECK_WRITE, null);
         }

         break;

      case W_BT_PAIRING_TYPE_P2P:
      case W_BT_PAIRING_TYPE_P2P_MASTER:
      case W_BT_PAIRING_TYPE_P2P_SLAVE:

         if (((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_CARD_P2P_TARGET) == 0)&&
            ((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_READER_P2P_INITIATOR) == 0))
         {
            PDebugError("PBTPairingStart: P2P protocol not supported");
            goto return_error;
         }

         if (nMode == W_BT_PAIRING_TYPE_P2P)
         {
            nError = PP2PCreateSocketDriver(pContext, W_P2P_TYPE_CLIENT_SERVER, g_aPBTPairingServiceName, sizeof(g_aPBTPairingServiceName), 0, &pPairingOperation->hSocket);
         }
         else if (nMode == W_BT_PAIRING_TYPE_P2P_MASTER)
         {
            nError = PP2PCreateSocketDriver(pContext, W_P2P_TYPE_CLIENT, g_aPBTPairingServiceName, sizeof(g_aPBTPairingServiceName), 0, &pPairingOperation->hSocket);
         }
         else
         {
            nError = PP2PCreateSocketDriver(pContext, W_P2P_TYPE_SERVER, g_aPBTPairingServiceName, sizeof(g_aPBTPairingServiceName), 0, &pPairingOperation->hSocket);
         }

         if (nError != W_SUCCESS)
         {
            PDebugError("PBTPairingStart: PP2PCreateSocketWrapper failed");
            goto return_error;
         }


         PP2PEstablishLinkWrapper(pContext, static_PBTPairingP2PEstablishLinkCompleted, pPairingOperation, static_PBTPairingP2PReleaseLinkCompleted, pPairingOperation, null);
         break;

      case W_BT_PAIRING_TYPE_READER :
         {
            static const uint8_t aProtocolArray[] =
               {W_PROP_NFC_TAG_TYPE_4_A, W_PROP_NFC_TAG_TYPE_4_B, W_PROP_NFC_TAG_TYPE_1, W_PROP_NFC_TAG_TYPE_2,
               W_PROP_NFC_TAG_TYPE_3, W_PROP_NFC_TAG_TYPE_5, W_PROP_NFC_TAG_TYPE_6};

            nError = PReaderListenToCardDetection(pContext,
               static_PBTPairingReaderCardDetectionHandler, pPairingOperation,
               W_PRIORITY_MAXIMUM,
               aProtocolArray, sizeof(aProtocolArray),
               &hEventRegistry);
            pPairingOperation->hReaderTag = hEventRegistry;
            if (nError != W_SUCCESS)
            {
               goto return_error;
            }
         }
         break;

      default:
         nError = W_ERROR_BAD_PARAMETER;
         goto return_error;
   }

   return;

return_error:
   /* Send the error */
   PDebugWarning("PBTPairingStart: input parameter, memory or creation error");

   static_PBTPairingFinishOperation(pContext, pPairingOperation, false, nError);

   if(phOperation != null)
   {
      PHandleClose(pContext, *phOperation);
      *phOperation = W_NULL_HANDLE;
   }


}


#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
