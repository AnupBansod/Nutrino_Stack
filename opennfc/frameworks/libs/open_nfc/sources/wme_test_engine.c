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
   Contains the test engine implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( TEST )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)


#include "wme_test_engine_autogen.h"

/* The maximum length in bytes of the product name */
#define P_TEST_PRODUCT_NAME_LENGTH     15

/* Current version number of the test engine */
#define P_TEST_BUILD_NUMBER_LENGTH     4
#define P_TEST_VERSION_NUMBER_LENGTH   4
#define P_TEST_PROTOCOL_VERSION        { 0x01, 0x00, 0x00, 0x07 }

/* Test engine state */
#define P_TEST_ENGINE_OFF           0x01
#define P_TEST_ENGINE_INIT          0x02
#define P_TEST_ENGINE_LOADED        0x03

/* List of message types */
#define P_TEST_MESSAGE_PING           0x00
#define P_TEST_MESSAGE_INIT           0x01
#define P_TEST_MESSAGE_DOWNLOAD       0x02
#define P_TEST_MESSAGE_EXECUTE        0x03
#define P_TEST_MESSAGE_RESETSTAT      0x04
#define P_TEST_MESSAGE_GETSTAT        0x05
#define P_TEST_MESSAGE_BOX            0x06
#define P_TEST_MESSAGE_PRESENT_OBJECT 0x07
#define P_TEST_MESSAGE_REMOVE_OBJECT  0x08
#define P_TEST_MESSAGE_EXECUTE_REMOTE 0x09

/* Automatic mode type flags */
#define P_TEST_MESSAGE_AUTOMATIC    0x01

/* The statistics flags */
#define P_TEST_STATISTIC_MEMORY     0x02
#define P_TEST_STATISTIC_TIME       0x04
#define P_TEST_STATISTIC_HANDLE     0x08
#define P_TEST_STATISTIC_PROTOCOL   0x10

/**
 * @brief   Returns the length in bytes of an ASCII zero-ended string.
 *
 * @param[in]  pString  The string.
 *
 * @return  The length in bytes of the string without the ending zero.
 **/
static uint32_t static_PTestStrlen(
            const char* pString)
{
   uint32_t nLength = 0;

   if ( pString != null )
   {
      while ( *pString++ != 0 )
      {
         nLength++;
      }
   }

   return nLength;
}

/**
 * @brief   Copies a value on more than one byte into a buffer.
 *
 * @param[in]  nValue  Value to copy in a buffer.
 *
 * @param[in]  pBuffer  Buffer to store the value over byte.
 **/
static void static_PTestValueToBuffer(
            tContext* pContext,
            uint32_t nValue,
            uint8_t* pBuffer )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   uint8_t i = 0x00;

   CDebugAssert( pBuffer != null );

   /* Check the little endian flag */
   if ( pTestEngineInstance->bLittleEndian == false )
   {
      for ( i=0; i<4; i++ )
      {
         pBuffer[i] = (uint8_t)( nValue >> ( 24 - ( i << 3 ) ) );
      }
   }
   else
   {
      for ( i=0; i<4; i++ )
      {
         pBuffer[3 - i] = (uint8_t)( nValue >> ( 24 - ( i << 3 ) ) );
      }
   }
}

/**
 * @brief   Gets a value from a buffer.
 *
 * @param[in]  pBuffer  Buffer to get the value from.
 *
 * @return     The retrieved information.
 **/
static uint32_t static_PTestBufferToValue(
            tContext* pContext,
            uint8_t* pBuffer )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   /* Check the little endian flag */
   if ( pTestEngineInstance->bLittleEndian == false )
   {
      return   (  ( pBuffer[0] * 0x1000000)
               +  ( pBuffer[1] * 0x10000)
               +  ( pBuffer[2] * 0x100)
               +  ( pBuffer[3] ) );
   }
   else
   {
      return *(uint32_t*)(&pBuffer);
   }
}

/**
 * @brief   Prepares a buffer to send.
 *
 * @param[in]  pContext  Context of the test instance.
 *
 * @param[in]  pBuffer  Buffer to fill in.
 *
 * @param[in]  nHeader  Header of the command.
 *
 * @param[in]  nErrorCode  Error code.
 *
 * @param[in]  nLength  Length in byte of the value to store in the buffer.
 **/
static void static_PTestFillBuffer(
            tContext* pContext,
            uint8_t* pBuffer,
            uint8_t nHeader,
            uint8_t nErrorCode,
            uint32_t nLength )
{
   /* Type of the command */
   pBuffer[0] = nHeader;
   /* Error Code */
   pBuffer[1] = nErrorCode;
   /* Result data length */
   static_PTestValueToBuffer( pContext, nLength, &pBuffer[2] );
}

/**
 * @brief   Cleans the test engine structure.
 *
 * @param[in]  pTestEngineInstance  The test engine instance.
 **/
static void static_PTestClearStructure(
            tTestEngineInstance* pTestEngineInstance )
{
   CDebugAssert( pTestEngineInstance != null );

   /* Clear the test information */
   pTestEngineInstance->nRunningTestId       = 0;
   pTestEngineInstance->bAutomaticMode       = false;
   pTestEngineInstance->nResultDataLength = 0;

   /* Clear the request & card information */
   pTestEngineInstance->nAutomaticResult     = 0;
   pTestEngineInstance->bRunningInteraction  = false;
}

/**
 * @brief   Reads the bundle data to get the test structure.
 *
 * @param[in]  pContext  Current context.
 *
 * @param[in]  pBundleData  Bundle data.
 *
 * @param[in]  nBundleDataLength  The compressed bundle data length in bytes
 *
 * @param[in]  nUncompressedSize  The uncompressed bundle data length in bytes
 *
 * @param[in]  nUUIDPosition  The bundle position where starts the UUID
 **/
