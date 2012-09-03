/*
 * Copyright (c) 2007-2011 Inside Secure, All Rights Reserved.
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
 File auto-generated with the autogen.exe tool - Do not modify manually
 The autogen.exe binary tool, the generation scripts and the files used
 for the source of the generation are available under Apache License, Version 2.0
 ******************************************************************************/

#ifdef P_TRACE_ACTIVE
#include <stdarg.h> /* Needed for the traces */
#endif /* P_TRACE_ACTIVE */
#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER)
extern tContext* g_pContext;

void W14Part3ExchangeData(W_HANDLE hConnection, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P14Part3ExchangeData( pContext, hConnection, null, &dfc, pReaderToCardBuffer, nReaderToCardBufferLength, pCardToReaderBuffer, nCardToReaderBufferMaxLength, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR W14Part3GetConnectionInfo(W_HANDLE hConnection, tW14Part3ConnectionInfo * p14Part3ConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part3GetConnectionInfo( pContext, hConnection, p14Part3ConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR W14Part3GetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part3GetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR W14Part3ListenToCardDetectionTypeB(tWReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint8_t nAFI, bool bUseCID, uint8_t nCID, uint32_t nBaudRate, const uint8_t * pHigherLayerDataBuffer, uint8_t nHigherLayerDataLength, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part3ListenToCardDetectionTypeB( pContext, null, &dfc, nPriority, nAFI, bUseCID, nCID, nBaudRate, pHigherLayerDataBuffer, nHigherLayerDataLength, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR W14Part3SetTimeout(W_HANDLE hConnection, uint32_t nTimeout)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part3SetTimeout( pContext, hConnection, nTimeout );
   PContextReleaseLock(pContext);
   return ret;
}

void W14Part4ExchangeData(W_HANDLE hConnection, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P14Part4ExchangeData( pContext, hConnection, null, &dfc, pReaderToCardBuffer, nReaderToCardBufferLength, pCardToReaderBuffer, nCardToReaderBufferMaxLength, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR W14Part4GetConnectionInfo(W_HANDLE hConnection, tW14Part4ConnectionInfo * p14Part4ConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part4GetConnectionInfo( pContext, hConnection, p14Part4ConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR W14Part4GetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part4GetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR W14Part4ListenToCardDetectionTypeA(tWReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, bool bUseCID, uint8_t nCID, uint32_t nBaudRate, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part4ListenToCardDetectionTypeA( pContext, null, &dfc, nPriority, bUseCID, nCID, nBaudRate, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR W14Part4ListenToCardDetectionTypeB(tWReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint8_t nAFI, bool bUseCID, uint8_t nCID, uint32_t nBaudRate, const uint8_t * pHigherLayerDataBuffer, uint8_t nHigherLayerDataLength, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part4ListenToCardDetectionTypeB( pContext, null, &dfc, nPriority, nAFI, bUseCID, nCID, nBaudRate, pHigherLayerDataBuffer, nHigherLayerDataLength, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR W14Part4SetNAD(W_HANDLE hConnection, uint8_t nNAD)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P14Part4SetNAD( pContext, hConnection, nNAD );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR W15GetConnectionInfo(W_HANDLE hConnection, tW15ConnectionInfo * pConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P15GetConnectionInfo( pContext, hConnection, pConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR W15GetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P15GetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR W15IsWritable(W_HANDLE hConnection, uint8_t nSectorIndex)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P15IsWritable( pContext, hConnection, nSectorIndex );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR W15ListenToCardDetection(tWReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint8_t nAFI, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P15ListenToCardDetection( pContext, null, &dfc, nPriority, nAFI, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

void W15Read(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P15Read( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, phOperation );
   PContextReleaseLock(pContext);
}

void W15SetAttribute(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nActions, uint8_t nAFI, uint8_t nDSFID, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P15SetAttribute( pContext, hConnection, null, &dfc, nActions, nAFI, nDSFID, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR W15SetTagSize(W_HANDLE hConnection, uint16_t nSectorNumber, uint8_t nSectorSize)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P15SetTagSize( pContext, hConnection, nSectorNumber, nSectorSize );
   PContextReleaseLock(pContext);
   return ret;
}

void W15Write(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockSectors, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P15Write( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, bLockSectors, phOperation );
   PContextReleaseLock(pContext);
}

void W7816CloseLogicalChannel(W_HANDLE hLogicalChannel, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P7816CloseLogicalChannel( pContext, hLogicalChannel, null, &dfc );
   PContextReleaseLock(pContext);
}

void W7816ExchangeAPDU(W_HANDLE hConnection, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pSendAPDUBuffer, uint32_t nSendAPDUBufferLength, uint8_t * pReceivedAPDUBuffer, uint32_t nReceivedAPDUBufferMaxLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P7816ExchangeAPDU( pContext, hConnection, null, &dfc, pSendAPDUBuffer, nSendAPDUBufferLength, pReceivedAPDUBuffer, nReceivedAPDUBufferMaxLength, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR W7816GetATR(W_HANDLE hConnection, uint8_t * pBuffer, uint32_t nBufferMaxLength, uint32_t * pnActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P7816GetATR( pContext, hConnection, pBuffer, nBufferMaxLength, pnActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR W7816GetATRSize(W_HANDLE hConnection, uint32_t * pnSize)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P7816GetATRSize( pContext, hConnection, pnSize );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR W7816GetResponseAPDUData(W_HANDLE hConnection, uint8_t * pReceivedAPDUBuffer, uint32_t nReceivedAPDUBufferMaxLength, uint32_t * pnReceivedAPDUActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = P7816GetResponseAPDUData( pContext, hConnection, pReceivedAPDUBuffer, nReceivedAPDUBufferMaxLength, pnReceivedAPDUActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

void W7816OpenLogicalChannel(W_HANDLE hConnection, tWBasicGenericHandleCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pAID, uint32_t nAIDLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   P7816OpenLogicalChannel( pContext, hConnection, null, &dfc, pAID, nAIDLength, phOperation );
   PContextReleaseLock(pContext);
}

void WBasicCancelOperation(W_HANDLE hOperation)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PBasicCancelOperation( pContext, hOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WBasicCheckConnectionProperty(W_HANDLE hConnection, uint8_t nPropertyIdentifier)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBasicCheckConnectionProperty( pContext, hConnection, nPropertyIdentifier );
   PContextReleaseLock(pContext);
   return ret;
}

void WBasicCloseHandle(W_HANDLE hHandle)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PBasicCloseHandle( pContext, hHandle );
   PContextReleaseLock(pContext);
}

void WBasicCloseHandleSafe(W_HANDLE hHandle, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PBasicCloseHandleSafe( pContext, hHandle, null, &dfc );
   PContextReleaseLock(pContext);
}

void WBasicExecuteEventLoop(void)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }
   PBasicExecuteEventLoop( pContext );
}

W_ERROR WBasicGetConnectionProperties(W_HANDLE hConnection, uint8_t * pPropertyArray, uint32_t nArrayLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBasicGetConnectionProperties( pContext, hConnection, pPropertyArray, nArrayLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WBasicGetConnectionPropertyNumber(W_HANDLE hConnection, uint32_t * pnPropertyNumber)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBasicGetConnectionPropertyNumber( pContext, hConnection, pnPropertyNumber );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WBasicInit(const tchar * pVersionString)
{
   tContext* pContext;

   pContext = g_pContext;
   return PBasicInit( pContext, pVersionString );
}

W_ERROR WBasicPumpEvent(bool bWait)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }
   return PBasicPumpEvent( pContext, bWait );
}

void WBasicStopEventLoop(void)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }
   PBasicStopEventLoop( pContext );
}

void WBasicTerminate(void)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }
   PBasicTerminate( pContext );
}

void WBasicTestExchangeMessage(tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pSendMessageBuffer, uint32_t nSendMessageBufferLength, uint8_t * pReceivedMessageBuffer)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PBasicTestExchangeMessage( pContext, null, &dfc, pSendMessageBuffer, nSendMessageBufferLength, pReceivedMessageBuffer );
   PContextReleaseLock(pContext);
}

void WBPrimeExchangeData(W_HANDLE hConnection, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PBPrimeExchangeData( pContext, hConnection, null, &dfc, pReaderToCardBuffer, nReaderToCardBufferLength, pCardToReaderBuffer, nCardToReaderBufferMaxLength, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WBPrimeGetConnectionInfo(W_HANDLE hConnection, tWBPrimeConnectionInfo * pBPrimeConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBPrimeGetConnectionInfo( pContext, hConnection, pBPrimeConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR WBPrimeGetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBPrimeGetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR WBPrimeListenToCardDetection(tWReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, const uint8_t * pAPGENBuffer, uint8_t nAPGENLength, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBPrimeListenToCardDetection( pContext, null, &dfc, nPriority, pAPGENBuffer, nAPGENLength, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WBPrimeSetTimeout(W_HANDLE hConnection, uint32_t nTimeout)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBPrimeSetTimeout( pContext, hConnection, nTimeout );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WBTPairingGetRemoteDeviceInfo(W_HANDLE hOperation, tWBTPairingInfo * pRemoteInfo, tchar * pRemoteDeviceNameBuffer, uint32_t nRemoteDeviceNameBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PBTPairingGetRemoteDeviceInfo( pContext, hOperation, pRemoteInfo, pRemoteDeviceNameBuffer, nRemoteDeviceNameBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

void WBTPairingStart(uint32_t nMode, const tWBTPairingInfo * pLocalInfo, const tchar * pLocalDeviceName, tWBTPairingStartCompleted * pCallback, void * pCallbackParameter, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PBTPairingStart( pContext, nMode, pLocalInfo, pLocalDeviceName, null, &dfc, phOperation );
   PContextReleaseLock(pContext);
}


#ifdef P_INCLUDE_JAVA_API
bool WDFCPumpJNICallback(uint32_t * pArgs, uint32_t nArgsSize)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PDFCPumpJNICallback( pContext, pArgs, nArgsSize );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

void WEmulClose(W_HANDLE hHandle, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PEmulClose( pContext, hHandle, null, &dfc );
   PContextReleaseLock(pContext);
}

W_ERROR WEmulGetMessageData(W_HANDLE hHandle, uint8_t * pDataBuffer, uint32_t nDataLength, uint32_t * pnActualDataLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PEmulGetMessageData( pContext, hHandle, pDataBuffer, nDataLength, pnActualDataLength );
   PContextReleaseLock(pContext);
   return ret;
}

bool WEmulIsPropertySupported(uint8_t nPropertyIdentifier)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PEmulIsPropertySupported( pContext, nPropertyIdentifier );
   PContextReleaseLock(pContext);
   return ret;
}

void WEmulOpenConnectionDriver1(tWBasicGenericCallbackFunction * pOpenCallback, void * pOpenCallbackParameter, const tWEmulConnectionInfo * pEmulConnectionInfo, uint32_t nSize, W_HANDLE * phHandle)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PEmulOpenConnectionDriver1( pContext, (tPBasicGenericCallbackFunction *)pOpenCallback, pOpenCallbackParameter, pEmulConnectionInfo, nSize, phHandle );
   PContextReleaseLock(pContext);
}

void WEmulOpenConnectionDriver2(W_HANDLE hHandle, tWEmulDriverEventReceived * pEventCallback, void * pEventCallbackParameter)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PEmulOpenConnectionDriver2( pContext, hHandle, (tPEmulDriverEventReceived *)pEventCallback, pEventCallbackParameter );
   PContextReleaseLock(pContext);
}

void WEmulOpenConnectionDriver3(W_HANDLE hHandle, tWEmulDriverCommandReceived * pCommandCallback, void * pCommandCallbackParameter)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PEmulOpenConnectionDriver3( pContext, hHandle, (tPEmulDriverCommandReceived *)pCommandCallback, pCommandCallbackParameter );
   PContextReleaseLock(pContext);
}

W_ERROR WEmulSendAnswer(W_HANDLE hDriverConnection, const uint8_t * pDataBuffer, uint32_t nDataLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PEmulSendAnswer( pContext, hDriverConnection, pDataBuffer, nDataLength );
   PContextReleaseLock(pContext);
   return ret;
}

void WFeliCaExchangeData(W_HANDLE hUserConnection, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PFeliCaExchangeData( pContext, hUserConnection, null, &dfc, pReaderToCardBuffer, nReaderToCardBufferLength, pCardToReaderBuffer, nCardToReaderBufferMaxLength, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WFeliCaGetCardList(W_HANDLE hConnection, tWFeliCaConnectionInfo * aFeliCaConnectionInfos, const uint32_t nArraySize)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PFeliCaGetCardList( pContext, hConnection, aFeliCaConnectionInfos, nArraySize );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WFeliCaGetCardNumber(W_HANDLE hConnection, uint32_t * pnCardNumber)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PFeliCaGetCardNumber( pContext, hConnection, pnCardNumber );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WFeliCaGetConnectionInfo(W_HANDLE hConnection, tWFeliCaConnectionInfo * pConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PFeliCaGetConnectionInfo( pContext, hConnection, pConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR WFeliCaGetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PFeliCaGetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR WFeliCaListenToCardDetection(tWReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint16_t nSystemCode, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PFeliCaListenToCardDetection( pContext, null, &dfc, nPriority, nSystemCode, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

void WFeliCaRead(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nLength, uint8_t nNumberOfService, const uint16_t * pServiceCodeList, uint8_t nNumberOfBlocks, const uint8_t * pBlockList, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PFeliCaRead( pContext, hConnection, null, &dfc, pBuffer, nLength, nNumberOfService, pServiceCodeList, nNumberOfBlocks, pBlockList, phOperation );
   PContextReleaseLock(pContext);
}


#ifdef P_INCLUDE_JAVA_API
void WFeliCaReadSimple(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nLength, uint32_t nNumberOfService, uint32_t * pServiceCodeList, uint32_t nNumberOfBlocks, const uint8_t * pBlockList)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PFeliCaReadSimple( pContext, hConnection, null, &dfc, pBuffer, nLength, nNumberOfService, pServiceCodeList, nNumberOfBlocks, pBlockList );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

void WFeliCaSelectCard(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const tWFeliCaConnectionInfo * pFeliCaConnectionInfo)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PFeliCaSelectCard( pContext, hConnection, null, &dfc, pFeliCaConnectionInfo );
   PContextReleaseLock(pContext);
}

void WFeliCaSelectSystem(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t nIndexSubSystem)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PFeliCaSelectSystem( pContext, hConnection, null, &dfc, nIndexSubSystem );
   PContextReleaseLock(pContext);
}

void WFeliCaWrite(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nLength, uint8_t nNumberOfService, const uint16_t * pServiceCodeList, uint8_t nNumberOfBlocks, const uint8_t * pBlockList, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PFeliCaWrite( pContext, hConnection, null, &dfc, pBuffer, nLength, nNumberOfService, pServiceCodeList, nNumberOfBlocks, pBlockList, phOperation );
   PContextReleaseLock(pContext);
}


#ifdef P_INCLUDE_JAVA_API
void WFeliCaWriteSimple(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nLength, uint32_t nNumberOfService, uint32_t * pServiceCodeList, uint32_t nNumberOfBlocks, const uint8_t * pBlockList)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PFeliCaWriteSimple( pContext, hConnection, null, &dfc, pBuffer, nLength, nNumberOfService, pServiceCodeList, nNumberOfBlocks, pBlockList );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_HANDLE WJavaCreateByteBuffer(uint8_t * pJavaBuffer, uint32_t nOffset, uint32_t nLength)
{
   W_HANDLE ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (W_HANDLE)0;
   }

   PContextLock(pContext);
   ret = PJavaCreateByteBuffer( pContext, pJavaBuffer, nOffset, nLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint32_t WJavaGetByteBufferLength(W_HANDLE hBufferReference)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PJavaGetByteBufferLength( pContext, hBufferReference );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint32_t WJavaGetByteBufferOffset(W_HANDLE hBufferReference)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PJavaGetByteBufferOffset( pContext, hBufferReference );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint8_t * WJavaGetByteBufferPointer(W_HANDLE hBufferReference)
{
   uint8_t * ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint8_t *)0;
   }

   PContextLock(pContext);
   ret = PJavaGetByteBufferPointer( pContext, hBufferReference );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint32_t WJavaNDEFGetMessageContent(W_HANDLE hMessage, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PJavaNDEFGetMessageContent( pContext, hMessage, pMessageBuffer, nMessageBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
void WJavaNDEFWriteMessage(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength, uint32_t nActionMask, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJavaNDEFWriteMessage( pContext, hConnection, null, &dfc, pMessageBuffer, nMessageBufferLength, nActionMask, phOperation );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
void WJavaNDEFWriteMessageOnAnyTag(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nPriority, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength, uint32_t nActionMask, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJavaNDEFWriteMessageOnAnyTag( pContext, null, &dfc, nPriority, pMessageBuffer, nMessageBufferLength, nActionMask, phOperation );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
bool WJavaNFCControllerGetBooleanProperty(uint8_t nPropertyIdentifier)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PJavaNFCControllerGetBooleanProperty( pContext, nPropertyIdentifier );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_ERROR WJavaP2PGetConfigurationBuffer(uint8_t * pConfigurationBuffer, uint32_t nConfigurationBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJavaP2PGetConfigurationBuffer( pContext, pConfigurationBuffer, nConfigurationBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_ERROR WJavaP2PGetLinkPropertiesBuffer(W_HANDLE hLink, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJavaP2PGetLinkPropertiesBuffer( pContext, hLink, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_ERROR WJavaP2PSetConfigurationBuffer(uint8_t * pConfigurationBuffer, uint32_t nConfigurationBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJavaP2PSetConfigurationBuffer( pContext, pConfigurationBuffer, nConfigurationBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
void WJavaReleaseByteBuffer(W_HANDLE hBufferReference, uint8_t * pJavaBuffer, uint32_t nJavaBufferLength)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJavaReleaseByteBuffer( pContext, hBufferReference, pJavaBuffer, nJavaBufferLength );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_J_EDITION_HELPER
void WJed14Part3ExchangeData(W_HANDLE hConnection, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOpeartion, bool bCheckResponseCRC)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJed14Part3ExchangeData( pContext, hConnection, null, &dfc, pReaderToCardBuffer, nReaderToCardBufferLength, pCardToReaderBuffer, nCardToReaderBufferMaxLength, phOpeartion, bCheckResponseCRC );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedAddHandle(W_HANDLE hHandle, uint32_t nIsolate)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedAddHandle( pContext, hHandle, nIsolate );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedCheckHandle(W_HANDLE hHandle, uint32_t * pnIsolate)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedCheckHandle( pContext, hHandle, pnIsolate );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void WJedCloseHandle(W_HANDLE hHandle)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJedCloseHandle( pContext, hHandle );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_HANDLE WJedCreateBufferObject(uint32_t nIsolate, uint32_t nParameter, uint32_t nLength, uint8_t ** ppContent)
{
   W_HANDLE ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (W_HANDLE)0;
   }

   PContextLock(pContext);
   ret = PJedCreateBufferObject( pContext, nIsolate, nParameter, nLength, ppContent );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedGetBufferContent(W_HANDLE hBufferObject, uint32_t * pnIsolate, uint32_t * pnParameter, uint8_t ** ppContent, uint32_t * pnLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedGetBufferContent( pContext, hBufferObject, pnIsolate, pnParameter, ppContent, pnLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedGetInfo(W_HANDLE hHandle, uint32_t nInfoType, uint32_t * pnValue)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedGetInfo( pContext, hHandle, nInfoType, pnValue );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedGetInfoBuffer(W_HANDLE hHandle, uint32_t nInfoType, uint8_t * pBuffer, uint32_t nBufferLength, uint32_t * pnActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedGetInfoBuffer( pContext, hHandle, nInfoType, pBuffer, nBufferLength, pnActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedGetNFCCProperty(const tchar * pPropertyName, tchar * pPropertyValueBuffer, uint32_t nPropertyValueBufferLength, uint32_t * pnPropertyValueLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedGetNFCCProperty( pContext, pPropertyName, pPropertyValueBuffer, nPropertyValueBufferLength, pnPropertyValueLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedGetP2PPushSocket(const tchar * pPushEntry, W_HANDLE * phSocket)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedGetP2PPushSocket( pContext, pPushEntry, phSocket );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedInitialize(uint32_t nKeepAliveConnectionTimeout, uint32_t nPushRegistryTimeout, uint8_t nReaderPriority, tWJedSendEvent * pSendEventFunction, void * pCallbackParameter)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pSendEventFunction;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedInitialize( pContext, nKeepAliveConnectionTimeout, nPushRegistryTimeout, nReaderPriority, null, &dfc );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_HANDLE WJedListenToCardDetection(uint32_t nIsolate, uint32_t nListenerId, const uint8_t * pProperties, uint32_t nPropertyNumber)
{
   W_HANDLE ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (W_HANDLE)0;
   }

   PContextLock(pContext);
   ret = PJedListenToCardDetection( pContext, nIsolate, nListenerId, pProperties, nPropertyNumber );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedMonitorEndOfTransaction(uint32_t nIsolate, bool bRegister, uint32_t nSlotId)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedMonitorEndOfTransaction( pContext, nIsolate, bRegister, nSlotId );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedPushRegistryAddEntry(tWJedPushRegistryEventHandler * pEventHandler, void * pHandlerParameter, uint32_t nApplicationIdentifier, const uint8_t * pURLString)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pEventHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedPushRegistryAddEntry( pContext, null, &dfc, nApplicationIdentifier, pURLString );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedPushRegistryCheckEntry(const uint8_t * pURLString)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedPushRegistryCheckEntry( pContext, pURLString );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedPushRegistryRemoveEntry(uint32_t nApplicationIdentifier, const uint8_t * pURLString)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedPushRegistryRemoveEntry( pContext, nApplicationIdentifier, pURLString );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void WJedReaderHandlerWorkPerformed(W_HANDLE hConnection, bool bGiveToNextListener, bool bCardApplicationMatch)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJedReaderHandlerWorkPerformed( pContext, hConnection, bGiveToNextListener, bCardApplicationMatch );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedReadOnAnyTag(uint32_t nIsolate, bool bRegister, uint32_t nListenerId, uint8_t nTNF, const tchar * pTypeString)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedReadOnAnyTag( pContext, nIsolate, bRegister, nListenerId, nTNF, pTypeString );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void WJedReleaseApplication(uint32_t nIsolate)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJedReleaseApplication( pContext, nIsolate );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR WJedRetrieveConnection(W_HANDLE hConnection)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PJedRetrieveConnection( pContext, hConnection );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void WJedTerminate(void)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJedTerminate( pContext );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_JUPITER_SE
void WJupiterSEApplyPolicy(uint32_t nSlotIdentifier, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJupiterSEApplyPolicy( pContext, nSlotIdentifier, null, &dfc );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_JUPITER_SE */


#ifdef P_INCLUDE_JUPITER_SE
void WJupiterSEGetAID(uint32_t nSlotIdentifier, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nBufferLength)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PJupiterSEGetAID( pContext, nSlotIdentifier, null, &dfc, pBuffer, nBufferLength );
   PContextReleaseLock(pContext);
}

#endif /* #ifdef P_INCLUDE_JUPITER_SE */

W_ERROR WMifareGetConnectionInfo(W_HANDLE hConnection, tWMifareConnectionInfo * pConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PMifareGetConnectionInfo( pContext, hConnection, pConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR WMifareGetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PMifareGetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

void WMifareRead(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMifareRead( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, phOperation );
   PContextReleaseLock(pContext);
}

void WMifareULCAuthenticate(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pKey, uint32_t nKeyLength)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMifareULCAuthenticate( pContext, hConnection, null, &dfc, pKey, nKeyLength );
   PContextReleaseLock(pContext);
}

void WMifareULCSetAccessRights(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pKey, uint32_t nKeyLength, uint8_t nThreshold, uint32_t nRights, bool bLockConfiguration)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMifareULCSetAccessRights( pContext, hConnection, null, &dfc, pKey, nKeyLength, nThreshold, nRights, bLockConfiguration );
   PContextReleaseLock(pContext);
}

W_ERROR WMifareULForceULC(W_HANDLE hConnection)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PMifareULForceULC( pContext, hConnection );
   PContextReleaseLock(pContext);
   return ret;
}

void WMifareULFreezeDataLockConfiguration(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMifareULFreezeDataLockConfiguration( pContext, hConnection, null, &dfc );
   PContextReleaseLock(pContext);
}

W_ERROR WMifareULGetAccessRights(W_HANDLE hConnection, uint32_t nOffset, uint32_t nLength, uint32_t * pnRights)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PMifareULGetAccessRights( pContext, hConnection, nOffset, nLength, pnRights );
   PContextReleaseLock(pContext);
   return ret;
}

void WMifareULRetrieveAccessRights(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMifareULRetrieveAccessRights( pContext, hConnection, null, &dfc );
   PContextReleaseLock(pContext);
}

void WMifareWrite(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockSectors, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMifareWrite( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, bLockSectors, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WMyDGetConnectionInfo(W_HANDLE hConnection, tWMyDConnectionInfo * pConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PMyDGetConnectionInfo( pContext, hConnection, pConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR WMyDGetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PMyDGetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

void WMyDMoveAuthenticate(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint32_t nPassword)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMyDMoveAuthenticate( pContext, hConnection, null, &dfc, nPassword );
   PContextReleaseLock(pContext);
}

void WMyDMoveFreezeDataLockConfiguration(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMyDMoveFreezeDataLockConfiguration( pContext, hConnection, null, &dfc );
   PContextReleaseLock(pContext);
}

void WMyDMoveGetConfiguration(W_HANDLE hConnection, tWMyDMoveGetConfigurationCompleted * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMyDMoveGetConfiguration( pContext, hConnection, null, &dfc );
   PContextReleaseLock(pContext);
}

void WMyDMoveSetConfiguration(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nStatusByte, uint8_t nPasswordRetryCounter, uint32_t nPassword, bool bLockConfiguration)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMyDMoveSetConfiguration( pContext, hConnection, null, &dfc, nStatusByte, nPasswordRetryCounter, nPassword, bLockConfiguration );
   PContextReleaseLock(pContext);
}

void WMyDRead(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMyDRead( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, phOperation );
   PContextReleaseLock(pContext);
}

void WMyDWrite(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockSectors, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PMyDWrite( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, bLockSectors, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WNDEFAppendRecord(W_HANDLE hMessage, W_HANDLE hRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFAppendRecord( pContext, hMessage, hRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFBuildMessage(const uint8_t * pBuffer, uint32_t nBufferLength, W_HANDLE * phMessage)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFBuildMessage( pContext, pBuffer, nBufferLength, phMessage );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFBuildRecord(const uint8_t * pBuffer, uint32_t nBufferLength, W_HANDLE * phRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFBuildRecord( pContext, pBuffer, nBufferLength, phRecord );
   PContextReleaseLock(pContext);
   return ret;
}

bool WNDEFCheckIdentifier(W_HANDLE hRecord, const tchar * pIdentifierString)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PNDEFCheckIdentifier( pContext, hRecord, pIdentifierString );
   PContextReleaseLock(pContext);
   return ret;
}

bool WNDEFCheckType(W_HANDLE hRecord, uint8_t nTNF, const tchar * pTypeString)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PNDEFCheckType( pContext, hRecord, nTNF, pTypeString );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFCreateNestedMessageRecord(uint8_t nTNF, const tchar * pTypeString, W_HANDLE hNestedMessage, W_HANDLE * phRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFCreateNestedMessageRecord( pContext, nTNF, pTypeString, hNestedMessage, phRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFCreateNewMessage(W_HANDLE * phMessage)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFCreateNewMessage( pContext, phMessage );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFCreateRecord(uint8_t nTNF, const tchar * pTypeString, const uint8_t * pPayloadBuffer, uint32_t nPayloadLength, W_HANDLE * phRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFCreateRecord( pContext, nTNF, pTypeString, pPayloadBuffer, nPayloadLength, phRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFGetEnclosedMessage(W_HANDLE hRecord, W_HANDLE * phMessage)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetEnclosedMessage( pContext, hRecord, phMessage );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFGetIdentifierString(W_HANDLE hRecord, tchar * pIdentifierStringBuffer, uint32_t nIdentifierStringBufferLength, uint32_t * pnActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetIdentifierString( pContext, hRecord, pIdentifierStringBuffer, nIdentifierStringBufferLength, pnActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFGetMessageContent(W_HANDLE hMessage, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength, uint32_t * pnActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetMessageContent( pContext, hMessage, pMessageBuffer, nMessageBufferLength, pnActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WNDEFGetMessageLength(W_HANDLE hMessage)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PNDEFGetMessageLength( pContext, hMessage );
   PContextReleaseLock(pContext);
   return ret;
}

W_HANDLE WNDEFGetNextMessage(W_HANDLE hMessage)
{
   W_HANDLE ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (W_HANDLE)0;
   }

   PContextLock(pContext);
   ret = PNDEFGetNextMessage( pContext, hMessage );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFGetPayloadPointer(W_HANDLE hRecord, uint8_t ** ppBuffer)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetPayloadPointer( pContext, hRecord, ppBuffer );
   PContextReleaseLock(pContext);
   return ret;
}

W_HANDLE WNDEFGetRecord(W_HANDLE hMessage, uint32_t nIndex)
{
   W_HANDLE ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (W_HANDLE)0;
   }

   PContextLock(pContext);
   ret = PNDEFGetRecord( pContext, hMessage, nIndex );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFGetRecordInfo(W_HANDLE hRecord, uint32_t nInfoType, uint32_t * pnValue)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetRecordInfo( pContext, hRecord, nInfoType, pnValue );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFGetRecordInfoBuffer(W_HANDLE hRecord, uint32_t nInfoType, uint8_t * pBuffer, uint32_t nBufferLength, uint32_t * pnActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetRecordInfoBuffer( pContext, hRecord, nInfoType, pBuffer, nBufferLength, pnActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WNDEFGetRecordNumber(W_HANDLE hMessage)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PNDEFGetRecordNumber( pContext, hMessage );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFGetTagInfo(W_HANDLE hConnection, tNDEFTagInfo * pTagInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetTagInfo( pContext, hConnection, pTagInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR WNDEFGetTagInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetTagInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR WNDEFGetTypeString(W_HANDLE hRecord, tchar * pTypeStringBuffer, uint32_t nTypeStringBufferLength, uint32_t * pnActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFGetTypeString( pContext, hRecord, pTypeStringBuffer, nTypeStringBufferLength, pnActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFInsertRecord(W_HANDLE hMessage, uint32_t nIndex, W_HANDLE hRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFInsertRecord( pContext, hMessage, nIndex, hRecord );
   PContextReleaseLock(pContext);
   return ret;
}

void WNDEFReadMessage(W_HANDLE hConnection, tWNDEFReadMessageCompleted * pCallback, void * pCallbackParameter, uint8_t nTNF, const tchar * pTypeString, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNDEFReadMessage( pContext, hConnection, null, &dfc, nTNF, pTypeString, phOperation );
   PContextReleaseLock(pContext);
}

void WNDEFReadMessageOnAnyTag(tWNDEFReadMessageCompleted * pCallback, void * pCallbackParameter, uint8_t nPriority, uint8_t nTNF, const tchar * pTypeString, W_HANDLE * phRegistry)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNDEFReadMessageOnAnyTag( pContext, null, &dfc, nPriority, nTNF, pTypeString, phRegistry );
   PContextReleaseLock(pContext);
}

W_ERROR WNDEFRemoveRecord(W_HANDLE hMessage, uint32_t nIndex)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFRemoveRecord( pContext, hMessage, nIndex );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFSetIdentifierString(W_HANDLE hRecord, const tchar * pIdentifierString)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFSetIdentifierString( pContext, hRecord, pIdentifierString );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFSetRecord(W_HANDLE hMessage, uint32_t nIndex, W_HANDLE hRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFSetRecord( pContext, hMessage, nIndex, hRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNDEFSetRecordInfo(W_HANDLE hRecord, uint32_t nInfoType, const uint8_t * pBuffer, uint32_t nBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNDEFSetRecordInfo( pContext, hRecord, nInfoType, pBuffer, nBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

void WNDEFWriteMessage(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, W_HANDLE hMessage, uint32_t nActionMask, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNDEFWriteMessage( pContext, hConnection, null, &dfc, hMessage, nActionMask, phOperation );
   PContextReleaseLock(pContext);
}

void WNDEFWriteMessageOnAnyTag(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nPriority, W_HANDLE hMessage, uint32_t nActionMask, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNDEFWriteMessageOnAnyTag( pContext, null, &dfc, nPriority, hMessage, nActionMask, phOperation );
   PContextReleaseLock(pContext);
}

void WNFCControllerFirmwareUpdate(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pUpdateBuffer, uint32_t nUpdateBufferLength, uint32_t nMode)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerFirmwareUpdate( pContext, null, &dfc, pUpdateBuffer, nUpdateBufferLength, nMode );
   PContextReleaseLock(pContext);
}

uint32_t WNFCControllerFirmwareUpdateState(void)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PNFCControllerFirmwareUpdateState( pContext );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNFCControllerGetBooleanProperty(uint8_t nPropertyIdentifier, bool * pbValue)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNFCControllerGetBooleanProperty( pContext, nPropertyIdentifier, pbValue );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNFCControllerGetIntegerProperty(uint8_t nPropertyIdentifier, uint32_t * pnValue)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNFCControllerGetIntegerProperty( pContext, nPropertyIdentifier, pnValue );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WNFCControllerGetMode(void)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PNFCControllerGetMode( pContext );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNFCControllerGetProperty(uint8_t nPropertyIdentifier, tchar * pValueBuffer, uint32_t nBufferLength, uint32_t * pnValueLength)
{
   tContext* pContext;

   pContext = g_pContext;
   return PNFCControllerGetProperty( pContext, nPropertyIdentifier, pValueBuffer, nBufferLength, pnValueLength );
}

W_ERROR WNFCControllerGetRawMessageData(uint8_t * pBuffer, uint32_t nBufferLength, uint32_t * pnActualLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNFCControllerGetRawMessageData( pContext, pBuffer, nBufferLength, pnActualLength );
   PContextReleaseLock(pContext);
   return ret;
}

void WNFCControllerGetRFActivity(uint8_t * pnReaderState, uint8_t * pnCardState, uint8_t * pnP2PState)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerGetRFActivity( pContext, pnReaderState, pnCardState, pnP2PState );
   PContextReleaseLock(pContext);
}

void WNFCControllerGetRFLock(uint32_t nLockSet, bool * pbReaderLock, bool * pbCardLock)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerGetRFLock( pContext, nLockSet, pbReaderLock, pbCardLock );
   PContextReleaseLock(pContext);
}

W_ERROR WNFCControllerMonitorException(tWBasicGenericEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNFCControllerMonitorException( pContext, (tPBasicGenericEventHandler *)pHandler, pHandlerParameter, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WNFCControllerMonitorFieldEvents(tWBasicGenericEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNFCControllerMonitorFieldEvents( pContext, (tPBasicGenericEventHandler *)pHandler, pHandlerParameter, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

void WNFCControllerProductionTest(const uint8_t * pParameterBuffer, uint32_t nParameterBufferLength, uint8_t * pResultBuffer, uint32_t nResultBufferLength, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerProductionTest( pContext, pParameterBuffer, nParameterBufferLength, pResultBuffer, nResultBufferLength, null, &dfc );
   PContextReleaseLock(pContext);
}

W_ERROR WNFCControllerRegisterRawListener(tWBasicGenericDataCallbackFunction * pReceiveMessageEventHandler, void * pHandlerParameter)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNFCControllerRegisterRawListener( pContext, (tPBasicGenericDataCallbackFunction *)pReceiveMessageEventHandler, pHandlerParameter );
   PContextReleaseLock(pContext);
   return ret;
}

void WNFCControllerReset(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint32_t nMode)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerReset( pContext, null, &dfc, nMode );
   PContextReleaseLock(pContext);
}

void WNFCControllerSelfTest(tWNFCControllerSelfTestCompleted * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerSelfTest( pContext, null, &dfc );
   PContextReleaseLock(pContext);
}

void WNFCControllerSetRFLock(uint32_t nLockSet, bool bReaderLock, bool bCardLock, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerSetRFLock( pContext, nLockSet, bReaderLock, bCardLock, null, &dfc );
   PContextReleaseLock(pContext);
}

W_ERROR WNFCControllerSwitchStandbyMode(bool bStandbyOn)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PNFCControllerSwitchStandbyMode( pContext, bStandbyOn );
   PContextReleaseLock(pContext);
   return ret;
}

void WNFCControllerSwitchToRawMode(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerSwitchToRawMode( pContext, null, &dfc );
   PContextReleaseLock(pContext);
}

void WNFCControllerWriteRawMessage(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nLength)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PNFCControllerWriteRawMessage( pContext, null, &dfc, pBuffer, nLength );
   PContextReleaseLock(pContext);
}

void WP2PConnect(W_HANDLE hSocket, W_HANDLE hLink, tWBasicGenericCallbackFunction * pEstablishmentCallback, void * pEstablishmentCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pEstablishmentCallback;
   dfc.pParameter = pEstablishmentCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PConnect( pContext, hSocket, hLink, null, &dfc );
   PContextReleaseLock(pContext);
}

W_ERROR WP2PCreateSocket(uint8_t nType, const tchar * pServiceURI, uint8_t nSAP, W_HANDLE * phSocket)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PP2PCreateSocket( pContext, nType, pServiceURI, nSAP, phSocket );
   PContextReleaseLock(pContext);
   return ret;
}

W_HANDLE WP2PEstablishLinkDriver1(tWBasicGenericHandleCallbackFunction * pEstablishmentCallback, void * pEstablishmentCallbackParameter)
{
   W_HANDLE ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (W_HANDLE)0;
   }

   PContextLock(pContext);
   ret = PP2PEstablishLinkDriver1( pContext, (tPBasicGenericHandleCallbackFunction *)pEstablishmentCallback, pEstablishmentCallbackParameter );
   PContextReleaseLock(pContext);
   return ret;
}

void WP2PEstablishLinkDriver2(W_HANDLE hLink, tWBasicGenericCallbackFunction * pReleaseCallback, void * pReleaseCallbackParameter, W_HANDLE * phOperation)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PEstablishLinkDriver2( pContext, hLink, (tPBasicGenericCallbackFunction *)pReleaseCallback, pReleaseCallbackParameter, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WP2PGetConfiguration(tWP2PConfiguration * pConfiguration)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PP2PGetConfiguration( pContext, pConfiguration );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WP2PGetLinkProperties(W_HANDLE hLink, tWP2PLinkProperties * pProperties)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PP2PGetLinkProperties( pContext, hLink, pProperties );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WP2PGetSocketParameter(W_HANDLE hSocket, uint32_t nParameter, uint32_t * pnValue)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PP2PGetSocketParameter( pContext, hSocket, nParameter, pnValue );
   PContextReleaseLock(pContext);
   return ret;
}

void WP2PRead(W_HANDLE hSocket, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pReceptionBuffer, uint32_t nReceptionBufferLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PRead( pContext, hSocket, null, &dfc, pReceptionBuffer, nReceptionBufferLength, phOperation );
   PContextReleaseLock(pContext);
}

void WP2PRecvFrom(W_HANDLE hSocket, tWP2PRecvFromCompleted * pCallback, void * pCallbackParameter, uint8_t * pReceptionBuffer, uint32_t nReceptionBufferLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PRecvFrom( pContext, hSocket, null, &dfc, pReceptionBuffer, nReceptionBufferLength, phOperation );
   PContextReleaseLock(pContext);
}

void WP2PSendTo(W_HANDLE hSocket, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nSAP, const uint8_t * pSendBuffer, uint32_t nSendBufferLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PSendTo( pContext, hSocket, null, &dfc, nSAP, pSendBuffer, nSendBufferLength, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WP2PSetConfiguration(const tWP2PConfiguration * pConfiguration)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PP2PSetConfiguration( pContext, pConfiguration );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WP2PSetSocketParameter(W_HANDLE hSocket, uint32_t nParameter, uint32_t nValue)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PP2PSetSocketParameter( pContext, hSocket, nParameter, nValue );
   PContextReleaseLock(pContext);
   return ret;
}

void WP2PShutdown(W_HANDLE hSocket, tWBasicGenericCallbackFunction * pReleaseCallback, void * pReleaseCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pReleaseCallback;
   dfc.pParameter = pReleaseCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PShutdown( pContext, hSocket, null, &dfc );
   PContextReleaseLock(pContext);
}

void WP2PURILookup(W_HANDLE hLink, tWP2PURILookupCompleted * pCallback, void * pCallbackParameter, const tchar * pServiceURI)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PURILookup( pContext, hLink, null, &dfc, pServiceURI );
   PContextReleaseLock(pContext);
}

void WP2PWrite(W_HANDLE hSocket, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pSendBuffer, uint32_t nSendBufferLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PP2PWrite( pContext, hSocket, null, &dfc, pSendBuffer, nSendBufferLength, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WPicoGetConnectionInfo(W_HANDLE hConnection, tWPicoConnectionInfo * pConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PPicoGetConnectionInfo( pContext, hConnection, pConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR WPicoGetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PPicoGetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR WPicoIsWritable(W_HANDLE hConnection)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PPicoIsWritable( pContext, hConnection );
   PContextReleaseLock(pContext);
   return ret;
}

void WPicoRead(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PPicoRead( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, phOperation );
   PContextReleaseLock(pContext);
}

void WPicoWrite(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockCard, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PPicoWrite( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, bLockCard, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WReaderErrorEventRegister(tWBasicGenericEventHandler * pHandler, void * pHandlerParameter, uint8_t nEventType, bool bCardDetectionRequested, W_HANDLE * phRegistryHandle)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PReaderErrorEventRegister( pContext, (tPBasicGenericEventHandler *)pHandler, pHandlerParameter, nEventType, bCardDetectionRequested, phRegistryHandle );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WReaderGetPulsePeriod(uint32_t * pnTimeout)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PReaderGetPulsePeriod( pContext, pnTimeout );
   PContextReleaseLock(pContext);
   return ret;
}

void WReaderHandlerWorkPerformed(W_HANDLE hConnection, bool bGiveToNextListener, bool bCardApplicationMatch)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PReaderHandlerWorkPerformed( pContext, hConnection, bGiveToNextListener, bCardApplicationMatch );
   PContextReleaseLock(pContext);
}

bool WReaderIsPropertySupported(uint8_t nPropertyIdentifier)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PReaderIsPropertySupported( pContext, nPropertyIdentifier );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WReaderListenToCardDetection(tWReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, const uint8_t * pConnectionPropertyArray, uint32_t nPropertyNumber, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pHandler;
   dfc.pParameter = pHandlerParameter;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PReaderListenToCardDetection( pContext, null, &dfc, nPriority, pConnectionPropertyArray, nPropertyNumber, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

bool WReaderPreviousApplicationMatch(W_HANDLE hConnection)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PReaderPreviousApplicationMatch( pContext, hConnection );
   PContextReleaseLock(pContext);
   return ret;
}

void WReaderSetPulsePeriod(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint32_t nPulsePeriod)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PReaderSetPulsePeriod( pContext, null, &dfc, nPulsePeriod );
   PContextReleaseLock(pContext);
}

bool WRTDIsTextRecord(W_HANDLE hRecord)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PRTDIsTextRecord( pContext, hRecord );
   PContextReleaseLock(pContext);
   return ret;
}

bool WRTDIsURIRecord(W_HANDLE hRecord)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PRTDIsURIRecord( pContext, hRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDTextAddRecord(W_HANDLE hMessage, const tchar * pLanguage, bool bUseUtf8, const tchar * pText, uint32_t nTextLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDTextAddRecord( pContext, hMessage, pLanguage, bUseUtf8, pText, nTextLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDTextCreateRecord(const tchar * pLanguage, bool bUseUtf8, const tchar * pText, uint32_t nTextLength, W_HANDLE * phRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDTextCreateRecord( pContext, pLanguage, bUseUtf8, pText, nTextLength, phRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDTextFind(W_HANDLE hMessage, const tchar * pLanguage1, const tchar * pLanguage2, W_HANDLE * phRecord, uint8_t * pnMatch)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDTextFind( pContext, hMessage, pLanguage1, pLanguage2, phRecord, pnMatch );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDTextGetLanguage(W_HANDLE hRecord, tchar * pLanguageBuffer, uint32_t nBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDTextGetLanguage( pContext, hRecord, pLanguageBuffer, nBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WRTDTextGetLength(W_HANDLE hRecord)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PRTDTextGetLength( pContext, hRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDTextGetValue(W_HANDLE hRecord, tchar * pBuffer, uint32_t nBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDTextGetValue( pContext, hRecord, pBuffer, nBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

uint8_t WRTDTextLanguageMatch(W_HANDLE hRecord, const tchar * pLanguage1, const tchar * pLanguage2)
{
   uint8_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint8_t)0;
   }

   PContextLock(pContext);
   ret = PRTDTextLanguageMatch( pContext, hRecord, pLanguage1, pLanguage2 );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDURIAddRecord(W_HANDLE hMessage, const tchar * pURI)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDURIAddRecord( pContext, hMessage, pURI );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDURICreateRecord(const tchar * pURI, W_HANDLE * phRecord)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDURICreateRecord( pContext, pURI, phRecord );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WRTDURIGetLength(W_HANDLE hRecord)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PRTDURIGetLength( pContext, hRecord );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WRTDURIGetValue(W_HANDLE hRecord, tchar * pBuffer, uint32_t nBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PRTDURIGetValue( pContext, hRecord, pBuffer, nBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WSecurityAuthenticate(const uint8_t * pApplicationDataBuffer, uint32_t nApplicationDataBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PSecurityAuthenticate( pContext, pApplicationDataBuffer, nApplicationDataBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WSEGetInfo(uint32_t nSlotIdentifier, tSEInfo * pSEInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PSEGetInfo( pContext, nSlotIdentifier, pSEInfo );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WSEGetTransactionAID(uint32_t nSlotIdentifier, uint8_t * pBuffer, uint32_t nBufferLength)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PSEGetTransactionAID( pContext, nSlotIdentifier, pBuffer, nBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WSEMonitorEndOfTransaction(uint32_t nSlotIdentifier, tWBasicGenericEventHandler2 * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PSEMonitorEndOfTransaction( pContext, nSlotIdentifier, (tPBasicGenericEventHandler2 *)pHandler, pHandlerParameter, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WSEMonitorHotPlugEvents(uint32_t nSlotIdentifier, tWBasicGenericEventHandler2 * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PSEMonitorHotPlugEvents( pContext, nSlotIdentifier, (tPBasicGenericEventHandler2 *)pHandler, pHandlerParameter, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

void WSEOpenConnection(uint32_t nSlotIdentifier, bool bForce, tWBasicGenericHandleCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PSEOpenConnection( pContext, nSlotIdentifier, bForce, null, &dfc );
   PContextReleaseLock(pContext);
}

void WSESetPolicy(uint32_t nSlotIdentifier, uint32_t nStorageType, uint32_t nProtocols, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PSESetPolicy( pContext, nSlotIdentifier, nStorageType, nProtocols, null, &dfc );
   PContextReleaseLock(pContext);
}

void * WTestAlloc(uint32_t nSize)
{
   void * ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (void *)0;
   }

   PContextLock(pContext);
   ret = PTestAlloc( pContext, nSize );
   PContextReleaseLock(pContext);
   return ret;
}

void WTestExecuteRemoteFunction(const char * pFunctionIdentifier, uint32_t nParameter, const uint8_t * pParameterBuffer, uint32_t nParameterBufferLength, uint8_t * pResultBuffer, uint32_t nResultBufferLength, tWBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestExecuteRemoteFunction( pContext, pFunctionIdentifier, nParameter, pParameterBuffer, nParameterBufferLength, pResultBuffer, nResultBufferLength, null, &dfc );
   PContextReleaseLock(pContext);
}

void WTestFree(void * pBuffer)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestFree( pContext, pBuffer );
   PContextReleaseLock(pContext);
}

const void * WTestGetConstAddress(const void * pConstData)
{
   const void * ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (const void *)0;
   }

   PContextLock(pContext);
   ret = PTestGetConstAddress( pContext, pConstData );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WTestGetCurrentTime(void)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PTestGetCurrentTime( pContext );
   PContextReleaseLock(pContext);
   return ret;
}

tTestExecuteContext * WTestGetExecuteContext(void)
{
   tTestExecuteContext * ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (tTestExecuteContext *)0;
   }

   PContextLock(pContext);
   ret = PTestGetExecuteContext( pContext );
   PContextReleaseLock(pContext);
   return ret;
}

bool WTestIsInAutomaticMode(void)
{
   bool ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (bool)0;
   }

   PContextLock(pContext);
   ret = PTestIsInAutomaticMode( pContext );
   PContextReleaseLock(pContext);
   return ret;
}

void WTestMessageBox(uint32_t nFlags, const char * pMessage, uint32_t nAutomaticResult, tWTestMessageBoxCompleted * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestMessageBox( pContext, nFlags, pMessage, nAutomaticResult, null, &dfc );
   PContextReleaseLock(pContext);
}

void WTestNotifyEnd(void)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestNotifyEnd( pContext );
   PContextReleaseLock(pContext);
}

void WTestPresentObject(const char * pObjectName, const char * pOperatorMessage, uint32_t nDistance, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestPresentObject( pContext, pObjectName, pOperatorMessage, nDistance, null, &dfc );
   PContextReleaseLock(pContext);
}

void WTestRemoveObject(const char * pOperatorMessage, bool bSaveState, bool bCheckUnmodifiedState, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestRemoveObject( pContext, pOperatorMessage, bSaveState, bCheckUnmodifiedState, null, &dfc );
   PContextReleaseLock(pContext);
}

void WTestSetResult(uint32_t nResult, const void * pResultData, uint32_t nResultDataLength)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestSetResult( pContext, nResult, pResultData, nResultDataLength );
   PContextReleaseLock(pContext);
}

void WTestSetTimer(uint32_t nTimeout, tWBasicGenericCompletionFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestSetTimer( pContext, nTimeout, null, &dfc );
   PContextReleaseLock(pContext);
}

void WTestTraceBuffer(const uint8_t * pBuffer, uint32_t nLength)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PTestTraceBuffer( pContext, pBuffer, nLength );
   PContextReleaseLock(pContext);
}

void WTestTraceError(const char * pMessage, ...)
{
#ifdef P_TRACE_ACTIVE
   va_list args;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   va_start(args, pMessage);

   PContextLock(pContext);
   PTestTraceError( pContext, pMessage, args );
   PContextReleaseLock(pContext);
   va_end(args);
#endif /* P_TRACE_ACTIVE */
}

void WTestTraceInfo(const char * pMessage, ...)
{
#ifdef P_TRACE_ACTIVE
   va_list args;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   va_start(args, pMessage);

   PContextLock(pContext);
   PTestTraceInfo( pContext, pMessage, args );
   PContextReleaseLock(pContext);
   va_end(args);
#endif /* P_TRACE_ACTIVE */
}

void WTestTraceWarning(const char * pMessage, ...)
{
#ifdef P_TRACE_ACTIVE
   va_list args;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   va_start(args, pMessage);

   PContextLock(pContext);
   PTestTraceWarning( pContext, pMessage, args );
   PContextReleaseLock(pContext);
   va_end(args);
#endif /* P_TRACE_ACTIVE */
}

W_ERROR WType1ChipGetConnectionInfo(W_HANDLE hConnection, tWType1ChipConnectionInfo * pConnectionInfo)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PType1ChipGetConnectionInfo( pContext, hConnection, pConnectionInfo );
   PContextReleaseLock(pContext);
   return ret;
}


#ifdef P_INCLUDE_JAVA_API
W_ERROR WType1ChipGetConnectionInfoBuffer(W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PType1ChipGetConnectionInfoBuffer( pContext, hConnection, pInfoBuffer, nInfoBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR WType1ChipIsWritable(W_HANDLE hConnection)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PType1ChipIsWritable( pContext, hConnection );
   PContextReleaseLock(pContext);
   return ret;
}

void WType1ChipRead(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PType1ChipRead( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, phOperation );
   PContextReleaseLock(pContext);
}

void WType1ChipWrite(W_HANDLE hConnection, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockBlocks, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PType1ChipWrite( pContext, hConnection, null, &dfc, pBuffer, nOffset, nLength, bLockBlocks, phOperation );
   PContextReleaseLock(pContext);
}

void WUICCActivateSWPLine(tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PUICCActivateSWPLine( pContext, null, &dfc );
   PContextReleaseLock(pContext);
}

void WUICCGetAccessPolicy(uint32_t nStorageType, tWUICCAccessPolicy * pAccessPolicy)
{
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PUICCGetAccessPolicy( pContext, nStorageType, pAccessPolicy );
   PContextReleaseLock(pContext);
}

W_ERROR WUICCGetConnectivityEventParameter(uint8_t * pDataBuffer, uint32_t nBufferLength, uint32_t * pnActualDataLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PUICCGetConnectivityEventParameter( pContext, pDataBuffer, nBufferLength, pnActualDataLength );
   PContextReleaseLock(pContext);
   return ret;
}

void WUICCGetSlotInfo(tWUICCGetSlotInfoCompleted * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PUICCGetSlotInfo( pContext, null, &dfc );
   PContextReleaseLock(pContext);
}

uint32_t WUICCGetTransactionEventAID(uint8_t * pBuffer, uint32_t nBufferLength)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PUICCGetTransactionEventAID( pContext, pBuffer, nBufferLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WUICCMonitorConnectivityEvent(tWUICCMonitorConnectivityEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PUICCMonitorConnectivityEvent( pContext, (tPUICCMonitorConnectivityEventHandler *)pHandler, pHandlerParameter, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WUICCMonitorTransactionEvent(tWUICCMonitorTransactionEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PUICCMonitorTransactionEvent( pContext, (tPUICCMonitorTransactionEventHandler *)pHandler, pHandlerParameter, phEventRegistry );
   PContextReleaseLock(pContext);
   return ret;
}

void WUICCSetAccessPolicy(uint32_t nStorageType, const tWUICCAccessPolicy * pAccessPolicy, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PUICCSetAccessPolicy( pContext, nStorageType, pAccessPolicy, null, &dfc );
   PContextReleaseLock(pContext);
}

W_ERROR WVirtualTagCreate(uint8_t nTagType, const uint8_t * pIdentifier, uint32_t nIdentifierLength, uint32_t nMaximumMessageLength, W_HANDLE * phVirtualTag)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PVirtualTagCreate( pContext, nTagType, pIdentifier, nIdentifierLength, nMaximumMessageLength, phVirtualTag );
   PContextReleaseLock(pContext);
   return ret;
}

void WVirtualTagStop(W_HANDLE hVirtualTag, tWBasicGenericCallbackFunction * pCompletionCallback, void * pCallbackParameter)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCompletionCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PVirtualTagStop( pContext, hVirtualTag, null, &dfc );
   PContextReleaseLock(pContext);
}

void WWIFIAccessPointConfigureToken(uint8_t nVersion, const uint8_t * pCredentialBuffer, uint32_t nCredentialLength, tWBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PWIFIAccessPointConfigureToken( pContext, nVersion, pCredentialBuffer, nCredentialLength, null, &dfc, phOperation );
   PContextReleaseLock(pContext);
}

void WWIFIEnrolleePairingStart(const uint8_t * pMessageM1, uint32_t nMessageM1Length, tWWIFIEnrolleePairingStartCompleted * pCallback, void * pCallbackParameter, W_HANDLE * phOperation)
{
   tContext* pContext;
   tDFCExternal dfc;
   dfc.pFunction = (tDFCCallback*)pCallback;
   dfc.pParameter = pCallbackParameter;

   if((pContext = g_pContext) == null)
   {
      return;
   }

   PContextLock(pContext);
   PWIFIEnrolleePairingStart( pContext, pMessageM1, nMessageM1Length, null, &dfc, phOperation );
   PContextReleaseLock(pContext);
}

W_ERROR WWIFIGetCredentialInfo(W_HANDLE hOperation, uint8_t * pVersion, uint8_t * pCredentialBuffer, uint32_t nCredentialLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PWIFIGetCredentialInfo( pContext, hOperation, pVersion, pCredentialBuffer, nCredentialLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WWIFIGetOOBDevicePasswordInfo(W_HANDLE hOperation, tWWIFIOOBDevicePasswordInfo * pOOBDevicePasswordInfo, uint8_t * pVersion, uint8_t * pDevicePasswordBuffer, uint32_t nDevicePasswordLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PWIFIGetOOBDevicePasswordInfo( pContext, hOperation, pOOBDevicePasswordInfo, pVersion, pDevicePasswordBuffer, nDevicePasswordLength );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WWIFIPairingGetMessageM(W_HANDLE hOperation, uint8_t * pMessageMBuffer, uint32_t nMessageMLength)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PWIFIPairingGetMessageM( pContext, hOperation, pMessageMBuffer, nMessageMLength );
   PContextReleaseLock(pContext);
   return ret;
}

uint32_t WWIFIPairingGetMessageMLength(W_HANDLE hOperation)
{
   uint32_t ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return (uint32_t)0;
   }

   PContextLock(pContext);
   ret = PWIFIPairingGetMessageMLength( pContext, hOperation );
   PContextReleaseLock(pContext);
   return ret;
}

W_ERROR WWIFIPairingWriteMessageM2(W_HANDLE hOperation, const uint8_t * pMessageM2Buffer, uint32_t nMessageM2Length)
{
   W_ERROR ret;
   tContext* pContext;

   if((pContext = g_pContext) == null)
   {
      return W_ERROR_BAD_STATE;
   }

   PContextLock(pContext);
   ret = PWIFIPairingWriteMessageM2( pContext, hOperation, pMessageM2Buffer, nMessageM2Length );
   PContextReleaseLock(pContext);
   return ret;
}

#endif /* #if (P_BUILD_CONFIG == P_CONFIG_USER) */

/* End of file */
