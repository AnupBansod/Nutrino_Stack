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

#ifndef __WME_CLIENT_API_AUTOGEN_H
#define __WME_CLIENT_API_AUTOGEN_H

typedef void tPBasicGenericCallbackFunction( tContext* pContext, void * pCallbackParameter, W_ERROR nResult );

typedef void tPBasicGenericCompletionFunction( tContext* pContext, void * pCallbakParameter );

typedef void tPBasicGenericDataCallbackFunction( tContext* pContext, void * pCallbackParameter, uint32_t nDataLength, W_ERROR nResult );

typedef void tPBasicGenericEventHandler( tContext* pContext, void * pHandlerParameter, W_ERROR nEventCode );

typedef void tPBasicGenericEventHandler2( tContext* pContext, void * pHandlerParameter, W_ERROR nEventCode1, W_ERROR nEventCode2 );

typedef void tPBasicGenericHandleCallbackFunction( tContext* pContext, void * pCallbackParameter, W_HANDLE hHandle, W_ERROR nResult );

typedef void tPBTPairingStartCompleted( tContext* pContext, void * pCallbackParameter, uint32_t nRemoteDeviceNameLength, bool bSetBTMasterMode, W_ERROR nError );

typedef void tPEmulCommandReceived( tContext* pContext, void * pCallbackParameter, uint32_t nDataLength );

typedef void tPEmulDriverCommandReceived( tContext* pContext, void * pCallbackParameter, uint32_t nDataLength );

typedef void tPEmulDriverEventReceived( tContext* pContext, void * pCallbackParameter, uint32_t nEventCode );


#ifdef P_INCLUDE_J_EDITION_HELPER
typedef void tPJedPushRegistryEventHandler( tContext* pContext, void * pHandlerParameter, uint32_t nApplicationIdentifier );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
typedef void tPJedSendEvent( tContext* pContext, void * pCallbackParameter, uint32_t nIsolate, uint32_t nEventCode, uint32_t nParam2, uint32_t nParam3, uint32_t nParam4 );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */

typedef void tPMyDMoveGetConfigurationCompleted( tContext* pContext, void * pCallbackParameter, W_ERROR nError, uint8_t nStatusByte, uint8_t nPasswordRetryCounter );

typedef void tPNDEFReadMessageCompleted( tContext* pContext, void * pCallbackParameter, W_HANDLE hMessage, W_ERROR nError );

typedef void tPNFCControllerSelfTestCompleted( tContext* pContext, void * pCallbackParameter, W_ERROR nError, uint32_t nResult );

typedef void tPP2PRecvFromCompleted( tContext* pContext, void * pCallbackParameter, uint32_t nDataLength, W_ERROR nError, uint8_t nSAP );

typedef void tPP2PURILookupCompleted( tContext* pContext, void * pCallbackParameter, uint8_t nDSAP, W_ERROR nError );

typedef void tPReaderCardDetectionHandler( tContext* pContext, void * pHandlerParameter, W_HANDLE hConnection, W_ERROR nError );

typedef void tPTestEntryPoint( tContext* pContext, tTestAPI * pAPI );

typedef void tPTestMessageBoxCompleted( tContext* pContext, void * pCallbackParameter, uint32_t nResult );

typedef void tPUICCGetSlotInfoCompleted( tContext* pContext, void * pCallbackParameter, uint32_t nSWPLinkStatus, W_ERROR nError );

typedef void tPUICCMonitorConnectivityEventHandler( tContext* pContext, void * pHandlerParameter, uint8_t nMessageCode, uint32_t nDataLength );

typedef void tPUICCMonitorTransactionEventHandler( tContext* pContext, void * pHandlerParameter );

typedef void tPWIFIEnrolleePairingStartCompleted( tContext* pContext, void * pCallbackParameter, uint8_t nMode, uint32_t nCredentialLength, uint32_t nDevicePasswordLength, W_ERROR nError );

void P14Part3ExchangeData( tContext* pContext, W_HANDLE hConnection, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation );

W_ERROR P14Part3GetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tW14Part3ConnectionInfo * p14Part3ConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR P14Part3GetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR P14Part3ListenToCardDetectionTypeB( tContext* pContext, tPReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint8_t nAFI, bool bUseCID, uint8_t nCID, uint32_t nBaudRate, const uint8_t * pHigherLayerDataBuffer, uint8_t nHigherLayerDataLength, W_HANDLE * phEventRegistry );

W_ERROR P14Part3SetTimeout( tContext* pContext, W_HANDLE hConnection, uint32_t nTimeout );

void P14Part4ExchangeData( tContext* pContext, W_HANDLE hConnection, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation );

W_ERROR P14Part4GetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tW14Part4ConnectionInfo * p14Part4ConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR P14Part4GetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR P14Part4ListenToCardDetectionTypeA( tContext* pContext, tPReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, bool bUseCID, uint8_t nCID, uint32_t nBaudRate, W_HANDLE * phEventRegistry );

W_ERROR P14Part4ListenToCardDetectionTypeB( tContext* pContext, tPReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint8_t nAFI, bool bUseCID, uint8_t nCID, uint32_t nBaudRate, const uint8_t * pHigherLayerDataBuffer, uint8_t nHigherLayerDataLength, W_HANDLE * phEventRegistry );

