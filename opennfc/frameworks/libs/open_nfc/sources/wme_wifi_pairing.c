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
   Contains the Wi-Fi pairing implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( WIFI_PAIR )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* The maximum length in Unicode character of a Device Password, not including the terminating zero */
#define WIFI_MAXIMUM_DEVICE_PASSWORD_LENGTH 32

/* The maximum length in Unicode character of a Device Password, not including the terminating zero */
#define WIFI_MINIMUM_DEVICE_PASSWORD_LENGTH 16

/* The maximum length in Unicode character of a Device Password, not including the terminating zero */
#define WIFI_MAXIMUM_MESSAGE_LENGTH 0x80FC

/* The minimum length in Unicode character of a OOB Device Password */
#define P_WIFI_PAIR_OOB_DEVICE_PASSWORD_MINIMUM_LENGTH 38

/* The minimum length in Unicode character of a OOB Device Password */
#define P_WIFI_PAIR_OOB_DEVICE_PASSWORD_MAXIMUM_LENGTH 54

/* The minimum length in Unicode character of a OOB Device Password */
#define P_WIFI_PAIR_CREDENTIAL_MINIMUM_LENGTH 35

/* The size of the TL of the TLV attribute */
#define P_WIFI_PAIR_TYPE_LENGTH 4

/* The size of the T of the TLV attribute */
#define P_WIFI_PAIR_TYPE 2

/* The size of the T of the TLV attribute */
#define P_WIFI_PAIR_OOB_HASH 20

/* The attribute ID of the credential */
#define P_WIFI_PAIR_CREDENTIAL_ATTRIBUTE_ID        0x100E

/* The attribute ID of the OOB Device Password */
#define P_WIFI_PAIR_OOB_DEVICE_PASSWORD_ATTRIBUTE_ID 0x102C

/* The attribute ID of the version */
#define P_WIFI_PAIR_VERSION_ATTRIBUTE_ID        0x1044

/* The P2P service name for the Wi-Fi pairing */
static const tchar g_aPWIFIPairingServiceName[] =
   { 'n', 'f', 'c', ':', 'w', 'i', 'f', 'i', '-', 'p', 'a', 'i', 'r', 'i', 'n', 'g', 0 };

typedef struct __tWIFIPairingOperation
{
   /* Operation header */
   tHandleObjectHeader           sObjectHeader;

   /* WIFI_PAIR operation handle */
   W_HANDLE                      hVirtualTag;
   W_HANDLE                      hReaderTag;
   W_HANDLE                      hReadOperation;
   W_HANDLE                      hConnection;
   W_HANDLE                      hWriteOperation;
   W_HANDLE                      hSocket;             /* P2P socket */
   W_HANDLE                      hLinkOperation;      /* P2P link establisment operation */
   W_HANDLE                      hLink;               /* P2P link handle */
   bool                          bIsSocketConnected;

   /* Command state */
   uint32_t                      nMode;

   uint8_t                       nVersion;

   uint8_t                       aCredentialBuffer[WIFI_MAXIMUM_MESSAGE_LENGTH];
   uint32_t                      nCredentialLength;

   tWWIFIOOBDevicePasswordInfo   sOOBDevicePasswordInfo;
   uint8_t                       aDevicePasswordBuffer[WIFI_MAXIMUM_DEVICE_PASSWORD_LENGTH];
   uint32_t                      nDevicePasswordLength;

   uint8_t                       aMessageMBuffer[WIFI_MAXIMUM_MESSAGE_LENGTH];
   uint32_t                      nMessageMLength;

   bool                          bIsEnrollee;
   bool                          bIsTagAlreadyWritten;

   W_HANDLE                      hMessage;
   W_ERROR                       nResultError;

   /* Callback context */
   tDFCCallbackContext           sCallbackContext;

   tDFCCallbackContext           sEventReadCallbackContext;
   tDFCCallbackContext           sEventWriteCallbackContext;

   W_HANDLE                      hPairingOperation;
   uint8_t                       nState;

} tWIFIPairingOperation;

static const tchar g_aWIFITypeString[] = {0x61, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x2F, 0x76, 0x6E, 0x64, 0x2E, 0x77, 0x66,
                                       0x61, 0x2E, 0x77, 0x73, 0x63, 0x00};

static const uint8_t g_aWIFITypeStringUTF8[] = {0x61, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x2F, 0x76, 0x6E, 0x64, 0x2E, 0x77, 0x66,
                                       0x61, 0x2E, 0x77, 0x73, 0x63, 0x00};

static const uint8_t g_aWIFIPUPI[] = { 0x01, 0x02, 0x03, 0x04 };

static void static_PWIFIEnrolleePairingWriteMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError);

/* Destroy operation callback */
static uint32_t static_PWIFIPairingDestroyOperation(
            tContext* pContext,
            void* pObject );

/* Operation registry Wi-Fi pairing type */
tHandleType g_sWIFI_PAIROperation = { static_PWIFIPairingDestroyOperation, null, null, null, null };
#define P_HANDLE_TYPE_WIFI_PAIR_OPERATION (&g_sWIFI_PAIROperation)


static void static_PWIFIPairingGenericCloseHandle(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation)
{
      PHandleClose( pContext, pPairingOperation->hVirtualTag);
      PHandleClose( pContext, pPairingOperation->hReaderTag);
      PHandleClose( pContext, pPairingOperation->hMessage);
      PHandleClose( pContext, pPairingOperation->hConnection);
      PHandleClose( pContext, pPairingOperation->hReadOperation);
      PHandleClose( pContext, pPairingOperation->hWriteOperation);
      PHandleClose( pContext, pPairingOperation->hSocket);
}

static void static_PWIFIPairingGenericInitHandle(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation)
{
   pPairingOperation->hVirtualTag = W_NULL_HANDLE;
   pPairingOperation->hReaderTag = W_NULL_HANDLE;
   pPairingOperation->hMessage = W_NULL_HANDLE;
   pPairingOperation->hConnection = W_NULL_HANDLE;
   pPairingOperation->hReadOperation = W_NULL_HANDLE;
   pPairingOperation->hWriteOperation = W_NULL_HANDLE;

   pPairingOperation->hLink = W_NULL_HANDLE;
   pPairingOperation->hLinkOperation = W_NULL_HANDLE;
   pPairingOperation->hSocket = W_NULL_HANDLE;

}

static void static_PWIFIPairingCancelOperation(
         tContext* pContext,
         void* pCancelParameter,
         bool bIsClosing);

static W_ERROR static_PWIFIPairingGenericHandleRegister(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_HANDLE *phOperation)
{
   W_ERROR nError;

   pPairingOperation->hPairingOperation = PBasicCreateOperation(
            pContext,
            static_PWIFIPairingCancelOperation,
            pPairingOperation);

   if (phOperation != null)
   {
      *phOperation = pPairingOperation->hPairingOperation;
   }

   if(pPairingOperation->hPairingOperation == W_NULL_HANDLE)
   {
      PDebugError("static_PWIFIPairingGenericHandleRegister: error on PBasicCreateOperation()");
      return W_ERROR_OUT_OF_RESOURCE;
   }

   /* Add the generic operation structure */
   if ( ( nError = PHandleAddHeir(
                     pContext,
                     pPairingOperation->hPairingOperation,
                     pPairingOperation,
                     P_HANDLE_TYPE_WIFI_PAIR_OPERATION ) ) != W_SUCCESS )
   {
      PDebugError("static_PWIFIPairingGenericHandleRegister: could not add the generic operation structure");
   }

   return nError;
}

/**
 * @brief   Destroyes a WIFI operation object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PWIFIPairingDestroyOperation(
            tContext* pContext,
            void* pObject )
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pObject;

   PDebugTrace("static_PWIFIPairingDestroyOperation");

   static_PWIFIPairingGenericCloseHandle(pContext,pPairingOperation);

   /* Free the operation structure */
   CMemoryFree( pPairingOperation );

   return P_HANDLE_DESTROY_DONE;
}

static void static_PWIFIEnrolleePairingFinishOperation(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_ERROR nResultError);


static void static_PWIFIEnrolleePairingShutdownCompleted(tContext * pContext, void * pCallbackParameter, W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   pPairingOperation->bIsSocketConnected = false;
   static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, pPairingOperation->nResultError);
}