static bool static_PTestReadBundle(
            tContext* pContext,
            uint8_t* pBundleData,
            uint32_t nBundleDataLength,
            uint32_t nUncompressedSize,
            uint32_t nUUIDPosition )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   uint8_t  i = 0x00;
   static const uint8_t pUUID[] = P_TEST_INTERFACE_UUID;
   uint32_t nValue = 0x00;
   tTestLinkTable* pLinkTable;
   uint32_t nCheckLength;
   uint32_t nZLibError;

   CDebugAssert( pTestEngineInstance != null );

   /* Create the complete buffer */
   pTestEngineInstance->pBundleData = CMemoryAlloc( nUncompressedSize );
   if ( pTestEngineInstance->pBundleData == null )
   {
      PDebugError("static_PTestReadBundle: could not allocate the buffer");
      return false;
   }

   pTestEngineInstance->nCurrentTestMemory += nUncompressedSize;
   if(pTestEngineInstance->nCurrentTestMemory > pTestEngineInstance->nPeakTestMemory)
   {
      pTestEngineInstance->nPeakTestMemory = pTestEngineInstance->nCurrentTestMemory;
   }
   nCheckLength = nUncompressedSize;
   /* uncompress the bundle data */
   nZLibError = WZLIB_uncompress(
         pContext,
         pTestEngineInstance->pBundleData,
         &nCheckLength,
         pBundleData,
         nBundleDataLength);
   if(nZLibError != ZLIB_OK)
   {
      PDebugError("static_PTestReadBundle: Error %d returned by ZLIB uncompress", nZLibError);
      goto return_error;
   }
   if(nCheckLength != nUncompressedSize)
   {
      PDebugError("static_PTestReadBundle: Error of length returned by ZLIB uncompress");
      goto return_error;
   }

   pBundleData = pTestEngineInstance->pBundleData;

   if(nUUIDPosition + sizeof(tTestLinkTable) >= nUncompressedSize)
   {
      PDebugError("static_PTestReadBundle: wrong position for the UUID");
      goto return_error;
   }

   pLinkTable = (tTestLinkTable*)&pBundleData[nUUIDPosition];

   /* Compare the UUID values */
   if ( CMemoryCompare( &pLinkTable->aInterfaceUUID, pUUID, P_TEST_INTERFACE_UUID_LENGTH ) != 0x00 )
   {
      PDebugError("static_PTestReadBundle: did not find the test engine UUID");
      goto return_error;
   }

   /* Compare the processor code */
   if ( pLinkTable->nProcessorCode != pTestEngineInstance->nProcessorCode )
   {
      PDebugError("static_PTestReadBundle: wrong processor code");
      goto return_error;
   }

   /* Checks the global variable size */
   if ( pLinkTable->nGlobalVariableSize != 0 )
   {
      PDebugError("static_PTestReadBundle: test code uses global variables");
      goto return_error;
   }

   pTestEngineInstance->nBundleDataLength = nUncompressedSize;

   /* Store the buffer start address */
   pTestEngineInstance->nStartAddress = (uint32_t)(pTestEngineInstance->pBundleData);

   /* Get the test structure address */
   pTestEngineInstance->pTestLinkTable = pLinkTable;

   /* Get the UUID address */
   nValue = pTestEngineInstance->pTestLinkTable->nDeltaReference;

   /* Get the difference between UUID compilation address and real address */
   if ( nValue > nUUIDPosition )
   {
      /* Store the offset information */
      pTestEngineInstance->bOffsetOrder  = false;
      pTestEngineInstance->nOffset       = nValue - nUUIDPosition;
   }
   else
   {
      /* Store the offset information */
      pTestEngineInstance->bOffsetOrder  = true;
      pTestEngineInstance->nOffset       = nUUIDPosition - nValue;
   }

   /* Check for the overlapping between the heap address and the link address */
   if((pTestEngineInstance->nOffset <= (pTestEngineInstance->nStartAddress + pTestEngineInstance->nBundleDataLength))
   && ((pTestEngineInstance->nOffset + pTestEngineInstance->nBundleDataLength) >= pTestEngineInstance->nStartAddress))
   {
      PDebugError("static_PTestReadBundle: overlapping between the heap address and the link address");
      goto return_error;
   }

   /* Get the test function addresses */
   for ( i=0; i<pTestEngineInstance->pTestLinkTable->nTestNumber; i++ )
   {
      /* Retrieve the address in the buffer */
      if ( pTestEngineInstance->bOffsetOrder != true )
      {
         nValue   = (uint32_t)(pTestEngineInstance->pTestLinkTable->aTestList[i].pEntryPoint)
                  - pTestEngineInstance->nOffset
                  + pTestEngineInstance->nStartAddress;
      }
      else
      {
         nValue   = (uint32_t)(pTestEngineInstance->pTestLinkTable->aTestList[i].pEntryPoint)
                  + pTestEngineInstance->nOffset
                  + pTestEngineInstance->nStartAddress;
      }

      /* Store the test function address */
      pTestEngineInstance->pTestLinkTable->aTestList[i].pEntryPoint = (tWTestEntryPoint*)nValue;
   }

   return true;

return_error:

   CMemoryFree(pTestEngineInstance->pBundleData);
   pTestEngineInstance->pBundleData = null;

   return false;
}

void static_PTestGetStatistics(
            tContext* pContext,
            uint8_t nFlags,
            tTestStatistics* pStatistics)
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
#if (P_BUILD_CONFIG == P_CONFIG_USER)
   tContextDriverMemoryStatistics sMemoryStatistics;
#endif /* P_CONFIG_USER */

   CMemoryFill(pStatistics, 0, sizeof(tTestStatistics));

   pStatistics->nFlags = nFlags;

   if((nFlags & P_TEST_STATISTIC_MEMORY) != 0)
   {
      pStatistics->nPeakTestMemory = pTestEngineInstance->nPeakTestMemory;
      pStatistics->nCurrentTestMemory = pTestEngineInstance->nCurrentTestMemory;

      /* Get the Memory statistics (user side) */
      CMemoryGetStatistics(
         &pStatistics->nCurrentUserMemory,
         &pStatistics->nPeakUserMemory );

#if (P_BUILD_CONFIG == P_CONFIG_USER)

      CMemoryFill(&sMemoryStatistics, 0, sizeof(sMemoryStatistics));

      PContextDriverGetMemoryStatistics(pContext,
         &sMemoryStatistics, sizeof(tContextDriverMemoryStatistics));

      /* @todo here, if the IOCTL failed, the sMemoryStatistics value will be set to zero */

      pStatistics->nCurrentDriverMemory = sMemoryStatistics.nCurrentMemory;
      pStatistics->nPeakDriverMemory = sMemoryStatistics.nPeakMemory;
#endif /* P_CONFIG_USER */
   }

   if((nFlags & P_TEST_STATISTIC_HANDLE) != 0)
   {
      PHandleGetCount(
         pContext,
         &pStatistics->nUserHandleNumber,
         &pStatistics->nDriverHandleNumber);
   }

   if((nFlags & P_TEST_STATISTIC_PROTOCOL) != 0)
   {
      PNALServiceDriverGetProtocolStatistics(pContext,
         &pStatistics->sProtocolStatistics, sizeof(tNALProtocolStatistics));
      /* @todo If the IOCTL failed, the protocols statistics are erroneous */
   }
}

static void static_PTestResetStatistics(
            tContext* pContext,
            uint8_t nFlags)
{
   /* Get the statistic type */
   if((nFlags & P_TEST_STATISTIC_MEMORY) != 0)
   {
      /* Reset the memory statistics */
      CMemoryResetStatistics( );
#if (P_BUILD_CONFIG == P_CONFIG_USER)
      /* Reset the driver memory statistics */
      PContextDriverResetMemoryStatistics(pContext);
      /*@todo here, if the IOCTL failed, the driver stats are not reset */

#endif /* P_CONFIG_USER */
   }

   if((nFlags & P_TEST_STATISTIC_PROTOCOL) != 0)
   {
      /* Reset the protocol statistics */
      PNALServiceDriverResetProtocolStatistics(pContext);
      /*@todo here, if the IOCTL failed, the driver stats are not reset */
   }
}

/**
 * @brief   Gets the statistic information in a buffer.
 *
 * @param[in]  pContext  Context of the test instance.
 *
 * @param[in]  pStatistics  The source of the statisitic data.
 *
 * @param[in]  pBuffer  Buffer to fill in.
 *
 * @return  The length of the generated command.
 **/
static uint32_t static_PTestGetStatisticsData(
            tContext* pContext,
            tTestStatistics* pStatistics,
            uint8_t* pBuffer )
{
   uint8_t* pStartBuffer = pBuffer;

   if((pStatistics->nFlags & P_TEST_STATISTIC_MEMORY) != 0)
   {
      static_PTestValueToBuffer( pContext, pStatistics->nCurrentUserMemory, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->nPeakUserMemory, pBuffer );
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->nCurrentDriverMemory, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->nPeakDriverMemory, pBuffer );
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->nCurrentTestMemory, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->nPeakTestMemory, pBuffer );
      pBuffer += 4;
   }

   if((pStatistics->nFlags & P_TEST_STATISTIC_PROTOCOL) != 0)
   {
      /* Fill in the return buffer */
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI5ReadMessageLost, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI5ReadByteErrorCount, pBuffer);
      pBuffer += 4;

      /* Fill in the return buffer */
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI6ReadMessageLost, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI6ReadByteErrorCount, pBuffer);
      pBuffer += 4;

      /* Fill in the return buffer */
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI4WindowSize, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI4ReadPayload, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI4ReadFrameLost, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI4ReadByteErrorCount, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI4WritePayload, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI4WriteFrameLost, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI4WriteByteErrorCount, pBuffer);
      pBuffer += 4;

      /* Fill in the return buffer */
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI2FrameReadByteErrorCount, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI2FrameReadByteTotalCount, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->sProtocolStatistics.nOSI2FrameWriteByteTotalCount, pBuffer);
      pBuffer += 4;
   }

   if((pStatistics->nFlags & P_TEST_STATISTIC_TIME) != 0)
   {
      static_PTestValueToBuffer( pContext, pStatistics->nCumulatedTime, pBuffer);
      pBuffer += 4;
   }

   if((pStatistics->nFlags & P_TEST_STATISTIC_HANDLE) != 0)
   {
      static_PTestValueToBuffer( pContext, pStatistics->nUserHandleNumber, pBuffer);
      pBuffer += 4;
      static_PTestValueToBuffer( pContext, pStatistics->nDriverHandleNumber, pBuffer);
      pBuffer += 4;
   }

   return pBuffer - pStartBuffer;
}

