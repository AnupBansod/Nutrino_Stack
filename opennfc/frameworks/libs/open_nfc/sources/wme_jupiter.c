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
   Contains the jupiter specific implementation.
*******************************************************************************/

#define P_MODULE  P_MODULE_DEC( JUPITER )

#include "wme_context.h"

#ifdef P_INCLUDE_JUPITER_SE

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* Buffer maximum size */
#define P_BUFFER_MAX_SIZE    (256 + 5)

/* Jupiter AID */
static const uint8_t g_aJupiterAID[] =
{
   0xF0, 0x69, 0x6E, 0x63, 0x6C, 0x53, 0x45, 0x53, 0x65, 0x74, 0x74, 0x69,
   0x6E, 0x67, 0x73
};

/* Disable all protocols Configuration Constant */
#define W_NFCC_DISABLE_ALL_PROTOCOLS   0x00000000

/* Declare an Jupiter SE structure paramters for asynchronous mode */
typedef struct __tPJupiterSEParamter
{
   /* Operation requested */
   bool bIsPolicyRequest;
   /* store the Callback context */
   tDFCCallbackContext sCallbackContext;
   /* SE persistent policy */
   uint32_t nPersistentPolicy;
   /* SE volatile policy */
   uint32_t nVolatilePolicy;
   /* hConnection handle */
   W_HANDLE hSEConnection;
   /* AIDs of the applets that were selected */
   uint8_t* pAIDs;
   /* AIDs length */
   uint32_t nAIDsLength;
   /* select Jupiter APDU command buffer*/
   uint8_t aAllocatedSelectJupiterAPDUBuffer[20];
   /* policy APDU command buffer*/
   uint8_t aAllocatedPolicyAPDUBuffer[9];
   /* AID APDU command buffer*/
   uint8_t aAllocatedAidAPDUBuffer[4];
   /* Response buffer */
   uint8_t aResponseBuffer[P_BUFFER_MAX_SIZE];
   /* Flag for the applet limitation: no support for 0xFFFF policy value */
   bool bNoSupportFor0xFFFF;
   /* Pending error, to be posted once the connection has been closed */
   W_ERROR nPendingError;
} tPJupiterSEParamter;

static void static_P7816ExchangeSelectAPDUForPolicyCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nDataLength,
               W_ERROR nResult);

/**
 * Callback of PHandleCloseSafe
 *
 * @param[in]  pContext context.
 *
 * @param[in]  pCallbackParameter  The blind parameter given to the function initiating the operation.
 *
 * @param[in]  nResult  The result code of the operation.
 **/
 static void static_CloseHandleSafeCompleted( tContext* pContext, void * pCallbackParameter, W_ERROR nResult )
{
   tPJupiterSEParamter* pJupiterSEParameter = (tPJupiterSEParamter*)pCallbackParameter;

   /* 5 send the result */
   PDebugTrace("5: send the result");

   if (pJupiterSEParameter->nPendingError != W_SUCCESS)
   {
      nResult = pJupiterSEParameter->nPendingError;
   }

   if (pJupiterSEParameter->bIsPolicyRequest)
   {
      PDFCPostContext2( &pJupiterSEParameter->sCallbackContext,
                        P_DFC_TYPE_SECURE_ELEMENT,
                        nResult );
   }
   else
   {
      PDFCPostContext3( &pJupiterSEParameter->sCallbackContext,
                        P_DFC_TYPE_SECURE_ELEMENT,
                        pJupiterSEParameter->nAIDsLength,
                        nResult );
   }
   if(nResult != W_SUCCESS)
   {
      PDebugError("static_CloseHandleSafeCompleted: Sending error %s", PUtilTraceError(nResult));
   }
   CMemoryFree(pJupiterSEParameter);
}

