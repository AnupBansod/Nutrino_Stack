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
   Contains the NDEF Type 4 API implementation.
*******************************************************************************/
#define P_MODULE P_MODULE_DEC( NDEFA4 )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* NFC Type 4 Tag specific command types */
#define P_NDEF_COMMAND_4_SELECT_APPLICATION  0x40
#define P_NDEF_COMMAND_4_SELECT_CC           0x41
#define P_NDEF_COMMAND_4_SELECT_EF           0x42

/* NFC Type 4 Tag defines */
#define P_NDEF_4_MAPPING_VERSION             0x20       /* 2.0 */

/* ------------------------------------------------------- */
/*                CONNECTION CREATION                      */
/* ------------------------------------------------------- */

#define P_NDEF_4_SELECT_APPLICATION    0x00
#define P_NDEF_4_SELECT_CC             0x01
#define P_NDEF_4_READ_CC_LENGTH        0x02
#define P_NDEF_4_READ_CC               0x03
#define P_NDEF_4_PARSE_CC              0x04
#define P_NDEF_4_SELECT_NDEF           0x05
#define P_NDEF_4_READ_NDEF_LENGTH      0x06


static W_ERROR static_PNDEFType4ReadCapabilityContainer(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection );

static void static_PNDEFType4ReadCCAutomaton(
               tContext * pContext,
               tNDEFConnection* pNDEFConnection);

static void static_PNDEFType4ReadCCAutomatonCallback(
               tContext* pContext,
               void *pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError);

static void static_PNDEFType4SelectApplication(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter);

static void static_PNDEFType4SelectFile(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter,
               uint16_t nFileId);

static void static_PNDEFType4ReadFile(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter,
               uint32_t nOffset,
               uint32_t nLength);

static void static_PNDEFType4ReadFileSlice(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               uint32_t nOffset,
               uint32_t nLength);

static void static_PNDEFType4ReadFileSliceCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError );

static void static_PNDEFType4ReadNDEFCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError );

static void static_PNDEFType4WriteFile(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter,
               uint32_t nOffset,
               uint32_t nLength);

static void static_PNDEFType4WriteFileSlice(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               uint32_t nOffset,
               uint32_t nLength);

static void static_PNDEFType4WriteFileSliceCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError );

static void static_PNDEFType4WriteNDEFCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError );


/**
  * @brief Creates a NDEF TYPE 4 connection
  *
  * @param[in] pContext The context
  *
  * @param[in] pNDEFConnection The NDEF connection
  *
  * @return W_SUCCESS if the NDEF connection creation is successfull
  */
static W_ERROR static_PNDEFType4CreateConnection(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   W_ERROR nError;

   /* Check the properties */
   if ( ( nError = PHandleCheckProperty(
                     pContext,
                     pNDEFConnection->hConnection,
                     W_PROP_ISO_7816_4 ) ) != W_SUCCESS )
   {
      PDebugError("PNDEFCreateConnection: not 7816-4");
      return nError;
   }

   /* Set default value of RAPDU, used until the CC file has been received / parsed */
   pNDEFConnection->sType.t4.nMaxRAPDUSize = 0x0F;

   /* Read the CC file */

   pNDEFConnection->sType.t4.nReadCCAutomatonState = P_NDEF_4_SELECT_APPLICATION;
   pNDEFConnection->sType.t4.nVariant = P_NDEF_4_VERSION_20;
   static_PNDEFType4ReadCCAutomaton(pContext, pNDEFConnection);

   return (W_SUCCESS);
}

/** Read CC automaton
  *
  * - Select application
  * - Select CC
  * - Read CC length
  * - Read CC
  * - Parse CC
  *
  * @param[in] pContext The context
  *
  * @param[in] pNDEFConnection The NDEF connection
 */

