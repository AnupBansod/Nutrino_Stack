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

package org.opennfc;

/**
 * @hide
 **/
/* package protected */final class MethodAutogen {

   private MethodAutogen() {}

   /* package protected */native static void W14Part3ExchangeData(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericDataCallbackFunction * */,
            int pReaderToCardBuffer /* const uint8_t * */,
            int pCardToReaderBuffer /* uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int W14Part3GetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static int W14Part3ListenToCardDetectionTypeB(
            int pHandler /* tWReaderCardDetectionHandler * */,
            int nPriority /* uint8_t */,
            int nAFI /* uint8_t */,
            boolean bUseCID,
            int nCID /* uint8_t */,
            int nBaudRate /* uint32_t */,
            byte[] pHigherLayerDataBuffer /* const uint8_t * */,
            int[] phEventRegistry /* W_HANDLE * */ );

   /* package protected */native static int W14Part3SetTimeout(
            int hConnection /* W_HANDLE */,
            int nTimeout /* uint32_t */ );

   /* package protected */native static void W14Part4ExchangeData(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericDataCallbackFunction * */,
            int pReaderToCardBuffer /* const uint8_t * */,
            int pCardToReaderBuffer /* uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int W14Part4GetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static int W14Part4ListenToCardDetectionTypeA(
            int pHandler /* tWReaderCardDetectionHandler * */,
            int nPriority /* uint8_t */,
            boolean bUseCID,
            int nCID /* uint8_t */,
            int nBaudRate /* uint32_t */,
            int[] phEventRegistry /* W_HANDLE * */ );

   /* package protected */native static int W14Part4ListenToCardDetectionTypeB(
            int pHandler /* tWReaderCardDetectionHandler * */,
            int nPriority /* uint8_t */,
            int nAFI /* uint8_t */,
            boolean bUseCID,
            int nCID /* uint8_t */,
            int nBaudRate /* uint32_t */,
            byte[] pHigherLayerDataBuffer /* const uint8_t * */,
            int[] phEventRegistry /* W_HANDLE * */ );

   /* package protected */native static int W14Part4SetNAD(
            int hConnection /* W_HANDLE */,
            int nNAD /* uint8_t */ );

   /* package protected */native static int W15GetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static int W15IsWritable(
            int hConnection /* W_HANDLE */,
            int nSectorIndex /* uint8_t */ );

   /* package protected */native static int W15ListenToCardDetection(
            int pHandler /* tWReaderCardDetectionHandler * */,
            int nPriority /* uint8_t */,
            int nAFI /* uint8_t */,
            int[] phEventRegistry /* W_HANDLE * */ );

   /* package protected */native static void W15Read(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* uint8_t * */,
            int nOffset /* uint32_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void W15SetAttribute(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int nActions /* uint8_t */,
            int nAFI /* uint8_t */,
            int nDSFID /* uint8_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int W15SetTagSize(
            int hConnection /* W_HANDLE */,
            int nSectorNumber /* uint16_t */,
            int nSectorSize /* uint8_t */ );

   /* package protected */native static void W15Write(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* const uint8_t * */,
            int nOffset /* uint32_t */,
            boolean bLockSectors,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void W7816CloseLogicalChannel(
            int hLogicalChannel /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static void W7816ExchangeAPDU(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericDataCallbackFunction * */,
            int pSendAPDUBuffer /* const uint8_t * */,
            int pReceivedAPDUBuffer /* uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int W7816GetATR(
            int hConnection /* W_HANDLE */,
            byte[] pBuffer /* uint8_t * */,
            int[] pnActualLength /* uint32_t * */ );

   /* package protected */native static int W7816GetATRSize(
            int hConnection /* W_HANDLE */,
            int[] pnSize /* uint32_t * */ );

   /* package protected */native static int W7816GetResponseAPDUData(
            int hConnection /* W_HANDLE */,
            byte[] pReceivedAPDUBuffer /* uint8_t * */,
            int[] pnReceivedAPDUActualLength /* uint32_t * */ );

   /* package protected */native static void W7816OpenLogicalChannel(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericHandleCallbackFunction * */,
            int pAID /* const uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WBasicCancelOperation(
            int hOperation /* W_HANDLE */ );

   /* package protected */native static int WBasicCheckConnectionProperty(
            int hConnection /* W_HANDLE */,
            int nPropertyIdentifier /* uint8_t */ );

   /* package protected */native static void WBasicCloseHandle(
            int hHandle /* W_HANDLE */ );

   /* package protected */native static void WBasicCloseHandleSafe(
            int hHandle /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static int WBasicGetConnectionProperties(
            int hConnection /* W_HANDLE */,
            byte[] pPropertyArray /* uint8_t * */ );

   /* package protected */native static int WBasicGetConnectionPropertyNumber(
            int hConnection /* W_HANDLE */,
            int[] pnPropertyNumber /* uint32_t * */ );

   /* package protected */native static int WBasicInit(
            String pVersionString );

   /* package protected */native static int WBasicPumpEvent(
            boolean bWait );

   /* package protected */native static void WBasicStopEventLoop( );

   /* package protected */native static void WBasicTerminate( );

   /* package protected */native static void WBPrimeExchangeData(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericDataCallbackFunction * */,
            int pReaderToCardBuffer /* const uint8_t * */,
            int pCardToReaderBuffer /* uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WBPrimeGetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static int WBPrimeListenToCardDetection(
            int pHandler /* tWReaderCardDetectionHandler * */,
            int nPriority /* uint8_t */,
            byte[] pAPGENBuffer /* const uint8_t * */,
            int[] phEventRegistry /* W_HANDLE * */ );

   /* package protected */native static int WBPrimeSetTimeout(
            int hConnection /* W_HANDLE */,
            int nTimeout /* uint32_t */ );

   /* package protected */native static boolean WDFCPumpJNICallback(
            int[] pArgs /* uint32_t * */ );

   /* package protected */native static void WEmulClose(
            int hHandle /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static int WEmulGetMessageData(
            int hHandle /* W_HANDLE */,
            byte[] pDataBuffer /* uint8_t * */,
            int[] pnActualDataLength /* uint32_t * */ );

   /* package protected */native static boolean WEmulIsPropertySupported(
            int nPropertyIdentifier /* uint8_t */ );

   /* package protected */native static void WEmulOpenConnectionBuffer(
            int pOpenCallback /* tWBasicGenericCallbackFunction * */,
            int pEventCallback /* tWBasicGenericEventHandler * */,
            int pCommandCallback /* tWEmulCommandReceived * */,
            int nCardType /* uint32_t */,
            byte[] pUID /* uint8_t * */,
            int nRandomIdentifierLength /* uint32_t */,
            int[] phHandle /* W_HANDLE * */ );

   /* package protected */native static int WEmulSendAnswer(
            int hDriverConnection /* W_HANDLE */,
            byte[] pDataBuffer /* const uint8_t * */ );

   /* package protected */native static int WFeliCaGetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static void WFeliCaReadSimple(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* uint8_t * */,
            int[] pServiceCodeList /* uint32_t * */,
            byte[] pBlockList /* const uint8_t * */ );

   /* package protected */native static void WFeliCaWriteSimple(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* const uint8_t * */,
            int[] pServiceCodeList /* uint32_t * */,
            byte[] pBlockList /* const uint8_t * */ );

   /* package protected */native static int WJavaCreateByteBuffer(
            byte[] pJavaBuffer /* uint8_t * */,
            int nOffset /* uint32_t */,
            int nLength /* uint32_t */ );

   /* package protected */native static void WJavaNDEFWriteMessage(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pMessageBuffer /* uint8_t * */,
            int nActionMask /* uint32_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WJavaNDEFWriteMessageOnAnyTag(
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int nPriority /* uint8_t */,
            int pMessageBuffer /* uint8_t * */,
            int nActionMask /* uint32_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static boolean WJavaNFCControllerGetBooleanProperty(
            int nPropertyIdentifier /* uint8_t */ );

   /* package protected */native static int WJavaP2PGetConfigurationBuffer(
            byte[] pConfigurationBuffer /* uint8_t * */ );

   /* package protected */native static int WJavaP2PGetLinkPropertiesBuffer(
            int hLink /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static int WJavaP2PSetConfigurationBuffer(
            byte[] pConfigurationBuffer /* uint8_t * */ );

   /* package protected */native static void WJavaReleaseByteBuffer(
            int hBufferReference /* W_HANDLE */,
            byte[] pJavaBuffer /* uint8_t * */ );

   /* package protected */native static int WMifareGetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static void WMifareRead(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* uint8_t * */,
            int nOffset /* uint32_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WMifareULCAuthenticate(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            byte[] pKey /* const uint8_t * */ );

   /* package protected */native static void WMifareULCSetAccessRights(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            byte[] pKey /* const uint8_t * */,
            int nThreshold /* uint8_t */,
            int nRights /* uint32_t */,
            boolean bLockConfiguration );

   /* package protected */native static int WMifareULForceULC(
            int hConnection /* W_HANDLE */ );

   /* package protected */native static void WMifareULFreezeDataLockConfiguration(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static int WMifareULGetAccessRights(
            int hConnection /* W_HANDLE */,
            int nOffset /* uint32_t */,
            int nLength /* uint32_t */,
            int[] pnRights /* uint32_t * */ );

   /* package protected */native static void WMifareULRetrieveAccessRights(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static void WMifareWrite(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* const uint8_t * */,
            int nOffset /* uint32_t */,
            boolean bLockSectors,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WMyDGetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static void WMyDMoveAuthenticate(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int nPassword /* uint32_t */ );

   /* package protected */native static void WMyDMoveFreezeDataLockConfiguration(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static void WMyDMoveGetConfiguration(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWMyDMoveGetConfigurationCompleted * */ );

   /* package protected */native static void WMyDMoveSetConfiguration(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int nStatusByte /* uint8_t */,
            int nPasswordRetryCounter /* uint8_t */,
            int nPassword /* uint32_t */,
            boolean bLockConfiguration );

   /* package protected */native static void WMyDRead(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* uint8_t * */,
            int nOffset /* uint32_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WMyDWrite(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* const uint8_t * */,
            int nOffset /* uint32_t */,
            boolean bLockSectors,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WNDEFGetTagInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static void WNDEFReadMessage(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWNDEFReadMessageCompleted * */,
            int nTNF /* uint8_t */,
            String pTypeString,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WNDEFReadMessageOnAnyTag(
            int pCallback /* tWNDEFReadMessageCompleted * */,
            int nPriority /* uint8_t */,
            int nTNF /* uint8_t */,
            String pTypeString,
            int[] phRegistry /* W_HANDLE * */ );

   /* package protected */native static void WNFCControllerFirmwareUpdate(
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pUpdateBuffer /* const uint8_t * */,
            int nMode /* uint32_t */ );

   /* package protected */native static int WNFCControllerFirmwareUpdateState( );

   /* package protected */native static int WNFCControllerGetFirmwareProperty(
            byte[] pUpdateBuffer /* const uint8_t * */,
            int nPropertyIdentifier /* uint8_t */,
            char[] pValueBuffer /* tchar * */,
            int[] pnValueLength /* uint32_t * */ );

   /* package protected */native static int WNFCControllerGetMode( );

   /* package protected */native static int WNFCControllerGetProperty(
            int nPropertyIdentifier /* uint8_t */,
            char[] pValueBuffer /* tchar * */,
            int[] pnValueLength /* uint32_t * */ );

   /* package protected */native static void WNFCControllerGetRFActivity(
            int[] pnReaderState /* uint8_t * */,
            int[] pnCardState /* uint8_t * */,
            int[] pnP2PState /* uint8_t * */ );

   /* package protected */native static void WNFCControllerGetRFLock(
            int nLockSet /* uint32_t */,
            int[] pbReaderLock /* bool * */,
            int[] pbCardLock /* bool * */ );

   /* package protected */native static int WNFCControllerMonitorException(
            int pHandler /* tWBasicGenericEventHandler * */,
            int[] phEventRegistry /* W_HANDLE * */ );

   /* package protected */native static void WNFCControllerProductionTest(
            int pParameterBuffer /* const uint8_t * */,
            int pResultBuffer /* uint8_t * */,
            int pCallback /* tWBasicGenericDataCallbackFunction * */ );

   /* package protected */native static void WNFCControllerReset(
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int nMode /* uint32_t */ );

   /* package protected */native static void WNFCControllerSelfTest(
            int pCallback /* tWNFCControllerSelfTestCompleted * */ );

   /* package protected */native static void WNFCControllerSetRFLock(
            int nLockSet /* uint32_t */,
            boolean bReaderLock,
            boolean bCardLock,
            int pCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static int WNFCControllerSwitchStandbyMode(
            boolean bStandbyOn );

   /* package protected */native static void WP2PConnect(
            int hSocket /* W_HANDLE */,
            int hLink /* W_HANDLE */,
            int pEstablishmentCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static int WP2PCreateSocket(
            int nType /* uint8_t */,
            String pServiceURI,
            int nSAP /* uint8_t */,
            int[] phSocket /* W_HANDLE * */ );

   /* package protected */native static void WP2PEstablishLink(
            int pEstablishmentCallback /* tWBasicGenericHandleCallbackFunction * */,
            int pReleaseCallback /* tWBasicGenericCallbackFunction * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WP2PGetSocketParameter(
            int hSocket /* W_HANDLE */,
            int nParameter /* uint32_t */,
            int[] pnValue /* uint32_t * */ );

   /* package protected */native static void WP2PRead(
            int hSocket /* W_HANDLE */,
            int pCallback /* tWBasicGenericDataCallbackFunction * */,
            int pReceptionBuffer /* uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WP2PRecvFrom(
            int hSocket /* W_HANDLE */,
            int pCallback /* tWP2PRecvFromCompleted * */,
            int pReceptionBuffer /* uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WP2PSendTo(
            int hSocket /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int nSAP /* uint8_t */,
            int pSendBuffer /* const uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WP2PSetSocketParameter(
            int hSocket /* W_HANDLE */,
            int nParameter /* uint32_t */,
            int nValue /* uint32_t */ );

   /* package protected */native static void WP2PShutdown(
            int hSocket /* W_HANDLE */,
            int pReleaseCallback /* tWBasicGenericCallbackFunction * */ );

   /* package protected */native static void WP2PURILookup(
            int hLink /* W_HANDLE */,
            int pCallback /* tWP2PURILookupCompleted * */,
            String pServiceURI );

   /* package protected */native static void WP2PWrite(
            int hSocket /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pSendBuffer /* const uint8_t * */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WPicoGetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static int WPicoIsWritable(
            int hConnection /* W_HANDLE */ );

   /* package protected */native static void WPicoRead(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* uint8_t * */,
            int nOffset /* uint32_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WPicoWrite(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* const uint8_t * */,
            int nOffset /* uint32_t */,
            boolean bLockCard,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WReaderErrorEventRegister(
            int pHandler /* tWBasicGenericEventHandler * */,
            int nEventType /* uint8_t */,
            boolean bCardDetectionRequested,
            int[] phRegistryHandle /* W_HANDLE * */ );

   /* package protected */native static int WReaderGetPulsePeriod(
            int[] pnTimeout /* uint32_t * */ );

   /* package protected */native static void WReaderHandlerWorkPerformed(
            int hConnection /* W_HANDLE */,
            boolean bGiveToNextListener,
            boolean bCardApplicationMatch );

   /* package protected */native static boolean WReaderIsPropertySupported(
            int nPropertyIdentifier /* uint8_t */ );

   /* package protected */native static int WReaderListenToCardDetection(
            int pHandler /* tWReaderCardDetectionHandler * */,
            int nPriority /* uint8_t */,
            byte[] pConnectionPropertyArray /* const uint8_t * */,
            int[] phEventRegistry /* W_HANDLE * */ );

   /* package protected */native static boolean WReaderPreviousApplicationMatch(
            int hConnection /* W_HANDLE */ );

   /* package protected */native static void WReaderSetPulsePeriod(
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int nPulsePeriod /* uint32_t */ );

   /* package protected */native static int WSecurityAuthenticate(
            byte[] pApplicationDataBuffer /* const uint8_t * */ );

   /* package protected */native static int WType1ChipGetConnectionInfoBuffer(
            int hConnection /* W_HANDLE */,
            byte[] pInfoBuffer /* uint8_t * */ );

   /* package protected */native static int WType1ChipIsWritable(
            int hConnection /* W_HANDLE */ );

   /* package protected */native static void WType1ChipRead(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* uint8_t * */,
            int nOffset /* uint32_t */,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static void WType1ChipWrite(
            int hConnection /* W_HANDLE */,
            int pCallback /* tWBasicGenericCallbackFunction * */,
            int pBuffer /* const uint8_t * */,
            int nOffset /* uint32_t */,
            boolean bLockBlocks,
            int[] phOperation /* W_HANDLE * */ );

   /* package protected */native static int WVirtualTagCreate(
            int nTagType /* uint8_t */,
            byte[] pIdentifier /* const uint8_t * */,
            int nMaximumMessageLength /* uint32_t */,
            int[] phVirtualTag /* W_HANDLE * */ );

   /* package protected */native static void WVirtualTagStart(
            int hVirtualTag /* W_HANDLE */,
            int pCompletionCallback /* tWBasicGenericCallbackFunction * */,
            int pEventCallback /* tWBasicGenericEventHandler * */,
            boolean bReadOnly );

   /* package protected */native static void WVirtualTagStop(
            int hVirtualTag /* W_HANDLE */,
            int pCompletionCallback /* tWBasicGenericCallbackFunction * */ );

}

/* package protected */abstract class Callback_tWBasicGenericCallbackFunction extends AsynchronousCompletion {
   protected Callback_tWBasicGenericCallbackFunction(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWBasicGenericCallbackFunction(
         int nResult);

   protected void invokeFunction(int[] args) {
      tWBasicGenericCallbackFunction(args[1]);
   }
}

/* package protected */abstract class Callback_tWBasicGenericCompletionFunction extends AsynchronousCompletion {
   protected Callback_tWBasicGenericCompletionFunction(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWBasicGenericCompletionFunction();

   protected void invokeFunction(int[] args) {
      tWBasicGenericCompletionFunction();
   }
}

/* package protected */abstract class Callback_tWBasicGenericDataCallbackFunction extends AsynchronousCompletion {
   protected Callback_tWBasicGenericDataCallbackFunction(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWBasicGenericDataCallbackFunction(
         int nDataLength,
         int nResult);

   protected void invokeFunction(int[] args) {
      tWBasicGenericDataCallbackFunction(args[1], args[2]);
   }
}

/* package protected */abstract class Callback_tWBasicGenericEventHandler extends AsynchronousCompletion {
   protected Callback_tWBasicGenericEventHandler(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWBasicGenericEventHandler(
         int nEventCode);

   protected void invokeFunction(int[] args) {
      tWBasicGenericEventHandler(args[1]);
   }
}

/* package protected */abstract class Callback_tWBasicGenericEventHandler2 extends AsynchronousCompletion {
   protected Callback_tWBasicGenericEventHandler2(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWBasicGenericEventHandler2(
         int nEventCode1,
         int nEventCode2);

   protected void invokeFunction(int[] args) {
      tWBasicGenericEventHandler2(args[1], args[2]);
   }
}

/* package protected */abstract class Callback_tWBasicGenericHandleCallbackFunction extends AsynchronousCompletion {
   protected Callback_tWBasicGenericHandleCallbackFunction(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWBasicGenericHandleCallbackFunction(
         int hHandle,
         int nResult);

   protected void invokeFunction(int[] args) {
      tWBasicGenericHandleCallbackFunction(args[1], args[2]);
   }
}

/* package protected */abstract class Callback_tWEmulCommandReceived extends AsynchronousCompletion {
   protected Callback_tWEmulCommandReceived(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWEmulCommandReceived(
         int nDataLength);

   protected void invokeFunction(int[] args) {
      tWEmulCommandReceived(args[1]);
   }
}

/* package protected */abstract class Callback_tWMyDMoveGetConfigurationCompleted extends AsynchronousCompletion {
   protected Callback_tWMyDMoveGetConfigurationCompleted(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWMyDMoveGetConfigurationCompleted(
         int nError,
         int nStatusByte,
         int nPasswordRetryCounter);

   protected void invokeFunction(int[] args) {
      tWMyDMoveGetConfigurationCompleted(args[1], args[2], args[3]);
   }
}

/* package protected */abstract class Callback_tWNDEFReadMessageCompleted extends AsynchronousCompletion {
   protected Callback_tWNDEFReadMessageCompleted(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWNDEFReadMessageCompleted(
         int hMessage,
         int nError);

   protected void invokeFunction(int[] args) {
      tWNDEFReadMessageCompleted(args[1], args[2]);
   }
}

/* package protected */abstract class Callback_tWNFCControllerSelfTestCompleted extends AsynchronousCompletion {
   protected Callback_tWNFCControllerSelfTestCompleted(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWNFCControllerSelfTestCompleted(
         int nError,
         int nResult);

   protected void invokeFunction(int[] args) {
      tWNFCControllerSelfTestCompleted(args[1], args[2]);
   }
}

/* package protected */abstract class Callback_tWP2PRecvFromCompleted extends AsynchronousCompletion {
   protected Callback_tWP2PRecvFromCompleted(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWP2PRecvFromCompleted(
         int nDataLength,
         int nError,
         int nSAP);

   protected void invokeFunction(int[] args) {
      tWP2PRecvFromCompleted(args[1], args[2], args[3]);
   }
}

/* package protected */abstract class Callback_tWP2PURILookupCompleted extends AsynchronousCompletion {
   protected Callback_tWP2PURILookupCompleted(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWP2PURILookupCompleted(
         int nDSAP,
         int nError);

   protected void invokeFunction(int[] args) {
      tWP2PURILookupCompleted(args[1], args[2]);
   }
}

/* package protected */abstract class Callback_tWReaderCardDetectionHandler extends AsynchronousCompletion {
   protected Callback_tWReaderCardDetectionHandler(boolean isEvent) {
      super(isEvent);
   }

   protected abstract void tWReaderCardDetectionHandler(
         int hConnection,
         int nError);

   protected void invokeFunction(int[] args) {
      tWReaderCardDetectionHandler(args[1], args[2]);
   }
}


/* End of file */