/* Finish the operation */
static void static_PWIFIEnrolleePairingFinishOperation(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_ERROR nResultError)
{
   W_ERROR nError;
   pPairingOperation->nResultError = nResultError;

   PBasicSetOperationCompleted(pContext, pPairingOperation->hPairingOperation);
   pPairingOperation->nState = P_OPERATION_STATE_COMPLETED;

   if (pPairingOperation->bIsSocketConnected)
   {
      PDebugTrace("static_PWIFIEnrolleePairingFinishOperation : PP2PShutdown");
      PP2PShutdownDriver(pContext, pPairingOperation->hSocket, static_PWIFIEnrolleePairingShutdownCompleted, pPairingOperation);

      /* @todo What to do if the IOCTL failed */
      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_WIFI_PAIR, static_PWIFIEnrolleePairingShutdownCompleted, pPairingOperation, nError);
      }
      return;
   }

   if (pPairingOperation->hLink != W_NULL_HANDLE)
   {
      PDebugTrace("static_PBTPairingFinishOperation : Close link");
      PHandleClose(pContext, pPairingOperation->hLink);
      pPairingOperation->hLink = W_NULL_HANDLE;
   }

   static_PWIFIPairingGenericCloseHandle(pContext,pPairingOperation);
   static_PWIFIPairingGenericInitHandle(pContext,pPairingOperation);


   if(nResultError != W_SUCCESS)
   {
      PDebugError("static_PWIFIEnrolleePairingFinishOperation: return the error %s",
         PUtilTraceError(nResultError));

      PDFCPostContext5(
         &pPairingOperation->sCallbackContext,
         P_DFC_TYPE_WIFI_PAIR,
         (uint32_t)0xFF,
         0,
         0,
         nResultError );
   }
   else
   {
      switch (pPairingOperation->nMode)
      {
         case W_WIFI_PAIRING_PASSWORD_TOKEN:
            PDFCPostContext5(
               &pPairingOperation->sCallbackContext,
               P_DFC_TYPE_WIFI_PAIR,
               (uint32_t)W_WIFI_PAIRING_PASSWORD_TOKEN,
               0,
               pPairingOperation->nDevicePasswordLength,
               nResultError );
            break;
         case W_WIFI_PAIRING_CONFIGURATION_TOKEN:
            PDFCPostContext5(
               &pPairingOperation->sCallbackContext,
               P_DFC_TYPE_WIFI_PAIR,
               (uint32_t)W_WIFI_PAIRING_CONFIGURATION_TOKEN,
               pPairingOperation->nCredentialLength,
               0,
               nResultError );
            break;
         case W_WIFI_PAIRING_TOUCHING_DEVICE:
            PDFCPostContext5(
               &pPairingOperation->sCallbackContext,
               P_DFC_TYPE_WIFI_PAIR,
               (uint32_t)W_WIFI_PAIRING_TOUCHING_DEVICE,
               0,
               0,
               nResultError );
            break;
         default:
            break;
      }
   }

   PHandleClose(pContext, pPairingOperation->hPairingOperation);
}


static void static_PWIFIEnrolleePairingReadMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_HANDLE hMessage,
            W_ERROR nError
   )
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;
   W_HANDLE hVirtualTagOperation;
   uint8_t *  pPayloadBuffer = null;
   W_HANDLE hRecord;

   PDebugTrace("static_PWIFIEnrolleePairingReadMessageCompleted");

   if (pPairingOperation->nState == P_OPERATION_STATE_COMPLETED)
   {
      PHandleClose(pContext, hMessage);
      return;
   }

   if (nError == W_SUCCESS)
   {
      if ((hRecord = PNDEFGetRecord(pContext, hMessage, 0)) == W_NULL_HANDLE)
      {
         PDebugError("static_PWIFIEnrolleePairingReadMessageCompleted: Error returned by PNDEFGetRecord");
         return;
      }

      PNDEFGetRecordInfo(pContext, hRecord, W_NDEF_INFO_PAYLOAD, &pPairingOperation->nMessageMLength);

      PNDEFGetPayloadPointer(pContext,hRecord,(uint8_t**)&pPayloadBuffer);

      if (pPairingOperation->nMessageMLength > 0)
      {
         CMemoryCopy(pPairingOperation->aMessageMBuffer,pPayloadBuffer,pPairingOperation->nMessageMLength);
      }

      PHandleClose( pContext, hRecord);

      static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
   }
   else
   {
      if (pPairingOperation->hVirtualTag == W_NULL_HANDLE)
      {
         PDebugWarning("static_PWIFIEnrolleePairingReadMessageCompleted: the virtual tag handle is null");

         static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation,W_ERROR_BAD_STATE);
         return;
      }

      PNDEFWriteMessage(pContext, pPairingOperation->hVirtualTag, static_PWIFIEnrolleePairingWriteMessageCompleted,
            pPairingOperation, pPairingOperation->hMessage, W_NDEF_ACTION_BIT_ERASE | W_NDEF_ACTION_BIT_CHECK_WRITE, &hVirtualTagOperation);
   }

   PHandleClose(pContext, hMessage);
}

static void static_PWIFIEnrolleePairingStopCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   W_HANDLE hOperation;
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIEnrolleePairingStopCompleted");

   if ( pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError == W_SUCCESS)
      {
         PNDEFReadMessage(pContext, pPairingOperation->hVirtualTag, static_PWIFIEnrolleePairingReadMessageCompleted,
            pPairingOperation, W_NDEF_TNF_MEDIA, g_aWIFITypeString, &hOperation);
         return;
      }

      PDebugWarning("static_PWIFIEnrolleePairingStopCompleted: tag stop error");

      static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
   }
}

static void static_PWIFIPairingEventDetected(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nEventCode)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIPairingEventDetected");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      switch(nEventCode)
      {
         case W_VIRTUAL_TAG_EVENT_READER_WRITE :

            PDebugTrace("static_PWIFIPairingEventDetected: W_VIRTUAL_TAG_EVENT_READER_WRITE");

            PVirtualTagStop( pContext, pPairingOperation->hVirtualTag, static_PWIFIEnrolleePairingStopCompleted,
                  pPairingOperation);
            break;

         case W_VIRTUAL_TAG_EVENT_READER_READ_ONLY :
         case W_VIRTUAL_TAG_EVENT_SELECTION :
         case W_VIRTUAL_TAG_EVENT_READER_LEFT :
         default :
            break;
      }
   }
}


static void static_PWIFIPairingStartCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIPairingStartCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PWIFIPairingStartCompleted: tag activation error");

         static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
      }
   }
}


static void static_PWIFIEnrolleePairingWriteMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIEnrolleePairingWriteMessageCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PWIFIEnrolleePairingWriteMessageCompleted: tag write error");
         static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
      }
      else
      {
         PVirtualTagStart(pContext,pPairingOperation->hVirtualTag, static_PWIFIPairingStartCompleted,
            pPairingOperation,static_PWIFIPairingEventDetected, pPairingOperation, false);
      }
   }
}


static uint16_t static_PWIFIGetUint16Value(
            uint8_t *pBuffer)
{
   return((*pBuffer)<<8) + (*(pBuffer+1));
}

static bool static_PWIFICheckAttribute(
            uint16_t    nValue,
            uint8_t     **pBuffer,
            uint16_t     *nLengthAttribute,
            tWIFIPairingOperation* pPairingOperation)
{
   uint16_t nAttributeID;
   uint16_t i;
   bool bAttributeFound;

   *pBuffer = &pPairingOperation->aMessageMBuffer[0];
   i = 0;
   bAttributeFound = false;
   while ((bAttributeFound == false) && (i<pPairingOperation->nMessageMLength))
   {
      nAttributeID = static_PWIFIGetUint16Value(*pBuffer);
      *pBuffer = *pBuffer + P_WIFI_PAIR_TYPE;
      (*nLengthAttribute) = static_PWIFIGetUint16Value(*pBuffer);
      if (nAttributeID == nValue)
      {
         (bAttributeFound) = true;
         *pBuffer = *pBuffer + P_WIFI_PAIR_TYPE;
         break;
      }
      else
      {
         *pBuffer = *pBuffer + P_WIFI_PAIR_TYPE + (*nLengthAttribute);
         i = i + P_WIFI_PAIR_TYPE_LENGTH + (*nLengthAttribute);
      }
   }

   return(bAttributeFound);
}

static W_ERROR static_PWIFIPairingCheckOOBDEvicePassword(
            uint8_t* pBuffer,
            tWIFIPairingOperation* pPairingOperation,
            uint8_t *  pPayloadBuffer,
            uint32_t   nPayloadBufferLength)
{
   if ((nPayloadBufferLength < P_WIFI_PAIR_OOB_DEVICE_PASSWORD_MINIMUM_LENGTH)
      || (nPayloadBufferLength > P_WIFI_PAIR_OOB_DEVICE_PASSWORD_MAXIMUM_LENGTH))
   {
      PDebugError("static_PWIFIPairingCheckOOBDEvicePassword: Out of range");
      return(W_ERROR_BAD_NDEF_FORMAT);
   }

   CMemoryCopy(pPairingOperation->sOOBDevicePasswordInfo.aPublicKeyHashData,pBuffer,
      sizeof(pPairingOperation->sOOBDevicePasswordInfo.aPublicKeyHashData));
   pBuffer = pBuffer + sizeof(pPairingOperation->sOOBDevicePasswordInfo.aPublicKeyHashData);
   pPairingOperation->sOOBDevicePasswordInfo.nPasswordId = ((*pBuffer)<<8) + (*(pBuffer+1));;
   pBuffer = pBuffer + sizeof(pPairingOperation->sOOBDevicePasswordInfo.nPasswordId);
   pPairingOperation->nDevicePasswordLength = (uint16_t)nPayloadBufferLength - P_WIFI_PAIR_TYPE - P_WIFI_PAIR_OOB_HASH;
   if ((pPairingOperation->nDevicePasswordLength > WIFI_MAXIMUM_DEVICE_PASSWORD_LENGTH) ||
      (pPairingOperation->nDevicePasswordLength < WIFI_MINIMUM_DEVICE_PASSWORD_LENGTH))
   {
      PDebugError("static_PWIFIPairingCheckOOBDEvicePassword: invalid device password size ");
      return(W_ERROR_BAD_NDEF_FORMAT);

   }
   CMemoryCopy(pPairingOperation->aDevicePasswordBuffer,pBuffer, pPairingOperation->nDevicePasswordLength);

   if ((pPairingOperation->nMessageMLength - 1 - (2*P_WIFI_PAIR_TYPE_LENGTH)) != nPayloadBufferLength)
   {
      PDebugError("static_PWIFIPairingCheckOOBDEvicePassword: invalid device password length ");
      return(W_ERROR_BAD_NDEF_FORMAT);
   }

   if (pPairingOperation->sOOBDevicePasswordInfo.nPasswordId <= 0x000F)
   {
      PDebugError("static_PWIFIPairingCheckOOBDEvicePassword: reserved Password ID");
      return(W_ERROR_BAD_NDEF_FORMAT);
   }

   return(W_SUCCESS);
}

