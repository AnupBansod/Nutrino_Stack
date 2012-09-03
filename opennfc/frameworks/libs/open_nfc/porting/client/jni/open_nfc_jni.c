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

#define P_MODULE  P_MODULE_DEC( JAVA )

#include "wme_context.h"

#include <jni.h>

#include "open_nfc_jni_autogen.h"
#include "open_nfc_jni.h"

/* See header file */
uint32_t WJavaGetIntArrayFirstElement(
            JNIEnv *env,
            jintArray arrayInstance)
{
   uint32_t value = 0;

   PDebugTrace("WJavaGetIntArrayFirstElement()");

   if(arrayInstance != null)
   {
      if((*env)->GetArrayLength(env, arrayInstance) != 0)
      {
         (*env)->GetIntArrayRegion(env, arrayInstance, 0, 1, (jint*)&value);
      }
   }

   return value;
}

/* See header file */
void WJavaSetIntArrayFirstElement(
            JNIEnv *env,
            jintArray arrayInstance,
            uint32_t value)
{
   PDebugTrace("WJavaSetIntArrayFirstElement()");

   if(arrayInstance != null)
   {
      if((*env)->GetArrayLength(env, arrayInstance) != 0)
      {
         (*env)->SetIntArrayRegion(env, arrayInstance, 0, 1, (const jint*)&value);
      }
   }
}

/* See header file */
jint* WJavaGetIntArrayElements(
            JNIEnv *env,
            jintArray arrayInstance)
{
   PDebugTrace("WJavaGetIntArrayElements()");

   if(arrayInstance == null)
   {
      return null;
   }

   return (*env)->GetIntArrayElements(env, arrayInstance, null);
}

/* See header file */
jchar* WJavaGetCharArrayElements(
            JNIEnv *env,
            jcharArray arrayInstance)
{
   PDebugTrace("WJavaGetCharArrayElements()");

   if(arrayInstance == null)
   {
      return null;
   }

   return (*env)->GetCharArrayElements(env, arrayInstance, null);
}

/* See header file */
jbyte* WJavaGetByteArrayElements(
            JNIEnv *env,
            jbyteArray arrayInstance)
{
   PDebugTrace("WJavaGetByteArrayElements()");

   if(arrayInstance == null)
   {
      return null;
   }

   return (*env)->GetByteArrayElements(env, arrayInstance, null);
}

/* See header file */
void WJavaReleaseIntArrayElements(
            JNIEnv *env,
            jintArray arrayInstance,
            jint* pArrayContent,
            bool bCopy)
{
   PDebugTrace("WJavaReleaseIntArrayElements()");

   if(arrayInstance != null)
   {
      (*env)->ReleaseIntArrayElements(env, arrayInstance, pArrayContent, bCopy?0:JNI_ABORT);
   }
}

/* See header file */
void WJavaReleaseCharArrayElements(
            JNIEnv *env,
            jcharArray arrayInstance,
            jchar* pArrayContent,
            bool bCopy)
{
   PDebugTrace("WJavaReleaseCharArrayElements()");

   if(arrayInstance != null)
   {
      (*env)->ReleaseCharArrayElements(env, arrayInstance, pArrayContent, bCopy?0:JNI_ABORT);
   }
}

/* See header file */
void WJavaReleaseByteArrayElements(
            JNIEnv *env,
            jbyteArray arrayInstance,
            jbyte* pArrayContent,
            bool bCopy)
{
   PDebugTrace("WJavaReleaseByteArrayElements()");

   if(arrayInstance != null)
   {
      (*env)->ReleaseByteArrayElements(env, arrayInstance, pArrayContent, bCopy?0:JNI_ABORT);
   }
}

/* See header file */
uint32_t WJavaGetArrayLength(
            JNIEnv *env,
            jarray arrayInstance)
{
   PDebugTrace("WJavaGetArrayLength()");

   if(arrayInstance == null)
   {
      return 0;
   }
   return (uint32_t)((*env)->GetArrayLength(env, arrayInstance));
}

/* See header file */
const tchar* WJavaGetStringContent(
            JNIEnv *env,
            jstring stringInstance) {

   tchar* pContent = null;
   const jchar* pInternalBuffer;
   jsize length;

   PDebugTrace("WJavaGetStringContent()");

   if(stringInstance != null) {
      length = (*env)->GetStringLength(env, stringInstance);
      pInternalBuffer = (*env)->GetStringCritical(env, stringInstance, null);

      pContent = (tchar*)CMemoryAlloc((length + 1) << 1);
      if(pContent != null) {
         CMemoryCopy(pContent, pInternalBuffer, length << 1);
      }
      pContent[length] = 0;
      (*env)->ReleaseStringCritical(env, stringInstance, pInternalBuffer);
   }

   return (const tchar*)pContent;
}