/* See header file */
const void* PTestGetConstAddress(
            tContext* pContext,
            const void* pConstData )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   CDebugAssert( pTestEngineInstance != null );

   /* Check if the test bundle is not already compiled in PIC */
   if(((uint32_t)(pConstData) >= pTestEngineInstance->nStartAddress)
   && ((uint32_t)(pConstData) < pTestEngineInstance->nStartAddress + pTestEngineInstance->nBundleDataLength))
   {
      return pConstData;
   }

   /* Check the offset order */
   if ( pTestEngineInstance->bOffsetOrder != true )
   {
      /* Return the correct address */
      return (const void*)((uint32_t)(pConstData)
               + pTestEngineInstance->nStartAddress
               - pTestEngineInstance->nOffset);
   }
   else
   {
      /* Return the correct address */
      return (const void*)((uint32_t)(pConstData)
               + pTestEngineInstance->nStartAddress
               + pTestEngineInstance->nOffset);
   }
}

/* See header file */
bool PTestIsInAutomaticMode(
            tContext* pContext )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   CDebugAssert( pTestEngineInstance != null );

   return pTestEngineInstance->bAutomaticMode;
}

/* See header file */
void PTestTraceInfo(
            tContext* pContext,
            const char * pMessage,
            va_list args )
{
#ifdef P_TRACE_ACTIVE
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   /* Display the trace */
   CDebugPrintTrace(
      pTestEngineInstance->aTestName,
      P_TRACE_TRACE,
      pMessage,
      args );
#endif /* P_TRACE_ACTIVE */
}

/* See header file */
void PTestTraceWarning(
            tContext* pContext,
            const char * pMessage,
            va_list args )
{
#ifdef P_TRACE_ACTIVE
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   /* Display the trace */
   CDebugPrintTrace(
      pTestEngineInstance->aTestName,
      P_TRACE_WARNING,
      pMessage,
      args );
#endif /* P_TRACE_ACTIVE */
}

/* See header file */
void PTestTraceError(
            tContext* pContext,
            const char * pMessage,
            va_list args )
{
#ifdef P_TRACE_ACTIVE
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   /* Display the trace */
   CDebugPrintTrace(
      pTestEngineInstance->aTestName,
      P_TRACE_WARNING,
      pMessage,
      args );
#endif /* P_TRACE_ACTIVE */
}

/* See header file */
void PTestTraceBuffer(
            tContext* pContext,
            const uint8_t* pBuffer,
            uint32_t nLength )
{
#ifdef P_TRACE_ACTIVE
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   /* Display the trace */
   CDebugPrintTraceBuffer(
      pTestEngineInstance->aTestName,
      P_TRACE_TRACE,
      pBuffer,
      nLength );
#endif /* P_TRACE_ACTIVE */
}

/* See header file */
void * PTestAlloc(
            tContext* pContext,
            uint32_t nSize )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   void* pBuffer = null;

   if(nSize != 0)
   {
      /* Call the system API */
      if((pBuffer = CMemoryAlloc( nSize + sizeof(uint32_t))) != null)
      {
         *((uint32_t*)pBuffer) = nSize;
         pBuffer = (void*)( ((uint8_t*)pBuffer) + sizeof(uint32_t));

         CMemoryFill(pBuffer, 0, nSize);

         pTestEngineInstance->nCurrentTestMemory += nSize + sizeof(uint32_t);
         if(pTestEngineInstance->nCurrentTestMemory > pTestEngineInstance->nPeakTestMemory)
         {
            pTestEngineInstance->nPeakTestMemory = pTestEngineInstance->nCurrentTestMemory;
         }
      }
   }

   return pBuffer;
}

/* See header file */
void PTestFree(
            tContext* pContext,
            void * pBuffer )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   if( pBuffer != null )
   {
      uint32_t nSize;

      pBuffer = (void*)( ((uint8_t*)pBuffer) - sizeof(uint32_t));
      nSize = *((uint32_t*)pBuffer);

      /* To be sure the buffer is not reused */
      CMemoryFill(pBuffer, 0xCC, nSize + sizeof(uint32_t));

      /* Call the system API */
      CMemoryFree ( pBuffer );

      CDebugAssert(pTestEngineInstance->nCurrentTestMemory >= nSize);
      pTestEngineInstance->nCurrentTestMemory -= nSize + sizeof(uint32_t);
   }
}

/* See header file */
void PTestSetTimer(
                  tContext* pContext,
                  uint32_t nTimeout,
                  tPBasicGenericCompletionFunction* pCallback,
                  void* pCallbackParameter )
{
   if(nTimeout == 0)
   {
      PMultiTimerCancelDriver(pContext, TIMER_T13_USER_TEST);
      /* @todo what can we do here if the driver call failed */
   }
   else
   {
      PMultiTimerSetDriver(pContext, TIMER_T13_USER_TEST,
                  PNALServiceDriverGetCurrentTime(pContext) + nTimeout,
                  pCallback, pCallbackParameter );
      /* @todo what can we do here if the driver call failed */
   }
}

/* See header file */
uint32_t PTestGetCurrentTime(
                  tContext* pContext )
{
   return PNALServiceDriverGetCurrentTime(pContext);
   /* @todo what can we do here if the driver call failed */
}

/* See header file */
void * WTestCopy(
            void * pDestination,
            void * pSource,
            uint32_t nLength )
{
   /* Call the system API */
   return CMemoryCopy( pDestination, (const void*)(pSource), nLength );
}

/* See header file */
void * WTestMove(
            void * pDestination,
            void * pSource,
            uint32_t nLength )
{
   /* Call the system API */
   return CMemoryMove( pDestination, (const void*)(pSource), nLength );
}

/* See header file */
void WTestFill(
            void * pBuffer,
            uint8_t nValue,
            uint32_t nLength )
{
   /* Call the system API */
   CMemoryFill( pBuffer, nValue, nLength );
}

/* See header file */
sint32_t WTestCompare(
            const void * pBuffer1,
            const void * pBuffer2,
            uint32_t nLength )
{
   /* Call the system API */
   return CMemoryCompare( pBuffer1, pBuffer2, nLength );
}

/* See header file */
uint32_t WTestConvertUTF16ToUTF8(
                  uint8_t* pDestUtf8,
                  const tchar* pSourceUtf16,
                  uint32_t nSourceCharLength )
{
   return PUtilConvertUTF16ToUTF8( pDestUtf8, pSourceUtf16, nSourceCharLength );
}

/* See header file */
uint32_t WTestWriteHexaUint8(
               tchar* pStringBuffer,
               uint8_t nValue)
{
   return PUtilWriteHexaUint8(pStringBuffer, nValue);
}

/* See header file */
uint32_t WTestWriteHexaUint32(
               tchar* pStringBuffer,
               uint32_t nValue)
{
   return PUtilWriteHexaUint32(pStringBuffer, nValue);
}

/* See header file */
uint32_t WTestWriteDecimalUint32(
               tchar* pStringBuffer,
               uint32_t nValue)
{
   return PUtilWriteDecimalUint32(pStringBuffer, nValue);
}

/* See header file */
uint32_t WTestStringLength(
               const tchar* pString )
{
   return PUtilStringLength( pString );
}