static W_ERROR static_PWIFIPairingCheckRemoteMessage(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_HANDLE hMessage)
{
   W_HANDLE hRecord;
   uint8_t *  pPayloadBuffer = null;
   uint32_t   nPayloadBufferLength;
   uint16_t   nLengthAttribute;
   uint8_t *pBuffer = null;

   if ((hRecord = PNDEFGetRecord(pContext, hMessage, 0)) == W_NULL_HANDLE)
   {
      PDebugError("static_PWIFIPairingCheckRemoteMessage: Error returned by PNDEFGetRecord");
      goto return_error_bad_format;
   }

   PNDEFGetRecordInfo(pContext, hRecord, W_NDEF_INFO_PAYLOAD, &nPayloadBufferLength);

   if (PNDEFGetPayloadPointer(pContext, hRecord,(uint8_t**)&pPayloadBuffer) != W_SUCCESS )
   {
      PDebugError("static_PWIFIPairingCheckRemoteMessage: Cannot extract the payload");
      goto return_error_bad_format;
   }

   pBuffer = (pPayloadBuffer) + P_WIFI_PAIR_TYPE_LENGTH;
   pPairingOperation->nMessageMLength = nPayloadBufferLength;
   CMemoryCopy(pPairingOperation->aMessageMBuffer,pPayloadBuffer,nPayloadBufferLength);

   if (static_PWIFICheckAttribute((uint16_t)P_WIFI_PAIR_VERSION_ATTRIBUTE_ID,(uint8_t **) &pBuffer, &nLengthAttribute,
      pPairingOperation) != true)
   {
      goto return_error_bad_format;
   }
   else
   {
      pPairingOperation->nVersion = (*pBuffer);

   }

   if (static_PWIFICheckAttribute((uint16_t)P_WIFI_PAIR_CREDENTIAL_ATTRIBUTE_ID,(uint8_t **) &pBuffer, &nLengthAttribute,
      pPairingOperation) == true)
   {
      pPairingOperation->nMode = W_WIFI_PAIRING_CONFIGURATION_TOKEN;

      if ((nLengthAttribute < P_WIFI_PAIR_CREDENTIAL_MINIMUM_LENGTH)
         || (nLengthAttribute > WIFI_MAXIMUM_MESSAGE_LENGTH))
      {
         PDebugError("static_PWIFIPairingCheckRemoteMessage: Out of range");
         PHandleClose( pContext, hRecord);
         return(W_ERROR_BAD_NDEF_FORMAT);
      }
      pPairingOperation->nCredentialLength = nLengthAttribute;

      CMemoryCopy(pPairingOperation->aCredentialBuffer,pBuffer,nLengthAttribute);
   }
   else if (static_PWIFICheckAttribute((uint16_t)P_WIFI_PAIR_OOB_DEVICE_PASSWORD_ATTRIBUTE_ID,(uint8_t **) &pBuffer, &nLengthAttribute,
      pPairingOperation) == true)
   {
      pPairingOperation->nMode = W_WIFI_PAIRING_PASSWORD_TOKEN;

      if (static_PWIFIPairingCheckOOBDEvicePassword(pBuffer, pPairingOperation,
         pBuffer, nLengthAttribute) != W_SUCCESS)
      {
         PDebugError("static_PWIFIPairingCheckRemoteMessage: invalid OOB Device Password ");
         goto return_error_bad_format;
      }
   }
   else
   {
      PDebugError("static_PWIFIPairingCheckRemoteMessage: Cannot find credential or OOB device password ");
      goto return_error_bad_format;
   }

   PHandleClose( pContext, hRecord);
   PHandleClose( pContext, hMessage);
   return W_SUCCESS;

return_error_bad_format:

   PDebugWarning("static_PWIFIPairingCheckRemoteMessage: Bad Wi-Fi parameters");
   PHandleClose( pContext, hRecord);
   PHandleClose( pContext, hMessage);
   return W_ERROR_BAD_NDEF_FORMAT;
}

static void static_PWIFIReadMessageOnAnyTagCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_HANDLE hMessage,
            W_ERROR nError )
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIReadMessageOnAnyTagCompleted");

   if (nError != W_ERROR_CANCEL)
   {
      /* Check the operation state */
      if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
      {
         if (nError != W_SUCCESS)
         {
            PDebugWarning("static_PWIFIReadMessageOnAnyTagCompleted: read error");
            PHandleClose( pContext, hMessage);
            static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
         }
         else
         {
            nError = static_PWIFIPairingCheckRemoteMessage(pContext, pPairingOperation, hMessage);

            if (nError != W_SUCCESS)
            {
               PDebugWarning("static_PWIFIReadMessageOnAnyTagCompleted: Bad Wi-Fi parameters");

               static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
               return;
            }
            else
            {
               static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
            }
         }
      }
   }
   else
   {
      PHandleClose( pContext, hMessage);
   }
}

static void static_PWIFIPairingCancelOperation(
         tContext* pContext,
         void* pCancelParameter,
         bool bIsClosing)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCancelParameter;

   PDebugTrace("static_PWIFIPairingCancelOperation");

   if (bIsClosing == true)
   {
      PDebugTrace("static_PWIFIPairingCancelOperation : bIsClosing is true, do nothing");
      return;
   }

   /* Check the operation state */
   if ( PBasicGetOperationState(pContext, pPairingOperation->hPairingOperation) == P_OPERATION_STATE_CANCELLED )
   {
      /* Send the error */
      if (pPairingOperation->hWriteOperation != W_NULL_HANDLE)
         PBasicCancelOperation( pContext, pPairingOperation->hWriteOperation);

      if (pPairingOperation->hReadOperation != W_NULL_HANDLE)
         PBasicCancelOperation( pContext, pPairingOperation->hReadOperation);

      if (pPairingOperation->hLinkOperation != W_NULL_HANDLE)
         PBasicCancelOperation( pContext, pPairingOperation->hLinkOperation);

      static_PWIFIPairingGenericCloseHandle(pContext,pPairingOperation);
      static_PWIFIPairingGenericInitHandle(pContext,pPairingOperation);
   }
}

void static_PWIFIEnrolleePairingP2PReadCompleted(
   tContext* pContext,
   void *pCallbackParameter,
   uint32_t nDataLength,
   W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;
   uint8_t *  pPayloadBuffer = null;
   uint32_t   nPayloadBufferLength;
   uint32_t   nTypeStringLength;
   uint8_t* pReceptionBuffer = pPairingOperation->aMessageMBuffer;

   PDebugTrace("static_PWIFIEnrolleePairingReadCompleted");

   PHandleClose(pContext, pPairingOperation->hReadOperation);
   pPairingOperation->hReadOperation = W_NULL_HANDLE;

   if (pPairingOperation->nState == P_OPERATION_STATE_COMPLETED)
   {
      PDebugTrace("static_PWIFIEnrolleePairingReadCompleted: BAD STATE");
      nError = W_ERROR_BAD_STATE;
      goto end;
   }

   if (nError != W_SUCCESS)
   {
      PDebugTrace("static_PWIFIEnrolleePairingReadCompleted: nError");
      goto end;
   }

   pReceptionBuffer++;
   nTypeStringLength =  (*pReceptionBuffer);
   pReceptionBuffer++;
   nPayloadBufferLength = (*pReceptionBuffer);
   pReceptionBuffer++;

   if (CMemoryCompare(pReceptionBuffer,g_aWIFITypeStringUTF8,nTypeStringLength) !=0)
   {
      PDebugWarning("static_PWIFIEnrolleePairingReadCompleted: not a wi-fi message");
      nError = W_ERROR_BAD_TAG_FORMAT;
      goto end;
   }

   pPayloadBuffer = pReceptionBuffer+nTypeStringLength;

   if (nPayloadBufferLength > 0)
   {
      CMemoryCopy(pPairingOperation->aMessageMBuffer,pPayloadBuffer,nPayloadBufferLength);
      pPairingOperation->nMessageMLength = nPayloadBufferLength;
   }
   else
   {
      pPairingOperation->nMessageMLength = 0;
   }

end:
   static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
}


void static_PWIFIEnrolleePairingP2PWriteCompleted(
   tContext* pContext,
   void *pCallbackParameter,
   W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIEnrolleePairingWriteCompleted");

   PHandleClose(pContext, pPairingOperation->hWriteOperation);
   pPairingOperation->hWriteOperation = W_NULL_HANDLE;

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PWIFIEnrolleePairingWriteCompleted: tag write error");
         static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
      }
      else
      {
         PP2PReadDriver(pContext, pPairingOperation->hSocket, static_PWIFIEnrolleePairingP2PReadCompleted, pPairingOperation,
            pPairingOperation->aMessageMBuffer, WIFI_MAXIMUM_MESSAGE_LENGTH, &pPairingOperation->hReadOperation);

         nError = PContextGetLastIoctlError(pContext);
         if (nError != W_SUCCESS)
         {
            PDFCPost3(pContext, P_DFC_TYPE_BT_PAIR, static_PWIFIEnrolleePairingP2PReadCompleted, pPairingOperation, 0, nError);
         }
      }
   }
}


