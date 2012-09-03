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
   Contains the implementation of the LLCP connection oriented API
*******************************************************************************/

#define P_MODULE  P_MODULE_DEC( P2P_LLC )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* See client API */
W_ERROR WP2PConnectSync(
   W_HANDLE hSocket,
   W_HANDLE hLink)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WP2PConnect(
         hSocket,
         hLink,
         PBasicGenericSyncCompletion, &param);
   }

   return PBasicGenericSyncWaitForResult(&param);
}


static void static_PP2PConnectDriverCompleted(
      tContext * pContext,
      void * pCallbackParameter,
      W_ERROR nError)
{
   PDFCPostContext2(pCallbackParameter, P_DFC_TYPE_NFCC, nError);
   CMemoryFree(pCallbackParameter);
}

void PP2PConnect(
      tContext * pContext,
      W_HANDLE hSocket,
      W_HANDLE hLink,
      tPBasicGenericCallbackFunction * pEstablishmentCallback,
      void * pEstablishmentCallbackParameter)
{
   tDFCCallbackContext * pCallbackContext = CMemoryAlloc(sizeof(tDFCCallbackContext));
   W_ERROR nError;

   if (pCallbackContext != null)
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pEstablishmentCallback, pEstablishmentCallbackParameter, pCallbackContext);

      PP2PConnectDriver(
         pContext,
         hSocket,
         hLink,
         static_PP2PConnectDriverCompleted, pCallbackContext);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_NFCC, static_PP2PConnectDriverCompleted, pCallbackContext, nError);
      }
   }
   else
   {
      tDFCCallbackContext sCallbackContext;

      PDFCFillCallbackContext(pContext, (tDFCCallback *) pEstablishmentCallback, pEstablishmentCallbackParameter, &sCallbackContext);
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NFCC, W_ERROR_OUT_OF_RESOURCE);
   }
}


static void static_PP2PShutdownDriverCompleted(
      tContext * pContext,
      void * pCallbackParameter,
      W_ERROR nError)
{
   PDFCPostContext2(pCallbackParameter, P_DFC_TYPE_NFCC, nError);
   CMemoryFree(pCallbackParameter);
}

void PP2PShutdown(
      tContext * pContext,
      W_HANDLE  hSocket,
      tPBasicGenericCallbackFunction * pReleaseCallback,
      void * pReleaseCallbackParameter)
{
   tDFCCallbackContext * pCallbackContext = CMemoryAlloc(sizeof(tDFCCallbackContext));
   W_ERROR nError;

   if (pCallbackContext != null)
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pReleaseCallback, pReleaseCallbackParameter, pCallbackContext);

      PP2PShutdownDriver(pContext, hSocket, static_PP2PShutdownDriverCompleted, pCallbackContext);

      nError = PContextGetLastIoctlError(pContext);
      if (nError != W_SUCCESS)
      {
         PDFCPost2(pContext, P_DFC_TYPE_NFCC, static_PP2PShutdownDriverCompleted, pCallbackContext, nError);
      }
   }
   else
   {
      tDFCCallbackContext sCallbackContext;

      PDFCFillCallbackContext(pContext, (tDFCCallback *) pReleaseCallback, pReleaseCallbackParameter, &sCallbackContext);
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NFCC, W_ERROR_OUT_OF_RESOURCE);
   }
}

/* See client API */
W_ERROR WP2PShutdownSync(
    W_HANDLE    hSocket
)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WP2PShutdown(
         hSocket,
         PBasicGenericSyncCompletion, &param);
   }

   return PBasicGenericSyncWaitForResult(&param);
}


static void static_PP2PReadCompleted(
   tContext* pContext,
   void * pCallbackParameter,
   uint32_t nDataLength,
   W_ERROR nResult )
{
   tDFCCallbackContext * pCallbackContext = (tDFCCallbackContext *) pCallbackParameter;

   PDFCPostContext3(pCallbackContext, P_DFC_TYPE_P2P, nDataLength, nResult);
   CMemoryFree(pCallbackContext);
}

