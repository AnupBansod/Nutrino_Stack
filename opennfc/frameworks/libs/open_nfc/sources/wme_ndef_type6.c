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
   Contains the NDEF Type 6 API implementation.
*******************************************************************************/
#define P_MODULE P_MODULE_DEC( NDEFA6 )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* NFC Type 6 Tag defines */
#define P_NDEF_6_CC_OFFSET                   0x00
#define P_NDEF_6_CC_LENGTH                   0x08

#define P_NDEF_6_MAPPING_VERSION             0x10       /* 1.0 */
#define P_NDEF_6_CC_BLOCK                    0x00

/* NFC Type 6 Tag defines */

#define P_NDEF_6_LOCK_START                  0x00
#define P_NDEF_6_WRITE_CC                    0x01
#define P_NDEF_6_LOCK_TAG                    0x02

/* ------------------------------------------------------- */
/*                CONNECTION CREATION                      */
/* ------------------------------------------------------- */

static void static_PNDEFType6ReadCCCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );
static W_ERROR static_PNDEFType6ReadCapabilityContainer(tContext* pContext, tNDEFConnection* pNDEFConnection );
static void static_PNDEFType6ReadNDEFLengthCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );

/**
 * @brief   Creates a NDEF TAG Type 6 connection
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pNDEFConnection  The PNDEF connection
 *
 * @return W_SUCCESS if the current connection is NDEF type 5 capable
 *
 * The NDEF connection is completed when the PNDEFSendError() is called
 **/
static W_ERROR static_PNDEFType6CreateConnection(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   uint32_t nMaxSize;
   W_ERROR nError;
   bool bIsFormattable;

   /* Check the type */
   if ( ( nError = P15P3UserCheckType6(
                        pContext,
                        pNDEFConnection->hConnection,
                        &nMaxSize,
                        &pNDEFConnection->bIsLocked,
                        &pNDEFConnection->bIsLockable,
                        &bIsFormattable,
                        pNDEFConnection->aSerialNumber,
                        &pNDEFConnection->nSerialNumberLength ) ) != W_SUCCESS )
   {
      PDebugError("static_PNDEFType6CreateConnection: not correct type 6");
      return nError;
   }
   /* Set the maximum space size */
   pNDEFConnection->nMaximumSpaceSize = nMaxSize;

   /* Read the Capability Container */
   pNDEFConnection->nReceivedDataLength = P_NDEF_6_CC_LENGTH;
   P15Read(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType6ReadCCCompleted,
            pNDEFConnection,
            pNDEFConnection->aCCFile,
            0,
            P_NDEF_6_CC_LENGTH,
            null );

   return W_SUCCESS;
}


/**
 * @brief Capability Container Read callback
 *
 */
static void static_PNDEFType6ReadCCCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;

   PDebugTrace("static_PNDEFType6ReadCCCompleted");

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFType6ReadCCCompleted : Error %s", PUtilTraceError(nError));

      /* Send the error */
      PNDEFSendError(pContext, pNDEFConnection, nError);
      return;
   }

   nError = static_PNDEFType6ReadCapabilityContainer(pContext, pNDEFConnection);

   if (nError != W_SUCCESS)
   {
      PNDEFSendError(pContext, pNDEFConnection, nError);
      return;
   }

   /* read the actual NDEF file size */

   pNDEFConnection->nReceivedDataLength = 2;

   P15Read(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType6ReadNDEFLengthCompleted,
            pNDEFConnection,
            pNDEFConnection->aReceivedBuffer,
            pNDEFConnection->nNDEFId,
            2,
            null );

}

/**
 * @brief   Parses the content of the capability container
 *
 */