void static_PWIFIEnrolleePairingP2PConnectCompleted(tContext* pContext, void *pCallbackParameter, W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;
   uint32_t nSendBufferLength, pnActualLength;
   uint8_t * pSendBuffer;

   PDebugTrace("static_PWIFIEnrolleePairingConnectCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError == W_SUCCESS)
      {
         nSendBufferLength = PNDEFGetMessageLength(pContext, pPairingOperation->hMessage);
         pSendBuffer = (uint8_t *)CMemoryAlloc(nSendBufferLength);
         PNDEFGetMessageContent(pContext, pPairingOperation->hMessage, pSendBuffer, nSendBufferLength,&pnActualLength);

         PP2PWrite(pContext,pPairingOperation->hSocket, static_PWIFIEnrolleePairingP2PWriteCompleted, pPairingOperation,
            pSendBuffer, nSendBufferLength, & pPairingOperation->hWriteOperation);

         CMemoryFree(pSendBuffer);
      }
      else
      {
         /* Send the error */
         PDebugWarning("static_PWIFIEnrolleePairingConnectCompleted: card detection error");
         static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);
      }
   }
}

static void static_PWIFIEnrolleePairingP2PLinkEstablished(
            tContext * pContext,
            void* pCallbackParameter,
            W_HANDLE hHandle,
            W_ERROR nResult )
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;
   W_ERROR nError;

   PDebugTrace("static_PWIFIEnrolleePairingP2PLinkEstablished : %s", PUtilTraceError(nResult));

   PHandleClose(pContext, pPairingOperation->hLinkOperation);
   pPairingOperation->hLinkOperation = W_NULL_HANDLE;

   if (nResult == W_SUCCESS)
   {
      pPairingOperation->hLink = hHandle;

      PP2PConnectDriver(pContext, pPairingOperation->hSocket, pPairingOperation->hLink, static_PWIFIEnrolleePairingP2PConnectCompleted, pPairingOperation);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_WIFI_PAIR, static_PWIFIEnrolleePairingP2PConnectCompleted, pPairingOperation, nError);
      }


   }
   else
   {
      /* Send the error */
      PDebugWarning("static_PWIFIEnrolleePairingP2PLinkEstablished: P2P link establishment error");

      static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nResult);
   }
}


static void static_PWIFIEnrolleePairingP2PLinkReleased(
         tContext * pContext,
         void * pCallbackParameter,
         W_ERROR nError)
{
   PDebugTrace("static_PWIFIEnrolleePairingP2PLinkReleased: P2P link released");
}


/* See Client API Specifications */
void PWIFIEnrolleePairingStart(
         tContext* pContext,
         const uint8_t *pMessageM1,
         uint32_t nMessageM1Length,
         tPWIFIEnrolleePairingStartCompleted *pCallback,
         void *pCallbackParameter,
         W_HANDLE *phOperation)
{
   W_ERROR nError = W_SUCCESS;
   W_HANDLE hMessage = W_NULL_HANDLE, hRecord;
   W_HANDLE hReadOperation;
   uint8_t nReadAndCardProtocolSupported = 0;
   tWIFIPairingOperation* pPairingOperation;
   bool   bValue;

   /* chech phOperation is not null */
   if(phOperation == null)
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("PWIFIEnrolleePairingStart: phOperation is null");

      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext5( &sCallbackContext, P_DFC_TYPE_WIFI_PAIR,
         0xFF, 0, 0, W_ERROR_BAD_PARAMETER );

      return;
   }

   * phOperation = W_NULL_HANDLE;

   /* Create the WIFI structure */
   pPairingOperation = (tWIFIPairingOperation*)CMemoryAlloc( sizeof(tWIFIPairingOperation) );
   if ( pPairingOperation == null )
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("PWIFIEnrolleePairingStart: Cannot allocate the operation structure");


      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext5( &sCallbackContext, P_DFC_TYPE_WIFI_PAIR,
         0xFF, 0, 0, W_ERROR_OUT_OF_RESOURCE );

      return;
   }

   CMemoryFill(pPairingOperation, 0, sizeof(tWIFIPairingOperation));

   /* Store the callback information */
   PDFCFillCallbackContext(
      pContext, (tDFCCallback*)pCallback, pCallbackParameter, &pPairingOperation->sCallbackContext );

   static_PWIFIPairingGenericInitHandle(pContext,pPairingOperation);

   if(((nError = PNFCControllerGetBooleanProperty(pContext, W_NFCC_PROP_P2P,  &bValue)) == W_SUCCESS )
   && (bValue == true))
   {
      nReadAndCardProtocolSupported |= (W_NFCC_PROTOCOL_CARD_P2P_TARGET|W_NFCC_PROTOCOL_READER_P2P_INITIATOR);
   }

   if ( (nError = static_PWIFIPairingGenericHandleRegister(
                     pContext,
                     pPairingOperation,
                     null))!= W_SUCCESS )
   {
      PDebugError("PWIFIEnrolleePairingStart: error on static_PWIFIPairingGenericHandleRegister()");
      goto return_error;
   }

   /* Duplicate the operation handle for user */

   if ((nError = PHandleDuplicate(pContext, pPairingOperation->hPairingOperation, phOperation)) != W_SUCCESS)
   {
      PDebugError("PWIFIEnrolleePairingStart: error on PHandleDuplicate()");
      goto return_error;
   }

   pPairingOperation->bIsEnrollee = true;

   if ((pMessageM1 != null) && (nMessageM1Length != 0))
   {
      pPairingOperation->nMode = W_WIFI_PAIRING_TOUCHING_DEVICE;
      nError = PNDEFCreateNewMessage(pContext, &hMessage);
      if (nError != W_SUCCESS)
      {
         goto return_error;
      }
      else
      {
         pPairingOperation->hMessage = hMessage;

         nError = PNDEFCreateRecord(pContext, W_NDEF_TNF_MEDIA, g_aWIFITypeString, pMessageM1, nMessageM1Length, &hRecord);

         if (nError != W_SUCCESS)
         {
            goto return_error;
         }

         nError = PNDEFAppendRecord(pContext, hMessage, hRecord);
         PHandleClose(pContext, hRecord);

         if (nError != W_SUCCESS)
         {
            goto return_error;
         }

         if (((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_CARD_P2P_TARGET) == 0)&&
            ((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_READER_P2P_INITIATOR) == 0))
         {
            nError = W_ERROR_RF_PROTOCOL_NOT_SUPPORTED;
            goto return_error;
         }

         nError = PP2PCreateSocketDriver(pContext, W_P2P_TYPE_CLIENT,
               g_aPWIFIPairingServiceName, (1 + PUtilStringLength(g_aPWIFIPairingServiceName)) * sizeof (tchar), 0, &pPairingOperation->hSocket);

         if (nError != W_SUCCESS)
         {
            goto return_error;
         }

         PP2PEstablishLinkWrapper(pContext, static_PWIFIEnrolleePairingP2PLinkEstablished, pPairingOperation, static_PWIFIEnrolleePairingP2PLinkReleased, null, &pPairingOperation->hLinkOperation);
      }
   }
   else
   {
      PNDEFReadMessageOnAnyTag(pContext, static_PWIFIReadMessageOnAnyTagCompleted,
         pPairingOperation, W_PRIORITY_MAXIMUM, W_NDEF_TNF_MEDIA, g_aWIFITypeString, &hReadOperation);

      pPairingOperation->hReadOperation = hReadOperation;
   }
   return;

return_error:
   /* Send the error */
   PDebugWarning("PWIFIEnrolleePairingStart: input parameter, memory or creation error");

   static_PWIFIEnrolleePairingFinishOperation(pContext, pPairingOperation, nError);

   PHandleClose(pContext, hMessage);
   hMessage = W_NULL_HANDLE;

   if(phOperation != null)
   {
      PHandleClose(pContext, *phOperation);
      *phOperation = W_NULL_HANDLE;
   }
}