W_ERROR P14Part4SetNAD( tContext* pContext, W_HANDLE hConnection, uint8_t nNAD );

W_ERROR P15GetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tW15ConnectionInfo * pConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR P15GetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR P15IsWritable( tContext* pContext, W_HANDLE hConnection, uint8_t nSectorIndex );

W_ERROR P15ListenToCardDetection( tContext* pContext, tPReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint8_t nAFI, W_HANDLE * phEventRegistry );

void P15Read( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation );

void P15SetAttribute( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nActions, uint8_t nAFI, uint8_t nDSFID, W_HANDLE * phOperation );

W_ERROR P15SetTagSize( tContext* pContext, W_HANDLE hConnection, uint16_t nSectorNumber, uint8_t nSectorSize );

void P15Write( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockSectors, W_HANDLE * phOperation );

void P7816CloseLogicalChannel( tContext* pContext, W_HANDLE hLogicalChannel, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void P7816ExchangeAPDU( tContext* pContext, W_HANDLE hConnection, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pSendAPDUBuffer, uint32_t nSendAPDUBufferLength, uint8_t * pReceivedAPDUBuffer, uint32_t nReceivedAPDUBufferMaxLength, W_HANDLE * phOperation );

W_ERROR P7816GetATR( tContext* pContext, W_HANDLE hConnection, uint8_t * pBuffer, uint32_t nBufferMaxLength, uint32_t * pnActualLength );

W_ERROR P7816GetATRSize( tContext* pContext, W_HANDLE hConnection, uint32_t * pnSize );

W_ERROR P7816GetResponseAPDUData( tContext* pContext, W_HANDLE hConnection, uint8_t * pReceivedAPDUBuffer, uint32_t nReceivedAPDUBufferMaxLength, uint32_t * pnReceivedAPDUActualLength );

void P7816OpenLogicalChannel( tContext* pContext, W_HANDLE hConnection, tPBasicGenericHandleCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pAID, uint32_t nAIDLength, W_HANDLE * phOperation );

void PBasicCancelOperation( tContext* pContext, W_HANDLE hOperation );

W_ERROR PBasicCheckConnectionProperty( tContext* pContext, W_HANDLE hConnection, uint8_t nPropertyIdentifier );

void PBasicCloseHandle( tContext* pContext, W_HANDLE hHandle );

void PBasicCloseHandleSafe( tContext* pContext, W_HANDLE hHandle, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void PBasicExecuteEventLoop( tContext* pContext );

W_ERROR PBasicGetConnectionProperties( tContext* pContext, W_HANDLE hConnection, uint8_t * pPropertyArray, uint32_t nArrayLength );

W_ERROR PBasicGetConnectionPropertyNumber( tContext* pContext, W_HANDLE hConnection, uint32_t * pnPropertyNumber );

W_ERROR PBasicInit( tContext* pContext, const tchar * pVersionString );

W_ERROR PBasicPumpEvent( tContext* pContext, bool bWait );

void PBasicStopEventLoop( tContext* pContext );

void PBasicTerminate( tContext* pContext );

void PBasicTestExchangeMessage( tContext* pContext, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pSendMessageBuffer, uint32_t nSendMessageBufferLength, uint8_t * pReceivedMessageBuffer );

void PBPrimeExchangeData( tContext* pContext, W_HANDLE hConnection, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation );

W_ERROR PBPrimeGetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tWBPrimeConnectionInfo * pBPrimeConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR PBPrimeGetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR PBPrimeListenToCardDetection( tContext* pContext, tPReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, const uint8_t * pAPGENBuffer, uint8_t nAPGENLength, W_HANDLE * phEventRegistry );

W_ERROR PBPrimeSetTimeout( tContext* pContext, W_HANDLE hConnection, uint32_t nTimeout );

W_ERROR PBTPairingGetRemoteDeviceInfo( tContext* pContext, W_HANDLE hOperation, tWBTPairingInfo * pRemoteInfo, tchar * pRemoteDeviceNameBuffer, uint32_t nRemoteDeviceNameBufferLength );

void PBTPairingStart( tContext* pContext, uint32_t nMode, const tWBTPairingInfo * pLocalInfo, const tchar * pLocalDeviceName, tPBTPairingStartCompleted * pCallback, void * pCallbackParameter, W_HANDLE * phOperation );


#ifdef P_INCLUDE_JAVA_API
bool PDFCPumpJNICallback( tContext* pContext, uint32_t * pArgs, uint32_t nArgsSize );

#endif /* #ifdef P_INCLUDE_JAVA_API */

void PEmulClose( tContext* pContext, W_HANDLE hHandle, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

W_ERROR PEmulGetMessageData( tContext* pContext, W_HANDLE hHandle, uint8_t * pDataBuffer, uint32_t nDataLength, uint32_t * pnActualDataLength );

bool PEmulIsPropertySupported( tContext* pContext, uint8_t nPropertyIdentifier );

void PEmulOpenConnectionDriver1( tContext* pContext, tPBasicGenericCallbackFunction * pOpenCallback, void * pOpenCallbackParameter, const tWEmulConnectionInfo * pEmulConnectionInfo, uint32_t nSize, W_HANDLE * phHandle );

void PEmulOpenConnectionDriver2( tContext* pContext, W_HANDLE hHandle, tPEmulDriverEventReceived * pEventCallback, void * pEventCallbackParameter );

void PEmulOpenConnectionDriver3( tContext* pContext, W_HANDLE hHandle, tPEmulDriverCommandReceived * pCommandCallback, void * pCommandCallbackParameter );

W_ERROR PEmulSendAnswer( tContext* pContext, W_HANDLE hDriverConnection, const uint8_t * pDataBuffer, uint32_t nDataLength );

void PFeliCaExchangeData( tContext* pContext, W_HANDLE hUserConnection, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOperation );

W_ERROR PFeliCaGetCardList( tContext* pContext, W_HANDLE hConnection, tWFeliCaConnectionInfo * aFeliCaConnectionInfos, const uint32_t nArraySize );

W_ERROR PFeliCaGetCardNumber( tContext* pContext, W_HANDLE hConnection, uint32_t * pnCardNumber );

W_ERROR PFeliCaGetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tWFeliCaConnectionInfo * pConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR PFeliCaGetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR PFeliCaListenToCardDetection( tContext* pContext, tPReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, uint16_t nSystemCode, W_HANDLE * phEventRegistry );

void PFeliCaRead( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nLength, uint8_t nNumberOfService, const uint16_t * pServiceCodeList, uint8_t nNumberOfBlocks, const uint8_t * pBlockList, W_HANDLE * phOperation );


#ifdef P_INCLUDE_JAVA_API
void PFeliCaReadSimple( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nLength, uint32_t nNumberOfService, uint32_t * pServiceCodeList, uint32_t nNumberOfBlocks, const uint8_t * pBlockList );

#endif /* #ifdef P_INCLUDE_JAVA_API */

void PFeliCaSelectCard( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const tWFeliCaConnectionInfo * pFeliCaConnectionInfo );

void PFeliCaSelectSystem( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t nIndexSubSystem );

void PFeliCaWrite( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nLength, uint8_t nNumberOfService, const uint16_t * pServiceCodeList, uint8_t nNumberOfBlocks, const uint8_t * pBlockList, W_HANDLE * phOperation );


#ifdef P_INCLUDE_JAVA_API
void PFeliCaWriteSimple( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nLength, uint32_t nNumberOfService, uint32_t * pServiceCodeList, uint32_t nNumberOfBlocks, const uint8_t * pBlockList );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_HANDLE PJavaCreateByteBuffer( tContext* pContext, uint8_t * pJavaBuffer, uint32_t nOffset, uint32_t nLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint32_t PJavaGetByteBufferLength( tContext* pContext, W_HANDLE hBufferReference );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint32_t PJavaGetByteBufferOffset( tContext* pContext, W_HANDLE hBufferReference );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint8_t * PJavaGetByteBufferPointer( tContext* pContext, W_HANDLE hBufferReference );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
uint32_t PJavaNDEFGetMessageContent( tContext* pContext, W_HANDLE hMessage, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
void PJavaNDEFWriteMessage( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength, uint32_t nActionMask, W_HANDLE * phOperation );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
void PJavaNDEFWriteMessageOnAnyTag( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nPriority, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength, uint32_t nActionMask, W_HANDLE * phOperation );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
bool PJavaNFCControllerGetBooleanProperty( tContext* pContext, uint8_t nPropertyIdentifier );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_ERROR PJavaP2PGetConfigurationBuffer( tContext* pContext, uint8_t * pConfigurationBuffer, uint32_t nConfigurationBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_ERROR PJavaP2PGetLinkPropertiesBuffer( tContext* pContext, W_HANDLE hLink, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
W_ERROR PJavaP2PSetConfigurationBuffer( tContext* pContext, uint8_t * pConfigurationBuffer, uint32_t nConfigurationBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_JAVA_API
void PJavaReleaseByteBuffer( tContext* pContext, W_HANDLE hBufferReference, uint8_t * pJavaBuffer, uint32_t nJavaBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */


#ifdef P_INCLUDE_J_EDITION_HELPER
void PJed14Part3ExchangeData( tContext* pContext, W_HANDLE hConnection, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pReaderToCardBuffer, uint32_t nReaderToCardBufferLength, uint8_t * pCardToReaderBuffer, uint32_t nCardToReaderBufferMaxLength, W_HANDLE * phOpeartion, bool bCheckResponseCRC );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedAddHandle( tContext* pContext, W_HANDLE hHandle, uint32_t nIsolate );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedCheckHandle( tContext* pContext, W_HANDLE hHandle, uint32_t * pnIsolate );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void PJedCloseHandle( tContext* pContext, W_HANDLE hHandle );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_HANDLE PJedCreateBufferObject( tContext* pContext, uint32_t nIsolate, uint32_t nParameter, uint32_t nLength, uint8_t ** ppContent );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedGetBufferContent( tContext* pContext, W_HANDLE hBufferObject, uint32_t * pnIsolate, uint32_t * pnParameter, uint8_t ** ppContent, uint32_t * pnLength );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedGetInfo( tContext* pContext, W_HANDLE hHandle, uint32_t nInfoType, uint32_t * pnValue );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedGetInfoBuffer( tContext* pContext, W_HANDLE hHandle, uint32_t nInfoType, uint8_t * pBuffer, uint32_t nBufferLength, uint32_t * pnActualLength );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedGetNFCCProperty( tContext* pContext, const tchar * pPropertyName, tchar * pPropertyValueBuffer, uint32_t nPropertyValueBufferLength, uint32_t * pnPropertyValueLength );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedGetP2PPushSocket( tContext* pContext, const tchar * pPushEntry, W_HANDLE * phSocket );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedInitialize( tContext* pContext, uint32_t nKeepAliveConnectionTimeout, uint32_t nPushRegistryTimeout, uint8_t nReaderPriority, tPJedSendEvent * pSendEventFunction, void * pCallbackParameter );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_HANDLE PJedListenToCardDetection( tContext* pContext, uint32_t nIsolate, uint32_t nListenerId, const uint8_t * pProperties, uint32_t nPropertyNumber );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedMonitorEndOfTransaction( tContext* pContext, uint32_t nIsolate, bool bRegister, uint32_t nSlotId );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedPushRegistryAddEntry( tContext* pContext, tPJedPushRegistryEventHandler * pEventHandler, void * pHandlerParameter, uint32_t nApplicationIdentifier, const uint8_t * pURLString );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedPushRegistryCheckEntry( tContext* pContext, const uint8_t * pURLString );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedPushRegistryRemoveEntry( tContext* pContext, uint32_t nApplicationIdentifier, const uint8_t * pURLString );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void PJedReaderHandlerWorkPerformed( tContext* pContext, W_HANDLE hConnection, bool bGiveToNextListener, bool bCardApplicationMatch );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedReadOnAnyTag( tContext* pContext, uint32_t nIsolate, bool bRegister, uint32_t nListenerId, uint8_t nTNF, const tchar * pTypeString );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void PJedReleaseApplication( tContext* pContext, uint32_t nIsolate );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
W_ERROR PJedRetrieveConnection( tContext* pContext, W_HANDLE hConnection );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_J_EDITION_HELPER
void PJedTerminate( tContext* pContext );

#endif /* #ifdef P_INCLUDE_J_EDITION_HELPER */


#ifdef P_INCLUDE_JUPITER_SE
void PJupiterSEApplyPolicy( tContext* pContext, uint32_t nSlotIdentifier, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

#endif /* #ifdef P_INCLUDE_JUPITER_SE */


#ifdef P_INCLUDE_JUPITER_SE
void PJupiterSEGetAID( tContext* pContext, uint32_t nSlotIdentifier, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nBufferLength );

#endif /* #ifdef P_INCLUDE_JUPITER_SE */

W_ERROR PMifareGetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tWMifareConnectionInfo * pConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR PMifareGetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

void PMifareRead( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation );

void PMifareULCAuthenticate( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pKey, uint32_t nKeyLength );

void PMifareULCSetAccessRights( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pKey, uint32_t nKeyLength, uint8_t nThreshold, uint32_t nRights, bool bLockConfiguration );

W_ERROR PMifareULForceULC( tContext* pContext, W_HANDLE hConnection );

void PMifareULFreezeDataLockConfiguration( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

W_ERROR PMifareULGetAccessRights( tContext* pContext, W_HANDLE hConnection, uint32_t nOffset, uint32_t nLength, uint32_t * pnRights );

void PMifareULRetrieveAccessRights( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void PMifareWrite( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockSectors, W_HANDLE * phOperation );

W_ERROR PMyDGetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tWMyDConnectionInfo * pConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR PMyDGetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

void PMyDMoveAuthenticate( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint32_t nPassword );

void PMyDMoveFreezeDataLockConfiguration( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void PMyDMoveGetConfiguration( tContext* pContext, W_HANDLE hConnection, tPMyDMoveGetConfigurationCompleted * pCallback, void * pCallbackParameter );

void PMyDMoveSetConfiguration( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nStatusByte, uint8_t nPasswordRetryCounter, uint32_t nPassword, bool bLockConfiguration );

void PMyDRead( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation );

void PMyDWrite( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockSectors, W_HANDLE * phOperation );

W_ERROR PNDEFAppendRecord( tContext* pContext, W_HANDLE hMessage, W_HANDLE hRecord );

W_ERROR PNDEFBuildMessage( tContext* pContext, const uint8_t * pBuffer, uint32_t nBufferLength, W_HANDLE * phMessage );

W_ERROR PNDEFBuildRecord( tContext* pContext, const uint8_t * pBuffer, uint32_t nBufferLength, W_HANDLE * phRecord );

bool PNDEFCheckIdentifier( tContext* pContext, W_HANDLE hRecord, const tchar * pIdentifierString );

bool PNDEFCheckType( tContext* pContext, W_HANDLE hRecord, uint8_t nTNF, const tchar * pTypeString );

W_ERROR PNDEFCreateNestedMessageRecord( tContext* pContext, uint8_t nTNF, const tchar * pTypeString, W_HANDLE hNestedMessage, W_HANDLE * phRecord );

W_ERROR PNDEFCreateNewMessage( tContext* pContext, W_HANDLE * phMessage );

W_ERROR PNDEFCreateRecord( tContext* pContext, uint8_t nTNF, const tchar * pTypeString, const uint8_t * pPayloadBuffer, uint32_t nPayloadLength, W_HANDLE * phRecord );

W_ERROR PNDEFGetEnclosedMessage( tContext* pContext, W_HANDLE hRecord, W_HANDLE * phMessage );

W_ERROR PNDEFGetIdentifierString( tContext* pContext, W_HANDLE hRecord, tchar * pIdentifierStringBuffer, uint32_t nIdentifierStringBufferLength, uint32_t * pnActualLength );

W_ERROR PNDEFGetMessageContent( tContext* pContext, W_HANDLE hMessage, uint8_t * pMessageBuffer, uint32_t nMessageBufferLength, uint32_t * pnActualLength );

uint32_t PNDEFGetMessageLength( tContext* pContext, W_HANDLE hMessage );

W_HANDLE PNDEFGetNextMessage( tContext* pContext, W_HANDLE hMessage );

W_ERROR PNDEFGetPayloadPointer( tContext* pContext, W_HANDLE hRecord, uint8_t ** ppBuffer );

W_HANDLE PNDEFGetRecord( tContext* pContext, W_HANDLE hMessage, uint32_t nIndex );

W_ERROR PNDEFGetRecordInfo( tContext* pContext, W_HANDLE hRecord, uint32_t nInfoType, uint32_t * pnValue );

W_ERROR PNDEFGetRecordInfoBuffer( tContext* pContext, W_HANDLE hRecord, uint32_t nInfoType, uint8_t * pBuffer, uint32_t nBufferLength, uint32_t * pnActualLength );

uint32_t PNDEFGetRecordNumber( tContext* pContext, W_HANDLE hMessage );

W_ERROR PNDEFGetTagInfo( tContext* pContext, W_HANDLE hConnection, tNDEFTagInfo * pTagInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR PNDEFGetTagInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR PNDEFGetTypeString( tContext* pContext, W_HANDLE hRecord, tchar * pTypeStringBuffer, uint32_t nTypeStringBufferLength, uint32_t * pnActualLength );

W_ERROR PNDEFInsertRecord( tContext* pContext, W_HANDLE hMessage, uint32_t nIndex, W_HANDLE hRecord );

void PNDEFReadMessage( tContext* pContext, W_HANDLE hConnection, tPNDEFReadMessageCompleted * pCallback, void * pCallbackParameter, uint8_t nTNF, const tchar * pTypeString, W_HANDLE * phOperation );

void PNDEFReadMessageOnAnyTag( tContext* pContext, tPNDEFReadMessageCompleted * pCallback, void * pCallbackParameter, uint8_t nPriority, uint8_t nTNF, const tchar * pTypeString, W_HANDLE * phRegistry );

W_ERROR PNDEFRemoveRecord( tContext* pContext, W_HANDLE hMessage, uint32_t nIndex );

W_ERROR PNDEFSetIdentifierString( tContext* pContext, W_HANDLE hRecord, const tchar * pIdentifierString );

W_ERROR PNDEFSetRecord( tContext* pContext, W_HANDLE hMessage, uint32_t nIndex, W_HANDLE hRecord );

W_ERROR PNDEFSetRecordInfo( tContext* pContext, W_HANDLE hRecord, uint32_t nInfoType, const uint8_t * pBuffer, uint32_t nBufferLength );

void PNDEFWriteMessage( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, W_HANDLE hMessage, uint32_t nActionMask, W_HANDLE * phOperation );

void PNDEFWriteMessageOnAnyTag( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nPriority, W_HANDLE hMessage, uint32_t nActionMask, W_HANDLE * phOperation );

void PNFCControllerFirmwareUpdate( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pUpdateBuffer, uint32_t nUpdateBufferLength, uint32_t nMode );

uint32_t PNFCControllerFirmwareUpdateState( tContext* pContext );

W_ERROR PNFCControllerGetBooleanProperty( tContext* pContext, uint8_t nPropertyIdentifier, bool * pbValue );

W_ERROR PNFCControllerGetIntegerProperty( tContext* pContext, uint8_t nPropertyIdentifier, uint32_t * pnValue );

uint32_t PNFCControllerGetMode( tContext* pContext );

W_ERROR PNFCControllerGetProperty( tContext* pContext, uint8_t nPropertyIdentifier, tchar * pValueBuffer, uint32_t nBufferLength, uint32_t * pnValueLength );

W_ERROR PNFCControllerGetRawMessageData( tContext* pContext, uint8_t * pBuffer, uint32_t nBufferLength, uint32_t * pnActualLength );

void PNFCControllerGetRFActivity( tContext* pContext, uint8_t * pnReaderState, uint8_t * pnCardState, uint8_t * pnP2PState );

void PNFCControllerGetRFLock( tContext* pContext, uint32_t nLockSet, bool * pbReaderLock, bool * pbCardLock );

W_ERROR PNFCControllerMonitorException( tContext* pContext, tPBasicGenericEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry );

W_ERROR PNFCControllerMonitorFieldEvents( tContext* pContext, tPBasicGenericEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry );

void PNFCControllerProductionTest( tContext* pContext, const uint8_t * pParameterBuffer, uint32_t nParameterBufferLength, uint8_t * pResultBuffer, uint32_t nResultBufferLength, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter );

W_ERROR PNFCControllerRegisterRawListener( tContext* pContext, tPBasicGenericDataCallbackFunction * pReceiveMessageEventHandler, void * pHandlerParameter );

void PNFCControllerReset( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint32_t nMode );

void PNFCControllerSelfTest( tContext* pContext, tPNFCControllerSelfTestCompleted * pCallback, void * pCallbackParameter );

void PNFCControllerSetRFLock( tContext* pContext, uint32_t nLockSet, bool bReaderLock, bool bCardLock, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

W_ERROR PNFCControllerSwitchStandbyMode( tContext* pContext, bool bStandbyOn );

void PNFCControllerSwitchToRawMode( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void PNFCControllerWriteRawMessage( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nLength );

void PP2PConnect( tContext* pContext, W_HANDLE hSocket, W_HANDLE hLink, tPBasicGenericCallbackFunction * pEstablishmentCallback, void * pEstablishmentCallbackParameter );

W_ERROR PP2PCreateSocket( tContext* pContext, uint8_t nType, const tchar * pServiceURI, uint8_t nSAP, W_HANDLE * phSocket );

W_HANDLE PP2PEstablishLinkDriver1( tContext* pContext, tPBasicGenericHandleCallbackFunction * pEstablishmentCallback, void * pEstablishmentCallbackParameter );

void PP2PEstablishLinkDriver2( tContext* pContext, W_HANDLE hLink, tPBasicGenericCallbackFunction * pReleaseCallback, void * pReleaseCallbackParameter, W_HANDLE * phOperation );

W_ERROR PP2PGetConfiguration( tContext* pContext, tWP2PConfiguration * pConfiguration );

W_ERROR PP2PGetLinkProperties( tContext* pContext, W_HANDLE hLink, tWP2PLinkProperties * pProperties );

W_ERROR PP2PGetSocketParameter( tContext* pContext, W_HANDLE hSocket, uint32_t nParameter, uint32_t * pnValue );

void PP2PRead( tContext* pContext, W_HANDLE hSocket, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pReceptionBuffer, uint32_t nReceptionBufferLength, W_HANDLE * phOperation );

void PP2PRecvFrom( tContext* pContext, W_HANDLE hSocket, tPP2PRecvFromCompleted * pCallback, void * pCallbackParameter, uint8_t * pReceptionBuffer, uint32_t nReceptionBufferLength, W_HANDLE * phOperation );

void PP2PSendTo( tContext* pContext, W_HANDLE hSocket, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t nSAP, const uint8_t * pSendBuffer, uint32_t nSendBufferLength, W_HANDLE * phOperation );

W_ERROR PP2PSetConfiguration( tContext* pContext, const tWP2PConfiguration * pConfiguration );

W_ERROR PP2PSetSocketParameter( tContext* pContext, W_HANDLE hSocket, uint32_t nParameter, uint32_t nValue );

void PP2PShutdown( tContext* pContext, W_HANDLE hSocket, tPBasicGenericCallbackFunction * pReleaseCallback, void * pReleaseCallbackParameter );

void PP2PURILookup( tContext* pContext, W_HANDLE hLink, tPP2PURILookupCompleted * pCallback, void * pCallbackParameter, const tchar * pServiceURI );

void PP2PWrite( tContext* pContext, W_HANDLE hSocket, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pSendBuffer, uint32_t nSendBufferLength, W_HANDLE * phOperation );

W_ERROR PPicoGetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tWPicoConnectionInfo * pConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR PPicoGetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR PPicoIsWritable( tContext* pContext, W_HANDLE hConnection );

void PPicoRead( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation );

void PPicoWrite( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockCard, W_HANDLE * phOperation );

W_ERROR PReaderErrorEventRegister( tContext* pContext, tPBasicGenericEventHandler * pHandler, void * pHandlerParameter, uint8_t nEventType, bool bCardDetectionRequested, W_HANDLE * phRegistryHandle );

W_ERROR PReaderGetPulsePeriod( tContext* pContext, uint32_t * pnTimeout );

void PReaderHandlerWorkPerformed( tContext* pContext, W_HANDLE hConnection, bool bGiveToNextListener, bool bCardApplicationMatch );

bool PReaderIsPropertySupported( tContext* pContext, uint8_t nPropertyIdentifier );

W_ERROR PReaderListenToCardDetection( tContext* pContext, tPReaderCardDetectionHandler * pHandler, void * pHandlerParameter, uint8_t nPriority, const uint8_t * pConnectionPropertyArray, uint32_t nPropertyNumber, W_HANDLE * phEventRegistry );

bool PReaderPreviousApplicationMatch( tContext* pContext, W_HANDLE hConnection );

void PReaderSetPulsePeriod( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint32_t nPulsePeriod );

bool PRTDIsTextRecord( tContext* pContext, W_HANDLE hRecord );

bool PRTDIsURIRecord( tContext* pContext, W_HANDLE hRecord );

W_ERROR PRTDTextAddRecord( tContext* pContext, W_HANDLE hMessage, const tchar * pLanguage, bool bUseUtf8, const tchar * pText, uint32_t nTextLength );

W_ERROR PRTDTextCreateRecord( tContext* pContext, const tchar * pLanguage, bool bUseUtf8, const tchar * pText, uint32_t nTextLength, W_HANDLE * phRecord );

W_ERROR PRTDTextFind( tContext* pContext, W_HANDLE hMessage, const tchar * pLanguage1, const tchar * pLanguage2, W_HANDLE * phRecord, uint8_t * pnMatch );

W_ERROR PRTDTextGetLanguage( tContext* pContext, W_HANDLE hRecord, tchar * pLanguageBuffer, uint32_t nBufferLength );

uint32_t PRTDTextGetLength( tContext* pContext, W_HANDLE hRecord );

W_ERROR PRTDTextGetValue( tContext* pContext, W_HANDLE hRecord, tchar * pBuffer, uint32_t nBufferLength );

uint8_t PRTDTextLanguageMatch( tContext* pContext, W_HANDLE hRecord, const tchar * pLanguage1, const tchar * pLanguage2 );

W_ERROR PRTDURIAddRecord( tContext* pContext, W_HANDLE hMessage, const tchar * pURI );

W_ERROR PRTDURICreateRecord( tContext* pContext, const tchar * pURI, W_HANDLE * phRecord );

uint32_t PRTDURIGetLength( tContext* pContext, W_HANDLE hRecord );

W_ERROR PRTDURIGetValue( tContext* pContext, W_HANDLE hRecord, tchar * pBuffer, uint32_t nBufferLength );

W_ERROR PSecurityAuthenticate( tContext* pContext, const uint8_t * pApplicationDataBuffer, uint32_t nApplicationDataBufferLength );

W_ERROR PSEGetInfo( tContext* pContext, uint32_t nSlotIdentifier, tSEInfo * pSEInfo );

uint32_t PSEGetTransactionAID( tContext* pContext, uint32_t nSlotIdentifier, uint8_t * pBuffer, uint32_t nBufferLength );

W_ERROR PSEMonitorEndOfTransaction( tContext* pContext, uint32_t nSlotIdentifier, tPBasicGenericEventHandler2 * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry );

W_ERROR PSEMonitorHotPlugEvents( tContext* pContext, uint32_t nSlotIdentifier, tPBasicGenericEventHandler2 * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry );

void PSEOpenConnection( tContext* pContext, uint32_t nSlotIdentifier, bool bForce, tPBasicGenericHandleCallbackFunction * pCallback, void * pCallbackParameter );

void PSESetPolicy( tContext* pContext, uint32_t nSlotIdentifier, uint32_t nStorageType, uint32_t nProtocols, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void * PTestAlloc( tContext* pContext, uint32_t nSize );

void PTestExecuteRemoteFunction( tContext* pContext, const char * pFunctionIdentifier, uint32_t nParameter, const uint8_t * pParameterBuffer, uint32_t nParameterBufferLength, uint8_t * pResultBuffer, uint32_t nResultBufferLength, tPBasicGenericDataCallbackFunction * pCallback, void * pCallbackParameter );

void PTestFree( tContext* pContext, void * pBuffer );

const void * PTestGetConstAddress( tContext* pContext, const void * pConstData );

uint32_t PTestGetCurrentTime( tContext* pContext );

tTestExecuteContext * PTestGetExecuteContext( tContext* pContext );

bool PTestIsInAutomaticMode( tContext* pContext );

void PTestMessageBox( tContext* pContext, uint32_t nFlags, const char * pMessage, uint32_t nAutomaticResult, tPTestMessageBoxCompleted * pCallback, void * pCallbackParameter );

void PTestNotifyEnd( tContext* pContext );

void PTestPresentObject( tContext* pContext, const char * pObjectName, const char * pOperatorMessage, uint32_t nDistance, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void PTestRemoveObject( tContext* pContext, const char * pOperatorMessage, bool bSaveState, bool bCheckUnmodifiedState, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void PTestSetResult( tContext* pContext, uint32_t nResult, const void * pResultData, uint32_t nResultDataLength );

void PTestSetTimer( tContext* pContext, uint32_t nTimeout, tPBasicGenericCompletionFunction * pCallback, void * pCallbackParameter );

void PTestTraceBuffer( tContext* pContext, const uint8_t * pBuffer, uint32_t nLength );

void PTestTraceError( tContext* pContext, const char * pMessage, va_list args );

void PTestTraceInfo( tContext* pContext, const char * pMessage, va_list args );

void PTestTraceWarning( tContext* pContext, const char * pMessage, va_list args );

W_ERROR PType1ChipGetConnectionInfo( tContext* pContext, W_HANDLE hConnection, tWType1ChipConnectionInfo * pConnectionInfo );


#ifdef P_INCLUDE_JAVA_API
W_ERROR PType1ChipGetConnectionInfoBuffer( tContext* pContext, W_HANDLE hConnection, uint8_t * pInfoBuffer, uint32_t nInfoBufferLength );

#endif /* #ifdef P_INCLUDE_JAVA_API */

W_ERROR PType1ChipIsWritable( tContext* pContext, W_HANDLE hConnection );

void PType1ChipRead( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, W_HANDLE * phOperation );

void PType1ChipWrite( tContext* pContext, W_HANDLE hConnection, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, const uint8_t * pBuffer, uint32_t nOffset, uint32_t nLength, bool bLockBlocks, W_HANDLE * phOperation );

void PUICCActivateSWPLine( tContext* pContext, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

void PUICCGetAccessPolicy( tContext* pContext, uint32_t nStorageType, tWUICCAccessPolicy * pAccessPolicy );

W_ERROR PUICCGetConnectivityEventParameter( tContext* pContext, uint8_t * pDataBuffer, uint32_t nBufferLength, uint32_t * pnActualDataLength );

void PUICCGetSlotInfo( tContext* pContext, tPUICCGetSlotInfoCompleted * pCallback, void * pCallbackParameter );

uint32_t PUICCGetTransactionEventAID( tContext* pContext, uint8_t * pBuffer, uint32_t nBufferLength );

W_ERROR PUICCMonitorConnectivityEvent( tContext* pContext, tPUICCMonitorConnectivityEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry );

W_ERROR PUICCMonitorTransactionEvent( tContext* pContext, tPUICCMonitorTransactionEventHandler * pHandler, void * pHandlerParameter, W_HANDLE * phEventRegistry );

void PUICCSetAccessPolicy( tContext* pContext, uint32_t nStorageType, const tWUICCAccessPolicy * pAccessPolicy, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter );

W_ERROR PVirtualTagCreate( tContext* pContext, uint8_t nTagType, const uint8_t * pIdentifier, uint32_t nIdentifierLength, uint32_t nMaximumMessageLength, W_HANDLE * phVirtualTag );

void PVirtualTagStop( tContext* pContext, W_HANDLE hVirtualTag, tPBasicGenericCallbackFunction * pCompletionCallback, void * pCallbackParameter );

void PWIFIAccessPointConfigureToken( tContext* pContext, uint8_t nVersion, const uint8_t * pCredentialBuffer, uint32_t nCredentialLength, tPBasicGenericCallbackFunction * pCallback, void * pCallbackParameter, W_HANDLE * phOperation );

void PWIFIEnrolleePairingStart( tContext* pContext, const uint8_t * pMessageM1, uint32_t nMessageM1Length, tPWIFIEnrolleePairingStartCompleted * pCallback, void * pCallbackParameter, W_HANDLE * phOperation );

W_ERROR PWIFIGetCredentialInfo( tContext* pContext, W_HANDLE hOperation, uint8_t * pVersion, uint8_t * pCredentialBuffer, uint32_t nCredentialLength );

W_ERROR PWIFIGetOOBDevicePasswordInfo( tContext* pContext, W_HANDLE hOperation, tWWIFIOOBDevicePasswordInfo * pOOBDevicePasswordInfo, uint8_t * pVersion, uint8_t * pDevicePasswordBuffer, uint32_t nDevicePasswordLength );

W_ERROR PWIFIPairingGetMessageM( tContext* pContext, W_HANDLE hOperation, uint8_t * pMessageMBuffer, uint32_t nMessageMLength );

uint32_t PWIFIPairingGetMessageMLength( tContext* pContext, W_HANDLE hOperation );

W_ERROR PWIFIPairingWriteMessageM2( tContext* pContext, W_HANDLE hOperation, const uint8_t * pMessageM2Buffer, uint32_t nMessageM2Length );

#ifndef OPEN_NFC_BUILD_NUMBER
#define OPEN_NFC_BUILD_NUMBER 0
#define OPEN_NFC_BUILD_NUMBER_S  { '0',  0 }
#endif /* #ifndef OPEN_NFC_BUILD_NUMBER */
#ifndef OPEN_NFC_PRODUCT_SIMPLE_VERSION
#define OPEN_NFC_PRODUCT_SIMPLE_VERSION 4.3
#define OPEN_NFC_PRODUCT_SIMPLE_VERSION_S  { '4', '.', '3',  0 }
#endif /* #ifndef OPEN_NFC_PRODUCT_SIMPLE_VERSION */
#ifndef OPEN_NFC_PRODUCT_VERSION
#define OPEN_NFC_PRODUCT_VERSION v4.3 (Build 0)
#define OPEN_NFC_PRODUCT_VERSION_S  { 'v', '4', '.', '3', ' ', '(', 'B', 'u', 'i', 'l', 'd', ' ', '0', ')',  0 }
#endif /* #ifndef OPEN_NFC_PRODUCT_VERSION */


#endif /* #ifdef __WME_CLIENT_API_AUTOGEN_H */
