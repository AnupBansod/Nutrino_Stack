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
   Contains the NDEF Type 1 API implementation.
*******************************************************************************/
#define P_MODULE P_MODULE_DEC( NDEFA1 )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* NFC Type 1 Tag defines */
#define P_NDEF_1_MAPPING_VERSION             0x10       /* 1.0 */
#define P_NDEF_1_BLOCK_SIZE                  0x08
#define P_NDEF_1_MAGIC_NUMBER                0xE1

#define P_NDEF_1_CC_OFFSET                   8
#define P_NDEF_1_CC_LENGTH                   4
#define P_NDEF_1_TLV_OFFSET                  12

#define P_NDEF_1_TLV_NULL                    0x00
#define P_NDEF_1_TLV_LOCK_CONTROL            0x01
#define P_NDEF_1_TLV_LOCK_CONTROL_LENGTH        3
#define P_NDEF_1_TLV_MEMORY_CONTROL          0x02
#define P_NDEF_1_TLV_MEMORY_CONTROL_LENGTH      3
#define P_NDEF_1_TLV_NDEF_MESSAGE            0x03
#define P_NDEF_1_TLV_PROPRIETARY             0xFD
#define P_NDEF_1_TLV_TERMINATOR              0xFE

/* Tag 1 states */
#define P_NDEF_1_STATE_INITIALISED           0x00
#define P_NDEF_1_STATE_READWRITE             0x01
#define P_NDEF_1_STATE_READONLY              0x02
#define P_NDEF_1_STATE_READWRITE_PENDING     0x03
#define P_NDEF_1_STATE_READONLY_PENDING      0x04

   /* write NDEF / write NDEF length states */
#define P_NDEF_1_WRITE_START                 0x00
#define P_NDEF_1_WRITE_NM0_00                0x01
#define P_NDEF_1_WRITE_NM0_E1                0x02
#define P_NDEF_1_WRITE_NDEF                  0x03

   /* lock tag states */
#define P_NDEF_1_WRITE_CC                    0x03
#define P_NDEF_1_LOCK_TAG                    0x04

   /* move NDEF states */
#define P_NDEF_1_MOVE_START                  0x00
#define P_NDEF_1_MOVE_READ                   0x01
#define P_NDEF_1_MOVE_WRITE                  0x02

/* ------------------------------------------------------- */
/*                CONNECTION CREATION                      */
/* ------------------------------------------------------- */

static void static_PNDEFType1ReadCCCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError);
static W_ERROR static_PNDEFType1ReadCapabilityContainer(tContext* pContext, tNDEFConnection* pNDEFConnection );
static void static_PNDEFType1ReadTLVCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError );
static W_ERROR static_PNDEFType1ParseTLV(tContext* pContext, tNDEFConnection* pNDEFConnection, uint32_t nReceivedLength);

/**
 * @brief   Creates a NDEF TAG Type 1 connection
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pNDEFConnection  The PNDEF connection
 *
 * @return W_SUCCESS if the current connection is NDEF type 1 capable
 *
 * The NDEF connection is completed when the PNDEFSendError() is called
 **/
static W_ERROR static_PNDEFType1CreateConnection(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   uint32_t nMaxSize;
   W_ERROR nError;
   bool    bIsFormattable;

   PDebugTrace("static_PNDEFType1CreateConnection");

   /* Check the type */
   nError = PType1ChipUserCheckType1(
                        pContext,
                        pNDEFConnection->hConnection,
                        &nMaxSize,
                        &pNDEFConnection->sType.t1.nSectorSize,
                        &pNDEFConnection->bIsLocked,
                        &pNDEFConnection->bIsLockable,
                        &bIsFormattable,
                        pNDEFConnection->aSerialNumber,
                        &pNDEFConnection->nSerialNumberLength
                        );

   if (nError != W_SUCCESS)
   {
      if (nError != W_ERROR_CONNECTION_COMPATIBILITY)
      {
         PDebugError("static_PNDEFType1CreateConnection: not correct Type 1");
      }
      return nError;
   }

   /* Set the maximum space size */
   pNDEFConnection->nMaximumSpaceSize = nMaxSize;

   /* Read the Capability Container */
   PType1ChipRead(pContext,
                  pNDEFConnection->hConnection,
                  static_PNDEFType1ReadCCCompleted,
                  pNDEFConnection,
                  pNDEFConnection->aCCFile,
                  P_NDEF_1_CC_OFFSET, P_NDEF_1_CC_LENGTH, null);


   return W_SUCCESS;
}

/**
 * @brief Capability Container Read callback
 *
 */
static void static_PNDEFType1ReadCCCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;

   PDebugTrace("static_PNDEFType1ReadCCCompleted");

   if (nError != W_SUCCESS)
   {
      /* Send the error */
      PNDEFSendError(pContext, pNDEFConnection, nError);
      return;
   }

   /* Parse the CC contents */
   nError = static_PNDEFType1ReadCapabilityContainer(pContext, pNDEFConnection);

   if (nError != W_SUCCESS)
   {
      PNDEFSendError(pContext, pNDEFConnection, W_ERROR_CONNECTION_COMPATIBILITY);
      return;
   }

   /* Read the TLV information bytes */
   PType1ChipRead(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType1ReadTLVCompleted,
            pNDEFConnection,
            pNDEFConnection->aReceivedBuffer,
            P_NDEF_1_TLV_OFFSET,
            pNDEFConnection->nMaximumSpaceSize - P_NDEF_1_TLV_OFFSET,
            null );
}

/**
 * @brief   Parses the content of the capability container
 *
 * @return W_SUCCESS if the capability container content is valid
 */