W_ERROR static_PWIFIGetStringInfo(
         tContext* pContext,
         W_HANDLE hOperation,
         uint8_t * pStringBuffer,
         uint32_t nStringLength,
         tWIFIPairingOperation* pPairingOperation,
         uint8_t *pReferenceBuffer)
{
   PDebugTrace("static_PWIFIGetStringInfo");

   if ((pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   || (pPairingOperation->nResultError != W_SUCCESS))
   {
      PDebugWarning("static_PWIFIGetStringInfo: bad state");
      return W_ERROR_BAD_STATE;
   }

   if (pPairingOperation->nResultError == W_SUCCESS)
   {
      if(pStringBuffer != null)
      {
         uint32_t nLengthToCopy = nStringLength;

         if(nLengthToCopy != 0)
         {
            CMemoryCopy(pStringBuffer,
               pReferenceBuffer,
               nLengthToCopy << 1);
         }
         pStringBuffer[nLengthToCopy] = 0;
      }
   }
   else
   {
      return(pPairingOperation->nResultError);
   }

   return W_SUCCESS;
}

/* See Client API Specifications */
W_ERROR PWIFIGetOOBDevicePasswordInfo(
         tContext* pContext,
         W_HANDLE hOperation,
         tWWIFIOOBDevicePasswordInfo *pOOBDevicePasswordInfo,
         uint8_t *pVersion,
         uint8_t *pDevicePasswordBuffer,
         uint32_t nDevicePasswordLength)
{
   tWIFIPairingOperation* pPairingOperation;
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("PWIFIGetOOBDevicePasswordInfo");

   if((pOOBDevicePasswordInfo == null) || (pVersion == null) || (pDevicePasswordBuffer == null))
   {
      PDebugWarning("PWIFIGetOOBDevicePasswordInfo: bad parameter");
      return W_ERROR_BAD_PARAMETER;
   }

   nError = PHandleGetObject(pContext, hOperation, P_HANDLE_TYPE_WIFI_PAIR_OPERATION, (void**)&pPairingOperation);
   if ( ( nError != W_SUCCESS ) || ( pPairingOperation == null ) )
   {
      PDebugWarning("PWIFIGetOOBDevicePasswordInfo: bad handle");
      return W_ERROR_BAD_HANDLE;
   }

   nError = static_PWIFIGetStringInfo(pContext, hOperation, pDevicePasswordBuffer,
      nDevicePasswordLength, pPairingOperation, pPairingOperation->aDevicePasswordBuffer);
   if (nError != W_SUCCESS )
   {
      PDebugWarning("PWIFIGetOOBDevicePasswordInfo: static_PWIFIGetStringInfo error");
      return nError;
   }

   (*pVersion) = pPairingOperation->nVersion;

   CMemoryCopy(pOOBDevicePasswordInfo, &pPairingOperation->sOOBDevicePasswordInfo, sizeof(tWWIFIOOBDevicePasswordInfo));

   return W_SUCCESS;
}

/* See Client API Specifications */
W_ERROR PWIFIGetCredentialInfo(
         tContext* pContext,
         W_HANDLE hOperation,
         uint8_t *pVersion,
         uint8_t *pCredentialBuffer,
         uint32_t nCredentialLength)
{
   tWIFIPairingOperation* pPairingOperation;
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("PWIFIGetCredentialInfo");

   if((pVersion == null) || (pCredentialBuffer == null))
   {
      PDebugWarning("PWIFIGetCredentialInfo: bad parameter");
      return W_ERROR_BAD_PARAMETER;
   }

   nError = PHandleGetObject(pContext, hOperation, P_HANDLE_TYPE_WIFI_PAIR_OPERATION, (void**)&pPairingOperation);
   if ( ( nError != W_SUCCESS ) || ( pPairingOperation == null ) )
   {
      PDebugWarning("PWIFIGetCredentialInfo: bad handle");
      return W_ERROR_BAD_HANDLE;
   }

   nError = static_PWIFIGetStringInfo(pContext, hOperation, pCredentialBuffer,
      nCredentialLength, pPairingOperation, pPairingOperation->aCredentialBuffer);
   if (nError != W_SUCCESS )
   {
      PDebugWarning("PWIFIGetCredentialInfo: static_PWIFIGetStringInfo error Credential");
      return nError;
   }

   (*pVersion) = pPairingOperation->nVersion;

   return W_SUCCESS;
}

/* Finish the operation */
static void static_PWIFIGenericPairingFinishOperation(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_ERROR nResultError)
{
   static_PWIFIPairingGenericCloseHandle(pContext,pPairingOperation);
   static_PWIFIPairingGenericInitHandle(pContext,pPairingOperation);

   CDebugAssert(pPairingOperation->nState != P_OPERATION_STATE_COMPLETED);

   PBasicSetOperationCompleted(pContext, pPairingOperation->hPairingOperation);
   pPairingOperation->nState = P_OPERATION_STATE_COMPLETED;

   pPairingOperation->nResultError = nResultError;

   PDFCPostContext2(
         &pPairingOperation->sCallbackContext,
         P_DFC_TYPE_WIFI_PAIR,
         nResultError );

   PHandleClose(pContext, pPairingOperation->hPairingOperation);
}


static void static_PWIFIAccessPointPairingFinishOperation(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_ERROR nResultError);

static void static_PWIFIAccessPointPairingShutdownCompleted(tContext * pContext, void * pCallbackParameter, W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   pPairingOperation->bIsSocketConnected = false;
   static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, pPairingOperation->nResultError);
}

static void static_PWIFIAccessPointPairingFinishOperation(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_ERROR nResultError)
{
   W_ERROR nError;
   if (pPairingOperation->bIsSocketConnected)
   {
      PDebugTrace("static_PWIFIAccessPointPairingFinishOperation : PP2PShutdown");
      PP2PShutdownDriver(pContext, pPairingOperation->hSocket, static_PWIFIAccessPointPairingShutdownCompleted, pPairingOperation);

      /* @todo What to do if the IOCTL failed */
      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_WIFI_PAIR, static_PWIFIAccessPointPairingShutdownCompleted, pPairingOperation, nError);
      }

      return;
   }

   if (pPairingOperation->hLink != W_NULL_HANDLE)
   {
      PDebugTrace("static_PBTPairingFinishOperation : Close link");
      PHandleClose(pContext, pPairingOperation->hLink);
      pPairingOperation->hLink = W_NULL_HANDLE;
   }

   static_PWIFIGenericPairingFinishOperation(pContext,pPairingOperation,nResultError);
}

/* See Client API Specifications */
W_ERROR PWIFIPairingGetMessageM(
         tContext* pContext,
         W_HANDLE hOperation,
         uint8_t *pMessageMBuffer,
         uint32_t nMessageMLength)
{
   W_ERROR nError = W_SUCCESS;
   tWIFIPairingOperation* pPairingOperation;

   PDebugTrace("PWIFIPairingGetMessageM");

   nError = PHandleGetObject(pContext, hOperation, P_HANDLE_TYPE_WIFI_PAIR_OPERATION, (void**)&pPairingOperation);
   if ( ( nError != W_SUCCESS ) || ( pPairingOperation == null ) )
   {
      PDebugWarning("PWIFIPairingGetMessageM: bad handle");
      if ( pPairingOperation != null )
      {
         if (pPairingOperation->bIsEnrollee == false)
         {
            static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, W_ERROR_BAD_HANDLE);
         }
      }
      return W_ERROR_BAD_HANDLE;
   }

   if (pMessageMBuffer == null)
   {
      PDebugWarning("PWIFIPairingGetMessageM: bad parameter");
      pPairingOperation->nResultError = W_ERROR_BAD_NDEF_FORMAT;
      return(W_ERROR_BAD_PARAMETER);
   }

   if (pPairingOperation->nMessageMLength <= nMessageMLength)
   {
      CMemoryCopy(pMessageMBuffer, pPairingOperation->aMessageMBuffer, pPairingOperation->nMessageMLength);
   }
   else
   {
      PDebugWarning("PWIFIPairingGetMessageM: bad length");
   }

   return(nError);
}

/* See Client API Specifications */
uint32_t PWIFIPairingGetMessageMLength(
         tContext* pContext,
         W_HANDLE hOperation)
{
   tWIFIPairingOperation* pPairingOperation;
   W_ERROR nError = W_SUCCESS;

   PDebugTrace("PWIFIPairingGetMessageMLength");

   nError = PHandleGetObject(pContext, hOperation, P_HANDLE_TYPE_WIFI_PAIR_OPERATION, (void**)&pPairingOperation);
   if ( ( nError != W_SUCCESS ) || ( pPairingOperation == null ) )
   {
      PDebugWarning("PWIFIPairingGetMessageMLength: bad handle");
      if (pPairingOperation->bIsEnrollee == false)
      {
         static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, W_ERROR_BAD_HANDLE);
      }
      return (0);
   }

   return(pPairingOperation->nMessageMLength);
}

/* See Client API Specifications */
static void static_PWIFIAccessPointPairingStart(
         tContext* pContext,
         tPBasicGenericCallbackFunction *pCallback,
         void *pCallbackParameter,
         tPBasicGenericEventHandler *pM1EventReadCallback,
         void *pM1EventReadCallbackParameter,
         W_HANDLE *phOperation);

extern tContext* g_pContext;

/* See Client API Specifications */
void WWIFIAccessPointPairingStart(
         tWBasicGenericCallbackFunction *pCallback,
         void *pCallbackParameter,
         tWBasicGenericEventHandler *pM1EventReadCallback,
         void *pM1EventReadCallbackParameter,
         W_HANDLE *phOperation)
{
   tContext* pContext;
   tDFCExternal dfc1;
   tDFCExternal dfc2;
   dfc1.pFunction = (tDFCCallback*)pCallback;
   dfc1.pParameter = pCallbackParameter;
   dfc2.pFunction = (tDFCCallback*)pM1EventReadCallback;
   dfc2.pParameter = pM1EventReadCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   static_PWIFIAccessPointPairingStart( pContext, null, &dfc1, null, &dfc2, phOperation );
   PContextReleaseLock(pContext);
}

