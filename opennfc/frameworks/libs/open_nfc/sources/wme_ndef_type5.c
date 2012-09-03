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
   Contains the NDEF Type 5 API implementation.
*******************************************************************************/
#define P_MODULE P_MODULE_DEC( NDEFA5 )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* Inside Type 5 Tag defines */
#define P_NDEF_5_MAPPING_VERSION             0x10       /* 1.0 */
#define P_NDEF_5_CC_BLOCK                    0x03
#define P_NDEF_5_NDEF_BLOCK                  0x04


static void static_PNDEFType5PicoRead( tContext* pContext, tNDEFConnection* pNDEFConnection,
                     tPBasicGenericCallbackFunction* pCallback, uint32_t nOffset, uint32_t nLength, bool bTrueCall);

static W_ERROR static_PNDEFType5Compress(tContext* pContext, tNDEFConnection *pNDEFConnection);
static void static_PNDEFType5CompressCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError);
static void static_PNDEFType5Decompress(tContext* pContext, void* pCallbackParameter, W_ERROR nError);

/* ------------------------------------------------------- */
/*                CONNECTION CREATION                      */
/* ------------------------------------------------------- */

static void static_PNDEFType5ReadCCCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );
static W_ERROR static_PNDEFType5ReadCapabilityContainer(tContext* pContext, tNDEFConnection* pNDEFConnection );

/**
 * @brief   Creates a NDEF TAG Type 5 connection
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pNDEFConnection  The PNDEF connection
 *
 * @return W_SUCCESS if the current connection is NDEF type 5 capable
 *
 * The NDEF connection is completed when the PNDEFSendError() is called
 **/
static W_ERROR static_PNDEFType5CreateConnection(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   uint32_t nMaxSize;
   W_ERROR nError;
   bool bIsFormattable;

#ifndef NO_COMPRESSION
   pNDEFConnection->bCompressionSupported = true;
#endif /*NO_COMPRESSION*/

   /* Check the type */
   if ( ( nError = PPicoCheckType5(
                        pContext,
                        pNDEFConnection->hConnection,
                        &nMaxSize,
                        &pNDEFConnection->bIsLocked,
                        &pNDEFConnection->bIsLockable,
                        &bIsFormattable,
                        pNDEFConnection->aSerialNumber,
                        &pNDEFConnection->nSerialNumberLength ) ) != W_SUCCESS )
   {
      PDebugError("static_PNDEFType5CreateConnection: not correct type 5");
      return nError;
   }

   /* Set the maximum space size */
   pNDEFConnection->nMaximumSpaceSize = nMaxSize;

   /* Read the Capability Container and the first 3 bytes of the NDEF file */
   static_PNDEFType5PicoRead( pContext, pNDEFConnection,
            static_PNDEFType5ReadCCCompleted,
            P_NDEF_5_CC_BLOCK * P_PICO_BLOCK_SIZE,
            P_PICO_BLOCK_SIZE + 0x03,
            true);

   return W_SUCCESS;
}

/**
 * @brief Capability Container Read callback
 *
 */
static void static_PNDEFType5ReadCCCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;

   PDebugTrace("static_PNDEFType5ReadCCCompleted : Error %s", PUtilTraceError(nError));

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFType5ReadCCCompleted : Error %s", PUtilTraceError(nError));

      /* Send the error */
      PNDEFSendError(pContext, pNDEFConnection, nError);
      return;
   }

   /* copy the contents of the received buffer in the CC file */
   CMemoryCopy(pNDEFConnection->aCCFile, pNDEFConnection->aReceivedBuffer, P_PICO_BLOCK_SIZE + 0x03);

   /* parse the contents of the capability container */
   nError = static_PNDEFType5ReadCapabilityContainer(pContext, pNDEFConnection);

   if (nError != W_SUCCESS)
   {
      PNDEFSendError(pContext, pNDEFConnection, nError);
   }
}

/**
 * @brief   Parses the content of the capability container
 *
 * @return W_SUCCESS if the capability container content is valid
 */
