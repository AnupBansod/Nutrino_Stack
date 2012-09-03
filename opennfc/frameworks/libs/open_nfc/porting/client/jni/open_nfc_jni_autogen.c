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


#include "open_nfc.h"
#include "open_nfc_jni_autogen.h"
#include "open_nfc_jni.h"


/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W14Part3ExchangeData(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericDataCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pReaderToCardBuffer,
 *               ( uint8_t * ) int pCardToReaderBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W14Part3ExchangeData
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pReaderToCardBuffer_jni,
   jint pCardToReaderBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   W14Part3ExchangeData(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericDataCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pReaderToCardBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pReaderToCardBuffer_jni),
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pCardToReaderBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pCardToReaderBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W14Part3GetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W14Part3GetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)W14Part3GetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W14Part3ListenToCardDetectionTypeB(
 *               ( tWReaderCardDetectionHandler * ) int pHandler,
 *               ( uint8_t ) int nPriority,
 *               ( uint8_t ) int nAFI,
 *               boolean bUseCID,
 *               ( uint8_t ) int nCID,
 *               ( uint32_t ) int nBaudRate,
 *               ( const uint8_t * ) byte[] pHigherLayerDataBuffer,
 *               ( W_HANDLE * ) int[] phEventRegistry )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W14Part3ListenToCardDetectionTypeB
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jint nPriority_jni,
   jint nAFI_jni,
   jboolean bUseCID_jni,
   jint nCID_jni,
   jint nBaudRate_jni,
   jbyteArray pHigherLayerDataBuffer_jni,
   jintArray phEventRegistry_jni ) {
   jint result;
   jbyte* pHigherLayerDataBuffer = WJavaGetByteArrayElements(env, pHigherLayerDataBuffer_jni);
   W_HANDLE phEventRegistry_value = (W_HANDLE)0;

   result = (jint)W14Part3ListenToCardDetectionTypeB(
        (tWReaderCardDetectionHandler *)pHandler_jni, null,
        (uint8_t)nPriority_jni,
        (uint8_t)nAFI_jni,
        (bUseCID_jni == JNI_FALSE)?false:true,
        (uint8_t)nCID_jni,
        (uint32_t)nBaudRate_jni,
        (const uint8_t *)pHigherLayerDataBuffer,
        (uint8_t)WJavaGetArrayLength(env, pHigherLayerDataBuffer_jni),
        (phEventRegistry_jni != null)?(&phEventRegistry_value):null );

   WJavaReleaseByteArrayElements(env, pHigherLayerDataBuffer_jni, pHigherLayerDataBuffer, false);
   WJavaSetIntArrayFirstElement(env, phEventRegistry_jni, phEventRegistry_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W14Part3SetTimeout(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint32_t ) int nTimeout )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W14Part3SetTimeout
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint nTimeout_jni ) {
   jint result;

   result = (jint)W14Part3SetTimeout(
        (W_HANDLE)hConnection_jni,
        (uint32_t)nTimeout_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W14Part4ExchangeData(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericDataCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pReaderToCardBuffer,
 *               ( uint8_t * ) int pCardToReaderBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W14Part4ExchangeData
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pReaderToCardBuffer_jni,
   jint pCardToReaderBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   W14Part4ExchangeData(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericDataCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pReaderToCardBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pReaderToCardBuffer_jni),
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pCardToReaderBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pCardToReaderBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W14Part4GetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W14Part4GetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)W14Part4GetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W14Part4ListenToCardDetectionTypeA(
 *               ( tWReaderCardDetectionHandler * ) int pHandler,
 *               ( uint8_t ) int nPriority,
 *               boolean bUseCID,
 *               ( uint8_t ) int nCID,
 *               ( uint32_t ) int nBaudRate,
 *               ( W_HANDLE * ) int[] phEventRegistry )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W14Part4ListenToCardDetectionTypeA
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jint nPriority_jni,
   jboolean bUseCID_jni,
   jint nCID_jni,
   jint nBaudRate_jni,
   jintArray phEventRegistry_jni ) {
   jint result;
   W_HANDLE phEventRegistry_value = (W_HANDLE)0;

   result = (jint)W14Part4ListenToCardDetectionTypeA(
        (tWReaderCardDetectionHandler *)pHandler_jni, null,
        (uint8_t)nPriority_jni,
        (bUseCID_jni == JNI_FALSE)?false:true,
        (uint8_t)nCID_jni,
        (uint32_t)nBaudRate_jni,
        (phEventRegistry_jni != null)?(&phEventRegistry_value):null );

   WJavaSetIntArrayFirstElement(env, phEventRegistry_jni, phEventRegistry_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W14Part4ListenToCardDetectionTypeB(
 *               ( tWReaderCardDetectionHandler * ) int pHandler,
 *               ( uint8_t ) int nPriority,
 *               ( uint8_t ) int nAFI,
 *               boolean bUseCID,
 *               ( uint8_t ) int nCID,
 *               ( uint32_t ) int nBaudRate,
 *               ( const uint8_t * ) byte[] pHigherLayerDataBuffer,
 *               ( W_HANDLE * ) int[] phEventRegistry )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W14Part4ListenToCardDetectionTypeB
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jint nPriority_jni,
   jint nAFI_jni,
   jboolean bUseCID_jni,
   jint nCID_jni,
   jint nBaudRate_jni,
   jbyteArray pHigherLayerDataBuffer_jni,
   jintArray phEventRegistry_jni ) {
   jint result;
   jbyte* pHigherLayerDataBuffer = WJavaGetByteArrayElements(env, pHigherLayerDataBuffer_jni);
   W_HANDLE phEventRegistry_value = (W_HANDLE)0;

   result = (jint)W14Part4ListenToCardDetectionTypeB(
        (tWReaderCardDetectionHandler *)pHandler_jni, null,
        (uint8_t)nPriority_jni,
        (uint8_t)nAFI_jni,
        (bUseCID_jni == JNI_FALSE)?false:true,
        (uint8_t)nCID_jni,
        (uint32_t)nBaudRate_jni,
        (const uint8_t *)pHigherLayerDataBuffer,
        (uint8_t)WJavaGetArrayLength(env, pHigherLayerDataBuffer_jni),
        (phEventRegistry_jni != null)?(&phEventRegistry_value):null );

   WJavaReleaseByteArrayElements(env, pHigherLayerDataBuffer_jni, pHigherLayerDataBuffer, false);
   WJavaSetIntArrayFirstElement(env, phEventRegistry_jni, phEventRegistry_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W14Part4SetNAD(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t ) int nNAD )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W14Part4SetNAD
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint nNAD_jni ) {
   jint result;

   result = (jint)W14Part4SetNAD(
        (W_HANDLE)hConnection_jni,
        (uint8_t)nNAD_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W15GetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W15GetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)W15GetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W15IsWritable(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t ) int nSectorIndex )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W15IsWritable
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint nSectorIndex_jni ) {
   jint result;

   result = (jint)W15IsWritable(
        (W_HANDLE)hConnection_jni,
        (uint8_t)nSectorIndex_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W15ListenToCardDetection(
 *               ( tWReaderCardDetectionHandler * ) int pHandler,
 *               ( uint8_t ) int nPriority,
 *               ( uint8_t ) int nAFI,
 *               ( W_HANDLE * ) int[] phEventRegistry )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W15ListenToCardDetection
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jint nPriority_jni,
   jint nAFI_jni,
   jintArray phEventRegistry_jni ) {
   jint result;
   W_HANDLE phEventRegistry_value = (W_HANDLE)0;

   result = (jint)W15ListenToCardDetection(
        (tWReaderCardDetectionHandler *)pHandler_jni, null,
        (uint8_t)nPriority_jni,
        (uint8_t)nAFI_jni,
        (phEventRegistry_jni != null)?(&phEventRegistry_value):null );

   WJavaSetIntArrayFirstElement(env, phEventRegistry_jni, phEventRegistry_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W15Read(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W15Read
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   W15Read(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W15SetAttribute(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t ) int nActions,
 *               ( uint8_t ) int nAFI,
 *               ( uint8_t ) int nDSFID,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W15SetAttribute
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint nActions_jni,
   jint nAFI_jni,
   jint nDSFID_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   W15SetAttribute(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t)nActions_jni,
        (uint8_t)nAFI_jni,
        (uint8_t)nDSFID_jni,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W15SetTagSize(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint16_t ) int nSectorNumber,
 *               ( uint8_t ) int nSectorSize )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W15SetTagSize
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint nSectorNumber_jni,
   jint nSectorSize_jni ) {
   jint result;

   result = (jint)W15SetTagSize(
        (W_HANDLE)hConnection_jni,
        (uint16_t)nSectorNumber_jni,
        (uint8_t)nSectorSize_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W15Write(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               boolean bLockSectors,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W15Write
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jboolean bLockSectors_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   W15Write(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (bLockSectors_jni == JNI_FALSE)?false:true,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W7816CloseLogicalChannel(
 *               ( W_HANDLE ) int hLogicalChannel,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W7816CloseLogicalChannel
   (JNIEnv *env, jclass clazz,
   jint hLogicalChannel_jni,
   jint pCallback_jni ) {

   W7816CloseLogicalChannel(
        (W_HANDLE)hLogicalChannel_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W7816ExchangeAPDU(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericDataCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pSendAPDUBuffer,
 *               ( uint8_t * ) int pReceivedAPDUBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W7816ExchangeAPDU
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pSendAPDUBuffer_jni,
   jint pReceivedAPDUBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   W7816ExchangeAPDU(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericDataCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pSendAPDUBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pSendAPDUBuffer_jni),
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pReceivedAPDUBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pReceivedAPDUBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W7816GetATR(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pBuffer,
 *               ( uint32_t * ) int[] pnActualLength )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W7816GetATR
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pBuffer_jni,
   jintArray pnActualLength_jni ) {
   jint result;
   jbyte* pBuffer = WJavaGetByteArrayElements(env, pBuffer_jni);
   uint32_t pnActualLength_value = (uint32_t)0;

   result = (jint)W7816GetATR(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pBuffer,
        (uint32_t)WJavaGetArrayLength(env, pBuffer_jni),
        (pnActualLength_jni != null)?(&pnActualLength_value):null );

   WJavaReleaseByteArrayElements(env, pBuffer_jni, pBuffer, true);
   WJavaSetIntArrayFirstElement(env, pnActualLength_jni, pnActualLength_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W7816GetATRSize(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint32_t * ) int[] pnSize )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W7816GetATRSize
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jintArray pnSize_jni ) {
   jint result;
   uint32_t pnSize_value = (uint32_t)0;

   result = (jint)W7816GetATRSize(
        (W_HANDLE)hConnection_jni,
        (pnSize_jni != null)?(&pnSize_value):null );

   WJavaSetIntArrayFirstElement(env, pnSize_jni, pnSize_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int W7816GetResponseAPDUData(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pReceivedAPDUBuffer,
 *               ( uint32_t * ) int[] pnReceivedAPDUActualLength )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_W7816GetResponseAPDUData
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pReceivedAPDUBuffer_jni,
   jintArray pnReceivedAPDUActualLength_jni ) {
   jint result;
   jbyte* pReceivedAPDUBuffer = WJavaGetByteArrayElements(env, pReceivedAPDUBuffer_jni);
   uint32_t pnReceivedAPDUActualLength_value = (uint32_t)0;

   result = (jint)W7816GetResponseAPDUData(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pReceivedAPDUBuffer,
        (uint32_t)WJavaGetArrayLength(env, pReceivedAPDUBuffer_jni),
        (pnReceivedAPDUActualLength_jni != null)?(&pnReceivedAPDUActualLength_value):null );

   WJavaReleaseByteArrayElements(env, pReceivedAPDUBuffer_jni, pReceivedAPDUBuffer, true);
   WJavaSetIntArrayFirstElement(env, pnReceivedAPDUActualLength_jni, pnReceivedAPDUActualLength_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void W7816OpenLogicalChannel(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericHandleCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pAID,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_W7816OpenLogicalChannel
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pAID_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   W7816OpenLogicalChannel(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericHandleCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pAID_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pAID_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WBasicCancelOperation(
 *               ( W_HANDLE ) int hOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WBasicCancelOperation
   (JNIEnv *env, jclass clazz,
   jint hOperation_jni ) {

   WBasicCancelOperation(
        (W_HANDLE)hOperation_jni );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBasicCheckConnectionProperty(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t ) int nPropertyIdentifier )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBasicCheckConnectionProperty
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint nPropertyIdentifier_jni ) {
   jint result;

   result = (jint)WBasicCheckConnectionProperty(
        (W_HANDLE)hConnection_jni,
        (uint8_t)nPropertyIdentifier_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WBasicCloseHandle(
 *               ( W_HANDLE ) int hHandle )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WBasicCloseHandle
   (JNIEnv *env, jclass clazz,
   jint hHandle_jni ) {

   WBasicCloseHandle(
        (W_HANDLE)hHandle_jni );

}

/*
 * Class:     org.opennfc.nfctag.MethodAutogen
 *
 * Method:    void WBasicCloseHandle(
 *               ( W_HANDLE ) int hHandle )
 */
JNIEXPORT void JNICALL Java_org_opennfc_nfctag_MethodAutogen_WBasicCloseHandle
   (JNIEnv *env, jclass clazz,
   jint hHandle_jni ) {

   WBasicCloseHandle(
        (W_HANDLE)hHandle_jni );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WBasicCloseHandleSafe(
 *               ( W_HANDLE ) int hHandle,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WBasicCloseHandleSafe
   (JNIEnv *env, jclass clazz,
   jint hHandle_jni,
   jint pCallback_jni ) {

   WBasicCloseHandleSafe(
        (W_HANDLE)hHandle_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBasicGetConnectionProperties(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pPropertyArray )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBasicGetConnectionProperties
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pPropertyArray_jni ) {
   jint result;
   jbyte* pPropertyArray = WJavaGetByteArrayElements(env, pPropertyArray_jni);

   result = (jint)WBasicGetConnectionProperties(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pPropertyArray,
        (uint32_t)WJavaGetArrayLength(env, pPropertyArray_jni) );

   WJavaReleaseByteArrayElements(env, pPropertyArray_jni, pPropertyArray, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBasicGetConnectionPropertyNumber(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint32_t * ) int[] pnPropertyNumber )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBasicGetConnectionPropertyNumber
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jintArray pnPropertyNumber_jni ) {
   jint result;
   uint32_t pnPropertyNumber_value = (uint32_t)0;

   result = (jint)WBasicGetConnectionPropertyNumber(
        (W_HANDLE)hConnection_jni,
        (pnPropertyNumber_jni != null)?(&pnPropertyNumber_value):null );

   WJavaSetIntArrayFirstElement(env, pnPropertyNumber_jni, pnPropertyNumber_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBasicInit(
 *               String pVersionString )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBasicInit
   (JNIEnv *env, jclass clazz,
   jstring pVersionString_jni ) {
   jint result;
   const tchar* pVersionString = WJavaGetStringContent(env, pVersionString_jni);

   result = (jint)WBasicInit(
        pVersionString );

   WJavaReleaseStringContent(pVersionString);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBasicPumpEvent(
 *               boolean bWait )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBasicPumpEvent
   (JNIEnv *env, jclass clazz,
   jboolean bWait_jni ) {
   jint result;

   result = (jint)WBasicPumpEvent(
        (bWait_jni == JNI_FALSE)?false:true );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WBasicStopEventLoop( )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WBasicStopEventLoop
   (JNIEnv *env, jclass clazz ) {

   WBasicStopEventLoop(
 );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WBasicTerminate( )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WBasicTerminate
   (JNIEnv *env, jclass clazz ) {

   WBasicTerminate(
 );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WBPrimeExchangeData(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericDataCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pReaderToCardBuffer,
 *               ( uint8_t * ) int pCardToReaderBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WBPrimeExchangeData
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pReaderToCardBuffer_jni,
   jint pCardToReaderBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WBPrimeExchangeData(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericDataCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pReaderToCardBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pReaderToCardBuffer_jni),
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pCardToReaderBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pCardToReaderBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBPrimeGetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBPrimeGetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WBPrimeGetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBPrimeListenToCardDetection(
 *               ( tWReaderCardDetectionHandler * ) int pHandler,
 *               ( uint8_t ) int nPriority,
 *               ( const uint8_t * ) byte[] pAPGENBuffer,
 *               ( W_HANDLE * ) int[] phEventRegistry )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBPrimeListenToCardDetection
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jint nPriority_jni,
   jbyteArray pAPGENBuffer_jni,
   jintArray phEventRegistry_jni ) {
   jint result;
   jbyte* pAPGENBuffer = WJavaGetByteArrayElements(env, pAPGENBuffer_jni);
   W_HANDLE phEventRegistry_value = (W_HANDLE)0;

   result = (jint)WBPrimeListenToCardDetection(
        (tWReaderCardDetectionHandler *)pHandler_jni, null,
        (uint8_t)nPriority_jni,
        (const uint8_t *)pAPGENBuffer,
        (uint8_t)WJavaGetArrayLength(env, pAPGENBuffer_jni),
        (phEventRegistry_jni != null)?(&phEventRegistry_value):null );

   WJavaReleaseByteArrayElements(env, pAPGENBuffer_jni, pAPGENBuffer, false);
   WJavaSetIntArrayFirstElement(env, phEventRegistry_jni, phEventRegistry_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WBPrimeSetTimeout(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint32_t ) int nTimeout )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WBPrimeSetTimeout
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint nTimeout_jni ) {
   jint result;

   result = (jint)WBPrimeSetTimeout(
        (W_HANDLE)hConnection_jni,
        (uint32_t)nTimeout_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( bool ) boolean WDFCPumpJNICallback(
 *               ( uint32_t * ) int[] pArgs )
 */
JNIEXPORT jboolean JNICALL Java_org_opennfc_MethodAutogen_WDFCPumpJNICallback
   (JNIEnv *env, jclass clazz,
   jintArray pArgs_jni ) {
   jboolean result;
   jint* pArgs = WJavaGetIntArrayElements(env, pArgs_jni);

   result = (jboolean)WDFCPumpJNICallback(
        (uint32_t *)pArgs,
        (uint32_t)WJavaGetArrayLength(env, pArgs_jni) );

   WJavaReleaseIntArrayElements(env, pArgs_jni, pArgs, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WEmulClose(
 *               ( W_HANDLE ) int hHandle,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WEmulClose
   (JNIEnv *env, jclass clazz,
   jint hHandle_jni,
   jint pCallback_jni ) {

   WEmulClose(
        (W_HANDLE)hHandle_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WEmulGetMessageData(
 *               ( W_HANDLE ) int hHandle,
 *               ( uint8_t * ) byte[] pDataBuffer,
 *               ( uint32_t * ) int[] pnActualDataLength )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WEmulGetMessageData
   (JNIEnv *env, jclass clazz,
   jint hHandle_jni,
   jbyteArray pDataBuffer_jni,
   jintArray pnActualDataLength_jni ) {
   jint result;
   jbyte* pDataBuffer = WJavaGetByteArrayElements(env, pDataBuffer_jni);
   uint32_t pnActualDataLength_value = (uint32_t)0;

   result = (jint)WEmulGetMessageData(
        (W_HANDLE)hHandle_jni,
        (uint8_t *)pDataBuffer,
        (uint32_t)WJavaGetArrayLength(env, pDataBuffer_jni),
        (pnActualDataLength_jni != null)?(&pnActualDataLength_value):null );

   WJavaReleaseByteArrayElements(env, pDataBuffer_jni, pDataBuffer, true);
   WJavaSetIntArrayFirstElement(env, pnActualDataLength_jni, pnActualDataLength_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( bool ) boolean WEmulIsPropertySupported(
 *               ( uint8_t ) int nPropertyIdentifier )
 */
JNIEXPORT jboolean JNICALL Java_org_opennfc_MethodAutogen_WEmulIsPropertySupported
   (JNIEnv *env, jclass clazz,
   jint nPropertyIdentifier_jni ) {
   jboolean result;

   result = (jboolean)WEmulIsPropertySupported(
        (uint8_t)nPropertyIdentifier_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WEmulOpenConnectionBuffer(
 *               ( tWBasicGenericCallbackFunction * ) int pOpenCallback,
 *               ( tWBasicGenericEventHandler * ) int pEventCallback,
 *               ( tWEmulCommandReceived * ) int pCommandCallback,
 *               ( uint32_t ) int nCardType,
 *               ( uint8_t * ) byte[] pUID,
 *               ( uint32_t ) int nRandomIdentifierLength,
 *               ( W_HANDLE * ) int[] phHandle )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WEmulOpenConnectionBuffer
   (JNIEnv *env, jclass clazz,
   jint pOpenCallback_jni,
   jint pEventCallback_jni,
   jint pCommandCallback_jni,
   jint nCardType_jni,
   jbyteArray pUID_jni,
   jint nRandomIdentifierLength_jni,
   jintArray phHandle_jni ) {
   jbyte* pUID = WJavaGetByteArrayElements(env, pUID_jni);
   W_HANDLE phHandle_value = (W_HANDLE)0;

   WEmulOpenConnectionBuffer(
        (tWBasicGenericCallbackFunction *)pOpenCallback_jni, null,
        (tWBasicGenericEventHandler *)pEventCallback_jni, null,
        (tWEmulCommandReceived *)pCommandCallback_jni, null,
        (uint32_t)nCardType_jni,
        (uint8_t *)pUID,
        (uint32_t)WJavaGetArrayLength(env, pUID_jni),
        (uint32_t)nRandomIdentifierLength_jni,
        (phHandle_jni != null)?(&phHandle_value):null );

   WJavaReleaseByteArrayElements(env, pUID_jni, pUID, false);
   WJavaSetIntArrayFirstElement(env, phHandle_jni, phHandle_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WEmulSendAnswer(
 *               ( W_HANDLE ) int hDriverConnection,
 *               ( const uint8_t * ) byte[] pDataBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WEmulSendAnswer
   (JNIEnv *env, jclass clazz,
   jint hDriverConnection_jni,
   jbyteArray pDataBuffer_jni ) {
   jint result;
   jbyte* pDataBuffer = WJavaGetByteArrayElements(env, pDataBuffer_jni);

   result = (jint)WEmulSendAnswer(
        (W_HANDLE)hDriverConnection_jni,
        (const uint8_t *)pDataBuffer,
        (uint32_t)WJavaGetArrayLength(env, pDataBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pDataBuffer_jni, pDataBuffer, false);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WFeliCaGetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WFeliCaGetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WFeliCaGetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WFeliCaReadSimple(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pBuffer,
 *               ( uint32_t * ) int[] pServiceCodeList,
 *               ( const uint8_t * ) byte[] pBlockList )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WFeliCaReadSimple
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jintArray pServiceCodeList_jni,
   jbyteArray pBlockList_jni ) {
   jint* pServiceCodeList = WJavaGetIntArrayElements(env, pServiceCodeList_jni);
   jbyte* pBlockList = WJavaGetByteArrayElements(env, pBlockList_jni);

   WFeliCaReadSimple(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (uint32_t)WJavaGetArrayLength(env, pServiceCodeList_jni),
        (uint32_t *)pServiceCodeList,
        (uint32_t)WJavaGetArrayLength(env, pBlockList_jni),
        (const uint8_t *)pBlockList );

   WJavaReleaseIntArrayElements(env, pServiceCodeList_jni, pServiceCodeList, false);
   WJavaReleaseByteArrayElements(env, pBlockList_jni, pBlockList, false);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WFeliCaWriteSimple(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pBuffer,
 *               ( uint32_t * ) int[] pServiceCodeList,
 *               ( const uint8_t * ) byte[] pBlockList )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WFeliCaWriteSimple
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jintArray pServiceCodeList_jni,
   jbyteArray pBlockList_jni ) {
   jint* pServiceCodeList = WJavaGetIntArrayElements(env, pServiceCodeList_jni);
   jbyte* pBlockList = WJavaGetByteArrayElements(env, pBlockList_jni);

   WFeliCaWriteSimple(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (uint32_t)WJavaGetArrayLength(env, pServiceCodeList_jni),
        (uint32_t *)pServiceCodeList,
        (uint32_t)WJavaGetArrayLength(env, pBlockList_jni),
        (const uint8_t *)pBlockList );

   WJavaReleaseIntArrayElements(env, pServiceCodeList_jni, pServiceCodeList, false);
   WJavaReleaseByteArrayElements(env, pBlockList_jni, pBlockList, false);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_HANDLE ) int WJavaCreateByteBuffer(
 *               ( uint8_t * ) byte[] pJavaBuffer,
 *               ( uint32_t ) int nOffset,
 *               ( uint32_t ) int nLength )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WJavaCreateByteBuffer
   (JNIEnv *env, jclass clazz,
   jbyteArray pJavaBuffer_jni,
   jint nOffset_jni,
   jint nLength_jni ) {
   jint result;
   jbyte* pJavaBuffer = WJavaGetByteArrayElements(env, pJavaBuffer_jni);

   result = (jint)WJavaCreateByteBuffer(
        (uint8_t *)pJavaBuffer,
        (uint32_t)nOffset_jni,
        (uint32_t)nLength_jni );

   WJavaReleaseByteArrayElements(env, pJavaBuffer_jni, pJavaBuffer, false);
   return result;
}

/*
 * Class:     org.opennfc.nfctag.MethodAutogen
 *
 * Method:    ( uint32_t ) int WJavaNDEFGetMessageContent(
 *               ( W_HANDLE ) int hMessage,
 *               ( uint8_t * ) byte[] pMessageBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_nfctag_MethodAutogen_WJavaNDEFGetMessageContent
   (JNIEnv *env, jclass clazz,
   jint hMessage_jni,
   jbyteArray pMessageBuffer_jni ) {
   jint result;
   jbyte* pMessageBuffer = WJavaGetByteArrayElements(env, pMessageBuffer_jni);

   result = (jint)WJavaNDEFGetMessageContent(
        (W_HANDLE)hMessage_jni,
        (uint8_t *)pMessageBuffer,
        (uint32_t)WJavaGetArrayLength(env, pMessageBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pMessageBuffer_jni, pMessageBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WJavaNDEFWriteMessage(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pMessageBuffer,
 *               ( uint32_t ) int nActionMask,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WJavaNDEFWriteMessage
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pMessageBuffer_jni,
   jint nActionMask_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WJavaNDEFWriteMessage(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pMessageBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pMessageBuffer_jni),
        (uint32_t)nActionMask_jni,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WJavaNDEFWriteMessageOnAnyTag(
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t ) int nPriority,
 *               ( uint8_t * ) int pMessageBuffer,
 *               ( uint32_t ) int nActionMask,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WJavaNDEFWriteMessageOnAnyTag
   (JNIEnv *env, jclass clazz,
   jint pCallback_jni,
   jint nPriority_jni,
   jint pMessageBuffer_jni,
   jint nActionMask_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WJavaNDEFWriteMessageOnAnyTag(
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t)nPriority_jni,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pMessageBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pMessageBuffer_jni),
        (uint32_t)nActionMask_jni,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( bool ) boolean WJavaNFCControllerGetBooleanProperty(
 *               ( uint8_t ) int nPropertyIdentifier )
 */
JNIEXPORT jboolean JNICALL Java_org_opennfc_MethodAutogen_WJavaNFCControllerGetBooleanProperty
   (JNIEnv *env, jclass clazz,
   jint nPropertyIdentifier_jni ) {
   jboolean result;

   result = (jboolean)WJavaNFCControllerGetBooleanProperty(
        (uint8_t)nPropertyIdentifier_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WJavaP2PGetConfigurationBuffer(
 *               ( uint8_t * ) byte[] pConfigurationBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WJavaP2PGetConfigurationBuffer
   (JNIEnv *env, jclass clazz,
   jbyteArray pConfigurationBuffer_jni ) {
   jint result;
   jbyte* pConfigurationBuffer = WJavaGetByteArrayElements(env, pConfigurationBuffer_jni);

   result = (jint)WJavaP2PGetConfigurationBuffer(
        (uint8_t *)pConfigurationBuffer,
        (uint32_t)WJavaGetArrayLength(env, pConfigurationBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pConfigurationBuffer_jni, pConfigurationBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WJavaP2PGetLinkPropertiesBuffer(
 *               ( W_HANDLE ) int hLink,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WJavaP2PGetLinkPropertiesBuffer
   (JNIEnv *env, jclass clazz,
   jint hLink_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WJavaP2PGetLinkPropertiesBuffer(
        (W_HANDLE)hLink_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WJavaP2PSetConfigurationBuffer(
 *               ( uint8_t * ) byte[] pConfigurationBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WJavaP2PSetConfigurationBuffer
   (JNIEnv *env, jclass clazz,
   jbyteArray pConfigurationBuffer_jni ) {
   jint result;
   jbyte* pConfigurationBuffer = WJavaGetByteArrayElements(env, pConfigurationBuffer_jni);

   result = (jint)WJavaP2PSetConfigurationBuffer(
        (uint8_t *)pConfigurationBuffer,
        (uint32_t)WJavaGetArrayLength(env, pConfigurationBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pConfigurationBuffer_jni, pConfigurationBuffer, false);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WJavaReleaseByteBuffer(
 *               ( W_HANDLE ) int hBufferReference,
 *               ( uint8_t * ) byte[] pJavaBuffer )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WJavaReleaseByteBuffer
   (JNIEnv *env, jclass clazz,
   jint hBufferReference_jni,
   jbyteArray pJavaBuffer_jni ) {
   jbyte* pJavaBuffer = WJavaGetByteArrayElements(env, pJavaBuffer_jni);

   WJavaReleaseByteBuffer(
        (W_HANDLE)hBufferReference_jni,
        (uint8_t *)pJavaBuffer,
        (uint32_t)WJavaGetArrayLength(env, pJavaBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pJavaBuffer_jni, pJavaBuffer, true);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WMifareGetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WMifareGetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WMifareGetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMifareRead(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMifareRead
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WMifareRead(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMifareULCAuthenticate(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) byte[] pKey )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMifareULCAuthenticate
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jbyteArray pKey_jni ) {
   jbyte* pKey = WJavaGetByteArrayElements(env, pKey_jni);

   WMifareULCAuthenticate(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)pKey,
        (uint32_t)WJavaGetArrayLength(env, pKey_jni) );

   WJavaReleaseByteArrayElements(env, pKey_jni, pKey, false);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMifareULCSetAccessRights(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) byte[] pKey,
 *               ( uint8_t ) int nThreshold,
 *               ( uint32_t ) int nRights,
 *               boolean bLockConfiguration )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMifareULCSetAccessRights
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jbyteArray pKey_jni,
   jint nThreshold_jni,
   jint nRights_jni,
   jboolean bLockConfiguration_jni ) {
   jbyte* pKey = WJavaGetByteArrayElements(env, pKey_jni);

   WMifareULCSetAccessRights(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)pKey,
        (uint32_t)WJavaGetArrayLength(env, pKey_jni),
        (uint8_t)nThreshold_jni,
        (uint32_t)nRights_jni,
        (bLockConfiguration_jni == JNI_FALSE)?false:true );

   WJavaReleaseByteArrayElements(env, pKey_jni, pKey, false);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WMifareULForceULC(
 *               ( W_HANDLE ) int hConnection )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WMifareULForceULC
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni ) {
   jint result;

   result = (jint)WMifareULForceULC(
        (W_HANDLE)hConnection_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMifareULFreezeDataLockConfiguration(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMifareULFreezeDataLockConfiguration
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni ) {

   WMifareULFreezeDataLockConfiguration(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WMifareULGetAccessRights(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint32_t ) int nOffset,
 *               ( uint32_t ) int nLength,
 *               ( uint32_t * ) int[] pnRights )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WMifareULGetAccessRights
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint nOffset_jni,
   jint nLength_jni,
   jintArray pnRights_jni ) {
   jint result;
   uint32_t pnRights_value = (uint32_t)0;

   result = (jint)WMifareULGetAccessRights(
        (W_HANDLE)hConnection_jni,
        (uint32_t)nOffset_jni,
        (uint32_t)nLength_jni,
        (pnRights_jni != null)?(&pnRights_value):null );

   WJavaSetIntArrayFirstElement(env, pnRights_jni, pnRights_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMifareULRetrieveAccessRights(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMifareULRetrieveAccessRights
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni ) {

   WMifareULRetrieveAccessRights(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMifareWrite(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               boolean bLockSectors,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMifareWrite
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jboolean bLockSectors_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WMifareWrite(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (bLockSectors_jni == JNI_FALSE)?false:true,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WMyDGetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WMyDGetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WMyDGetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMyDMoveAuthenticate(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint32_t ) int nPassword )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMyDMoveAuthenticate
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint nPassword_jni ) {

   WMyDMoveAuthenticate(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint32_t)nPassword_jni );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMyDMoveFreezeDataLockConfiguration(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMyDMoveFreezeDataLockConfiguration
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni ) {

   WMyDMoveFreezeDataLockConfiguration(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMyDMoveGetConfiguration(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWMyDMoveGetConfigurationCompleted * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMyDMoveGetConfiguration
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni ) {

   WMyDMoveGetConfiguration(
        (W_HANDLE)hConnection_jni,
        (tWMyDMoveGetConfigurationCompleted *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMyDMoveSetConfiguration(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t ) int nStatusByte,
 *               ( uint8_t ) int nPasswordRetryCounter,
 *               ( uint32_t ) int nPassword,
 *               boolean bLockConfiguration )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMyDMoveSetConfiguration
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint nStatusByte_jni,
   jint nPasswordRetryCounter_jni,
   jint nPassword_jni,
   jboolean bLockConfiguration_jni ) {

   WMyDMoveSetConfiguration(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t)nStatusByte_jni,
        (uint8_t)nPasswordRetryCounter_jni,
        (uint32_t)nPassword_jni,
        (bLockConfiguration_jni == JNI_FALSE)?false:true );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMyDRead(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMyDRead
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WMyDRead(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WMyDWrite(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               boolean bLockSectors,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WMyDWrite
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jboolean bLockSectors_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WMyDWrite(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (bLockSectors_jni == JNI_FALSE)?false:true,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WNDEFGetTagInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WNDEFGetTagInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WNDEFGetTagInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNDEFReadMessage(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWNDEFReadMessageCompleted * ) int pCallback,
 *               ( uint8_t ) int nTNF,
 *               String pTypeString,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNDEFReadMessage
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint nTNF_jni,
   jstring pTypeString_jni,
   jintArray phOperation_jni ) {
   const tchar* pTypeString = WJavaGetStringContent(env, pTypeString_jni);
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WNDEFReadMessage(
        (W_HANDLE)hConnection_jni,
        (tWNDEFReadMessageCompleted *)pCallback_jni, null,
        (uint8_t)nTNF_jni,
        pTypeString,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaReleaseStringContent(pTypeString);
   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNDEFReadMessageOnAnyTag(
 *               ( tWNDEFReadMessageCompleted * ) int pCallback,
 *               ( uint8_t ) int nPriority,
 *               ( uint8_t ) int nTNF,
 *               String pTypeString,
 *               ( W_HANDLE * ) int[] phRegistry )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNDEFReadMessageOnAnyTag
   (JNIEnv *env, jclass clazz,
   jint pCallback_jni,
   jint nPriority_jni,
   jint nTNF_jni,
   jstring pTypeString_jni,
   jintArray phRegistry_jni ) {
   const tchar* pTypeString = WJavaGetStringContent(env, pTypeString_jni);
   W_HANDLE phRegistry_value = (W_HANDLE)0;

   WNDEFReadMessageOnAnyTag(
        (tWNDEFReadMessageCompleted *)pCallback_jni, null,
        (uint8_t)nPriority_jni,
        (uint8_t)nTNF_jni,
        pTypeString,
        (phRegistry_jni != null)?(&phRegistry_value):null );

   WJavaReleaseStringContent(pTypeString);
   WJavaSetIntArrayFirstElement(env, phRegistry_jni, phRegistry_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNFCControllerFirmwareUpdate(
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pUpdateBuffer,
 *               ( uint32_t ) int nMode )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerFirmwareUpdate
   (JNIEnv *env, jclass clazz,
   jint pCallback_jni,
   jint pUpdateBuffer_jni,
   jint nMode_jni ) {

   WNFCControllerFirmwareUpdate(
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pUpdateBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pUpdateBuffer_jni),
        (uint32_t)nMode_jni );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( uint32_t ) int WNFCControllerFirmwareUpdateState( )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerFirmwareUpdateState
   (JNIEnv *env, jclass clazz ) {
   jint result;

   result = (jint)WNFCControllerFirmwareUpdateState(
 );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WNFCControllerGetFirmwareProperty(
 *               ( const uint8_t * ) byte[] pUpdateBuffer,
 *               ( uint8_t ) int nPropertyIdentifier,
 *               ( tchar * ) char[] pValueBuffer,
 *               ( uint32_t * ) int[] pnValueLength )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerGetFirmwareProperty
   (JNIEnv *env, jclass clazz,
   jbyteArray pUpdateBuffer_jni,
   jint nPropertyIdentifier_jni,
   jcharArray pValueBuffer_jni,
   jintArray pnValueLength_jni ) {
   jint result;
   jbyte* pUpdateBuffer = WJavaGetByteArrayElements(env, pUpdateBuffer_jni);
   jchar* pValueBuffer = WJavaGetCharArrayElements(env, pValueBuffer_jni);
   uint32_t pnValueLength_value = (uint32_t)0;

   result = (jint)WNFCControllerGetFirmwareProperty(
        (const uint8_t *)pUpdateBuffer,
        (uint32_t)WJavaGetArrayLength(env, pUpdateBuffer_jni),
        (uint8_t)nPropertyIdentifier_jni,
        (tchar *)pValueBuffer,
        (uint32_t)WJavaGetArrayLength(env, pValueBuffer_jni),
        (pnValueLength_jni != null)?(&pnValueLength_value):null );

   WJavaReleaseByteArrayElements(env, pUpdateBuffer_jni, pUpdateBuffer, false);
   WJavaReleaseCharArrayElements(env, pValueBuffer_jni, pValueBuffer, true);
   WJavaSetIntArrayFirstElement(env, pnValueLength_jni, pnValueLength_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( uint32_t ) int WNFCControllerGetMode( )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerGetMode
   (JNIEnv *env, jclass clazz ) {
   jint result;

   result = (jint)WNFCControllerGetMode(
 );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WNFCControllerGetProperty(
 *               ( uint8_t ) int nPropertyIdentifier,
 *               ( tchar * ) char[] pValueBuffer,
 *               ( uint32_t * ) int[] pnValueLength )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerGetProperty
   (JNIEnv *env, jclass clazz,
   jint nPropertyIdentifier_jni,
   jcharArray pValueBuffer_jni,
   jintArray pnValueLength_jni ) {
   jint result;
   jchar* pValueBuffer = WJavaGetCharArrayElements(env, pValueBuffer_jni);
   uint32_t pnValueLength_value = (uint32_t)0;

   result = (jint)WNFCControllerGetProperty(
        (uint8_t)nPropertyIdentifier_jni,
        (tchar *)pValueBuffer,
        (uint32_t)WJavaGetArrayLength(env, pValueBuffer_jni),
        (pnValueLength_jni != null)?(&pnValueLength_value):null );

   WJavaReleaseCharArrayElements(env, pValueBuffer_jni, pValueBuffer, true);
   WJavaSetIntArrayFirstElement(env, pnValueLength_jni, pnValueLength_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNFCControllerGetRFActivity(
 *               ( uint8_t * ) int[] pnReaderState,
 *               ( uint8_t * ) int[] pnCardState,
 *               ( uint8_t * ) int[] pnP2PState )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerGetRFActivity
   (JNIEnv *env, jclass clazz,
   jintArray pnReaderState_jni,
   jintArray pnCardState_jni,
   jintArray pnP2PState_jni ) {
   uint8_t pnReaderState_value = (uint8_t)0;
   uint8_t pnCardState_value = (uint8_t)0;
   uint8_t pnP2PState_value = (uint8_t)0;

   WNFCControllerGetRFActivity(
        (pnReaderState_jni != null)?(&pnReaderState_value):null,
        (pnCardState_jni != null)?(&pnCardState_value):null,
        (pnP2PState_jni != null)?(&pnP2PState_value):null );

   WJavaSetIntArrayFirstElement(env, pnReaderState_jni, pnReaderState_value);
   WJavaSetIntArrayFirstElement(env, pnCardState_jni, pnCardState_value);
   WJavaSetIntArrayFirstElement(env, pnP2PState_jni, pnP2PState_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNFCControllerGetRFLock(
 *               ( uint32_t ) int nLockSet,
 *               ( bool * ) int[] pbReaderLock,
 *               ( bool * ) int[] pbCardLock )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerGetRFLock
   (JNIEnv *env, jclass clazz,
   jint nLockSet_jni,
   jintArray pbReaderLock_jni,
   jintArray pbCardLock_jni ) {
   bool pbReaderLock_value = (bool)0;
   bool pbCardLock_value = (bool)0;

   WNFCControllerGetRFLock(
        (uint32_t)nLockSet_jni,
        (pbReaderLock_jni != null)?(&pbReaderLock_value):null,
        (pbCardLock_jni != null)?(&pbCardLock_value):null );

   WJavaSetIntArrayFirstElement(env, pbReaderLock_jni, pbReaderLock_value);
   WJavaSetIntArrayFirstElement(env, pbCardLock_jni, pbCardLock_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WNFCControllerMonitorException(
 *               ( tWBasicGenericEventHandler * ) int pHandler,
 *               ( W_HANDLE * ) int[] phEventRegistry )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerMonitorException
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jintArray phEventRegistry_jni ) {
   jint result;
   W_HANDLE phEventRegistry_value = (W_HANDLE)0;

   result = (jint)WNFCControllerMonitorException(
        (tWBasicGenericEventHandler *)pHandler_jni, null,
        (phEventRegistry_jni != null)?(&phEventRegistry_value):null );

   WJavaSetIntArrayFirstElement(env, phEventRegistry_jni, phEventRegistry_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNFCControllerProductionTest(
 *               ( const uint8_t * ) int pParameterBuffer,
 *               ( uint8_t * ) int pResultBuffer,
 *               ( tWBasicGenericDataCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerProductionTest
   (JNIEnv *env, jclass clazz,
   jint pParameterBuffer_jni,
   jint pResultBuffer_jni,
   jint pCallback_jni ) {

   WNFCControllerProductionTest(
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pParameterBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pParameterBuffer_jni),
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pResultBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pResultBuffer_jni),
        (tWBasicGenericDataCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNFCControllerReset(
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint32_t ) int nMode )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerReset
   (JNIEnv *env, jclass clazz,
   jint pCallback_jni,
   jint nMode_jni ) {

   WNFCControllerReset(
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint32_t)nMode_jni );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNFCControllerSelfTest(
 *               ( tWNFCControllerSelfTestCompleted * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerSelfTest
   (JNIEnv *env, jclass clazz,
   jint pCallback_jni ) {

   WNFCControllerSelfTest(
        (tWNFCControllerSelfTestCompleted *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WNFCControllerSetRFLock(
 *               ( uint32_t ) int nLockSet,
 *               boolean bReaderLock,
 *               boolean bCardLock,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerSetRFLock
   (JNIEnv *env, jclass clazz,
   jint nLockSet_jni,
   jboolean bReaderLock_jni,
   jboolean bCardLock_jni,
   jint pCallback_jni ) {

   WNFCControllerSetRFLock(
        (uint32_t)nLockSet_jni,
        (bReaderLock_jni == JNI_FALSE)?false:true,
        (bCardLock_jni == JNI_FALSE)?false:true,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WNFCControllerSwitchStandbyMode(
 *               boolean bStandbyOn )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WNFCControllerSwitchStandbyMode
   (JNIEnv *env, jclass clazz,
   jboolean bStandbyOn_jni ) {
   jint result;

   result = (jint)WNFCControllerSwitchStandbyMode(
        (bStandbyOn_jni == JNI_FALSE)?false:true );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PConnect(
 *               ( W_HANDLE ) int hSocket,
 *               ( W_HANDLE ) int hLink,
 *               ( tWBasicGenericCallbackFunction * ) int pEstablishmentCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PConnect
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint hLink_jni,
   jint pEstablishmentCallback_jni ) {

   WP2PConnect(
        (W_HANDLE)hSocket_jni,
        (W_HANDLE)hLink_jni,
        (tWBasicGenericCallbackFunction *)pEstablishmentCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WP2PCreateSocket(
 *               ( uint8_t ) int nType,
 *               String pServiceURI,
 *               ( uint8_t ) int nSAP,
 *               ( W_HANDLE * ) int[] phSocket )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WP2PCreateSocket
   (JNIEnv *env, jclass clazz,
   jint nType_jni,
   jstring pServiceURI_jni,
   jint nSAP_jni,
   jintArray phSocket_jni ) {
   jint result;
   const tchar* pServiceURI = WJavaGetStringContent(env, pServiceURI_jni);
   W_HANDLE phSocket_value = (W_HANDLE)0;

   result = (jint)WP2PCreateSocket(
        (uint8_t)nType_jni,
        pServiceURI,
        (uint8_t)nSAP_jni,
        (phSocket_jni != null)?(&phSocket_value):null );

   WJavaReleaseStringContent(pServiceURI);
   WJavaSetIntArrayFirstElement(env, phSocket_jni, phSocket_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PEstablishLink(
 *               ( tWBasicGenericHandleCallbackFunction * ) int pEstablishmentCallback,
 *               ( tWBasicGenericCallbackFunction * ) int pReleaseCallback,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PEstablishLink
   (JNIEnv *env, jclass clazz,
   jint pEstablishmentCallback_jni,
   jint pReleaseCallback_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WP2PEstablishLink(
        (tWBasicGenericHandleCallbackFunction *)pEstablishmentCallback_jni, null,
        (tWBasicGenericCallbackFunction *)pReleaseCallback_jni, null,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WP2PGetSocketParameter(
 *               ( W_HANDLE ) int hSocket,
 *               ( uint32_t ) int nParameter,
 *               ( uint32_t * ) int[] pnValue )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WP2PGetSocketParameter
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint nParameter_jni,
   jintArray pnValue_jni ) {
   jint result;
   uint32_t pnValue_value = (uint32_t)0;

   result = (jint)WP2PGetSocketParameter(
        (W_HANDLE)hSocket_jni,
        (uint32_t)nParameter_jni,
        (pnValue_jni != null)?(&pnValue_value):null );

   WJavaSetIntArrayFirstElement(env, pnValue_jni, pnValue_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PRead(
 *               ( W_HANDLE ) int hSocket,
 *               ( tWBasicGenericDataCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pReceptionBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PRead
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint pCallback_jni,
   jint pReceptionBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WP2PRead(
        (W_HANDLE)hSocket_jni,
        (tWBasicGenericDataCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pReceptionBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pReceptionBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PRecvFrom(
 *               ( W_HANDLE ) int hSocket,
 *               ( tWP2PRecvFromCompleted * ) int pCallback,
 *               ( uint8_t * ) int pReceptionBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PRecvFrom
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint pCallback_jni,
   jint pReceptionBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WP2PRecvFrom(
        (W_HANDLE)hSocket_jni,
        (tWP2PRecvFromCompleted *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pReceptionBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pReceptionBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PSendTo(
 *               ( W_HANDLE ) int hSocket,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t ) int nSAP,
 *               ( const uint8_t * ) int pSendBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PSendTo
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint pCallback_jni,
   jint nSAP_jni,
   jint pSendBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WP2PSendTo(
        (W_HANDLE)hSocket_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t)nSAP_jni,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pSendBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pSendBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WP2PSetSocketParameter(
 *               ( W_HANDLE ) int hSocket,
 *               ( uint32_t ) int nParameter,
 *               ( uint32_t ) int nValue )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WP2PSetSocketParameter
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint nParameter_jni,
   jint nValue_jni ) {
   jint result;

   result = (jint)WP2PSetSocketParameter(
        (W_HANDLE)hSocket_jni,
        (uint32_t)nParameter_jni,
        (uint32_t)nValue_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PShutdown(
 *               ( W_HANDLE ) int hSocket,
 *               ( tWBasicGenericCallbackFunction * ) int pReleaseCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PShutdown
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint pReleaseCallback_jni ) {

   WP2PShutdown(
        (W_HANDLE)hSocket_jni,
        (tWBasicGenericCallbackFunction *)pReleaseCallback_jni, null );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PURILookup(
 *               ( W_HANDLE ) int hLink,
 *               ( tWP2PURILookupCompleted * ) int pCallback,
 *               String pServiceURI )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PURILookup
   (JNIEnv *env, jclass clazz,
   jint hLink_jni,
   jint pCallback_jni,
   jstring pServiceURI_jni ) {
   const tchar* pServiceURI = WJavaGetStringContent(env, pServiceURI_jni);

   WP2PURILookup(
        (W_HANDLE)hLink_jni,
        (tWP2PURILookupCompleted *)pCallback_jni, null,
        pServiceURI );

   WJavaReleaseStringContent(pServiceURI);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WP2PWrite(
 *               ( W_HANDLE ) int hSocket,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pSendBuffer,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WP2PWrite
   (JNIEnv *env, jclass clazz,
   jint hSocket_jni,
   jint pCallback_jni,
   jint pSendBuffer_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WP2PWrite(
        (W_HANDLE)hSocket_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pSendBuffer_jni),
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pSendBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WPicoGetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WPicoGetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WPicoGetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WPicoIsWritable(
 *               ( W_HANDLE ) int hConnection )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WPicoIsWritable
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni ) {
   jint result;

   result = (jint)WPicoIsWritable(
        (W_HANDLE)hConnection_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WPicoRead(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WPicoRead
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WPicoRead(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WPicoWrite(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               boolean bLockCard,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WPicoWrite
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jboolean bLockCard_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WPicoWrite(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (bLockCard_jni == JNI_FALSE)?false:true,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WReaderErrorEventRegister(
 *               ( tWBasicGenericEventHandler * ) int pHandler,
 *               ( uint8_t ) int nEventType,
 *               boolean bCardDetectionRequested,
 *               ( W_HANDLE * ) int[] phRegistryHandle )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WReaderErrorEventRegister
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jint nEventType_jni,
   jboolean bCardDetectionRequested_jni,
   jintArray phRegistryHandle_jni ) {
   jint result;
   W_HANDLE phRegistryHandle_value = (W_HANDLE)0;

   result = (jint)WReaderErrorEventRegister(
        (tWBasicGenericEventHandler *)pHandler_jni, null,
        (uint8_t)nEventType_jni,
        (bCardDetectionRequested_jni == JNI_FALSE)?false:true,
        (phRegistryHandle_jni != null)?(&phRegistryHandle_value):null );

   WJavaSetIntArrayFirstElement(env, phRegistryHandle_jni, phRegistryHandle_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WReaderGetPulsePeriod(
 *               ( uint32_t * ) int[] pnTimeout )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WReaderGetPulsePeriod
   (JNIEnv *env, jclass clazz,
   jintArray pnTimeout_jni ) {
   jint result;
   uint32_t pnTimeout_value = (uint32_t)0;

   result = (jint)WReaderGetPulsePeriod(
        (pnTimeout_jni != null)?(&pnTimeout_value):null );

   WJavaSetIntArrayFirstElement(env, pnTimeout_jni, pnTimeout_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WReaderHandlerWorkPerformed(
 *               ( W_HANDLE ) int hConnection,
 *               boolean bGiveToNextListener,
 *               boolean bCardApplicationMatch )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WReaderHandlerWorkPerformed
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jboolean bGiveToNextListener_jni,
   jboolean bCardApplicationMatch_jni ) {

   WReaderHandlerWorkPerformed(
        (W_HANDLE)hConnection_jni,
        (bGiveToNextListener_jni == JNI_FALSE)?false:true,
        (bCardApplicationMatch_jni == JNI_FALSE)?false:true );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( bool ) boolean WReaderIsPropertySupported(
 *               ( uint8_t ) int nPropertyIdentifier )
 */
JNIEXPORT jboolean JNICALL Java_org_opennfc_MethodAutogen_WReaderIsPropertySupported
   (JNIEnv *env, jclass clazz,
   jint nPropertyIdentifier_jni ) {
   jboolean result;

   result = (jboolean)WReaderIsPropertySupported(
        (uint8_t)nPropertyIdentifier_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WReaderListenToCardDetection(
 *               ( tWReaderCardDetectionHandler * ) int pHandler,
 *               ( uint8_t ) int nPriority,
 *               ( const uint8_t * ) byte[] pConnectionPropertyArray,
 *               ( W_HANDLE * ) int[] phEventRegistry )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WReaderListenToCardDetection
   (JNIEnv *env, jclass clazz,
   jint pHandler_jni,
   jint nPriority_jni,
   jbyteArray pConnectionPropertyArray_jni,
   jintArray phEventRegistry_jni ) {
   jint result;
   jbyte* pConnectionPropertyArray = WJavaGetByteArrayElements(env, pConnectionPropertyArray_jni);
   W_HANDLE phEventRegistry_value = (W_HANDLE)0;

   result = (jint)WReaderListenToCardDetection(
        (tWReaderCardDetectionHandler *)pHandler_jni, null,
        (uint8_t)nPriority_jni,
        (const uint8_t *)pConnectionPropertyArray,
        (uint32_t)WJavaGetArrayLength(env, pConnectionPropertyArray_jni),
        (phEventRegistry_jni != null)?(&phEventRegistry_value):null );

   WJavaReleaseByteArrayElements(env, pConnectionPropertyArray_jni, pConnectionPropertyArray, false);
   WJavaSetIntArrayFirstElement(env, phEventRegistry_jni, phEventRegistry_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( bool ) boolean WReaderPreviousApplicationMatch(
 *               ( W_HANDLE ) int hConnection )
 */
JNIEXPORT jboolean JNICALL Java_org_opennfc_MethodAutogen_WReaderPreviousApplicationMatch
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni ) {
   jboolean result;

   result = (jboolean)WReaderPreviousApplicationMatch(
        (W_HANDLE)hConnection_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WReaderSetPulsePeriod(
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint32_t ) int nPulsePeriod )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WReaderSetPulsePeriod
   (JNIEnv *env, jclass clazz,
   jint pCallback_jni,
   jint nPulsePeriod_jni ) {

   WReaderSetPulsePeriod(
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint32_t)nPulsePeriod_jni );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WSecurityAuthenticate(
 *               ( const uint8_t * ) byte[] pApplicationDataBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WSecurityAuthenticate
   (JNIEnv *env, jclass clazz,
   jbyteArray pApplicationDataBuffer_jni ) {
   jint result;
   jbyte* pApplicationDataBuffer = WJavaGetByteArrayElements(env, pApplicationDataBuffer_jni);

   result = (jint)WSecurityAuthenticate(
        (const uint8_t *)pApplicationDataBuffer,
        (uint32_t)WJavaGetArrayLength(env, pApplicationDataBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pApplicationDataBuffer_jni, pApplicationDataBuffer, false);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WType1ChipGetConnectionInfoBuffer(
 *               ( W_HANDLE ) int hConnection,
 *               ( uint8_t * ) byte[] pInfoBuffer )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WType1ChipGetConnectionInfoBuffer
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jbyteArray pInfoBuffer_jni ) {
   jint result;
   jbyte* pInfoBuffer = WJavaGetByteArrayElements(env, pInfoBuffer_jni);

   result = (jint)WType1ChipGetConnectionInfoBuffer(
        (W_HANDLE)hConnection_jni,
        (uint8_t *)pInfoBuffer,
        (uint32_t)WJavaGetArrayLength(env, pInfoBuffer_jni) );

   WJavaReleaseByteArrayElements(env, pInfoBuffer_jni, pInfoBuffer, true);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WType1ChipIsWritable(
 *               ( W_HANDLE ) int hConnection )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WType1ChipIsWritable
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni ) {
   jint result;

   result = (jint)WType1ChipIsWritable(
        (W_HANDLE)hConnection_jni );

   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WType1ChipRead(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WType1ChipRead
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WType1ChipRead(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WType1ChipWrite(
 *               ( W_HANDLE ) int hConnection,
 *               ( tWBasicGenericCallbackFunction * ) int pCallback,
 *               ( const uint8_t * ) int pBuffer,
 *               ( uint32_t ) int nOffset,
 *               boolean bLockBlocks,
 *               ( W_HANDLE * ) int[] phOperation )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WType1ChipWrite
   (JNIEnv *env, jclass clazz,
   jint hConnection_jni,
   jint pCallback_jni,
   jint pBuffer_jni,
   jint nOffset_jni,
   jboolean bLockBlocks_jni,
   jintArray phOperation_jni ) {
   W_HANDLE phOperation_value = (W_HANDLE)0;

   WType1ChipWrite(
        (W_HANDLE)hConnection_jni,
        (tWBasicGenericCallbackFunction *)pCallback_jni, null,
        (const uint8_t *)WJavaGetByteBufferPointer((W_HANDLE)pBuffer_jni),
        (uint32_t)nOffset_jni,
        (uint32_t)WJavaGetByteBufferLength((W_HANDLE)pBuffer_jni),
        (bLockBlocks_jni == JNI_FALSE)?false:true,
        (phOperation_jni != null)?(&phOperation_value):null );

   WJavaSetIntArrayFirstElement(env, phOperation_jni, phOperation_value);
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    ( W_ERROR ) int WVirtualTagCreate(
 *               ( uint8_t ) int nTagType,
 *               ( const uint8_t * ) byte[] pIdentifier,
 *               ( uint32_t ) int nMaximumMessageLength,
 *               ( W_HANDLE * ) int[] phVirtualTag )
 */
JNIEXPORT jint JNICALL Java_org_opennfc_MethodAutogen_WVirtualTagCreate
   (JNIEnv *env, jclass clazz,
   jint nTagType_jni,
   jbyteArray pIdentifier_jni,
   jint nMaximumMessageLength_jni,
   jintArray phVirtualTag_jni ) {
   jint result;
   jbyte* pIdentifier = WJavaGetByteArrayElements(env, pIdentifier_jni);
   W_HANDLE phVirtualTag_value = (W_HANDLE)0;

   result = (jint)WVirtualTagCreate(
        (uint8_t)nTagType_jni,
        (const uint8_t *)pIdentifier,
        (uint32_t)WJavaGetArrayLength(env, pIdentifier_jni),
        (uint32_t)nMaximumMessageLength_jni,
        (phVirtualTag_jni != null)?(&phVirtualTag_value):null );

   WJavaReleaseByteArrayElements(env, pIdentifier_jni, pIdentifier, false);
   WJavaSetIntArrayFirstElement(env, phVirtualTag_jni, phVirtualTag_value);
   return result;
}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WVirtualTagStart(
 *               ( W_HANDLE ) int hVirtualTag,
 *               ( tWBasicGenericCallbackFunction * ) int pCompletionCallback,
 *               ( tWBasicGenericEventHandler * ) int pEventCallback,
 *               boolean bReadOnly )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WVirtualTagStart
   (JNIEnv *env, jclass clazz,
   jint hVirtualTag_jni,
   jint pCompletionCallback_jni,
   jint pEventCallback_jni,
   jboolean bReadOnly_jni ) {

   WVirtualTagStart(
        (W_HANDLE)hVirtualTag_jni,
        (tWBasicGenericCallbackFunction *)pCompletionCallback_jni, null,
        (tWBasicGenericEventHandler *)pEventCallback_jni, null,
        (bReadOnly_jni == JNI_FALSE)?false:true );

}

/*
 * Class:     org.opennfc.MethodAutogen
 *
 * Method:    void WVirtualTagStop(
 *               ( W_HANDLE ) int hVirtualTag,
 *               ( tWBasicGenericCallbackFunction * ) int pCompletionCallback )
 */
JNIEXPORT void JNICALL Java_org_opennfc_MethodAutogen_WVirtualTagStop
   (JNIEnv *env, jclass clazz,
   jint hVirtualTag_jni,
   jint pCompletionCallback_jni ) {

   WVirtualTagStop(
        (W_HANDLE)hVirtualTag_jni,
        (tWBasicGenericCallbackFunction *)pCompletionCallback_jni, null );

}


/* End of file */
