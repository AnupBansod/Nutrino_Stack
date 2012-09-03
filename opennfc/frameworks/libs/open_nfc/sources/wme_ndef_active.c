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

#define P_MODULE P_MODULE_DEC( NDEFA )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* The NDEF Information types */
extern tNDEFTypeEntry g_sPNDEFType1Info;
extern tNDEFTypeEntry g_sPNDEFType2Info;
extern tNDEFTypeEntry g_sPNDEFType3Info;
extern tNDEFTypeEntry g_sPNDEFType4AInfo;
extern tNDEFTypeEntry g_sPNDEFType4BInfo;
extern tNDEFTypeEntry g_sPNDEFType5Info;
extern tNDEFTypeEntry g_sPNDEFType6Info;

/* NDEF Information array */
static tNDEFTypeEntry* const g_aNDEFTypeArray[] =
{
   &g_sPNDEFType1Info,
   &g_sPNDEFType2Info,
   &g_sPNDEFType3Info,
   &g_sPNDEFType4AInfo,
   &g_sPNDEFType4BInfo,
   &g_sPNDEFType5Info,
   &g_sPNDEFType6Info
};

/* Destroy connection callback */
static uint32_t static_PNDEFDestroyConnection(
            tContext* pContext,
            void* pObject );

/* Get properties connection callback */
static uint32_t static_PNDEFGetPropertyNumber(
            tContext* pContext,
            void* pObject);

/* Get properties connection callback */
static bool static_PNDEFGetProperties(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray );

/* Check properties connection callback */
static bool static_PNDEFCheckProperties(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue );

/* Handle NDEF connection type */
tHandleType g_sNDEFConnection = {   static_PNDEFDestroyConnection,
                                    null,
                                    static_PNDEFGetPropertyNumber,
                                    static_PNDEFGetProperties,
                                    static_PNDEFCheckProperties };

#define P_HANDLE_TYPE_NDEF_CONNECTION (&g_sNDEFConnection)

/**
 * @brief   Destroyes a NDEF connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PNDEFDestroyConnection(
            tContext* pContext,
            void* pObject )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pObject;

   PDebugTrace("static_PNDEFDestroyConnection");

   PDFCFlushCall(&pNDEFConnection->sCallbackContext);

   /* Free the NDEF connection structure */
   CMemoryFree( pNDEFConnection );

   return P_HANDLE_DESTROY_DONE;
}


/**
 * @brief   Gets the NDEF connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 **/
static uint32_t static_PNDEFGetPropertyNumber(
            tContext* pContext,
            void* pObject)
{
   return 1;
}

/**
 * @brief   Gets the NDEF connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  pPropertyArray  The property array.
 **/
static bool static_PNDEFGetProperties(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pObject;

   PDebugTrace("static_PNDEFGetProperties");

   if ( pNDEFConnection->pTagType == null )
   {
      PDebugError("static_PNDEFGetProperties: no property");
      return false;
   }

   pPropertyArray[0] = pNDEFConnection->pTagType->nProperty;
   return true;
}

/**
 * @brief   Checkes the NDEF connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  nPropertyValue  The property value.
 **/