/**
 * Parses the AID buffer sent by the Jupiter applet.
 *
 * The AID of the Jupiter applet is removed from the list. The format of the list is:
 *    |Length 1 | AID 1| ... |Length N | AID N |
 *
 * @param[in]  pSourceBuffer  A pointer on the AID list returned by the applet.
 *
 * @param[in]  nSourceBufferLength  The length in bytes of the AID buffer list.
 *
 * @param[in]  pDestBuffer  A pointer on the buffer receiving the filtered list.
 *
 * @param[in]  nDestBufferLength  the length in bytes of the destination buffer.
 *
 * @return  the actual size in bytes of the filtered list.
 **/
static uint32_t static_PJupiterSEFilterAID(
               const uint8_t* pSourceBuffer,
               uint32_t nSourceBufferLength,
               uint8_t* pDestBuffer,
               uint32_t nDestBufferLength)
{
   uint32_t nNewBufferLength = 0;
   uint8_t nJupiterAIDLength = sizeof(g_aJupiterAID);
   uint8_t nAIDLength = 0;
   uint32_t nSourcePos;

   for(nSourcePos = 0; nSourcePos < nSourceBufferLength; nSourcePos += nAIDLength)
   {
      nAIDLength = pSourceBuffer[nSourcePos++];
      if (((nAIDLength > (nSourceBufferLength - nSourcePos)) || (nAIDLength == 0)))
      {
         /* SE malformed response */
         return 0;
      }

      if ((nAIDLength != nJupiterAIDLength)
      || (CMemoryCompare(&pSourceBuffer[nSourcePos], g_aJupiterAID, nJupiterAIDLength) != 0))
      {
         if((nNewBufferLength + 1 + nAIDLength) > nDestBufferLength)
         {
            /* Destination buffer too short */
            break;
         }

         /* The AID length is different from the jupiter OR the content are different */
         pDestBuffer[nNewBufferLength++] = nAIDLength;
         CMemoryCopy(&pDestBuffer[nNewBufferLength], &pSourceBuffer[nSourcePos] , nAIDLength);
         nNewBufferLength += nAIDLength;
      }
   }

   return nNewBufferLength;
}

/**
 * Callback of P7816ExchangeAPDU
 *
 * @param[in]  pContext context.
 *
 * @param[in]  pCallbackParameter  The blind parameter given to the function initiating the operation.
 *
 * @param[in]  nDataLength  The length in bytes of the data received.
 *
 * @param[in]  nResult  The result code of the operation:
 **/