/* See header file */
void WJavaReleaseStringContent(
            const tchar* pStringContent)
{
   PDebugTrace("WJavaReleaseStringContent()");

   CMemoryFree((void*)pStringContent);
}

/* The buffer reference structure */
typedef struct __tPJavaByteBufferReference
{
   /* Header for the object registry */
   tHandleObjectHeader sObjectHeader;

   /* The buffer pointer, offset and length */
   uint8_t* pBuffer;
   uint32_t nOffset;
   uint32_t nLength;

} tPJavaByteBufferReference;

/* Destroy object callback */
static uint32_t static_PJavaDestroyBuffer(
            tContext* pContext,
            void* pObject )
{
   tPJavaByteBufferReference* pBufferReference = (tPJavaByteBufferReference*)pObject;

   PDebugTrace("static_PJavaDestroyBuffer()");

   /* Free the buffer */
   CMemoryFree( pBufferReference->pBuffer );

   /* Free the object */
   CMemoryFree( pBufferReference );

   return P_HANDLE_DESTROY_DONE;
}

/* The buffer reference type */
tHandleType g_sPJavaByteBufferReference = { static_PJavaDestroyBuffer,
                                          null,
										  null,
                                          null,
                                          null };

#define P_HANDLE_TYPE_JAVA_BUFFER_REFERENCE (&g_sPJavaByteBufferReference)

/* See header file */
uint8_t* PJavaGetByteBufferPointer(
            tContext* pContext,
            W_HANDLE hBufferReference)
{
   tPJavaByteBufferReference* pBufferReference;
   PDebugTrace("PJavaGetByteBufferPointer()");

   if(PHandleGetObject(
            pContext, hBufferReference,
            P_HANDLE_TYPE_JAVA_BUFFER_REFERENCE,
            (void**)&pBufferReference) != W_SUCCESS)
   {
      PDebugError("PJavaGetByteBufferPointer: Invalid handle");
   }
   else if(pBufferReference != null)
   {
      return pBufferReference->pBuffer;
   }

   return null;
}

/* See header file */
uint32_t PJavaGetByteBufferLength(
            tContext* pContext,
            W_HANDLE hBufferReference)
{
   tPJavaByteBufferReference* pBufferReference;
   PDebugTrace("PJavaGetByteBufferLength()");

   if(PHandleGetObject(
            pContext, hBufferReference,
            P_HANDLE_TYPE_JAVA_BUFFER_REFERENCE,
            (void**)&pBufferReference) != W_SUCCESS)
   {
      PDebugError("PJavaGetByteBufferLength: Invalid handle");
   }
   else if(pBufferReference != null)
   {
      return pBufferReference->nLength;
   }

   return 0;
}

/* See header file */
uint32_t PJavaGetByteBufferOffset(
            tContext* pContext,
            W_HANDLE hBufferReference)
{
   tPJavaByteBufferReference* pBufferReference;
   PDebugTrace("PJavaGetByteBufferOffset()");

   if(PHandleGetObject(
            pContext, hBufferReference,
            P_HANDLE_TYPE_JAVA_BUFFER_REFERENCE,
            (void**)&pBufferReference) != W_SUCCESS)
   {
      PDebugError("PJavaGetByteBufferOffset: Invalid handle");
   }
   else if(pBufferReference != null)
   {
      return pBufferReference->nOffset;
   }

   return 0;
}

/* See header file */
W_HANDLE PJavaCreateByteBuffer(
            tContext* pContext,
            uint8_t* pJavaBuffer,
            uint32_t nOffset,
            uint32_t nLength)
{
   tPJavaByteBufferReference* pBufferReference;
   W_ERROR nError;
   W_HANDLE hBuffer;
   uint8_t* pBuffer = null;

   PDebugTrace("PJavaCreateByteBuffer()");

   if(nLength != 0)
   {
      pBuffer = (uint8_t*)CMemoryAlloc(nLength);
      if(pBuffer == null)
      {
         PDebugError("PJavaCreateByteBuffer: cannot allocate the native buffer");
         return W_NULL_HANDLE;
      }

      if(pJavaBuffer != null)
      {
         CMemoryCopy(pBuffer, &pJavaBuffer[nOffset], nLength);
      }
   }
   /* Create the buffer reference */
   pBufferReference = (tPJavaByteBufferReference*)CMemoryAlloc(sizeof(tPJavaByteBufferReference));
   if ( pBufferReference == null )
   {
      PDebugError("PJavaCreateByteBuffer: pBufferReference == null");
      CMemoryFree(pBuffer);
      return W_NULL_HANDLE;
   }
   CMemoryFill(pBufferReference, 0, sizeof(tPJavaByteBufferReference));

   /* Create the 14443-3 operation handle */
   if ( ( nError = PHandleRegister(
                     pContext,
                     pBufferReference,
                     P_HANDLE_TYPE_JAVA_BUFFER_REFERENCE,
                     &hBuffer ) ) != W_SUCCESS )
   {
      PDebugError("PJavaCreateByteBuffer: cannot create the buffer reference");
      CMemoryFree(pBufferReference);
      CMemoryFree(pBuffer);
      return W_NULL_HANDLE;
   }

   pBufferReference->pBuffer = pBuffer;
   pBufferReference->nOffset = nOffset;
   pBufferReference->nLength = nLength;

   return hBuffer;
}