static void static_PNDEFType4ReadCCAutomaton(
   tContext * pContext,
   tNDEFConnection* pNDEFConnection
   )
{
   W_ERROR nError;


   switch (pNDEFConnection->sType.t4.nReadCCAutomatonState)
   {
      case P_NDEF_4_SELECT_APPLICATION :

         /* select the NDEF application */
         static_PNDEFType4SelectApplication(pContext, pNDEFConnection, static_PNDEFType4ReadCCAutomatonCallback, pNDEFConnection);
         break;

      case P_NDEF_4_SELECT_CC :

         /* select the CC file */
         static_PNDEFType4SelectFile(pContext, pNDEFConnection,  static_PNDEFType4ReadCCAutomatonCallback, pNDEFConnection, 0xE103);
         break;

      case P_NDEF_4_READ_CC_LENGTH :

         /* the length of the CC file is stored in the first two bytes of the CC file */

         static_PNDEFType4ReadFile(pContext, pNDEFConnection, static_PNDEFType4ReadCCAutomatonCallback, pNDEFConnection, 0, 2);
         break;

      case P_NDEF_4_READ_CC :

         /* the length of the CC file has been retrieve, read the whole CC file */
         static_PNDEFType4ReadFile(pContext, pNDEFConnection, static_PNDEFType4ReadCCAutomatonCallback, pNDEFConnection, 0, pNDEFConnection->nCCLength);
         break;

      case P_NDEF_4_PARSE_CC :

         /* The whole CC file has been retrieve, parser it */
         nError = static_PNDEFType4ReadCapabilityContainer(pContext, pNDEFConnection);

         if (nError != W_SUCCESS)
         {
            /* send the result of the parse operation */
            PNDEFSendError(pContext, pNDEFConnection, nError);
         }
         /* walk through .... */
         pNDEFConnection->sType.t4.nReadCCAutomatonState = P_NDEF_4_SELECT_NDEF;

      case P_NDEF_4_SELECT_NDEF :

         /* Select the NDEF file */
         static_PNDEFType4SelectFile(pContext, pNDEFConnection, static_PNDEFType4ReadCCAutomatonCallback, pNDEFConnection, pNDEFConnection->nNDEFId);
         break;

      case P_NDEF_4_READ_NDEF_LENGTH :

         /* Read the NDEF file length */
         static_PNDEFType4ReadFile(pContext, pNDEFConnection, static_PNDEFType4ReadCCAutomatonCallback, pNDEFConnection, 0, 2);
         break;
   }
}

/** Read CC automaton callback
  *
  */
static void static_PNDEFType4ReadCCAutomatonCallback(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nReceivedAPDULength,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   uint32_t nCardErrorCode = 0;
   if((nError == W_SUCCESS) && (nReceivedAPDULength >= 2))
   {
      nCardErrorCode = (pNDEFConnection->aReceivedBuffer[nReceivedAPDULength - 2] << 8) | pNDEFConnection->aReceivedBuffer[nReceivedAPDULength - 1];
   }

   if (pNDEFConnection->sType.t4.nReadCCAutomatonState == P_NDEF_4_SELECT_APPLICATION)
   {
      /* Special type 4 non-standard behavior */

      if ( (nError == W_SUCCESS) && (nCardErrorCode != 0x9000))
      {
         if  (pNDEFConnection->sType.t4.nVariant == P_NDEF_4_VERSION_20)
         {
            pNDEFConnection->sType.t4.nVariant = P_NDEF_4_VERSION_10;

            /* Re-select the NDEF application */
            static_PNDEFType4SelectApplication(
               pContext,
               pNDEFConnection,
               static_PNDEFType4ReadCCAutomatonCallback,
               pNDEFConnection);

            return;
         }
         else if (pNDEFConnection->sType.t4.nVariant == P_NDEF_4_VERSION_10)
         {
            pNDEFConnection->sType.t4.nVariant = P_NDEF_4_VERSION_10_NON_STANDARD;

               /* Re-select the NDEF application */
            static_PNDEFType4SelectApplication(
               pContext,
               pNDEFConnection,
               static_PNDEFType4ReadCCAutomatonCallback,
               pNDEFConnection);

            return;
         }
      }
   }

   if ((nError != W_SUCCESS) || (nCardErrorCode != 0x9000))
   {

      PDebugLog("static_PNDEFType4ReadCCAutomatonCallback : nError %d, nCardErrorCode %08x", nError, nCardErrorCode);

      PNDEFSendError(pContext, pNDEFConnection, W_ERROR_CONNECTION_COMPATIBILITY);
   }
   else
   {
      switch (pNDEFConnection->sType.t4.nReadCCAutomatonState)
      {
         case P_NDEF_4_SELECT_APPLICATION :

            pNDEFConnection->sType.t4.nReadCCAutomatonState = P_NDEF_4_SELECT_CC;
            break;

         case P_NDEF_4_SELECT_CC :

            pNDEFConnection->sType.t4.nReadCCAutomatonState = P_NDEF_4_READ_CC_LENGTH;
            break;

         case P_NDEF_4_READ_CC_LENGTH :

            /* compute the CC length */
            pNDEFConnection->nCCLength = (pNDEFConnection->aReceivedBuffer[0] << 8) + pNDEFConnection->aReceivedBuffer[1];

            if ((pNDEFConnection->nCCLength < 0x0F) || (pNDEFConnection->nCCLength > 0x0FFE))
            {
               PNDEFSendError(pContext, pNDEFConnection, W_ERROR_CONNECTION_COMPATIBILITY );
               return;
            }

            pNDEFConnection->sType.t4.nReadCCAutomatonState = P_NDEF_4_READ_CC;
            break;

         case P_NDEF_4_READ_CC :

            CMemoryCopy(pNDEFConnection->aCCFile, pNDEFConnection->aReceivedBuffer, pNDEFConnection->nCCLength);
            pNDEFConnection->sType.t4.nReadCCAutomatonState = P_NDEF_4_PARSE_CC;
            break;

         case P_NDEF_4_SELECT_NDEF :
            pNDEFConnection->sType.t4.nReadCCAutomatonState = P_NDEF_4_READ_NDEF_LENGTH;
            break;

         case P_NDEF_4_READ_NDEF_LENGTH :
            pNDEFConnection->nNDEFFileLength = (pNDEFConnection->aReceivedBuffer[0] << 8) + pNDEFConnection->aReceivedBuffer[1];

            if (pNDEFConnection->nNDEFFileLength > pNDEFConnection->nMaximumSpaceSize)
            {
               PDebugError("static_PNDEFType4ReadCCAutomatonCallback : invalid NDEF length");
               PNDEFSendError(pContext, pNDEFConnection, W_ERROR_CONNECTION_COMPATIBILITY );
            }
            else
            {
               pNDEFConnection->nFreeSpaceSize = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;
               PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS);
            }
            /* no more processing */
            return;
      }

      static_PNDEFType4ReadCCAutomaton(pContext, pNDEFConnection);
   }
}