void static_PWIFIAccessPointPairingReadCompleted(
   tContext* pContext,
   void *pCallbackParameter,
   uint32_t nDataLength,
   W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;
   uint8_t *  pPayloadBuffer = null;
   uint32_t   nPayloadBufferLength;
   uint32_t   nTypeStringLength;
   uint8_t* pReceptionBuffer = pPairingOperation->aMessageMBuffer;

   PDebugTrace("static_PWIFIAccessPointPairingReadCompleted");

   PHandleClose(pContext, pPairingOperation->hReadOperation);
   pPairingOperation->hReadOperation = W_NULL_HANDLE;

   if (pPairingOperation->nState == P_OPERATION_STATE_COMPLETED)
   {
      PDebugTrace("static_PWIFIAccessPointPairingReadCompleted: BAD STATE");
      nError = W_ERROR_BAD_STATE;
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
      return;
   }

   if (nError != W_SUCCESS)
   {
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
      return;
   }

   pReceptionBuffer++;
   nTypeStringLength =  (*pReceptionBuffer);
   pReceptionBuffer++;
   nPayloadBufferLength = (*pReceptionBuffer);
   pReceptionBuffer++;

   pPairingOperation->nMessageMLength = nPayloadBufferLength;
   pPayloadBuffer = pReceptionBuffer+nTypeStringLength;

   if (pPairingOperation->nMessageMLength > WIFI_MAXIMUM_MESSAGE_LENGTH)
   {
      /* Send the result */
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, W_ERROR_BAD_NDEF_FORMAT);
      return;
   }

   if (pPairingOperation->nMessageMLength > 0)
   {
      CMemoryCopy(pPairingOperation->aMessageMBuffer,pPayloadBuffer,pPairingOperation->nMessageMLength);
   }

   PDFCPostContext2(
            &pPairingOperation->sEventReadCallbackContext,
            P_DFC_TYPE_WIFI_PAIR,
            nError );
 }


void static_PWIFIAccessPointPairingP2PConnectCompleted(tContext* pContext, void *pCallbackParameter, W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIAccessPointPairingConnectCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError == W_SUCCESS)
      {
         PP2PReadDriver(pContext, pPairingOperation->hSocket, static_PWIFIAccessPointPairingReadCompleted, pPairingOperation,
            pPairingOperation->aMessageMBuffer, WIFI_MAXIMUM_MESSAGE_LENGTH, & pPairingOperation->hReadOperation);

         nError = PContextGetLastIoctlError(pContext);
         if (nError != W_SUCCESS)
         {
            PDFCPost3(pContext, P_DFC_TYPE_BT_PAIR, static_PWIFIAccessPointPairingReadCompleted, pPairingOperation, 0, nError);
         }
      }
      else
      {
         /* Send the error */
         PDebugWarning("static_PWIFIAccessPointPairingConnectCompleted: card detection error");

         static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
      }
   }
}

static void static_PWIFIAccessPointPairingP2PLinkEstablished(
            tContext * pContext,
            void* pCallbackParameter,
            W_HANDLE hHandle,
            W_ERROR nResult )
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIAccessPointPairingP2PLinkEstablished : %s", PUtilTraceError(nResult));

   PHandleClose(pContext, pPairingOperation->hLinkOperation);
   pPairingOperation->hLinkOperation = W_NULL_HANDLE;

   if (nResult == W_SUCCESS)
   {
      W_ERROR nError;

      pPairingOperation->hLink = hHandle;

      PP2PConnectDriver(pContext, pPairingOperation->hSocket, pPairingOperation->hLink, static_PWIFIAccessPointPairingP2PConnectCompleted, pPairingOperation);
      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_WIFI_PAIR, static_PWIFIAccessPointPairingP2PConnectCompleted, pPairingOperation, nError);
      }
   }
   else
   {
      /* Send the error */
      PDebugWarning("static_PBTPairingEstablishLinkCompleted: P2P link establishment error");

      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nResult);
   }
}


static void static_PWIFIAccessPointPairingP2PLinkReleased(
         tContext * pContext,
         void * pCallbackParameter,
         W_ERROR nError)
{
   PDebugTrace("static_PWIFIAccessPointPairingP2PLinkReleased: P2P link released");
}

/* See Client API Specifications */
static void static_PWIFIAccessPointPairingStart(
         tContext* pContext,
         tPBasicGenericCallbackFunction *pCallback,
         void *pCallbackParameter,
         tPBasicGenericEventHandler *pM1EventReadCallback,
         void *pM1EventReadCallbackParameter,
         W_HANDLE *phOperation)
{
   W_ERROR nError = W_SUCCESS;
   tWIFIPairingOperation* pPairingOperation;
   uint8_t nReadAndCardProtocolSupported = 0;
   bool   bValue;

   if (phOperation != null)
   {
      * phOperation = W_NULL_HANDLE;
   }

   /* Create the WIFI structure */
   pPairingOperation = (tWIFIPairingOperation*)CMemoryAlloc( sizeof(tWIFIPairingOperation) );
   if ( pPairingOperation == null )
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("static_PWIFIAccessPointPairingStart: Cannot allocate the operation structure");


      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_WIFI_PAIR, W_ERROR_OUT_OF_RESOURCE );

      return;
   }
   CMemoryFill(pPairingOperation, 0, sizeof(tWIFIPairingOperation));

   /* Store the callback information */
   PDFCFillCallbackContext(
      pContext, (tDFCCallback*)pCallback, pCallbackParameter, &pPairingOperation->sCallbackContext );

   /* Store the callback information */
   PDFCFillCallbackContext(
      pContext, (tDFCCallback*)pM1EventReadCallback, pM1EventReadCallbackParameter, &pPairingOperation->sEventReadCallbackContext);

   static_PWIFIPairingGenericInitHandle(pContext,pPairingOperation);

   if(((nError = PNFCControllerGetBooleanProperty(pContext, W_NFCC_PROP_P2P,  &bValue)) == W_SUCCESS )
   && (bValue == true))
   {
      nReadAndCardProtocolSupported |= (W_NFCC_PROTOCOL_CARD_P2P_TARGET|W_NFCC_PROTOCOL_READER_P2P_INITIATOR);
   }

   if ( (nError = static_PWIFIPairingGenericHandleRegister(
                     pContext,
                     pPairingOperation,
                     null))!= W_SUCCESS )
   {
      PDebugError("static_PWIFIAccessPointPairingStart: error on static_PWIFIPairingGenericHandleRegister()");
      goto return_error;
   }

   if (phOperation != null)
   {
      if ((nError = PHandleDuplicate(pContext, pPairingOperation->hPairingOperation, phOperation)) != W_SUCCESS)
      {
         PDebugError("static_PWIFIAccessPointPairingStart: error on PHandleDuplicate()");
         goto return_error;
      }
   }

   pPairingOperation->bIsEnrollee = false;

   if (((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_CARD_P2P_TARGET) == 0)&&
      ((nReadAndCardProtocolSupported&W_NFCC_PROTOCOL_READER_P2P_INITIATOR) == 0))
   {
      nError = W_ERROR_RF_PROTOCOL_NOT_SUPPORTED;
      goto return_error;
   }

   nError = PP2PCreateSocketDriver(pContext, W_P2P_TYPE_SERVER, g_aPWIFIPairingServiceName, (1 + PUtilStringLength(g_aPWIFIPairingServiceName)) * sizeof(tchar), 0, &pPairingOperation->hSocket);
   if (nError != W_SUCCESS)
   {
      goto return_error;
   }

   PP2PEstablishLinkWrapper(pContext, static_PWIFIAccessPointPairingP2PLinkEstablished, pPairingOperation, static_PWIFIAccessPointPairingP2PLinkReleased, null, &pPairingOperation->hLinkOperation);

   return;

return_error:

   PDebugWarning("static_PWIFIAccessPointPairingStart: input parameter, memory or creation error");

   /* Send the error */
   static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);

   if(phOperation != null)
   {
      PHandleClose(pContext, *phOperation);
      *phOperation = W_NULL_HANDLE;
   }
}


static void static_PWIFIPairingWriteM2MessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIPairingWriteM2MessageCompleted");

   PHandleClose(pContext, pPairingOperation->hWriteOperation);
   pPairingOperation->hWriteOperation = W_NULL_HANDLE;

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PWIFIPairingWriteM2MessageCompleted: write M2 error");
      }
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
   }
}