static W_ERROR static_PNDEFType1ReadCapabilityContainer(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection )
{
   bool bWriteStatus = false;
   uint8_t nReadWriteAccess;
   uint32_t nIndex = 0;
   uint32_t nLength;

   PDebugTrace("static_PNDEFType1ReadCapabilityContainer");

   pNDEFConnection->sType.t1.nTagState = P_NDEF_1_STATE_INITIALISED;

   /* Check the NDEF identification byte */
   if ( pNDEFConnection->aCCFile[nIndex++] != P_NDEF_1_MAGIC_NUMBER )
   {
      PDebugLog("static_PNDEFType1ReadCapabilityContainer: wrong identification byte");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Mapping version */
   PDebugTrace(
      "static_PNDEFType1ReadCapabilityContainer: version %d.%d",
      (pNDEFConnection->aCCFile[nIndex] >> 4),
      (pNDEFConnection->aCCFile[nIndex] & 0x0F) );

   /* Check the mapping version */
   if ( ( P_NDEF_1_MAPPING_VERSION & 0xF0 ) < ( pNDEFConnection->aCCFile[nIndex] & 0xF0 ) )
   {
      PDebugError("static_PNDEFType1ReadCapabilityContainer: higher version");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Calculate the maximum message length */
   nLength = (pNDEFConnection->aCCFile[nIndex + 1] + 1) * P_NDEF_1_BLOCK_SIZE;
   if ( nLength > pNDEFConnection->nMaximumSpaceSize )
   {
      PDebugWarning(
         "static_PNDEFType1ReadCapabilityContainer: wrong length 0x%02X",
         nLength );
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   /* Possible size values */
   /*  - 120 bytes are indicated by 0Eh  */
   /*  - 256 bytes are indicated by 1Fh  */
   /*  - 2048 bytes are indicated by FFh  */
   /* Store the maximum file size */
   PDebugTrace(
      "static_PNDEFType1ReadCapabilityContainer: nMaximumSpaceSize 0x%04X",
      pNDEFConnection->nMaximumSpaceSize );

   /* Check the read access value */
   nReadWriteAccess = (pNDEFConnection->aCCFile[nIndex + 2] >> 4) & 0x0F;
   if (  (  ( nReadWriteAccess > 0x00 )
      && ( nReadWriteAccess <= 0x07 ) )
         || ( nReadWriteAccess == 0x0F ) )
   {
      PDebugWarning(
         "static_PNDEFType1ReadCapabilityContainer: incorrect read access 0x%02X",
         nReadWriteAccess );
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   /* Check the write access value */
   nReadWriteAccess = pNDEFConnection->aCCFile[nIndex + 2] & 0x0F;
   if (  ( nReadWriteAccess > 0x00 )
      && ( nReadWriteAccess <= 0x07 ) )
   {
      PDebugWarning(
         "static_PNDEFType1ReadCapabilityContainer: incorrect write access 0x%02X",
         nReadWriteAccess );
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   if ( nReadWriteAccess == 0x0F )
   {
      bWriteStatus = true;
      pNDEFConnection->sType.t1.nTagState = P_NDEF_1_STATE_READONLY_PENDING;
      PDebugTrace("static_PNDEFType1ReadCapabilityContainer: locked");
   }
   else
   {
      bWriteStatus = false;
      pNDEFConnection->sType.t1.nTagState = P_NDEF_1_STATE_READWRITE_PENDING;
      PDebugTrace("static_PNDEFType1ReadCapabilityContainer: not locked");
   }
   if ( pNDEFConnection->bIsLocked != bWriteStatus )
   {
      PDebugWarning("static_PNDEFType1ReadCapabilityContainer: inconsistent write access status");

      /* Inconsistency between physical lock and NDEF logical lock => consider the Tag as Read-Only, but allow Reading of the Tag */
      pNDEFConnection->bIsLocked = true;
   }

   return W_SUCCESS;
}

/**
 * @brief  TLV Read callback
 *
 */
static void static_PNDEFType1ReadTLVCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;

   PDebugTrace("static_PNDEFType1ReadTLVCompleted");

   if (nError != W_SUCCESS)
   {
      /* Send the error */
      PNDEFSendError(pContext, pNDEFConnection, nError );
      return;
   }

   if ((nError = static_PNDEFType1ParseTLV(
                              pContext,
                              pNDEFConnection,
                              pNDEFConnection->nMaximumSpaceSize - P_NDEF_1_TLV_OFFSET )) != W_SUCCESS )
   {
      /* Send the error */
      PNDEFSendError(pContext, pNDEFConnection, nError);
      return;
   }

   /* check if Lock TLV and Memory TLV have been found (for dynamic tag only) */

   /* @todo  */

   /* Check if a NDEF Message has been found */
   if ( pNDEFConnection->nNDEFId != 0xFFFF )
   {
      /* Send the result */
      PNDEFSendError(pContext, pNDEFConnection, W_SUCCESS );
   }
   else
   {
      PNDEFSendError(pContext, pNDEFConnection, W_ERROR_CONNECTION_COMPATIBILITY);
   }
   return;
}


/**
  * @brief Compute offsets dealing with the reserved memory area of a type1 tag
  *        when walking through data area
  *
  * @param[in] current position in the tag
  *
  * @param[in] value to increase
  */

static int static_PNDEFType1Increment(
      tContext* pContext,
      tNDEFConnection* pNDEFConnection,
      uint32_t idx,
      uint32_t cnt)
{
   /* add the TLV OFFSET since all checks below consider index from top of the card instead of begin of TLV area */
   idx += P_NDEF_1_TLV_OFFSET;

   while (cnt--)
   {
      idx++;

      /* Sectors 0x00, 0x0D and 0xOE are not usable for NDEF ! */
      if ((idx >= P_NDEF_1_BLOCK_SIZE * 0x0D) && (idx < P_NDEF_1_BLOCK_SIZE * 0x0F))
      {
         PDebugTrace("static_PNDEFType1Increment : skipping blocks 0x0D and 0x0E");

         idx = P_NDEF_1_BLOCK_SIZE * 0x0F;
      }

      if (pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress <= pNDEFConnection->sType.t1.nDynamicLockAddress)
      {
         if ( (idx >= pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress)  &&
              (idx < pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize))
         {
            PDebugTrace("static_PNDEFType1Increment : skipping dynamic reserved memory area");

            idx = pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize;
         }

         if ( (idx >= pNDEFConnection->sType.t1.nDynamicLockAddress)  &&
              (idx < pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize))
         {
            PDebugTrace("static_PNDEFType1Increment : skipping dynamic lock memory area");

            idx = pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize;
         }
      }
      else
      {
         if ( (idx >= pNDEFConnection->sType.t1.nDynamicLockAddress)  &&
              (idx < pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize))
         {
            PDebugTrace("static_PNDEFType1Increment : skipping dynamic lock memory area");

            idx = pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize;
         }

         if ( (idx >= pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress)  &&
              (idx < pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize))
         {
            PDebugTrace("static_PNDEFType1Increment : skipping dynamic reserved memory area");

            idx = pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize;
         }
      }
   }

   /* remove the TLV offset */
   idx -= P_NDEF_1_TLV_OFFSET;

   return idx;
}



/**
 * @brief   Parses the Type 1 Tag Message TLV blocks.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pNDEFConnection  The NDEF connection structure.
 *
 * @param[in]  nReceivedLength  The length of the received buffer.
 **/
static W_ERROR static_PNDEFType1ParseTLV(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint32_t nReceivedLength )
{
   uint32_t nIndex = 0;
   uint8_t  nLengthFieldSize;
   uint32_t nLength;
   uint32_t nPagesAddr;
   uint32_t nByteOffset;
   uint32_t nBytesPerPage;
   uint32_t nIndex2;
   uint32_t nPendingConsumedBytes  = 0;
   uint32_t nConsumedBytes = 0;

   bool     bIsNDEFTLVfound  = false;
   bool     bIsProprietaryTLVfound = false;
   bool     bIsLockControlTLVfound = false;
   bool     bIsMemoryControlTLVfound = false;

   PDebugTrace("static_PNDEFType1ParseTLV");

   /* Reset the memory and lock address */
   pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress = 0xFFFF;
   pNDEFConnection->sType.t1.nDynamicLockAddress = 0xFFFF;
   pNDEFConnection->sType.t1.nDynamicLockSize = 0;
   pNDEFConnection->sType.t1.nDynamicReservedMemorySize = 0;

   /* Sectors 0x00, 0x0D and 0xOE are not usable for NDEF !
      the NDEF TAG begins at byte 4 of sector 1 */
   pNDEFConnection->nMaximumSpaceSize = pNDEFConnection->nMaximumSpaceSize - 3 * P_NDEF_1_BLOCK_SIZE - 4;

   pNDEFConnection->nNDEFFileLength = 0;

   /* Walk through the TLV block(s) */
   while ( nIndex < nReceivedLength )
   {
      /* Check the TLV type */
      switch ( pNDEFConnection->aReceivedBuffer[nIndex] )
      {
         /* null */
         case P_NDEF_1_TLV_NULL:

            PDebugTrace("static_PNDEFType1ParseTLV: null");
            nIndex = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 1);
            nPendingConsumedBytes++;
            break;

         /* Lock Control */
         case P_NDEF_1_TLV_LOCK_CONTROL:

            PDebugTrace("static_PNDEFType1ParseTLV: Lock Control");

            /*  NDEF Message TLVs and Proprietary TLVs are present after all Lock Control TLVs and Memory Control TLVs */

            if ((bIsNDEFTLVfound == true) || (bIsProprietaryTLVfound == true))
            {
               PDebugError("static_PNDEFType1ParseTLV : lock control after NDEF or PROPRIETARRY TLV !");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2 + P_NDEF_1_TLV_LOCK_CONTROL_LENGTH);

            if (nIndex2 > nReceivedLength)
            {
               /* we've reached the end of the memory */
               PDebugError("static_PNDEFType1ParseTLV: end of tag reached");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 1);

            if (pNDEFConnection->aReceivedBuffer[nIndex2] != P_NDEF_1_TLV_LOCK_CONTROL_LENGTH)
            {
               PDebugError("static_PNDEFType1ParseTLV: wrong TLV length");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            if (bIsLockControlTLVfound == false)
            {
               bIsLockControlTLVfound = true;

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2);

               /* Position (MSB) */
               nPagesAddr =  (pNDEFConnection->aReceivedBuffer[nIndex2] & 0xF0) >> 4;
               nByteOffset = (pNDEFConnection->aReceivedBuffer[nIndex2] & 0x0F);

               /* Size (Middle) */

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 3);

               pNDEFConnection->sType.t1.nDynamicLockSize = (pNDEFConnection->aReceivedBuffer[nIndex2] + 7) / 8;

               if (pNDEFConnection->sType.t1.nDynamicLockSize == 0)
               {
                  pNDEFConnection->sType.t1.nDynamicLockSize = 256 / 8;
               }

               /* Page control (LSB) */

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 4);

               nBytesPerPage = pNDEFConnection->aReceivedBuffer[nIndex2] & 0x0F;
               nBytesPerPage = 1 << nBytesPerPage;

               pNDEFConnection->sType.t1.nLockedBytesPerBit = (pNDEFConnection->aReceivedBuffer[nIndex2] & 0xF0) >> 4;
               pNDEFConnection->sType.t1.nLockedBytesPerBit = 1 << pNDEFConnection->sType.t1.nLockedBytesPerBit;

               pNDEFConnection->sType.t1.nDynamicLockAddress = (nPagesAddr * nBytesPerPage) + nByteOffset;
            }
            else
            {
               PDebugError("static_PNDEFType1ParseTLV : several lock control TLV is not supported");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            nIndex = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2 + P_NDEF_1_TLV_LOCK_CONTROL_LENGTH );

            nConsumedBytes = nPendingConsumedBytes += 2 + P_NDEF_1_TLV_LOCK_CONTROL_LENGTH;
            break;

         /* Memory control */
         case P_NDEF_1_TLV_MEMORY_CONTROL:

            PDebugTrace("static_PNDEFType1ParseTLV: Memory Control");

            /*  NDEF Message TLVs and Proprietary TLVs are present after all Lock Control TLVs and Memory Control TLVs */

            if ((bIsNDEFTLVfound == true) || (bIsProprietaryTLVfound == true))
            {
               PDebugError("static_PNDEFType1ParseTLV : memory control after NDEF TLV !");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2 + P_NDEF_1_TLV_MEMORY_CONTROL_LENGTH);

            if (nIndex2 > nReceivedLength)
            {
               /* we've reached the end of the memory */
               PDebugError("static_PNDEFType1ParseTLV: end of tag reached");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 1);

            if (pNDEFConnection->aReceivedBuffer[nIndex2] != P_NDEF_1_TLV_MEMORY_CONTROL_LENGTH)
            {
               PDebugError("static_PNDEFType1ParseTLV: wrong TLV length");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            if (bIsMemoryControlTLVfound == false)
            {
               bIsMemoryControlTLVfound = true;

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2);

               /* Position (MSB) */
               nPagesAddr =  (pNDEFConnection->aReceivedBuffer[nIndex2] & 0xF0) >> 4;
               nByteOffset = (pNDEFConnection->aReceivedBuffer[nIndex2] & 0x0F);

               /* Size (Middle) */

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 3);

               pNDEFConnection->sType.t1.nDynamicReservedMemorySize = pNDEFConnection->aReceivedBuffer[nIndex2];

               if (pNDEFConnection->sType.t1.nDynamicReservedMemorySize == 0)
               {
                  pNDEFConnection->sType.t1.nDynamicReservedMemorySize = 256;
               }

               /* Partial page control (LSB) */

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 4);

               /* YWA :  @todo

                  we've encountered some tags (Topaz 512) where partial page control byte nibbles seem inverted
                  comparing to the NDEF Type 1 specification (e.g nBytesPerPages seems to be stored in the most significant nibble !!! )

                  According to NDEF type 1 spec, we use least significant nibble to compute nBytesPerPages,
                  but if we detect the nBytesPerPages has a value of 0 (reserved value), we consider they have been swapped and use the most
                  significant nibble instead

               */

               nBytesPerPage = pNDEFConnection->aReceivedBuffer[nIndex2] & 0x0F;

               if (nBytesPerPage == 0)
               {
                  nBytesPerPage = (pNDEFConnection->aReceivedBuffer[nIndex2] & 0xF0) >> 4;
               }

               nBytesPerPage = 1 << nBytesPerPage;

               pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress = (nPagesAddr * nBytesPerPage) + nByteOffset;
            }
            else
            {
               PDebugError("static_PNDEFType1ParseTLV : several lock control TLV is not supported");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            nIndex = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2 + P_NDEF_1_TLV_MEMORY_CONTROL_LENGTH);

            nConsumedBytes = nPendingConsumedBytes += 2 + P_NDEF_1_TLV_LOCK_CONTROL_LENGTH;
            break;

         /* NDEF Message */
         case P_NDEF_1_TLV_NDEF_MESSAGE:

            PDebugTrace("static_PNDEFType1ParseTLV: NDEF Message");

            nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 1);

            if (nIndex2 >= nReceivedLength)
            {
               PDebugError("static_PNDEFType1ParseTLV: end of tag reached");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            /* Check the length */
            if ( pNDEFConnection->aReceivedBuffer[nIndex2] < 0xFF )
            {
               nLength = pNDEFConnection->aReceivedBuffer[nIndex2];
               nLengthFieldSize = 1;
            }
            else
            {
               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 3);
               if (nIndex2 >= nReceivedLength)
               {
                  PDebugError("static_PNDEFType1ParseTLV: end of tag reached");
                  return W_ERROR_CONNECTION_COMPATIBILITY;
               }

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2);
               nLength = (pNDEFConnection->aReceivedBuffer[nIndex2] << 8);

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 3);
               nLength += (pNDEFConnection->aReceivedBuffer[nIndex2]);

               if (nLength < 0xFF)
               {
                  PDebugWarning("static_PNDEFType1ParseTLV: wrong TLV length");
               }

               nLengthFieldSize = 3;
            }

            nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 1 + nLengthFieldSize + nLength);
            if (nIndex2 > nReceivedLength)
            {
               PDebugError("static_PNDEFType1ParseTLV : end of tag reached");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            if (bIsNDEFTLVfound == false)
            {
               bIsNDEFTLVfound = true;

               pNDEFConnection->nByteLength = nLengthFieldSize;
               pNDEFConnection->nNDEFFileLength = nLength;

               if (pNDEFConnection->nNDEFFileLength !=0)
               {
                  if (pNDEFConnection->sType.t1.nTagState == P_NDEF_1_STATE_READWRITE_PENDING)
                  {
                     pNDEFConnection->sType.t1.nTagState = P_NDEF_1_STATE_READWRITE;
                  }
                  else if ((pNDEFConnection->sType.t1.nTagState == P_NDEF_1_STATE_READONLY_PENDING)&& (pNDEFConnection->bIsLocked == true))
                  {
                     pNDEFConnection->sType.t1.nTagState = P_NDEF_1_STATE_READONLY;
                  }
               }
            }

            nConsumedBytes = nPendingConsumedBytes;

            /* do not parse the TAG after the NDEF TLV */
            goto end;


         /* Proprietary and other TLVs*/

         default :

            if ((pNDEFConnection->aReceivedBuffer[nIndex]) == P_NDEF_1_TLV_PROPRIETARY)
            {
               PDebugTrace("static_PNDEFType1ParseTLV: Proprietary");
               bIsProprietaryTLVfound = true;
            }
            else
            {
               PDebugTrace("static_PNDEFType1ParseTLV: unkown TLV");
            }

            nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 1);

            if (nIndex2 >= nReceivedLength)
            {
               PDebugError("static_PNDEFType1ParseTLV: end of tag reached");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            if ( pNDEFConnection->aReceivedBuffer[nIndex2] < 0xFF )
            {
               nLength = pNDEFConnection->aReceivedBuffer[nIndex2];
               nLengthFieldSize = 1;
            }
            else
            {

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 3);

               if (nIndex2 >= nReceivedLength)
               {
                  PDebugError("static_PNDEFType1ParseTLV: end of tag reached");
                  return W_ERROR_CONNECTION_COMPATIBILITY;
               }

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 2);
               nLength = (pNDEFConnection->aReceivedBuffer[nIndex2] << 8);

               nIndex2 = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 3);
               nLength += pNDEFConnection->aReceivedBuffer[nIndex2];

               nLengthFieldSize = 3;

               if (nLength < 0xFF)
               {
                  PDebugWarning("static_PNDEFType1ParseTLV: wrong TLV length");
               }
            }

            nIndex = static_PNDEFType1Increment(pContext, pNDEFConnection, nIndex, 1 + nLengthFieldSize + nLength);

            if (nIndex > nReceivedLength)
            {
               PDebugError("static_PNDEFType1ParseTLV: end of tag reached");
               return W_ERROR_CONNECTION_COMPATIBILITY;
            }

            nConsumedBytes += 1 + nLengthFieldSize + nLength;
            break;

         /* Terminator */
         case P_NDEF_1_TLV_TERMINATOR:

            PDebugTrace("static_PNDEFType1ParseTLV: Terminator");
            goto end;
      }
   }