static bool static_PNDEFCheckProperties(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pObject;

   PDebugTrace(
      "static_PNDEFCheckProperties: nPropertyValue=%s (0x%02X)",
      PUtilTraceConnectionProperty(nPropertyValue), nPropertyValue  );

   if ( pNDEFConnection->pTagType != null )
   {
      if ( nPropertyValue == pNDEFConnection->pTagType->nProperty )
      {
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      PDebugError("static_PNDEFCheckProperties: no property");
      return false;
   }
}

/**
 * @brief   Checks the NDEF file data.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pNDEFConnection  The NDEF connection structure.
 *
 * @param[in]  pReceivedBuffer  The NDEF file content from the specified offset.
 *
 * @param[in]  nReceivedLength  The NDEF file size from the specified offset.
 **/
static W_ERROR static_PNDEFCheckFile(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint8_t* pReceivedBuffer,
            uint32_t nReceivedLength )
{
   /* Check the length */
   if ( pNDEFConnection->nMessageLength == nReceivedLength )
   {
      if ( CMemoryCompare( pNDEFConnection->pMessageBuffer, pReceivedBuffer, nReceivedLength ) == 0 )
      {
         return W_SUCCESS;
      }
   }

   /* Send the error */
   return W_ERROR_TAG_WRITE;
}

/* See Header file */
void PNDEFCreateConnection(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nNDEFType )
{
   tNDEFConnection* pNDEFConnection;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;
   uint32_t nPos;

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Create the NDEF buffer */
   pNDEFConnection = (tNDEFConnection*)CMemoryAlloc( sizeof(tNDEFConnection) );
   if ( pNDEFConnection == null )
   {
      PDebugError("PNDEFCreateConnection: pNDEFConnection == null");
      /* Send the error */
      PDFCPostContext2(
         &sCallbackContext,
         P_DFC_TYPE_NDEF,
         W_ERROR_OUT_OF_RESOURCE );
      return;
   }
   CMemoryFill(pNDEFConnection, 0, sizeof(tNDEFConnection));

   /* Store the callback context */
   pNDEFConnection->sCallbackContext = sCallbackContext;

   /* Store the connection information */
   pNDEFConnection->hConnection = hConnection;
   pNDEFConnection->nCommandState = P_NDEF_ACTION_RESET;
   pNDEFConnection->pTagType = null;

   for(nPos = 0; nPos < (sizeof(g_aNDEFTypeArray)/sizeof(tNDEFTypeEntry*)); nPos++)
   {
      if(g_aNDEFTypeArray[nPos]->nProperty == nNDEFType)
      {
         pNDEFConnection->pTagType = g_aNDEFTypeArray[nPos];
         break;
      }
   }

   if(pNDEFConnection->pTagType == null)
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   CDebugAssert(pNDEFConnection->pTagType->pCreateConnection != null);
   if ( ( nError = pNDEFConnection->pTagType->pCreateConnection(
               pContext,
               pNDEFConnection ) ) != W_SUCCESS )
   {
      goto return_error;
   }

   /* Add the NDEF connection structure */
   if ( ( nError = PHandleAddHeir(
                     pContext,
                     hConnection,
                     pNDEFConnection,
                     P_HANDLE_TYPE_NDEF_CONNECTION ) ) != W_SUCCESS )
   {
      PDebugError("PNDEFCreateConnection: could not add the NDEF buffer");
      goto return_error;
   }

   return;

return_error:
   /* Send the error */
   PNDEFSendError(
      pContext,
      pNDEFConnection,
      nError );

   /* Free the buffer */
   CMemoryFree(pNDEFConnection);
   return;
}


/* See Header file */
void PNDEFRemoveConnection(
            tContext* pContext,
            W_HANDLE hConnection )
{
   tNDEFConnection* pNDEFConnection;

   pNDEFConnection = PHandleRemoveLastHeir(pContext, hConnection, P_HANDLE_TYPE_NDEF_CONNECTION);

   if (pNDEFConnection != null)
   {

      PDebugTrace("PNDEFRemoveConnection %p ", pNDEFConnection);

      PDFCFlushCall(&pNDEFConnection->sCallbackContext);

      /* Free the NDEF connection structure */
      CMemoryFree( pNDEFConnection );
   }

}
/* See Header file */
W_ERROR PNDEFCheckReadParameters(
            tContext* pContext,
            uint8_t nTNF,
            const tchar* pTypeString )
{
   uint32_t nLength = 0;

   PDebugTrace("PNDEFCheckReadParameters: nTNF 0x%02x", nTNF);

   /* Check the parameters */
   switch (nTNF)
   {
      case W_NDEF_TNF_EMPTY:
      case W_NDEF_TNF_UNKNOWN:
      case W_NDEF_TNF_UNCHANGED:
      case W_NDEF_TNF_ANY_TYPE:

         if ( pTypeString != null )
         {
            PDebugError("PNDEFCheckReadParameters: type string must be null");
            return W_ERROR_BAD_PARAMETER;
         }

         break;

      case W_NDEF_TNF_WELL_KNOWN:
      case W_NDEF_TNF_MEDIA:
      case W_NDEF_TNF_ABSOLUTE_URI:
      case W_NDEF_TNF_EXTERNAL:

         if (pTypeString == null)
         {
            PDebugError("PNDEFCheckReadParameters: type string must be not null");
            return W_ERROR_BAD_PARAMETER;
         }

         PDebugTrace("PNDEFCheckReadParameters: W_NDEF_TNF_WELL_KNOWN");

         nLength = PUtilStringLength(pTypeString);

         if ( (nLength == 0) || (nLength > P_NDEF_MAX_STRING_LENGTH))
         {
            PDebugError( "PNDEFCheckReadParameters: invalid type string length");
            return W_ERROR_BAD_PARAMETER;

         }
         break;

      default:
         PDebugError("PNDEFCheckReadParameters: Unknown TNF 0x%02x", nTNF );
         return W_ERROR_BAD_PARAMETER;
   }

   return W_SUCCESS;
}

/* See Header file */
W_ERROR PNDEFParseFile(
            tContext* pContext,
            uint8_t   nTNFToFind,
            const tchar*    pTypeToFind,
            const tchar*    pIdentifierToFind,
            const uint8_t*  pNDEFFile,
            uint32_t  nMessageLength,
            W_HANDLE* phMessage )
{
   bool bMB = false;
   bool bME = false;
   bool bCF = false;
   bool bFound = false;
   uint8_t  nTNF;
   uint32_t  nTypeLength;
   uint32_t  nIdentifierLength;
   uint32_t nPayloadLength;
   uint32_t nTypeIndex;
   uint32_t nIdentifierIndex;
   uint32_t nPayloadIndex;
   uint32_t  nTypeBufferIndex;
   uint32_t  nIdentifierBufferIndex;
   uint32_t  nPayloadBufferIndex;
   uint32_t nIndex;
   uint32_t nBufferLength;
   uint32_t nTmpLength;
   W_HANDLE hMessage = W_NULL_HANDLE;
   W_HANDLE hMessageOld = W_NULL_HANDLE;

   /* Reset the message handle */
   *phMessage = W_NULL_HANDLE;

   /* Parse the message */
   nIndex = 0;
   while ( nIndex < nMessageLength /*&& bFound == false*/)
   {
      /* Get the MB, ME, CF, SR, IL and TNF value */
      bMB = (((pNDEFFile[nIndex] >> 7) & 0x01) == 1) ? true : false;
      bME = (((pNDEFFile[nIndex] >> 6) & 0x01) == 1) ? true : false;
      bCF = (((pNDEFFile[nIndex] >> 5) & 0x01) == 1) ? true : false;
      nTNF = (pNDEFFile[nIndex] & 0x07);

      /* Check if it is the end of the message*/
      /* but not the last chunked record */
      if (bCF != false)
      {
         PDebugError("PNDEFParseFile: chunked record not supported");
         return W_ERROR_CONNECTION_COMPATIBILITY;
      }

      /* If it is the first record */
      if ( bMB != false )
      {
         uint32_t nSize;
         tchar * pTemp;

         /* If a message is finished */
         if (bME != false)
         {
            PDebugTrace("PNDEFParseFile: first & last record");
         }
         else
         {
            PDebugTrace("PNDEFParseFile: first record");
         }

         /* Get the index and length of each value */
         if ( PNDEFParseBuffer(
                  &pNDEFFile[nIndex],
                  nMessageLength - nIndex,
                  &nTypeBufferIndex,
                  &nTypeLength,
                  &nIdentifierBufferIndex,
                  &nIdentifierLength,
                  &nPayloadBufferIndex,
                  &nPayloadLength ) == 0 )
         {
            PDebugError("PNDEFParseFile: PNDEFParseBuffer error");
            return W_ERROR_CONNECTION_COMPATIBILITY;
         }

         /* Add the index to each value */
         nTypeIndex = nTypeBufferIndex + nIndex;
         nIdentifierIndex = nIdentifierBufferIndex + nIndex;
         nPayloadIndex = nPayloadBufferIndex + nIndex;

         if (nTypeLength != 0)
         {
            nSize = PNDEFUtilConvertByteArrayTo8BitCompatibleString(null, & pNDEFFile[nTypeIndex], nTypeLength);

            pTemp = CMemoryAlloc((nSize + 1) * sizeof(tchar));

            if (pTemp != null)
            {
               PNDEFUtilConvertByteArrayTo8BitCompatibleString(pTemp, & pNDEFFile[nTypeIndex], nTypeLength);
               pTemp[nSize] = 0x00;
            }
         }
         else
         {
            pTemp = null;
         }

         switch (nTNFToFind)
         {
            case W_NDEF_TNF_EMPTY:
            case W_NDEF_TNF_UNKNOWN :
            case W_NDEF_TNF_UNCHANGED :
            case W_NDEF_TNF_ANY_TYPE :

               bFound = WNDEFCompareRecordType(nTNFToFind, null, nTNF, pTemp);
               break;

            default:

               bFound = WNDEFCompareRecordType(nTNFToFind, pTypeToFind, nTNF, pTemp);
               break;
         }

         if (pTemp != null)
         {
            CMemoryFree(pTemp);
         }
      }

      nBufferLength = PNDEFGetRecordLength(&pNDEFFile[nIndex],nMessageLength - nIndex);

      if ( nBufferLength == 0 )
      {
         PDebugError("PNDEFParseFile: PNDEFGetRecordLength error");
         return W_ERROR_BAD_NDEF_FORMAT;
      }

      while (bME == false)
      {
         bME = (((pNDEFFile[nIndex + nBufferLength] >> 6) & 0x01) == 1) ? true : false;
         if ( (nTmpLength = PNDEFGetRecordLength(&pNDEFFile[nIndex + nBufferLength],nMessageLength - nIndex - nBufferLength)) == 0 )
         {
            PDebugError("PNDEFParseFile: PNDEFGetRecordLength error");
            return W_ERROR_BAD_NDEF_FORMAT;
         }
         nBufferLength += nTmpLength;
      }

      /* If a correct message has been found */
      if ( bFound != false )
      {
       /* If it is not the first correct message found */
         if ( hMessage != W_NULL_HANDLE )
         {
            hMessageOld = hMessage;
         }

         if ( PNDEFBuildMessage(pContext, &pNDEFFile[nIndex], nBufferLength, &hMessage ) != W_SUCCESS )
         {
            PDebugError("PNDEFParseFile:  PNDEFBuildMessage error");
            return W_ERROR_OUT_OF_RESOURCE;
         }

         /* If it is the first correct message found */
         if ( hMessageOld == W_NULL_HANDLE )
         {
            /* Store the message handle */
            *phMessage = hMessage;
         }
         else
         {
            /* Set the next message */
            if ( PNDEFSetNextMessage( pContext, hMessageOld, hMessage ) != W_SUCCESS )
            {
               PDebugError("PNDEFParseFile: PNDEFSetNextMessage error");
               return W_ERROR_OUT_OF_RESOURCE;
            }
         }
      }
      nIndex += nBufferLength;
   }

   /* If at least one message has been found */
   if ( *phMessage != W_NULL_HANDLE )
   {
      return W_SUCCESS;
   }

   return W_ERROR_ITEM_NOT_FOUND;
}

/* See Header file */
void PNDEFSendError(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            W_ERROR nError )
{
   /* Store the error code */
   pNDEFConnection->nError = nError;

   /* Check the command state */
   switch ( pNDEFConnection->nCommandState )
   {
      case P_NDEF_ACTION_READ:
      case P_NDEF_ACTION_READ_SPLIT:
         /* Send the error */
         PDFCPostContext3(
            &pNDEFConnection->sCallbackContext,
            P_DFC_TYPE_NDEF,
            W_NULL_HANDLE,
            nError );

         /* In case of error */
         if ( nError != W_SUCCESS )
         {
            PHandleClose(pContext, pNDEFConnection->hMessage);
            pNDEFConnection->hMessage = W_NULL_HANDLE;
         }
         break;

      case P_NDEF_ACTION_RESET:

         if (nError != W_SUCCESS)
         {
            PDFCPostContext2(
               &pNDEFConnection->sCallbackContext,
               P_DFC_TYPE_NDEF,
               W_ERROR_CONNECTION_COMPATIBILITY );
         }
         else
         {
            PDFCPostContext2(
            &pNDEFConnection->sCallbackContext,
            P_DFC_TYPE_NDEF,
            nError);
         }
         break;

      case P_NDEF_ACTION_WRITE:
         /* Send the error */
         PDFCPostContext2(
            &pNDEFConnection->sCallbackContext,
            P_DFC_TYPE_NDEF,
            nError );


         if ( pNDEFConnection->nCommandState == P_NDEF_ACTION_WRITE )
         {
            /* Close the message */
            PHandleClose(
               pContext,
               pNDEFConnection->hMessage );
         }
         break;
   }

   if ( pNDEFConnection->pMessageBuffer != null )
   {
      /* Free the message buffer */
      CMemoryFree( pNDEFConnection->pMessageBuffer );
      pNDEFConnection->pMessageBuffer = null;
   }

   /* Reset the message handle */
   pNDEFConnection->hMessage = W_NULL_HANDLE;
}

/** @brief READ NDEF operation automaton
  *
  * @param[in] pContext The context
  *
  * @param[in] pNDEFConnection The current NDEF connection
  *
  * @param[in] pBuffer the buffer that contains the operation result
  *
  * @param[in] nLength the length of the buffer
*/

W_ERROR static_PNDEFReadAutomaton(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint8_t* pBuffer,
            uint32_t nLength)
{
   W_ERROR  nError;
   W_HANDLE hMessage;

   bool bME = false;
   uint8_t nTNF;
   uint32_t nTotalLength;


   switch (pNDEFConnection->nCommandType)
   {
      case P_NDEF_COMMAND_READ_NDEF_SPLIT:

         PDebugTrace("static_PNDEFReadAutomaton: P_NDEF_COMMAND_READ_NDEF_SPLIT");

         /* Get the ME and TNF value */
         bME = (((pBuffer[0] >> 6) & 0x01) == 1) ? true : false;
         nTNF = (pBuffer[0] & 0x07);

         /* Get the index and length of each value */
         nTotalLength = PNDEFGetRecordLength(
                  pBuffer,
                  pNDEFConnection->nNDEFFileLength - pNDEFConnection->nOffset);

               /* we use the remaining NDEF file length instead of the received length,
                  this is due to the fact that PNDEFGetRecordLength checks the provided length is compatible
                  with the length found in the NDEF record */

         if ( nTotalLength == 0 )
         {
            PDebugError("static_PNDEFReadAutomaton: PNDEFParseBuffer error");
            return W_ERROR_CONNECTION_COMPATIBILITY;
         }

         /* Check the TNF value */
         if ((nTNF == pNDEFConnection->nTNF) || ((pNDEFConnection->bIsTNFFound != false) && (nTNF == W_NDEF_TNF_UNCHANGED)))
         {
            /* The current record has the expected TNF or
               is a continuation of a previous record with the good TNF */

            /* If this is not the last record of the message, remember we've found a good TNF */
            pNDEFConnection->bIsTNFFound = (bME == false) ? true : false;

            /* Read the remaining part of the NDEF record (if any) */
            if (nTotalLength > nLength)
            {
               /* Copy the beginning of the record */
               CMemoryCopy(& pNDEFConnection->aBuffer[pNDEFConnection->nIndex], pBuffer, nLength );
               pNDEFConnection->nIndex += nLength;

               /* read the end of the record */
               PNDEFSendCommand(
                  pContext,
                  pNDEFConnection,
                  P_NDEF_COMMAND_READ_NDEF,
                  pNDEFConnection->nOffset + nLength,
                  nTotalLength - nLength );

               return W_SUCCESS;
            }
            else
            {
               /* We already have received the whole NDEF record contents */
               CMemoryCopy(& pNDEFConnection->aBuffer[pNDEFConnection->nIndex], pBuffer, nTotalLength );
               pNDEFConnection->nIndex += nTotalLength;
            }
         }

         /* If we did not reached the end of the NDEF file, get next record TNF... */

         if (pNDEFConnection->nOffset + nTotalLength < pNDEFConnection->nNDEFFileLength )
         {
            if (pNDEFConnection->nOffset + nTotalLength + 7 <= pNDEFConnection->nNDEFFileLength )
            {
               /* Go to the next record */
               PNDEFSendCommand(
                  pContext,
                  pNDEFConnection,
                  P_NDEF_COMMAND_READ_NDEF_SPLIT,
                  pNDEFConnection->nOffset + nTotalLength,
                  7 );

               return W_SUCCESS;
            }

            if (pNDEFConnection->nOffset + nTotalLength + 3 <= pNDEFConnection->nNDEFFileLength )
            {
               /* Go to the next record */
               PNDEFSendCommand(
                  pContext,
                  pNDEFConnection,
                  P_NDEF_COMMAND_READ_NDEF_SPLIT,
                  pNDEFConnection->nOffset + nTotalLength,
                  3 );

               return W_SUCCESS;
            }

            /* We did not reached the end of the NDEF file, but there's no root to store any NDEF message ???? */
            return W_ERROR_CONNECTION_COMPATIBILITY;
         }

         /* ok, we've reached the end of the NDEF message, parse it */

        break;

      case P_NDEF_COMMAND_READ_NDEF :

         PDebugTrace("static_PNDEFReadAutomaton: P_NDEF_COMMAND_READ_NDEF");

         /* The contents of the read NDEF message is stored in pNDEFConnection->aReceivedBuffer,
            copy it in pNDEFConnection->aBuffer */

         CMemoryCopy(pNDEFConnection->aBuffer + pNDEFConnection->nIndex, pBuffer, nLength);
         pNDEFConnection->nIndex += nLength;

         /* If in split ndef read action */

         if ( pNDEFConnection->nCommandState == P_NDEF_ACTION_READ_SPLIT )
         {
            if (pNDEFConnection->nOffset + nLength < pNDEFConnection->nNDEFFileLength )
            {
               /* If we did not reached the end of the NDEF file, get next record TNF... */

               if (pNDEFConnection->nOffset + nLength + 7 <= pNDEFConnection->nNDEFFileLength )
               {
                  /* Go to the next record */
                  PNDEFSendCommand(
                     pContext,
                     pNDEFConnection,
                     P_NDEF_COMMAND_READ_NDEF_SPLIT,
                     pNDEFConnection->nOffset + nLength,
                     7 );

                  return W_SUCCESS;
               }

               if (pNDEFConnection->nOffset + nLength + 3 <= pNDEFConnection->nNDEFFileLength )
               {
                  /* Go to the next record */
                  PNDEFSendCommand(
                     pContext,
                     pNDEFConnection,
                     P_NDEF_COMMAND_READ_NDEF_SPLIT,
                     pNDEFConnection->nOffset + nLength,
                     3 );

                  return W_SUCCESS;
               }

               /* We did not reached the end of the NDEF file, but there's no root to store any NDEF message ???? */
               return (W_ERROR_CONNECTION_COMPATIBILITY);
            }
         }

         break;

      default :

         return W_ERROR_BAD_STATE;
   }

   /* we've reached the end of the NDEF message, parse it */

   if (pNDEFConnection->nIndex == 0)
   {
      return W_ERROR_ITEM_NOT_FOUND;
   }


   /* Parse the information */

   nError = PNDEFParseFile(
               pContext,
               pNDEFConnection->nTNF,
               pNDEFConnection->aTypeString,
               pNDEFConnection->aIdentifierString,
               pNDEFConnection->aBuffer,
               pNDEFConnection->nIndex,
               &hMessage );

   if (nError == W_SUCCESS)
   {
      /* Set the error code */
      pNDEFConnection->nError = W_SUCCESS;
      pNDEFConnection->hMessage = hMessage;

      /* Send the message */
      PDFCPostContext3(&pNDEFConnection->sCallbackContext, P_DFC_TYPE_NDEF, pNDEFConnection->hMessage, W_SUCCESS );
   }

   return nError;
}

/** @brief WRITE NDEF operation automaton
  *
  * @param[in] pContext The context
  *
  * @param[in] pNDEFConnection The current NDEF connection
  *
  * @param[in] pBuffer the buffer that contains the operation result
  *
  * @param[in] nLength the length of the buffer
*/

W_ERROR static_PNDEFWriteAutomaton(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint8_t* pBuffer,
            uint32_t nLength)
{
   W_ERROR nError;

   switch (pNDEFConnection->nCommandType)
   {
      case P_NDEF_COMMAND_WRITE_NDEF :

         PDebugTrace("static_PNDEFWriteAutomaton : P_NDEF_COMMAND_WRITE_NDEF");

         /* The write NDEF operation succeeded, write the NDEF length */
         PNDEFSendCommand( pContext, pNDEFConnection, P_NDEF_COMMAND_WRITE_NDEF_LENGTH, 0, 0);
         return W_SUCCESS;


      case P_NDEF_COMMAND_WRITE_NDEF_LENGTH :

         PDebugTrace("static_PNDEFWriteAutomaton : P_NDEF_COMMAND_WRITE_NDEF_LENGTH");

         /* The Write NDEF length operation succeeded */

         if ((pNDEFConnection->nActionMask & W_NDEF_ACTION_BIT_CHECK_WRITE) != 0)
         {
            if (pNDEFConnection->pTagType->pInvalidateCache)
            {
               /* here, need to flush the cache (if any) to ensure data are read from the chip */
               pNDEFConnection->pTagType->pInvalidateCache(pContext, pNDEFConnection, pNDEFConnection->nOffset, pNDEFConnection->nMessageLength);
            }

            /* if a check has been requested, read the TAG contents to check it */
            PNDEFSendCommand( pContext, pNDEFConnection, P_NDEF_COMMAND_READ_NDEF, pNDEFConnection->nOffset, pNDEFConnection->nMessageLength);
            return W_SUCCESS;
         }

         if ((pNDEFConnection->nActionMask & W_NDEF_ACTION_BIT_LOCK) != 0)
         {
            /* If lock has been requested, lock the TAG contents */
            PNDEFSendCommand(pContext, pNDEFConnection, P_NDEF_COMMAND_LOCK_TAG, 0, 0);
            return W_SUCCESS;
         }

         /* ok, the write operation is completed */
         pNDEFConnection->nError = W_SUCCESS;
         PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS );

         break;

      case P_NDEF_COMMAND_READ_NDEF :

         PDebugTrace("static_PNDEFWriteAutomaton : P_NDEF_COMMAND_READ_NDEF");

         /* the read NDEF operation succeeded */

         /* compare the contents of the tag with the expected contents */
         if ((nError = static_PNDEFCheckFile( pContext, pNDEFConnection, pBuffer, nLength)) != W_SUCCESS)
         {
            /* difference found between the tag contents and the expected NDEF message */
            return (nError);
         }

         /* ok, the check passed */
         if ((pNDEFConnection->nActionMask & W_NDEF_ACTION_BIT_LOCK) != 0)
         {
            /* If lock has been requested, lock the TAG contents */
            PNDEFSendCommand(pContext, pNDEFConnection, P_NDEF_COMMAND_LOCK_TAG, 0, 0);
            return W_SUCCESS;
         }

         /* ok, the write operation is completed */
         pNDEFConnection->nError = W_SUCCESS;
         PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS );

         break;

      case P_NDEF_COMMAND_LOCK_TAG :

         PDebugTrace("static_PNDEFWriteAutomaton : P_NDEF_COMMAND_LOCK_TAG");

         /* the lock operation succeeded */

         /* ok, the write operation is completed */
         pNDEFConnection->nError = W_SUCCESS;
         PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS );

         break;

      default :

         PDebugError("static_PNDEFWriteAutomaton : unexpected nCommandType %d", pNDEFConnection->nCommandType);
         return W_ERROR_BAD_STATE;
   }

   return W_SUCCESS;
}