/* See Client API Specifications */
W_ERROR PWIFIPairingWriteMessageM2(
         tContext* pContext,
         W_HANDLE hOperation,
         const uint8_t*    pMessageM2Buffer,
         uint32_t    nMessageM2Length)
{
   tWIFIPairingOperation* pPairingOperation;
   W_ERROR nError = W_SUCCESS;
   W_HANDLE hMessage, hRecord;

   PDebugTrace("PWIFIPairingWriteMessageM2");

   nError = PHandleGetObject(pContext, hOperation, P_HANDLE_TYPE_WIFI_PAIR_OPERATION, (void**)&pPairingOperation);
   if ( ( nError != W_SUCCESS ) || ( pPairingOperation == null ) )
   {
      PDebugWarning("PWIFIPairingWriteMessageM2: bad handle");
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, W_ERROR_BAD_HANDLE);
      return W_ERROR_BAD_HANDLE;
   }

   if(pPairingOperation->nResultError != W_SUCCESS)
   {
      PDebugWarning("PWIFIPairingWriteMessageM2: bad state");
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, pPairingOperation->nResultError);
      return W_ERROR_BAD_STATE;
   }

   if ((pMessageM2Buffer == null) || (nMessageM2Length == 0))
   {
      PDebugWarning("PWIFIPairingWriteMessageM2: bad parameter");
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, W_ERROR_BAD_STATE);
      return W_ERROR_BAD_PARAMETER;
   }

   nError = PNDEFCreateNewMessage(pContext, &hMessage);
   if (nError != W_SUCCESS)
   {
      PDebugWarning("PWIFIPairingWriteMessageM2: PNDEFCreateNewMessage error");
      static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
      return (nError);
   }
   else
   {
      nError = PNDEFCreateRecord(pContext, W_NDEF_TNF_MEDIA, g_aWIFITypeString, pMessageM2Buffer, nMessageM2Length, &hRecord);

      if (nError != W_SUCCESS)
      {
         PDebugWarning("PWIFIPairingWriteMessageM2: PNDEFCreateRecord error");
         static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
         PHandleClose(pContext, hMessage);
         return (nError);
      }

      nError = PNDEFAppendRecord(pContext, hMessage, hRecord);

      PHandleClose(pContext, hRecord);

      if (nError != W_SUCCESS)
      {
         PDebugWarning("PWIFIPairingWriteMessageM2: PNDEFAppendRecord error");
         static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
         PHandleClose(pContext, hMessage);
         return (nError);
      }

      pPairingOperation->nMessageMLength = PNDEFGetMessageLength(pContext, hMessage);

      nError = PNDEFGetMessageContent( pContext, hMessage, pPairingOperation->aMessageMBuffer,
         pPairingOperation->nMessageMLength, &pPairingOperation->nMessageMLength);
      if (nError != W_SUCCESS)
      {
         PDebugWarning("PWIFIPairingWriteMessageM2: PNDEFGetMessageContent error");
         static_PWIFIAccessPointPairingFinishOperation(pContext, pPairingOperation, nError);
         PHandleClose(pContext, hMessage);
         return (nError);
      }

      PP2PWriteDriver(pContext,pPairingOperation->hSocket, static_PWIFIPairingWriteM2MessageCompleted, pPairingOperation,
               pPairingOperation->aMessageMBuffer, pPairingOperation->nMessageMLength, & pPairingOperation->hWriteOperation);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_WIFI_PAIR, static_PWIFIPairingWriteM2MessageCompleted, pPairingOperation, nError);
      }
   }

   PHandleClose(pContext, hMessage);

   return (W_SUCCESS);
}

/* Finish the operation */
static void static_PWIFIAccessPointConfigureTokenFinishOperation(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_ERROR nResultError)
{
   if(nResultError != W_SUCCESS)
   {
      PDebugError("static_PWIFIAccessPointConfigureTokenFinishOperation: return the error %s",
         PUtilTraceError(nResultError));
   }

   static_PWIFIGenericPairingFinishOperation(pContext,pPairingOperation,nResultError);
}

static void static_PWIFIWriteMessageOnAnyTagCompleted(tContext* pContext, void *pCallbackParameter, W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFIWriteMessageOnAnyTagCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
     static_PWIFIAccessPointConfigureTokenFinishOperation(pContext, pPairingOperation,  nError);
   }
}

static void static_PWIFIAddValueInCredential(
         uint16_t    nValue,
         uint8_t **   pBuffer)
{
   (**pBuffer) = (uint8_t)((nValue&0xFF00)>>8);
   (*pBuffer)++;
   (**pBuffer) = (uint8_t)(nValue&0xFF);
   (*pBuffer)++;
}

static void static_PWIFIAddAttributeLengthOfCredential(
         uint16_t    nAttributeId,
         uint16_t    nLength,
         uint8_t **   pBuffer)
{
   static_PWIFIAddValueInCredential(nAttributeId,pBuffer);
   static_PWIFIAddValueInCredential(nLength,pBuffer);
}


/* See Client API Specifications */
void PWIFIAccessPointConfigureToken(
         tContext* pContext,
         uint8_t nVersion,
         const uint8_t* pCredentialBuffer,
         uint32_t nCredentialLength,
         tPBasicGenericCallbackFunction* pCallback,
         void* pCallbackParameter,
         W_HANDLE *phOperation)
{

   W_ERROR nError = W_SUCCESS;
   tWIFIPairingOperation* pPairingOperation;
   W_HANDLE hMessage = W_NULL_HANDLE;
   W_HANDLE hRecord;
   uint8_t *  pPayloadBuffer = null;
   uint8_t *  pBuffer;
   uint32_t  nPayloadLength;
   W_HANDLE  hOperation;

   if (phOperation != null)
   {
      * phOperation = W_NULL_HANDLE;
   }

   if ((pCredentialBuffer == null)  || (nCredentialLength > WIFI_MAXIMUM_MESSAGE_LENGTH))
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("PWIFIAccessPointConfigureToken: Bad parameter");

      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_WIFI_PAIR, W_ERROR_BAD_PARAMETER );

      return;
   }
   /* Create the WIFI structure */
   pPairingOperation = (tWIFIPairingOperation*)CMemoryAlloc( sizeof(tWIFIPairingOperation) );
   if ( pPairingOperation == null )
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("PWIFIAccessPointConfigureToken: Cannot allocate the operation structure");

      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_WIFI_PAIR, W_ERROR_OUT_OF_RESOURCE );

      return;
   }

   CMemoryFill(pPairingOperation, 0, sizeof(tWIFIPairingOperation));

   /* Store the callback information */
   PDFCFillCallbackContext(
      pContext, (tDFCCallback*)pCallback, pCallbackParameter, &pPairingOperation->sCallbackContext );

   static_PWIFIPairingGenericInitHandle(pContext,pPairingOperation);

   nError = PNDEFCreateNewMessage(pContext, &hMessage);
   if (nError != W_SUCCESS)
   {
      goto return_error;
   }

   nPayloadLength = P_WIFI_PAIR_TYPE_LENGTH + sizeof(nVersion) + nCredentialLength + P_WIFI_PAIR_TYPE_LENGTH;

   pPayloadBuffer = (uint8_t *)CMemoryAlloc(nPayloadLength);

   if (pPayloadBuffer == null)
   {
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   pBuffer = pPayloadBuffer;

   static_PWIFIAddAttributeLengthOfCredential((uint16_t)P_WIFI_PAIR_VERSION_ATTRIBUTE_ID, (uint16_t)sizeof(nVersion),(uint8_t **)&pBuffer);
   (*pBuffer) = nVersion;
   pBuffer = pBuffer + sizeof(nVersion);
   static_PWIFIAddAttributeLengthOfCredential((uint16_t)P_WIFI_PAIR_CREDENTIAL_ATTRIBUTE_ID,(uint16_t)nCredentialLength,(uint8_t **)&pBuffer);
   CMemoryCopy(pBuffer,pCredentialBuffer,nCredentialLength);

   nError = PNDEFCreateRecord(pContext, W_NDEF_TNF_MEDIA, g_aWIFITypeString, pPayloadBuffer, nPayloadLength, &hRecord);
   if (nError != W_SUCCESS)
   {
      goto return_error;
   }

   nError = PNDEFAppendRecord(pContext, hMessage, hRecord);
   PHandleClose(pContext, hRecord);

   if (nError != W_SUCCESS)
   {
      goto return_error;
   }

   CMemoryFree(pPayloadBuffer);
   pPayloadBuffer = null;

   if ( (nError = static_PWIFIPairingGenericHandleRegister(
                     pContext,
                     pPairingOperation,
                     null))!= W_SUCCESS )
   {
      PDebugError("PWIFIAccessPointConfigureToken: error on static_PWIFIPairingGenericHandleRegister()");
      goto return_error;
   }

   if (phOperation != null)
   {
      if ((nError = PHandleDuplicate(pContext, pPairingOperation->hPairingOperation, phOperation)) != W_SUCCESS)
      {
         PDebugError("PWIFIAccessPointConfigureToken: error on PHandleDuplicate()");
         goto return_error;
      }
   }

   PNDEFWriteMessageOnAnyTag(pContext,static_PWIFIWriteMessageOnAnyTagCompleted, pPairingOperation, W_PRIORITY_MAXIMUM,
      hMessage, W_NDEF_ACTION_BIT_ERASE | W_NDEF_ACTION_BIT_CHECK_WRITE, &hOperation);

   pPairingOperation->hWriteOperation = hOperation;
   PHandleClose(pContext, hMessage);

   return;

return_error:

   PDebugWarning("PWIFIAccessPointConfigureToken: input parameter, memory or creation error");

      /* Send the error */

   static_PWIFIAccessPointConfigureTokenFinishOperation(pContext, pPairingOperation, nError);

   if (hMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, hMessage);
      hMessage = W_NULL_HANDLE;
   }

   if (pPayloadBuffer  != null)
   {
      CMemoryFree(pPayloadBuffer);
   }

   if (phOperation != null)
   {
      PHandleClose(pContext, * phOperation);
   }
}


/* Finish the operation */
static void static_PWIFITransmitPairingFinishOperation(
            tContext* pContext,
            tWIFIPairingOperation* pPairingOperation,
            W_ERROR nResultError)
{
   if(nResultError != W_SUCCESS)
   {
      PDebugError("static_PWIFITransmitPairingFinishOperation: return the error %s",
         PUtilTraceError(nResultError));
   }

   static_PWIFIGenericPairingFinishOperation(pContext,pPairingOperation,nResultError);
}

/* See Client API Specifications */
static void static_PWIFIInfoPairingTransmit(
         tContext* pContext,
         tPBasicGenericCallbackFunction *pCallback,
         void *pCallbackParameter,
         tPBasicGenericEventHandler *pEventWriteCallback,
         void *pEventWriteCallbackParameter,
         W_HANDLE *phOperation);