/* See header file */
void PJavaReleaseByteBuffer(
            tContext* pContext,
            W_HANDLE hBufferReference,
            uint8_t* pJavaBuffer,
            uint32_t nJavaBufferLength)
{
   tPJavaByteBufferReference* pBufferReference;
   PDebugTrace("PJavaReleaseByteBuffer()");

   if(PHandleGetObject(
            pContext, hBufferReference,
            P_HANDLE_TYPE_JAVA_BUFFER_REFERENCE,
            (void**)&pBufferReference) != W_SUCCESS)
   {
      PDebugError("PJavaReleaseByteBuffer: Invalid handle");
   }
   else if(pBufferReference != null)
   {
      if(pJavaBuffer != null)
      {
         if(pBufferReference->nOffset + pBufferReference->nLength > nJavaBufferLength)
         {
            PDebugError("PJavaReleaseByteBuffer: Buffer too short");
         }
         else
         {
            CMemoryCopy(
               &pJavaBuffer[pBufferReference->nOffset],
               pBufferReference->pBuffer,
               pBufferReference->nLength);
         }
      }
   }

   PHandleClose(pContext, hBufferReference);
}

/* See header file */
W_ERROR P14Part3GetConnectionInfoBuffer(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tW14Part3ConnectionInfo info;
   W_ERROR nError = P14Part3GetConnectionInfo(pContext, hConnection, &info);

   /* Iso14443Part3AConnectionImpl */
   if((nError == W_SUCCESS) && (nInfoBufferLength == 14))
   {
      /* short atqa */
      PUtilWriteUint16ToBigEndianBuffer(info.sW14TypeA.nATQA, &pInfoBuffer[0]);
      /* byte sak */
      pInfoBuffer[2] = info.sW14TypeA.nSAK;
      /* int uidLength */
      pInfoBuffer[3] = info.sW14TypeA.nUIDLength;
      /* byte uid[4,7 or 10] */
      CMemoryCopy(&pInfoBuffer[4], info.sW14TypeA.aUID, info.sW14TypeA.nUIDLength);
   }
   /* Iso14443Part3BConnectionImpl */
   else if((nError == W_SUCCESS) && (nInfoBufferLength == 532))
   {
      uint32_t nIndex = 0;
      /* byte[] atqb */
      CMemoryCopy(&pInfoBuffer[nIndex], info.sW14TypeB.aATQB, 12);
      nIndex += 12;
      /* byte afi */
      pInfoBuffer[nIndex++] = info.sW14TypeB.nAFI;
      /* boolean isCidSupported */
      pInfoBuffer[nIndex++] == (info.sW14TypeB.bIsCIDSupported)?1:0;
      /* boolean isNadSupported */
      pInfoBuffer[nIndex++] == (info.sW14TypeB.bIsNADSupported)?1:0;
      /* int cardInputBufferSize */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeB.nCardInputBufferSize, &pInfoBuffer[nIndex]);
      nIndex += 4;
      /* int readerInputBufferSize */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeB.nReaderInputBufferSize, &pInfoBuffer[nIndex]);
      nIndex += 4;
      /* int baudRate */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeB.nBaudRate, &pInfoBuffer[nIndex]);
      nIndex += 4;
      /* int timeout */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeB.nTimeout, &pInfoBuffer[nIndex]);
      nIndex += 4;
      /* byte mbliCid */
      pInfoBuffer[nIndex++] = info.sW14TypeB.nMBLI_CID;
      /* int higherLayerDataLength */
      pInfoBuffer[nIndex++] = info.sW14TypeB.nHigherLayerDataLength;
      /* byte[] higherLayerData */
      CMemoryCopy(&pInfoBuffer[nIndex], info.sW14TypeB.aHigherLayerData, info.sW14TypeB.nHigherLayerDataLength);
      nIndex += info.sW14TypeB.nHigherLayerDataLength;
      /* int higherLayerResponseLength */
      pInfoBuffer[nIndex++] = info.sW14TypeB.nHigherLayerResponseLength;
      /* byte[] higherLayerResponse */
      CMemoryCopy(&pInfoBuffer[nIndex], info.sW14TypeB.aHigherLayerResponse, info.sW14TypeB.nHigherLayerResponseLength);
   }

   return nError;
}