static void static_P7816ExchangeAPDUCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nDataLength,
               W_ERROR nResult)
{
   tPJupiterSEParamter* pJupiterSEParameter = (tPJupiterSEParamter*)pCallbackParameter;
   uint8_t* pCardToReaderBuffer;

   PDebugTrace("static_P7816ExchangeAPDUCompleted()");

   /* Check the error code return */
   if ( nResult != W_SUCCESS )
   {
      goto send_error;
   }

   /* Check the parameters */
   if ( nDataLength < 2 )
   {
      nResult = W_ERROR_RF_COMMUNICATION;
      goto send_error;
   }

   pCardToReaderBuffer = pJupiterSEParameter->aResponseBuffer;

   /* Check the SW1 & SW2 values */
   if ( pCardToReaderBuffer[nDataLength - 2] != 0x90)
   {
      /* Filter the error status '6A80' on the disable all protocols cmd. */
      if((pJupiterSEParameter->bIsPolicyRequest != false)
      && (pJupiterSEParameter->bNoSupportFor0xFFFF == false)
      && ((pJupiterSEParameter->nVolatilePolicy == W_NFCC_DISABLE_ALL_PROTOCOLS)
         || (pJupiterSEParameter->nPersistentPolicy == W_NFCC_DISABLE_ALL_PROTOCOLS))
      && (pCardToReaderBuffer[nDataLength - 2] == 0x6A)
      && (pCardToReaderBuffer[nDataLength - 1] == 0x80) )
      {
         PDebugWarning("static_P7816ExchangeAPDUCompleted: 6A80 is not an error, 0xFFFF is not supported");
         /* Set the limitation of the applet */
         pJupiterSEParameter->bNoSupportFor0xFFFF = true;
         /* Resend the command without 0xFFFF */
         pCardToReaderBuffer[0] = 0x90;
         pCardToReaderBuffer[1] = 0x00;
         static_P7816ExchangeSelectAPDUForPolicyCompleted(
               pContext, pJupiterSEParameter, 2, W_SUCCESS);
         return;
      }
      else
      {
         PDebugError("static_P7816ExchangeAPDUCompleted: received %d bytes SW1=0x%02X SW2=0x%02X",
                  nDataLength,
                  pCardToReaderBuffer[nDataLength - 2],
                  pCardToReaderBuffer[nDataLength - 1]);

         if((pJupiterSEParameter->bIsPolicyRequest != false)
         && (pCardToReaderBuffer[nDataLength - 2] == 0x6A)
         && (pCardToReaderBuffer[nDataLength - 1] == 0x80) )
         {
            nResult = W_ERROR_FEATURE_NOT_SUPPORTED;
         }
         else
         {
            nResult = W_ERROR_RF_COMMUNICATION;
         }
         goto send_error;
      }
   }

   /* Copy the response */
   if (pJupiterSEParameter->bIsPolicyRequest == false)
   {
      /* First retrieve the AIDs Lenght */
      pJupiterSEParameter->nAIDsLength = static_PJupiterSEFilterAID(
         pCardToReaderBuffer, (nDataLength-2),
         pJupiterSEParameter->pAIDs, pJupiterSEParameter->nAIDsLength);
   }

   /* 4 close the SE channel */
   PDebugTrace("4: close the SE channel");
   PHandleCloseSafe( pContext,
                     pJupiterSEParameter->hSEConnection,
                     static_CloseHandleSafeCompleted,
                     pJupiterSEParameter);
   return;

send_error:

   PDebugError("static_P7816ExchangeAPDUCompleted: Sending error %s", PUtilTraceError(nResult));
   if (pJupiterSEParameter->bIsPolicyRequest)
   {
      PDFCPostContext2( &pJupiterSEParameter->sCallbackContext,
                      P_DFC_TYPE_SECURE_ELEMENT,
                      nResult);
   }
   else
   {
      PDFCPostContext3( &pJupiterSEParameter->sCallbackContext,
                   P_DFC_TYPE_SECURE_ELEMENT,
                   0,
                   nResult);
   }
   PHandleClose(pContext, pJupiterSEParameter->hSEConnection);
   CMemoryFree(pJupiterSEParameter);
}

static const uint32_t g_nINDEX_HANDSET_OFF = 5;
static const uint32_t g_nINDEX_HANDSET_ON = 7;

/* Build the APDU value of the policy */
static uint8_t* static_PJupiterSEBuildPolicy(
            tPJupiterSEParamter* pJupiterSEParameter,
            uint32_t nProtocolPolicy,
            uint8_t* pValue)
{
   if (nProtocolPolicy != W_NFCC_DISABLE_ALL_PROTOCOLS)
   {
      pValue[0] = 0;
      pValue[1] = 0;

      if ((nProtocolPolicy & W_NFCC_PROTOCOL_CARD_ISO_14443_4_A) != 0)
      {
         pValue[0] |= 0x00; pValue[1] |= 0x10;
      }
      if ((nProtocolPolicy & W_NFCC_PROTOCOL_CARD_ISO_14443_4_B) != 0)
      {
         pValue[0] |= 0x00; pValue[1] |= 0x20;
      }
      if ((nProtocolPolicy & W_NFCC_PROTOCOL_CARD_MIFARE_CLASSIC) != 0)
      {
         pValue[0] |= 0x00; pValue[1] |= 0x80;
      }
      if ((nProtocolPolicy & W_NFCC_PROTOCOL_CARD_ISO_15693_2) != 0)
      {
         pValue[0] |= 0x01; pValue[1] |= 0x00;
      }
   }
   else
   {
      pValue[0] = 0xFF; pValue[1] = 0xFF;

      if(pJupiterSEParameter->bNoSupportFor0xFFFF != false)
      {
         /* Use ISO 14443 B instead of none */
         pValue[0] = 0x00; pValue[1] = 0x20;
      }
   }

   return pValue;
}