end:

   /* remove the lock and reserved bytes from the TAG maximum space */

   pNDEFConnection->nMaximumSpaceSize -= pNDEFConnection->sType.t1.nDynamicLockSize;
   pNDEFConnection->nMaximumSpaceSize -= pNDEFConnection->sType.t1.nDynamicReservedMemorySize;
   pNDEFConnection->nMaximumSpaceSize -= nConsumedBytes;

   pNDEFConnection->nNDEFId = (uint16_t) (pNDEFConnection->nOffset + static_PNDEFType1Increment(pContext, pNDEFConnection, 0, nConsumedBytes));

   if (bIsNDEFTLVfound == false)
   {
      if (pNDEFConnection->nMaximumSpaceSize >= 2)
      {
         /* if we did not found the NDEF TLV, use the current nMaximumSpaceSize value to compute the byte length format to be used */
         if (pNDEFConnection->nMaximumSpaceSize <= 255)
         {
            pNDEFConnection->nByteLength = 1;
         }
         else
         {
            pNDEFConnection->nByteLength = 3;
         }

         pNDEFConnection->nMaximumSpaceSize -= (1 +  pNDEFConnection->nByteLength);
         pNDEFConnection->nFreeSpaceSize = pNDEFConnection->nMaximumSpaceSize;
      }
      else
      {
         /* no way to store NDEF TLV in less than 2 bytes */
         pNDEFConnection->nFreeSpaceSize = pNDEFConnection->nMaximumSpaceSize = 0;
      }
   }
   else
   {
      pNDEFConnection->nMaximumSpaceSize -= (1 +  pNDEFConnection->nByteLength);

      if ((pNDEFConnection->nMaximumSpaceSize >= 254) && (pNDEFConnection->nByteLength == 1))
      {
         /* take account of the growing of the NDEF length */
         pNDEFConnection->nMaximumSpaceSize -= 2;
      }

      pNDEFConnection->nFreeSpaceSize  = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;
   }

   return W_SUCCESS;
}