/* See header file */
W_ERROR P14Part4GetConnectionInfoBuffer(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tW14Part4ConnectionInfo info;
   W_ERROR nError= P14Part4GetConnectionInfo(pContext, hConnection, &info);

   /* interface Iso14443Part4BConnectionImpl */
   if((nError == W_SUCCESS) && (nInfoBufferLength == 1))
   {
      pInfoBuffer[0] = info.sW14TypeB.nNAD;
   }
   /* interface Iso14443Part4AConnectionImpl */
   else if((nError == W_SUCCESS) && (nInfoBufferLength >= (uint32_t)(23+info.sW14TypeA.nApplicationDataLength)))
   {
      /* boolean isCidSupported */
      pInfoBuffer[0] = (info.sW14TypeA.bIsCIDSupported)?1:0;
      /* byte cid */
      pInfoBuffer[1] = info.sW14TypeA.nCID;
      /* boolean isNadSupported */
      pInfoBuffer[2] = (info.sW14TypeA.bIsNADSupported)?1:0;
      /* byte nad */
      pInfoBuffer[3] = info.sW14TypeA.nNAD;
      /* int cardInputBufferSize */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeA.nCardInputBufferSize, &pInfoBuffer[4]);
      /* int readerInputBufferSize */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeA.nReaderInputBufferSize, &pInfoBuffer[8]);
      /* byte fwiSfgi */
      pInfoBuffer[12] = info.sW14TypeA.nFWI_SFGI;
      /* int timeout */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeA.nTimeout, &pInfoBuffer[13]);
      /* byte dataRateMaxDiv */
      pInfoBuffer[17] = info.sW14TypeA.nDataRateMaxDiv;
      /* int baudRate */
      PUtilWriteUint32ToBigEndianBuffer(info.sW14TypeA.nBaudRate, &pInfoBuffer[18]);
      /* int applicationDatalength */
      pInfoBuffer[22] = info.sW14TypeA.nApplicationDataLength;
      /* byte applicationData[15] */
      CMemoryCopy(&pInfoBuffer[23],info.sW14TypeA.aApplicationData,info.sW14TypeA.nApplicationDataLength);
   }

   return nError;
}

/* See header file */
W_ERROR P15GetConnectionInfoBuffer (
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tW15ConnectionInfo info;
   W_ERROR nError= P15GetConnectionInfo(pContext, hConnection, &info);
   if((nError == W_SUCCESS) && (nInfoBufferLength >= 22))
   {
      /* byte[] uid */
      CMemoryCopy(&pInfoBuffer[0], info.UID, 8);
      /* boolean isAfiSupported */
      pInfoBuffer[8] = (info.bIsAFISupported)?1:0;
      /* boolean isAfiLocked */
      pInfoBuffer[9] = (info.bIsAFILocked)?1:0;
      /* boolean isDsfidSupported */
      pInfoBuffer[10] = (info.bIsDSFIDSupported)?1:0;
      /* boolean isDsfidLocked */
      pInfoBuffer[11] = (info.bIsDSFIDLocked)?1:0;
      /* byte afi */
      pInfoBuffer[12] = info.nAFI;
      /* byte dsfid */
      pInfoBuffer[13] = info.nDSFID;
      /* int sectorSize */
      PUtilWriteUint32ToBigEndianBuffer(info.nSectorSize, &pInfoBuffer[14]);
      /* int sectorNumber */
      PUtilWriteUint32ToBigEndianBuffer(info.nSectorNumber, &pInfoBuffer[18]);
   }
   return nError;
}

/* See header file */
W_ERROR PPicoGetConnectionInfoBuffer (
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tWPicoConnectionInfo info;
   W_ERROR nError= PPicoGetConnectionInfo(pContext, hConnection, &info);
   if((nError == W_SUCCESS) && (nInfoBufferLength >= 16))
   {
      /* byte[] uid */
      CMemoryCopy(&pInfoBuffer[0], info.UID, 8);
      /* int sectorSize */
      PUtilWriteUint32ToBigEndianBuffer(info.nSectorSize, &pInfoBuffer[8]);
      /* int sectorNumber */
      PUtilWriteUint32ToBigEndianBuffer(info.nSectorNumber, &pInfoBuffer[12]);
   }
   return nError;
}