/* See client API */
void PP2PRead(
   tContext* pContext,
   W_HANDLE hSocket,
   tPBasicGenericDataCallbackFunction * pCallback,
   void * pCallbackParameter,
   uint8_t * pReceptionBuffer,
   uint32_t nReceptionBufferLength,
   W_HANDLE * phOperation )
{
   tDFCCallbackContext * pCallbackContext = (tDFCCallbackContext *) CMemoryAlloc(sizeof(tDFCCallbackContext));
   tDFCCallbackContext   sCallbackContext;
   W_ERROR nError;

   if ( ((pReceptionBuffer == null) && (nReceptionBufferLength != 0)) ||
        ((pReceptionBuffer != null) && (nReceptionBufferLength == 0)) )
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if (pCallbackContext == null) {

      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);
   PP2PReadDriver(pContext, hSocket, static_PP2PReadCompleted, pCallbackContext, pReceptionBuffer, nReceptionBufferLength, phOperation);

   nError = PContextGetLastIoctlError(pContext);
   if (nError != W_SUCCESS)
   {
      PDFCPost3(pContext, P_DFC_TYPE_P2P, static_PP2PReadCompleted, pCallbackContext, 0, nError);
   }
   return;

return_error:

   PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);
   PDFCPostContext3(&sCallbackContext, P_DFC_TYPE_P2P, 0, nError);
}



/** See Client API Specifications */
W_ERROR WP2PReadSync (
   W_HANDLE hConnection,
   uint8_t* pReceptionBuffer,
   uint32_t nReceptionBufferLength,
   uint32_t *pnDataLength)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WP2PRead(
         hConnection,
         PBasicGenericSyncCompletionUint32, &param,
         pReceptionBuffer, nReceptionBufferLength,
         null );
   }

   return PBasicGenericSyncWaitForResultUint32(&param, pnDataLength);
}

static void static_PP2PWriteCompleted(
   tContext* pContext,
   void * pCallbackParameter,
   W_ERROR nResult )
{
   tDFCCallbackContext * pCallbackContext = (tDFCCallbackContext *) pCallbackParameter;

   PDFCPostContext2(pCallbackContext, P_DFC_TYPE_P2P, nResult);
   CMemoryFree(pCallbackContext);
}

void PP2PWrite(
      tContext * pContext,
      W_HANDLE hSocket,
      tPBasicGenericCallbackFunction * pCallback,
      void * pCallbackParameter,
      const uint8_t * pSendBuffer,
      uint32_t nSendBufferLength,
      W_HANDLE * phOperation )
{
   tDFCCallbackContext * pCallbackContext = (tDFCCallbackContext *) CMemoryAlloc(sizeof(tDFCCallbackContext));
   tDFCCallbackContext   sCallbackContext;
   W_ERROR nError;

   if ( ((pSendBuffer == null) && (nSendBufferLength != 0)) ||
        ((pSendBuffer != null) && (nSendBufferLength == 0)) )
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if (pCallbackContext == null) {

      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);
   PP2PWriteDriver(pContext, hSocket, static_PP2PWriteCompleted, pCallbackContext, pSendBuffer, nSendBufferLength, phOperation);

   nError = PContextGetLastIoctlError(pContext);
   if (nError != W_SUCCESS)
   {
      PDFCPost2(pContext, P_DFC_TYPE_P2P, static_PP2PWriteCompleted, pCallbackContext, nError);
   }
   return;

return_error:

   PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_P2P, nError);
}

/** See Client API Specifications */
W_ERROR WP2PWriteSync (
                  W_HANDLE hConnection,
                  const uint8_t* pSendBuffer,
                  uint32_t nSendBufferLength)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WP2PWrite(
         hConnection,
         PBasicGenericSyncCompletion, &param,
         pSendBuffer, nSendBufferLength,
         null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

#endif

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)


/**
 * Calls connect callback.
 *
 * @param[in] pContext     The Context
 * @param[in] pP2PSocket   The socket
 * @param[in] nError       The error code
 *
 * @return void
 */

static void static_PP2PCallConnectCallback(
   tContext     * pContext,
   tP2PSocket  * pP2PSocket,
   W_ERROR        nError)
{
   PDebugTrace("static_PP2PCallConnectCallback : nError %d", nError);

   if ((pP2PSocket != null) && (pP2PSocket->sConnection.bConnectInProgress == true))
   {
      pP2PSocket->sConnection.bIsConnectCalled = true;
      pP2PSocket->sConnection.bConnectInProgress = false;
      PDFCDriverPostCC2(pP2PSocket->sConnection.pConnectCC, P_DFC_TYPE_P2P, nError);
   }
}