static W_ERROR static_PNDEFType5ReadCapabilityContainer(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   uint32_t nIndex = 0;
   uint32_t nLength;
   uint16_t nLen;

   static const uint8_t pType5String[] = { 0x4E, 0x44, 0x45, 0x46 }; /* "NDEF" */

   /* Check the NDEF identification string */
   if ( CMemoryCompare(
            pType5String,
            pNDEFConnection->aCCFile,
            4 ) != 0 )
   {
      PDebugLog("static_PNDEFType5ReadCapabilityContainer: wrong identification string");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   nIndex = 4;

   /* Mapping version */
   PDebugTrace(
      "static_PNDEFType5ReadCapabilityContainer: version %d.%d",
      (pNDEFConnection->aCCFile[nIndex] >> 4),
      (pNDEFConnection->aCCFile[nIndex] & 0x0F) );
   if ( ( P_NDEF_5_MAPPING_VERSION & 0xF0 ) < ( pNDEFConnection->aCCFile[nIndex] & 0xF0 ) )
   {
      PDebugError("static_PNDEFType5ReadCapabilityContainer: higher version");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Calculate the maximum message length */
   nLength  = (pNDEFConnection->aCCFile[nIndex + 1] << 8) + pNDEFConnection->aCCFile[nIndex + 2];

   if ( nLength + 2 > pNDEFConnection->nMaximumSpaceSize )
   {
      PDebugWarning(
         "static_PNDEFType5ReadCapabilityContainer: wrong length 0x%02X",
         nLength );

      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Store the maximum file size */
   pNDEFConnection->nMaximumSpaceSize = nLength;
   pNDEFConnection->nFreeSpaceSize = nLength;

   PDebugTrace("static_PNDEFType5ReadCapabilityContainer: nMaximumSpaceSize 0x%04X", pNDEFConnection->nMaximumSpaceSize );

   /* Set the default file id */
   pNDEFConnection->nNDEFId = P_NDEF_5_NDEF_BLOCK;

   /* Check the RFU value */
   if ( pNDEFConnection->aCCFile[nIndex + 3] != 0x00 )
   {
      PDebugError("static_PNDEFType5ReadCapabilityContainer: wrong RFU value");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* retrieve the actual NDEF length (two first bytes of the NDEF area) */

   nLen = (((uint16_t)(pNDEFConnection->aCCFile[nIndex + 4])) << 8) + (uint16_t)(pNDEFConnection->aCCFile[nIndex + 5]);

   if ((nLen == 0xFFFF) || (nLen > pNDEFConnection->nMaximumSpaceSize))
   {
      PDebugError( "static_PNDEFType5ReadCapabilityContainer, NDEF File length not valid");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   if (pNDEFConnection->bCompressionSupported == true)
   {
      /* Read compressed NDEF file */
      static_PNDEFType5PicoRead( pContext, pNDEFConnection,
         static_PNDEFType5Decompress,
         (P_NDEF_5_NDEF_BLOCK * P_PICO_BLOCK_SIZE),
         nLen + 2,
         true);
   }
   else /* old school, no decompression to do*/
   {
      pNDEFConnection->nNDEFFileLength = nLen;
      pNDEFConnection->nFreeSpaceSize = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;

      /* The connection creation is now completed */
      PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS);
   }

  return W_SUCCESS;
}

/**
 * @brief  Read NDEF file callback function, used when compression is supported
 *
 */
static void static_PNDEFType5Decompress(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   uint16_t nLen, dLen;
   uint32_t nCheckDecompSize;
   uint8_t *pComprBuffer;
   W_ERROR ErrCde = W_ERROR_TAG_FULL;
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   uint8_t* pBuffer = pNDEFConnection->aReceivedBuffer;

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFType5Decompress: error occured during the READ operation");
      ErrCde = nError;
      goto send_error;
   }

   /* Get the NDEF message length */
   dLen = nLen = (((uint16_t)pBuffer[0]) << 8) + (uint16_t)(pBuffer[1]);

   if(pBuffer[2] != 0x00)
   {
      /* NDEF File is not compressed, no need to decompress it */
      goto success;
   }

   /* Get the size in bytes of the uncompressed File*/
   dLen = (((uint16_t)pBuffer[3]) << 8) + (uint16_t)(pBuffer[4]);

   /* check for overlaps: P_NDEF_MAX_LENGTH is the size of received buffer */
   if( dLen > P_NDEF_MAX_LENGTH)
   {
    PDebugError("static_PNDEFType5Decompress: data are not valid");
    goto send_error;
   }

   /*backup the  compressed data*/
   pComprBuffer = (uint8_t*)CMemoryAlloc(nLen - 3);

   if( pComprBuffer == null)
   {
    PDebugError("static_PNDEFType5Decompress: could not allocate pComprBuffer");
    goto send_error;
   }
   CMemoryCopy(pComprBuffer, &pBuffer[5], (nLen - 3));

   /*uncompress NDEF Message */
   nCheckDecompSize = dLen + 1;
   ErrCde = WZLIB_uncompress(
             null,
             &pBuffer[2],
             &nCheckDecompSize,
             pComprBuffer,
             (nLen - 3));

   CMemoryFree(pComprBuffer);

    if(ErrCde != ZLIB_OK)
   {
      PDebugError("static_PNDEFType5Decompress: Error %d returned by ZLIB uncompress", ErrCde);
      goto send_error;
   }

   if((uint16_t)nCheckDecompSize != dLen)
   {
      PDebugError("static_PNDEFType5Decompress: Error of length returned by ZLIB uncompress");
      goto send_error;
   }
   /* update buffer containt*/

   pBuffer[0] = (uint8_t)(dLen >> 8);
   pBuffer[1] = (uint8_t)dLen;

success:

   pNDEFConnection->nNDEFFileLength = dLen;

   if (dLen > nLen)
   {
      /* the tag is compressed */
      pNDEFConnection->nFreeSpaceSize = (pNDEFConnection->nMaximumSpaceSize - nLen) * dLen / nLen;
   }
   else
   {
      pNDEFConnection->nFreeSpaceSize = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;
   }

   /* the connection creation is now complete */
   PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS);
   return;

send_error:
   /* Send the error */
   PNDEFSendError(
      pContext,
      pNDEFConnection,
      ErrCde );
   return;
}

/* ------------------------------------------------------- */
/*                COMMAND PROCESSING                       */
/* ------------------------------------------------------- */

static void static_PNDEFType5ReadCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );
static void static_PNDEFType5WriteCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );

/**
 * Calls the tag layer to perform a read operation.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pNDEFConnection  The connection.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  nOffset  The offset in bytes.
 *
 * @param[in]  nLength  The length in bytes.
 *
 * @param[in]  bTrueCall  true to perform a true call or false to simulate a call.
 **/
static void static_PNDEFType5PicoRead(
                     tContext* pContext,
                     tNDEFConnection* pNDEFConnection,
                     tPBasicGenericCallbackFunction* pCallback,
                     uint32_t nOffset,
                     uint32_t nLength,
                     bool bTrueCall)
{
   /* Store the parameter to be used in the callback */
   pNDEFConnection->nReceivedDataLength = nLength;


   if(bTrueCall != false)
   {
      /* Perform a true call */
      W_HANDLE hSubOperation = W_NULL_HANDLE;
      W_HANDLE* phSubOperation;
      if(pNDEFConnection->hCurrentOperation != W_NULL_HANDLE)
      {
         phSubOperation = &hSubOperation;
      }
      else
      {
         phSubOperation = null;
      }

      pNDEFConnection->nReceivedDataOffset = 0;

      PPicoRead(
         pContext,
         pNDEFConnection->hConnection,
         pCallback,
         pNDEFConnection,
         pNDEFConnection->aReceivedBuffer,
         nOffset,
         nLength,
         phSubOperation );

      if((pNDEFConnection->hCurrentOperation != W_NULL_HANDLE)
      && (hSubOperation != W_NULL_HANDLE))
      {
         if(PBasicAddSubOperationAndClose(pContext,
            pNDEFConnection->hCurrentOperation, hSubOperation) != W_SUCCESS)
         {
            PDebugError(
            "static_PNDEFType5PicoRead: error returned by PBasicAddSubOperationAndClose(), ignored");
         }
      }
   }
   else
   {
      /* Simulate the call with DFC */
      tDFCCallbackContext sCallbackContext;

      pNDEFConnection->nReceivedDataOffset = nOffset + 2;

      PDFCFillCallbackContext(
                         pContext,
                         (tDFCCallback*)pCallback,
                         pNDEFConnection,
                         &sCallbackContext );

      PDFCPostContext2(
         &sCallbackContext,
         P_DFC_TYPE_TYPE5,
         W_SUCCESS );
   }
}