/* See Header file */
W_ERROR PNDEFSendCommandCompleted(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint8_t* pBuffer,
            uint32_t nLength,
            uint32_t nError )
{

   /* Check if the operation has been cancelled by the user */
   if ( pNDEFConnection->nError == W_ERROR_CANCEL )
   {
      PDebugError("PNDEFSendCommandCompleted: W_ERROR_CANCEL");
      return W_ERROR_CANCEL;
   }

   /* Check the parameters */
   if ( pNDEFConnection->hConnection == W_NULL_HANDLE )
   {
      PDebugError("PNDEFSendCommandCompleted: W_ERROR_BAD_HANDLE");
      return W_ERROR_BAD_HANDLE;
   }

   /* Check the error code return */
   if ( nError != W_SUCCESS )
   {
      PDebugError("PNDEFSendCommandCompleted: Error %s", PUtilTraceError(nError) );

      if ( nError == W_ERROR_ITEM_LOCKED )
      {
         nError = W_ERROR_LOCKED_TAG;
      }
      return nError;
   }

   switch (pNDEFConnection->nCommandState)
   {
      case P_NDEF_ACTION_WRITE :
         nError = static_PNDEFWriteAutomaton(pContext, pNDEFConnection, pBuffer, nLength);
         break;

      case P_NDEF_ACTION_READ :
      case P_NDEF_ACTION_READ_SPLIT :
         nError = static_PNDEFReadAutomaton(pContext, pNDEFConnection, pBuffer, nLength);
         break;

      default :
         CDebugAssert(0);
         nError = W_ERROR_BAD_STATE;
         break;
   }

   return nError;
}