/**
 * Sends Select Application APDU
 *
 * @param[in] pContext The context
 *
 * @param[in] pNDEFConnection The NDEF connection
 */
static void static_PNDEFType4SelectApplication(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter)
{
   /* NFC Type 4 Tag v2.0 specific commands (with optional Le byte) */
   static const uint8_t aSelectApplicationv20[]    = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 0x00 };

   /* NFC Type 4 Tag v1.0 specific commands (with non-standard Le byte) */
   static const uint8_t aSelectApplicationv10[]    = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x00, 0x00 };

   const uint8_t * pSelectApplication;
   uint8_t nCommandLength = 0;

   PDebugTrace("static_PNDEFType4SelectApplication");

   switch (pNDEFConnection->sType.t4.nVariant)
   {
      case P_NDEF_4_VERSION_20:
         pSelectApplication = aSelectApplicationv20;
         nCommandLength = sizeof(aSelectApplicationv20);
         break;

      case P_NDEF_4_VERSION_10:
         pSelectApplication = aSelectApplicationv10;
         nCommandLength = sizeof (aSelectApplicationv10) - 1;
         break;

      case P_NDEF_4_VERSION_10_NON_STANDARD:
         pSelectApplication = aSelectApplicationv10;
         nCommandLength = sizeof (aSelectApplicationv10);
         break;

      default:
         PDebugError("static_PNDEFType4SelectApplication : invalid tag type");
         return;
   }

   P7816ExchangeAPDU( pContext,
            pNDEFConnection->hConnection,
            pCallback,
            pCallbackParameter,
            pSelectApplication,
            nCommandLength,
            pNDEFConnection->aReceivedBuffer,
            P_NDEF_COMMAND_MAX_LENGTH,
            null );
}

/**
 * Selects the CC file
 *
 * @param[in] pContext The context
 *
 * @param[in] pNDEFConnection The NDEF connection
 */
static void static_PNDEFType4SelectFile(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter,
               uint16_t nFileID)
{
   static const uint8_t pSelect[]                  = { 0x00, 0xA4, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0x00 };
   uint8_t nCommandLength = 0;

   /* Prepare the command */
   nCommandLength = sizeof(pSelect) - 1;

   if (pNDEFConnection->sType.t4.nVariant == P_NDEF_4_VERSION_10_NON_STANDARD)
   {
      nCommandLength++;
   }

   CMemoryCopy( pNDEFConnection->aSendBuffer, pSelect, nCommandLength );

   if (pNDEFConnection->sType.t4.nVariant == P_NDEF_4_VERSION_20)
   {
      pNDEFConnection->aSendBuffer[3] = 0x0C;
   }

   pNDEFConnection->aSendBuffer[5] = (uint8_t)(nFileID >> 8);
   pNDEFConnection->aSendBuffer[6] = (uint8_t) nFileID & 0xFF;

   /* Send the command */
   P7816ExchangeAPDU(
      pContext,
      pNDEFConnection->hConnection,
      pCallback,
      pCallbackParameter,
      pNDEFConnection->aSendBuffer,
      nCommandLength,
      pNDEFConnection->aReceivedBuffer,
      P_NDEF_COMMAND_MAX_LENGTH,
      null );
}