/* ------------------------------------------------------- */
/*                COMMAND PROCESSING                       */
/* ------------------------------------------------------- */

static bool static_PNDEFType1ReadWriteAreaAutomaton(tContext* pContext, tNDEFConnection* pNDEFConnection);
static void static_PNDEFType1ReadNDEFCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError);
static void static_PNDEFType1WriteNDEFCompleted(tContext* pContext, void* pCallbackParameter, W_ERROR nError);
static void static_PNDEFType1LockAutomaton(tContext* pContext, void* pCallbackParameter, W_ERROR nError);
static void static_PNDEFType1MoveAutomaton(tContext* pContext, void* pCallbackParameter, W_ERROR nError);

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
static W_ERROR static_PNDEFType1SendCommand(
            tContext* pContext,
            tNDEFConnection* pNDEFConnection,
            uint32_t nOffset,
            uint32_t nLength )
{
   /* Send the corresponding command */

   uint8_t nRequestedByteLength;

   switch ( pNDEFConnection->nCommandType )
   {
      case P_NDEF_COMMAND_READ_NDEF:
      case P_NDEF_COMMAND_READ_NDEF_SPLIT:

         if (pNDEFConnection->nCommandType == P_NDEF_COMMAND_READ_NDEF)
         {
            PDebugTrace("static_PNDEFType1SendCommand : P_NDEF_COMMAND_READ_NDEF");
         }
         else
         {
            PDebugTrace("static_PNDEFType1SendCommand : P_NDEF_COMMAND_READ_NDEF_SPLIT");
         }

         /* pNDEFConnection->nNDEFId contains the offset of the NDEV TLV in the TLV area */


         pNDEFConnection->sType.t1.nBegin  = P_NDEF_1_TLV_OFFSET + static_PNDEFType1Increment(pContext, pNDEFConnection, 0, pNDEFConnection->nNDEFId + 1 + pNDEFConnection->nByteLength + nOffset);
         pNDEFConnection->sType.t1.nBytesToProcess = nLength;
         pNDEFConnection->sType.t1.nBytesProcessed  = 0;
         pNDEFConnection->sType.t1.nBytesPending  = 0;
         pNDEFConnection->sType.t1.pBuffer = pNDEFConnection->aReceivedBuffer;
         pNDEFConnection->sType.t1.nCurrentOperation = P_NDEF_COMMAND_READ_NDEF;

         static_PNDEFType1ReadNDEFCompleted(pContext, pNDEFConnection, W_SUCCESS);
         break;

      case P_NDEF_COMMAND_WRITE_NDEF:

         PDebugTrace("static_PNDEFType1SendCommand: P_NDEF_COMMAND_WRITE_NDEF : nOffset %d - nLength %d", nOffset, nLength);

         nRequestedByteLength = (pNDEFConnection->nUpdatedNDEFFileLength <= 254) ? 1 : 3;

         /* specific case when writing a nOffset = 0 (e.g we are replacing the content of the NDEF file)
            we directly set the nByteLength to the correct length since there's no need to keep
            previous tag content */

         if (nOffset == 0)
         {
            pNDEFConnection->nByteLength = nRequestedByteLength;
         }

         if (nRequestedByteLength == pNDEFConnection->nByteLength)
         {
            /* the NDEF file length format  is not changing :o) */

            pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_START;

            pNDEFConnection->sType.t1.nBegin = P_NDEF_1_TLV_OFFSET + static_PNDEFType1Increment(pContext, pNDEFConnection, 0, pNDEFConnection->nNDEFId + 1 + pNDEFConnection->nByteLength + nOffset);
            pNDEFConnection->sType.t1.nBytesToProcess = nLength;
            pNDEFConnection->sType.t1.nBytesProcessed  = 0;
            pNDEFConnection->sType.t1.nBytesPending  = 0;
            pNDEFConnection->sType.t1.pBuffer = pNDEFConnection->pMessageBuffer;
            pNDEFConnection->sType.t1.nCurrentOperation = P_NDEF_COMMAND_WRITE_NDEF;

            static_PNDEFType1WriteNDEFCompleted(pContext, pNDEFConnection, W_SUCCESS);
         }
         else
         {
            /* Need to change the format of the length of the NDEF file */

            PDebugTrace("static_PNDEFType1SendCommand : the NDEF size is changing...");

            /* store the write command for later processing */

            pNDEFConnection->sType.t1.nPendingWriteOffset = nOffset;
            pNDEFConnection->sType.t1.nPendingWriteLength = nLength;

            pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_MOVE_START;
            static_PNDEFType1MoveAutomaton(pContext, pNDEFConnection, W_SUCCESS);
         }
         break;

      case P_NDEF_COMMAND_WRITE_NDEF_LENGTH:

         PDebugTrace("static_PNDEFType1SendCommand: P_NDEF_COMMAND_WRITE_NDEF_LENGTH");

         pNDEFConnection->sType.t1.nBegin  = P_NDEF_1_TLV_OFFSET + pNDEFConnection->nNDEFId;

         /* Update the length of the NDEF file */
         pNDEFConnection->nNDEFFileLength = pNDEFConnection->nUpdatedNDEFFileLength;
         pNDEFConnection->nFreeSpaceSize  = pNDEFConnection->nMaximumSpaceSize - pNDEFConnection->nNDEFFileLength;

         /* we use the same format for the NDEF file length as the current one */
         if (pNDEFConnection->nByteLength == 1)
         {
            CDebugAssert(pNDEFConnection->nNDEFFileLength <= 254);

            pNDEFConnection->aSendBuffer[0] = P_NDEF_1_TLV_NDEF_MESSAGE;
            pNDEFConnection->aSendBuffer[1] = (uint8_t) pNDEFConnection->nNDEFFileLength;
            pNDEFConnection->sType.t1.nBytesToProcess = 2;
         }
         else
         {
            pNDEFConnection->aSendBuffer[0] = P_NDEF_1_TLV_NDEF_MESSAGE;
            pNDEFConnection->aSendBuffer[1] = 0xFF;
            pNDEFConnection->aSendBuffer[2] = (uint8_t) (pNDEFConnection->nNDEFFileLength >> 8);
            pNDEFConnection->aSendBuffer[3] = (uint8_t) (pNDEFConnection->nNDEFFileLength);
            pNDEFConnection->sType.t1.nBytesToProcess = 4;
         }

         pNDEFConnection->sType.t1.nBytesProcessed  = 0;
         pNDEFConnection->sType.t1.nBytesPending  = 0;
         pNDEFConnection->sType.t1.pBuffer = pNDEFConnection->aSendBuffer;

         pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_START;
         static_PNDEFType1WriteNDEFCompleted(pContext, pNDEFConnection, W_SUCCESS);
         break;

      case P_NDEF_COMMAND_LOCK_TAG:

         PDebugTrace("static_PNDEFType1SendCommand: P_NDEF_COMMAND_LOCK_TAG");

         pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_START;
         static_PNDEFType1LockAutomaton(pContext, pNDEFConnection, W_SUCCESS);
         break;

      default:
         PDebugError("static_PNDEFType1SendCommand: command 0x%02X not supported", pNDEFConnection->nCommandType);
         return W_ERROR_BAD_PARAMETER;
   }

   return W_SUCCESS;
}