static W_ERROR static_PNDEFType6ReadCapabilityContainer(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   bool bWriteStatus = false;
   uint8_t nCCBlockNumber;
   uint8_t nSectorNumber;
   uint32_t nIndex = 0;
   uint32_t nLength;
   static const uint8_t pType6String[] = { 0x4E, 0x44, 0x45 }; /* "NDE" */

   /* Check the NDEF identification string */
   if ( CMemoryCompare(
            pType6String,
            pNDEFConnection->aCCFile,
            3 ) != 0 )
   {
      PDebugLog("static_PNDEFType6ReadCapabilityContainer: wrong identification string");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   nIndex = 3;
   /* Mapping version */
   PDebugTrace(
      "static_PNDEFType6ReadCapabilityContainer: version %d.%d",
      (pNDEFConnection->aCCFile[nIndex] >> 4),
      (pNDEFConnection->aCCFile[nIndex] & 0x0F) );
   if ( ( P_NDEF_6_MAPPING_VERSION & 0xF0 ) < ( pNDEFConnection->aCCFile[nIndex] & 0xF0 ) )
   {
      PDebugError("static_PNDEFType6ReadCapabilityContainer: higher version");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   nIndex ++;
   /* Get the block number and size */
   pNDEFConnection->sType.t6.nSectorSize = (pNDEFConnection->aCCFile[nIndex ++] & 0x1F) + 1;
   nSectorNumber = pNDEFConnection->aCCFile[nIndex ++] + 1;

   /* Set the sector number and size */
   if (P15SetTagSize(pContext, pNDEFConnection->hConnection, nSectorNumber, pNDEFConnection->sType.t6.nSectorSize ) == W_ERROR_BAD_PARAMETER)
   {
      PDebugError("static_PNDEFType6ReadCapabilityContainer: unable to set the tag size (CC NDEF bad format)");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Get the CC file block number */
   nCCBlockNumber = (P_NDEF_6_CC_LENGTH + pNDEFConnection->sType.t6.nSectorSize - 1) / pNDEFConnection->sType.t6.nSectorSize;

   pNDEFConnection->nMaximumSpaceSize = (nSectorNumber - nCCBlockNumber) * pNDEFConnection->sType.t6.nSectorSize - 2;

   /* Check the write access value */
   switch ( pNDEFConnection->aCCFile[nIndex ++] )
   {
      case 0x00:
         PDebugTrace("static_PNDEFType6ReadCapabilityContainer: not locked");
         bWriteStatus = false;
         break;
      case 0xFF:
         PDebugTrace("static_PNDEFType6ReadCapabilityContainer: locked");
         bWriteStatus = true;
         break;
      default:
         PDebugWarning(
            "static_PNDEFType6ReadCapabilityContainer: incorrect write access 0x%02X",
            pNDEFConnection->aCCFile[nIndex - 1] );
         return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   if ( pNDEFConnection->bIsLocked != bWriteStatus )
   {
      /* Inconsistency between physical lock and NDEF logical lock => consider the Tag as Read-Only, but allow Reading of the Tag */
      pNDEFConnection->bIsLocked = true;

      PDebugWarning("static_PNDEFType6ReadCapabilityContainer: inconsistent write access status");
   }

   /* Check the maximum number of block */
   if ( pNDEFConnection->aCCFile[nIndex] > ( nSectorNumber - nCCBlockNumber ) )
   {
      PDebugWarning(
         "static_PNDEFType6ReadCapabilityContainer: wrong maximum block number 0x%02X",
         pNDEFConnection->aCCFile[nIndex] );
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   /* Calculate the maximum message length */
   nLength  = pNDEFConnection->aCCFile[nIndex] * pNDEFConnection->sType.t6.nSectorSize - 2;
   if ( nLength > pNDEFConnection->nMaximumSpaceSize )
   {
      PDebugWarning(
         "static_PNDEFType6ReadCapabilityContainer: wrong length 0x%02X",
         nLength );
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   /* Store the maximum file size */
   pNDEFConnection->nMaximumSpaceSize = nLength;
   pNDEFConnection->nFreeSpaceSize = nLength;
   PDebugTrace(
      "static_PNDEFType6ReadCapabilityContainer: nMaximumSpaceSize 0x%04X",
      pNDEFConnection->nMaximumSpaceSize );

   /* Set the default file id */
   pNDEFConnection->nNDEFId = nCCBlockNumber * pNDEFConnection->sType.t6.nSectorSize;

   return W_SUCCESS;
}

/**
 * @brief   Read NDEF length callback function
 *
 */
static void static_PNDEFType6ReadNDEFLengthCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;

   PDebugTrace("static_PNDEFType6ReadNDEFLengthCompleted");

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFType6ReadNDEFLengthCompleted : Error %s", PUtilTraceError(nError));

      /* Send the error */
      PNDEFSendError(pContext, pNDEFConnection, nError);
      return;
   }

   pNDEFConnection->nNDEFFileLength = (pNDEFConnection->aReceivedBuffer[0] << 8) + pNDEFConnection->aReceivedBuffer[1];

   if (pNDEFConnection->nNDEFFileLength > pNDEFConnection->nMaximumSpaceSize)
   {
      PDebugError("static_PNDEFType6ReadNDEFLengthCompleted : NDEF actual size larger than NDEF maximum size");
      PNDEFSendError(pContext, pNDEFConnection, W_ERROR_CONNECTION_COMPATIBILITY);
      return;
   }

   /* Compute the remaining available size */
   pNDEFConnection->nFreeSpaceSize = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;


   /* Ok, the operation is now completed */
   PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS);
}


/* ------------------------------------------------------- */
/*                COMMAND PROCESSING                       */
/* ------------------------------------------------------- */

static void static_PNDEFType6ReadNDEFCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );
static void static_PNDEFType6WriteNDEFCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );
static void static_PNDEFType6WriteNDEFLengthCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );
static void static_PNDEFType6LockAutomaton(tContext* pContext, void* pCallbackParameter, W_ERROR nError );

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
static W_ERROR static_PNDEFType6SendCommand(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint32_t nOffset,
            uint32_t nLength )
{
   /* Send the corresponding command */
   switch ( pNDEFConnection->nCommandType )
   {

      case P_NDEF_COMMAND_READ_NDEF:
      case P_NDEF_COMMAND_READ_NDEF_SPLIT:

            if ( pNDEFConnection->nCommandType == P_NDEF_COMMAND_READ_NDEF_SPLIT )
            {
               PDebugTrace("static_PNDEFType6SendCommand: P_NDEF_COMMAND_READ_NDEF_SPLIT");
            }
            else
            {
               PDebugTrace("static_PNDEFType6SendCommand: P_NDEF_COMMAND_READ_NDEF_TNF");
            }

         /* Get the file information */
         pNDEFConnection->nReceivedDataLength = nLength;

         P15Read(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType6ReadNDEFCompleted,
            pNDEFConnection,
            pNDEFConnection->aReceivedBuffer,
            pNDEFConnection->nNDEFId + 2 + nOffset,
            nLength,
            null );

         break;

      case P_NDEF_COMMAND_WRITE_NDEF:

         PDebugTrace("static_PNDEFType6SendCommand : P_NDEF_COMMAND_WRITE_NDEF");

         P15Write(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType6WriteNDEFCompleted,
            pNDEFConnection,
            pNDEFConnection->pMessageBuffer,
            pNDEFConnection->nNDEFId + 2 + nOffset,
            nLength,
            false,
            null );
          break;


      case P_NDEF_COMMAND_WRITE_NDEF_LENGTH:
         PDebugTrace("static_PNDEFType6SendCommand: P_NDEF_COMMAND_WRITE_NDEF_LENGTH");

         pNDEFConnection->nNDEFFileLength = pNDEFConnection->nUpdatedNDEFFileLength;
         pNDEFConnection->nFreeSpaceSize  = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;

         pNDEFConnection->aSendBuffer[0] = (uint8_t) (pNDEFConnection->nNDEFFileLength >> 8);
         pNDEFConnection->aSendBuffer[1] = (uint8_t) pNDEFConnection->nNDEFFileLength;

         P15Write(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType6WriteNDEFLengthCompleted,
            pNDEFConnection,
            pNDEFConnection->aSendBuffer,
            pNDEFConnection->nNDEFId,
            2,
            false,
            null );
         break;

      case P_NDEF_COMMAND_LOCK_TAG:

         PDebugTrace("static_PNDEFType6SendCommand: P_NDEF_COMMAND_LOCK_TAG");

         pNDEFConnection->sType.t6.nLockState = P_NDEF_6_LOCK_START;
         static_PNDEFType6LockAutomaton(pContext, pNDEFConnection, W_SUCCESS);
         break;

      default:
         PDebugError("static_PNDEFType6SendCommand: command 0x%02X not supported", pNDEFConnection->nCommandType);
         return W_ERROR_BAD_PARAMETER;
   }

   return W_SUCCESS;
}