/**
 * @brief Reads data from currently selected file, deals with the RAPDU limit
 *
 * @param[in] pContext The context
 *
 * @param[in] pNDEFConnection The NDEF connection
 *
 * @param[in] pCallback The callback
 *
 * @param[in] pCallback The callback parameter
 *
 * @param[in] nOffset The offset
 *
 * @param[in] nLength The length
 */

static void static_PNDEFType4ReadFile(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter,
               uint32_t nOffset,
               uint32_t nLength)
{
   pNDEFConnection->sType.t4.nBytesToProcess = nLength;
   pNDEFConnection->sType.t4.nBytesProcessed = 0;

   pNDEFConnection->sType.t4.pCallback = pCallback;
   pNDEFConnection->sType.t4.pCallbackParameter = pCallbackParameter;

   pNDEFConnection->sType.t4.nOffset = nOffset;
   static_PNDEFType4ReadFileSlice(pContext, pNDEFConnection, nOffset, nLength);
}

/**
  * Reads a slice from the currently selected file
  *
  * @param[in] pContext The context
  *
  * @param[in] pNDEFConnection The NDEFConnection,
  *
  * @param[in] nOffset The offset
  *
  * @param[in] nLength The length
  */

static void static_PNDEFType4ReadFileSlice(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               uint32_t nOffset,
               uint32_t nLength)
{
   static const uint8_t pRead[] = { 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00 };
   uint8_t nCommandLength;

   /* Prepare the command */
   CMemoryCopy( pNDEFConnection->aSendBuffer, pRead, 3 );

   /* Copy the offset */
   pNDEFConnection->aSendBuffer[2] = (uint8_t)(nOffset >> 8);
   pNDEFConnection->aSendBuffer[3] = (uint8_t)nOffset;

   /* Read no more than the maximum RAPDU size */
   if ( nLength > pNDEFConnection->sType.t4.nMaxRAPDUSize )
   {
      nLength = pNDEFConnection->sType.t4.nMaxRAPDUSize;
   }

   /* Copy the length */
   if ( nLength <= 0xFF )
   {
      pNDEFConnection->aSendBuffer[4] = (uint8_t)(nLength);
      nCommandLength = 5;
   }
   else
   {
      pNDEFConnection->aSendBuffer[4] = P_NDEF_4_EXTENDED_LENGTH;
      pNDEFConnection->aSendBuffer[5] = (uint8_t)(nLength >> 8);
      pNDEFConnection->aSendBuffer[6] = (uint8_t)(nLength);
      nCommandLength = 7;
   }

   /* Send the command */
   P7816ExchangeAPDU(
      pContext,
      pNDEFConnection->hConnection,
      static_PNDEFType4ReadFileSliceCompleted,
      pNDEFConnection,
      pNDEFConnection->aSendBuffer,
      nCommandLength,
      pNDEFConnection->aReceivedBuffer + pNDEFConnection->sType.t4.nBytesProcessed,
      P_NDEF_COMMAND_MAX_LENGTH,
      null );
}

/**
  * Read file callback.
  *
  */
static void static_PNDEFType4ReadFileSliceCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   uint32_t nCardErrorCode = 0;
   if((nError == W_SUCCESS) && (nReceivedAPDULength >= 2))
   {
      nCardErrorCode = (pNDEFConnection->aReceivedBuffer[pNDEFConnection->sType.t4.nBytesProcessed + nReceivedAPDULength - 2] << 8) |
      pNDEFConnection->aReceivedBuffer[pNDEFConnection->sType.t4.nBytesProcessed + nReceivedAPDULength - 1];
   }

   if ((nError != W_SUCCESS) || (nCardErrorCode != 0x9000))
   {
      PDebugError("static_PNDEFType4ReadFileCompleted %d, nCardErrorCode %08x", nError, nCardErrorCode);

      pNDEFConnection->sType.t4.pCallback(pContext, pNDEFConnection->sType.t4.pCallbackParameter, nReceivedAPDULength, nError);
      return;
   }

   if ((nReceivedAPDULength - 2) > pNDEFConnection->sType.t4.nBytesToProcess)
   {
      PDebugError("static_PNDEFType4ReadFileCompleted : received more data than requested");
      pNDEFConnection->sType.t4.pCallback(pContext, pCallbackParameter, nReceivedAPDULength, W_ERROR_BAD_STATE);
      return;
   }

   pNDEFConnection->sType.t4.nBytesProcessed += (nReceivedAPDULength - 2);
   pNDEFConnection->sType.t4.nBytesToProcess -= (nReceivedAPDULength - 2);

   if (pNDEFConnection->sType.t4.nBytesToProcess > 0)
   {
      static_PNDEFType4ReadFileSlice(pContext, pNDEFConnection, pNDEFConnection->sType.t4.nOffset + pNDEFConnection->sType.t4.nBytesProcessed, pNDEFConnection->sType.t4.nBytesToProcess);
   }
   else
   {
      /* The read operation is now complete, call the callback function */
      pNDEFConnection->sType.t4.pCallback(pContext, pNDEFConnection->sType.t4.pCallbackParameter, nReceivedAPDULength, nError);
   }
}

