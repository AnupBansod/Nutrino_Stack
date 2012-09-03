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
   Contains the implementation of the My-d functions
*******************************************************************************/

#define P_MODULE  P_MODULE_DEC( MY_D )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#define P_MY_D_MAX_SIZE_BUFFER               16

/* The My-d move is organised in 38 blocks of 4 bytes */
#define P_MY_D_BLOCK_SIZE                    P_NDEF2GEN_BLOCK_SIZE
#define P_MY_D_MOVE_BLOCK_NUMBER             38

/* The My-d NFC is organised in pages of 8 bytes */
#define P_MY_D_NFC_PAGE_SIZE                 8

/* My-d block info */
#define P_MY_D_MOVE_CONFIG_BYTE_OFFSET       1
#define P_MY_D_MOVE_PROTECTED_FIRST_BLOCK    0x10
#define P_MY_D_MOVE_LOCK_BLOCK               0x24
#define P_MY_D_MOVE_LOCK_LENGTH              4

#define P_MY_D_FIRST_DATA_BLOCK              (P_NDEF2GEN_STATIC_LOCK_BLOCK + 1)
#define P_MY_D_MOVE_LAST_DATA_BLOCK          (P_MY_D_MOVE_LOCK_BLOCK - 1)
#define P_MY_D_NFC_LAST_DATA_BLOCK           (pMyDConnection->nSectorNumber - 1)

#define P_MY_D_MOVE_MAX_RETRY_COUNTER        7
#define P_MY_D_MOVE_STATUS_BYTE_MASK         (W_MY_D_MOVE_16BITS_COUNTER_ENABLED | W_MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED | W_MY_D_MOVE_WRITE_PASSWORD_ENABLED | W_MY_D_MOVE_CONFIG_LOCKED)
#define P_MY_D_MOVE_PASSWORD_LENGTH          4

/* Smart cache read/write block number and align */
#define P_MY_D_NFC_READ_BLOCK_NUMBER         1
#define P_MY_D_NFC_READ_BLOCK_ALIGN          1
#define P_MY_D_NFC_WRITE_BLOCK_NUMBER        1
#define P_MY_D_NFC_WRITE_BLOCK_ALIGN         1

#define P_MY_D_NFC_CMD_WRITE                 0x30
#define P_MY_D_NFC_CMD_WRITE_BYTE            0xE0
#define P_MY_D_NFC_CMD_READ                  0x10

#define P_MY_D_NFC_OFFSET_ACCESS_CONDITION   0x09

#define P_MY_D_NFC_VALUE_ACCESS_CONDITION_LOCKED   0x66


/*cache Connection defines*/
#define P_MY_D_IDENTIFIER_LEVEL                    ZERO_IDENTIFIER_LEVEL

extern tSmartCacheSectorSize g_sSectorSize8;
extern tSmartCacheSectorSize g_sSectorSize4;


/* Declare a My-d exchange data structure */
typedef struct __tMyDConnection
{
   /* Memory handle registry */
   tHandleObjectHeader        sObjectHeader;
   /* Connection handle */
   W_HANDLE                   hConnection;

   /* Connection information */
   uint8_t                    nUIDLength;
   uint8_t                    UID[7];

   /* Type of the card (Move, NFC) */
   uint8_t                    nType;

   /* Lock bytes */
   uint8_t                    aLockBytes[6];
   bool                       bLockBytesRetrieved;

 
   union
   {
      struct
      {
         /* Config byte of My-d move */
         uint8_t              nMyDMoveConfigByte;
         uint8_t              nPendingConfigByte;

         

         /* true if authentication has been done */
         bool                 bAuthenticated;
      } sMove;
      struct
      {
         tSmartCache*         pSmartCacheDynamic;
         tDFCCallbackContext  sCacheCallbackContext;
      }sNFC;
   } uMyD;     

   /* temporary data */
   uint8_t                    aCommandBuffer[P_MY_D_MAX_SIZE_BUFFER];
   uint8_t                    aResponseBuffer[P_MY_D_MAX_SIZE_BUFFER];

   uint32_t                   nOffsetToLock;
   uint32_t                   nLengthToLock;

   uint32_t                   nCurrentOperationState;

   /* Smart cache data */
   tSmartCacheSectorSize*     pSectorSize;
   uint32_t                   nSectorNumber;

   /* true if an operation is pending to avoid reentrance */
   bool                       bOperationPending;

   /* Callback context */
   tDFCCallbackContext        sCallbackContext;

} tMyDConnection;

static void static_PMyDNFCReadDynamic(
               tContext* pContext,
               void* pConnection,
               uint32_t nSectorOffset,
               uint32_t nSectorNumber,
               uint8_t* pBuffer,
               tPBasicGenericCallbackFunction* pCallback,
               void* pCallbackParameter);


/* See tSmartCacheWriteSector */
static void static_PMyDNFCWriteDynamic(
               tContext* pContext,
               void* pConnection,
               uint32_t nSectorOffset,
               uint32_t nSectorNumber,
               const uint8_t* pBuffer,
               tPBasicGenericCallbackFunction* pCallback,
               void* pCallbackParameter);


/*SmartCacheDescriptor for myD NFC */
static tSmartCacheDescriptor g_sDescriptorMyDNFCDynamic = {
   P_MY_D_IDENTIFIER_LEVEL, &g_sSectorSize8,
   {
      { P_MY_D_NFC_READ_BLOCK_ALIGN, P_MY_D_NFC_READ_BLOCK_NUMBER, static_PMyDNFCReadDynamic },
      { 0, 0, null }
   },
   {
      { P_MY_D_NFC_WRITE_BLOCK_ALIGN, P_MY_D_NFC_WRITE_BLOCK_NUMBER, static_PMyDNFCWriteDynamic },
      { 0, 0, null }
   },
};

/**
 * @brief   Destroyes a My-d connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PMyDDestroyConnection(
            tContext* pContext,
            void* pObject )
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pObject;

   PDebugTrace("static_PMyDDestroyConnection");

   PDFCFlushCall(&pMyDConnection->sCallbackContext);

   if(pMyDConnection->nType == W_PROP_MY_D_NFC
      && pMyDConnection->uMyD.sNFC.pSmartCacheDynamic != null)
   {

      PSmartCacheDestroyCache(pContext, pMyDConnection->uMyD.sNFC.pSmartCacheDynamic);
      /* Free the smartCache used by the connection */
      CMemoryFree(pMyDConnection->uMyD.sNFC.pSmartCacheDynamic);
   }

   /* Free the My-d connection structure */
   CMemoryFree( pMyDConnection );

   return P_HANDLE_DESTROY_DONE;
}

/**
 * @brief   Gets the My-d connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 **/

static uint32_t static_PMyDGetPropertyNumber(
            tContext* pContext,
            void* pObject)
{
   return 1;
}

/**
 * @brief   Gets the My-d connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  pPropertyArray  The property array.
 **/
static bool static_PMyDGetProperties(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray )
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pObject;

   PDebugTrace("static_PMyDGetProperties");

   pPropertyArray[0] = pMyDConnection->nType;

   return true;
}

/**
 * @brief   Checkes the My-d connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  nPropertyValue  The property value.
 **/
static bool static_PMyDCheckProperties(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue )
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pObject;

   PDebugTrace("static_PMyDCheckProperties: nPropertyValue=%s (0x%02X)",
               PUtilTraceConnectionProperty(nPropertyValue), nPropertyValue);

   return (pMyDConnection->nType == nPropertyValue) ? true : false;
}

/* Handle registry My-d connection type */
tHandleType g_sMyDConnection = { static_PMyDDestroyConnection,
                                 null,
                                 static_PMyDGetPropertyNumber,
                                 static_PMyDGetProperties,
                                 static_PMyDCheckProperties };

#define P_HANDLE_TYPE_MY_D_CONNECTION (&g_sMyDConnection)


#define GetBit(value, bit)    (((value) >> (bit)) & 0x01)
#define SetBit(value, bit)    ((value) = (value) | (1 << (bit)))

/**
  * Retreive the lock status of a sector by parsing the lock bytes of the card
  *
  * return true if the sector is locked
  */