/* See Header file */
void PNDEFSendCommand(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint8_t nCommandType,
            uint32_t nOffset,
            uint32_t nLength )
{
   /* uint8_t nCommandLength = 0; */
   W_ERROR nError;
/*    static const uint8_t g_aNDEFEmpty[] = { 0x00, 0x03, 0xD0, 0x00, 0x00 }; */

   /* Store the command type */
   pNDEFConnection->nCommandType = nCommandType;

   CDebugAssert(pNDEFConnection->pTagType->pSendCommand != null);

   /* Send the corresponding command */
   switch ( nCommandType )
   {
      case P_NDEF_COMMAND_READ_NDEF:
      case P_NDEF_COMMAND_READ_NDEF_SPLIT:
      case P_NDEF_COMMAND_WRITE_NDEF :

         pNDEFConnection->nOffset = nOffset;

         CDebugAssert(pNDEFConnection->pTagType->pSendCommand != null);

         nError = pNDEFConnection->pTagType->pSendCommand( pContext, pNDEFConnection, nOffset, nLength );

         if(nError != W_SUCCESS)
         {
            PDebugError("PNDEFSendCommand: error %s", PUtilTraceError(nError) );
            /* Send the error */
            PNDEFSendError( pContext, pNDEFConnection, nError );
         }

         break;

      case P_NDEF_COMMAND_LOCK_TAG :
      case P_NDEF_COMMAND_WRITE_NDEF_LENGTH :

         CDebugAssert(pNDEFConnection->pTagType->pSendCommand != null);

         nError = pNDEFConnection->pTagType->pSendCommand(pContext, pNDEFConnection, 0, 0 );

         if(nError != W_SUCCESS)
         {
            PDebugError("PNDEFSendCommand: error %s", PUtilTraceError(nError) );
            /* Send the error */
            PNDEFSendError( pContext, pNDEFConnection, nError );
         }

         break;
   }
}