/**
 * Writes data into the currently selected file
 *
 * @param[in] pContext The context
 *
 * @param[in] pNDEFConnection The NDEF connection
 *
 * @param[in] pCallback The callback
 *
 * @param[in] pCallback The callback parameter
 *
 * @param[in] nOffset The offset
 *
 * @param[in] nLength The length
 */
static void static_PNDEFType4WriteFile(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               tPBasicGenericDataCallbackFunction * pCallback,
               void * pCallbackParameter,
               uint32_t nOffset,
               uint32_t nLength)
{
   pNDEFConnection->sType.t4.nBytesToProcess = nLength;
   pNDEFConnection->sType.t4.nBytesProcessed = 0;

   pNDEFConnection->sType.t4.pCallback = pCallback;
   pNDEFConnection->sType.t4.pCallbackParameter = pCallbackParameter;

   pNDEFConnection->sType.t4.nOffset = nOffset;

   static_PNDEFType4WriteFileSlice(pContext, pNDEFConnection, nOffset, nLength);
}

/**
 * Writes a slice data into the currently selected file
 *
 * @param[in] pContext The context
 *
 * @param[in] pNDEFConnection The NDEF connection
 *
 * @param[in] nOffset The offset
 *
 * @param[in] nLength The length
 */

static void static_PNDEFType4WriteFileSlice(
               tContext* pContext,
               tNDEFConnection* pNDEFConnection,
               uint32_t nOffset,
               uint32_t nLength)
{
   static const uint8_t pWrite[] = { 0x00, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00 };
   uint32_t nCommandLength;

   /* Prepare the command */
   CMemoryCopy(
            pNDEFConnection->aSendBuffer,
            pWrite,
            3 );

   /* Copy the offset */
   pNDEFConnection->aSendBuffer[2] = (uint8_t)(nOffset >> 8);
   pNDEFConnection->aSendBuffer[3] = (uint8_t)nOffset;

   if ( nLength > pNDEFConnection->sType.t4.nMaxCAPDUSize )
   {
      nLength = pNDEFConnection->sType.t4.nMaxCAPDUSize;
   }

   /* Copy the length */
   if ( nLength <= 0xFF )
   {
      pNDEFConnection->aSendBuffer[4] = (uint8_t)(nLength);
      nCommandLength = 5;
   }
   else
   {
      pNDEFConnection->aSendBuffer[4] = P_NDEF_4_EXTENDED_LENGTH;
      pNDEFConnection->aSendBuffer[5] = (uint8_t)(nLength >> 8);
      pNDEFConnection->aSendBuffer[6] = (uint8_t)(nLength);
      nCommandLength = 7;
   }

   /* Copy the data to be written in the send buffer */
   CMemoryCopy(pNDEFConnection->aSendBuffer + nCommandLength, pNDEFConnection->sType.t4.pBuffer + pNDEFConnection->sType.t4.nBytesProcessed, nLength);

   pNDEFConnection->sType.t4.nBytesPending = nLength;

   /* Send the command */
   P7816ExchangeAPDU(
      pContext,
      pNDEFConnection->hConnection,
      static_PNDEFType4WriteFileSliceCompleted,
      pNDEFConnection,
      pNDEFConnection->aSendBuffer,
      nCommandLength + nLength,
      pNDEFConnection->aReceivedBuffer,
      P_NDEF_COMMAND_MAX_LENGTH,
      null );
}

/**
  * Write file callback.
  *
  */