/**
 * @brief  Read / write automaton, deals with skipping the reserved area during read / write operations
 *
 * @param[in] pContext The context
 *
 * @param[in] pNDEFConnection The NDEF connection
 *
 * @return true if the operation is completed, false
 */
static bool static_PNDEFType1ReadWriteAreaAutomaton(
   tContext* pContext,
   tNDEFConnection* pNDEFConnection)
{
   uint32_t nBegin;
   uint32_t nEnd;

   PDebugTrace("static_PNDEFType1ReadWriteAreaAutomaton");

   pNDEFConnection->sType.t1.nBytesProcessed += pNDEFConnection->sType.t1.nBytesPending;

   if (pNDEFConnection->sType.t1.nBytesProcessed == pNDEFConnection->sType.t1.nBytesToProcess)
   {
      /* the operation is now complete */
      return true;
   }

   nBegin = pNDEFConnection->sType.t1.nBegin;

   /* if the current read operation start belongs to a reserved memory area, skip it */

   /* Sectors 0x00, 0x0D and 0xOE are not usable for NDEF ! */
   if ((nBegin >= P_NDEF_1_BLOCK_SIZE * 0x0D) && (nBegin < P_NDEF_1_BLOCK_SIZE * 0x0F))
   {
      PDebugTrace("static_PNDEFType1Increment : skipping blocks 0x0D and 0x0E");

      nBegin = P_NDEF_1_BLOCK_SIZE * 0x0F;
   }

   if (pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress <= pNDEFConnection->sType.t1.nDynamicLockAddress)
   {
      if ( (nBegin >= pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress)  &&
           (nBegin < pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize))
      {
         PDebugTrace("static_PNDEFType1Increment : skipping dynamic reserved memory area");

         nBegin = pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize;
      }

      if ( (nBegin >= pNDEFConnection->sType.t1.nDynamicLockAddress)  &&
           (nBegin < pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize))
      {
         PDebugTrace("static_PNDEFType1Increment : skipping dynamic lock memory area");

         nBegin = pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize;
      }
   }
   else
   {
      if ( (nBegin >= pNDEFConnection->sType.t1.nDynamicLockAddress)  &&
           (nBegin < pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize))
      {
         PDebugTrace("static_PNDEFType1Increment : skipping dynamic lock memory area");

         nBegin = pNDEFConnection->sType.t1.nDynamicLockAddress + pNDEFConnection->sType.t1.nDynamicLockSize;
      }

      if ( (nBegin >= pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress)  &&
           (nBegin < pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize))
      {
         PDebugTrace("static_PNDEFType1Increment : skipping dynamic reserved memory area");

         nBegin = pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress + pNDEFConnection->sType.t1.nDynamicReservedMemorySize;
      }
   }

   /* if a reserved memory area belongs to the current read range, shorten the read range */

   nEnd = nBegin + pNDEFConnection->sType.t1.nBytesToProcess - pNDEFConnection->sType.t1.nBytesProcessed - 1;

   /* Sectors 0x00, 0x0D and 0xOE are not usable for NDEF ! */
   if ((nBegin < P_NDEF_1_BLOCK_SIZE * 0x0D) && (P_NDEF_1_BLOCK_SIZE * 0x0D <= nEnd))
   {
      PDebugTrace("static_PNDEFType1Increment : skipping blocks 0x0D and 0x0E");

      nEnd = P_NDEF_1_BLOCK_SIZE * 0x0D - 1;
   }

   if ((nBegin < pNDEFConnection->sType.t1.nDynamicLockAddress) && (pNDEFConnection->sType.t1.nDynamicLockAddress <= nEnd))
   {
      nEnd = pNDEFConnection->sType.t1.nDynamicLockAddress - 1;
   }

   if ((nBegin < pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress) && (pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress <= nEnd))
   {
      nEnd = pNDEFConnection->sType.t1.nDynamicReservedMemoryAddress - 1;
   }

   /* store the received data length since the callback do not provide this value */

   pNDEFConnection->sType.t1.nBytesPending = nEnd - nBegin + 1;

   pNDEFConnection->sType.t1.nBegin = nEnd + 1;

   switch (pNDEFConnection->sType.t1.nCurrentOperation)
   {
      case P_NDEF_COMMAND_READ_NDEF :

         PDebugTrace("static_PNDEFType1ReadWriteAreaAutomaton : read[%d-%d]", nBegin, nEnd);

         PType1ChipRead(pContext,
                     pNDEFConnection->hConnection, static_PNDEFType1ReadNDEFCompleted,
                     pNDEFConnection,
                     pNDEFConnection->sType.t1.pBuffer + pNDEFConnection->sType.t1.nBytesProcessed,
                     nBegin,
                     pNDEFConnection->sType.t1.nBytesPending,
                     null);

         break;

      case P_NDEF_COMMAND_WRITE_NDEF :

         PDebugTrace("static_PNDEFType1ReadWriteAreaAutomaton : write[%d-%d]", nBegin, nEnd);

         PType1ChipWrite(pContext,
                           pNDEFConnection->hConnection, static_PNDEFType1WriteNDEFCompleted,
                           pNDEFConnection,
                           pNDEFConnection->sType.t1.pBuffer + pNDEFConnection->sType.t1.nBytesProcessed,
                           nBegin,
                           pNDEFConnection->sType.t1.nBytesPending,
                           false,
                           null);
         break;


      case P_NDEF_COMMAND_MOVE_READ :

         PDebugTrace("static_PNDEFType1ReadWriteAreaAutomaton : MOVE : read[%d-%d]", nBegin, nEnd);

         PType1ChipRead(pContext,
                     pNDEFConnection->hConnection, static_PNDEFType1MoveAutomaton,
                     pNDEFConnection,
                     pNDEFConnection->sType.t1.pBuffer + pNDEFConnection->sType.t1.nBytesProcessed,
                     nBegin,
                     pNDEFConnection->sType.t1.nBytesPending,
                     null);

         break;

      case P_NDEF_COMMAND_MOVE_WRITE :

         PDebugTrace("static_PNDEFType1ReadWriteAreaAutomaton : MOVE : write[%d-%d]", nBegin, nEnd);

         PType1ChipWrite(pContext,
                           pNDEFConnection->hConnection, static_PNDEFType1MoveAutomaton,
                           pNDEFConnection,
                           pNDEFConnection->sType.t1.pBuffer + pNDEFConnection->sType.t1.nBytesProcessed,
                           nBegin,
                           pNDEFConnection->sType.t1.nBytesPending,
                           false,
                           null);
         break;
   }

   /* the operation is not yet complete */
   return false;
}


/* @brief   Receives the answer to a Type 1 Tag read NDEF command.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PNDEFType1ReadNDEFCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*) pCallbackParameter;
   bool bIsComplete = false;

   PDebugTrace("static_PNDEFType1ReadNDEFCompleted");

   if (nError == W_SUCCESS)
   {
      bIsComplete = static_PNDEFType1ReadWriteAreaAutomaton(pContext, pNDEFConnection);
   }

   if ((bIsComplete == true) || (nError != W_SUCCESS))
   {
      W_ERROR nError2;

      nError2 = PNDEFSendCommandCompleted(
                  pContext,
                  pNDEFConnection,
                  pNDEFConnection->sType.t1.pBuffer,
                  pNDEFConnection->sType.t1.nBytesProcessed,
                  nError);

      if (nError2 != W_SUCCESS)
      {
         /* Send the error */
         PNDEFSendError(pContext, pNDEFConnection, nError2 );
      }
   }
}

/* @brief   Receives the answer to a Type 1 Tag write NDEF command.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The value provided to the function PMifareRead() when the operation was initiated.
 *
 * @param[in]  nError  The error code of the operation.
 **/
static void static_PNDEFType1WriteNDEFCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;
   bool bIsComplete = false;

   if (nError != W_SUCCESS)
   {
      /* Call the generic callback function */
      nError2 = PNDEFSendCommandCompleted(
                  pContext,
                  pNDEFConnection,
                  null,
                  0,
                  nError );

      if (nError2 != W_SUCCESS)
      {
         /* Send the error */
         PNDEFSendError(pContext, pNDEFConnection, nError2 );
      }

      return;
   }

   switch (pNDEFConnection->sType.t1.nCommandWriteState)
   {
      case P_NDEF_1_WRITE_START :

         /* Set NM0 to 0x00 */

          pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_NM0_00;
          pNDEFConnection->sType.t1.nMagicNumber = 0x00;

          PType1ChipWrite(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType1WriteNDEFCompleted,
            pNDEFConnection,
            &pNDEFConnection->sType.t1.nMagicNumber,
            P_NDEF_1_CC_OFFSET,
            1,
            false,
            null );

          break;


      case P_NDEF_1_WRITE_NM0_00 :

         /* the NM0 has been set to zero, write the NDEF message */

         pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_NDEF;

         static_PNDEFType1ReadWriteAreaAutomaton(pContext, pNDEFConnection);

         break;

      case P_NDEF_1_WRITE_NDEF :

         bIsComplete = static_PNDEFType1ReadWriteAreaAutomaton(pContext, pNDEFConnection);

         if (bIsComplete == false)
            break;

         /* The NDEF has been written, set the NMO to 0xE1 */

         pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_NM0_E1;
         pNDEFConnection->sType.t1.nMagicNumber = 0xE1;

         PType1ChipWrite(
               pContext,
               pNDEFConnection->hConnection,
               static_PNDEFType1WriteNDEFCompleted,
               pNDEFConnection,
               &pNDEFConnection->sType.t1.nMagicNumber,
               P_NDEF_1_CC_OFFSET,
               1,
               false,
               null );

         break;

      case P_NDEF_1_WRITE_NM0_E1 :

         /* The NM0 has bet set to E1, the operation is now completed */

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

         break;
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
static void static_PNDEFType1LockAutomaton(
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

      if (nError2 != W_SUCCESS)
      {
         PNDEFSendError(pContext, pNDEFConnection, nError2 );
      }

      return;
   }

   switch (pNDEFConnection->sType.t1.nCommandWriteState)
   {
      case P_NDEF_1_WRITE_START :

         pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_CC;

         /* Set the lock flag in the CC file */
         pNDEFConnection->aCCFile[3] = 0x0F;

         PType1ChipWrite(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType1LockAutomaton,
            pNDEFConnection,
            & pNDEFConnection->aCCFile[3],
            P_NDEF_1_CC_OFFSET + 3,
            1,
            false,
            null);

         break;

      case P_NDEF_1_WRITE_CC :

         /* the CC has been set to WO, lock the TAG */

         pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_LOCK_TAG;

         PType1ChipWrite(
            pContext,
            pNDEFConnection->hConnection,
            static_PNDEFType1LockAutomaton,
            pNDEFConnection,
            null,
            0,
            0,
            true,
            null );

         break;

      case P_NDEF_1_LOCK_TAG :

         /* The TAG has been locked, the operation is now completed */

         nError2 = PNDEFSendCommandCompleted(
               pContext,
               pNDEFConnection,
               null,
               0,
               nError);

         if (nError2 != W_SUCCESS)
         {
            PNDEFSendError(pContext, pNDEFConnection, nError2 );
         }

         break;
   }
}
/**
 * @brief  NDEF move automaton, used to increase the NDEF length
 *
 * @param[in] pContext The context
 *
 * @param[in] pNDEFConnection The NDEF connection
 *
 * @return true if the operation is completed, false
 */