/* See Client API Specifications */
void PNDEFReadMessage(
            tContext* pContext,
            W_HANDLE hConnection,
            tPNDEFReadMessageCompleted* pCallback,
            void* pCallbackParameter,
            uint8_t nTNF,
            const tchar* pTypeString,
            W_HANDLE* phOperation )
{
   tNDEFConnection* pNDEFConnection;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   if(phOperation != null)
   {
      *phOperation = PBasicCreateOperation(pContext, null, null );
   }

   /* Check if the connection handle is valid */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_NDEF_CONNECTION, (void**)&pNDEFConnection);
   if ( nError != W_SUCCESS )
   {
      W_HANDLE hMessage = W_NULL_HANDLE;

      /* Check if it is a Tag Simulation */
      nError = PVirtualTagReadMessage(
               pContext,
               hConnection,
               &hMessage,
               nTNF,
               pTypeString);

      PDFCPostContext3(
         &sCallbackContext,
         P_DFC_TYPE_NDEF,
         hMessage,
         nError );

      return;
   }

   /* Check if an operation is still pending */
   if ( pNDEFConnection->nError == P_ERROR_STILL_PENDING )
   {
      /* Send the error */
      PDFCPostContext3(
         &sCallbackContext,
         P_DFC_TYPE_NDEF,
         W_NULL_HANDLE,
         W_ERROR_BAD_STATE );
      return;
   }

   /* Store the connection information */
   pNDEFConnection->sCallbackContext   = sCallbackContext;
   if ( nTNF == W_NDEF_TNF_ANY_TYPE )
   {
      pNDEFConnection->nCommandState   = P_NDEF_ACTION_READ;
   }
   else
   {
      pNDEFConnection->nCommandState   = P_NDEF_ACTION_READ_SPLIT;
   }
   /* Reset the buffer addresses */
   pNDEFConnection->pMessageBuffer     = null;

   /* Check the read parameters */
   if ( ( nError = PNDEFCheckReadParameters(
                     pContext,
                     nTNF,
                     pTypeString ) ) != W_SUCCESS )
   {
      PDebugError("PNDEFReadMessage: PNDEFCheckReadParameters error");
      goto return_error;
   }

   /* Check the NDEF file size */
   if ( pNDEFConnection->nNDEFFileLength < 3 )
   {
      PDebugError("PNDEFReadMessage: W_ERROR_ITEM_NOT_FOUND");
      nError = W_ERROR_ITEM_NOT_FOUND;
      goto return_error;
   }

   /* Store the command information */
   pNDEFConnection->nTNF            = nTNF;
   /* Reset the values */
   pNDEFConnection->hMessage        = W_NULL_HANDLE;
   pNDEFConnection->nError          = P_ERROR_STILL_PENDING;
   pNDEFConnection->nIndex          = 0;
   pNDEFConnection->nOffset         = 0;
   pNDEFConnection->nMessageLength  = 0;
   /* Get the values */
   switch (nTNF)
   {
      case W_NDEF_TNF_EMPTY:
      case W_NDEF_TNF_UNKNOWN:
      case W_NDEF_TNF_UNCHANGED:
      case W_NDEF_TNF_ANY_TYPE:
         pNDEFConnection->aTypeString[0] = '\0';
         break;

      case W_NDEF_TNF_WELL_KNOWN:
      case W_NDEF_TNF_MEDIA:
      case W_NDEF_TNF_ABSOLUTE_URI:
      case W_NDEF_TNF_EXTERNAL:
         CMemoryCopy(
            pNDEFConnection->aTypeString,
            pTypeString,
            PUtilStringLength(pTypeString) * 2);

         pNDEFConnection->aTypeString[PUtilStringLength(pTypeString)] = '\0';
         pNDEFConnection->aIdentifierString[0] = '\0';
         break;
   }

   /* Read the NDEF file */
      PNDEFSendCommand(
         pContext,
         pNDEFConnection,
         P_NDEF_COMMAND_READ_NDEF,
         0x00,
         pNDEFConnection->nNDEFFileLength);

#if 0

   /* @todo :
       the optimisation of reading only some records of a message masks several problems such as formatting problems
       it is disabled for now */

   if ( pNDEFConnection->nTNF == W_NDEF_TNF_ANY_TYPE )
   {
      /* Read the NDEF file */
      PNDEFSendCommand(
         pContext,
         pNDEFConnection,
         P_NDEF_COMMAND_READ_NDEF,
         0x00,
         pNDEFConnection->nNDEFFileLength);
   }
   else
   {
      /* Only read the TNF */
      PNDEFSendCommand(
         pContext,
         pNDEFConnection,
         P_NDEF_COMMAND_READ_NDEF_SPLIT,
         0x00,
         7 /* flags (MB, ME, CF, SR, IL, TNF), type length, payload length and id length */ );
   }
#endif

   return;

return_error:
   if (phOperation != null)
   {
      PBasicSetOperationCompleted(
            pContext,
            *phOperation);
   }

   /* Send the error */
   PNDEFSendError(
      pContext,
      pNDEFConnection,
      nError );
   return;
}