/** See WP2PConnect */
void PP2PConnectDriver(
   tContext * pContext,
   W_HANDLE hSocket,
   W_HANDLE hLink,
   tPBasicGenericCallbackFunction * pCallback,
   void * pCallbackParameter
)
{
   tDFCDriverCCReference pDriverCC;
   tP2PInstance * pP2PInstance = PContextGetP2PInstance(pContext);
   tP2PLink * pP2PLink;
   tP2PSocket * pP2PSocket;
   tLLCPConnection * pConnection = null;
   W_ERROR nError;

   PDFCDriverFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter,& pDriverCC);

   if (PP2PCheckP2PSupport(pContext) == false)
   {
      nError = W_ERROR_RF_PROTOCOL_NOT_SUPPORTED;
      goto error;
   }

   if ( (nError = PP2PGetLinkObject(pContext, hLink, &pP2PLink)) != W_SUCCESS)
   {
      PDebugError("PP2PConnect : invalid hLink value");
      goto error;
   }

   if ( (nError = PP2PGetSocketObject(pContext, hSocket, &pP2PSocket)) != W_SUCCESS)
   {
      PDebugError("PP2PConnect : invalid hSocket value");
      goto error;
   }

   if (pP2PSocket->sConfig.nType == W_P2P_TYPE_CONNECTIONLESS)
   {
      PDebugError("PP2PConnect : called on a connectionless socket");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto error;
   }

   if (pP2PInstance->sLLCPInstance.nRole == LLCP_ROLE_NONE)
   {
      PDebugError("PP2PConnect : link is no longer established");
      nError = W_ERROR_BAD_STATE;
      goto error;
   }

   if (pP2PSocket->sConnection.bIsConnected == true)
   {
      if (pP2PSocket->sConnection.bIsConnectCalled == true)
      {
         PDebugError("PP2PConnect : called on an already connected socket");
         nError = W_ERROR_BAD_STATE;
         goto error;
      }
      else
      {
         PDebugError("PP2PConnect : connection already established");

         pP2PSocket->sConnection.bConnectInProgress = true;
         pP2PSocket->sConnection.pConnectCC = pDriverCC;
         static_PP2PCallConnectCallback(pContext, pP2PSocket, W_SUCCESS);
         return;
      }
   }

   PP2PCleanSocketContext(pContext, pP2PSocket);

   if ((pP2PSocket->sConfig.nType == W_P2P_TYPE_CLIENT) || (pP2PSocket->sConfig.nType == W_P2P_TYPE_CLIENT_SERVER))
   {
      pConnection = PLLCPConnectionAllocContext(pContext, pP2PSocket->sConfig.nLocalSap, 0, pP2PSocket);

      if (pConnection == null)
      {
         PDebugError("PP2PConnect : no resources");
         nError = W_ERROR_OUT_OF_RESOURCE;
         goto error;
      }

      pP2PSocket->sConnection.pPendingConnection = pConnection;

      /* ok, all is fine, establish the connection */
      if (pP2PSocket->sConfig.pRemoteURI)
      {
         nError = PLLCPConnect(pContext, pConnection, pP2PSocket->sConfig.pRemoteURI, pP2PSocket->sConfig.nRemoteURILength - 1, pP2PSocket->sConfig.nRemoteSAP);
      }
      else
      {
         nError = PLLCPConnect(pContext, pConnection, null, 0, pP2PSocket->sConfig.nRemoteSAP);
      }

      if (nError != W_SUCCESS)
      {
         goto error;
      }
   }

   pP2PSocket->sConnection.bConnectInProgress = true;
   pP2PSocket->sConnection.pConnectCC = pDriverCC;

   return;

error:

   if (pConnection)
   {
      PLLCPConnectionFreeContext(pContext, pConnection);
   }

   PDFCDriverPostCC2(pDriverCC, P_DFC_TYPE_P2P, nError);
}

/**
  * call P2P socket disconnection callback
  *
  * @param[in] pContext The current context
  *
  * @param[in] pP2PSocket The socket
  *
  * @param[in] nError The error
  */
static void static_PP2PCallDisconnectCallback(
   tContext     * pContext,
   tP2PSocket  * pP2PSocket,
   W_ERROR        nError)
{
   PDebugTrace("static_PP2PCallDisconnectCallback : nError %d", nError);

   if (pP2PSocket->sConnection.bShutdownInProgress == true)
   {
      pP2PSocket->sConnection.bShutdownInProgress = false;
      PDFCDriverPostCC2(pP2PSocket->sConnection.pShutdownCC, P_DFC_TYPE_P2P, nError);
   }
}