/* See header file */
sint32_t WTestStringCompare(
               const tchar* pString1,
               const tchar* pString2 )
{
   return PUtilStringCompare( pString1, pString2 );
}

/* See header file */
tchar* WTestStringCopy(
               tchar* pBuffer,
               uint32_t* pPos,
               const tchar* pString)
{
   return PUtilStringCopy( pBuffer, pPos, pString);
}

/* See header file */
void PTestNotifyEnd(
            tContext* pContext )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   uint8_t* pBuffer = &pTestEngineInstance->pResultData[6];
   tTestStatistics sEndStatistics;
   uint32_t nDataLength;

   CDebugAssert( pTestEngineInstance != null );

   if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
   {
      pTestEngineInstance->sStartStatistics.nCumulatedTime +=
         PNALServiceDriverGetCurrentTime(pContext) - pTestEngineInstance->sStartStatistics.nStartTime;
      /* @todo what can we do here if the driver call failed */
   }

   static_PTestGetStatistics(pContext,
      pTestEngineInstance->sStartStatistics.nFlags, &sEndStatistics);

   sEndStatistics.nCumulatedTime = pTestEngineInstance->sStartStatistics.nCumulatedTime;
   pTestEngineInstance->sStartStatistics.nCumulatedTime = 0;

   nDataLength = static_PTestGetStatisticsData( pContext,
            &pTestEngineInstance->sStartStatistics, pBuffer );
   nDataLength += static_PTestGetStatisticsData( pContext,
            &sEndStatistics, pBuffer + nDataLength );
   pBuffer += nDataLength;

   /* Check if a result has been received */
   if ( pTestEngineInstance->bIsResultSet == false )
   {
      /* Adding the result */
      static_PTestValueToBuffer( pContext, P_TEST_RESULT_ERROR_IN_TEST, pBuffer);
      nDataLength += 4;
   }
   else
   {
      /* Adding the result */
      static_PTestValueToBuffer( pContext, pTestEngineInstance->nResultErrorCode, pBuffer);
      pBuffer += 4;
      nDataLength += 4;

      if(pTestEngineInstance->nResultDataLength != 0)
      {
         /* Moving the data */
         CDebugAssert(nDataLength + 6 <= W_TEST_SERVER_RESPONSE_MAX_LENGTH);
         if((6 + nDataLength + pTestEngineInstance->nResultDataLength) > W_TEST_SERVER_RESPONSE_MAX_LENGTH)
         {
            pTestEngineInstance->nResultDataLength = W_TEST_SERVER_RESPONSE_MAX_LENGTH - nDataLength - 6;
            PDebugWarning("PTestNotifyEnd: Truncate the test data to fit in the buffer");
         }

         CMemoryMove(pBuffer,
            pTestEngineInstance->pResultData + W_TEST_SERVER_RESPONSE_MAX_LENGTH - pTestEngineInstance->nResultDataLength,
            pTestEngineInstance->nResultDataLength);

         nDataLength += pTestEngineInstance->nResultDataLength;
      }
   }

   /* Prepare the response */
   static_PTestFillBuffer(
      pContext,
      pTestEngineInstance->pResultData,
      P_TEST_MESSAGE_EXECUTE,
      P_TEST_ENGINE_RESULT_OK,
      nDataLength );

   /* Send the result to the test server */
   PDFCPostContext3(
      &pTestEngineInstance->sCallbackContext,
      P_DFC_TYPE_TEST_ENGINE,
      nDataLength + 6, W_SUCCESS );

   /* Clear the structure */
   static_PTestClearStructure( pTestEngineInstance );

   PContextTriggerEventPump(pContext);

}

/* See header file */
void PTestSetResult(
            tContext* pContext,
            uint32_t nResult,
            const void * pResultData,
            uint32_t nResultDataLength )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   CDebugAssert( pTestEngineInstance != null );

   if(pTestEngineInstance->bIsResultSet != false)
   {
      PDebugError("PTestSetResult: The test sets the result twice");
   }
   else
   {
      pTestEngineInstance->bIsResultSet = true;

      /* Calculate the length */
      pTestEngineInstance->nResultDataLength = nResultDataLength;
      pTestEngineInstance->nResultErrorCode = nResult;

      if(nResultDataLength + 6 > W_TEST_SERVER_RESPONSE_MAX_LENGTH)
      {
         PDebugWarning("PTestSetResult: Truncate the test answer to fit in the buffer");
         nResultDataLength = W_TEST_SERVER_RESPONSE_MAX_LENGTH - 6;
      }

      /* Store the result data */
      if (  ( nResultDataLength != 0 )
         && ( pResultData != null ) )
      {
         CMemoryCopy(
            pTestEngineInstance->pResultData + W_TEST_SERVER_RESPONSE_MAX_LENGTH - nResultDataLength,
            pResultData,
            nResultDataLength );
      }
   }
}

/* See header file */
void PTestMessageBox(
            tContext* pContext,
            uint32_t nFlags,
            const char * pMessage,
            uint32_t nAutomaticResult,
            tPTestMessageBoxCompleted * pCallback,
            void * pCallbackParameter )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   uint32_t nValue = 0x00;

   CDebugAssert( pTestEngineInstance != null );

   /* Check if an interaction is pending */
   if ( pTestEngineInstance->bRunningInteraction == true )
   {
      PDebugError("Message box - a message is already displayed");
      return;
   }

   PDFCFillCallbackContext(
                  pContext,
                  (tDFCCallback*)pCallback,
                  pCallbackParameter,
                  &pTestEngineInstance->aInteractionCallback );

   /* Check if the test is in automatic mode */
   if ( pTestEngineInstance->bAutomaticMode != false )
   {
      PDebugTrace("Message box - a test is running in automatic mode");
      /* Call the callback with the expected result */
      PDFCPostContext2(
         &pTestEngineInstance->aInteractionCallback,
         P_DFC_TYPE_TEST_ENGINE,
         nAutomaticResult );
   }
   else
   {
      /* Set the displayed message box/card flag */
      pTestEngineInstance->bRunningInteraction = true;

      if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
      {
         pTestEngineInstance->sStartStatistics.nCumulatedTime +=
            PNALServiceDriverGetCurrentTime(pContext) - pTestEngineInstance->sStartStatistics.nStartTime;
         /* @todo what can we do here if the driver call failed */
      }

      /* Store the callback and callback parameter */
      pTestEngineInstance->nAutomaticResult = nAutomaticResult;

      /* Send the request to the test server */
      /* - type of message */
      pTestEngineInstance->pResultData[0] = P_TEST_MESSAGE_BOX;
      /* - request data length */
      nValue = static_PTestStrlen(pMessage);
      static_PTestValueToBuffer(
         pContext,
         4 + nValue,
         &pTestEngineInstance->pResultData[1]);
      /* - request flags */
      static_PTestValueToBuffer(
         pContext,
         nFlags,
         &pTestEngineInstance->pResultData[5]);
      /* - request data */
      if ( nValue != 0x00)
      {
         CMemoryCopy(
            &pTestEngineInstance->pResultData[9],
            (void*)pMessage,
            nValue );
      }

      /* Call the test server callback */
      PDFCPostContext3(
         &pTestEngineInstance->sCallbackContext,
         P_DFC_TYPE_TEST_ENGINE,
         nValue + 9, W_SUCCESS );
   }
}

/* See header file */
tTestExecuteContext * PTestGetExecuteContext( tContext* pContext )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   return &(pTestEngineInstance->sExecuteContext);
}