/**
 * Calls the tag layer to perform a write operation.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pNDEFConnection  The connection.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  nOffset  The offset in bytes.
 *
 * @param[in]  nLength  The length in bytes.
 *
 * @param[in]  bLockTag  The lock tag flag.
 *
 * @param[in]  bTrueCall  true to perform a true call or false to simulate a call.
 **/
static void static_PNDEFType5PicoWrite(
                     tContext* pContext,
                     tNDEFConnection* pNDEFConnection,
                     tPBasicGenericCallbackFunction* pCallback,
                     uint32_t nOffset,
                     uint32_t nLength,
                     bool bLockTag,
                     bool bTrueCall)
{
   if(bTrueCall != false)
   {
      uint8_t * pBuffer;

      /* Perform a true call */
      W_HANDLE hSubOperation = W_NULL_HANDLE;
      W_HANDLE* phSubOperation;

      phSubOperation = (pNDEFConnection->hCurrentOperation != W_NULL_HANDLE) ? &hSubOperation : null;
      pBuffer = (nLength == 0) ? null : pNDEFConnection->aSendBuffer;

      PPicoWrite( pContext, pNDEFConnection->hConnection, pCallback, pNDEFConnection,
                     pBuffer, nOffset, nLength, bLockTag, phSubOperation );

      if ((pNDEFConnection->hCurrentOperation != W_NULL_HANDLE) && (hSubOperation != W_NULL_HANDLE))
      {
         if(PBasicAddSubOperationAndClose(pContext,
            pNDEFConnection->hCurrentOperation, hSubOperation) != W_SUCCESS)
         {
            PDebugError(
            "static_PNDEFType5PicoWrite: error returned by PBasicAddSubOperationAndClose(), ignored");
         }
      }
   }
   else
   {
      /* Simulate the call with DFC */
      tDFCCallbackContext sCallbackContext;
      PDFCFillCallbackContext(
                         pContext,
                         (tDFCCallback*)pCallback,
                         pNDEFConnection,
                         &sCallbackContext );

      PDFCPostContext2(
         &sCallbackContext,
         P_DFC_TYPE_TYPE5,
         W_SUCCESS );
   }
}


/**
  * @brief Processes the different NDEF commands
  *
  * @param[in] pContext The context
  *
  * @param[in] pNDEFConnection The NDEF connection
  *
  * @param[in] nOffset The offset, for read / write operations
  *
  * @param[in] nLength The length, for read / write operations
  *
  * @return W_SUCCESS on success
  */