/* See header file */
W_ERROR PMifareGetConnectionInfoBuffer (
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tWMifareConnectionInfo info;
   W_ERROR nError= PMifareGetConnectionInfo(pContext, hConnection, &info);
   if((nError == W_SUCCESS) && (nInfoBufferLength >= 15))
   {
      /* byte[] uid */
      CMemoryCopy(&pInfoBuffer[0], info.UID, 7);
      /* int sectorSize */
      PUtilWriteUint32ToBigEndianBuffer(info.nSectorSize, &pInfoBuffer[7]);
      /* int sectorNumber */
      PUtilWriteUint32ToBigEndianBuffer(info.nSectorNumber, &pInfoBuffer[11]);
   }
   return nError;
}


/* See header file */
W_ERROR PType1ChipGetConnectionInfoBuffer (
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tWType1ChipConnectionInfo info;
   W_ERROR nError= PType1ChipGetConnectionInfo(pContext, hConnection, &info);
   if((nError == W_SUCCESS) && (nInfoBufferLength >= 17))
   {
      /* byte[] uid */
      CMemoryCopy(&pInfoBuffer[0], info.UID, 7);
      /* int blockSize */
      PUtilWriteUint32ToBigEndianBuffer(info.nBlockSize, &pInfoBuffer[7]);
      /* int blockNumber */
      PUtilWriteUint32ToBigEndianBuffer(info.nBlockNumber, &pInfoBuffer[11]);
      /* byte[] headerRom */
      CMemoryCopy(&pInfoBuffer[15], info.aHeaderRom, 2);
   }
   return nError;
}

/* See header file */
W_ERROR PFeliCaGetConnectionInfoBuffer (
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tWFeliCaConnectionInfo info;
   W_ERROR nError= PFeliCaGetConnectionInfo(pContext, hConnection, &info);
   if((nError == W_SUCCESS) && (nInfoBufferLength >= 18))
   {
      /* byte[] mManufacturerId */
      CMemoryCopy(&pInfoBuffer[0], info.aManufacturerID, 8);
      /* short mSystemCode */
      PUtilWriteUint16ToBigEndianBuffer(info.nSystemCode, &pInfoBuffer[8]);
      /* byte[] mManufacturerParameter */
      CMemoryCopy(&pInfoBuffer[10], info.aManufacturerParameter, 8);
   }
   return nError;
}

/* See header file */
W_ERROR PBPrimeGetConnectionInfoBuffer(
			tContext* pContext,
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength)
{
   tWBPrimeConnectionInfo info;

   W_ERROR nError= PBPrimeGetConnectionInfo(pContext, hConnection, &info);

   if((nError == W_SUCCESS) && (nInfoBufferLength >= info.nREPGENLength+1))
   {
	  pInfoBuffer[0] = info.nREPGENLength;

	  if(info.nREPGENLength>0)
	  {
		  CMemoryCopy(&pInfoBuffer[1], info.aREPGEN, info.nREPGENLength);
	  }
   }

   return nError;
}

/* See header file */
W_ERROR PMyDGetConnectionInfoBuffer(
			tContext* pContext,
			W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength)
{
	tWMyDConnectionInfo info;

	W_ERROR nError= PMyDGetConnectionInfo(pContext, hConnection, &info);

   if((nError == W_SUCCESS) && (nInfoBufferLength >= 11))
   {
	  CMemoryCopy(&pInfoBuffer[0], info.UID, 7);
	  PUtilWriteUint16ToBigEndianBuffer(info.nSectorSize, &pInfoBuffer[7]);
	  PUtilWriteUint16ToBigEndianBuffer(info.nSectorNumber, &pInfoBuffer[9]);
   }

   return nError;
}

/* See header file */
void PFeliCaReadSimple (
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t* pBuffer,
            uint32_t nLength,
            uint32_t  nNumberOfService,
            uint32_t* pServiceCodeList,
            uint32_t  nNumberOfBlocks,
            const uint8_t* pBlockList)
{
   uint16_t aServiceCodeList[16];
   uint32_t num = (nNumberOfService <= 16)?nNumberOfService:16;
   while(num)
   {
      num--;
      aServiceCodeList[num] = (uint16_t)pServiceCodeList[num];
   }

   PFeliCaRead (
            pContext, hConnection,
            pCallback, pCallbackParameter,
            pBuffer, nLength,
            (uint8_t)nNumberOfService, aServiceCodeList,
            (uint8_t)nNumberOfBlocks, pBlockList,
            null);
}

/* See header file */
void PFeliCaWriteSimple (
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            const uint8_t* pBuffer,
            uint32_t nLength,
            uint32_t  nNumberOfService,
            uint32_t* pServiceCodeList,
            uint32_t  nNumberOfBlocks,
            const uint8_t* pBlockList)
{
   uint16_t aServiceCodeList[16];
   uint32_t num = (nNumberOfService <= 16)?nNumberOfService:16;
   while(num)
   {
      num--;
      aServiceCodeList[num] = (uint16_t)pServiceCodeList[num];
   }

   PFeliCaWrite (
            pContext, hConnection,
            pCallback, pCallbackParameter,
            pBuffer, nLength,
            (uint8_t)nNumberOfService, aServiceCodeList,
            (uint8_t)nNumberOfBlocks, pBlockList,
            null);
}