/* See header file */
void PTestExecuteRemoteFunction(
                                tContext* pContext,
                                const char* pFunctionIdentifier,
                                uint32_t nParameter,
                                const uint8_t* pParameterBuffer,
                                uint32_t nParameterBufferLength,
                                uint8_t* pResultBuffer,
                                uint32_t nResultBufferLength,
                                tPBasicGenericDataCallbackFunction* pCallback,
                                void* pCallbackParameter )
{
   uint32_t nLength;
   uint32_t nPos;

   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );

   /* Check if an interaction is pending */
   if ( pTestEngineInstance->bRunningInteraction == true )
   {
      PDebugError("PTestExecuteRemoteFunction - an interaction is pending");
      return;
   }

   /* Set the displayed message box/card flag */
   pTestEngineInstance->bRunningInteraction = true;

   if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
   {
      pTestEngineInstance->sStartStatistics.nCumulatedTime +=
         PNALServiceDriverGetCurrentTime(pContext) - pTestEngineInstance->sStartStatistics.nStartTime;
      /* @todo what can we do here if the driver call failed */
   }

   /*Configure the result buffer*/
   pTestEngineInstance->pResultBuffer= pResultBuffer;
   pTestEngineInstance->nResultBufferLength= nResultBufferLength;

   /* Store the callback and callback parameter */
   pTestEngineInstance->nAutomaticResult = P_TEST_MESSAGE_AUTOMATIC;

   /* Send the request to the test server */
   /* - type of message */

   nPos=0;
   pTestEngineInstance->pResultData[nPos] = P_TEST_MESSAGE_EXECUTE_REMOTE;
   nPos+=1;
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &pTestEngineInstance->aInteractionCallback );


   nLength = static_PTestStrlen(pFunctionIdentifier)+nParameterBufferLength+4*3;

   /* - request data length */
   static_PTestValueToBuffer(
      pContext,
      nLength,
      &pTestEngineInstance->pResultData[nPos]);
   nPos+=4;

   /* - parameter */
   static_PTestValueToBuffer(
      pContext,
      nParameter,
      &pTestEngineInstance->pResultData[nPos]);
   nPos+=4;

   /* function name string length */
   static_PTestValueToBuffer(
             pContext,
             static_PTestStrlen(pFunctionIdentifier),
             &pTestEngineInstance->pResultData[nPos]);
   nPos+= 4;
   /* function name string */
   CMemoryCopy(
      &pTestEngineInstance->pResultData[nPos],
      (void*)pFunctionIdentifier,
      static_PTestStrlen((char *)pFunctionIdentifier));
   nPos+=  static_PTestStrlen((char *)pFunctionIdentifier);
   /* function name string nParameterBufferLength */
   static_PTestValueToBuffer(
      pContext,
      nParameterBufferLength,
      &pTestEngineInstance->pResultData[nPos]);
   nPos+=4;
   if(nParameterBufferLength>0)
   {
      CMemoryCopy(
         &pTestEngineInstance->pResultData[nPos],
         (void*)pParameterBuffer,
         nParameterBufferLength);
   }

   /* Call the test server callback */
   PDFCPostContext3(
      &pTestEngineInstance->sCallbackContext,
      P_DFC_TYPE_TEST_ENGINE,
      nLength+5, W_SUCCESS);
}

/* See header file */
void PTestPresentObject(
            tContext* pContext,
            const char * pObjectName,
            const char * pOperatorMessage,
            uint32_t nDistance,
            tPBasicGenericCallbackFunction * pCallback,
            void * pCallbackParameter )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   uint32_t nOperatorMessageLength = 0;
   uint32_t nObjectNameLength = 0;
   uint32_t nIndex = 0;

   CDebugAssert( pTestEngineInstance != null );

   /* Check if an interaction is pending */
   if ( pTestEngineInstance->bRunningInteraction == true )
   {
      PDebugError("Presentation robot message - an interaction is pending");
      return;
   }

   PDFCFillCallbackContext(
                  pContext,
                  (tDFCCallback*)pCallback, pCallbackParameter,
                  &pTestEngineInstance->aInteractionCallback );

   /* Set the displayed message box/card flag */
   pTestEngineInstance->bRunningInteraction = true;

   if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
   {
      pTestEngineInstance->sStartStatistics.nCumulatedTime +=
         PNALServiceDriverGetCurrentTime(pContext) - pTestEngineInstance->sStartStatistics.nStartTime;
      /* @todo what can we do here if the driver call failed */
   }

   /* Send the request to the test server */
   /* - type of message */
   pTestEngineInstance->pResultData[nIndex++] = P_TEST_MESSAGE_PRESENT_OBJECT;
   /* - request data length */
   nOperatorMessageLength = static_PTestStrlen(pOperatorMessage);
   nObjectNameLength = static_PTestStrlen(pObjectName);

   static_PTestValueToBuffer(pContext,
      nObjectNameLength, &pTestEngineInstance->pResultData[nIndex]);
   nIndex += 4;

   static_PTestValueToBuffer(pContext,
      nOperatorMessageLength, &pTestEngineInstance->pResultData[nIndex]);
   nIndex += 4;

   static_PTestValueToBuffer(pContext,
      nDistance, &pTestEngineInstance->pResultData[nIndex]);
   nIndex += 4;

   /* - request card name */
   if ( nObjectNameLength != 0 )
   {
      CMemoryCopy(
         &pTestEngineInstance->pResultData[nIndex],
         (void*)pObjectName, nObjectNameLength );
      nIndex += nObjectNameLength;
   }

   if ( nOperatorMessageLength != 0 )
   {
      CMemoryCopy(
         &pTestEngineInstance->pResultData[nIndex],
         (void*)pOperatorMessage, nOperatorMessageLength );
      nIndex += nOperatorMessageLength;
   }

   PDFCPostContext3(
      &pTestEngineInstance->sCallbackContext,
      P_DFC_TYPE_TEST_ENGINE, nIndex, W_SUCCESS );
}

/* See header file */
void PTestRemoveObject(
            tContext* pContext,
            const char* pOperatorMessage,
            bool bSaveState,
            bool bCheckUnmodifiedState,
            tPBasicGenericCallbackFunction * pCallback,
            void * pCallbackParameter )
{
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   uint32_t nOperatorMessageLength = 0;
   uint32_t nIndex = 0;
   uint32_t nFlags = 0;

   CDebugAssert( pTestEngineInstance != null );

   /* Check if an interaction is pending */
   if ( pTestEngineInstance->bRunningInteraction == true )
   {
      PDebugError("Presentation robot message - an interaction is pending");
      return;
   }

   PDFCFillCallbackContext(
                  pContext,
                  (tDFCCallback*)pCallback, pCallbackParameter,
                  &pTestEngineInstance->aInteractionCallback );

   /* Set the displayed message box/card flag */
   pTestEngineInstance->bRunningInteraction = true;

   if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
   {
      pTestEngineInstance->sStartStatistics.nCumulatedTime +=
         PNALServiceDriverGetCurrentTime(pContext) - pTestEngineInstance->sStartStatistics.nStartTime;
      /* @todo what can we do here if the driver call failed */
   }

   /* Send the request to the test server */
   /* - type of message */
   pTestEngineInstance->pResultData[nIndex++] = P_TEST_MESSAGE_REMOVE_OBJECT;
   /* - request data length */
   nOperatorMessageLength = static_PTestStrlen(pOperatorMessage);

   static_PTestValueToBuffer(pContext,
      nOperatorMessageLength, &pTestEngineInstance->pResultData[nIndex]);
   nIndex += 4;

   if(bSaveState != false)
   {
      nFlags |= 0x01;
   }
   if(bCheckUnmodifiedState != false)
   {
      nFlags |= 0x02;
   }
   static_PTestValueToBuffer(pContext,
      nFlags, &pTestEngineInstance->pResultData[nIndex]);
   nIndex += 4;

   if ( nOperatorMessageLength != 0 )
   {
      CMemoryCopy(
         &pTestEngineInstance->pResultData[nIndex],
         (void*)pOperatorMessage, nOperatorMessageLength );
      nIndex += nOperatorMessageLength;
   }

   /* Call the test server callback */
   PDFCPostContext3(
      &pTestEngineInstance->sCallbackContext,
      P_DFC_TYPE_TEST_ENGINE, nIndex, W_SUCCESS );
}