static void static_PNDEFType4WriteFileSliceCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   uint32_t nCardErrorCode = 0;
   if((nError == W_SUCCESS) && (nReceivedAPDULength >= 2))
   {
      nCardErrorCode = (pNDEFConnection->aReceivedBuffer[nReceivedAPDULength - 2] << 8) | pNDEFConnection->aReceivedBuffer[nReceivedAPDULength - 1];
   }

   if ((nError != W_SUCCESS) || (nCardErrorCode != 0x9000))
   {
      PDebugError("static_PNDEFType4WriteFileSliceCompleted %d, nCardErrorCode %08x", nError, nCardErrorCode);

      pNDEFConnection->sType.t4.pCallback(pContext, pNDEFConnection->sType.t4.pCallbackParameter, nReceivedAPDULength, nError);
      return;
   }

   pNDEFConnection->sType.t4.nBytesProcessed += pNDEFConnection->sType.t4.nBytesPending;
   pNDEFConnection->sType.t4.nBytesToProcess -= pNDEFConnection->sType.t4.nBytesPending;

   if (pNDEFConnection->sType.t4.nBytesToProcess > 0)
   {
      static_PNDEFType4WriteFileSlice(pContext, pNDEFConnection, pNDEFConnection->sType.t4.nOffset  + pNDEFConnection->sType.t4.nBytesProcessed, pNDEFConnection->sType.t4.nBytesToProcess);
   }
   else
   {
      /* The read operation is now complete, call the callback function */
      pNDEFConnection->sType.t4.pCallback(pContext, pNDEFConnection->sType.t4.pCallbackParameter, nReceivedAPDULength, nError);
   }
}