static W_ERROR static_PNDEFType5SendCommand(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint32_t nOffset,
            uint32_t nLength )
{
   W_ERROR ErrCde = W_SUCCESS;

   /* Send the corresponding command */
   switch ( pNDEFConnection->nCommandType )
   {
      case P_NDEF_COMMAND_READ_NDEF:
      case P_NDEF_COMMAND_READ_NDEF_SPLIT:

         if ( pNDEFConnection->nCommandType == P_NDEF_COMMAND_READ_NDEF_SPLIT )
         {
            PDebugTrace("static_PNDEFType5SendCommand: P_NDEF_COMMAND_READ_NDEF_SPLIT");
         }
         else
         {
            PDebugTrace("static_PNDEFType5SendCommand: P_NDEF_COMMAND_READ_NDEF_TNF");
         }

         /* if the tag can be compressed, whole tag content has been read during connection creation and
            is directly read from the aReceived Buffer */

         if(pNDEFConnection->bCompressionSupported == true)
         {
            static_PNDEFType5PicoRead( pContext, pNDEFConnection, static_PNDEFType5ReadCompleted, nOffset, nLength, false);
            return W_SUCCESS;
         }

         /* otherwise, perform a real read operation to get the tag contents */
         static_PNDEFType5PicoRead( pContext, pNDEFConnection, static_PNDEFType5ReadCompleted, P_NDEF_5_NDEF_BLOCK * P_PICO_BLOCK_SIZE + nOffset, nLength, true);
         break;


      case P_NDEF_COMMAND_WRITE_NDEF:

         PDebugTrace("static_PNDEFType5SendCommand: P_NDEF_COMMAND_WRITE_NDEF");

         if(pNDEFConnection->bCompressionSupported == true)
         {
            /* pNDEFConnection->aReceivedBuffer contains the whole decompressed NDEF file,
               the write operation is done only in this buffer */

            CMemoryCopy(pNDEFConnection->aReceivedBuffer + nOffset + 2 , pNDEFConnection->pMessageBuffer, nLength);

            /* Simulate a call to the write function */
            static_PNDEFType5PicoWrite( pContext, pNDEFConnection, static_PNDEFType5WriteCompleted,
                     (pNDEFConnection->nNDEFId * P_PICO_BLOCK_SIZE) + nOffset,
                     nLength,
                     false,
                     false);
         }
         else
         {
            /* compression is not supported, directly write in the tag */
            static_PNDEFType5PicoWrite( pContext, pNDEFConnection, static_PNDEFType5WriteCompleted,
                     (pNDEFConnection->nNDEFId * P_PICO_BLOCK_SIZE) + nOffset + 2,
                     nLength,
                     false,
                     true);
         }
         break;


      case P_NDEF_COMMAND_WRITE_NDEF_LENGTH:

         PDebugTrace("static_PNDEFType5SendCommand: P_NDEF_COMMAND_WRITE_NDEF_LENGTH");

         pNDEFConnection->nNDEFFileLength = pNDEFConnection->nUpdatedNDEFFileLength;

         if(pNDEFConnection->bCompressionSupported == true)
         {
            pNDEFConnection->aReceivedBuffer[0] = (uint8_t) (pNDEFConnection->nNDEFFileLength >> 8);
            pNDEFConnection->aReceivedBuffer[1] = (uint8_t) pNDEFConnection->nNDEFFileLength;

            ErrCde = static_PNDEFType5Compress(pContext, pNDEFConnection);
         }
         else
         {
            /* compression is not supported, directly write in the tag */


            pNDEFConnection->aSendBuffer[0] = (uint8_t) (pNDEFConnection->nNDEFFileLength >> 8);
            pNDEFConnection->aSendBuffer[1] = (uint8_t) pNDEFConnection->nNDEFFileLength;

            static_PNDEFType5PicoWrite( pContext, pNDEFConnection,
                     static_PNDEFType5WriteCompleted,
                     pNDEFConnection->nNDEFId * P_PICO_BLOCK_SIZE,
                     2,
                     false,
                     true);
         }

         break;

      case P_NDEF_COMMAND_LOCK_TAG:

         PDebugTrace("static_PNDEFType5SendCommand: P_NDEF_COMMAND_LOCK_TAG");

         /* Send the command */
         static_PNDEFType5PicoWrite( pContext, pNDEFConnection,
                     static_PNDEFType5WriteCompleted,
                     0,
                     0,
                     true,
                     true);
         break;

      default:
         PDebugError("static_PNDEFType5SendCommand: command 0x%02X not supported", pNDEFConnection->nCommandType);
         return W_ERROR_BAD_PARAMETER;
   }

   return ErrCde;
}


/* @brief   Read command callback function
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PNDEFType5ReadCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;
   uint8_t* pBuffer = &pNDEFConnection->aReceivedBuffer[pNDEFConnection->nReceivedDataOffset];
   uint32_t nLength = pNDEFConnection->nReceivedDataLength;

   /* Call the generic callback function */
   nError2 = PNDEFSendCommandCompleted(
                     pContext,
                     pNDEFConnection,
                     pBuffer,
                     nLength,
                     nError );

   if (nError2 != W_SUCCESS )
   {
      /* Send the error */
      PNDEFSendError(
         pContext,
         pNDEFConnection,
         nError2 );
   }
}