/* See Client API Specifications */
void PBasicTestExchangeMessage(
            tContext* pContext,
            tPBasicGenericDataCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t* pSendMessageBuffer,
            uint32_t nSendMessageBufferLength,
            uint8_t* pReceivedMessageBuffer )
{
   /* Generic values */
   tTestEngineInstance* pTestEngineInstance = PContextGetTestEngineInstance( pContext );
   uint8_t  i = 0x00;
   uint8_t  j = 0x00;
   uint8_t  k = 0x00;
   uint32_t nReceivedMessageLength = 0;
   uint32_t nValue = 0;
   uint32_t nValue2 = 0;
   uint32_t nValue3 = 0;
   uint32_t nValue4 = 0;
   static const uint8_t  pProtocolVersion[] = P_TEST_PROTOCOL_VERSION;
   static const uint8_t  pUUID[] = P_TEST_INTERFACE_UUID;
   static const char* pProductName = P_TEST_PRODUCT_NAME;
   tTestExecuteContext *ptExecuteContext;
   CDebugAssert( pTestEngineInstance != null );

   PDFCFillCallbackContext(
                  pContext,
                  (tDFCCallback*)pCallback,
                  pCallbackParameter,
                  &pTestEngineInstance->sCallbackContext );

   /* Check the parameters */
   if ( pSendMessageBuffer == null )
   {
      PDebugError("Exchange Message - pSendMessageBuffer == null");
      goto exchange_error;
   }

   /* If the test engine has not been initialised */
   /* and the current message is not an initialising one */
   if (  ( pTestEngineInstance->nState == P_TEST_ENGINE_OFF )
      && ( pSendMessageBuffer[0] != P_TEST_MESSAGE_INIT)
      && ( pSendMessageBuffer[0] != P_TEST_MESSAGE_PING) )
   {
      PDebugError("Exchange Message - initialise the test engine first");
      goto exchange_error;
   }

   /* Get the type of message */
   switch ( pSendMessageBuffer[0] )
   {
      case P_TEST_MESSAGE_PING:
         PDebugTrace("Exchange Message - PING");

         /* Check the length of the data */
         nValue = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[1] );
         if ( nValue != 0x00 )
         {
            PDebugError(
               "Exchange Message - wrong length 0x%08X",
               nValue );

            goto exchange_error;
         }

         /* Prepare the ping response */
         static_PTestFillBuffer(
            pContext,
            pReceivedMessageBuffer,
            pSendMessageBuffer[0],
            P_TEST_ENGINE_RESULT_OK,
            0x00 );

         /* Call the test server callback */
         PDFCPostContext3(
            &pTestEngineInstance->sCallbackContext,
            P_DFC_TYPE_TEST_ENGINE,
            6, W_SUCCESS );
         break;

      case P_TEST_MESSAGE_EXECUTE_REMOTE:
          PDebugTrace("Execute Remote function");

          /* Store the callback information */
         pTestEngineInstance->pResultData         = pReceivedMessageBuffer;
         pTestEngineInstance->nResultDataLength   = 0;

         /* Reset the displayed message box/card flag */
         pTestEngineInstance->bRunningInteraction = false;

         /* Get the length of the data */
         nValue = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[1] );

         nValue2 = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[5] );
         /* Get the length of the result */
         nValue3 = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[9] );
         if(nValue3>pTestEngineInstance->nResultBufferLength)
         {
            PDebugError("Input buffer to small");
            goto exchange_error;
         }
         pTestEngineInstance->nResultBufferLength=nValue3;

         if(pTestEngineInstance->nResultBufferLength>0)
         {
            CMemoryCopy( pTestEngineInstance->pResultBuffer, &pSendMessageBuffer[13], pTestEngineInstance->nResultBufferLength);
         }

         /* Call the test request callback */
         PDFCPostContext3(
            &pTestEngineInstance->aInteractionCallback,
            P_DFC_TYPE_TEST_ENGINE,
            pTestEngineInstance->nResultBufferLength,
            nValue2);

         if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
         {
            pTestEngineInstance->sStartStatistics.nStartTime = PNALServiceDriverGetCurrentTime(pContext);
         }
         break;
      case P_TEST_MESSAGE_INIT:
         {
            uint32_t nBuildNumber = OPEN_NFC_BUILD_NUMBER;

            PDebugTrace("Exchange Message - INIT");

            /* Set the little endian flag to FALSE */
            pTestEngineInstance->bLittleEndian = false;

            /* Check the length of the data */
            nValue = static_PTestBufferToValue(
                        pContext,
                        &pSendMessageBuffer[1] );
            if ( nValue != 0x04 )
            {
               /* Check if the command is configured in little indean */
               if ( nValue == 0x04000000 )
               {
                  /* Set the little endian flag to FALSE */
                  pTestEngineInstance->bLittleEndian = true;
               }
               else
               {
                  PDebugError(
                     "Exchange Message - wrong length 0x%08X",
                     nValue );
                  /* Set the test engine state */
                  pTestEngineInstance->nState = P_TEST_ENGINE_OFF;

                  goto exchange_error;
               }
            }

            /* Compare the protocol version number with the one received from the test server */
            if ( CMemoryCompare( &pSendMessageBuffer[5], pProtocolVersion, P_TEST_VERSION_NUMBER_LENGTH ) != 0x00 )
            {
               PDebugError(
                  "Exchange Message - wrong version number %02d.%02d.%02d.%02d",
                  pSendMessageBuffer[5],
                  pSendMessageBuffer[6],
                  pSendMessageBuffer[7],
                  pSendMessageBuffer[8] );

               /* Set the test engine state */
               pTestEngineInstance->nState = P_TEST_ENGINE_OFF;

               goto exchange_error;
            }

            /* Initialize the test memory consumption */
            pTestEngineInstance->nPeakTestMemory = 0;
            pTestEngineInstance->nCurrentTestMemory = 0;

            /* Set the test engine state */
            pTestEngineInstance->nState = P_TEST_ENGINE_INIT;

            /* Prepare the init response */
            static_PTestFillBuffer(
               pContext,
               pReceivedMessageBuffer,
               pSendMessageBuffer[0],
               P_TEST_ENGINE_RESULT_OK,
               1 /* processor code*/
               + P_TEST_PRODUCT_NAME_LENGTH
               + P_TEST_VERSION_NUMBER_LENGTH
               + P_TEST_BUILD_NUMBER_LENGTH
               + P_TEST_INTERFACE_UUID_LENGTH );

            nValue = 6;
            /* Processor code */
            pReceivedMessageBuffer[nValue++] = pTestEngineInstance->nProcessorCode;
            /* Product name */
            for(i = 0; i < P_TEST_PRODUCT_NAME_LENGTH; i++)
            {
               if(pProductName[i] != 0)
               {
                  pReceivedMessageBuffer[nValue++] = pProductName[i];
               }
               else
               {
                  break;
               }
            }

            while(i++ < P_TEST_PRODUCT_NAME_LENGTH)
            {
               pReceivedMessageBuffer[nValue++] = 0x20;
            }

            /* Version number */
            CMemoryCopy( &pReceivedMessageBuffer[nValue], pProtocolVersion, P_TEST_VERSION_NUMBER_LENGTH );
            nValue += P_TEST_VERSION_NUMBER_LENGTH;
            /* Build number */
            static_PTestValueToBuffer( pContext, nBuildNumber, &pReceivedMessageBuffer[nValue]);
            nValue += P_TEST_BUILD_NUMBER_LENGTH;
            /* UUID */
            CMemoryCopy( &pReceivedMessageBuffer[nValue], pUUID, P_TEST_INTERFACE_UUID_LENGTH );
            nValue += P_TEST_INTERFACE_UUID_LENGTH;

            /* Call the test server callback */
            PDFCPostContext3(
               &pTestEngineInstance->sCallbackContext,
               P_DFC_TYPE_TEST_ENGINE,
               nValue, W_SUCCESS );
         }
         break;

      case P_TEST_MESSAGE_DOWNLOAD:
         /* Get the bundle data length */
         nValue = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[1] );

         /* Check the download type */
         if ( nValue != 0x00 )
         {
            PDebugTrace("Exchange Message - DOWNLOAD");
         }
         else
         {
            PDebugTrace("Exchange Message - UNLOAD");
         }

         /* Check if a current bundle is loaded */
         /* and a test is still running */
         if (  ( pTestEngineInstance->nState == P_TEST_ENGINE_LOADED )
            && ( pTestEngineInstance->nRunningTestId != 0 ) )
         {
            PDebugError(
               "Exchange Message - test 0x%08X currently running",
               pTestEngineInstance->nRunningTestId );
            goto exchange_error;
         }

         /* Unload the current bundle data, if any */
         CMemoryFree( pTestEngineInstance->pBundleData );
         CDebugAssert(pTestEngineInstance->nCurrentTestMemory >= pTestEngineInstance->nBundleDataLength);
         pTestEngineInstance->nCurrentTestMemory -= pTestEngineInstance->nBundleDataLength;

         /* Clear the bundle information */
         pTestEngineInstance->nState         = P_TEST_ENGINE_INIT;
         pTestEngineInstance->pBundleData    = null;
         pTestEngineInstance->nBundleDataLength = 0;
         pTestEngineInstance->pTestLinkTable = null;
         pTestEngineInstance->nStartAddress  = 0;
         pTestEngineInstance->nOffset        = 0;

         /* Clear the structure */
         static_PTestClearStructure( pTestEngineInstance );

         /* If it is not an unload message */
         if ( nValue != 0x00 )
         {
            /* Get the bundle position where starts the UUID */
            uint32_t nUUIDPosition = static_PTestBufferToValue(pContext, &pSendMessageBuffer[5]);
            uint32_t nUncompressedSize = static_PTestBufferToValue(pContext, &pSendMessageBuffer[9]);

            /* Get the bundle position in the buffer and store it */
            if ( static_PTestReadBundle( pContext, &pSendMessageBuffer[13], nValue - 8, nUncompressedSize, nUUIDPosition ) != true )
            {
               PDebugError("Exchange Message - could not read the bundle data");
               goto exchange_error;
            }

            /* Set the test engine state */
            pTestEngineInstance->nState = P_TEST_ENGINE_LOADED;
         }

         /* Prepare the download response */
         static_PTestFillBuffer(
            pContext,
            pReceivedMessageBuffer,
            pSendMessageBuffer[0],
            P_TEST_ENGINE_RESULT_OK,
            0x00 );

         /* Call the test server callback */
         PDFCPostContext3(
            &pTestEngineInstance->sCallbackContext,
            P_DFC_TYPE_TEST_ENGINE,
            6, W_SUCCESS );
         break;

      case P_TEST_MESSAGE_EXECUTE:
         PDebugTrace("Exchange Message - EXECUTE");

         /* Check if a bundle has been loaded */
         if ( pTestEngineInstance->nState == P_TEST_ENGINE_LOADED )
         {
            /* Check if no test is currently running */
            if ( pTestEngineInstance->nRunningTestId == 0x00 )
            {
               /* Check the length of the data */
               nValue = static_PTestBufferToValue(
                           pContext,
                           &pSendMessageBuffer[1] );
               if ( nValue >=0x09 )
               {
                  /* Get the test identifier */
                  nValue = static_PTestBufferToValue(
                              pContext,
                              &pSendMessageBuffer[6] );

                  /* Search for the test number in the currently loaded bundle */
                  for ( i=0; i<pTestEngineInstance->pTestLinkTable->nTestNumber; i++ )
                  {
                     /* Compare the identifier */
                     if ( nValue == pTestEngineInstance->pTestLinkTable->aTestList[i].nIdentifier )
                     {
                        tWTestEntryPoint* pEntryPoint;

                        /* Store the test information */
                        pTestEngineInstance->nRunningTestId = nValue;
                        nValue3 = 0;
                        for (j=0; j<5; j++)
                        {
                           nValue4 = 1;
                           for (k=j; k<4; k++)
                           {
                              nValue4 *= 10;
                           }
                           nValue2 = (uint8_t)((uint8_t)((nValue - nValue3 ) / nValue4));
                           nValue3 += nValue2 * nValue4;
                           pTestEngineInstance->aTestName[j]     = (char)(0x30 + nValue2);
                        }
                        pTestEngineInstance->pResultData         = pReceivedMessageBuffer;
                        pTestEngineInstance->nResultDataLength   = 0;
                        pTestEngineInstance->bIsResultSet = false;

                        /* Check if the test is in automatic mode or not */
                        if ( pSendMessageBuffer[5] & P_TEST_MESSAGE_AUTOMATIC )
                        {
                           pTestEngineInstance->bAutomaticMode = true;
                        }
                        else
                        {
                           pTestEngineInstance->bAutomaticMode = false;
                        }

                        pEntryPoint = (tWTestEntryPoint*)(pTestEngineInstance->pTestLinkTable->aTestList[i].pEntryPoint);

                        /* Reset the test peak memory */
                        pTestEngineInstance->nPeakTestMemory = pTestEngineInstance->nCurrentTestMemory;

                        static_PTestGetStatistics(pContext, pSendMessageBuffer[5],
                           &pTestEngineInstance->sStartStatistics);

                        if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
                        {
                           pTestEngineInstance->sStartStatistics.nStartTime = PNALServiceDriverGetCurrentTime(pContext);
                           pTestEngineInstance->sStartStatistics.nCumulatedTime = 0;
                        }

                        nValue = static_PTestBufferToValue(
                           pContext,
                           &pSendMessageBuffer[10] );
                        ptExecuteContext=PTestGetExecuteContext(pContext);
                        ptExecuteContext->nInputData=0;
                        ptExecuteContext->pInputData=null;


                        /*Read the optional test remote parameter*/
                        /* Get the test identifier */
                        if(nValue==0)
                        {
                            ptExecuteContext->bValid=false;
                        }
                        else
                        {
                           ptExecuteContext->bValid=true;

                           if(nValue>=4)
                           {
                              nValue2=static_PTestBufferToValue(
                                 pContext,
                                 &pSendMessageBuffer[14] );
                              ptExecuteContext->nParam=nValue2;
                              if(nValue>4)
                              {
                                 ptExecuteContext->nInputData=nValue-4;
                                 ptExecuteContext->pInputData= CMemoryAlloc( nValue-4 );
                                 if (ptExecuteContext->pInputData== null )
                                 {
                                    PDebugError("PBasicTestExchangeMessage: could not allocate the buffer");
                                 }
                                 else
                                 {
                                    CMemoryCopy(
                                    ptExecuteContext->pInputData,
                                    &pSendMessageBuffer[18],
                                    nValue-4);
                                 }
                              }
                           }
                        }
                        /* Release the context protection before calling the test */
                        PContextReleaseLock(pContext);

                        /* Launch the corresponding test */
                        pEntryPoint( (tTestAPI*)(&g_aTestAPI) );

                        /* Get the context protection after calling the test */
                        PContextLock(pContext);
                        if(ptExecuteContext->pInputData!=null)
                        {
                           CMemoryFree(ptExecuteContext->pInputData);
                           ptExecuteContext->pInputData=null;
                        }
                        PDebugTrace("Exchange Message - EXECUTE Finish");
                        return;
                     }
                  }

                  PDebugError(
                     "Exchange Message - test 0x%08X unknown",
                     nValue );
               }
               else
               {
                  PDebugError(
                     "Exchange Message - wrong length 0x%08X",
                     nValue );
               }
            }
            else
            {
               PDebugError(
                  "Exchange Message - test 0x%08X currently running",
                  pTestEngineInstance->nRunningTestId );
            }
         }
         else
         {
            PDebugError("Exchange Message - no bundle loaded");
         }
         goto exchange_error;

      case P_TEST_MESSAGE_RESETSTAT:

         PDebugTrace("Exchange Message - RESETSTAT");

         /* Check the length of the data */
         nValue = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[1] );
         if ( nValue != 0x01 )
         {
            PDebugError("Exchange Message - message too long");
            goto exchange_error;
         }

         static_PTestResetStatistics(pContext, pSendMessageBuffer[5]);

         /* Prepare the reset statistic response */
         static_PTestFillBuffer(
            pContext,
            pReceivedMessageBuffer,
            pSendMessageBuffer[0],
            P_TEST_ENGINE_RESULT_OK,
            0x00 );

         /* Call the test server callback */
         PDFCPostContext3(
            &pTestEngineInstance->sCallbackContext,
            P_DFC_TYPE_TEST_ENGINE,
            6, W_SUCCESS );
         break;

      case P_TEST_MESSAGE_GETSTAT:
         {
            tTestStatistics sStatistics;

            PDebugTrace("Exchange Message - GETSTAT");

            /* Check the length of the data */
            nValue = static_PTestBufferToValue(
                        pContext,
                        &pSendMessageBuffer[1] );
            if ( nValue != 0x01 )
            {
               PDebugError("Exchange Message - message too long");
               goto exchange_error;
            }

            static_PTestGetStatistics(pContext, pSendMessageBuffer[5], &sStatistics);

            /* Get the statistic information */
            if ( (nReceivedMessageLength = static_PTestGetStatisticsData(
                                                pContext,
                                                &sStatistics,
                                                &pReceivedMessageBuffer[6] ) ) == 0x00 )
            {
               PDebugError(
                  "Exchange Message - wrong statistic type 0x%02X",
                  pSendMessageBuffer[5] );
               goto exchange_error;
            }

            /* Prepare the reset statistic response */
            static_PTestFillBuffer(
               pContext,
               pReceivedMessageBuffer,
               pSendMessageBuffer[0],
               P_TEST_ENGINE_RESULT_OK,
               nReceivedMessageLength );

            /* Call the test server callback */
            PDFCPostContext3(
               &pTestEngineInstance->sCallbackContext,
               P_DFC_TYPE_TEST_ENGINE,
               nReceivedMessageLength + 6, W_SUCCESS );
         }
         break;

      case P_TEST_MESSAGE_BOX:
         PDebugTrace("Exchange Message - BOX");

         /* Store the callback information */
         pTestEngineInstance->pResultData         = pReceivedMessageBuffer;
         pTestEngineInstance->nResultDataLength   = 0;
         /* Reset the displayed message box/card flag */
         pTestEngineInstance->bRunningInteraction = false;

         /* Get the length of the data */
         nValue = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[2] );

         /* Check the result or the length of the answer */
         if (  ( pSendMessageBuffer[1] != P_TEST_ENGINE_RESULT_OK )
            || ( nValue != 0x04 ) )
         {
            PDebugError(
               "Exchange Message - error (0x%02X) or wrong length (0x%08X)",
               pSendMessageBuffer[1],
               nValue );

            /* Call the test request callback */
            PDFCPostContext2(
               &pTestEngineInstance->aInteractionCallback,
               P_DFC_TYPE_TEST_ENGINE,
               P_TEST_RESULT_CANCELLED );
         }
         else
         {
            /* Get the code of the button pressed by the operator */
            nValue = static_PTestBufferToValue(
                        pContext,
                        &pSendMessageBuffer[6] );

            /* Call the test request callback */
            PDFCPostContext2(
               &pTestEngineInstance->aInteractionCallback,
               P_DFC_TYPE_TEST_ENGINE,
               nValue );
         }

         if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
         {
            pTestEngineInstance->sStartStatistics.nStartTime = PNALServiceDriverGetCurrentTime(pContext);
         }

         break;

      case P_TEST_MESSAGE_PRESENT_OBJECT:
      case P_TEST_MESSAGE_REMOVE_OBJECT:
         PDebugTrace("Exchange Message - PRESENT OBJECT or REMOVE OBJECT");

         /* Store the callback information */
         pTestEngineInstance->pResultData         = pReceivedMessageBuffer;
         pTestEngineInstance->nResultDataLength   = 0;
         /* Reset the displayed message box/card flag */
         pTestEngineInstance->bRunningInteraction = false;

         /* Get the length of the data */
         nValue = static_PTestBufferToValue(
                     pContext,
                     &pSendMessageBuffer[2] );

         /* Check the result or the length of the answer */
         if (  ( pSendMessageBuffer[1] != P_TEST_ENGINE_RESULT_OK )
            || ( nValue != 0x04 ) )
         {
            PDebugError(
               "Exchange Message - error (0x%02X) or wrong length (0x%08X)",
               pSendMessageBuffer[1],
               nValue );

            /* Call the test request callback */
            PDFCPostContext2(
               &pTestEngineInstance->aInteractionCallback,
               P_DFC_TYPE_TEST_ENGINE,
               P_TEST_RESULT_CANCELLED );
         }
         else
         {
            /* Get the result */
            nValue = static_PTestBufferToValue(
                        pContext,
                        &pSendMessageBuffer[6] );

            /* Get the correct error code */
            switch ( nValue )
            {
               case P_TEST_RESULT_PASSED:
                  nValue2 = W_SUCCESS;
                  break;
               case P_TEST_RESULT_CANCELLED:
                  nValue2 = W_ERROR_CANCEL;
                  break;
               default:
                  nValue2 = W_ERROR_BAD_PARAMETER;
                  PDebugError(
                     "Exchange Message - wrong result value 0x%08X",
                     nValue );
                  break;
            }

            /* Call the test request callback */
            PDFCPostContext2(
               &pTestEngineInstance->aInteractionCallback,
               P_DFC_TYPE_TEST_ENGINE,
               nValue2 );
         }

         if((pTestEngineInstance->sStartStatistics.nFlags & P_TEST_STATISTIC_TIME) != 0)
         {
            pTestEngineInstance->sStartStatistics.nStartTime = PNALServiceDriverGetCurrentTime(pContext);
         }

         break;

      default:
         PDebugError(
            "Exchange Message - wrong message type 0x%02X",
            pSendMessageBuffer[0] );
         goto exchange_error;
   }

   return;