static void static_PNDEFType1MoveAutomaton(tContext* pContext, void* pCallbackParameter, W_ERROR nError)
{
   tNDEFConnection* pNDEFConnection = (tNDEFConnection*)pCallbackParameter;
   W_ERROR nError2;
   bool bIsComplete;

   if (nError != W_SUCCESS)
   {
      /* Call the generic callback function */
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

      return;
   }

   switch (pNDEFConnection->sType.t1.nCommandWriteState)
   {
      case P_NDEF_1_MOVE_START :

         PDebugTrace("static_PNDEFType1MoveAutomaton : start reading the area to move");

         pNDEFConnection->sType.t1.nBegin = P_NDEF_1_TLV_OFFSET + static_PNDEFType1Increment(pContext, pNDEFConnection, 0, pNDEFConnection->nNDEFId + 1 + pNDEFConnection->nByteLength);
         pNDEFConnection->sType.t1.nBytesToProcess = pNDEFConnection->nNDEFFileLength;
         pNDEFConnection->sType.t1.nBytesProcessed  = 0;
         pNDEFConnection->sType.t1.nBytesPending  = 0;
         pNDEFConnection->sType.t1.pBuffer = pNDEFConnection->aReceivedBuffer;
         pNDEFConnection->sType.t1.nCurrentOperation = P_NDEF_COMMAND_MOVE_READ;

         pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_MOVE_READ;

         static_PNDEFType1MoveAutomaton(pContext, pNDEFConnection, W_SUCCESS);
         break;

      case P_NDEF_1_MOVE_READ :

         PDebugTrace("static_PNDEFType1MoveAutomaton : reading of the area to move in progress");

         bIsComplete = static_PNDEFType1ReadWriteAreaAutomaton(pContext, pNDEFConnection);

         if (bIsComplete == true)
         {

            PDebugTrace("static_PNDEFType1MoveAutomaton : start writing of the area to move");

            pNDEFConnection->nByteLength = (pNDEFConnection->nByteLength == 1) ? 3 : 1;

            pNDEFConnection->sType.t1.nBegin  = P_NDEF_1_TLV_OFFSET + static_PNDEFType1Increment(pContext, pNDEFConnection, 0, pNDEFConnection->nNDEFId + 1 + pNDEFConnection->nByteLength);
            pNDEFConnection->sType.t1.nBytesToProcess = pNDEFConnection->nNDEFFileLength;
            pNDEFConnection->sType.t1.nBytesProcessed  = 0;
            pNDEFConnection->sType.t1.nBytesPending  = 0;
            pNDEFConnection->sType.t1.pBuffer = pNDEFConnection->aReceivedBuffer;
            pNDEFConnection->sType.t1.nCurrentOperation = P_NDEF_COMMAND_MOVE_WRITE;

            pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_MOVE_WRITE;

            static_PNDEFType1MoveAutomaton(pContext, pNDEFConnection, W_SUCCESS);
         }
         break;

      case P_NDEF_1_MOVE_WRITE :

         PDebugTrace("static_PNDEFType1MoveAutomaton : writing of the area to move in progress");

         bIsComplete = static_PNDEFType1ReadWriteAreaAutomaton(pContext, pNDEFConnection);

         if (bIsComplete == true)
         {

            PDebugTrace("static_PNDEFType1MoveAutomaton : start of the NDEF message write");

            pNDEFConnection->sType.t1.nCommandWriteState = P_NDEF_1_WRITE_START;

            pNDEFConnection->sType.t1.nBegin = P_NDEF_1_TLV_OFFSET + static_PNDEFType1Increment(pContext, pNDEFConnection, 0, pNDEFConnection->nNDEFId + 1 + pNDEFConnection->nByteLength + pNDEFConnection->sType.t1.nPendingWriteOffset);
            pNDEFConnection->sType.t1.nBytesToProcess = pNDEFConnection->sType.t1.nPendingWriteLength;
            pNDEFConnection->sType.t1.nBytesProcessed  = 0;
            pNDEFConnection->sType.t1.nBytesPending  = 0;
            pNDEFConnection->sType.t1.pBuffer = pNDEFConnection->pMessageBuffer;
            pNDEFConnection->sType.t1.nCurrentOperation = P_NDEF_COMMAND_WRITE_NDEF;

            static_PNDEFType1WriteNDEFCompleted(pContext, pNDEFConnection, W_SUCCESS);
         }

         break;
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

static W_ERROR static_PNDEFType1InvalidateCache(
   tContext* pContext,
   tNDEFConnection* pNDEFConnection,
   uint32_t nOffset,
   uint32_t nLength)
{
   W_ERROR nError;

   /* Type 1 tags provide fast access to whole chip memory using RSEG / RALL commands
      so, invalidate the whole cache instead of only some parts of the cache... */

   nError = PType1ChipUserInvalidateCache(pContext, pNDEFConnection->hConnection, 0, pNDEFConnection->nMaximumSpaceSize);



   return nError;
}


/* The NDEF type information structure */
tNDEFTypeEntry g_sPNDEFType1Info = {
   W_PROP_NFC_TAG_TYPE_1,
   static_PNDEFType1CreateConnection,
   static_PNDEFType1SendCommand,
   static_PNDEFType1InvalidateCache
};

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