static bool static_PMyDIsSectorLocked(
      tContext * pContext,
      tMyDConnection * pMyDConnection,
      uint32_t         nSector)
{
   if (nSector <= 1)
   {
      /* Sectors [0 - 1] are locked */
      return true;
   }

   if (nSector == 2)
   {
      /* Check the block locking lockbits */
      return (pMyDConnection->aLockBytes[0] & 0x07) == 0x07;
   }

   if ((3 <= nSector) && (nSector <= 7))
   {
      /* sectors 3-7 locks are located in aLockBytes[0] */
      return GetBit(pMyDConnection->aLockBytes[0], nSector);
   }

   if ((8 <= nSector) && (nSector <= 15))
   {
      /* sectors 8-15 locks are located in aLockBytes[1] */
      return GetBit(pMyDConnection->aLockBytes[1], nSector - 8);
   }

   if (W_PROP_MY_D_MOVE == pMyDConnection->nType)
   {
      if ((16 <= nSector)  && (nSector <= 23))
      {
         return GetBit(pMyDConnection->aLockBytes[2], nSector - 16);
      }

      if ((24 <= nSector)  && (nSector <= 31))
      {
         return GetBit(pMyDConnection->aLockBytes[3], nSector - 24);
      }

      if ((32 <= nSector)  && (nSector <= 35))
      {
         return GetBit(pMyDConnection->aLockBytes[4], nSector - 32);
      }

      if (nSector == 36)
      {
         /* Check the block locking lockbits */
         return ( (pMyDConnection->aLockBytes[5] & 0x0F) == 0x0F );
      }

      if (nSector == 37)
      {
         /* Manufacturer block is locked */
         return true;
      }
   }
   else if (W_PROP_MY_D_NFC == pMyDConnection->nType)
   {
      /* No lock bytes for these sectors in My-d NFC */
      if ((16 <= nSector)  && (nSector < pMyDConnection->nSectorNumber))
      {
         return false;
      }
   }

   /* should not occur */
   CDebugAssert(0);
   return true;
}


static void static_PMyDLockSector(
      tContext * pContext,
      tMyDConnection * pMyDConnection,
      uint32_t         nSector)
{
   if ((3 <= nSector) && (nSector <= 7))
   {
      /* sectors 3-7 locks are located in aLockBytes[0] */
      SetBit(pMyDConnection->aLockBytes[0], nSector);
      return;
   }

   if ((8 <= nSector) && (nSector <= 15))
   {
      /* sectors 8-15 locks are located in aLockBytes[1] */
      SetBit(pMyDConnection->aLockBytes[1], nSector - 8);
      return;
   }

   if (W_PROP_MY_D_MOVE == pMyDConnection->nType)
   {
      if ((16 <= nSector)  && (nSector <= 23))
      {
         SetBit(pMyDConnection->aLockBytes[2], nSector - 16);
         return;
      }

      if ((24 <= nSector)  && (nSector <= 31))
      {
         SetBit(pMyDConnection->aLockBytes[3], nSector - 24);
         return;
      }

      if ((32 <= nSector)  && (nSector <= 35))
      {
         SetBit(pMyDConnection->aLockBytes[4], nSector - 32);
         return;
      }
   }
   else if (W_PROP_MY_D_NFC == pMyDConnection->nType)
   {
      /* No lock bytes for these sectors in My-d NFC */
      if ((16 <= nSector)  && (nSector < pMyDConnection->nSectorNumber))
      {
         return;
      }
   }

   /* should not occur */
   CDebugAssert(0);
}

/**
  * Retreive the lockable status of a sector by parsing the lock bytes of the card
  *
  * return true if the sector is lockable
  */
static bool static_PMyDIsSectorLockable(
      tContext * pContext,
      tMyDConnection * pMyDConnection,
      uint32_t         nSector)
{
   if (pMyDConnection->nType == W_PROP_MY_D_MOVE)
   {
      if (nSector <= 2)
      {
         /* Lock bytes shall be locked with the dedicated API */
         return false;
      }

      if ((3 <= nSector) && (nSector <= 15))
      {
         return GetBit(pMyDConnection->aLockBytes[0], (nSector + 2) / 6) == 0;
      }

      if ((16 <= nSector) && (nSector <= 35))
      {
         return GetBit(pMyDConnection->aLockBytes[5], (nSector - 16) / 5) == 0;
      }

      if (nSector == 36)
      {
         /* Lock bytes shall be locked with the dedicated API */
         return false;
      }

      if (nSector == 37)
      {
         /* Manufacturer block is not lockable */
         return false;
      }
   }
   else
   {
      /* My-d NFC is not lockable */
      return false;
   }

   /* should not occur */
   CDebugAssert(0);
   return false;
}

/**
 * @brief   Sends the result.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pMyDConnection  The My-d connection.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PMyDSendResult(
            tContext* pContext,
            tMyDConnection* pMyDConnection,
            W_ERROR nError )
{
   PDebugTrace("static_PMyDSendResult");

   /* Clear the operation in progress flag */
   CDebugAssert(pMyDConnection->bOperationPending != false);
   pMyDConnection->bOperationPending = false;

   /* Send the error */
   PDFCPostContext2(&pMyDConnection->sCallbackContext, P_DFC_TYPE_MY_D, nError);

   /* Decrement the reference count of the connection. This may destroy the object */
   PHandleDecrementReferenceCount(pContext, pMyDConnection);
}

/**
 *  Lock Bytes 0-1 have been written.
 */
static void static_PMyDWriteLockBytesCompleted(
            tContext * pContext,
            void * pCallbackParameter,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*) pCallbackParameter;

   PDebugTrace("static_PMyDWriteLockBytesCompleted");

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PMyDWriteLockBytesCompleted: returning %s", PUtilTraceError(nError));
   }

   static_PMyDSendResult(pContext, pMyDConnection, nError);
}

/**
 *  Lock Bytes 2-3 have been written.
 */
static void static_PMyDWriteLockBytes2Completed(
            tContext * pContext,
            void * pCallbackParameter,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = pCallbackParameter;

   PDebugTrace("static_PMyDWriteLockBytes2Completed");

   if (nError == W_SUCCESS)
   {
      PNDEF2GenWrite(pContext,
                     pMyDConnection->hConnection,
                     static_PMyDWriteLockBytesCompleted,
                     pMyDConnection,
                     &pMyDConnection->aLockBytes[0],
                     P_NDEF2GEN_STATIC_LOCK_BYTE_ADDRESS,
                     P_NDEF2GEN_STATIC_LOCK_BYTE_LENGTH,
                     null);
   }
   else
   {
      PDebugError("static_PMyDWriteLockBytes2Completed: returning %s", PUtilTraceError(nError));

      static_PMyDSendResult(pContext, pMyDConnection, nError);
   }
}


/**
 *  Writes the lock bytes into the card
 */
static void static_PMyDWriteLockBytes(
            tContext* pContext,
            tMyDConnection* pMyDConnection)
{
   PDebugTrace("static_PMyDWriteLockBytes");

   if (pMyDConnection->nType == W_PROP_MY_D_MOVE)
   {
      PNDEF2GenWrite(pContext,
                     pMyDConnection->hConnection,
                     static_PMyDWriteLockBytes2Completed,
                     pMyDConnection,
                     &pMyDConnection->aLockBytes[2],
                     pMyDConnection->pSectorSize->pMultiply(P_MY_D_MOVE_LOCK_BLOCK),
                     P_MY_D_MOVE_LOCK_LENGTH,
                     null);
   }
   else
   {
      /* fake the write of the Lock Bytes 2-3 */
      static_PMyDWriteLockBytes2Completed(pContext, pMyDConnection, W_SUCCESS);
   }
}

/**
 * Bytes have been read
 */
static void static_PMyDReadCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;

   PDebugTrace("static_PMyDReadCompleted");

   static_PMyDSendResult(pContext, pMyDConnection, nError );
}