/* See header file */
W_ERROR PNDEFGetTagInfoBuffer(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tNDEFTagInfo info;
   W_ERROR nError= PNDEFGetTagInfo(pContext, hConnection, &info);

   if((nError == W_SUCCESS) && (nInfoBufferLength >= (uint32_t)(7+info.nSerialNumberLength)))
   {
      /* bool isReadOnly */
      pInfoBuffer[0] = (info.bIsLocked)?1:0;
      /* bool islockable */
      pInfoBuffer[1] = (info.bIsLockable)?1:0;
      /* int freeSpaceSize */
      PUtilWriteUint32ToBigEndianBuffer(info.nFreeSpaceSize, &pInfoBuffer[2]);
      /* int serial number length */
      pInfoBuffer[6] = info.nSerialNumberLength;
      /* int serial number */
      CMemoryCopy(&pInfoBuffer[7],info.aSerialNumber,info.nSerialNumberLength);
   }
   return nError;
}

/* See header file */
void WEmulOpenConnectionBuffer(
            tWBasicGenericCallbackFunction* pOpenCallback,
            void* pOpenCallbackParameter,
            tWBasicGenericEventHandler* pEventCallback,
            void* pEventCallbackParameter,
            tWEmulCommandReceived* pCommandCallback,
            void* pCommandCallbackParameter,
            uint32_t nCardType,
            uint8_t* pUID,
            uint32_t nUIDLength,
            uint32_t nRandomIdentifierLength,
            W_HANDLE* phHandle)
{
   tWEmulConnectionInfo *pInfo = (tWEmulConnectionInfo*)CMemoryAlloc(sizeof(tWEmulConnectionInfo));
   if(pInfo != null)
   {
      if (nCardType == W_PROP_ISO_14443_4_A)
      {
         pInfo->nCardType = W_PROP_ISO_14443_4_A;
         pInfo->sCardInfo.s14A.bSetCIDSupport = false;
         pInfo->sCardInfo.s14A.nApplicationDataLength = 0;
         pInfo->sCardInfo.s14A.nATQA = 0x0000;
         pInfo->sCardInfo.s14A.nCID  = 0;
         pInfo->sCardInfo.s14A.nDataRateMax = 0x00;
         pInfo->sCardInfo.s14A.nFWI_SFGI = 0xEE;
         pInfo->sCardInfo.s14A.nNAD = 0;
         pInfo->sCardInfo.s14A.nSAK = 0x00;
         pInfo->sCardInfo.s14A.nUIDLength = (uint8_t)nRandomIdentifierLength;
         CMemoryCopy(pInfo->sCardInfo.s14A.UID, pUID, (nUIDLength <= 10)?nUIDLength:10);
      }
      else /*W_PROP_ISO_14443_4_B */
      {
         pInfo->nCardType = W_PROP_ISO_14443_4_B;
         pInfo->sCardInfo.s14B.nAFI = 0;
         pInfo->sCardInfo.s14B.nATQB = 0x0000;
         pInfo->sCardInfo.s14B.bSetCIDSupport = false;
         pInfo->sCardInfo.s14B.nCID = 0;
         pInfo->sCardInfo.s14B.nNAD = 0;
         pInfo->sCardInfo.s14B.nPUPILength = (uint8_t)nRandomIdentifierLength;
         CMemoryCopy(pInfo->sCardInfo.s14B.PUPI, pUID, (nUIDLength <= 4)?nUIDLength:4);
         pInfo->sCardInfo.s14B.nHigherLayerResponseLength = 0;
      }

      WEmulOpenConnection(
               pOpenCallback, pOpenCallbackParameter,
               pEventCallback, pEventCallbackParameter,
               pCommandCallback, pCommandCallbackParameter,
               pInfo, phHandle);
   }
   CMemoryFree(pInfo);
}

/* See header file */
bool PJavaNFCControllerGetBooleanProperty(
            tContext* pContext,
            uint8_t nPropertyIdentifier )
{
   bool bValue;
   W_ERROR nError = PNFCControllerGetBooleanProperty(
         pContext, nPropertyIdentifier, &bValue );

   if(nError != W_SUCCESS)
   {
      PDebugError("PJavaNFCControllerGetBooleanProperty: Error %s returned by PNFCControllerGetBooleanProperty()",
              PUtilTraceError(nError));
      bValue = false;
   }

   return bValue;
}