/** See WP2PShutdown */
void PP2PShutdownDriver(
   tContext * pContext,
   W_HANDLE  hSocket,
   tPBasicGenericCallbackFunction * pCallback,
   void * pCallbackParameter
)
{
   tP2PInstance * pP2PInstance = PContextGetP2PInstance(pContext);
   tDFCDriverCCReference pDriverCC;
   tP2PSocket * pP2PSocket;
   W_ERROR nError;

   PDFCDriverFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter,& pDriverCC);

   if (PP2PCheckP2PSupport(pContext) == false)
   {
      nError = W_ERROR_RF_PROTOCOL_NOT_SUPPORTED;
      goto error;
   }

   if ( (nError = PP2PGetSocketObject(pContext, hSocket, &pP2PSocket)) != W_SUCCESS)
   {
      goto error;
   }

   if (pP2PSocket->sConfig.nType == W_P2P_TYPE_CONNECTIONLESS)
   {
      PDebugError("PP2PShutdown : called on a connectionless socket");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto error;
   }

   if (pP2PInstance->sLLCPInstance.nRole == LLCP_ROLE_NONE)
   {
      PDebugError("PP2PShutdown : link is no longer established");
      nError = W_ERROR_BAD_STATE;
      goto error;
   }

   if ((pP2PSocket->sConnection.bIsConnected == false) && (pP2PSocket->sConnection.bConnectInProgress == false))
   {
      PDebugError("PP2PShutdown : called on an non connected socket");
      nError = W_ERROR_BAD_STATE;
      goto error;
   }

   if (pP2PSocket->sConnection.bShutdownInProgress == true)
   {
      PDebugError("PP2PShutdown : called on an non connected socket");
      nError = W_ERROR_BAD_STATE;
      goto error;
   }

   if (pP2PSocket->sConnection.bIsConnected == true)
   {
      pP2PSocket->sConnection.bShutdownInProgress = true;
      pP2PSocket->sConnection.pShutdownCC = pDriverCC;

      PLLCPDisconnect(pContext, pP2PSocket->sConnection.pConnection);
   }
   else
   {
      static_PP2PCallConnectCallback(pContext, pP2PSocket, W_ERROR_CANCEL);
      PDFCDriverFlushCall(pDriverCC);
   }
   return;

error:

   PDFCDriverPostCC2(pDriverCC, P_DFC_TYPE_P2P, nError);
}

/* See header */
void PP2PConnectCompleteCallback
(
   tContext         * pContext,
   tLLCPConnection  * pConnection,
   W_ERROR            nError
)
{
   tP2PSocket * pP2PSocket = pConnection->pP2PSocket;

   if (nError == W_SUCCESS)
   {
      pP2PSocket->sConnection.bIsConnected = true;
      pP2PSocket->sConnection.bIsServer = false;
      pP2PSocket->sConnection.pConnection = pConnection;
      static_PP2PCallConnectCallback(pContext, pP2PSocket, W_SUCCESS);
   }
   else
   {
      if (pP2PSocket->sConfig.nType == W_P2P_TYPE_CLIENT)
      {
         static_PP2PCallConnectCallback(pContext, pP2PSocket, nError);
         PLLCPConnectionFreeContext(pContext, pConnection);
      }
   }
}

/* See header */
void PP2PConnectIndicationCallback
(
   tContext         * pContext,
   tLLCPConnection  * pConnection
)
{
   tP2PSocket * pP2PSocket = pConnection->pP2PSocket;

   pP2PSocket->sConnection.bIsConnected = true;
   pP2PSocket->sConnection.bIsServer = true;
   pP2PSocket->sConnection.pConnection = pConnection;

   static_PP2PCallConnectCallback(pContext, pConnection->pP2PSocket, W_SUCCESS);
}

/* See header */
void PP2PDisconnectIndicationCallback
(
   tContext         * pContext,
   tLLCPConnection  * pP2PConnection
)
{
   PDebugTrace("PP2PDisconnectIndicationCallback");

   pP2PConnection->pP2PSocket->sConnection.bIsConnected = false;

   /* call the pending completion callbacks */
   PP2PCallSocketReadAndWriteCallbacks(pContext, pP2PConnection->pP2PSocket, W_ERROR_CANCEL);

   /* call the disconnect callback if any */
   static_PP2PCallDisconnectCallback(pContext, pP2PConnection->pP2PSocket, W_SUCCESS);
}