/* See Client API Specifications */
W_ERROR PNDEFGetTagInfo(
            tContext* pContext,
            W_HANDLE hConnection,
            tNDEFTagInfo* pTagInfo )
{
   tNDEFConnection* pNDEFConnection;
   W_ERROR nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_NDEF_CONNECTION, (void**)&pNDEFConnection);

   /* Check the parameters */
   if ( pTagInfo == null )
   {
      PDebugError("PNDEFGetTagInfo: pTagInfo == null");
      return W_ERROR_BAD_PARAMETER;
   }

   /* Fill in the buffer with zeros */
   CMemoryFill(pTagInfo, 0, sizeof(tNDEFTagInfo));

   if ( nError == W_SUCCESS )
   {
      /* Store the information on the provided connection */
      pTagInfo->nTagType            = pNDEFConnection->pTagType->nProperty;
      pTagInfo->bIsLocked           = pNDEFConnection->bIsLocked;
      pTagInfo->bIsLockable         = pNDEFConnection->bIsLockable;
      pTagInfo->nFreeSpaceSize      = pNDEFConnection->nFreeSpaceSize;
      pTagInfo->nSerialNumberLength = pNDEFConnection->nSerialNumberLength;
      if ( pNDEFConnection->nSerialNumberLength != 0 )
      {
         CMemoryCopy(pTagInfo->aSerialNumber, pNDEFConnection->aSerialNumber, pTagInfo->nSerialNumberLength);
      }
   }

   return nError;
}

/* See Client API Specifications */
W_ERROR WNDEFReadMessageSync(
            W_HANDLE hConnection,
            uint8_t nTNF,
            const tchar* pTypeString,
            W_HANDLE* phMessage)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WNDEFReadMessage(
         hConnection,
         PBasicGenericSyncCompletionHandle, &param,
         nTNF,
         pTypeString,
         null );
   }

   return PBasicGenericSyncWaitForResultHandle(&param, phMessage);
}

const uint8_t g_SupportedNonFormattedTags[] =
{
   /* TAG type 1  */
   W_PROP_TYPE1_CHIP,      /* Innovision topaz */

   /* TAG type 2 */
   W_PROP_MIFARE_UL_C,     /* NXP MIFARE Ultra Light C */
   W_PROP_MIFARE_UL,       /* NXP MIFARE Ultra Light */
   W_PROP_MY_D_MOVE,       /* Infineon My-D Move */
   W_PROP_MY_D_NFC,        /* Infineon My-D NFC */

#if 0 /* @todo */
      /* read / write is not yet supported */

   W_PROP_MIFARE_1K,       /* NXP mifare 1K */
   W_PROP_MIFARE_4K,       /* NXP mifare 4K */

#endif

   /* TAG type 5 */
   W_PROP_PICOPASS_2K,     /* Inside Secure PicoPass */
   W_PROP_PICOPASS_32K,

   /* TAG type 6 */
   W_PROP_TI_TAGIT,
   W_PROP_NXP_ICODE,
   W_PROP_ST_LRI_512,
};

typedef struct sNDEFOperation
{
   W_HANDLE                      hConnection;
   W_HANDLE                      hMessage;
   uint32_t                      nActionMask;
   W_HANDLE                      hOperation;
   uint8_t                       nNDEFType;
   tDFCCallbackContext           sCallbackContext;

} tNDEFOperation;


/* NDEF Write message completed callback */
static void static_PNDEFFormatWriteMessageCompleted(
            tContext * pContext,
            void * pCallbackParameter,
            W_ERROR nError)
{
   tNDEFOperation * pNDEFOperation = (tNDEFOperation *) pCallbackParameter;

   PDFCPostContext2(& pNDEFOperation->sCallbackContext, P_DFC_TYPE_NDEF, nError);

   /* Free the no longer needed NDEF operation */
   CMemoryFree(pNDEFOperation);
}

/* NDEF creation completed */

static void static_PNDEFFormatConnectionCreated(
            tContext * pContext,
            void     * pCallbackParameter,
            W_ERROR    nError)
{
   tNDEFOperation * pNDEFOperation = (tNDEFOperation *) pCallbackParameter;

   PDebugTrace("static_PNDEFFormatConnectionCreated");

   if (nError == W_SUCCESS)
   {
      /* the connection has been upgraded to NDEF, we can now perform the NDEF operation pending */

      PNDEFWriteMessage(pContext, pNDEFOperation->hConnection, static_PNDEFFormatWriteMessageCompleted, pNDEFOperation, pNDEFOperation->hMessage, pNDEFOperation->nActionMask, null);
   }
   else
   {
      PDebugError("static_PNDEFFormatConnectionCreated : nError %d", nError);

      PDFCPostContext2(& pNDEFOperation->sCallbackContext, P_DFC_TYPE_NDEF, nError);

      /* Free the no longer needed NDEF operation */
      CMemoryFree(pNDEFOperation);
   }
}

/* TAG format completed */
static void static_PNDEFFormatCompleted(
            tContext * pContext,
            void     * pCallbackParameter,
            W_ERROR    nError)
{
   tNDEFOperation * pNDEFOperation = (tNDEFOperation *) pCallbackParameter;

   PDebugTrace("static_PNDEFFormatCompleted");

   if (nError == W_SUCCESS)
   {
      /* If the NDEF connection was already existing, we need to destroy it and to recreate it to allow proper update of the connection infos */

      if (PBasicCheckConnectionProperty(pContext, pNDEFOperation->hConnection, pNDEFOperation->nNDEFType) == W_SUCCESS)
      {
         PNDEFRemoveConnection(pContext, pNDEFOperation->hConnection);
      }

      /* create the NDEF connection */
      PNDEFCreateConnection(pContext, pNDEFOperation->hConnection, static_PNDEFFormatConnectionCreated, pNDEFOperation, pNDEFOperation->nNDEFType);

   }
   else
   {
      PDebugError("static_PNDEFFormatCompleted : nError %d", nError);

      PDFCPostContext2(& pNDEFOperation->sCallbackContext, P_DFC_TYPE_NDEF, nError);

      CMemoryFree(pNDEFOperation);
   }
}