/**
 * Callback of P7816ExchangeAPDU.
 *
 * @param[in]  pContext context.
 *
 * @param[in]  pCallbackParameter  The blind parameter given to the function initiating the operation.
 *
 * @param[in]  nDataLength  The length in bytes of the data received.
 *
 * @param[in]  nResult  The result code of the operation:
 **/
static void static_P7816ExchangeSelectAPDUForPolicyCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nDataLength,
               W_ERROR nResult)
{
   tPJupiterSEParamter* pJupiterSEParameter = (tPJupiterSEParamter*)pCallbackParameter;
   uint8_t* pCardToReaderBuffer = pJupiterSEParameter->aResponseBuffer;
   uint8_t aValue[2];

   /* Set enabled SE protocols APDU */
   static uint8_t aSetPolicyAPDU[] = {0x90, 0xDA, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};

   /* Check the error code return */
   if ( nResult != W_SUCCESS )
   {
      goto send_error;
   }

   /* Check the parameters */
   if ( nDataLength < 2 )
   {
      nResult = W_ERROR_RF_COMMUNICATION;
      goto send_error;
   }

   /* Check the SW1 & SW2 values */
   if ( pCardToReaderBuffer[nDataLength - 2] != 0x90 )
   {
      PDebugError("static_P7816ExchangeSelectAPDUForPolicyCompleted: received %d bytes SW1=0x%02X SW2=0x%02X",
                     nDataLength,
                     pCardToReaderBuffer[nDataLength - 2],
                     pCardToReaderBuffer[nDataLength - 1]);
      nResult = W_ERROR_RF_COMMUNICATION;
      goto send_error;
   }

   /* The persistent policy of WSESetPolicy() shall be mapped on the "handset OFF" policy of the SE. */
   CMemoryCopy(&aSetPolicyAPDU[g_nINDEX_HANDSET_OFF], static_PJupiterSEBuildPolicy(pJupiterSEParameter, pJupiterSEParameter->nPersistentPolicy , aValue), 2);

   /* The volatile policy of WSESetPolicy() shall be mapped on the "handset ON" policy of the SE. */
   CMemoryCopy(&aSetPolicyAPDU[g_nINDEX_HANDSET_ON], static_PJupiterSEBuildPolicy(pJupiterSEParameter, pJupiterSEParameter->nVolatilePolicy , aValue), 2);

   /* Copy the command */
   CMemoryCopy(pJupiterSEParameter->aAllocatedPolicyAPDUBuffer, aSetPolicyAPDU, sizeof(aSetPolicyAPDU));

   /* 3 send SET POLICY APDU */
   PDebugTrace("3: send SET POLICY APDU");
   P7816ExchangeAPDU(
         pContext,
         pJupiterSEParameter->hSEConnection,
         static_P7816ExchangeAPDUCompleted,
         pJupiterSEParameter,
         pJupiterSEParameter->aAllocatedPolicyAPDUBuffer,
         sizeof(pJupiterSEParameter->aAllocatedPolicyAPDUBuffer),
         pJupiterSEParameter->aResponseBuffer, 2,
         null);
   return;

send_error:

   PDebugError("static_P7816ExchangeSelectAPDUForPolicyCompleted: Sending error %s", PUtilTraceError(nResult));

   pJupiterSEParameter->nPendingError = nResult;
   PHandleCloseSafe(pContext, pJupiterSEParameter->hSEConnection, static_CloseHandleSafeCompleted, pJupiterSEParameter);
}

/**
 * Callback of P7816ExchangeAPDU
 *
 * @param[in]  pContext context.
 *
 * @param[in]  pCallbackParameter  The blind parameter given to the function initiating the operation.
 *
 * @param[in]  nDataLength  The length in bytes of the data received.
 *
 * @param[in]  nResult  The result code of the operation:
 **/
static void static_P7816ExchangeSelectAPDUForAIDCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nDataLength,
               W_ERROR nResult)
{
   tPJupiterSEParamter* pJupiterSEParameter = (tPJupiterSEParamter*)pCallbackParameter;
   static uint8_t aGetAidAPDU[] = {0x90, 0xCA, 0x01, 0x01};
   uint8_t* pCardToReaderBuffer = pJupiterSEParameter->aResponseBuffer;
   uint32_t nReceivedAPDUBufferMaxLength;

   /* Check the error code return */
   if ( nResult != W_SUCCESS )
   {
      goto send_error;
   }

   /* Check the parameters */
   if ( nDataLength < 2 )
   {
      nResult = W_ERROR_RF_COMMUNICATION;
      goto send_error;
   }

   /* Check the SW1 & SW2 values */
   if ( pCardToReaderBuffer[nDataLength - 2] != 0x90 )
   {
      PDebugError("static_P7816ExchangeSelectAPDUForPolicyCompleted: received %d bytes SW1=0x%02X SW2=0x%02X",
                     nDataLength,
                     pCardToReaderBuffer[nDataLength - 2],
                     pCardToReaderBuffer[nDataLength - 1]);
      nResult = W_ERROR_RF_COMMUNICATION;
      goto send_error;
   }

   /* Copy the command */
   CMemoryCopy(pJupiterSEParameter->aAllocatedAidAPDUBuffer, &aGetAidAPDU[0], sizeof(aGetAidAPDU));

   /* 3 send GET AID APDU */
   PDebugTrace("3: send GET AID APDU");
   nReceivedAPDUBufferMaxLength = (P_BUFFER_MAX_SIZE < pJupiterSEParameter->nAIDsLength) ? P_BUFFER_MAX_SIZE : pJupiterSEParameter->nAIDsLength;
   P7816ExchangeAPDU(
      pContext,
      pJupiterSEParameter->hSEConnection,
      static_P7816ExchangeAPDUCompleted,
      pJupiterSEParameter,
      pJupiterSEParameter->aAllocatedAidAPDUBuffer,
      sizeof(pJupiterSEParameter->aAllocatedAidAPDUBuffer),
      pJupiterSEParameter->aResponseBuffer,
      nReceivedAPDUBufferMaxLength,
      null);
   return;

send_error:

   PDebugError("static_P7816ExchangeSelectAPDUForAIDCompleted: Sending error %s", PUtilTraceError(nResult));

   pJupiterSEParameter->nPendingError = nResult;
   PHandleCloseSafe(pContext, pJupiterSEParameter->hSEConnection, static_CloseHandleSafeCompleted, pJupiterSEParameter);
}

/**
 * Callback of PSEOpenConnection
 *
 * @param[in]  pContext context.
 *
 * @param[in]  pCallbackParameter  The blind parameter given to the function initiating the operation.
 *
 * @param[in]  hHandle  the conncetion handle with the SE.
 *
 * @param[in]  nResult  The result code of the operation:
 **/
static void static_PSEOpenConnectionCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               W_HANDLE hHandle,
               W_ERROR nResult)
{
   tPJupiterSEParamter* pJupiterSEParameter = (tPJupiterSEParamter*)pCallbackParameter;

   PDebugTrace("static_PSEOpenConnectionCompleted()");

   if(nResult != W_SUCCESS)
   {
      PDebugError("static_PSEOpenConnectionCompleted: error %s returned by PSEOpenConnectionCompleted()", PUtilTraceError(nResult));
      if (pJupiterSEParameter->bIsPolicyRequest)
      {
         PDFCPostContext2( &pJupiterSEParameter->sCallbackContext,
                      P_DFC_TYPE_SECURE_ELEMENT,
                      nResult);
      }
      else
      {
         PDFCPostContext3( &pJupiterSEParameter->sCallbackContext,
                      P_DFC_TYPE_SECURE_ELEMENT,
                      0,
                      nResult);
      }
      CMemoryFree(pJupiterSEParameter);
      return;
   }

   /* Store the connection information */
   pJupiterSEParameter->hSEConnection = hHandle;

   /* Copy the SELECT AID command */
   pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer[0] = 0x00;
   pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer[1] = 0xA4;
   pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer[2] = 0x04;
   pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer[3] = 0x00;
   pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer[4] = 0x0F;
   CMemoryCopy(&pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer[5], g_aJupiterAID, sizeof(g_aJupiterAID));

   /* 2 Select the Jupiter applet AID */
   PDebugTrace("2: Select the Jupiter applet AID");

   P7816ExchangeAPDU(
      pContext,
      pJupiterSEParameter->hSEConnection,
      (pJupiterSEParameter->bIsPolicyRequest != false)?static_P7816ExchangeSelectAPDUForPolicyCompleted:static_P7816ExchangeSelectAPDUForAIDCompleted,
      pJupiterSEParameter,
      pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer,
      sizeof(pJupiterSEParameter->aAllocatedSelectJupiterAPDUBuffer),
      pJupiterSEParameter->aResponseBuffer, 2,
      null);
}