/* See header */
void PP2PDisconnectCompleteCallback(
   tContext * pContext,
   tLLCPConnection  * pP2PConnection,
   W_ERROR nError)
{
   PDebugTrace("PP2PDisconnectCompleteCallback %s", PUtilTraceError(nError));

   pP2PConnection->pP2PSocket->sConnection.bIsConnected = false;
   pP2PConnection->pP2PSocket->sConnection.pConnection = null;

   /* call the pending completion callbacks */
   PP2PCallSocketReadAndWriteCallbacks(pContext, pP2PConnection->pP2PSocket, W_ERROR_CANCEL);

   /* call the disconnect callback if any */
   static_PP2PCallDisconnectCallback(pContext, pP2PConnection->pP2PSocket, W_SUCCESS);
}

/* See header */
void PP2PCallAllSocketConnectCallbacks(
   tContext * pContext,
   W_ERROR nError)
{
   tP2PInstance * pP2PInstance = PContextGetP2PInstance(pContext);
   tP2PSocket * pP2PSocket;

   PDebugTrace("PP2PCallAllSocketConnectCallbacks");

   for (pP2PSocket = pP2PInstance->pFirstSocket; pP2PSocket != null; pP2PSocket = pP2PSocket->pNext)
   {
      static_PP2PCallConnectCallback(pContext, pP2PSocket, nError);
   }
}

/**
  * Operation cancellation callback called when user call PBasicCancelOperation
  * on the operation handle returned in PP2PRead()
  *
  * @param[in) pContext          The context
  *
  * @param[in] pCancelParameter  The cancel parameter, e.g pP2Pservice
  *
  * @param[in] bIsClosing
  *
  */
static void static_PP2PCancelReadOperation(
   tContext * pContext,
   void     * pCancelParameter,
   bool       bIsClosing
)
{
   tP2PSocket * pP2PSocket  = pCancelParameter;

   PDebugTrace("static_PP2PCancelReadOperation");

   if (bIsClosing == false)
   {
      PP2PCallSocketReadCallback(pContext, pP2PSocket, 0, W_ERROR_CANCEL, 0);
   }
   else
   {
      pP2PSocket->hRecvOperation = W_NULL_HANDLE;
   }
}

/** See WP2PRead */
void PP2PReadDriver (
   tContext             * pContext,
   W_HANDLE               hSocket,
   tPBasicGenericDataCallbackFunction   * pCallback,
   void                 * pCallbackParameter,
   uint8_t              * pReceptionBuffer,
   uint32_t               nReceptionBufferLength,
   W_HANDLE             * phOperation)
{
   tDFCDriverCCReference pDriverCC;
   W_ERROR               nError;
   tP2PSocket          * pP2PSocket;

   /* some inits */
   PDFCDriverFillCallbackContext( pContext, (tDFCCallback*)pCallback, pCallbackParameter, &pDriverCC);

   nError = PP2PGetSocketObject(pContext, hSocket, & pP2PSocket);

   if (nError != W_SUCCESS)
   {
      goto error;
   }

   if (pP2PSocket->sConfig.nType == W_P2P_TYPE_CONNECTIONLESS)
   {
      PDebugError("PP2PRead on a connectionless socket\n");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto error;
   }

   if ((pP2PSocket->sConnection.bIsConnected == false) && (pP2PSocket->pFirstRecvPDU == null))
   {
      PDebugError("PP2PRead on a non connected socket\n");
      nError = W_ERROR_BAD_STATE;
      goto error;
   }

   /* check if there's not a pending read operation in progress */

   if (pP2PSocket->bRecvInProgress == true)
   {
      PDebugError("PP2PRead : Read operation still in progress");

      nError = W_ERROR_RETRY;
      goto error;
   }

   if ( ((pReceptionBuffer == null) && (nReceptionBufferLength != 0)) ||
        ((pReceptionBuffer != null) && (nReceptionBufferLength == 0)))
   {
      PDebugError("PP2PRead : pReceptionBuffer, nReceptionBufferLength inconsistency");
      nError = W_ERROR_BAD_PARAMETER;
      goto error;
   }


   /* if an operation is required, try to create it */
   if (phOperation != null)
   {
      * phOperation = pP2PSocket->hRecvOperation = PBasicCreateOperation(pContext, static_PP2PCancelReadOperation, pP2PSocket);
   }

   /* all is fine */
   pP2PSocket->bRecvInProgress = true;
   pP2PSocket->pRecvCC = pDriverCC;
   pP2PSocket->pRecvBuffer = pReceptionBuffer;
   pP2PSocket->nRecvBufferLength = nReceptionBufferLength;

   PLLCPConnectionStartRecvSDU(pContext, pP2PSocket->sConnection.pConnection);

   return;

error:

   PDFCDriverPostCC3(pDriverCC, P_DFC_TYPE_P2P, 0, nError);
}