/* @brief   Receives the answer to a Type 6 Tag read NDEF command.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PNDEFType6ReadNDEFCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;

   if (nError == W_SUCCESS)
   {
      PDebugTrace("static_PNDEFType6ReadNDEFCompleted : nError %s", PUtilTraceError(nError));
   }
   else
   {
      PDebugError("static_PNDEFType6ReadNDEFCompleted : nError %s", PUtilTraceError(nError));
   }

   /* Call the generic callback function */
   nError2 = PNDEFSendCommandCompleted(
                     pContext,
                     pNDEFConnection,
                     pNDEFConnection->aReceivedBuffer,
                     pNDEFConnection->nReceivedDataLength,
                     nError );

   if ( (nError2 != W_SUCCESS))
   {
      /* Send the error */
      PNDEFSendError( pContext, pNDEFConnection, nError2 );
   }
}

/* @brief   Receives the answer to a Type 6 Tag write NDEF command.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PNDEFType6WriteNDEFCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;

   if (nError == W_SUCCESS)
   {
      PDebugTrace("static_PNDEFType6WriteNDEFCompleted : nError %s", PUtilTraceError(nError));
   }
   else
   {
      PDebugError("static_PNDEFType6WriteNDEFCompleted : nError %s", PUtilTraceError(nError));
   }

   /* Call the generic callback function */
   nError2 = PNDEFSendCommandCompleted(
                     pContext,
                     pNDEFConnection,
                     null,
                     0,
                     nError );

   if ( (nError2 != W_SUCCESS))
   {
      /* Send the error */
      PNDEFSendError( pContext, pNDEFConnection, nError2 );
   }
}