/* See Client API Specifications */
void WWIFIInfoPairingTransmit(
         tWBasicGenericCallbackFunction *pCallback,
         void *pCallbackParameter,
         tWBasicGenericEventHandler *pEventWriteCallback,
         void *pEventWriteCallbackParameter,
         W_HANDLE *phOperation)
{
   tContext* pContext;
   tDFCExternal dfc1;
   tDFCExternal dfc2;
   dfc1.pFunction = (tDFCCallback*)pCallback;
   dfc1.pParameter = pCallbackParameter;
   dfc2.pFunction = (tDFCCallback*)pEventWriteCallback;
   dfc2.pParameter = pEventWriteCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   static_PWIFIInfoPairingTransmit( pContext, null, &dfc1, null, &dfc2, phOperation );
   PContextReleaseLock(pContext);
}


static void static_PWIFITransmitPairingStartCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFITransmitPairingStartCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PWIFITransmitPairingStartCompleted: tag activation error");

         static_PWIFITransmitPairingFinishOperation(pContext, pPairingOperation, nError);
      }
   }
}

static void static_PWIFITransmitPairingEventDetected(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nEventCode)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFITransmitPairingEventDetected");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      switch(nEventCode)
      {
         case W_VIRTUAL_TAG_EVENT_READER_WRITE :
            PDebugTrace("static_PWIFITransmitPairingEventDetected: W_VIRTUAL_TAG_EVENT_READER_WRITE");

            pPairingOperation->bIsTagAlreadyWritten = true;

            PDFCPostContext2(
               &pPairingOperation->sEventWriteCallbackContext,
               P_DFC_TYPE_WIFI_PAIR,
               W_SUCCESS);
            break;

         case W_VIRTUAL_TAG_EVENT_READER_READ_ONLY :
            PDebugTrace("static_PWIFITransmitPairingEventDetected: W_VIRTUAL_TAG_EVENT_READER_READ_ONLY");
            if (pPairingOperation->bIsTagAlreadyWritten == true)
            {
               static_PWIFITransmitPairingFinishOperation(pContext, pPairingOperation, W_SUCCESS);
            }
            break;

         case W_VIRTUAL_TAG_EVENT_SELECTION :
         case W_VIRTUAL_TAG_EVENT_READER_LEFT :
         default :
            break;
      }
   }
}

static void static_PWIFITransmitPairingWriteMessageCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError)
{
   tWIFIPairingOperation* pPairingOperation = (tWIFIPairingOperation*)pCallbackParameter;

   PDebugTrace("static_PWIFITransmitPairingWriteMessageCompleted");

   if (pPairingOperation->nState != P_OPERATION_STATE_COMPLETED)
   {
      if (nError != W_SUCCESS)
      {
         PDebugWarning("static_PWIFITransmitPairingWriteMessageCompleted: tag write error");
         static_PWIFITransmitPairingFinishOperation(pContext, pPairingOperation, nError);
      }
      else
      {
         PVirtualTagStart(pContext,pPairingOperation->hVirtualTag, static_PWIFITransmitPairingStartCompleted,
            pPairingOperation,static_PWIFITransmitPairingEventDetected, pPairingOperation, false);
      }
   }
}

/* See Client API Specifications */
static void static_PWIFIInfoPairingTransmit(
         tContext* pContext,
         tPBasicGenericCallbackFunction *pCallback,
         void *pCallbackParameter,
         tPBasicGenericEventHandler *pEventWriteCallback,
         void *pEventWriteCallbackParameter,
         W_HANDLE *phOperation)
{

   W_ERROR nError = W_SUCCESS;
   uint8_t nReadAndCardProtocolSupported = 0;
   tWIFIPairingOperation* pPairingOperation;
   W_HANDLE hMessage = W_NULL_HANDLE;
   W_HANDLE hRecord;
   uint8_t  nTagType;
   W_HANDLE hVirtualTag = W_NULL_HANDLE;

   if(phOperation == null)
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("static_PWIFIInfoPairingTransmit: bad hoperation handle");

      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_WIFI_PAIR, W_ERROR_BAD_HANDLE );

      return;
   }
   else
   {
      * phOperation = W_NULL_HANDLE;
   }

   /* Create the WIFI structure */
   pPairingOperation = (tWIFIPairingOperation*)CMemoryAlloc( sizeof(tWIFIPairingOperation) );
   if ( pPairingOperation == null )
   {
      tDFCCallbackContext sCallbackContext;

      PDebugError("static_PWIFIInfoPairingTransmit: Cannot allocate the operation structure");

      PDFCFillCallbackContext(
         pContext, (tDFCCallback*)pCallback, pCallbackParameter, &sCallbackContext );

      PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_WIFI_PAIR, W_ERROR_OUT_OF_RESOURCE );

      return;
   }
   CMemoryFill(pPairingOperation, 0, sizeof(tWIFIPairingOperation));


   /* Store the callback information */
   PDFCFillCallbackContext(
      pContext, (tDFCCallback*)pCallback, pCallbackParameter, &pPairingOperation->sCallbackContext );

   /* Store the callback information */
   PDFCFillCallbackContext(
      pContext, (tDFCCallback*)pEventWriteCallback, pEventWriteCallbackParameter, &pPairingOperation->sEventWriteCallbackContext);

   static_PWIFIPairingGenericInitHandle(pContext,pPairingOperation);

   if (PEmulIsPropertySupported(pContext, W_PROP_ISO_14443_4_A) != false)
   {
      nReadAndCardProtocolSupported |= 0x2;
   }

   if (PEmulIsPropertySupported(pContext, W_PROP_ISO_14443_4_B) != false)
   {
      nReadAndCardProtocolSupported |= 0x1;
   }

   if ( (nError = static_PWIFIPairingGenericHandleRegister(
                     pContext,
                     pPairingOperation,
                     null))!= W_SUCCESS )
   {
      PDebugError("static_PWIFIInfoPairingTransmit: error on static_PWIFIPairingGenericHandleRegister()");
      goto return_error;
   }

   if (phOperation != null)
   {
      nError = PHandleDuplicate(pContext, pPairingOperation->hPairingOperation, phOperation);

      if (nError != W_SUCCESS)
      {
         PDebugError("static_PWIFIInfoPairingTransmit: error on PHandleDuplicate()");
         goto return_error;
      }
   }

   pPairingOperation->bIsTagAlreadyWritten = false;

   nError = PNDEFCreateNewMessage(pContext, &hMessage);
   if (nError != W_SUCCESS)
   {
      goto return_error;
   }
   else
   {
      nError = PNDEFCreateRecord(pContext, W_NDEF_TNF_MEDIA, g_aWIFITypeString, null, 0, &hRecord);
      if (nError != W_SUCCESS)
      {
         goto return_error;
      }

      nError = PNDEFAppendRecord(pContext, hMessage, hRecord);
      PHandleClose(pContext, hRecord);

      if (nError != W_SUCCESS)
      {
         goto return_error;
      }

      if ((nReadAndCardProtocolSupported&0x3) == 0)
      {
         nError = W_ERROR_RF_PROTOCOL_NOT_SUPPORTED;
         goto return_error;
      }
      else if ((nReadAndCardProtocolSupported&0x1) != 0)
      {
         nTagType = W_PROP_NFC_TAG_TYPE_4_B;
      }
      else
      {
         nTagType = W_PROP_NFC_TAG_TYPE_4_A;
      }

      nError = PVirtualTagCreate(pContext,nTagType, g_aWIFIPUPI, sizeof(g_aWIFIPUPI), WIFI_MAXIMUM_MESSAGE_LENGTH, &hVirtualTag);

      if(nError != W_SUCCESS)
      {
         goto return_error;
      }
      else
      {
         pPairingOperation->hVirtualTag = hVirtualTag;
         PNDEFWriteMessage(pContext, hVirtualTag, static_PWIFITransmitPairingWriteMessageCompleted,
                        pPairingOperation, hMessage, W_NDEF_ACTION_BIT_ERASE |  W_NDEF_ACTION_BIT_CHECK_WRITE, null);
      }
   }
   PHandleClose(pContext, hMessage);
   return;

return_error:

   PDebugWarning("static_PWIFIInfoPairingTransmit: input parameter, memory or creation error");

   /* Send the error */
   static_PWIFITransmitPairingFinishOperation(pContext, pPairingOperation, nError);

   PHandleClose(pContext, hMessage);

   if(phOperation != null)
   {
      PHandleClose(pContext, *phOperation);
      *phOperation = W_NULL_HANDLE;
   }
}

/** See Cleint API Specification */
W_ERROR WWIFIAccessPointPairingStartSync(
               tWBasicGenericEventHandler *pM1EventReadCallback,
               void *pM1EventReadCallbackParameter)
{
    tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WWIFIAccessPointPairingStart(
         PBasicGenericSyncCompletion, &param,
         pM1EventReadCallback, pM1EventReadCallbackParameter,
         null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/** See Client API Specification */
W_ERROR WWIFIAccessPointConfigureTokenSync(
               uint8_t nVersion,
               const uint8_t* pCredentialBuffer,
               uint32_t nCredentialLength)
{
    tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WWIFIAccessPointConfigureToken(
         nVersion,
         pCredentialBuffer, nCredentialLength,
         PBasicGenericSyncCompletion, &param,
         null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

W_ERROR WWIFIInfoPairingTransmitSync(
               tWBasicGenericEventHandler *pEventWriteCallback,
               void *pEventWriteCallbackParameter)
{
    tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WWIFIInfoPairingTransmit(
         PBasicGenericSyncCompletion, &param,
         pEventWriteCallback, pEventWriteCallbackParameter,
         null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}


#endif  /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