/* See header file */
/* See header file */
W_ERROR PJavaP2PSetConfigurationBuffer(
            tContext* pContext,
            uint8_t* pConfigurationBuffer,
            uint32_t nConfigurationBufferLength)
{
   tWP2PConfiguration * pConfiguration = (tWP2PConfiguration*)CMemoryAlloc(sizeof(tWP2PConfiguration));;

   if(nConfigurationBufferLength >= 5)
   {
      uint32_t nIndex = 0;
      /* uint16_t nLocalLTO */
      pConfiguration->nLocalLTO = PUtilReadUint16FromBigEndianBuffer(&pConfigurationBuffer[nIndex]);
      nIndex += 2;
      /* uint16_t nLocalMIU */
      pConfiguration->nLocalMIU = PUtilReadUint16FromBigEndianBuffer(&pConfigurationBuffer[nIndex]);
      nIndex += 2;
      /* bool bActivateInitiator */
      pConfiguration->bActivateInitiator = pConfigurationBuffer[nIndex];
   }
   return PP2PSetConfigurationDriver(pContext, pConfiguration, sizeof(*pConfiguration));
}

/* See header file */
 W_ERROR PJavaP2PGetConfigurationBuffer(
            tContext* pContext,
            uint8_t* pConfigurationBuffer,
            uint32_t nConfigurationBufferLength)
{
   tWP2PConfiguration pConfiguration;

   W_ERROR nError = PP2PGetConfigurationDriver(pContext, &pConfiguration, sizeof(pConfiguration));
   if((nError == W_SUCCESS) && (nConfigurationBufferLength >= 5))
   {
      uint32_t nIndex = 0;
      /* uint16_t nLocalLTO */
      PUtilWriteUint16ToBigEndianBuffer(pConfiguration.nLocalLTO, &pConfigurationBuffer[nIndex]);
      nIndex += 2;
      /* uint16_t nLocalMIU */
      PUtilWriteUint16ToBigEndianBuffer(pConfiguration.nLocalMIU, &pConfigurationBuffer[nIndex]);
      nIndex += 2;
      /* bool bActivateInitiator */
      pConfigurationBuffer[nIndex] = (pConfiguration.bActivateInitiator)?1:0;
   }
   return nError;
}

/* See header file */
W_ERROR PJavaP2PGetLinkPropertiesBuffer(
            tContext* pContext,
            W_HANDLE hLink,
            uint8_t* pInfoBuffer,
            uint32_t nInfoBufferLength)
{
   tWP2PLinkProperties pProperties;

   W_ERROR nError= PP2PGetLinkPropertiesDriver(pContext, hLink, &pProperties, sizeof(tWP2PLinkProperties));
   if((nError == W_SUCCESS) && (nInfoBufferLength >= 17))
   {
      uint32_t nIndex = 0;
      /* uint8_t  nAgreedLLCPVersion */
      pInfoBuffer[nIndex++] = pProperties.nAgreedLLCPVersion;
      /* uint8_t  nRemoteLLCPVersion */
      pInfoBuffer[nIndex++] = pProperties.nRemoteLLCPVersion;
      /* uint16_t  nRemoteWKS */
      PUtilWriteUint16ToBigEndianBuffer(pProperties.nRemoteWKS, &pInfoBuffer[nIndex]);
      nIndex += 2;
      /* uint32_t  nRemoteMIU */
      PUtilWriteUint32ToBigEndianBuffer(pProperties.nRemoteMIU, &pInfoBuffer[nIndex]);
      nIndex += 4;
      /* uint32_t  nRemoteLTO */
      PUtilWriteUint32ToBigEndianBuffer(pProperties.nRemoteLTO, &pInfoBuffer[nIndex]);
      nIndex += 4;
      /* uint32_t  nBaudRate */
      PUtilWriteUint32ToBigEndianBuffer(pProperties.nBaudRate, &pInfoBuffer[nIndex]);
      nIndex += 4;
      /* bool  bIsInitiator */
      pInfoBuffer[nIndex] = (pProperties.bIsInitiator)?1:0;
   }
   return nError;
}

/* See header file */
uint32_t PJavaNDEFGetMessageContent(
                  tContext* pContext,
                  W_HANDLE hMessage,
                  uint8_t* pMessageBuffer,
                  uint32_t nMessageBufferLength)
{
   uint32_t nTotalLength = 0;
   uint32_t nActualLength = 0;
   W_HANDLE hNextMessage = hMessage;

   if (PNDEFGetMessageContent( pContext,
                              hNextMessage,
                              pMessageBuffer+nActualLength,
                              nMessageBufferLength,
                              &nActualLength) != W_SUCCESS)
   {
      return 0;
   }

   nTotalLength = nActualLength;

   while ((hNextMessage = PNDEFGetNextMessage(pContext,hNextMessage)) != W_NULL_HANDLE)
   {
      if (PNDEFGetMessageContent( pContext,
                              hNextMessage,
                              pMessageBuffer+nTotalLength,
                              nMessageBufferLength,
                              &nActualLength) != W_SUCCESS)
      {
         nTotalLength = 0;
         break;
      }
      nTotalLength += nActualLength;
      nActualLength = 0;
   }

   return nTotalLength;
}