/* @brief   Receives the answer to a Type 6 Tag write NDEF length command.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PNDEFType6WriteNDEFLengthCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;

   if (nError == W_SUCCESS)
   {
      PDebugTrace("static_PNDEFType6WriteNDEFLengthCompleted : nError %s", PUtilTraceError(nError));
   }
   else
   {
      PDebugError("static_PNDEFType6WriteNDEFLengthCompleted : nError %s", PUtilTraceError(nError));
   }

   /* Call the generic callback function */
   nError2 = PNDEFSendCommandCompleted(
                     pContext,
                     pNDEFConnection,
                     null,
                     0,
                     nError );

   if ( (nError2 != W_SUCCESS))
   {
      /* Send the error */
      PNDEFSendError( pContext, pNDEFConnection, nError2 );
   }
}

/**
  * @brief TAG locking automaton
  *
  * @param[in]  pContext  The context.
  *
  * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
  *
  * @param[in]  nError  The error code of the operation.
  */
static void static_PNDEFType6LockAutomaton(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;

   if (nError != W_SUCCESS)
   {
      /* Call the generic callback function */
      nError2 = PNDEFSendCommandCompleted(
                        pContext,
                        pNDEFConnection,
                        null,
                        0,
                        nError );

      if ( (nError2 != W_SUCCESS))
      {
         /* Send the error */
         PNDEFSendError( pContext, pNDEFConnection, nError2 );
         return;
      }
   }

   switch (pNDEFConnection->sType.t6.nLockState)
   {
      case P_NDEF_6_LOCK_START :

         pNDEFConnection->aCCFile[0x06] = 0xFF;

         P15Write(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType6LockAutomaton,
            pNDEFConnection,
            pNDEFConnection->aCCFile,
            P_NDEF_6_CC_OFFSET,
            P_NDEF_6_CC_LENGTH,
            false,
            null );

         pNDEFConnection->sType.t6.nLockState = P_NDEF_6_WRITE_CC;

         break;

      case P_NDEF_6_WRITE_CC :

         P15Write(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType6LockAutomaton,
            pNDEFConnection,
            null,
            0,
            0,
            true,
            null );

         pNDEFConnection->sType.t6.nLockState = P_NDEF_6_LOCK_TAG;
         break;


      case P_NDEF_6_LOCK_TAG :

         nError2 = PNDEFSendCommandCompleted(
               pContext,
               pNDEFConnection,
               null,
               0,
               nError );

         if (nError2 != W_SUCCESS)
         {
            PNDEFSendError(pContext, pNDEFConnection, nError2 );
         }
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

static W_ERROR static_PNDEFType6InvalidateCache(
   tContext* pContext,
   tNDEFConnection* pNDEFConnection,
   uint32_t nOffset,
   uint32_t nLength)
{
   return P15InvalidateCache(pContext, pNDEFConnection->hConnection, pNDEFConnection->nNDEFId + 2 + nOffset, nLength);
}



/* The NDEF type information structure */
tNDEFTypeEntry g_sPNDEFType6Info = {
   W_PROP_NFC_TAG_TYPE_6,
   static_PNDEFType6CreateConnection,
   static_PNDEFType6SendCommand,
   static_PNDEFType6InvalidateCache};

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