/**
  * Operation cancellation callback called when user call PBasicCancelOperation
  * on the operation handle returned in PP2PWrite()
  *
  * @param[in) pContext          The context*
  *
  * @param[in] pCancelParameter  The cancel parameter, e.g pP2Pservice
  *
  * @param[in] bIsClosing
  */
static void static_PP2PCancelWriteOperation(
   tContext * pContext,
   void     * pCancelParameter,
   bool       bIsClosing
)
{
   tP2PSocket  * pP2PSocket  = (tP2PSocket *) pCancelParameter;

   PDebugTrace("static_PP2PCancelWriteOperation");

   if (bIsClosing == false)
   {
      PP2PCallSocketWriteCallback(pContext, pP2PSocket, W_ERROR_CANCEL);
   }
   else
   {
      pP2PSocket->hXmitOperation = W_NULL_HANDLE;
   }
}

/* See WP2PWrite */
void PP2PWriteDriver(
   tContext             * pContext,
   W_HANDLE hSocket,
   tPBasicGenericCallbackFunction* pCallback,
   void* pCallbackParameter,
   const uint8_t* pSendBuffer,
   uint32_t nSendBufferLength,
   W_HANDLE* phOperation)

{
   tDFCDriverCCReference pDriverCC;
   W_ERROR               nError;
   tP2PSocket          * pP2PSocket;

      /* some inits */
   PDFCDriverFillCallbackContext( pContext, (tDFCCallback*)pCallback, pCallbackParameter, & pDriverCC );

   nError = PP2PGetSocketObject(pContext, hSocket, &pP2PSocket);

   if (nError != W_SUCCESS)
   {
      goto error;
   }

   if (pP2PSocket->sConfig.nType == W_P2P_TYPE_CONNECTIONLESS)
   {
      PDebugError("PP2PWrite on a connectionless socket\n");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto error;
   }

   if (pP2PSocket->sConnection.bIsConnected == false)
   {
      PDebugError("PP2PWrite on a non connected socket\n");
      nError = W_ERROR_BAD_STATE;
      goto error;
   }

   /* check if there's not a pending write operation in progress */

   if (pP2PSocket->bXmitInProgress == true)
   {
      PDebugError("PP2PWrite : wrote operation still in progress");

      nError = W_ERROR_RETRY;
      goto error;
   }

   if ( ((pSendBuffer == null) && (nSendBufferLength != 0)) ||
        ((pSendBuffer != null) && (nSendBufferLength == 0)))
   {
      PDebugError("PP2PWrite : pSendBuffer, nSendBufferLength inconsistency");
      nError = W_ERROR_BAD_PARAMETER;
      goto error;
   }

   /* if an operation is required, try to create it */
   if (phOperation != null)
   {
      * phOperation = pP2PSocket->hXmitOperation = PBasicCreateOperation(pContext, static_PP2PCancelWriteOperation, pP2PSocket);
   }

   /* OK, record the write request */

   pP2PSocket->bXmitInProgress = true;
   pP2PSocket->pXmitBuffer = (uint8_t *) pSendBuffer;
   pP2PSocket->nXmitBufferLength = nSendBufferLength;
   pP2PSocket->pXmitCC = pDriverCC;

   /* trigger SDU transmission */

   PLLCPConnectionStartXmitSDU(pContext, pP2PSocket->sConnection.pConnection);

   return;

error :

   PDFCDriverPostCC2( pDriverCC, P_DFC_TYPE_P2P, nError);
}


void PP2PCallAllDisconnectIndicationCallback(
   tContext * pContext)
{
   tP2PInstance * pP2PInstance = PContextGetP2PInstance(pContext);
   tP2PSocket * pP2PSocket;

   PDebugTrace("PP2PCallAllSocketConnectCallbacks");

   for (pP2PSocket = pP2PInstance->pFirstSocket; pP2PSocket != null; pP2PSocket = pP2PSocket->pNext)
   {
      if (pP2PSocket->sConnection.pConnection != null)
      {
         PP2PDisconnectIndicationCallback(pContext, pP2PSocket->sConnection.pConnection);
      }
   }
}

#endif