/**
 * @brief   Compress a NDEF Message and update parameters.
 *
 * @param[in]  pNDEFConnection  The NDEF connection.
**/
static W_ERROR  static_PNDEFType5Compress(tContext* pContext,
                           tNDEFConnection *pNDEFConnection)
{
   uint32_t nComprSize;
   uint32_t ErrCde;
   uint16_t dLen;

   /* Get the NDEF File length */
   dLen = (((uint16_t)pNDEFConnection->aReceivedBuffer[0]) << 8)
         + (uint16_t)(pNDEFConnection->aReceivedBuffer[1]);

   /*force the compression to fail if compressed size greater than uncompressed size
   nComprSize = dLen;*/

   /*compresse the NDEF File only if Message overlaps tag size*/
   if( dLen > (uint16_t)pNDEFConnection->nMaximumSpaceSize)
   {
      ErrCde = WZLIB_compress2(
                  &pNDEFConnection->aSendBuffer[5],
                  &nComprSize,
                  &pNDEFConnection->aReceivedBuffer[2],
                   dLen,
                   ZLIB_BEST_COMPRESSION);
      if(ErrCde != ZLIB_OK)
      {
         PDebugError("static_PNDEFType5Compress: Error %d returned by ZLIB compress", ErrCde);
         return (W_ERROR)ErrCde;
      }
      if( nComprSize + 0x03 > (uint16_t)pNDEFConnection->nMaximumSpaceSize)
      {
         return W_ERROR_TAG_FULL;
      }
      /* update buffer with compressed File*/
      pNDEFConnection->aSendBuffer[2] = 0x00;

      pNDEFConnection->aSendBuffer[3] = (uint8_t)(dLen >> 8);
      pNDEFConnection->aSendBuffer[4] = (uint8_t)dLen;

      /* save the compressed File length*/
      pNDEFConnection->sType.t5.nLen = (uint16_t)(nComprSize + 0x03);
   }
   else
   {
      CMemoryCopy(
                 pNDEFConnection->aSendBuffer,
                 pNDEFConnection->aReceivedBuffer,
                 dLen + 2);
      pNDEFConnection->sType.t5.nLen = dLen;
   }
   /*start by erasing file Length field*/
   pNDEFConnection->aSendBuffer[0] = 0x00;
   pNDEFConnection->aSendBuffer[1] = 0x00;

   /* for safety write, began to write the entire file except the File length */
   static_PNDEFType5PicoWrite( pContext, pNDEFConnection,
               static_PNDEFType5CompressCompleted,
               (P_NDEF_5_NDEF_BLOCK * P_PICO_BLOCK_SIZE),
               pNDEFConnection->sType.t5.nLen + 2,
               false,
               true);

   return W_SUCCESS;

}

/**
 * @brief   Write compressed NDEF Message callback function
 *
**/
static void static_PNDEFType5CompressCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;

   /* set ndef file length*/
   pNDEFConnection->aSendBuffer[0] = (uint8_t)(pNDEFConnection->sType.t5.nLen >> 8);
   pNDEFConnection->aSendBuffer[1] = (uint8_t)pNDEFConnection->sType.t5.nLen;

   /* Send the command to write NDEF File length and notify upper layer with callback*/
   static_PNDEFType5PicoWrite( pContext, pNDEFConnection,
               static_PNDEFType5WriteCompleted,
               (P_NDEF_5_NDEF_BLOCK * P_PICO_BLOCK_SIZE),
               0x02,
               false,
               true);
}

/* @brief   Write command callback function
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PNDEFType5WriteCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;

   /* Call the generic callback function */
   nError2 = PNDEFSendCommandCompleted(
                     pContext,
                     pNDEFConnection,
                     null,
                     0,
                     nError );

   if (nError2 != W_SUCCESS )
   {
      /* Send the error */
      PNDEFSendError(
         pContext,
         pNDEFConnection,
         nError2 );
    }
}

/* ------------------------------------------------------- */
/*                CACHE MANAGEMENT                         */
/* ------------------------------------------------------- */

/**
  * @brief Invalidate cache associated to the connection
  *
  * @param[in] pContext The context
  *
  * @param[in] pNDEFConnection The NDEF connection
  */

static W_ERROR static_PNDEFType5InvalidateCache(
   tContext* pContext,
   tNDEFConnection* pNDEFConnection,
   uint32_t nOffset,
   uint32_t nLength)
{
   if (pNDEFConnection->bCompressionSupported == false)
   {
      return PPicoInvalidateCache(pContext, pNDEFConnection->hConnection, pNDEFConnection->nNDEFId * P_PICO_BLOCK_SIZE + 2 + nOffset, nLength);
   }
   else
   {
      return W_SUCCESS;
   }
}


/* The NDEF type information structure */
tNDEFTypeEntry g_sPNDEFType5Info = {
   W_PROP_NFC_TAG_TYPE_5,
   static_PNDEFType5CreateConnection,
   static_PNDEFType5SendCommand,
   static_PNDEFType5InvalidateCache
};

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