/* See Header file */
static W_ERROR static_PNDEFType4ReadCapabilityContainer(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   uint8_t nNumber;
   uint8_t nType;
   uint16_t nNDEFId;
   uint32_t nIndex = 2;
   uint32_t nLength;
   uint32_t nTLVLength;

   /* Mapping version */
   PDebugTrace(
      "static_PNDEFType4ReadCapabilityContainer: version %d.%d",
      (pNDEFConnection->aCCFile[nIndex] >> 4),
      (pNDEFConnection->aCCFile[nIndex] & 0x0F) );
   if ( ( P_NDEF_4_MAPPING_VERSION & 0xF0 ) < ( pNDEFConnection->aCCFile[nIndex] & 0xF0 ) )
   {
      PDebugError("static_PNDEFType4ReadCapabilityContainer: higher version");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   /* Reset the serial number information */
   pNDEFConnection->nSerialNumberLength = 0;

   /* Maximum R-APDU size */
   pNDEFConnection->sType.t4.nMaxRAPDUSize   = (pNDEFConnection->aCCFile[nIndex + 1] << 8) + pNDEFConnection->aCCFile[nIndex + 2];
   /* Maximum C-APDU size */
   pNDEFConnection->sType.t4.nMaxCAPDUSize   = (pNDEFConnection->aCCFile[nIndex + 3] << 8) + pNDEFConnection->aCCFile[nIndex + 4];

   PDebugTrace(
      "static_PNDEFType4ReadCapabilityContainer: maximum R-APDU size %d",
      pNDEFConnection->sType.t4.nMaxRAPDUSize );

   PDebugTrace(
      "static_PNDEFType4ReadCapabilityContainer: maximum C-APDU size %d",
      pNDEFConnection->sType.t4.nMaxCAPDUSize );

   /* Reset the NDEF ID */
   pNDEFConnection->nNDEFId = 0xFFFF;
   /* Go through the TLV block(s) */
   nIndex = 7;
   nNumber = 0;
   while ( pNDEFConnection->nCCLength - nIndex > 0 )
   {
      PDebugTrace(
         "static_PNDEFType4ReadCapabilityContainer: TLV block %d",
         nNumber );
      /* Get the TLV type */
      nType = pNDEFConnection->aCCFile[nIndex];
      /* Get the length */
      if ( pNDEFConnection->aCCFile[nIndex + 1] == 0xFF )
      {
         nTLVLength  = (pNDEFConnection->aCCFile[nIndex + 2] << 8)
                     + pNDEFConnection->aCCFile[nIndex + 3];
         nIndex += 4;
      }
      else
      {
         nTLVLength = pNDEFConnection->aCCFile[nIndex + 1];
         nIndex += 2;
      }
      /* Check the TLV type */
      switch ( nType )
      {
         case 0x04:
            PDebugTrace("static_PNDEFType4ReadCapabilityContainer: NDEF");
            break;
         case 0x05:
            PDebugTrace("static_PNDEFType4ReadCapabilityContainer: Proprietary");
            nIndex += nTLVLength;
            continue;
         default:
            PDebugError(
               "static_PNDEFType4ReadCapabilityContainer: invalid TLV type 0x%02X",
               pNDEFConnection->aCCFile[nIndex] );
            return W_ERROR_CONNECTION_COMPATIBILITY;
      }
      PDebugTrace(
         "static_PNDEFType4ReadCapabilityContainer: length %d",
         nTLVLength );

      /* Calculate the NDEF ID */
      nNDEFId  = (pNDEFConnection->aCCFile[nIndex] << 8)
               + pNDEFConnection->aCCFile[nIndex + 1];
      /* Check the value */
      if (  ( nNDEFId == 0xFFFF )
         || ( nNDEFId == 0xE102 )
         || ( nNDEFId == 0xE103 )
         || ( nNDEFId == 0x3F00 )
         || ( nNDEFId == 0x3FFF ) )
      {
         PDebugWarning(
            "static_PNDEFType4ReadCapabilityContainer: incorrect nNDEFId 0x%04X",
            nNDEFId );
         nIndex += nTLVLength;
         continue;
      }

      if ((pNDEFConnection->sType.t4.nVariant == P_NDEF_4_VERSION_20) && ( nNDEFId == 0x0000))
      {
           PDebugWarning(
            "static_PNDEFType4ReadCapabilityContainer: incorrect nNDEFId 0x%04X",
            nNDEFId );
         nIndex += nTLVLength;
         continue;
      }

      /* Store the NDEF ID */
      pNDEFConnection->nNDEFId = nNDEFId;
      PDebugTrace(
         "static_PNDEFType4ReadCapabilityContainer: nNDEFId 0x%04X",
         pNDEFConnection->nNDEFId );
      nIndex += 2;

      /* Calculate the maximum file size */
      nLength  = (pNDEFConnection->aCCFile[nIndex] << 8)
               + pNDEFConnection->aCCFile[nIndex + 1];
      /* Check the value */
      if (  ( nLength == 0x0000 )
         || ( nLength == 0x0001 )
         || ( nLength == 0x0002 )
         || ( nLength == 0xFFFF ) )
      {
         PDebugWarning(
            "static_PNDEFType4ReadCapabilityContainer: incorrect length 0x%04X",
            nLength );
         nIndex += nTLVLength - 2;
         continue;
      }
      if (  ( nType == 0x04 )
         && (  ( nLength == 0x0003 )
            || ( nLength == 0x0004 ) ) )
      {
         PDebugWarning(
            "static_PNDEFType4ReadCapabilityContainer: incorrect length 0x%04X",
            nLength );
         nIndex += nTLVLength - 2;
         continue;
      }
      /* Store the maximum file size */
      pNDEFConnection->nMaximumSpaceSize = nLength - 2;
      pNDEFConnection->nFreeSpaceSize = nLength - 2;
      PDebugTrace(
         "static_PNDEFType4ReadCapabilityContainer: nMaximumSpaceSize 0x%04X",
         pNDEFConnection->nMaximumSpaceSize );
      nIndex += 2;

      /* Check the read access value */
      if (  (  ( nType == 0x04 )
            && ( pNDEFConnection->aCCFile[nIndex] != 0 ) )
         || (  ( pNDEFConnection->aCCFile[nIndex] > 0x00 )
            && ( pNDEFConnection->aCCFile[nIndex] <= 0x7F ) )
         || ( pNDEFConnection->aCCFile[nIndex] == 0xFF ) )
      {
         PDebugWarning(
            "static_PNDEFType4ReadCapabilityContainer: incorrect read access 0x%02X",
            pNDEFConnection->aCCFile[nIndex] );
         nIndex += nTLVLength - 4;
         continue;
      }
      nIndex += 1;

      /* Check the write access value */
      if (  (  ( nType == 0x04 )
            && ( pNDEFConnection->aCCFile[nIndex] != 0 )
            && ( pNDEFConnection->aCCFile[nIndex] != 0xFF ) )
         || (  ( pNDEFConnection->aCCFile[nIndex] > 0x00 )
            && ( pNDEFConnection->aCCFile[nIndex] <= 0x7F ) ) )
      {
         PDebugWarning(
            "static_PNDEFType4ReadCapabilityContainer: incorrect write access 0x%02X",
            pNDEFConnection->aCCFile[nIndex] );
         nIndex += nTLVLength - 5;
         continue;
      }
      if ( pNDEFConnection->aCCFile[nIndex] == 0xFF )
      {
         PDebugTrace("static_PNDEFType4ReadCapabilityContainer: locked");
         pNDEFConnection->bIsLocked = true;
      }
      else
      {
         PDebugTrace("static_PNDEFType4ReadCapabilityContainer: not locked");
         pNDEFConnection->bIsLocked = false;

      }

      /* Type 4 tags are not lockable */
      pNDEFConnection->bIsLockable = false;
      nIndex += 1;
   }

   /* If no NDEF has been found */
   if ( pNDEFConnection->nNDEFId == 0xFFFF )
   {
      PDebugError("static_PNDEFType4ReadCapabilityContainer: no nNDEFId found");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   return W_SUCCESS;
}

/* See Header file */
static W_ERROR static_PNDEFType4SendCommand(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint32_t nOffset,
            uint32_t nLength )
{
   /* Send the corresponding command */
   switch ( pNDEFConnection->nCommandType )
   {
      /* Generic commands */

      case P_NDEF_COMMAND_READ_NDEF:
      case P_NDEF_COMMAND_READ_NDEF_SPLIT:

         PDebugTrace("P_NDEF_COMMAND_READ_NDEF : nOffset %d nLength %d", nOffset, nLength);

         if ( pNDEFConnection->nCommandType == P_NDEF_COMMAND_READ_NDEF )
         {
            PDebugTrace("static_PNDEFType4SendCommand: P_NDEF_COMMAND_READ_NDEF");
         }
         else
         {
            PDebugTrace("static_PNDEFType4SendCommand: P_NDEF_COMMAND_READ_NDEF_SPLIT");
         }

         static_PNDEFType4ReadFile(pContext, pNDEFConnection, static_PNDEFType4ReadNDEFCompleted, pNDEFConnection, nOffset + 2, nLength);
         break;

      case P_NDEF_COMMAND_WRITE_NDEF :

         PDebugTrace("P_NDEF_COMMAND_WRITE_NDEF : nOffset %d nLength %d", nOffset, nLength);

         pNDEFConnection->sType.t4.pBuffer = pNDEFConnection->pMessageBuffer;
         static_PNDEFType4WriteFile(pContext, pNDEFConnection, static_PNDEFType4WriteNDEFCompleted, pNDEFConnection, nOffset + 2, nLength);
         break;

      case P_NDEF_COMMAND_WRITE_NDEF_LENGTH :

         PDebugTrace("static_PNDEFType4SendCommand: P_NDEF_COMMAND_WRITE_NDEF_LENGTH");

         pNDEFConnection->nNDEFFileLength = pNDEFConnection->nUpdatedNDEFFileLength;
         pNDEFConnection->nFreeSpaceSize = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;

         pNDEFConnection->aSendBuffer[0x10] = (uint8_t)((pNDEFConnection->nNDEFFileLength >> 8) & 0xFF);
         pNDEFConnection->aSendBuffer[0x11] = (uint8_t)(pNDEFConnection->nNDEFFileLength & 0xFF);
         pNDEFConnection->sType.t4.pBuffer = pNDEFConnection->aSendBuffer + 0x10;

         static_PNDEFType4WriteFile(pContext, pNDEFConnection, static_PNDEFType4WriteNDEFCompleted, pNDEFConnection, 0, 2);
         break;

      case P_NDEF_COMMAND_LOCK_TAG:


         PDebugError("static_PNDEFType4SendCommand: P_NDEF_COMMAND_LOCK_TAG not supported for type 4 tags");

         PNDEFSendError(pContext, pNDEFConnection, W_ERROR_TAG_NOT_LOCKABLE);
         break;

      default:
         PDebugError("static_PNDEFType4SendCommand: command 0x%02X not supported", pNDEFConnection->nCommandType);
         return W_ERROR_BAD_PARAMETER;

   }

   return W_SUCCESS;
}

/**
  * Read NDEF operation completed
  */
static void static_PNDEFType4ReadNDEFCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;

   PDebugTrace("static_PNDEFType4ReadNDEFCompleted nError %d", nError);

   nError2 = PNDEFSendCommandCompleted(pContext, pNDEFConnection, pNDEFConnection->aReceivedBuffer, pNDEFConnection->sType.t4.nBytesProcessed, nError);

   if (nError2 != W_SUCCESS)
   {
      PNDEFSendError(pContext, pNDEFConnection, nError2);
   }
}


/**
  * Write NDEF operation completed
  */
static void static_PNDEFType4WriteNDEFCompleted(
               tContext* pContext,
               void * pCallbackParameter,
               uint32_t nReceivedAPDULength,
               W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;

   nError2 = PNDEFSendCommandCompleted(pContext, pNDEFConnection, null, 0, nError);

   if (nError2 != W_SUCCESS)
   {
      PNDEFSendError(pContext, pNDEFConnection, nError2);
   }
}


/* The NDEF type information structure */
tNDEFTypeEntry g_sPNDEFType4AInfo = {
   W_PROP_NFC_TAG_TYPE_4_A,
   &static_PNDEFType4CreateConnection,
   &static_PNDEFType4SendCommand,
   null};      /* no cache invalidation */

/* The NDEF type information structure */
tNDEFTypeEntry g_sPNDEFType4BInfo = {
   W_PROP_NFC_TAG_TYPE_4_B,
   &static_PNDEFType4CreateConnection,
   &static_PNDEFType4SendCommand,
   null};      /* no cache invalidation */

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