typedef struct __tPJavaWriteMessageOperation
{
   tDFCCallbackContext sCallbackContext;
   W_HANDLE hMessage;
} tPJavaWriteMessageOperation;

/* NDEF Write message completed callback */
static void static_PJavaWriteMessageCompleted(
                  tContext* pContext,
                  void * pCallbackParameter,
                  W_ERROR nResult)
{
   tPJavaWriteMessageOperation* pJavaWriteMessageOperation =
         (tPJavaWriteMessageOperation*)pCallbackParameter;

   PHandleClose(pContext, pJavaWriteMessageOperation->hMessage);

   PDFCPostContext2( &pJavaWriteMessageOperation->sCallbackContext,
                     P_DFC_TYPE_NDEF,
                     nResult);

   CMemoryFree(pJavaWriteMessageOperation);
}

/* See header file */
void PJavaNDEFWriteMessage(
                  tContext* pContext,
                  W_HANDLE hConnection,
                  tPBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t* pMessageBuffer,
                  uint32_t nMessageBufferLength,
                  uint32_t nActionMask,
                  W_HANDLE* phOperation)
{
   W_HANDLE hMessage = W_NULL_HANDLE;
   W_ERROR nError;

   tPJavaWriteMessageOperation* pJavaWriteMessageOperation =
      (tPJavaWriteMessageOperation*)CMemoryAlloc(sizeof(tPJavaWriteMessageOperation));

   if (pJavaWriteMessageOperation == null)
   {
      PDebugError("PJavaNDEFWriteMessage : CMemoryAlloc() failed");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   if ((nError = PNDEFBuildMessage(pContext, pMessageBuffer, nMessageBufferLength, &hMessage)) != W_SUCCESS)
   {
      PDebugError("PJavaNDEFWriteMessage : PNDEFBuildMessage failed %s", PUtilTraceError(nError));
      goto return_error;
   }

   pJavaWriteMessageOperation->hMessage = hMessage;

   /* prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &pJavaWriteMessageOperation->sCallbackContext);

   PNDEFWriteMessage(pContext,
                     hConnection,
                     static_PJavaWriteMessageCompleted,
                     pJavaWriteMessageOperation,
                     hMessage,
                     nActionMask,
                     phOperation);
   return;

return_error:
   {
      /* prepare the callback context */
      tDFCCallbackContext sCallbackContext;
      PDFCFillCallbackContext( pContext,(tDFCCallback*)pCallback,
               pCallbackParameter, &sCallbackContext);
      /* Send the error */
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NDEF, nError);
      return;
   }
}

/* See header file */
void PJavaNDEFWriteMessageOnAnyTag(
                  tContext* pContext,
                  tPBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t nPriority,
                  uint8_t* pMessageBuffer,
                  uint32_t nMessageBufferLength,
                  uint32_t nActionMask,
                  W_HANDLE* phOperation)
{
   W_HANDLE hMessage = W_NULL_HANDLE;
   W_ERROR nError;

   tPJavaWriteMessageOperation* pJavaWriteMessageOperation =
      (tPJavaWriteMessageOperation*)CMemoryAlloc(sizeof(tPJavaWriteMessageOperation));

   if (pJavaWriteMessageOperation == null)
   {
      PDebugError("PJavaNDEFWriteMessageOnAnyTag : CMemoryAlloc() failed");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   if ((nError = PNDEFBuildMessage(pContext, pMessageBuffer, nMessageBufferLength, &hMessage)) != W_SUCCESS)
   {
      PDebugError("PJavaNDEFWriteMessageOnAnyTag : PNDEFBuildMessage failed %s", PUtilTraceError(nError));
      goto return_error;
   }

   pJavaWriteMessageOperation->hMessage = hMessage;

   /* prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &pJavaWriteMessageOperation->sCallbackContext);

   PNDEFWriteMessageOnAnyTag( pContext,
                              static_PJavaWriteMessageCompleted,
                              pJavaWriteMessageOperation,
                              nPriority,
                              hMessage,
                              nActionMask,
                              phOperation);
   return;

return_error:
   {
      /* prepare the callback context */
      tDFCCallbackContext sCallbackContext;
      PDFCFillCallbackContext( pContext,(tDFCCallback*)pCallback,
               pCallbackParameter, &sCallbackContext);
      /* Send the error */
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NDEF, nError);
      return;
   }
}