/**
 * @brief Sets the policy for a Jupiter SE.
 *
 * The policy of the SE is set with the value of the NFC Controller policy.
 *
 * If the callback is called with an error code, the policy is not modified.
 *
 * The callback function returns one of the following result codes:
 *  - \ref W_SUCCESS The flag value is set.
 *  - \ref W_ERROR_BAD_PARAMETER One of the parameter is wrong.
 *  - \ref W_ERROR_EXCLUSIVE_REJECTED A set policy operation is already pending.
 *  - \ref W_ERROR_FEATURE_NOT_SUPPORTED The parameter values are valid but
 *    this configuration is not supported by the Secure Element.
 *  - others If any other error occurred.
 *
 * @param[in]  nSlotIdentifier  The slot identifier in the range [0, \ref W_NFCC_PROP_SE_NUMBER - 1].
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameter  A blind parameter provided to the callback function.
 *
 * @see     WSESESetPolicy().
 *
 * @since Open NFC 4.2
 **/
void PJupiterSEApplyPolicy(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter)
{
   tPJupiterSEParamter* pJupiterSEParameter;
   tSEInfo sSEInfo;
   W_ERROR nError;

   nError = PSEGetInfo(pContext, nSlotIdentifier, &sSEInfo);
   if (nError != W_SUCCESS)
   {
      PDebugError("PJupiterSEApplyPolicy : Error returned by PSEGetInfo()");
      goto return_error;
   }

   pJupiterSEParameter = (tPJupiterSEParamter*)CMemoryAlloc(sizeof(tPJupiterSEParamter));

   if (pJupiterSEParameter == null)
   {
      PDebugError("PJupiterSEApplyPolicy : CMemoryAlloc() failed");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   CMemoryFill(pJupiterSEParameter, 0, sizeof(tPJupiterSEParamter));
   pJupiterSEParameter->bIsPolicyRequest = true;
   pJupiterSEParameter->nPersistentPolicy = sSEInfo.nPersistentPolicy;
   pJupiterSEParameter->nVolatilePolicy = sSEInfo.nVolatilePolicy;
   pJupiterSEParameter->nPendingError = W_SUCCESS;

   PDFCFillCallbackContext( pContext,
         (tDFCCallback*)pCallback, pCallbackParameter,
         &pJupiterSEParameter->sCallbackContext );

   /* 1 Open SE channel */
   PDebugTrace("1: Open SE channel");
   PSEOpenConnection(pContext, nSlotIdentifier, true,
                     static_PSEOpenConnectionCompleted, pJupiterSEParameter);

   return;

return_error:

   {
      /* Prepare the callback context */
      tDFCCallbackContext sCallbackContext;
      PDFCFillCallbackContext( pContext,(tDFCCallback*)pCallback,
               pCallbackParameter, &sCallbackContext);
      /* Send the error */
      PDebugError("PJupiterSEApplyPolicy : Returning error %s", PUtilTraceError(nError));
      PDFCPostContext2( &sCallbackContext,
                        P_DFC_TYPE_SECURE_ELEMENT,
                        nError );
   }
}

/* This function is a synchronous helper of WJupiterSEApplyPolicy() */
W_ERROR WJupiterSEApplyPolicySync(
               uint32_t nSlotIdentifier)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WJupiterSEApplyPolicy(
         nSlotIdentifier,
         PBasicGenericSyncCompletion, &param);
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/**
 * @brief Returns the AID list from the Secure Element.
 *
 * The format of the data copied in the buffer is a list of one or several AID values.
 * Each AID is stored with a length-value pair. The length is stored on one byte
 * and is followed by the bytes of the AID. The length value is the number of bytes in the AID.
 *
 * Upon completion, the callback method receives the actual length of the AID list stored inthe buffer.
 *
 * @param[in]  nSlotIdentifier  The slot identifier.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameter  The callback parameter.
 *
 * @param[out] pBuffer  A pointer on a buffer receiving the list of AID values.
 *             The content of the buffer is left unchanged if WJupiterSEGetAID()
 *             returns an error.
 *
 * @param[in]  nBufferLength  The length in bytes of the buffer.
 *
 * @since Open NFC 4.2
 **/
void PJupiterSEGetAID(
            tContext* pContext,
            uint32_t nSlotIdentifier,
            tPBasicGenericDataCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t* pBuffer,
            uint32_t nBufferLength)
{
   W_ERROR nError;
   tPJupiterSEParamter* pJupiterSEParameter;

   /* Check the parameters */
   if (  (  ( pBuffer == null )
         && ( nBufferLength != 0 ) )
      || (  ( pBuffer != null )
         && ( nBufferLength == 0 ) ) )
   {
      PDebugError("PJupiterSEGetAID: bad buffer parameter");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   pJupiterSEParameter =
      (tPJupiterSEParamter*)CMemoryAlloc(sizeof(tPJupiterSEParamter));
   if (pJupiterSEParameter == null)
   {
      PDebugError("PJupiterSEGetAID: CMemoryAlloc() failed");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }
   CMemoryFill(pJupiterSEParameter, 0, sizeof(tPJupiterSEParamter));

   pJupiterSEParameter->bIsPolicyRequest = false;
   pJupiterSEParameter->pAIDs = pBuffer;
   pJupiterSEParameter->nAIDsLength = nBufferLength;
   pJupiterSEParameter->nPendingError = W_SUCCESS;

   PDFCFillCallbackContext( pContext,
         (tDFCCallback*)pCallback, pCallbackParameter,
         &pJupiterSEParameter->sCallbackContext );

   /* 1 Open SE channel */
   PDebugTrace("PJupiterSEGetAID Step 1: Open SE channel");
   PSEOpenConnection(pContext, nSlotIdentifier, true,
                     static_PSEOpenConnectionCompleted, pJupiterSEParameter);
   return;

return_error:

   PDebugError("PJupiterSEGetAID: return error %s", PUtilTraceError(nError));

   {
      /* Prepare the callback context */
      tDFCCallbackContext sCallbackContext;
      PDFCFillCallbackContext( pContext,(tDFCCallback*)pCallback,
               pCallbackParameter, &sCallbackContext);
      /* Send the error */
      PDFCPostContext3( &sCallbackContext,
                        P_DFC_TYPE_SECURE_ELEMENT,
                        0,
                        nError );
   }
}

/* This function is a synchronous helper of WJupiterSEGetAID() */
W_ERROR WJupiterSEGetAIDSync(
            uint32_t nSlotIdentifier,
            uint8_t* pBuffer,
            uint32_t nBufferLength,
            uint32_t* pnActualBufferLength )
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WJupiterSEGetAID(
         nSlotIdentifier,
         PBasicGenericSyncCompletionUint32, &param,
         pBuffer, nBufferLength );
   }

   return PBasicGenericSyncWaitForResultUint32(&param, pnActualBufferLength);
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* P_INCLUDE_JUPITER_SE */