exchange_error:
   /* Prepare the error response */
   static_PTestFillBuffer(
      pContext,
      pReceivedMessageBuffer,
      pSendMessageBuffer[0],
      P_TEST_ENGINE_RESULT_ERROR,
      0x00 );

   /* Call the test server callback */
   PDFCPostContext3(
      &pTestEngineInstance->sCallbackContext,
      P_DFC_TYPE_TEST_ENGINE,
      6, W_SUCCESS );
}

/* See header file */
void PTestEngineCreate(
            tTestEngineInstance* pTestEngineInstance )
{
   CMemoryFill(pTestEngineInstance, 0, sizeof(tTestEngineInstance));

   /* Clear the bundle information */
   pTestEngineInstance->nState         = P_TEST_ENGINE_OFF;
   pTestEngineInstance->nProcessorCode = P_TEST_PROCESSOR;
   pTestEngineInstance->pBundleData    = null;
   pTestEngineInstance->nBundleDataLength = 0;
   pTestEngineInstance->pTestLinkTable = null;
   pTestEngineInstance->nStartAddress  = 0;
   pTestEngineInstance->nOffset        = 0;

   /* Clear the structure */
   static_PTestClearStructure( pTestEngineInstance );
}

/* See header file */
void PTestEngineDestroy(
            tTestEngineInstance* pTestEngineInstance )
{
   if ( pTestEngineInstance != null )
   {
      CMemoryFree(pTestEngineInstance->pBundleData);
      CMemoryFill(pTestEngineInstance, 0, sizeof(tTestEngineInstance));
   }
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