static void static_PMyDNFCLockSectorAutomaton(
            tContext* pContext,
            void * pCallbackParameter,
            uint32_t nLength,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;
   PDebugTrace("static_PMyDWriteCompleted");

   if(nLength == 1)
   {
      /* if we have locked all sectors, we return send the callback and exit this function */
      if(pMyDConnection->nLengthToLock == 0)
      {
         static_PMyDSendResult( pContext, pMyDConnection, W_SUCCESS );
      }
      else
      {
         uint32_t nIndex = 0;

         CDebugAssert(pMyDConnection->nLengthToLock > 0);

         /* Command byte */
         pMyDConnection->aCommandBuffer[nIndex++] = P_MY_D_NFC_CMD_WRITE_BYTE;

         /* Page Adr => lower byte first*/
         pMyDConnection->aCommandBuffer[nIndex++] = (uint8_t)(pMyDConnection->nOffsetToLock & 0x000000FF);
         pMyDConnection->aCommandBuffer[nIndex++] = (uint8_t)((pMyDConnection->nOffsetToLock & 0x0000FF00) >> 8);

         /* Byte written offset => Access condition */
         pMyDConnection->aCommandBuffer[nIndex++] = P_MY_D_NFC_OFFSET_ACCESS_CONDITION;
         pMyDConnection->aCommandBuffer[nIndex++] = P_MY_D_NFC_VALUE_ACCESS_CONDITION_LOCKED;

         pMyDConnection->nLengthToLock --;
         pMyDConnection->nOffsetToLock ++;

         P14P3UserExchangeData(
                  pContext,
                  pMyDConnection->hConnection,
                  static_PMyDNFCLockSectorAutomaton,
                  pMyDConnection,
                  pMyDConnection->aCommandBuffer,
                  nIndex,
                  pMyDConnection->aResponseBuffer,
                  sizeof(pMyDConnection->aResponseBuffer),
                  null,
                  false);
      }
      return;
   }

    /* Send the result */
   static_PMyDSendResult( pContext, pMyDConnection, nError );
}

/**
 * Bytes have been written
 */
static void static_PMyDNFCWriteCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;

   PDebugTrace("static_PMyDWriteCompleted");

   if(nError == W_SUCCESS)
   {
      /* Check if we need to lock the bytes */
      if ( pMyDConnection->nLengthToLock != 0 )
      {
         /* We give 1 as third argument to perform the first step*/
         static_PMyDNFCLockSectorAutomaton(
                     pContext,
                     pMyDConnection,
                     1,
                     W_SUCCESS);
         return;
      }
   }

   /* Send the result */
   static_PMyDSendResult( pContext, pMyDConnection, nError );
}

/**
 * Bytes have been written
 */
static void static_PMyDMoveWriteCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;

   PDebugTrace("static_PMyDWriteCompleted");

   if(nError == W_SUCCESS)
   {
      /* Check if we need to lock the bytes */
      if ( pMyDConnection->nLengthToLock != 0 )
      {
         uint32_t nBlockStart, nBlockEnd, nIndex;

         nBlockStart = pMyDConnection->pSectorSize->pDivide(pMyDConnection->nOffsetToLock);
         nBlockEnd   = pMyDConnection->pSectorSize->pDivide(pMyDConnection->nLengthToLock + pMyDConnection->nOffsetToLock - 1);

         for( nIndex = nBlockStart; nIndex <= nBlockEnd; nIndex ++ )
         {
            static_PMyDLockSector(pContext, pMyDConnection, nIndex);
         }

         pMyDConnection->nLengthToLock = 0;

         /* Write lock bytes */
         static_PMyDWriteLockBytes(pContext, pMyDConnection);

         return;
      }
   }

   /* Send the result */
   static_PMyDSendResult( pContext, pMyDConnection, nError );
}

/**
 * The Configuration byte has been written
 */
static void static_PMyDMoveWriteConfigurationCompleted(
      tContext * pContext,
      void     * pCallbackParameter,
      W_ERROR    nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*) pCallbackParameter;

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PMyDMoveWriteConfigurationCompleted : nError %d", nError);
      static_PMyDSendResult(pContext, pMyDConnection, nError);
      return;
   }

   /* Save the new config byte */
   pMyDConnection->uMyD.sMove.nMyDMoveConfigByte = pMyDConnection->uMyD.sMove.nPendingConfigByte;

   static_PMyDSendResult(pContext, pMyDConnection, W_SUCCESS);
}

/**
 * The Password have been written
 */
static void static_PMyDMoveSetPasswordCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            uint32_t nDataLength,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;

   PDebugTrace("static_PMyDMoveSetPasswordCompleted");

   /* Clear password in buffers */
   CMemoryFill(&pMyDConnection->aCommandBuffer[1], 0, P_MY_D_MOVE_PASSWORD_LENGTH);
   CMemoryFill(&pMyDConnection->aResponseBuffer[0], 0, P_MY_D_MOVE_PASSWORD_LENGTH);

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PMyDMoveSetPasswordCompleted : nError %d", nError);
      static_PMyDSendResult(pContext, pMyDConnection, nError);
      return;
   }

   /* If config is locked, status byte and retry counter are ignored */
   if ( (pMyDConnection->uMyD.sMove.nMyDMoveConfigByte & W_MY_D_MOVE_CONFIG_LOCKED) != 0 )
   {
      static_PMyDMoveWriteConfigurationCompleted(pContext, pMyDConnection, W_SUCCESS);
   }
   else
   {
      /* Else, write the configuration byte */
      PNDEF2GenWrite(pContext,
                     pMyDConnection->hConnection,
                     static_PMyDMoveWriteConfigurationCompleted,
                     pMyDConnection,
                     &pMyDConnection->uMyD.sMove.nPendingConfigByte,
                     pMyDConnection->pSectorSize->pMultiply(P_NDEF2GEN_STATIC_LOCK_BLOCK) + P_MY_D_MOVE_CONFIG_BYTE_OFFSET,
                     1,
                     null);
   }
}

/**
 * Authentication has been done
 */
static void static_PMyDMoveAuthenticateCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            uint32_t nDataLength,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;

   PDebugTrace("static_PMyDMoveSetPasswordCompleted");

   /* Clear password in buffer */
   CMemoryFill(&pMyDConnection->aCommandBuffer[1], 0, P_MY_D_MOVE_PASSWORD_LENGTH);

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PMyDMoveSetPasswordCompleted : nError %d", nError);
      pMyDConnection->uMyD.sMove.bAuthenticated = false;
      static_PMyDSendResult(pContext, pMyDConnection, nError);
      return;
   }

   /* Authenticated */
   pMyDConnection->uMyD.sMove.bAuthenticated = true;

   static_PMyDSendResult(pContext, pMyDConnection, W_SUCCESS);
}

/* Automaton used during connection creation */
static void static_PMyDCreateConnectionAutomaton(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;

   PDebugTrace("static_PMyDCreateConnectionAutomaton");

   if (W_SUCCESS != nError)
   {
      /* Send error */
      if (W_ERROR_CONNECTION_COMPATIBILITY != nError)
      {
         PDebugError("static_PMyDCreateConnectionAutomaton: returning %s", PUtilTraceError(nError));
      }
      PDFCPostContext2(&pMyDConnection->sCallbackContext, P_DFC_TYPE_MY_D, nError);

      if (W_ERROR_CONNECTION_COMPATIBILITY == nError)
      {
         /* Free pMyDConnection here since PHandleAddHeir has not been called yet
            thus PMyDRemoveConnection cannot free it */
         CMemoryFree(pMyDConnection);
      }
      return;
   }

   switch (pMyDConnection->nCurrentOperationState)
   {
      case 0:
         /* Add the My-d connection structure */
         if ( ( nError = PHandleAddHeir(
                           pContext,
                           pMyDConnection->hConnection,
                           pMyDConnection,
                           P_HANDLE_TYPE_MY_D_CONNECTION ) ) != W_SUCCESS )
         {
            PDebugError("static_PMyDCreateConnectionAutomaton: error returned by PHandleAddHeir()");
            PDFCPostContext2(&pMyDConnection->sCallbackContext, P_DFC_TYPE_MY_D, nError);
            /* Free pMyDConnection here since PHandleAddHeir has not been called yet
               thus PMyDRemoveConnection cannot free it */
            CMemoryFree(pMyDConnection);
            return;
         }

         /* Create the dynamic smart cache */
         if ( (nError = PNDEF2GenCreateSmartCacheDynamic(
                           pContext,
                           pMyDConnection->hConnection,
                           pMyDConnection->nSectorNumber)) != W_SUCCESS)
         {
            PDebugError("static_PMyDCreateConnectionAutomaton : PNDEF2GenCreateSmartCacheDynamic returned %s", PUtilTraceError(nError));
            PDFCPostContext2(&pMyDConnection->sCallbackContext, P_DFC_TYPE_MY_D, nError);
            return;
         }

         /* Read config byte and lock bytes 0-1 */
         PNDEF2GenRead(pContext,
                       pMyDConnection->hConnection,
                       static_PMyDCreateConnectionAutomaton,
                       pMyDConnection,
                       pMyDConnection->aResponseBuffer,
                       pMyDConnection->pSectorSize->pMultiply(P_NDEF2GEN_STATIC_LOCK_BLOCK),
                       P_NDEF2GEN_BLOCK_SIZE,
                       null);
         break;

      case 1:
         /* Retrieve lock bytes 0-1 */
         CMemoryCopy(&pMyDConnection->aLockBytes[0],
                     &pMyDConnection->aResponseBuffer[P_NDEF2GEN_STATIC_LOCK_BYTE_OFFSET],
                     P_NDEF2GEN_STATIC_LOCK_BYTE_LENGTH);

         if (W_PROP_MY_D_MOVE == pMyDConnection->nType)
         {
            /* Retrieve config byte */
            pMyDConnection->uMyD.sMove.nMyDMoveConfigByte = pMyDConnection->aResponseBuffer[P_MY_D_MOVE_CONFIG_BYTE_OFFSET];

            /* Read lock bytes 2-5 */
            PNDEF2GenRead(pContext,
                          pMyDConnection->hConnection,
                          static_PMyDCreateConnectionAutomaton,
                          pMyDConnection,
                          &pMyDConnection->aLockBytes[P_NDEF2GEN_STATIC_LOCK_BYTE_LENGTH],
                          pMyDConnection->pSectorSize->pMultiply(P_MY_D_MOVE_LOCK_BLOCK),
                          P_MY_D_MOVE_LOCK_LENGTH,
                          null);
            break;
         }
         else
         {
            if(W_PROP_MY_D_NFC == pMyDConnection->nType)
            {
               pMyDConnection->uMyD.sNFC.pSmartCacheDynamic = (tSmartCache *) CMemoryAlloc(sizeof(tSmartCache));

               if(pMyDConnection->uMyD.sNFC.pSmartCacheDynamic == null)
               {
                  PDebugError("PNDEF2GenCreateSmartCacheDynamic: not enough memory for smartcache");
                  PDFCPostContext2(&pMyDConnection->sCallbackContext, P_DFC_TYPE_MY_D, nError);

                  PHandleRemoveLastHeir(pContext, pMyDConnection->hConnection, P_HANDLE_TYPE_MY_D_CONNECTION);
                  CMemoryFree(pMyDConnection);
                  return;

               }

               /* Create the smart cache for the dynamic area */
               if((nError = PSmartCacheCreateCache(
                           pContext,
                           pMyDConnection->uMyD.sNFC.pSmartCacheDynamic,
                           pMyDConnection->nSectorNumber,
                           &g_sDescriptorMyDNFCDynamic,
                           pMyDConnection)) != W_SUCCESS)
               {
                  PDebugError("PNDEF2GenCreateSmartCacheDynamic: error creating the smart cache");
                  PDFCPostContext2(&pMyDConnection->sCallbackContext, P_DFC_TYPE_MY_D, nError);

                  PHandleRemoveLastHeir(pContext, pMyDConnection->hConnection, P_HANDLE_TYPE_MY_D_CONNECTION);
                  CMemoryFree(pMyDConnection->uMyD.sNFC.pSmartCacheDynamic);
                  pMyDConnection->uMyD.sNFC.pSmartCacheDynamic = null;

                  CMemoryFree(pMyDConnection);
                  return;
               }
            }
            pMyDConnection->nCurrentOperationState++;
            /* No break. For My-d NFC, go directly to the next step */
         }

      case 2:
         /* All bytes have been read */
         pMyDConnection->bLockBytesRetrieved = true;
         PDFCPostContext2(&pMyDConnection->sCallbackContext, P_DFC_TYPE_MY_D, W_SUCCESS);
         break;
   }

   pMyDConnection->nCurrentOperationState++;
}

/**
 * The My-d NFC manufacturer bytes have been read
 */
static void static_PMyDNFCReadManufacturerCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            uint32_t nDataLength,
            W_ERROR nError)
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)pCallbackParameter;

   PDebugTrace("static_PMyDNFCReadManufacturerCompleted");

   if (W_SUCCESS == nError)
   {
      /* Retrieve the My-d NFC type and size from the manufacturer CI byte */
      switch (pMyDConnection->aResponseBuffer[0] & 0x07)
      {
      case 0x04:
         /* SLE 66R16P */
         pMyDConnection->nSectorNumber = 256;   /* 256 blocks = 1024 bytes */
         break;

      case 0x05:
         /* SLE 66R32P */
         pMyDConnection->nSectorNumber = 512;   /* 512 blocks = 2048 bytes */
         break;

      default:
         PDebugLog("static_PMyDNFCReadManufacturerCompleted: unsupported My-d NFC card");
         nError = W_ERROR_CONNECTION_COMPATIBILITY;
         break;
      }
   }

   /* Continue creation */
   static_PMyDCreateConnectionAutomaton(pContext, pCallbackParameter, nError);
}



/* See Header file */
void PMyDCreateConnection(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProperty )
{
   static const uint8_t pRead[] = { 0x10, 0x02, 0x00 };
   tMyDConnection* pMyDConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMyDCreateConnection");

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Create the NDEF buffer */
   pMyDConnection = (tMyDConnection*)CMemoryAlloc( sizeof(tMyDConnection) );
   if ( pMyDConnection == null )
   {
      PDebugError("PMyDCreateConnection: pMyDConnection == null");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }
   CMemoryFill(pMyDConnection, 0, sizeof(tMyDConnection));

   /* Get the 14443-3 information level */
   if ( ( nError = P14P3UserCheckMyD(
                     pContext,
                     hConnection,
                     pMyDConnection->UID,
                     &pMyDConnection->nUIDLength,
                     &pMyDConnection->nType ) ) != W_SUCCESS )
   {
      PDebugLog("PMyDCreateConnection: not a My-d card");
      goto return_error;
   }

   PDebugTrace("PMyDCreateConnection: detection of the type %s",
      PUtilTraceConnectionProperty(pMyDConnection->nType));

   /* Store the connection information */
   pMyDConnection->hConnection = hConnection;

   /* Store the callback context */
   pMyDConnection->sCallbackContext = sCallbackContext;

   /* Begining of automaton */
   pMyDConnection->nCurrentOperationState = 0;

   pMyDConnection->pSectorSize = &g_sSectorSize4;

   switch ( pMyDConnection->nType )
   {
   case W_PROP_MY_D_MOVE:
      /* My-d move size is known */
      pMyDConnection->nSectorNumber = P_MY_D_MOVE_BLOCK_NUMBER;
      static_PMyDCreateConnectionAutomaton(pContext, pMyDConnection, W_SUCCESS);
      break;

   case W_PROP_MY_D_NFC:
      /* Read My-d NFC size in Manufacturer CI byte */
      P14P3UserExchangeData(
         pContext,
         pMyDConnection->hConnection,
         static_PMyDNFCReadManufacturerCompleted, pMyDConnection,
         pRead, sizeof(pRead),
         pMyDConnection->aResponseBuffer, P_MY_D_NFC_PAGE_SIZE,
         W_NULL_HANDLE,
         true);


      break;

   default:
      PDebugError("PMyDCreateConnection: unknown type");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   return;

return_error:

   if(pMyDConnection != null)
   {
      CMemoryFree(pMyDConnection);
   }

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MY_D, nError);
}

/** See tPReaderUserRemoveSecondaryConnection */
void PMyDRemoveConnection(
            tContext* pContext,
            W_HANDLE hUserConnection )
{
   tMyDConnection* pMyDConnection = (tMyDConnection*)PHandleRemoveLastHeir(
            pContext, hUserConnection,
            P_HANDLE_TYPE_MY_D_CONNECTION);

   PDebugTrace("PMyDRemoveConnection");

   /* Remove the connection object */
   if(pMyDConnection != null)
   {
      CMemoryFree(pMyDConnection);
   }
}

/* See Header file */
W_ERROR PMyDCheckType2(
            tContext* pContext,
            W_HANDLE hConnection,
            uint32_t* pnMaximumSpaceSize,
            uint8_t* pnSectorSize,
            bool* pbIsLocked,
            bool* pbIsLockable,
            bool* pbIsFormattable,
            uint8_t* pSerialNumber,
            uint8_t* pnSerialNumberLength )
{
   tMyDConnection* pMyDConnection;
   uint32_t nIndex;
   W_ERROR nError;
   bool    bIsLocked = false;
   bool    bIsLockable = true;
   uint32_t nLastBlockIndex;

   PDebugTrace("PMyDCheckType2");

   /* Reset the maximum tag size */
   if (pnMaximumSpaceSize != null) *pnMaximumSpaceSize = 0;

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if ( nError == W_SUCCESS )
   {
      /* Maximum size */
      switch ( pMyDConnection->nType )
      {
      case W_PROP_MY_D_MOVE:
         nLastBlockIndex = P_MY_D_MOVE_LAST_DATA_BLOCK;
         break;
      case W_PROP_MY_D_NFC:
         nLastBlockIndex = P_MY_D_NFC_LAST_DATA_BLOCK;
         break;
      default:
         /* other My-d cards are not supported */
         PDebugError("PMyDCheckType2: Unknown type");
         return W_ERROR_CONNECTION_COMPATIBILITY;
      }

      if (pnMaximumSpaceSize != null)
      {
         *pnMaximumSpaceSize = pMyDConnection->pSectorSize->pMultiply(nLastBlockIndex - P_MY_D_FIRST_DATA_BLOCK + 1);
      }

      /* Go through the lock byte */
      for (nIndex=4; nIndex <= nLastBlockIndex; nIndex++)
      {
         bIsLocked |= static_PMyDIsSectorLocked(pContext, pMyDConnection, nIndex);
         bIsLockable &= static_PMyDIsSectorLockable(pContext, pMyDConnection, nIndex);
      }

      if (pbIsLocked != null) *pbIsLocked = bIsLocked;
      if (pbIsLockable != null) *pbIsLockable = bIsLockable;
      if (pnSectorSize != null) *pnSectorSize = (uint8_t)pMyDConnection->pSectorSize->nValue;
      if (pbIsFormattable != null) *pbIsFormattable = ! bIsLocked;

      if (pnSerialNumberLength != null) *pnSerialNumberLength = pMyDConnection->nUIDLength;
      if (pSerialNumber != null) CMemoryCopy(pSerialNumber, pMyDConnection->UID, pMyDConnection->nUIDLength );

      return W_SUCCESS;
   }

   return nError;
}

/* See header file */
W_ERROR PMyDNDEF2Lock(tContext * pContext,
                      W_HANDLE hConnection)
{
   tMyDConnection* pMyDConnection = null;
   W_ERROR nError;

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMyDNDEF2Lock : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      return nError;
   }

   /* Update lock bytes */
   CMemoryFill(&pMyDConnection->aLockBytes[0], 0xFF, 2);
   if (W_PROP_MY_D_MOVE == pMyDConnection->nType)
   {
      CMemoryFill(&pMyDConnection->aLockBytes[2], 0xFF, 2);
      CMemoryFill(&pMyDConnection->aLockBytes[4], 0x0F, 2);
   }

   return W_SUCCESS;
}

/* See Client API Specifications */
W_ERROR PMyDGetConnectionInfo(
            tContext* pContext,
            W_HANDLE hConnection,
            tWMyDConnectionInfo *pConnectionInfo )
{
   tMyDConnection* pMyDConnection;
   W_ERROR nError;

   PDebugTrace("PMyDGetConnectionInfo");

   /* Check the parameters */
   if ( pConnectionInfo == null )
   {
      PDebugError("PMyDGetConnectionInfo: pConnectionInfo == null");
      return W_ERROR_BAD_PARAMETER;
   }

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if ( nError == W_SUCCESS )
   {
      /* UID */
      CMemoryCopy(
         pConnectionInfo->UID,
         pMyDConnection->UID,
         pMyDConnection->nUIDLength );
      /* Sector size */
      pConnectionInfo->nSectorSize = P_MY_D_BLOCK_SIZE;
      /* Sector number */
      pConnectionInfo->nSectorNumber = (uint16_t)pMyDConnection->nSectorNumber;
      return W_SUCCESS;
   }
   else
   {
      PDebugError("PMyDGetConnectionInfo: could not get pMyDConnection buffer");

      /* Fill in the structure with zeros */
      CMemoryFill(pConnectionInfo, 0, sizeof(tWMifareConnectionInfo));

      return nError;
   }
}

/* See tPBasicGenericDataCallback */
static void static_PMyDNFCSmartCacheActionCompleted(tContext* pContext, void * pCallbackParameter, uint32_t nDataLength, W_ERROR nResult)
{
   /* Just make the wrapping for SmartCache Read/Write function */
   tMyDConnection * pMyDConnection = (tMyDConnection*) pCallbackParameter;

   PDFCPostContext2(&(pMyDConnection->uMyD.sNFC.sCacheCallbackContext), P_DFC_TYPE_MY_D, nResult);
}

W_ERROR PMyDNFCInvalidateSmartCache(
                  tContext * pContext,
                  W_HANDLE hConnection,
                  uint32_t nOffset,
                  uint32_t nLength)
{
   /* Just make the wrapping for SmartCache Read/Write function */
   tMyDConnection * pMyDConnection = null;
   W_ERROR nError;

   PDebugTrace("pMyDNFCInvalidateSmartCache");

   /* Check if the connection handle is valid */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if ( nError != W_SUCCESS )
   {
      PDebugError("pMyDNFCInvalidateSmartCache: Bad handle");
      return nError;
   }

   PSmartCacheInvalidateCache(
         pContext, 
         pMyDConnection->uMyD.sNFC.pSmartCacheDynamic,
         nOffset,
         nLength);

   return W_SUCCESS;
}

static void static_PMyDNFCReadDynamic(
               tContext* pContext,
               void* pConnection,
               uint32_t nSectorOffset,
               uint32_t nSectorNumber,
               uint8_t* pBuffer,
               tPBasicGenericCallbackFunction* pCallback,
               void* pCallbackParameter)
{
   tMyDConnection* pMyDConnection = (tMyDConnection *) pConnection;

   uint8_t nIndex = 0;

   PDebugTrace("static_PMyDNFCReadDynamic");
   CDebugAssert(nSectorNumber == 1);

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &(pMyDConnection->uMyD.sNFC.sCacheCallbackContext) );

   if( ( nSectorNumber < 1 ) 
      ||( nSectorOffset >= pMyDConnection->nSectorNumber)) 
   {
      PDebugTrace("Error on static_PMyDNFCReadDynamic : W_ERROR_BAD_PARAMETERS => \n\
                     nSectorNumber = %d \n\
                     nSectorOffset = %d \n\
                     pMyDConnection->nSectorNumber = %d",nSectorNumber, nSectorOffset, pMyDConnection->nSectorNumber );

      PDFCPostContext2(&(pMyDConnection->uMyD.sNFC.sCacheCallbackContext), P_DFC_TYPE_MY_D,W_ERROR_BAD_PARAMETER);
      return;
   }

   pMyDConnection->aCommandBuffer[nIndex++] = P_MY_D_NFC_CMD_READ;
   pMyDConnection->aCommandBuffer[nIndex++] = (uint8_t) (nSectorOffset & 0x000000FF);
   pMyDConnection->aCommandBuffer[nIndex++] = (uint8_t) ((nSectorOffset & 0x0000FF00) >> 8);

   P14P3UserExchangeData(
      pContext,
      pMyDConnection->hConnection,
      static_PMyDNFCSmartCacheActionCompleted,
      pMyDConnection,
      pMyDConnection->aCommandBuffer,
      nIndex,
      pBuffer,
      g_sSectorSize8.nValue,
      null,
      true);
}

static void static_PMyDMoveRead(
               tContext* pContext,
               tMyDConnection* pMyDConnection,
               uint8_t *pBuffer,
               uint32_t nOffset,
               uint32_t nLength,
               tPBasicGenericCallbackFunction* pCallback,
               void* pCallbackParameter,
               W_HANDLE *phOperation)
{
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   if ( (nOffset + nLength) > pMyDConnection->pSectorSize->pMultiply(pMyDConnection->nSectorNumber) )
   {
      PDebugError("PMyDRead: the data to read/write is too large");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

    /* For My-d move, check if authentication is required and done or not */
   if ( ((nOffset + nLength) > pMyDConnection->pSectorSize->pMultiply(P_MY_D_MOVE_PROTECTED_FIRST_BLOCK)) &&
        ((pMyDConnection->uMyD.sMove.nMyDMoveConfigByte & W_MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED) != 0) &&
        (false == pMyDConnection->uMyD.sMove.bAuthenticated) )
   {
      PDebugError("PMyDRead: authentication is required");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Store the connection information */
   pMyDConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMyDConnection->bOperationPending = true;

   /* Increment the reference count to keep the connection object alive during
      the operation. The reference count is decreased in static_PMyDSendResult()
      when the operation is completed */
   PHandleIncrementReferenceCount(pMyDConnection);

   PNDEF2GenRead(pContext,
                 pMyDConnection->hConnection,
                 static_PMyDReadCompleted,
                 pMyDConnection,
                 pBuffer,
                 nOffset,
                 nLength,
                 phOperation);

   return;

return_error:

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MY_D, nError);

}

static void static_PMyDNFCRead(
               tContext* pContext,
               tMyDConnection* pMyDConnection,
               uint8_t *pBuffer,
               uint32_t nOffset,
               uint32_t nLength,
               tPBasicGenericCallbackFunction* pCallback,
               void* pCallbackParameter,
               W_HANDLE *phOperation)
{

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &pMyDConnection->sCallbackContext );

   /* set the operation in progress flag to avoid reentrancy */
   pMyDConnection->bOperationPending = true;

   /* Increment the reference count to keep the connection object alive during
      the operation. The reference count is decreased in static_PMyDSendResult()
      when the operation is completed */
   PHandleIncrementReferenceCount(pMyDConnection);

   PSmartCacheRead(
            pContext,
            pMyDConnection->uMyD.sNFC.pSmartCacheDynamic,
            nOffset, nLength,
            pBuffer,
            static_PMyDReadCompleted,
            pMyDConnection);
}


/* See Client API Specifications */
void PMyDRead(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void *pCallbackParameter,
            uint8_t *pBuffer,
            uint32_t nOffset,
            uint32_t nLength,
            W_HANDLE *phOperation )
{
   tMyDConnection* pMyDConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMyDRead");

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Check if the connection handle is valid */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if ( nError != W_SUCCESS )
   {
      PDebugError("PMyDRead: Bad handle");
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMyDConnection->bOperationPending != false)
   {
      PDebugError("PMyDRead: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check parameters */
   if ( (null == pBuffer) || (0 == nLength) )
   {
      PDebugError("PMyDRead: null parameter");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   switch(pMyDConnection->nType)
   {
      case W_PROP_MY_D_MOVE:
         static_PMyDMoveRead(
            pContext,
            pMyDConnection,
            pBuffer,
            nOffset,
            nLength,
            pCallback,
            pCallbackParameter,
            phOperation);
         break;

      case W_PROP_MY_D_NFC:
         static_PMyDNFCRead(
            pContext,
            pMyDConnection,
            pBuffer,
            nOffset,
            nLength,
            pCallback,
            pCallbackParameter,
            phOperation);
         break;

      default:
         nError = W_ERROR_FEATURE_NOT_SUPPORTED;
         goto return_error;
   }
   return;

return_error:

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MY_D, nError);
}



/* See tSmartCacheWriteSector */
static void static_PMyDNFCWriteDynamic(
               tContext* pContext,
               void* pConnection,
               uint32_t nSectorOffset,
               uint32_t nSectorNumber,
               const uint8_t* pBuffer,
               tPBasicGenericCallbackFunction* pCallback,
               void* pCallbackParameter)
{
   tMyDConnection* pMyDConnection = (tMyDConnection *) pConnection;

   uint8_t nIndex = 0;

   PDebugTrace("static_PMyDNFCWriteDynamic");

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &(pMyDConnection->uMyD.sNFC.sCacheCallbackContext) );

   if( ( nSectorNumber > 1 ) 
      ||( nSectorOffset >= pMyDConnection->nSectorNumber)) 
   {
      PDebugTrace("Error on static_PMyDNFCWriteDynamic : W_ERROR_BAD_PARAMETERS => \n\
                     nSectorNumber = %d \n\
                     nSectorOffset = %d \n\
                     pMyDConnection->nSectorNumber = %d",nSectorNumber, nSectorOffset, pMyDConnection->nSectorNumber );

      PDFCPostContext2(&(pMyDConnection->uMyD.sNFC.sCacheCallbackContext), P_DFC_TYPE_MY_D, W_ERROR_BAD_PARAMETER);
      return;
   }

   pMyDConnection->aCommandBuffer[nIndex++] = P_MY_D_NFC_CMD_WRITE;
   pMyDConnection->aCommandBuffer[nIndex++] = (uint8_t) (nSectorOffset & 0x000000FF);
   pMyDConnection->aCommandBuffer[nIndex++] = (uint8_t) ((nSectorOffset & 0x0000FF00) >> 8);

   CMemoryCopy(&pMyDConnection->aCommandBuffer[nIndex], pBuffer, 8);
   nIndex += 8;


   P14P3UserExchangeData(
      pContext,
      pMyDConnection->hConnection,
      static_PMyDNFCSmartCacheActionCompleted,
      pMyDConnection,
      pMyDConnection->aCommandBuffer,
      nIndex,
      pMyDConnection->aResponseBuffer,
      sizeof(pMyDConnection->aResponseBuffer),
      null,
      false);
}



static void  static_PMyDNFCWrite(
            tContext * pContext,
            tMyDConnection * pMyDConnection,
            const uint8_t* pBuffer,
            uint32_t nOffset,
            uint32_t nLength,
            bool bLockSectors,
            W_HANDLE *phOperation )
{
   /** MyD NFC */
   PDebugTrace("static_PMyDNFCWrite");

   pMyDConnection->bOperationPending = true;

   /* Increment the reference count to keep the connection object alive during
      the operation. The reference count is decreased in static_PMyDSendResult()
      when the operation is completed */
   PHandleIncrementReferenceCount(pMyDConnection);

   if(bLockSectors == true)
   {
      pMyDConnection->bLockBytesRetrieved = true;

      if(nLength == 0 && nOffset == 0)
      {
         pMyDConnection->nOffsetToLock=0;
         pMyDConnection->nLengthToLock = pMyDConnection->nSectorNumber;
      }
      else
      {
         uint32_t tmp = 0;

         /* search the first offset to lock*/
         pMyDConnection->nOffsetToLock = g_sSectorSize8.pDivide(nOffset);

         /* search the number of sector to lock */
         tmp = g_sSectorSize8.pMultiply(pMyDConnection->nOffsetToLock);
         tmp = tmp - (nOffset + nLength);

         pMyDConnection->nLengthToLock = g_sSectorSize8.pDivide(tmp) + 1;
      }

      if(pBuffer == null)
      {

         /* You must write nothing but just locking */
         static_PMyDNFCLockSectorAutomaton(pContext, pMyDConnection, 1, W_SUCCESS);
         return;
      }

   }
   else
   {
      pMyDConnection->bLockBytesRetrieved = false;
   }


   PSmartCacheWrite(
            pContext,
            pMyDConnection->uMyD.sNFC.pSmartCacheDynamic,
            nOffset, nLength,
            pBuffer,
            static_PMyDNFCWriteCompleted,
            pMyDConnection);

   PNDEF2GenInvalidateCache(pContext, pMyDConnection->hConnection, nOffset, nLength);
}


/* Specific process for myD Move Writting */
static void static_PMyDMoveWrite(
            tContext * pContext,
            tMyDConnection * pMyDConnection,
            const uint8_t* pBuffer,
            uint32_t nOffset,
            uint32_t nLength,
            bool bLockSectors,
            W_HANDLE *phOperation )
{
   W_ERROR nError;
   uint32_t nIndex;

   if ( (pMyDConnection->pSectorSize->pDivide(nOffset) == P_MY_D_MOVE_LOCK_BLOCK) ||
           (pMyDConnection->pSectorSize->pDivide(nOffset + nLength) == P_MY_D_MOVE_LOCK_BLOCK) )
   {
      PDebugError("PMyDWrite: writing of lock bytes is not allowed. Use the specialized API for locking !");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ((pBuffer == null) && (nOffset == 0) && (nLength == 0))
   {
      nOffset =  pMyDConnection->pSectorSize->pMultiply(P_MY_D_FIRST_DATA_BLOCK);
      nLength =  pMyDConnection->pSectorSize->pMultiply(P_MY_D_MOVE_LAST_DATA_BLOCK - P_MY_D_FIRST_DATA_BLOCK + 1);
   }

   /* For My-d move, check if authentication is required and done or not */
   if ( ((nOffset + nLength) > pMyDConnection->pSectorSize->pMultiply(P_MY_D_MOVE_PROTECTED_FIRST_BLOCK)) &&
        ((pMyDConnection->uMyD.sMove.nMyDMoveConfigByte & W_MY_D_MOVE_WRITE_PASSWORD_ENABLED) != 0) &&
        (false == pMyDConnection->uMyD.sMove.bAuthenticated) )
   {
      PDebugError("PMyDWrite: authentication is required");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check if the My-d card is locked/lockable */
   for (nIndex = pMyDConnection->pSectorSize->pDivide(nOffset);
            nIndex <= pMyDConnection->pSectorSize->pDivide(nOffset + nLength - 1); nIndex++)
   {
      if ((pBuffer != null) && (static_PMyDIsSectorLocked(pContext, pMyDConnection, nIndex) != false))
      {
         PDebugError("PMyDWrite: item locked");
         nError = W_ERROR_ITEM_LOCKED;
         goto return_error;
      }

      if ((bLockSectors != false) && (static_PMyDIsSectorLockable(pContext, pMyDConnection, nIndex) != true))
      {
         PDebugError("PMyDWrite: item not lockable");
         nError = W_ERROR_ITEM_LOCKED;
         goto return_error;
      }
   }

   /* Store the exchange information */
   if(bLockSectors != false)
   {
      pMyDConnection->nOffsetToLock = nOffset;
      pMyDConnection->nLengthToLock = nLength;
   }
   else
   {
      pMyDConnection->nOffsetToLock = 0;
      pMyDConnection->nLengthToLock = 0;
   }

   /* set the operation in progress flag to avoid reentrancy */
   pMyDConnection->bOperationPending = true;

   /* Increment the reference count to keep the connection object alive during
      the operation. The reference count is decreased in static_PMyDSendResult()
      when the operation is completed */
   PHandleIncrementReferenceCount(pMyDConnection);

   if (pBuffer != null)
   {
      PNDEF2GenWrite(pContext,
                     pMyDConnection->hConnection,
                     static_PMyDMoveWriteCompleted,
                     pMyDConnection,
                     pBuffer,
                     nOffset,
                     nLength,
                     phOperation);
   }
   else
   {
      static_PMyDMoveWriteCompleted(pContext, pMyDConnection, W_SUCCESS);
   }
   return;

return_error:
   PDFCPostContext2(&(pMyDConnection->sCallbackContext), P_DFC_TYPE_MY_D, nError);
}

/* See Client API Specifications */
void PMyDWrite(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction *pCallback,
            void* pCallbackParameter,
            const uint8_t* pBuffer,
            uint32_t nOffset,
            uint32_t nLength,
            bool bLockSectors,
            W_HANDLE *phOperation )
{
   tMyDConnection* pMyDConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;
   

   PDebugTrace("PMyDWrite");

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Check if the connection handle is valid */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if ( nError != W_SUCCESS )
   {
      PDebugError("PMyDWrite: Bad handle");
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMyDConnection->bOperationPending != false)
   {
      PDebugError("PMyDWrite: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check the parameters */
   if ((pBuffer == null) && (bLockSectors == false))
   {
      /* pBuffer null is only allowed for lock */
      PDebugError("PMyDWrite: W_ERROR_BAD_PARAMETER");

      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ((nLength == 0) && ((pBuffer != null) || (nOffset != 0) || (bLockSectors == false)))
   {
      /* nLength == 0 is only valid for whole tag lock */
      PDebugError("PMyDWrite: W_ERROR_BAD_PARAMETER");

      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ( (nOffset + nLength) > pMyDConnection->pSectorSize->pMultiply(pMyDConnection->nSectorNumber) )
   {
      PDebugError("PMyDWrite: the data to read/write is too large");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ( (nLength > 0) && (nOffset < pMyDConnection->pSectorSize->pMultiply(P_MY_D_FIRST_DATA_BLOCK)) )
   {
      PDebugError("PMyDWrite: writing of Serial Number or Lock bytes is not allowed. Use the specialized API for locking !");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

    /* Store the connection information */
   pMyDConnection->sCallbackContext = sCallbackContext;
   
   switch(pMyDConnection->nType)
   {
      case W_PROP_MY_D_MOVE:
         static_PMyDMoveWrite(pContext,
                              pMyDConnection,
                              pBuffer,
                              nOffset,
                              nLength,
                              bLockSectors,
                              phOperation);
         break;

      case W_PROP_MY_D_NFC:
         static_PMyDNFCWrite(pContext,
                              pMyDConnection,
                              pBuffer,
                              nOffset,
                              nLength,
                              bLockSectors,
                              phOperation);
         break;
      default:
         nError = W_ERROR_FEATURE_NOT_SUPPORTED;
         goto return_error;
   }

   return;

return_error:

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MY_D, nError);
}

/* ------------------------------------------------------ */

/* See client API */
void PMyDMoveFreezeDataLockConfiguration(
      tContext * pContext,
      W_HANDLE hConnection,
      tPBasicGenericCallbackFunction * pCallback,
      void * pCallbackParameter)
{
   tMyDConnection* pMyDConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMyDMoveFreezeDataLockConfiguration");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMyDMoveFreezeDataLockConfiguration : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   if (pMyDConnection->nType != W_PROP_MY_D_MOVE)
   {
      /* Not a My-d move connection */
      PDebugError("PMyDMoveFreezeDataLockConfiguration : not a My-d move");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMyDConnection->bOperationPending != false)
   {
      PDebugError("PMyDMoveFreezeDataLockConfiguration: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* store the callback context */
   pMyDConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMyDConnection->bOperationPending = true;

   /* increment the ref count to avoid prematurely freeing of the connection */
   PHandleIncrementReferenceCount(pMyDConnection);

   /* LOCK byte # 0 : Set the bits 1 and 2 that lock the other lock bits */
   pMyDConnection->aLockBytes[0] |= 0x06;
   /* LOCK byte # 5 : Set the bits 0-3 that lock the other lock bits */
   pMyDConnection->aLockBytes[5] |= 0x0F;

   /* Write lock bytes */
   static_PMyDWriteLockBytes(pContext, pMyDConnection);

   return;

return_error :

   /* Send the error */
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MY_D, nError);
}

/* ------------------------------------------------------ */

/* See client API */
void PMyDMoveGetConfiguration (
      tContext * pContext,
      W_HANDLE hConnection,
      tPMyDMoveGetConfigurationCompleted * pCallback,
      void * pCallbackParameter)
{
   tMyDConnection* pMyDConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;
   uint8_t nStatusByte = 0;
   uint8_t nPasswordRetryCounter = 0;

   PDebugTrace("PMyDMoveGetConfiguration");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMyDMoveGetConfiguration : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   /* Only My-d move are supported */
   if (pMyDConnection->nType != W_PROP_MY_D_MOVE)
   {
      PDebugError("PMyDMoveGetConfiguration : not a My-d move");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }   

   /* Return parameters */
   nStatusByte = (pMyDConnection->uMyD.sMove.nMyDMoveConfigByte & P_MY_D_MOVE_STATUS_BYTE_MASK);
   nPasswordRetryCounter = ((pMyDConnection->uMyD.sMove.nMyDMoveConfigByte >> 4) & 0x07);

   PDFCPostContext4(&sCallbackContext, P_DFC_TYPE_MY_D, W_SUCCESS, nStatusByte, nPasswordRetryCounter);

   return;

return_error:

   /* Send the error */
   PDFCPostContext4(&sCallbackContext, P_DFC_TYPE_MY_D, nError, 0, 0);
}

/* ------------------------------------------------------ */

/* See client API */
void PMyDMoveSetConfiguration (
      tContext * pContext,
      W_HANDLE hConnection,
      tPBasicGenericCallbackFunction *pCallback,
      void *pCallbackParameter,
      uint8_t nStatusByte,
      uint8_t nPasswordRetryCounter,
      uint32_t nPassword,
      bool bLockConfiguration)
{
   tMyDConnection* pMyDConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMyDMoveSetConfiguration");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMyDMoveSetConfiguration : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   /* Only My-d move are supported */
   if (pMyDConnection->nType != W_PROP_MY_D_MOVE)
   {
      PDebugError("PMyDMoveSetConfiguration : not a My-d move connection");
      nError =  W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMyDConnection->bOperationPending != false)
   {
      PDebugError("PMyDMoveSetConfiguration: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check if authentication is required and done or not */
   if ( (((pMyDConnection->uMyD.sMove.nMyDMoveConfigByte & W_MY_D_MOVE_WRITE_PASSWORD_ENABLED) != 0) || 
         ((pMyDConnection->uMyD.sMove.nMyDMoveConfigByte & W_MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED) != 0)) &&      
         (false == pMyDConnection->uMyD.sMove.bAuthenticated) )
   {
      PDebugError("PMyDWrite: authentication is required to set password");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* check parameters */
   if (nPasswordRetryCounter > P_MY_D_MOVE_MAX_RETRY_COUNTER)
   {
      PDebugError("PMyDMoveSetConfiguration: wrong nPasswordRetryCounter");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* If config is locked, status byte and retry counter are ignored */
   if ( (pMyDConnection->uMyD.sMove.nMyDMoveConfigByte & W_MY_D_MOVE_CONFIG_LOCKED) == 0 )
   {
      /* RFU and PCN bytes must be 0 */
      /* READ without WRITE password protection does not exist */
      /* Config bits are one time programmable and cannot be reset to 0 */
      if ( ((nStatusByte & P_MY_D_MOVE_STATUS_BYTE_MASK) != nStatusByte) ||
           (((nStatusByte | pMyDConnection->uMyD.sMove.nMyDMoveConfigByte) & P_MY_D_MOVE_STATUS_BYTE_MASK) != nStatusByte))
      {
         PDebugError("PMyDMoveSetConfiguration: wrong nStatusByte");
         nError = W_ERROR_BAD_PARAMETER;
         goto return_error;
      }

      /* New value of config byte to write after password */
      pMyDConnection->uMyD.sMove.nPendingConfigByte = (nStatusByte | (nPasswordRetryCounter << 4));
   }

   /* store the callback context */
   pMyDConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMyDConnection->bOperationPending = true;

   /* increment the ref count to avoid prematurely freeing of the connection */
   PHandleIncrementReferenceCount(pMyDConnection);

   /* Set My-d Move password */
   pMyDConnection->aCommandBuffer[0] = 0xB1;
   pMyDConnection->aCommandBuffer[1] = (uint8_t)(nPassword);
   pMyDConnection->aCommandBuffer[2] = (uint8_t)(nPassword >> 8);
   pMyDConnection->aCommandBuffer[3] = (uint8_t)(nPassword >> 16);
   pMyDConnection->aCommandBuffer[4] = (uint8_t)(nPassword >> 24);

   P14P3UserExchangeData(
      pContext,
      pMyDConnection->hConnection,
      static_PMyDMoveSetPasswordCompleted, pMyDConnection,
      pMyDConnection->aCommandBuffer, 1 + P_MY_D_MOVE_PASSWORD_LENGTH,
      pMyDConnection->aResponseBuffer, P_MY_D_MOVE_PASSWORD_LENGTH,
      W_NULL_HANDLE,
      true);

   return;

return_error:

   /* Send the error */
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MY_D, nError);
}

/* ------------------------------------------------------ */

/* See Client API */
void PMyDMoveAuthenticate (
      tContext * pContext,
      W_HANDLE hConnection,
      tPBasicGenericCallbackFunction *pCallback,
      void *pCallbackParameter,
      uint32_t nPassword)
{
   tMyDConnection* pMyDConnection;
   tDFCCallbackContext sCallbackContext;
   W_ERROR             nError;

   PDebugTrace("PMyDMoveAuthenticate");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MY_D_CONNECTION, (void**)&pMyDConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMyDMoveAuthenticate : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   /* Only My-d move are supported */
   if (pMyDConnection->nType != W_PROP_MY_D_MOVE)
   {
      PDebugError("PMyDMoveAuthenticate : not a My-d move connection");
      nError =  W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if ( pMyDConnection->bOperationPending != false )
   {
      PDebugError("PMyDMoveAuthenticate: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* store the callback context */
   pMyDConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMyDConnection->bOperationPending = true;

   /* increment the ref count to avoid prematurely freeing of the connection */
   PHandleIncrementReferenceCount(pMyDConnection);

   /* Send ACCESS command */
   pMyDConnection->aCommandBuffer[0] = 0xB2;
   pMyDConnection->aCommandBuffer[1] = (uint8_t)(nPassword);
   pMyDConnection->aCommandBuffer[2] = (uint8_t)(nPassword >> 8);
   pMyDConnection->aCommandBuffer[3] = (uint8_t)(nPassword >> 16);
   pMyDConnection->aCommandBuffer[4] = (uint8_t)(nPassword >> 24);

   P14P3UserExchangeData(
      pContext,
      pMyDConnection->hConnection,
      static_PMyDMoveAuthenticateCompleted, pMyDConnection,
      pMyDConnection->aCommandBuffer, 1 + P_MY_D_MOVE_PASSWORD_LENGTH,
      null, 0, /* No data in response */
      W_NULL_HANDLE,
      false);  /* No CRC in response */

   return;

return_error:

   /* Send the error */
   PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_MY_D, nError );
}

/* ------------------------------------------------------ */

/* See Client API Specifications */
W_ERROR WMyDReadSync(
                  W_HANDLE hConnection,
                  uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMyDReadSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMyDRead(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            pBuffer, nOffset, nLength,
            null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See Client API Specifications */
W_ERROR WMyDWriteSync(
                  W_HANDLE hConnection,
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength,
                  bool bLockSectors)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMyDWriteSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMyDWrite(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            pBuffer, nOffset, nLength, bLockSectors,
            null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See Client API Specifications */
W_ERROR WMyDMoveFreezeDataLockConfigurationSync(
                  W_HANDLE hConnection)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMyDMoveFreezeDataLockConfigurationSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMyDMoveFreezeDataLockConfiguration(
            hConnection,
            PBasicGenericSyncCompletion,
            &param );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See Client API Specifications */
W_ERROR WMyDMoveGetConfigurationSync(
                  W_HANDLE hConnection,
                  uint8_t * pnStatusByte,
                  uint8_t * pnPasswordRetryCounter)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMyDMoveGetConfigurationSync");

   if ((pnStatusByte == null) || (pnPasswordRetryCounter == null))
   {
      return W_ERROR_BAD_PARAMETER;
   }
   
   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMyDMoveGetConfiguration(
            hConnection,
            PBasicGenericSyncCompletionUint8Uint8,
            &param );
   }

   return PBasicGenericSyncWaitForResultUint8Uint8(&param, pnStatusByte, pnPasswordRetryCounter);         
}

/* See Client API Specifications */
W_ERROR WMyDMoveSetConfigurationSync(
                  W_HANDLE hConnection,
                  uint8_t nStatusByte,
                  uint8_t nPasswordRetryCounter,
                  uint32_t nPassword,
                  bool bLockConfiguration)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMyDMoveSetConfigurationSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMyDMoveSetConfiguration(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            nStatusByte, nPasswordRetryCounter, nPassword, bLockConfiguration );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See Client API Specifications */
W_ERROR WMyDMoveAuthenticateSync(
                  W_HANDLE hConnection,
                  uint32_t nPassword)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMyDMoveAuthenticateSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMyDMoveAuthenticate(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            nPassword );
   }

   return PBasicGenericSyncWaitForResult(&param);
}


#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