/* See Client API Specifications */
void PNDEFWriteMessage(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            W_HANDLE hMessage,
            uint32_t nActionMask,
            W_HANDLE* phOperation )
{
   tDFCCallbackContext sCallbackContext;
   tNDEFConnection* pNDEFConnection;
   W_ERROR nError;
   uint8_t nTagType = 0;

   PDebugTrace("PNDEFWriteMessage");

   /* prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* force * phOperation to null in case of error */
   if (phOperation != null)
   {
      * phOperation = W_NULL_HANDLE;
   }

   /* check the nActionMask value */
   if ((nActionMask & W_NDEF_ACTION_BITMASK) != nActionMask)
   {
      PDebugError("PNDEFWriteMessage : invalid nActionMask value");
      nError = W_ERROR_BAD_PARAMETER;
      goto error;
   }

   /* specific case for Virtual tags */

   if (PBasicCheckConnectionProperty(pContext, hConnection, W_PROP_VIRTUAL_TAG) == W_SUCCESS)
   {
      nError = PVirtualTagWriteMessage(pContext, hConnection, hMessage, nActionMask);

      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NDEF, nError);
      return;
   }

   /* Generic case */

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_NDEF_CONNECTION, (void**)&pNDEFConnection);

   if ( ((nError == W_SUCCESS) && ((nActionMask & W_NDEF_ACTION_FORMAT_BITMASK) == W_NDEF_ACTION_BIT_FORMAT_ALL)) ||
        ((nError != W_SUCCESS) && ((nActionMask & W_NDEF_ACTION_FORMAT_BITMASK) != 0)))
   {
      /* A format operation has been requested...
       *
       * Check if we are able to format this kind of tag
       */

      uint32_t i;

      for (i=0; i<sizeof(g_SupportedNonFormattedTags); i++)
      {
         nError = PBasicCheckConnectionProperty(pContext, hConnection, g_SupportedNonFormattedTags[i]);

         if (nError == W_ERROR_BAD_HANDLE)
         {
            PDebugError("PNDEFWriteMessage : PBasicCheckConnectionProperty returned W_ERROR_BAD_HANDLE");
            goto error;
         }

         if (nError == W_SUCCESS)
         {
            nTagType = g_SupportedNonFormattedTags[i];
            break;
         }
      }

      if (nError != W_SUCCESS)
      {
         /* We don't support formatting of this tag...
          * If it is already formatted, simply ignore the format request and replace it by a erase request
          */

         if (pNDEFConnection != null)
         {
            nActionMask &= ~W_NDEF_ACTION_FORMAT_BITMASK;
            nActionMask |= W_NDEF_ACTION_BIT_ERASE;
         }
         else
         {
            nError = W_ERROR_CONNECTION_COMPATIBILITY;
            goto error;
         }
      }
   }

   /*
    *  Depending of the type of the connection and the request :
    *  - directly write into the tag
    *  - or format it before...
    */

   if ((pNDEFConnection != null) && ((nActionMask & W_NDEF_ACTION_FORMAT_BITMASK) != W_NDEF_ACTION_BIT_FORMAT_ALL))
   {
      /* We already have a NDEF connection, and unconditionnal format has not been resquested,
       * perform the write operation...
       */

      uint32_t nOffset;
      uint32_t nMessageSize = 0;
      uint32_t nRequestedSize = 0;
      uint32_t nTrailerSize = 0;
      uint32_t nActualLength;

      /* Check if an NDEF operation is still pending */

      if ( pNDEFConnection->nError == P_ERROR_STILL_PENDING )
      {
         /* Send the error */
         PDebugError("PNDEFWriteMessage : operation pending");
         nError = W_ERROR_BAD_STATE;
         goto error;
      }

      /* Check the connection locked flag */
      if ( pNDEFConnection->bIsLocked != false )
      {
         PDebugError("PNDEFWriteMessage: W_ERROR_LOCKED_TAG");
         nError = W_ERROR_LOCKED_TAG;
         goto error;
      }

      /* Check the command lock flag */
      if ((nActionMask & W_NDEF_ACTION_BIT_LOCK) != false )
      {
         /* Check the connection lockable flag */
         if ( pNDEFConnection->bIsLockable == false )
         {
            PDebugError("PNDEFWriteMessage: W_ERROR_TAG_NOT_LOCKABLE");
            nError = W_ERROR_TAG_NOT_LOCKABLE;
            goto error;
         }
      }

      if (((nActionMask & W_NDEF_ACTION_BIT_ERASE) != 0) || (hMessage != W_NULL_HANDLE))
      {
         /* A write action is needed */

         /* if ERASE action has been requested,
            the requested size does not include the current NDEF message length,
            otherwise, the requested size include the current NDEF message length */

         nOffset = ((nActionMask & W_NDEF_ACTION_BIT_ERASE) == 0) ? pNDEFConnection->nNDEFFileLength : 0;

         if (hMessage != W_NULL_HANDLE)
         {
            /* get the message size */
            nMessageSize = PNDEFGetMessageLength( pContext, hMessage);

            if (nMessageSize == 0)
            {
               PDebugError("PNDEFWriteMessage: unknown hMessage");
               nError = W_ERROR_BAD_HANDLE;
               goto error;
            }
         }
         else
         {
            nMessageSize = 3;     /* empty NDEF message: D0 00 00 */
         }

         nRequestedSize = nOffset + nMessageSize;

         /* store the updated message length for later processing */
         pNDEFConnection->nUpdatedNDEFFileLength = nRequestedSize;

         if ((nRequestedSize > pNDEFConnection->nMaximumSpaceSize) && (pNDEFConnection->bCompressionSupported == false))
         {
            PDebugError("PNDEFWriteMessage : no room to store the message");
            nError = W_ERROR_TAG_FULL;
            goto error;
         }

         /* Duplicate the message handle */

         if ( ( nError = PHandleDuplicate(pContext, hMessage, &pNDEFConnection->hMessage ) ) != W_SUCCESS )
         {
            PDebugError("PNDEFWriteMessage : PHandleDuplicate failed");
            goto error;
         }

         /* Compute the trailer size */

         switch (pNDEFConnection->pTagType->nProperty)
         {
            case W_PROP_NFC_TAG_TYPE_1 :
            case W_PROP_NFC_TAG_TYPE_2 :

               /* For theses tags, if the NDEF area is not full, we must append a terminator
                  TLV at the end of the message */

               if (nRequestedSize < pNDEFConnection->nMaximumSpaceSize)
               {
                  nTrailerSize = 1;
               }
               else
               {
                  nTrailerSize = 0;
               }
               break;

            case W_PROP_NFC_TAG_TYPE_3 :

               nTrailerSize = 15;
               break;

            case W_PROP_NFC_TAG_TYPE_4_A :
            case W_PROP_NFC_TAG_TYPE_4_B :
            case W_PROP_NFC_TAG_TYPE_5 :
            case W_PROP_NFC_TAG_TYPE_6 :

               nTrailerSize = 0;
               break;

            default:
               CDebugAssert(0);
               break;
         }

         pNDEFConnection->pMessageBuffer = (uint8_t *) CMemoryAlloc(nMessageSize + nTrailerSize);

         if (pNDEFConnection->pMessageBuffer == null)
         {
            PDebugError("PNDEFWriteMessage : CMemoryAlloc() failed");
            nError = W_ERROR_OUT_OF_RESOURCE;
            goto error;
         }

         if (pNDEFConnection->hMessage != W_NULL_HANDLE)
         {
            nError = PNDEFGetMessageContent(pContext, pNDEFConnection->hMessage, pNDEFConnection->pMessageBuffer, nMessageSize, &nActualLength );

            if (nError != W_SUCCESS )
            {
               PDebugError("PNDEFWriteMessage: could not retrieve message contents");
               goto error;
            }
         }
         else
         {
            /* empty NDEF message */
            pNDEFConnection->pMessageBuffer[0] = 0xD0;
            pNDEFConnection->pMessageBuffer[1] = 0x00;
            pNDEFConnection->pMessageBuffer[2] = 0x00;
            nActualLength = 3;
         }

         if (nTrailerSize != 0)
         {
            switch (pNDEFConnection->pTagType->nProperty)
            {
               case W_PROP_NFC_TAG_TYPE_1 :

                  pNDEFConnection->pMessageBuffer[nActualLength] = P_NDEF_1_TLV_TERMINATOR;
                  break;

               case W_PROP_NFC_TAG_TYPE_2 :
                  pNDEFConnection->pMessageBuffer[nActualLength] = P_NDEF_2_TLV_TERMINATOR;
                  break;

               case W_PROP_NFC_TAG_TYPE_3 :
                  CMemoryFill(&pNDEFConnection->pMessageBuffer[nActualLength], 0xFF, nTrailerSize);
                  nTrailerSize = 0;
                  break;
            }
         }

         if (phOperation != null)
         {
            pNDEFConnection->hCurrentOperation = PBasicCreateOperation(pContext, null, null );

            if (pNDEFConnection->hCurrentOperation == W_NULL_HANDLE)
            {
               PDebugError("PNDEFWriteMessage: could not create operation handle");
               goto error;
            }

            if (PHandleDuplicate(pContext, pNDEFConnection->hCurrentOperation, phOperation) != W_SUCCESS)
            {
               * phOperation = pNDEFConnection->hCurrentOperation;
            }
         }
         else
         {
            pNDEFConnection->hCurrentOperation = W_NULL_HANDLE;
         }

         /* Store the connection information */
         pNDEFConnection->sCallbackContext   = sCallbackContext;
         pNDEFConnection->nCommandState      = P_NDEF_ACTION_WRITE;
         pNDEFConnection->nMessageLength     = nActualLength;

         pNDEFConnection->nActionMask = nActionMask & ~0x03;

         /* Perform the write operation */
         PNDEFSendCommand(pContext, pNDEFConnection, P_NDEF_COMMAND_WRITE_NDEF, nOffset, nMessageSize + nTrailerSize);

      }
      else
      {
         /* no write operation */

         if ((nActionMask & W_NDEF_ACTION_BIT_LOCK) == 0)
         {
            PDebugError("PNDEFWriteMessage : No write / no lock operation");
            nError = W_SUCCESS;
            goto error;
         }

         /* Store the connection information */
         pNDEFConnection->sCallbackContext   = sCallbackContext;
         pNDEFConnection->nCommandState      = P_NDEF_ACTION_WRITE;

         /* Reset the buffer addresses */
         pNDEFConnection->hMessage           = W_NULL_HANDLE;
         pNDEFConnection->pMessageBuffer     = null;
         pNDEFConnection->nMessageLength     = 0;

         /* Perform the lock operation */
         PNDEFSendCommand(pContext, pNDEFConnection, P_NDEF_COMMAND_LOCK_TAG, 0, 0 );
      }
   }
   else
   {
      uint32_t nTagSize = 0;
      uint8_t  nSectorSize;
      bool     bIsLocked = false;
      bool     bIsLockable = false;
      bool     bIsFormattable = false;
      tNDEFOperation * pNDEFOperation;

      /*
       * We have to format the TAG....
       */

      switch (nTagType)
      {
         /* Type 1 TAG */
         case W_PROP_TYPE1_CHIP :

            nError = PType1ChipUserCheckType1(pContext, hConnection, &nTagSize, &nSectorSize, &bIsLocked, &bIsLockable, &bIsFormattable, null, null);
            break;

         /* Type 2 TAG */
         case W_PROP_MIFARE_UL :
         case W_PROP_MIFARE_UL_C :

            nError = PMifareCheckType2(pContext, hConnection, &nTagSize, &nSectorSize, &bIsLocked, &bIsLockable, &bIsFormattable, null, null);
            break;

         case W_PROP_MY_D_MOVE :
         case W_PROP_MY_D_NFC :

            nError = PMyDCheckType2(pContext, hConnection, &nTagSize, &nSectorSize, &bIsLocked, &bIsLockable, &bIsFormattable, null, null);
            break;

         /* Type 5 TAG */
         case W_PROP_PICOPASS_2K :
         case W_PROP_PICOPASS_32K :

            nError = PPicoCheckType5(pContext, hConnection, &nTagSize, &bIsLocked, &bIsLockable, &bIsFormattable, null, null);
            break;

         /* Type 6 TAG */
         case W_PROP_NXP_ICODE :
         case W_PROP_ST_LRI_512 :
         case W_PROP_TI_TAGIT :

            nError = P15P3UserCheckType6(pContext, hConnection, &nTagSize, &bIsLocked, &bIsLockable, &bIsFormattable, null, null);
            break;

         default :
            /* should not occur */
            nError = W_ERROR_CONNECTION_COMPATIBILITY;
            break;
      }

      if (nError != W_SUCCESS)
      {
         PDebugError("PNDEFWriteMessage : check type failed");
         goto error;
      }


      if (bIsFormattable == false)
      {
         nError = (bIsLocked == true) ? W_ERROR_LOCKED_TAG : W_ERROR_CONNECTION_COMPATIBILITY;

         PDebugError("PNDEFWriteMessage : Unable to format this tag : %d", nError);
         goto error;
      }

      /* check we can also perform the write operation if any */

      if (hMessage != W_NULL_HANDLE)
      {
         uint32_t nLength = PNDEFGetMessageLength( pContext, hMessage);

         if (nLength == 0)
         {
            PDebugError("PNDEFWriteMessage: unknown hMessage");
            nError = W_ERROR_BAD_HANDLE;
            goto error;
         }

         if (nLength > nTagSize)       /* @todo : compresssion ? */
         {
            PDebugError("PNDEFWriteMessage : NDEF message too long");
            nError = W_ERROR_TAG_FULL;
            goto error;
         }
      }

      /* check if we can perform the lock operation if requested */

      if ((nActionMask & W_NDEF_ACTION_BIT_LOCK) && (bIsLockable == false))
      {
         PDebugError("PNDEFWriteMessage : unable to lock");
         nError = W_ERROR_TAG_NOT_LOCKABLE;
         goto error;
      }

      /* allocate a NDEF operation to store the current operation parameters */

      pNDEFOperation = (tNDEFOperation *) CMemoryAlloc(sizeof(tNDEFOperation));

      if (pNDEFOperation == W_NULL_HANDLE)
      {
         PDebugError("PNDEFWriteMessage : can not allocate operation");
         nError = W_ERROR_OUT_OF_RESOURCE;
         goto error;
      }

      pNDEFOperation->hConnection = hConnection;
      pNDEFOperation->hMessage = hMessage;
      pNDEFOperation->nActionMask = nActionMask & ~W_NDEF_ACTION_BIT_FORMAT_ALL;
      pNDEFOperation->sCallbackContext = sCallbackContext;

      if (phOperation != null)
      {
         * phOperation = pNDEFOperation->hOperation = PBasicCreateOperation(pContext, null, null );
      }

      /* start format operation */

      switch (nTagType)
      {
         /* Type 1 TAG */
         case W_PROP_TYPE1_CHIP :

            pNDEFOperation->nNDEFType = W_PROP_NFC_TAG_TYPE_1;
            PNDEFFormatNDEFType1(pContext, hConnection, static_PNDEFFormatCompleted, pNDEFOperation, nTagType);
            break;

         /* Type 2 TAG */
         case W_PROP_MIFARE_UL :
         case W_PROP_MIFARE_UL_C :
         case W_PROP_MY_D_MOVE :
         case W_PROP_MY_D_NFC :

            pNDEFOperation->nNDEFType = W_PROP_NFC_TAG_TYPE_2;
            PNDEFFormatNDEFType2(pContext, hConnection, static_PNDEFFormatCompleted, pNDEFOperation, nTagType);
            break;

         /* Type 5 TAG */
         case W_PROP_PICOPASS_2K :
         case W_PROP_PICOPASS_32K :

            pNDEFOperation->nNDEFType = W_PROP_NFC_TAG_TYPE_5;
            PNDEFFormatNDEFType5(pContext, hConnection, static_PNDEFFormatCompleted, pNDEFOperation, nTagType);
            break;

         /* Type 6 TAG */
         case W_PROP_NXP_ICODE :
         case W_PROP_ST_LRI_512 :
         case W_PROP_TI_TAGIT :

            pNDEFOperation->nNDEFType = W_PROP_NFC_TAG_TYPE_6;
            PNDEFFormatNDEFType6(pContext, hConnection, static_PNDEFFormatCompleted, pNDEFOperation, nTagType);
            break;

         default :
            /* should not occur */

            nError = W_ERROR_CONNECTION_COMPATIBILITY;
            break;
      }
   }

   return;

error:

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NDEF, nError);
}


/* See Client API Specifications */
W_ERROR WNDEFWriteMessageSync(
            W_HANDLE hConnection,
            W_HANDLE hMessage,
            uint32_t nActionMask)
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WNDEFWriteMessage(
         hConnection,
         PBasicGenericSyncCompletion, &param,
         hMessage,
         nActionMask,
         null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}



#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */


