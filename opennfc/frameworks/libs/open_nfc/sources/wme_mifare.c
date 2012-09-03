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
   Contains the implementation of the Mifare functions
*******************************************************************************/

#define P_MODULE  P_MODULE_DEC( MIFA )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* The Mifare UL is organised 16 blocks of 4 bytes */
#define P_MIFARE_BLOCK_NUMBER_UL                   16
#define P_MIFARE_BLOCK_SIZE_UL                     P_NDEF2GEN_BLOCK_SIZE

/* The Mifare UL_C is organised 48 blocks of 4 bytes */
#define P_MIFARE_BLOCK_NUMBER_UL_C                 48
#define P_MIFARE_BLOCK_SIZE_UL_C                   P_NDEF2GEN_BLOCK_SIZE
#define P_MIFARE_BLOCK_NUMBER_DATA_UL_C            44

/* The Mifare 1K is organised in 5 sectors with 4 blocks  of 16 bytes, */
#define P_MIFARE_BLOCK_NUMBER_MINI                 (5 * 4)
#define P_MIFARE_BLOCK_SIZE_MINI                   16

/* The Mifare 1K is organised in 16 sectors with 4 blocks, sector trailer on block 3, of 16 bytes, */
/* with manufacturer data on block 0 of sector 0 */
#define P_MIFARE_BLOCK_NUMBER_1K                  (16 * 4)
#define P_MIFARE_BLOCK_SIZE_1K                     16

/* The Mifare 4K is organised in 32 sectors with 4 blocks, sector trailer on block 3, of 16 bytes */
/* and 8 sectors with 16 blocks of 16 bytes, */
/* with manufacturer data on block 0 of sector 0 */
#define P_MIFARE_BLOCK_NUMBER_4K                  (32 * 4 + 8 * 16)
#define P_MIFARE_BLOCK_SIZE_4K                     16

/* The Mifare Plus 2K is organised in 32 sectors with 4 blocks if 16 bytes */
#define P_MIFARE_BLOCK_NUMBER_PLUS_2K             (32 * 4)
#define P_MIFARE_BLOCK_SIZE_PLUS_2K                16

/* The Mifare Plus 4K is organised in 32 sectors with 4 blocks of 16 bytes
   followed with 8 sectors of 16 blocks of 16 bytes....
   as an approximation, report 64 sectors of 4 blocks of 16 bytes */

#define P_MIFARE_BLOCK_NUMBER_PLUS_4K             (32 * 4 + 8 * 16)
#define P_MIFARE_BLOCK_SIZE_PLUS_4K                16

/* Mifare block info */
#define P_MIFARE_ULC_LOCK_BLOCK              0x28
#define P_MIFARE_ULC_LOCK_LENGTH             2

#define P_MIFARE_FIRST_DATA_BLOCK            (P_NDEF2GEN_STATIC_LOCK_BLOCK + 1)
#define P_MIFARE_UL_LAST_DATA_BLOCK          (P_MIFARE_BLOCK_NUMBER_UL - 1)
#define P_MIFARE_ULC_LAST_DATA_BLOCK         (P_MIFARE_ULC_LOCK_BLOCK - 1)

#define P_MIFARE_ULC_AUTH0_BLOCK             0x2A
#define P_MIFARE_ULC_AUTH1_BLOCK             0x2B
#define P_MIFARE_ULC_KEY_BLOCK               0x2C


/*cache Connection defines*/
#define P_MIFARE_IDENTIFIER_LEVEL            ZERO_IDENTIFIER_LEVEL

#ifndef P_MIFARE_UL_C_DEFAULT_KEY
#define P_MIFARE_UL_C_DEFAULT_KEY            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F }
#endif

static const uint8_t g_MifareULCDefaultKey[] = P_MIFARE_UL_C_DEFAULT_KEY;
static const uint8_t g_MifareULCAuthenticateCommand[] = { 0x1A, 0x00 };

static const uint8_t g_MifareDesfireGetVersion[]                = { 0x90, 0x60, 0x00, 0x00, 0x00 };
static const uint8_t g_MifareDesfireGetNextVersion[]            = { 0x90, 0xAF, 0x00, 0x00, 0x00 };
static const uint8_t g_MifareDesfireGetVersionAnswerMask[]      = { 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF  };
static const uint8_t g_MifareDesfireGetVersionAnswerMasked[]    = { 0x04, 0x01, 0x01, 0x00, 0x00, 0x00, 0x05, 0x91, 0xAF  };

/* Declare a Mifare exchange data structure */
typedef struct __tMifareConnection
{
   /* Memory handle registry */
   tHandleObjectHeader        sObjectHeader;
   /* Connection handle */
   W_HANDLE                   hConnection;

   /* Connection information */
   uint8_t                    nUIDLength;
   uint8_t                    UID[7];

   uint8_t                    aLockBytes[4];
   uint8_t                    nAuthenticateMode;
   uint8_t                    nAuthenticateThreshold;
   bool                       bULCAccessRightsRetrieved;

   /* a temporary buffer used for authentication set processing */
   uint8_t                    aTempBuffer[32];
   uint8_t                    aKey[16];
   uint8_t                    aIvec[8];
   uint8_t                    aRandom[8];

   uint8_t                    nRequestedAuthenticateMode;
   uint8_t                    nRequestedAuthenticateThreshold;
   bool                       bLockAuthentication;
   uint32_t                   nCurrentOperationState;

   tSmartCacheSectorSize*     pSectorSize;
   uint32_t                   nSectorNumber;

   uint32_t                   nSectorSize;

   /* Type of the card (UL, 1K, 4K, Desfire) */
   uint8_t                    nType;

   /* Command informtion */
   uint32_t                   nOffsetToLock;
   uint32_t                   nLengthToLock;

   /* true if an operation is pending to avoid reentrance */
   bool                       bOperationPending;

   /* Callback context */
   tDFCCallbackContext        sCallbackContext;

} tMifareConnection;

/* Declare a Mifare 4A exchange data structure */
typedef struct __tMifareConnection4A
{
   /* Memory handle registry */
   tHandleObjectHeader        sObjectHeader;

   /* Type of the card (Desfire) */
   uint8_t                    nType;

   uint8_t                    nUIDLength;
   uint8_t                    UID[7];

   /* get Version command related stuff */
   uint8_t                    nState;
   W_HANDLE                   hConnection;
   tDFCCallbackContext        sCallbackContext;
   uint8_t                    aBuffer[32];

} tMifareConnection4A;

/**
 * @brief   Destroyes a Mifare connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PMifareDestroyConnection4A(
            tContext* pContext,
            void* pObject )
{
   tMifareConnection4A* pMifareConnection4A = (tMifareConnection4A*)pObject;

   PDebugTrace("static_PMifareDestroyConnection4A");

   CMemoryFree( pMifareConnection4A );

   return P_HANDLE_DESTROY_DONE;
}

/**
 * @brief   Gets the Mifare connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  pPropertyArray  The property array.
 **/
static uint32_t static_PMifareGetPropertyNumber4A(
            tContext* pContext,
            void* pObject)
{
   return 1;
}

/**
 * @brief   Gets the Mifare connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  pPropertyArray  The property array.
 **/
static bool static_PMifareGetProperties4A(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray )
{
   tMifareConnection4A* pMifareConnection4A = (tMifareConnection4A*)pObject;

   PDebugTrace("static_PMifareGetProperties4A");
   pPropertyArray[0] = pMifareConnection4A->nType;
   return true;
}

/**
 * @brief   Checkes the Mifare connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  nPropertyValue  The property value.
 **/
static bool static_PMifareCheckProperties4A(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue )
{
   tMifareConnection4A* pMifareConnection4A = (tMifareConnection4A*)pObject;

   PDebugTrace(
      "static_PMifareCheckProperties4A: nPropertyValue=%s (0x%02X)",
      PUtilTraceConnectionProperty(nPropertyValue), nPropertyValue  );

   return ( nPropertyValue == pMifareConnection4A->nType )?true:false;
}

/* Handle registry Mifare connection type */
tHandleType g_sMifareConnection4A = { static_PMifareDestroyConnection4A,
                                    null,
                                    static_PMifareGetPropertyNumber4A,
                                    static_PMifareGetProperties4A,
                                    static_PMifareCheckProperties4A };

#define P_HANDLE_TYPE_MIFARE_CONNECTION_4_A (&g_sMifareConnection4A)

/**
 * @brief   Destroyes a Mifare connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PMifareDestroyConnection(
            tContext* pContext,
            void* pObject )
{
   tMifareConnection* pMifareConnection = (tMifareConnection*)pObject;

   PDebugTrace("static_PMifareDestroyConnection");

   PDFCFlushCall(&pMifareConnection->sCallbackContext);

   /* Free the Mifare connection structure */
   CMemoryFree( pMifareConnection );

   return P_HANDLE_DESTROY_DONE;
}

/**
 * @brief   Gets the Mifare connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 **/

static uint32_t static_PMifareGetPropertyNumber(
            tContext* pContext,
            void* pObject)
{

   tMifareConnection* pMifareConnection = (tMifareConnection*)pObject;

   if (pMifareConnection->nType == W_PROP_MIFARE_UL_C)
   {
      /* special case for Mifare UL-C, return two properties : W_PROP_MIFARE_UL and W_PROP_MIFARE_UL_C */
      return 2;
   }
   else
   {
      return 1;
   }
}

/**
 * @brief   Gets the Mifare connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  pPropertyArray  The property array.
 **/
static bool static_PMifareGetProperties(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray )
{
   tMifareConnection* pMifareConnection = (tMifareConnection*)pObject;

   PDebugTrace("static_PMifareGetProperties");

   if (pMifareConnection->nType == W_PROP_MIFARE_UL_C)
   {
      /* special case for Mifare UL-C, return two properties : W_PROP_MIFARE_UL and W_PROP_MIFARE_UL_C */
      pPropertyArray[0] = W_PROP_MIFARE_UL_C;
      pPropertyArray[1] = W_PROP_MIFARE_UL;
   }
   else
   {
      pPropertyArray[0] = pMifareConnection->nType;
   }

   return true;
}

/**
 * @brief   Checkes the Mifare connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  nPropertyValue  The property value.
 **/
static bool static_PMifareCheckProperties(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue )
{
   tMifareConnection* pMifareConnection = (tMifareConnection*)pObject;

   PDebugTrace(
      "static_PMifareCheckProperties: nPropertyValue=%s (0x%02X)",
      PUtilTraceConnectionProperty(nPropertyValue), nPropertyValue  );

   if (pMifareConnection->nType == W_PROP_MIFARE_UL_C)
   {
      /* special case for Mifare UL-C, return two properties : W_PROP_MIFARE_UL and W_PROP_MIFARE_UL_C */
      if ((nPropertyValue == W_PROP_MIFARE_UL_C) || (nPropertyValue == W_PROP_MIFARE_UL))
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
      /* standard case */

      return ( nPropertyValue == pMifareConnection->nType )? true : false;
   }
}

/* Handle registry Mifare connection type */
tHandleType g_sMifareConnection = { static_PMifareDestroyConnection,
                                    null,
                                    static_PMifareGetPropertyNumber,
                                    static_PMifareGetProperties,
                                    static_PMifareCheckProperties };

#define P_HANDLE_TYPE_MIFARE_CONNECTION (&g_sMifareConnection)


#define GetBit(value, bit)    (((value) >> (bit)) & 0x01)
#define SetBit(value, bit)    ((value) = (value) | (1 << (bit)))

/**
  * Retreive the lock status of a sector by parsing the lock bytes of the card
  *
  * return true if the sector is locked
  */

static bool static_PMifareIsSectorLocked(
      tContext * pContext,
      tMifareConnection * pMifareConnection,
      uint32_t            nSector)
{
   CDebugAssert((pMifareConnection->nType != W_PROP_MIFARE_UL_C) || (pMifareConnection->bULCAccessRightsRetrieved != false));

   if (nSector <= 1)
   {
      /* Sectors [0 - 1] are locked */
      return true;
   }

   if (nSector == 2)
   {
      /* Check the block locking lockbits */
      return (pMifareConnection->aLockBytes[0] & 0x07) == 0x07;
   }

   if ((3 <= nSector) && (nSector <= 7))
   {
      /* sectors 3-7 locks are located in aLockBytes[0] */
      return GetBit(pMifareConnection->aLockBytes[0], nSector);
   }

   if ((8 <= nSector) && (nSector <= 15))
   {
      /* sectors 8-15 locks are located in aLockBytes[1] */
      return GetBit(pMifareConnection->aLockBytes[1], nSector - 8);
   }

   /* UL-C only */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType)
   {
      if ((16 <= nSector)  && (nSector <= 27))
      {
         return GetBit(pMifareConnection->aLockBytes[2], 1 + (nSector - 16) / 4);
      }

      if ((28 <= nSector)  && (nSector <= 39))
      {
         return GetBit(pMifareConnection->aLockBytes[2], 5 + (nSector - 28) / 4);
      }

      if (nSector == 40)
      {
         /* Check the block locking lockbits */
         return ((pMifareConnection->aLockBytes[2] & 0x11) == 0x11) && ((pMifareConnection->aLockBytes[3] & 0x0F) == 0x0F);
      }

      if ((41 <= nSector) && (nSector <= 43))
      {
         return GetBit(pMifareConnection->aLockBytes[3], nSector - 37);
      }

      if ((44 <= nSector) && (nSector <= 47))
      {
         return GetBit(pMifareConnection->aLockBytes[3], 7);
      }
   }

   /* should not occur */
   CDebugAssert(0);
   return true;
}


static void static_PMifareLockSector(
      tContext * pContext,
      tMifareConnection * pMifareConnection,
      uint32_t            nSector)
{
   CDebugAssert((pMifareConnection->nType != W_PROP_MIFARE_UL_C) || (pMifareConnection->bULCAccessRightsRetrieved != false));

   if ((3 <= nSector) && (nSector <= 7))
   {
      /* sectors 3-7 locks are located in aLockBytes[0] */
      SetBit(pMifareConnection->aLockBytes[0], nSector);
      return;
   }

   if ((8 <= nSector) && (nSector <= 15))
   {
      /* sectors 8-15 locks are located in aLockBytes[1] */
      SetBit(pMifareConnection->aLockBytes[1], nSector - 8);
      return;
   }

   /* UL-C only */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType)
   {
      if ((16 <= nSector)  && (nSector <= 27))
      {
         SetBit(pMifareConnection->aLockBytes[2], 1 + (nSector - 16) / 4);
         return;
      }

      if ((28 <= nSector)  && (nSector <= 39))
      {
         SetBit(pMifareConnection->aLockBytes[2], 5 + (nSector - 28) / 4);
         return;
      }

      if (nSector == 40)
      {
         return;
      }

      if ((41 <= nSector) && (nSector <= 43))
      {
         SetBit(pMifareConnection->aLockBytes[3], 4 + nSector - 41);
         return;
      }

      if ((nSector <= 44) && (nSector <= 47))
      {
         SetBit(pMifareConnection->aLockBytes[3], 7);
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
static bool static_PMifareIsSectorLockable(
      tContext * pContext,
      tMifareConnection * pMifareConnection,
      uint32_t            nSector)
{
   CDebugAssert((pMifareConnection->nType != W_PROP_MIFARE_UL_C) || (pMifareConnection->bULCAccessRightsRetrieved != false));

   if (nSector <= 2)
   {
      /* Lock bytes shall be locked with the dedicated API */
      return false;
   }

   if (nSector == 3)
   {
      return GetBit(pMifareConnection->aLockBytes[0], 0) == 0;
   }

   if ((4 <= nSector) && (nSector <= 9))
   {
      return GetBit(pMifareConnection->aLockBytes[0], 1) == 0;
   }

   if ((10 <= nSector) && (nSector <= 15))
   {
      return GetBit(pMifareConnection->aLockBytes[0], 2) == 0;
   }

   /* UL-C only */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType)
   {
      if ((16 <= nSector) && (nSector <= 27))
      {
         return GetBit(pMifareConnection->aLockBytes[2], 0) == 0;
      }

      if ((28 <= nSector) && (nSector <= 39))
      {
         return GetBit(pMifareConnection->aLockBytes[2], 4) == 0;
      }

      if (nSector == 40)
      {
         /* Lock bytes shall be locked with the dedicated API */
         return false;
      }

      if (nSector == 41)
      {
         return GetBit(pMifareConnection->aLockBytes[3], 0) == 0;
      }

      if (nSector == 42)
      {
         return GetBit(pMifareConnection->aLockBytes[3], 1) == 0;
      }

      if (nSector == 43)
      {
         return GetBit(pMifareConnection->aLockBytes[3], 2) == 0;
      }

      if ((44 <= nSector) && (nSector <= 47))
      {
         return GetBit(pMifareConnection->aLockBytes[3], 3) == 0;
      }
   }

   /* should not occur... */
   CDebugAssert(0);
   return false;
}

/**
 * @brief   Sends the result.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pMifareConnection  The Mifare connection.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PMifareSendResult(
            tContext* pContext,
            tMifareConnection* pMifareConnection,
            W_ERROR nError )
{
   PDebugTrace("static_PMifareSendResult");

   /* Clear the operation in progress flag */
   CDebugAssert(pMifareConnection->bOperationPending != false);
   pMifareConnection->bOperationPending = false;

   /* Send the error */
   PDFCPostContext2(&pMifareConnection->sCallbackContext, P_DFC_TYPE_MIFARE, nError);

   /* Decrement the reference count of the connection. This may destroy the object */
   PHandleDecrementReferenceCount(pContext, pMifareConnection);
}

/**
 *  Lock Bytes 0-1 have been written.
 */
static void static_PMifareWriteLockBytesCompleted(
            tContext * pContext,
            void * pCallbackParameter,
            W_ERROR nError)
{
   tMifareConnection* pMifareConnection = pCallbackParameter;

   PDebugTrace("static_PMifareWriteLockBytesCompleted");

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PMifareWriteLockBytesCompleted: returning %s", PUtilTraceError(nError));
   }

   static_PMifareSendResult(pContext, pMifareConnection, nError);
}

/**
 *  Lock Bytes 2-3 have been written.
 */
static void static_PMifareWriteLockBytes2Completed(
            tContext * pContext,
            void * pCallbackParameter,
            W_ERROR nError)
{
   tMifareConnection* pMifareConnection = pCallbackParameter;

   PDebugTrace("static_PMifareWriteLockBytes2Completed");

   if (nError == W_SUCCESS)
   {
      PNDEF2GenWrite(pContext,
                     pMifareConnection->hConnection,
                     static_PMifareWriteLockBytesCompleted,
                     pMifareConnection,
                     &pMifareConnection->aLockBytes[0],
                     P_NDEF2GEN_STATIC_LOCK_BYTE_ADDRESS,
                     P_NDEF2GEN_STATIC_LOCK_BYTE_LENGTH,
                     null);
   }
   else
   {
      PDebugError("static_PMifareWriteLockBytes2Completed: returning %s", PUtilTraceError(nError));

      static_PMifareSendResult(pContext, pMifareConnection, nError);
   }
}


/**
 *  Writes the lock bytes into the card
 */
static void static_PMifareWriteLockBytes(
            tContext* pContext,
            tMifareConnection* pMifareConnection)
{
   PDebugTrace("static_PMifareWriteLockBytes");

   if (pMifareConnection->nType == W_PROP_MIFARE_UL_C)
   {
      PNDEF2GenWrite(pContext,
                     pMifareConnection->hConnection,
                     static_PMifareWriteLockBytes2Completed,
                     pMifareConnection,
                     &pMifareConnection->aLockBytes[2],
                     pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_LOCK_BLOCK),
                     P_MIFARE_ULC_LOCK_LENGTH,
                     null);
   }
   else
   {
      /* fake the write of the Lock Bytes 2-3 */
      static_PMifareWriteLockBytes2Completed(pContext, pMifareConnection, W_SUCCESS);
   }
}

/**
 * Bytes have been read
 */
static void static_PMifareReadCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tMifareConnection* pMifareConnection = (tMifareConnection*)pCallbackParameter;

   PDebugTrace("static_PMifareReadCompleted");

   static_PMifareSendResult(pContext, pMifareConnection, nError );
}

/**
 * Bytes have been written
 */
static void static_PMifareWriteCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tMifareConnection* pMifareConnection = (tMifareConnection*)pCallbackParameter;

   PDebugTrace("static_PMifareWriteCompleted");

   if(nError == W_SUCCESS)
   {
      /* Check if we need to lock the card */
      if ( pMifareConnection->nLengthToLock != 0 )
      {
         uint32_t nBlockStart, nBlockEnd, nIndex;

         nBlockStart = pMifareConnection->pSectorSize->pDivide(pMifareConnection->nOffsetToLock);
         nBlockEnd   = pMifareConnection->pSectorSize->pDivide(pMifareConnection->nLengthToLock + pMifareConnection->nOffsetToLock - 1);

         for( nIndex = nBlockStart; nIndex <= nBlockEnd; nIndex ++ )
         {
            static_PMifareLockSector(pContext, pMifareConnection, nIndex);
         }

         pMifareConnection->nLengthToLock = 0;

         /* Write lock bytes */
         static_PMifareWriteLockBytes(pContext, pMifareConnection);

         return;
      }
   }

   /* Send the result */
   static_PMifareSendResult( pContext, pMifareConnection, nError );
}

/** access rigths configuration automaton  */
static void static_PMifareULCSetAccessRightsAutomaton(
      tContext * pContext,
      void     * pCallbackParameter,
      W_ERROR    nError)
{
   tMifareConnection* pMifareConnection = pCallbackParameter;

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PMifareULCSetAccessRightsAutomaton : nError %d", nError);
      static_PMifareSendResult(pContext, pMifareConnection, nError);
      return;
   }

   switch (pMifareConnection->nCurrentOperationState)
   {
      case 0 :

         /* to avoid to make the card unusable if something goes wrong during the configuration,
          *  we first set the threshold to P_MIFARE_BLOCK_NUMBER_UL_C to allow non authenticated access
          *  to the whole card content.
          *
          *  The effective threshold value will be set once all other operations (e.g key programming successfully done)
          */

         pMifareConnection->aTempBuffer[0] = P_MIFARE_BLOCK_NUMBER_UL_C;

         PNDEF2GenWrite(pContext, pMifareConnection->hConnection,
                        static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                        pMifareConnection->aTempBuffer,
                        pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_AUTH0_BLOCK), 1,
                        null);

         break;

      case 1 :

         /* update the current authentication threshold */
         pMifareConnection->nAuthenticateThreshold = P_MIFARE_BLOCK_NUMBER_UL_C;

         /* Write the requested authentication mode */
         PNDEF2GenWrite(pContext, pMifareConnection->hConnection,
                        static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                        &pMifareConnection->nRequestedAuthenticateMode,
                        pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_AUTH1_BLOCK), 1,
                        null);
         break;

      case 2 :

         /* update the current authentication mode */
         pMifareConnection->nAuthenticateMode = pMifareConnection->nRequestedAuthenticateMode;

         /* write the requested authentication keys : we do not use the smart cache for this section
            since smart cache performs a read prior a write, and authencication keys are not readable ! */

         /* First 4 bytes of the key */
         PNDEF2GenDirectWrite(pContext, pMifareConnection->hConnection,
                              static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                              pMifareConnection->aKey, P_MIFARE_ULC_KEY_BLOCK);
         break;

      case 3 :

         /* Second 4 bytes of the key */
         PNDEF2GenDirectWrite(pContext, pMifareConnection->hConnection,
                              static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                              pMifareConnection->aKey + 4, P_MIFARE_ULC_KEY_BLOCK + 1);
         break;

      case 4 :

         /* Third 4 bytes of the key */
         PNDEF2GenDirectWrite(pContext, pMifareConnection->hConnection,
                              static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                              pMifareConnection->aKey + 8, P_MIFARE_ULC_KEY_BLOCK + 2);
         break;

      case 5 :

         /* Last 4 bytes of the key */
         PNDEF2GenDirectWrite(pContext, pMifareConnection->hConnection,
                              static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                              pMifareConnection->aKey + 12, P_MIFARE_ULC_KEY_BLOCK + 3);
         break;

      case 6 :

         /* Erase the key to avoid to remain it in memory longer than needed */
         CMemoryFill(pMifareConnection->aKey, 0, sizeof(pMifareConnection->aKey));

         /* write the effective threshold value */
         PNDEF2GenWrite(pContext, pMifareConnection->hConnection,
                        static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                        &pMifareConnection->nRequestedAuthenticateThreshold,
                        pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_AUTH0_BLOCK), 1,
                        null);
         break;

      case 7 :

         /* update the current threshold value */
         pMifareConnection->nAuthenticateThreshold = pMifareConnection->nRequestedAuthenticateThreshold;

         /* after a modification of the authentication configuration, we need to invalidate the smart cache :

            - to avoid further access to authenticated area without prior authentication

            - Mifare UL-C smart cache may be corrupted if cache threshold was not a multiple of 4 sectors
              (in this case, when a read operation goes beyond the threshold, the read operation wraps and returns contents of first sectors of the card)
         */

         PNDEF2GenInvalidateCache(pContext, pMifareConnection->hConnection, 0, P_MIFARE_BLOCK_NUMBER_DATA_UL_C * P_MIFARE_BLOCK_SIZE_UL_C);

         if (pMifareConnection->bLockAuthentication == true)
         {
            /* the lock of the authentication area has been requested */

            pMifareConnection->aTempBuffer[0] = 0xE0;

            PNDEF2GenWrite(pContext, pMifareConnection->hConnection,
                           static_PMifareULCSetAccessRightsAutomaton, pMifareConnection,
                           pMifareConnection->aTempBuffer,
                           pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_LOCK_BLOCK) + 1, 1,
                           null);
         }
         else
         {
            /* no lock requested, the operation is now completed  */
            static_PMifareSendResult(pContext, pMifareConnection, W_SUCCESS);
            return;
         }

         break;

      case 8 :

         static_PMifareSendResult(pContext, pMifareConnection, W_SUCCESS);
         break;
   }

   pMifareConnection->nCurrentOperationState++;
}

static void static_PMifareCreateConnectionCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   tMifareConnection* pMifareConnection = (tMifareConnection*)pCallbackParameter;

   PDebugTrace("static_PMifareCreateConnectionCompleted");

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PMifareCreateConnectionCompleted: returning %s", PUtilTraceError(nError));
   }

   static_PMifareSendResult(pContext, pMifareConnection, nError);
}

extern tSmartCacheSectorSize g_sSectorSize4;

/* See Header file */
void PMifareCreateConnection3A(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProperty )
{
   tMifareConnection* pMifareConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMifareCreateConnection3A");

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Create the NDEF buffer */
   pMifareConnection = (tMifareConnection*)CMemoryAlloc( sizeof(tMifareConnection) );
   if ( pMifareConnection == null )
   {
      PDebugError("PMifareCreateConnection3A: pMifareConnection == null");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }
   CMemoryFill(pMifareConnection, 0, sizeof(tMifareConnection));

   /* Get the 14443-3 information level */
   if ( ( nError = P14P3UserCheckMifare(
                     pContext,
                     hConnection,
                     pMifareConnection->UID,
                     &pMifareConnection->nUIDLength,
                     &pMifareConnection->nType ) ) != W_SUCCESS )
   {
      PDebugLog("PMifareCreateConnection3A: not a Mifare card");
      goto return_error;
   }

   PDebugTrace("PMifareCreateConnection3A: detection of the type %s",
      PUtilTraceConnectionProperty(pMifareConnection->nType));

   switch ( pMifareConnection->nType )
   {
      case W_PROP_MIFARE_UL:
      case W_PROP_MIFARE_UL_C :
         /* for now, we can not distinguish the Mifare UL from the Mifare UL-C card */
         pMifareConnection->nSectorNumber = P_MIFARE_BLOCK_NUMBER_UL;
         pMifareConnection->nSectorSize   = P_MIFARE_BLOCK_SIZE_UL;
         pMifareConnection->pSectorSize   = &g_sSectorSize4;
         break;

      case W_PROP_MIFARE_MINI :
         pMifareConnection->nSectorNumber = P_MIFARE_BLOCK_NUMBER_MINI;
         pMifareConnection->nSectorSize   = P_MIFARE_BLOCK_SIZE_MINI;
         break;

      case W_PROP_MIFARE_1K:
         pMifareConnection->nSectorNumber = P_MIFARE_BLOCK_NUMBER_1K;
         pMifareConnection->nSectorSize   = P_MIFARE_BLOCK_SIZE_1K;
         break;
      case W_PROP_MIFARE_4K:
         pMifareConnection->nSectorNumber = P_MIFARE_BLOCK_NUMBER_4K;
         pMifareConnection->nSectorSize   = P_MIFARE_BLOCK_SIZE_4K;
         break;

      case W_PROP_MIFARE_PLUS_S_2K :
      case W_PROP_MIFARE_PLUS_X_2K :
         pMifareConnection->nSectorNumber = P_MIFARE_BLOCK_NUMBER_PLUS_2K;
         pMifareConnection->nSectorSize   = P_MIFARE_BLOCK_SIZE_PLUS_2K;
         break;

      case W_PROP_MIFARE_PLUS_S_4K :
      case W_PROP_MIFARE_PLUS_X_4K :
         pMifareConnection->nSectorNumber = P_MIFARE_BLOCK_NUMBER_PLUS_4K;
         pMifareConnection->nSectorSize   = P_MIFARE_BLOCK_SIZE_PLUS_4K;
         break;

      default:
         PDebugError(
            "PMifareCreateConnection3A: unknown type" );
         nError = W_ERROR_CONNECTION_COMPATIBILITY;
         goto return_error;
   }

   /* Add the Mifare connection structure */
   if ( ( nError = PHandleAddHeir(
                     pContext,
                     hConnection,
                     pMifareConnection,
                     P_HANDLE_TYPE_MIFARE_CONNECTION ) ) != W_SUCCESS )
   {
      PDebugError("PMifareCreateConnection3A: error returned by PHandleAddHeir()");
      goto return_error;
   }

   /* Store the connection information */
   pMifareConnection->hConnection = hConnection;

   if (W_PROP_MIFARE_UL != pMifareConnection->nType && W_PROP_MIFARE_UL_C != pMifareConnection->nType)
   {
      /* Nothing to read in this type of card */
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MIFARE, W_SUCCESS);
   }
   else
   {
      /* --- Read Lock bytes 0-1 (in block 2) --- */
      /* Store the callback context */
      pMifareConnection->sCallbackContext = sCallbackContext;

      /* set the operation in progress flag to avoid reentrancy */
      pMifareConnection->bOperationPending = true;

      /* Increment the reference count to keep the connection object alive during
         the operation. The reference count is decreased in static_PMifareSendResult()
         when the operation is completed */
      PHandleIncrementReferenceCount(pMifareConnection);

      /* Read data */
      PNDEF2GenRead(pContext,
                    hConnection,
                    static_PMifareCreateConnectionCompleted,
                    pMifareConnection,
                    &pMifareConnection->aLockBytes[0],
                    P_NDEF2GEN_STATIC_LOCK_BYTE_ADDRESS,
                    P_NDEF2GEN_STATIC_LOCK_BYTE_LENGTH,
                    null);
   }

   return;

return_error:

   if (nError != W_ERROR_CONNECTION_COMPATIBILITY)
   {
      PDebugError("PMifareCreateConnection3A: return error %s", PUtilTraceError(nError));
   }

   if(pMifareConnection != null)
   {
      CMemoryFree(pMifareConnection);
   }

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MIFARE, nError);
}

/** See tPReaderUserRemoveSecondaryConnection */
void PMifareRemoveConnection3A(
            tContext* pContext,
            W_HANDLE hUserConnection )
{
   tMifareConnection* pMifareConnection = (tMifareConnection*)PHandleRemoveLastHeir(
            pContext, hUserConnection,
            P_HANDLE_TYPE_MIFARE_CONNECTION);

   PDebugTrace("PMifareRemoveConnection3A");

   /* Remove the connection object */
   if(pMifareConnection != null)
   {
      CMemoryFree(pMifareConnection);
   }
}

static void static_PMifareGetVersionCompleted(
      tContext * pContext,
      void * pCallbackParameter,
      uint32_t nLength,
      W_ERROR  nError)
{
   tMifareConnection4A* pMifareConnection4A = (tMifareConnection4A*) pCallbackParameter;
   uint32_t             i;

   if (nError != W_SUCCESS)
   {
      goto end;
   }

   switch (pMifareConnection4A->nState)
   {
      case 0 :  /* First answer */

         if (nLength != sizeof (g_MifareDesfireGetVersionAnswerMask))
         {
            PDebugError("static_PMifareGetVersionCompleted : Invalid answer length");
            nError = W_ERROR_CONNECTION_COMPATIBILITY;
            goto end;
         }

         /* apply the mask to the answer received from the card */
         for (i=0; i<nLength; i++)
         {
            if ((pMifareConnection4A->aBuffer[i] & g_MifareDesfireGetVersionAnswerMask[i]) != g_MifareDesfireGetVersionAnswerMasked[i])
            {
               PDebugError("static_PMifareGetVersionCompleted : Invalid answer payload");
               nError = W_ERROR_CONNECTION_COMPATIBILITY;
               goto end;
            }
         }

         switch (pMifareConnection4A->aBuffer[3])     /* Major version numer */
         {
            case 0x00 :

               /* DESFIRE D40 - size should be 4 K */

               if (pMifareConnection4A->aBuffer[5] == 0x18)    /* Size */
               {
                  pMifareConnection4A->nType = W_PROP_MIFARE_DESFIRE_D40;
               }
               else
               {
                  PDebugError("static_PMifareGetVersionCompleted : Invalid size for DESFilre D40 !!!");
                  nError = W_ERROR_CONNECTION_COMPATIBILITY;
                  goto end;
               }
               break;

            case 0x01 :

               /* DESFIRE EV1 series */

               switch (pMifareConnection4A->aBuffer[5])        /* Size */
               {
                  case 0x16 :
                     pMifareConnection4A->nType = W_PROP_MIFARE_DESFIRE_EV1_2K;
                     nError = W_SUCCESS;
                     break;

                  case 0x18 :
                     pMifareConnection4A->nType = W_PROP_MIFARE_DESFIRE_EV1_4K;
                     nError = W_SUCCESS;
                     break;

                  case 0x1A :
                     pMifareConnection4A->nType = W_PROP_MIFARE_DESFIRE_EV1_8K;
                     nError = W_SUCCESS;
                     break;

                  default :

                     PDebugError("static_PMifareGetVersionCompleted : Invalid size for DESFire EV1 !!!");
                     nError = W_ERROR_CONNECTION_COMPATIBILITY;
                     goto end;
               }
               break;

            default :

               PDebugError("static_PMifareGetVersionCompleted : Invalid major version");
               nError = W_ERROR_CONNECTION_COMPATIBILITY;
               goto end;
         }


         /* Ok, all checks passed */
         P14Part4ExchangeData(pContext, pMifareConnection4A->hConnection, static_PMifareGetVersionCompleted, pMifareConnection4A,
                                 g_MifareDesfireGetNextVersion, sizeof(g_MifareDesfireGetNextVersion),
                                 pMifareConnection4A->aBuffer, sizeof(pMifareConnection4A->aBuffer), null);
         pMifareConnection4A->nState++;

         /* no more processing here */
         return;

      case 1 : /* Second answer */

         if (nLength != 9)
         {
            PDebugError("static_PMifareGetVersionCompleted : Invalid answer length/3");
            nError = W_ERROR_CONNECTION_COMPATIBILITY;
         }

         P14Part4ExchangeData(pContext, pMifareConnection4A->hConnection, static_PMifareGetVersionCompleted, pMifareConnection4A,
                                 g_MifareDesfireGetNextVersion, sizeof(g_MifareDesfireGetNextVersion),
                                 pMifareConnection4A->aBuffer, sizeof(pMifareConnection4A->aBuffer), null);

         pMifareConnection4A->nState++;

         /* no more processing here */
         return;

      case 2 :  /* Third and last answer */

         if (nLength != 16)
         {
            PDebugError("static_PMifareGetVersionCompleted : Invalid answer length/3");
            nError = W_ERROR_CONNECTION_COMPATIBILITY;
         }
   }

end:

   /* Send the result */

   if (nError == W_SUCCESS)
   {
      /* Add the Mifare connection structure */
      if ( ( nError = PHandleAddHeir(
                        pContext,
                        pMifareConnection4A->hConnection,
                        pMifareConnection4A,
                        P_HANDLE_TYPE_MIFARE_CONNECTION_4_A ) ) != W_SUCCESS )
      {
         PDebugError("PMifareCreateConnection4A: Error in PHandleAddHeir()");
      }
   }

   /* Send the result */
   PDFCPostContext2( & pMifareConnection4A->sCallbackContext, P_DFC_TYPE_MIFARE, nError );

   if (nError != W_SUCCESS)
   {
      CMemoryFree(pMifareConnection4A);
   }
}

/* See Header file */
void PMifareCreateConnection4A(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProperty )
{
   tMifareConnection4A* pMifareConnection4A = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMifareCreateConnection4A");

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Create the NDEF buffer */
   pMifareConnection4A = (tMifareConnection4A*)CMemoryAlloc( sizeof(tMifareConnection4A) );
   if ( pMifareConnection4A == null )
   {
      PDebugError("PMifareCreateConnection4A: pMifareConnection == null");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }
   CMemoryFill(pMifareConnection4A, 0, sizeof(tMifareConnection4A));

   nError = P14P4UserCheckMifare(
                     pContext,
                     hConnection,
                     pMifareConnection4A->UID,
                     &pMifareConnection4A->nUIDLength,
                     &pMifareConnection4A->nType );

   if (nError != W_SUCCESS)
   {
      goto return_error;
   }

   /* Check the property */
   switch (pMifareConnection4A->nType)
   {
      /* DESFire series */
      case W_PROP_MIFARE_DESFIRE_D40 :

         /* generic desfire detected, send the Get Version command to get the variant */

         pMifareConnection4A->hConnection = hConnection;
         pMifareConnection4A->sCallbackContext = sCallbackContext;

         P14Part4ExchangeData(pContext, hConnection, static_PMifareGetVersionCompleted, pMifareConnection4A,
                                 g_MifareDesfireGetVersion, sizeof(g_MifareDesfireGetVersion),
                                 pMifareConnection4A->aBuffer, sizeof(pMifareConnection4A->aBuffer), null);
         return;

      case W_PROP_MIFARE_DESFIRE_EV1_2K :
      case W_PROP_MIFARE_DESFIRE_EV1_4K :
      case W_PROP_MIFARE_DESFIRE_EV1_8K :

         break;

      default :
         PDebugError("PMifareCreateConnection4A: not an expected Mifare");
         nError = W_ERROR_CONNECTION_COMPATIBILITY;
         goto return_error;
   }

   /* Add the Mifare connection structure */
   if ( ( nError = PHandleAddHeir(
                     pContext,
                     hConnection,
                     pMifareConnection4A,
                     P_HANDLE_TYPE_MIFARE_CONNECTION_4_A ) ) != W_SUCCESS )
   {
      PDebugError("PMifareCreateConnection4A: Error in PHandleAddHeir()");
      goto return_error;
   }

   /* Send the result */
   PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_MIFARE, W_SUCCESS );
   return;

return_error:

   if (nError != W_ERROR_CONNECTION_COMPATIBILITY)
   {
      PDebugError("PMifareCreateConnection4A: return error %s", PUtilTraceError(nError));
   }

   PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_MIFARE, nError );

   CMemoryFree(pMifareConnection4A);
}

/* See Header file */
W_ERROR PMifareCheckType2(
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
   tMifareConnection* pMifareConnection;
   uint8_t nIndex;
   W_ERROR nError;
   bool    bIsLocked = false;
   bool    bIsLockable = true;
   uint32_t nLastBlockIndex;

   PDebugTrace("PMifareCheckType2");

   /* Reset the maximum tag size */
   if (pnMaximumSpaceSize != null) *pnMaximumSpaceSize = 0;

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if ( nError == W_SUCCESS )
   {
      /* Maximum size */
      switch ( pMifareConnection->nType )
      {
      case W_PROP_MIFARE_UL:
         nLastBlockIndex = P_MIFARE_UL_LAST_DATA_BLOCK;
         break;
      case W_PROP_MIFARE_UL_C :
         nLastBlockIndex = P_MIFARE_ULC_LAST_DATA_BLOCK;

         if (pMifareConnection->bULCAccessRightsRetrieved == false)
         {
            /* Access rights information have not been retreived, we can not format this card */
            PDebugError("PMifareCheckType2: access rights unavailable");
            return W_ERROR_MISSING_INFO;
         }
         break;
      default:
         /* other mifare cards are not supported (since we do not support read/write) */
         PDebugError("PMifareCheckType2: Unknown type");
         return W_ERROR_CONNECTION_COMPATIBILITY;
      }

      if (pnMaximumSpaceSize != null)
      {
         *pnMaximumSpaceSize = pMifareConnection->pSectorSize->pMultiply(nLastBlockIndex - P_MIFARE_FIRST_DATA_BLOCK + 1);
      }

      /* Go through the lock byte */
      for (nIndex=4; nIndex <= nLastBlockIndex; nIndex++)
      {
         bIsLocked |= static_PMifareIsSectorLocked(pContext, pMifareConnection, nIndex);
         bIsLockable &= static_PMifareIsSectorLockable(pContext, pMifareConnection, nIndex);
      }

      if (pbIsLocked != null)       *pbIsLocked = bIsLocked;
      if (pbIsLockable != null)     *pbIsLockable = bIsLockable;
      if (pnSectorSize != null)     *pnSectorSize = (uint8_t)pMifareConnection->pSectorSize->nValue;
      if (pbIsFormattable != null)  *pbIsFormattable = ! bIsLocked;
      if (pnSerialNumberLength != null) *pnSerialNumberLength = pMifareConnection->nUIDLength;
      if (pSerialNumber != null) CMemoryCopy( pSerialNumber, pMifareConnection->UID, pMifareConnection->nUIDLength );

      return W_SUCCESS;
   }

   return nError;
}

/* See Client API Specifications */
W_ERROR PMifareGetConnectionInfo(
            tContext* pContext,
            W_HANDLE hConnection,
            tWMifareConnectionInfo *pConnectionInfo )
{
   tMifareConnection* pMifareConnection;
   W_ERROR nError;

   PDebugTrace("PMifareGetConnectionInfo");

   /* Check the parameters */
   if ( pConnectionInfo == null )
   {
      PDebugError("PMifareGetConnectionInfo: pConnectionInfo == null");
      return W_ERROR_BAD_PARAMETER;
   }

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if ( nError == W_SUCCESS )
   {
      /* UID */
      CMemoryCopy(
         pConnectionInfo->UID,
         pMifareConnection->UID,
         pMifareConnection->nUIDLength );
      /* Sector size */
      pConnectionInfo->nSectorSize = (uint16_t)pMifareConnection->nSectorSize;
      /* Sector number */
      pConnectionInfo->nSectorNumber = (uint16_t)pMifareConnection->nSectorNumber;
      return W_SUCCESS;
   }
   else
   {
      tMifareConnection4A * pMifareConnection4A;

      nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION_4_A, (void**)&pMifareConnection4A);

      if ( nError == W_SUCCESS )
      {
         /* UID */
         CMemoryCopy(
            pConnectionInfo->UID,
            pMifareConnection4A->UID,
            pMifareConnection4A->nUIDLength );
         /* Sector size is meaningless for DESFire */
         pConnectionInfo->nSectorSize = 0;
         /* Sector number is meaningless for DESFire */
         pConnectionInfo->nSectorNumber = 0;
      }
      else
      {
         PDebugError("PMifareGetConnectionInfo: could not get pMifareConnection buffer");

         /* Fill in the structure with zeros */
         CMemoryFill(pConnectionInfo, 0, sizeof(tWMifareConnectionInfo));
      }

      return nError;
   }
}

/* See Client API Specifications */
void PMifareRead(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void *pCallbackParameter,
            uint8_t *pBuffer,
            uint32_t nOffset,
            uint32_t nLength,
            W_HANDLE *phOperation )
{
   tMifareConnection* pMifareConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMifareRead");

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Check if the connection handle is valid */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if ( nError != W_SUCCESS )
   {
      PDebugError("PMifareRead: Bad handle");
      goto return_error;
   }

   /* For now, we only support MIFARE UL and MIFARE UL-C */
   if ((pMifareConnection->nType != W_PROP_MIFARE_UL) && (pMifareConnection->nType != W_PROP_MIFARE_UL_C))
   {
      PDebugError("PMifareRead: not a Mifare UL / UL-C");
      nError = W_ERROR_FEATURE_NOT_SUPPORTED;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMifareConnection->bOperationPending != false)
   {
      PDebugError("PMifareRead: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* For reading blocks from 3 to n, check if access rights have been retrieved */
   /* Blocks 0 to 2 are always readable */
   if ( W_PROP_MIFARE_UL_C == pMifareConnection->nType && pMifareConnection->bULCAccessRightsRetrieved != true &&
        (nOffset + nLength) > pMifareConnection->pSectorSize->pMultiply(P_MIFARE_FIRST_DATA_BLOCK) )
   {
      PDebugError("PMifareRead : called without prior call to PMifareULRetrieveAccessRights");
      nError = W_ERROR_MISSING_INFO;
      goto return_error;
   }

   /* Check parameters */
   if ( (null == pBuffer) || (0 == nLength) )
   {
      PDebugError("PMifareRead: null parameter");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* for Mifare UL-C, specific check to avoid read of the keys, which can make the card unusable... */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType)
   {
      if ( (nOffset + nLength) > pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_KEY_BLOCK) )
      {
         PDebugError("PMifareRead: the data to read/write is too large (reading keys is not allowed)");
         nError = W_ERROR_BAD_PARAMETER;
         goto return_error;
      }
   }
   else
   {
      if ( (nOffset + nLength) > pMifareConnection->pSectorSize->pMultiply(pMifareConnection->nSectorNumber) )
      {
         PDebugError("PMifareRead: the data to read/write is too large");
         nError = W_ERROR_BAD_PARAMETER;
         goto return_error;
      }
   }

   /* Store the connection information */
   pMifareConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMifareConnection->bOperationPending = true;

   /* Increment the reference count to keep the connection object alive during
      the operation. The reference count is decreased in static_PMifareSendResult()
      when the operation is completed */
   PHandleIncrementReferenceCount(pMifareConnection);

   PNDEF2GenRead(pContext,
                 hConnection,
                 static_PMifareReadCompleted,
                 pMifareConnection,
                 pBuffer,
                 nOffset,
                 nLength,
                 phOperation);

   return;

return_error:

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MIFARE, nError);
}

/* See Client API Specifications */
void PMifareWrite(
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
   tMifareConnection* pMifareConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;
   uint32_t nIndex;

   PDebugTrace("PMifareWrite");

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Check if the connection handle is valid */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if ( nError != W_SUCCESS )
   {
      PDebugError("PMifareWrite: Bad handle");
      goto return_error;
   }

   /* For now, we only support MIFARE UL and MIFARE UL-C */
   if ((pMifareConnection->nType != W_PROP_MIFARE_UL) && (pMifareConnection->nType != W_PROP_MIFARE_UL_C))
   {
      PDebugError("PMifareWrite: not a Mifare UL / UL-C");
      nError = W_ERROR_FEATURE_NOT_SUPPORTED;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMifareConnection->bOperationPending != false)
   {
      PDebugError("PMifareWrite: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check if access rights have been retrieved */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType && pMifareConnection->bULCAccessRightsRetrieved != true)
   {
      PDebugError("PMifareWrite : called without prior call to PMifareULRetrieveAccessRights");
      nError = W_ERROR_MISSING_INFO;
      goto return_error;
   }

   /* Check the parameters */
   if ((pBuffer == null) && (bLockSectors == false))
   {
      /* pBuffer null is only allowed for lock */
      PDebugError("PMifareWrite: W_ERROR_BAD_PARAMETER");

      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ((nLength == 0) && ((pBuffer != null) || (nOffset != 0) || (bLockSectors == false)))
   {
      /* nLength == 0 is only valid for whole tag lock */
      PDebugError("PMifareWrite: W_ERROR_BAD_PARAMETER");

      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ( (nOffset + nLength) > pMifareConnection->pSectorSize->pMultiply(pMifareConnection->nSectorNumber) )
   {
      PDebugError("PMifareWrite: the data to read/write is too large");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* for Mifare UL-C, specific check to avoid direct write of the keys, which can
      make the card unusable... */
   if (pMifareConnection->nType == W_PROP_MIFARE_UL_C)
   {
      if ((nOffset + nLength) > pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_KEY_BLOCK))
      {
         PDebugError("PMifareWrite: direct write of the key is not supported (unsafe) - use the specialised API !!!");
         nError = W_ERROR_FEATURE_NOT_SUPPORTED;
         goto return_error;
      }
   }

   if ((pBuffer == null) && (nOffset == 0) && (nLength == 0))
   {
      /* specific case used to lock the entire data area of the card */
      if (pMifareConnection->nType == W_PROP_MIFARE_UL)
      {
         nOffset =  pMifareConnection->pSectorSize->pMultiply(P_MIFARE_FIRST_DATA_BLOCK);
         nLength =  pMifareConnection->pSectorSize->pMultiply(P_MIFARE_UL_LAST_DATA_BLOCK - P_MIFARE_FIRST_DATA_BLOCK + 1);
      }
      else
      {
         nOffset =  pMifareConnection->pSectorSize->pMultiply(P_MIFARE_FIRST_DATA_BLOCK);
         nLength =  pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_LAST_DATA_BLOCK - P_MIFARE_FIRST_DATA_BLOCK + 1);
      }
   }

   /* Check if the Mifare card is locked/lockable */
   for (nIndex = pMifareConnection->pSectorSize->pDivide(nOffset);
            nIndex <= pMifareConnection->pSectorSize->pDivide(nOffset + nLength - 1); nIndex++)
   {
      if ((pBuffer != null) && (static_PMifareIsSectorLocked(pContext, pMifareConnection, nIndex) != false))
      {
         PDebugError("PMifareWrite: item locked");
         nError = W_ERROR_ITEM_LOCKED;
         goto return_error;
      }

      if ((bLockSectors != false) && (static_PMifareIsSectorLockable(pContext, pMifareConnection, nIndex) != true))
      {
         PDebugError("PMifareWrite: item not lockable");
         nError = W_ERROR_ITEM_LOCKED;
         goto return_error;
      }
   }

   /* Store the exchange information */
   if(bLockSectors != false)
   {
      pMifareConnection->nOffsetToLock = nOffset;
      pMifareConnection->nLengthToLock = nLength;
   }
   else
   {
      pMifareConnection->nOffsetToLock = 0;
      pMifareConnection->nLengthToLock = 0;
   }

   /* Store the connection information */
   pMifareConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMifareConnection->bOperationPending = true;

   /* Increment the reference count to keep the connection object alive during
      the operation. The reference count is decreased in static_PMifareSendResult()
      when the operation is completed */
   PHandleIncrementReferenceCount(pMifareConnection);

   if (pBuffer != null)
   {
      PNDEF2GenWrite(pContext,
                     pMifareConnection->hConnection,
                     static_PMifareWriteCompleted,
                     pMifareConnection,
                     pBuffer,
                     nOffset,
                     nLength,
                     phOperation);
   }
   else
   {
      static_PMifareWriteCompleted(pContext, pMifareConnection, W_SUCCESS);
   }

   return;

return_error:

   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MIFARE, nError);
}

/* See Client API Specifications */
W_ERROR WMifareReadSync(
            W_HANDLE hConnection,
            uint8_t *pBuffer,
            uint32_t nOffset,
            uint32_t nLength )
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMifareReadSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMifareRead(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            pBuffer, nOffset, nLength,
            null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See Client API Specifications */
W_ERROR WMifareWriteSync(
            W_HANDLE hConnection,
            const uint8_t* pBuffer,
            uint32_t nOffset,
            uint32_t nLength,
            bool bLockCard )
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMifareWriteSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMifareWrite(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            pBuffer, nOffset, nLength, bLockCard,
            null );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See client API specification */
W_ERROR PMifareULForceULC(
                  tContext * pContext,
                  W_HANDLE hConnection)
{
   tMifareConnection* pMifareConnection = null;
   W_ERROR            nError;

   PDebugTrace("PMifareULForceULC");

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);

   if (nError != W_SUCCESS)
   {
      PDebugError("PMifareULForceULC : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      return nError;
   }

   switch (pMifareConnection->nType)
   {
      case W_PROP_MIFARE_UL :

         /* Create the smart cache for the dynamic area (>64bytes) */
         if ( (nError = PNDEF2GenCreateSmartCacheDynamic(pContext, hConnection, P_MIFARE_BLOCK_NUMBER_UL_C))  != W_SUCCESS)
         {
            PDebugError("PMifareULForceULC : PNDEF2GenCreateSmartCacheDynamic returned %s", PUtilTraceError(nError));
            return nError;
         }

         /* Update the sector number */
         pMifareConnection->nSectorNumber = P_MIFARE_BLOCK_NUMBER_UL_C;

         /* upgrade the connection to a Mifare UL-C */
         pMifareConnection->nType = W_PROP_MIFARE_UL_C;
         break;

      case W_PROP_MIFARE_UL_C :
         /* already a Mifare UL-C connection, nothing to to */
         break;

      default :
         PDebugError("PMifareULForceULC : not a Mifare UL / UL-C connection");
         return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   return W_SUCCESS;
}

/* ------------------------------------------------------ */

/* See client API */
void PMifareULFreezeDataLockConfiguration(
      tContext * pContext,
      W_HANDLE hConnection,
      tPBasicGenericCallbackFunction * pCallback,
      void * pCallbackParameter)
{

   tMifareConnection* pMifareConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMifareULFreezeDataLockConfiguration");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMifareULFreezeDataLockConfiguration : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   if ((pMifareConnection->nType != W_PROP_MIFARE_UL) && (pMifareConnection->nType != W_PROP_MIFARE_UL_C))
   {
      /* Not a Mifare UL / Mifare UL-C connection */
      PDebugError("PMifareULFreezeDataLockConfiguration : not a Mifare UL / UL-C");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMifareConnection->bOperationPending != false)
   {
      PDebugError("PMifareULFreezeDataLockConfiguration: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check if access rights have been retrieved */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType && pMifareConnection->bULCAccessRightsRetrieved != true)
   {
      PDebugError("PMifareULFreezeDataLockConfiguration : called without prior call to PMifareULRetrieveAccessRights");
      nError = W_ERROR_MISSING_INFO;
      goto return_error;
   }

   /* store the callback context */
   pMifareConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMifareConnection->bOperationPending = true;

   /* increment the ref count to avoid prematurely freeing of the connection */
   PHandleIncrementReferenceCount(pMifareConnection);

   /* LOCK byte # 0 : Set the bits 1 and 2 that lock the other lock bits */
   pMifareConnection->aLockBytes[0] |= 0x06;

   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType)
   {
      /* LOCK byte # 3 : Set the bits 0 and 4 that lock the other lock bits */
      pMifareConnection->aLockBytes[2] |= 0x11;
   }

   /* Write lock bytes */
   static_PMifareWriteLockBytes(pContext, pMifareConnection);

   return;

return_error :

   /* Send the error */
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MIFARE, nError);
}

/* See client API */
W_ERROR WMifareULFreezeDataLockConfigurationSync(
      W_HANDLE hConnection)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMifareULFreezeDataLockConfigurationSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMifareULFreezeDataLockConfiguration(
            hConnection,
            PBasicGenericSyncCompletion,
            &param);
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* ------------------------------------------------------ */

/** access rigths configuration automaton  */
static void static_PMifareULCRetrieveAccessRightsAutomaton(
      tContext * pContext,
      void     * pCallbackParameter,
      W_ERROR    nError)
{
   tMifareConnection* pMifareConnection = (tMifareConnection*) pCallbackParameter;

   if (nError != W_SUCCESS)
   {
      PDebugError("static_PMifareULCRetrieveAccessRightsAutomaton at state %d returning %s",
                     pMifareConnection->nCurrentOperationState, PUtilTraceError(nError));
      goto return_error;
   }

   switch (pMifareConnection->nCurrentOperationState)
   {
      case 0 :
         /* Read Auth byte 0 (in block 0x2A) */
         PNDEF2GenRead(pContext,
                       pMifareConnection->hConnection,
                       static_PMifareULCRetrieveAccessRightsAutomaton,
                       pMifareConnection,
                       &pMifareConnection->nAuthenticateThreshold,
                       pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_AUTH0_BLOCK),
                       1,
                       null);
         break;

      case 1 :
         /* Read Auth byte 1 (in block 0x2B) */
         PNDEF2GenRead(pContext,
                       pMifareConnection->hConnection,
                       static_PMifareULCRetrieveAccessRightsAutomaton,
                       pMifareConnection,
                       &pMifareConnection->nAuthenticateMode,
                       pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_AUTH1_BLOCK),
                       1,
                       null);
         break;

      case 2 :
         /* All bytes have been read */
         pMifareConnection->nCurrentOperationState = 0;
         pMifareConnection->bULCAccessRightsRetrieved = true;
         static_PMifareSendResult(pContext, pMifareConnection, W_SUCCESS);
         return;
   }

   pMifareConnection->nCurrentOperationState++;

   return;

return_error:

   /* Send error */
   static_PMifareSendResult(pContext, pMifareConnection, nError);
}

/* See client API */
void PMifareULRetrieveAccessRights (
                  tContext * pContext,
                  W_HANDLE hConnection,
                  tPBasicGenericCallbackFunction *pCallback,
                  void *pCallbackParameter)
{
   tMifareConnection* pMifareConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDebugTrace("PMifareULRetrieveAccessRights");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMifareULRetrieveAccessRights : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   /* Only Mifare UL / UL-C are supported */
   if ((pMifareConnection->nType != W_PROP_MIFARE_UL) && (pMifareConnection->nType != W_PROP_MIFARE_UL_C))
   {
      PDebugError("PMifareULRetrieveAccessRights: Not a Mifare UL / UL-C");
      nError = W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if ( pMifareConnection->bOperationPending != false )
   {
      PDebugError("PMifareULRetrieveAccessRights: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* store the callback context */
   pMifareConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMifareConnection->bOperationPending = true;

   /* increment the ref count to avoid prematurely freeing of the connection */
   PHandleIncrementReferenceCount(pMifareConnection);

   /* For UL-C, access rights (lock and auth bytes) must be read */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType && pMifareConnection->bULCAccessRightsRetrieved != true)
   {
      /* Init automaton */
      pMifareConnection->nCurrentOperationState = 0;

      /* Read Lock bytes 2-3 (in block 0x28) */
      PNDEF2GenRead(pContext,
                    pMifareConnection->hConnection,
                    static_PMifareULCRetrieveAccessRightsAutomaton,
                    pMifareConnection,
                    &pMifareConnection->aLockBytes[2],
                    pMifareConnection->pSectorSize->pMultiply(P_MIFARE_ULC_LOCK_BLOCK),
                    P_MIFARE_ULC_LOCK_LENGTH,
                    null);
   }
   else
   {
      /* For Mifare UL, only the lock bytes are needed, and have been already read, so nothing to do */
      static_PMifareSendResult(pContext, pMifareConnection, W_SUCCESS);
   }

   return;

return_error :

   /* Send the error */
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MIFARE, nError);
}


/* See client API */
W_ERROR WMifareULRetrieveAccessRightsSync (
                  W_HANDLE hConnection)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMifareULRetrieveAccessRightsSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMifareULRetrieveAccessRights(
            hConnection,
            PBasicGenericSyncCompletion,
            &param);
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See header */
W_ERROR PMifareNDEF2Lock(tContext * pContext,
                         W_HANDLE hConnection)
{
   tMifareConnection* pMifareConnection = null;
   W_ERROR nError;

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMifareNDEF2Lock : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      return nError;
   }

   /* Only Mifare UL / UL-C are supported */
   if ((pMifareConnection->nType != W_PROP_MIFARE_UL) && (pMifareConnection->nType != W_PROP_MIFARE_UL_C))
   {
      PDebugError("PMifareNDEF2Lock: Not a Mifare UL / UL-C");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Update lock bytes */
   CMemoryFill(pMifareConnection->aLockBytes, 0xFF, sizeof(pMifareConnection->aLockBytes));

   return W_SUCCESS;
}

/* ------------------------------------------------------ */

/* See client API */
W_ERROR PMifareULGetAccessRights (
                  tContext * pContext,
                  W_HANDLE hConnection,
                  uint32_t nOffset,
                  uint32_t nLength,
                  uint32_t *pnRights)
{
   tMifareConnection* pMifareConnection = null;
   bool               bIsLocked;
   W_ERROR nError;
   uint32_t i, nStart, nStop;

   PDebugTrace("PMifareULGetAccessRights");

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMifareULGetAccessRights : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      return (nError);
   }

   /* Only Mifare UL / UL-C are supported */
   if ((pMifareConnection->nType != W_PROP_MIFARE_UL) && (pMifareConnection->nType != W_PROP_MIFARE_UL_C))
   {
      PDebugError("PMifareULGetAccessRights : not a Mifare UL / UL-C");
      return (W_ERROR_CONNECTION_COMPATIBILITY);
   }

   /* Check if access rights have been retrieved */
   if (W_PROP_MIFARE_UL_C == pMifareConnection->nType && pMifareConnection->bULCAccessRightsRetrieved != true)
   {
      PDebugError("PMifareULGetAccessRights : called without prior call to PMifareULRetrieveAccessRights");
      return (W_ERROR_MISSING_INFO);
   }

   /* Check parameters */
   if (null == pnRights)
   {
      PDebugError("PMifareULGetAccessRights : pnRights == null");
      return (W_ERROR_BAD_PARAMETER);
   }

   nStart = pMifareConnection->pSectorSize->pDivide(nOffset);
   nStop  = pMifareConnection->pSectorSize->pDivide(nOffset + nLength - 1);

   if ((nStart >= pMifareConnection->nSectorNumber) || (nStop >= pMifareConnection->nSectorNumber))
   {
      PDebugError("PMifareULGetAccessRights : specified area goes beyond end of the tag");
      return (W_ERROR_BAD_PARAMETER);
   }

   /* Ok, all is fine, compute the access rights */

   /* compute lock access */

   * pnRights = 0;

   bIsLocked = static_PMifareIsSectorLocked(pContext, pMifareConnection, nStart);

   for (i = nStart + 1; i <= nStop; i++)
   {
      if (static_PMifareIsSectorLocked(pContext, pMifareConnection, i) != bIsLocked)
      {
         /* The locks are not consistent through the whole area */
         return (W_ERROR_HETEROGENEOUS_DATA);
      }
   }

   if (pMifareConnection->nType == W_PROP_MIFARE_UL_C)
   {
      /* Mifare UL-C */
      if ((nStart < pMifareConnection->nAuthenticateThreshold) && (pMifareConnection->nAuthenticateThreshold <= nStop))
      {
         /* the authentication access is not consistent through all the whole area */
         return (W_ERROR_HETEROGENEOUS_DATA);
      }

      if (nStop < pMifareConnection->nAuthenticateThreshold)
      {
         /* we are under the authenticate threshold */

         * pnRights |= W_MIFARE_UL_READ_OK;

         if (bIsLocked == true)
         {
            * pnRights |= (W_MIFARE_UL_WRITE_LOCKED << 4);
         }
         else
         {
            * pnRights |= (W_MIFARE_UL_WRITE_OK << 4);
         }
      }
      else
      {
         /* we are above the authenticate threshold */

         if (pMifareConnection->nAuthenticateMode & 0x01)
         {
            * pnRights |= W_MIFARE_UL_READ_OK;
         }
         else
         {
            * pnRights |= W_MIFARE_UL_READ_AUTHENTICATED;
         }

         if (bIsLocked == true)
         {
            * pnRights |= (W_MIFARE_UL_WRITE_LOCKED << 4);
         }
         else
         {
            * pnRights |= (W_MIFARE_UL_WRITE_AUTHENTICATED << 4);
         }
      }
   }
   else
   {
      /* Mifare UL */

      * pnRights |= W_MIFARE_UL_READ_OK;

      if (bIsLocked)
      {
         * pnRights |= (W_MIFARE_UL_WRITE_LOCKED << 4);
      }
      else
      {
         * pnRights |= W_MIFARE_UL_WRITE_OK;
      }
   }

   return (W_SUCCESS);
}

/* ------------------------------------------------------ */

/* See client API */
void PMifareULCSetAccessRights (
      tContext * pContext,
      W_HANDLE hConnection,
      tPBasicGenericCallbackFunction *pCallback,
      void *pCallbackParameter,
      const uint8_t * pKey,
      uint32_t nKeyLength,
      uint8_t nThreshold,
      uint32_t nRights,
      bool bLockConfiguration)
{
   tMifareConnection* pMifareConnection = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;
   uint32_t i;

   PDebugTrace("PMifareULCSetAccessRights");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMifareULSetAccessRights : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   /* Only Mifare UL-C are supported */
   if (pMifareConnection->nType != W_PROP_MIFARE_UL_C)
   {
      PDebugError("PMifareULCSetAccessRights : not a Mifare UL-C connection");
      nError =  W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if (pMifareConnection->bOperationPending != false)
   {
      PDebugError("PMifareULSetAccessRights: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check if access rights have been retrieved */
   if (pMifareConnection->bULCAccessRightsRetrieved != true)
   {
      PDebugError("PMifareULSetAccessRights : called without prior call to PMifareULRetrieveAccessRights");
      nError = W_ERROR_MISSING_INFO;
      goto return_error;
   }

   /* check the key pointer */
   if (pKey == null)
   {
      PDebugError("PMifareULSetAccessRights : pKey is null");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if(nKeyLength != 16)
   {
      PDebugError("PMifareULSetAccessRights : wrong key length");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   for (i=0; i<8; i++)
   {
      pMifareConnection->aKey[i]   = pKey[7-i];
      pMifareConnection->aKey[8+i] = pKey[15-i];
   }

   /* check the Threshold value */
   if ((nThreshold < P_MIFARE_FIRST_DATA_BLOCK) || (nThreshold > P_MIFARE_BLOCK_NUMBER_UL_C))
   {
      PDebugError("PMifareULSetAccessRights : invalid threashold value");
      nError =  W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   pMifareConnection->nRequestedAuthenticateThreshold = nThreshold;

   /* Check the rigths value */
   switch (nRights)
   {
      case W_MIFARE_UL_READ_OK | (W_MIFARE_UL_WRITE_AUTHENTICATED << 4) :
         /* write authentication */
         pMifareConnection->nRequestedAuthenticateMode = 0x01;
         break;

      case W_MIFARE_UL_READ_AUTHENTICATED | (W_MIFARE_UL_WRITE_AUTHENTICATED << 4) :
         /* both read and write authentication */
         pMifareConnection->nRequestedAuthenticateMode = 0x00;
         break;

      default :
         PDebugError("PMifareULSetAccessRights : invalid access rights value");
         nError = W_ERROR_BAD_PARAMETER;
         goto return_error;
   }

   pMifareConnection->bLockAuthentication = bLockConfiguration;

   /* store the callback context */
   pMifareConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMifareConnection->bOperationPending = true;

   /* increment the ref count to avoid prematurely freeing of the connection */
   PHandleIncrementReferenceCount(pMifareConnection);

   /* set the current opeation state */
   pMifareConnection->nCurrentOperationState = 0;

   /* all checks are done, perform the operation */
   static_PMifareULCSetAccessRightsAutomaton(pContext, pMifareConnection, W_SUCCESS);

   return;

return_error:

   /* Send the error */
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_MIFARE, nError);
}

/* See client API */
W_ERROR WMifareULCSetAccessRightsSync (
      W_HANDLE hConnection,
      const uint8_t * pKey,
      uint32_t nKeyLength,
      uint8_t nThreshold,
      uint32_t nRights,
      bool bLockConfiguration)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMifareULCSetAccessRightsSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMifareULCSetAccessRights(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            pKey, nKeyLength,
            nThreshold,
            nRights,
            bLockConfiguration);
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* ------------------------------------------------------ */

/** access rigths configuration automaton  */
static void static_PMifareULCAuthenticateAutomaton(
      tContext * pContext,
      void     * pCallbackParameter,
      uint32_t   nDataLength,
      W_ERROR    nError)
{
   tMifareConnection* pMifareConnection = pCallbackParameter;

   if (nError == W_SUCCESS)
   {

      switch (pMifareConnection->nCurrentOperationState)
      {
         case 0 :

            /* Send the Authenticate command */

            P14P3UserExchangeData(pContext, pMifareConnection->hConnection, static_PMifareULCAuthenticateAutomaton, pMifareConnection,
                                    g_MifareULCAuthenticateCommand, sizeof(g_MifareULCAuthenticateCommand),
                                    pMifareConnection->aTempBuffer, sizeof(pMifareConnection->aTempBuffer),
                                    null, true);
            break;

         case 1 :

            /* We received the answer of the authenticate command */
            /* the format of this command is 0xAF followed by a 8 bytes random encrypted */

            if ((nDataLength == 9) && (pMifareConnection->aTempBuffer[0] == 0xAF))
            {
               /* Decrypt the received random */
               uint8_t  aTemp[16];
               uint32_t nRandom;

               CMemoryFill(pMifareConnection->aIvec, 0, 8);
               PCrypto3DesDecipherCbc(pMifareConnection->aTempBuffer + 1, pMifareConnection->aKey, pMifareConnection->aKey + 8, pMifareConnection->aKey, pMifareConnection->aIvec, & aTemp[7]);
               aTemp[15] = aTemp[7];

               /* Generate a 8 bytes random value */
               /* @todo here, if the IOCTL failed, the random value will be set to zero */
               nRandom = PContextDriverGenerateRandom(pContext);
               CMemoryCopy(aTemp, &nRandom, 4);

               /* @todo here, if the IOCTL failed, the random value will be set to zero */
               nRandom = PContextDriverGenerateRandom(pContext);
               CMemoryCopy(aTemp + 4, &nRandom, 4);

               /* Save rotated random in the context for later comparison */
               CMemoryCopy(pMifareConnection->aRandom, aTemp + 1, 7);
               pMifareConnection->aRandom[7] = aTemp[0];

               PCrypto3DesCipherCbc(aTemp, pMifareConnection->aKey, pMifareConnection->aKey + 8, pMifareConnection->aKey, pMifareConnection->aIvec, pMifareConnection->aTempBuffer + 1);
               PCrypto3DesCipherCbc(aTemp + 8, pMifareConnection->aKey, pMifareConnection->aKey + 8, pMifareConnection->aKey, pMifareConnection->aIvec, pMifareConnection->aTempBuffer + 9);

               pMifareConnection->aTempBuffer[0] = 0xAF;

               P14P3UserExchangeData(pContext, pMifareConnection->hConnection, static_PMifareULCAuthenticateAutomaton, pMifareConnection,
                                    pMifareConnection->aTempBuffer, 17,
                                    pMifareConnection->aTempBuffer, sizeof(pMifareConnection->aTempBuffer),
                                    null, true);
            }
            else
            {
               PDebugError("static_PMifareULCAuthenticateAutomaton : bad authenticate answer length / format");
               nError = W_ERROR_CONNECTION_COMPATIBILITY;
               goto return_error;
            }
            break;

         case 2 :

            /* We received the answer of the second part of the authentication */
            /* the format of this command is 0x00 followed by a 8 bytes random encrypted */

            if ((nDataLength == 9) && (pMifareConnection->aTempBuffer[0] == 0x00))
            {
               /* Decrypt the received random */
               uint8_t aRandom[8];

               PCrypto3DesDecipherCbc(pMifareConnection->aTempBuffer + 1, pMifareConnection->aKey, pMifareConnection->aKey + 8, pMifareConnection->aKey, pMifareConnection->aIvec, aRandom);

               if (CMemoryCompare(aRandom, pMifareConnection->aRandom, 8) == 0)
               {
                  /* The decrypted random matches */
                  static_PMifareSendResult(pContext, pMifareConnection, W_SUCCESS);
                  return;
               }
               else
               {
                  PDebugError("static_PMifareULCAuthenticateAutomaton : bad random");
                  nError = W_ERROR_CONNECTION_COMPATIBILITY;
                  goto return_error;
               }
            }
            else
            {
               PDebugError("static_PMifareULCAuthenticateAutomaton : bad authenticate answer length / format");
               nError = W_ERROR_CONNECTION_COMPATIBILITY;
               goto return_error;
            }
            break;
      }

      pMifareConnection->nCurrentOperationState++;
      return;
   }

return_error:

   static_PMifareSendResult(pContext, pMifareConnection, nError);
}

/* See Client API */
void PMifareULCAuthenticate (
      tContext * pContext,
      W_HANDLE hConnection,
      tPBasicGenericCallbackFunction *pCallback,
      void *pCallbackParameter,
      const uint8_t * pKey,
      uint32_t nKeyLength)
{
   tMifareConnection* pMifareConnection;
   tDFCCallbackContext sCallbackContext;
   W_ERROR             nError;

   PDebugTrace("PMifareULCAuthenticate");

   /* Prepare the callback context */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* retrieve the context */
   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_MIFARE_CONNECTION, (void**)&pMifareConnection);
   if (nError != W_SUCCESS)
   {
      PDebugError("PMifareULCAuthenticate : PReaderUserGetConnectionObject returned %s", PUtilTraceError(nError));
      goto return_error;
   }

   /* Only Mifare UL-C are supported */
   if (pMifareConnection->nType != W_PROP_MIFARE_UL_C)
   {
      PDebugError("PMifareULCAuthenticate : not a Mifare UL-C connection");
      nError =  W_ERROR_CONNECTION_COMPATIBILITY;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if ( pMifareConnection->bOperationPending != false )
   {
      PDebugError("PMifareULCAuthenticate: Operation already pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* check the key pointer */
   if ( ((pKey != null) && (nKeyLength != 16)) ||
        ((pKey == null) && (nKeyLength != 0)) )
   {
      PDebugError("PMifareULCAuthenticate : wrong key length");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if (pKey != null)
   {
      CMemoryCopy(pMifareConnection->aKey, pKey, 16);
   }
   else
   {
      PDebugWarning("PMifareULCAuthenticate : pKey is null, using default key");
      CMemoryCopy(pMifareConnection->aKey, g_MifareULCDefaultKey, sizeof(g_MifareULCDefaultKey));
   }

   /* store the callback context */
   pMifareConnection->sCallbackContext = sCallbackContext;

   /* set the operation in progress flag to avoid reentrancy */
   pMifareConnection->bOperationPending = true;

   /* increment the ref count to avoid prematurely freeing of the connection */
   PHandleIncrementReferenceCount(pMifareConnection);

   /* set the current opeation state */
   pMifareConnection->nCurrentOperationState = 0;

   /* all checks are done, perform the operation */
   static_PMifareULCAuthenticateAutomaton(pContext, pMifareConnection, 0, W_SUCCESS);

   return;

return_error:

   /* Send the error */
   PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_MIFARE, nError );
}

/* See client API */
W_ERROR WMifareULCAuthenticateSync (
                  W_HANDLE hConnection,
                  const uint8_t* pKey,
                  uint32_t nKeyLength)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("WMifareULCAuthenticateSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WMifareULCAuthenticate(
            hConnection,
            PBasicGenericSyncCompletion,
            &param,
            pKey, nKeyLength);
   }

   return PBasicGenericSyncWaitForResult(&param);
}


#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
