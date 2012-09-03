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
   Contains the ISO7816-4 implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( 78164 )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* The specific SW1 response command */
#define P_7816_4_SW1_61             0x61
#define P_7816_4_SW1_6C             0x6C

/* Specific commands */
#define P_7816_4_INS_MANAGE_CHANNEL  0x70
#define P_7816_4_INS_SELECT          0xA4
#define P_7816_4_INS_SELECT_AID_P1   0x04
#define P_7816_4_INS_SELECT_AID_P2   0x00


/* Buffer maximum size */
#define P_7816_4_BUFFER_MAX_SIZE    (256 + 5)

#define P_7816_MAX_LOGICAL_CHANNEL  8

/* The logical channel status */
#define P_7816_CHANNEL_STATUS_UNUSED               0
#define P_7816_CHANNEL_STATUS_LOCKED               1
#define P_7816_CHANNEL_STATUS_OPEN_PENDING         2
#define P_7816_CHANNEL_STATUS_SELECT_AID_PENDING   3
#define P_7816_CHANNEL_STATUS_OPEN                 4
#define P_7816_CHANNEL_STATUS_CLOSE_PENDING        5
#define P_7816_CHANNEL_STATUS_CLOSE_ERROR_PENDING  6

struct __t7816Connection;

/* The structure of the logical channel */
typedef struct __tP7816LogicalChannel
{
   /* Header for the object registry */
   tHandleObjectHeader sObjectHeader;

   uint32_t nStatus;

   uint32_t nChannelId;

   W_HANDLE hLogicalChannel;

   W_HANDLE hConnection;

   struct __t7816Connection* p7816Connection;

   uint8_t * pAllocatedReceivedAPDUBuffer;
   uint32_t  nAllocatedReceivedAPDUBufferMaxLength;
   uint32_t  nResponseIndex;

   W_ERROR nCloseError;

} tP7816LogicalChannel;

/**
 * @brief   Destroyes a logical channel object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_P7816DestroyLogicalChannel(
            tContext* pContext,
            void* pObject )
{
   tP7816LogicalChannel* pLogicalChannel = (tP7816LogicalChannel*)pObject;

   if(pLogicalChannel->hConnection != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pLogicalChannel->hConnection);
      pLogicalChannel->hConnection = W_NULL_HANDLE;
   }

   if((pLogicalChannel->nStatus != P_7816_CHANNEL_STATUS_UNUSED)
   && (pLogicalChannel->nStatus != P_7816_CHANNEL_STATUS_LOCKED))
   {
      pLogicalChannel->hLogicalChannel = W_NULL_HANDLE;
      pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_LOCKED;

      if (pLogicalChannel->pAllocatedReceivedAPDUBuffer != null)
      {
         CMemoryFree(pLogicalChannel->pAllocatedReceivedAPDUBuffer);
         pLogicalChannel->pAllocatedReceivedAPDUBuffer = null;
         pLogicalChannel->nAllocatedReceivedAPDUBufferMaxLength = 0;
      }
   }
   else
   {
      CDebugAssert(pLogicalChannel->hLogicalChannel == W_NULL_HANDLE);
      CDebugAssert(pLogicalChannel->hConnection     == W_NULL_HANDLE);
   }

   return P_HANDLE_DESTROY_DONE;
}

/* Handle type for a logical channel */
tHandleType g_s7816LogicalChannel =
   { static_P7816DestroyLogicalChannel, null, null, null, null };

#define P_HANDLE_TYPE_7816_LOGICAL_CHANNEL (&g_s7816LogicalChannel)

/* The specific CLA and INS parameters for a get response command */
/* @note: is CLA equals to 0x00 applies to all cases/smart cards? */
#define P_7816_4_GETREPONSE_CLA     0x00
#define P_7816_4_GETREPONSE_INS     0xC0

#define P_7816_4_ATR_MAX_LENGTH      0x30

/* Declare an exchange APDU structure for asynchronous mode */
typedef struct __t7816Connection
{
   /* Memory handle registry */
   tHandleObjectHeader        sObjectHeader;
   /* hConnection handle */
   W_HANDLE                   hConnection;

   /* End index of the last response received */
   uint32_t                   nResponseIndex;

   /* Exchange pending flag */
   bool                       bIsExchangePending;

   /* APDU command buffer length */
   uint32_t                   nSendAPDUBufferLength;
   /* APDU response buffer */
   uint8_t*                   pReceivedAPDUBuffer;

   /* APDU response buffer maximum length */
   uint32_t                   nReceivedAPDUBufferMaxLength;

   /* Command buffer */
   uint8_t                    aReaderToCardBuffer[P_7816_4_BUFFER_MAX_SIZE];
   /* Response buffer */
   uint8_t                    aCardToReaderBuffer[P_7816_4_BUFFER_MAX_SIZE];

   /* Allocated response buffer */

   uint8_t *                  pAllocatedReceivedAPDUBuffer;
   uint32_t                   nAllocatedReceivedAPDUBufferMaxLength;
   /* Response buffer */
   uint8_t                    aInternalCardToReaderBuffer[P_7816_4_BUFFER_MAX_SIZE];
   /* Callback context */
   tDFCCallbackContext        sCallbackContext;

   /* The logical channel array */
   tP7816LogicalChannel aLogicalChannelArray[P_7816_MAX_LOGICAL_CHANNEL];
   tP7816LogicalChannel * pCurrentLogicalChannel;

   /* The AID for opennig a logical channel */
   const uint8_t* pAID;
   uint32_t nAIDLength;

   uint8_t nATRLength;
   uint8_t aATR[P_7816_4_ATR_MAX_LENGTH];


} t7816Connection;

/* Destroy connection callback */
static uint32_t static_P7816DestroyConnection(
            tContext* pContext,
            void* pObject );

/* Get properties connection callback */
static uint32_t static_P7816GetPropertyNumber(
            tContext* pContext,
            void* pObject );

/* Get properties connection callback */
static bool static_P7816GetProperties(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray );

/* Check properties connection callback */
static bool static_P7816CheckProperties(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue );

/* See tWBasicGenericDataCallbackFunction */
static void static_P7816ExchangeDataCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nDataLength,
            W_ERROR nError );

/* Handle registry 7816 type */
tHandleType g_s7816Connection = {   static_P7816DestroyConnection,
                                    null,
                                    static_P7816GetPropertyNumber,
                                    static_P7816GetProperties,
                                    static_P7816CheckProperties };

#define P_HANDLE_TYPE_7816_CONNECTION (&g_s7816Connection)

/**
 * @brief   Destroyes a 7816 connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_P7816DestroyConnection(
            tContext* pContext,
            void* pObject )
{
   t7816Connection* p7816Connection = (t7816Connection*)pObject;
   uint32_t nChannelIndex;

   PDebugTrace("static_P7816DestroyConnection");

   PDFCFlushCall(&p7816Connection->sCallbackContext);

   for(nChannelIndex = 0; nChannelIndex < P_7816_MAX_LOGICAL_CHANNEL; nChannelIndex++)
   {
      CDebugAssert(p7816Connection->aLogicalChannelArray[nChannelIndex].nStatus == P_7816_CHANNEL_STATUS_UNUSED
               ||  p7816Connection->aLogicalChannelArray[nChannelIndex].nStatus == P_7816_CHANNEL_STATUS_LOCKED);
   }

   if (p7816Connection->pAllocatedReceivedAPDUBuffer != null)
   {
      CMemoryFree(p7816Connection->pAllocatedReceivedAPDUBuffer);
   }

   CMemoryFree( p7816Connection );

   return P_HANDLE_DESTROY_DONE;
}

/**
 * @brief   Gets the 7816 connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 **/
static uint32_t static_P7816GetPropertyNumber(
            tContext* pContext,
            void* pObject)
{
   return 1;
}


/**
 * @brief   Gets the 7816 connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  pPropertyArray  The property array.
 **/
static bool static_P7816GetProperties(
            tContext* pContext,
            void* pObject,
            uint8_t* pPropertyArray )
{
   PDebugTrace("static_P7816GetProperties");

   pPropertyArray[0] = W_PROP_ISO_7816_4;

   return true;
}

/**
 * @brief   Checkes the 7816 connection properties.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object.
 *
 * @param[in]  nPropertyValue  The property value.
 **/
static bool static_P7816CheckProperties(
            tContext* pContext,
            void* pObject,
            uint8_t nPropertyValue )
{
   PDebugTrace(
      "static_P7816CheckProperties: nPropertyValue=%s (0x%02X)",
      PUtilTraceConnectionProperty(nPropertyValue), nPropertyValue  );

   if ( nPropertyValue == W_PROP_ISO_7816_4 )
   {
      return true;
   }
   else
   {
      return false;
   }
}

/**
 * Exchanges data with the card.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  p7816Connection  The 7816 connection structure.
 *
 * @param[in]  pReaderToCardBuffer  The buffer to send.
 *
 * @param[out]  phOperation  A pointer on a variable valued with the handle of the operation.
 **/

#define Min(a,b) (((a) < (b)) ? (a) : (b))

static W_HANDLE static_P7816GetCurrentHandleConnection(t7816Connection* p7816Connection)
{
   if(p7816Connection->pCurrentLogicalChannel == null)
   {
      return p7816Connection->hConnection;
   }
   else if(p7816Connection->pCurrentLogicalChannel->hConnection == W_NULL_HANDLE)
   {
      return p7816Connection->hConnection;
   }
   else
   {
      return p7816Connection->pCurrentLogicalChannel->hConnection;
   }
}

static void static_P7816ExchangeData(
            tContext* pContext,
            t7816Connection* p7816Connection,
            W_HANDLE* phOperation)
{

   W_HANDLE hConnectionUsed = static_P7816GetCurrentHandleConnection(p7816Connection);

   PHandleIncrementReferenceCount(p7816Connection);

   if (p7816Connection->pReceivedAPDUBuffer == p7816Connection->aInternalCardToReaderBuffer)
   {
      CDebugAssert(p7816Connection->pCurrentLogicalChannel != null);

      if (p7816Connection->pCurrentLogicalChannel == null)
      {
         PDebugError("static_P7816ExchangeData : logical channel operations should use logical channel buffer");
      }
   }

   P14Part4ExchangeData(
      pContext,
      hConnectionUsed,
      static_P7816ExchangeDataCompleted,
      p7816Connection,
      p7816Connection->aReaderToCardBuffer,
      p7816Connection->nSendAPDUBufferLength,
      p7816Connection->aCardToReaderBuffer,
      P_7816_4_BUFFER_MAX_SIZE,
      phOperation );
}

static W_ERROR static_P7816ComputeClassByte(uint8_t* pnClass, uint8_t nChannelId);


static W_ERROR static_P7816CopyToReceiveAPDUBuffer(
            t7816Connection * p7816Connection,
            uint8_t         * pCardToReaderBuffer,
            uint32_t          nDataLength,
            uint32_t          nRemainingDataLength)
{
   W_ERROR nError;
   tP7816LogicalChannel * pLogicalChannel = p7816Connection->pCurrentLogicalChannel;

   uint32_t * pnResponseIndex, nResponseIndex;
   uint32_t * pnAllocatedReceivedAPDUBufferMaxLength, nAllocatedReceivedAPDUBufferMaxLength;
   uint8_t  ** ppAllocatedReceivedAPDUBuffer, * pAllocatedReceivedAPDUBuffer;


   /* point to the current receive buffer */

   if (pLogicalChannel != null)
   {
      pnResponseIndex = & pLogicalChannel->nResponseIndex;
      pnAllocatedReceivedAPDUBufferMaxLength = & pLogicalChannel->nAllocatedReceivedAPDUBufferMaxLength;
      ppAllocatedReceivedAPDUBuffer = & pLogicalChannel->pAllocatedReceivedAPDUBuffer;
   }
   else
   {
      pnResponseIndex = & p7816Connection->nResponseIndex;
      pnAllocatedReceivedAPDUBufferMaxLength = & p7816Connection->nAllocatedReceivedAPDUBufferMaxLength;
      ppAllocatedReceivedAPDUBuffer = & p7816Connection->pAllocatedReceivedAPDUBuffer;
   }


   nResponseIndex = * pnResponseIndex;
   nAllocatedReceivedAPDUBufferMaxLength = * pnAllocatedReceivedAPDUBufferMaxLength;
   pAllocatedReceivedAPDUBuffer = * ppAllocatedReceivedAPDUBuffer;


   if ((nDataLength + nRemainingDataLength + nResponseIndex) <= nAllocatedReceivedAPDUBufferMaxLength)
   {
      /* Copy the response */
      CMemoryCopy( pAllocatedReceivedAPDUBuffer + nResponseIndex, pCardToReaderBuffer, nDataLength );
      nError = W_SUCCESS;
   }
   else
   {
      /* realloc a new buffer */
      uint8_t * pBuffer = CMemoryAlloc(nResponseIndex + nDataLength + nRemainingDataLength);

      if (pBuffer != null)
      {
         if (pAllocatedReceivedAPDUBuffer != null)
         {
            CMemoryCopy(pBuffer, pAllocatedReceivedAPDUBuffer, nResponseIndex);
            CMemoryFree(pAllocatedReceivedAPDUBuffer);
         }

         pAllocatedReceivedAPDUBuffer = pBuffer;
         nAllocatedReceivedAPDUBufferMaxLength = nResponseIndex + nDataLength + nRemainingDataLength;

         CMemoryCopy(pAllocatedReceivedAPDUBuffer + nResponseIndex, pCardToReaderBuffer, nDataLength);

         nError = W_SUCCESS;
      }
      else
      {
         PDebugError("static_P7816CopyToReceiveAPDUBuffer: unable to allocate a buffer; answer APDU is lost");
         nError = W_ERROR_OUT_OF_RESOURCE;
      }
   }

   if (nError == W_SUCCESS)
   {
      nResponseIndex += nDataLength;

      * pnResponseIndex = nResponseIndex;
      * pnAllocatedReceivedAPDUBufferMaxLength = nAllocatedReceivedAPDUBufferMaxLength;
      * ppAllocatedReceivedAPDUBuffer = pAllocatedReceivedAPDUBuffer;
   }

   return nError;
}

/* See tWBasicGenericDataCallbackFunction */
static void static_P7816ExchangeDataCompleted(
            tContext* pContext,
            void *pCallbackParameter,
            uint32_t nDataLength,
            W_ERROR nError)
{
   t7816Connection* p7816Connection = (t7816Connection*)pCallbackParameter;
   uint8_t* pCardToReaderBuffer = p7816Connection->aCardToReaderBuffer;
   uint32_t nChannelIndex;

   PDebugTrace("static_P7816ExchangeDataCompleted");

   /* Check the error code return */
   if ( nError != W_SUCCESS )
   {
      goto send_error;
   }

   /* Check the parameters */
   if ( nDataLength < 2 )
   {
      nError = W_ERROR_RF_COMMUNICATION;
      goto send_error;
   }

   /* Check the SW1 value */
   switch ( pCardToReaderBuffer[nDataLength - 2] )
   {
      case P_7816_4_SW1_61:
         PDebugTrace("static_P7816ExchangeDataCompleted: more data to retrieve");
         {
            uint32_t nChannelId;

            nError = static_P7816CopyToReceiveAPDUBuffer(p7816Connection, pCardToReaderBuffer, nDataLength - 2, pCardToReaderBuffer[nDataLength - 1]);
            if (nError != W_SUCCESS)
            {
               goto send_error;
            }

            /* Extract the channel id from the last APDU */
            nChannelId = 0;
            if(p7816Connection->nSendAPDUBufferLength >= 1)
            {
               uint8_t nClass = p7816Connection->aReaderToCardBuffer[0];
               if((nClass & 0x40) != 0)
               {
                  nChannelId = nClass & 0x0F;
                  nChannelId += 4;
               }
               else
               {
                  nChannelId = nClass & 0x03;
               }
            }

            /* Generate the GET_RESPONSE */

            /* Store the length of the APDU */
            p7816Connection->nSendAPDUBufferLength = 5;
            /* Fill in the command */
            p7816Connection->aReaderToCardBuffer[0] = P_7816_4_GETREPONSE_CLA;
            p7816Connection->aReaderToCardBuffer[1] = P_7816_4_GETREPONSE_INS;
            p7816Connection->aReaderToCardBuffer[2] = 0x00;
            p7816Connection->aReaderToCardBuffer[3] = 0x00;
            p7816Connection->aReaderToCardBuffer[4] = pCardToReaderBuffer[nDataLength - 1];

            /* No error expected here with a class byte of zero */
            (void)static_P7816ComputeClassByte(&p7816Connection->aReaderToCardBuffer[0], (uint8_t)nChannelId);
            static_P7816ExchangeData(pContext, p7816Connection, null);
         }
         break;

      case P_7816_4_SW1_6C:
         PDebugTrace("static_P7816ExchangeDataCompleted: wrong expected length");

         /* Resend the command with the correct Le */
         p7816Connection->aReaderToCardBuffer[p7816Connection->nSendAPDUBufferLength - 1] = pCardToReaderBuffer[nDataLength - 1];
         static_P7816ExchangeData(pContext, p7816Connection, null);
         break;

      default:

         /* Check the reponse length */
         if ( nDataLength > 0 )
         {
            nError = static_P7816CopyToReceiveAPDUBuffer(p7816Connection, pCardToReaderBuffer, nDataLength, 0);

            if (nError != W_SUCCESS)
            {
               goto send_error;
            }
         }

         p7816Connection->bIsExchangePending = false;

         if (p7816Connection->pReceivedAPDUBuffer == p7816Connection->aInternalCardToReaderBuffer)
         {
            uint32_t nLength;

            if (p7816Connection->pCurrentLogicalChannel != null)
            {
               nLength = p7816Connection->pCurrentLogicalChannel->nResponseIndex;
               if(nLength > P_7816_4_BUFFER_MAX_SIZE)
               {
                  PDebugError("static_P7816ExchangeDataCompleted: response APDU too large (%d bytes), trucated to %d bytes",
                     nLength, P_7816_4_BUFFER_MAX_SIZE);
                  nLength = P_7816_4_BUFFER_MAX_SIZE;
               }
               CMemoryCopy(p7816Connection->pReceivedAPDUBuffer, p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer, nLength);
               CMemoryFree(p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer);
               p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer = null;
               p7816Connection->pCurrentLogicalChannel->nAllocatedReceivedAPDUBufferMaxLength = 0;
               p7816Connection->pCurrentLogicalChannel->nResponseIndex = 0;
            }
            else
            {
               PDebugError("Processing logical channel operation : answer stored in main connection buffer : POSSIBLE DATA LOSS");

               nLength = p7816Connection->nResponseIndex;
               CMemoryCopy(p7816Connection->pReceivedAPDUBuffer, p7816Connection->pAllocatedReceivedAPDUBuffer, nLength);
               CMemoryFree(p7816Connection->pAllocatedReceivedAPDUBuffer);
               p7816Connection->pAllocatedReceivedAPDUBuffer = null;
               p7816Connection->nAllocatedReceivedAPDUBufferMaxLength = 0;
               p7816Connection->nResponseIndex = 0;
            }

            for(nChannelIndex = 0; nChannelIndex < P_7816_MAX_LOGICAL_CHANNEL; nChannelIndex++)
            {
               tP7816LogicalChannel* pLogicalChannel = &p7816Connection->aLogicalChannelArray[nChannelIndex];

               if(pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_CLOSE_PENDING)
               {
                  W_HANDLE hLogicalChannel = pLogicalChannel->hLogicalChannel;

                  pLogicalChannel->hLogicalChannel = W_NULL_HANDLE;
                  pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_UNUSED;
                  PHandleClose(pContext, hLogicalChannel);

                  if((nLength != 2)
                  || (p7816Connection->aInternalCardToReaderBuffer[0] != 0x90)
                  || (p7816Connection->aInternalCardToReaderBuffer[1] != 0x00))
                  {
                     PDebugError("static_P7816ExchangeDataCompleted: received %d bytes SW1=0x%02X SW2=0x%02X",
                        nLength,
                        p7816Connection->aInternalCardToReaderBuffer[0],
                        p7816Connection->aInternalCardToReaderBuffer[1]);
                     nError = W_ERROR_RF_COMMUNICATION;
                  }
                  else
                  {
                     nError = W_SUCCESS;
                  }

                  PDFCPostContext2(
                     &p7816Connection->sCallbackContext,
                     P_DFC_TYPE_ISO_7816_4,
                     nError );

                  goto return_function;
               }
               else if(pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_CLOSE_ERROR_PENDING)
               {
                  CDebugAssert(pLogicalChannel->hLogicalChannel == W_NULL_HANDLE);
                  pLogicalChannel->hLogicalChannel = W_NULL_HANDLE;
                  pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_UNUSED;

                  if((nLength != 2)
                  || (p7816Connection->aInternalCardToReaderBuffer[0] != 0x90)
                  || (p7816Connection->aInternalCardToReaderBuffer[1] != 0x00))
                  {
                     PDebugError("static_P7816ExchangeDataCompleted: received %d bytes SW1=0x%02X SW2=0x%02X",
                        nLength,
                        p7816Connection->aInternalCardToReaderBuffer[0],
                        p7816Connection->aInternalCardToReaderBuffer[1]);
                  }

                  nError = pLogicalChannel->nCloseError;
                  goto send_error;
               }
               else if(pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_OPEN_PENDING)
               {
                  if((nLength == 2)
                  && (p7816Connection->aInternalCardToReaderBuffer[0] == 0x68)
                  && (p7816Connection->aInternalCardToReaderBuffer[1] == 0x81))
                  {
                     PDebugError("static_P7816ExchangeDataCompleted: Logical channel not supported");
                     nError = W_ERROR_FEATURE_NOT_SUPPORTED;
                     goto send_error;
                  }
                  else if((nLength == 2)
                  && (p7816Connection->aInternalCardToReaderBuffer[0] == 0x6A)
                  && (p7816Connection->aInternalCardToReaderBuffer[1] == 0x81))
                  {
                     PDebugError("static_P7816ExchangeDataCompleted: Logical channel not supported");
                     nError = W_ERROR_FEATURE_NOT_SUPPORTED;
                     goto send_error;
                  }
                  else if((nLength != 3)
                  || (p7816Connection->aInternalCardToReaderBuffer[1] != 0x90)
                  || (p7816Connection->aInternalCardToReaderBuffer[2] != 0x00))
                  {
                     PDebugError("static_P7816ExchangeDataCompleted: received %d bytes SW1=0x%02X SW2=0x%02X",
                        nLength,
                        p7816Connection->aInternalCardToReaderBuffer[0],
                        p7816Connection->aInternalCardToReaderBuffer[1]);
                     nError = W_ERROR_RF_COMMUNICATION;
                     goto send_error;
                  }
                  else
                  {
                     /* Set the logical channel */
                     uint32_t nChannelId = p7816Connection->aInternalCardToReaderBuffer[0];
                     if((nChannelId < 1) || (nChannelId > 19))
                     {
                        PDebugError("static_P7816ExchangeDataCompleted: bad logical channel received %d", nChannelId);
                        nError = W_ERROR_RF_COMMUNICATION;
                        goto send_error;
                     }
                     else
                     {
                        pLogicalChannel->nChannelId = nChannelId;

                        pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_SELECT_AID_PENDING;

                        p7816Connection->aReaderToCardBuffer[0] = 0x00; /* Class is set later */
                        (void)static_P7816ComputeClassByte(&p7816Connection->aReaderToCardBuffer[0], (uint8_t)nChannelId);
                        p7816Connection->aReaderToCardBuffer[1] = P_7816_4_INS_SELECT;
                        p7816Connection->aReaderToCardBuffer[2] = 0x04; /* P1 -> select AID */
                        p7816Connection->aReaderToCardBuffer[3] = 0x00; /* P2 */
                        p7816Connection->aReaderToCardBuffer[4] = (uint8_t)p7816Connection->nAIDLength; /* Lc */
                        CMemoryCopy(&p7816Connection->aReaderToCardBuffer[5],
                           p7816Connection->pAID, p7816Connection->nAIDLength);

                        p7816Connection->nSendAPDUBufferLength = 5 + p7816Connection->nAIDLength;

                        /* Store the callback buffer */
                        p7816Connection->pReceivedAPDUBuffer            = p7816Connection->aInternalCardToReaderBuffer;

                        /* Store the connection information */
                        p7816Connection->bIsExchangePending             = true;
                        p7816Connection->nReceivedAPDUBufferMaxLength   = P_7816_4_BUFFER_MAX_SIZE;

                        p7816Connection->pCurrentLogicalChannel = pLogicalChannel;
                        static_P7816ExchangeData(pContext, p7816Connection, null);

                        goto return_function;
                     }
                  }
               }
               else if(pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_SELECT_AID_PENDING)
               {
                  W_HANDLE hLogicalChannel = W_NULL_HANDLE;

                  if((nLength >= 2)
                  && (p7816Connection->aInternalCardToReaderBuffer[nLength - 2] == 0x90)
                  && (p7816Connection->aInternalCardToReaderBuffer[nLength - 1] == 0x00))
                  {
                     nError = PHandleRegister(pContext, pLogicalChannel,
                        P_HANDLE_TYPE_7816_LOGICAL_CHANNEL, &hLogicalChannel);
                     if(nError == W_SUCCESS)
                     {
                        nError = PHandleDuplicate(pContext, p7816Connection->hConnection, &(pLogicalChannel->hConnection));

                        pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_OPEN;
                        pLogicalChannel->hLogicalChannel = hLogicalChannel;

                        if(nError == W_SUCCESS)
                        {
                           /* If the length is more than 2 bytes, copy the FCI data
                              for an access with WGetResponseAPDUData() */
                           if(nLength > 2)
                           {
                              pLogicalChannel->pAllocatedReceivedAPDUBuffer = (uint8_t*)CMemoryAlloc(nLength);
                              if(pLogicalChannel->pAllocatedReceivedAPDUBuffer == null)
                              {
                                 PDebugError("static_P7816ExchangeDataCompleted: Error allocating the FCI data buffer");
                                 PHandleClose(pContext, pLogicalChannel->hLogicalChannel);
                                 nError = W_ERROR_OUT_OF_RESOURCE;
                              }
                              else
                              {
                                 CMemoryCopy(
                                    pLogicalChannel->pAllocatedReceivedAPDUBuffer,
                                    p7816Connection->aInternalCardToReaderBuffer,
                                    nLength);
                                 pLogicalChannel->nResponseIndex = nLength;
                              }
                           }
                        }

                        if(nError == W_SUCCESS)
                        {
                           PDFCPostContext3(
                              &p7816Connection->sCallbackContext,
                              P_DFC_TYPE_ISO_7816_4,
                              hLogicalChannel,
                              nError );

                           goto return_function;
                        }
                        else
                        {
                           PDebugError("static_P7816ExchangeDataCompleted: Error returned by PHandleDuplicate()");
                           PHandleClose(pContext, pLogicalChannel->hLogicalChannel);
                        }
                     }
                  }
                  else
                  {
                     PDebugError("static_P7816ExchangeDataCompleted: Error selecting the application (received %d bytes)",
                        nLength);
                     nError = W_ERROR_ITEM_NOT_FOUND;
                  }

                  pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_CLOSE_ERROR_PENDING;
                  pLogicalChannel->nCloseError = nError;

                  p7816Connection->aReaderToCardBuffer[0] = 0x00; /* Class is computed later */
                  p7816Connection->aReaderToCardBuffer[1] = P_7816_4_INS_MANAGE_CHANNEL;
                  p7816Connection->aReaderToCardBuffer[2] = 0x80; /* P1 -> close channel id in class byte */
                  p7816Connection->aReaderToCardBuffer[3] = (uint8_t)pLogicalChannel->nChannelId; /* P2 */
                  p7816Connection->nSendAPDUBufferLength = 4;

                  (void)static_P7816ComputeClassByte(&p7816Connection->aReaderToCardBuffer[0], (uint8_t)pLogicalChannel->nChannelId);

                  /* Store the callback buffer */
                  p7816Connection->pReceivedAPDUBuffer            = p7816Connection->aInternalCardToReaderBuffer;

                  /* Store the connection information */
                  p7816Connection->bIsExchangePending             = true;
                  p7816Connection->nReceivedAPDUBufferMaxLength   = P_7816_4_BUFFER_MAX_SIZE;

                  p7816Connection->pCurrentLogicalChannel = pLogicalChannel;
                  static_P7816ExchangeData(pContext, p7816Connection, null);

                  goto return_function;
               }
            }
         }

         if (p7816Connection->pCurrentLogicalChannel != null)
         {
            if (p7816Connection->pCurrentLogicalChannel->nResponseIndex <= p7816Connection->nReceivedAPDUBufferMaxLength)
            {
               CMemoryCopy(p7816Connection->pReceivedAPDUBuffer, p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer, p7816Connection->pCurrentLogicalChannel->nResponseIndex);
               CMemoryFree(p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer);
               p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer = null;
               p7816Connection->pCurrentLogicalChannel->nAllocatedReceivedAPDUBufferMaxLength = 0;

               /* Provide the card response */
               PDFCPostContext3(
                  &p7816Connection->sCallbackContext,
                  P_DFC_TYPE_ISO_7816_4,
                  p7816Connection->pCurrentLogicalChannel->nResponseIndex,
                  W_SUCCESS );
            }
            else
            {
               /* Provide the card response */
               PDFCPostContext3(
                  &p7816Connection->sCallbackContext,
                  P_DFC_TYPE_ISO_7816_4,
                  p7816Connection->pCurrentLogicalChannel->nResponseIndex,
                  W_ERROR_BUFFER_TOO_SHORT );
            }
         }
         else
         {
            if (p7816Connection->nResponseIndex <= p7816Connection->nReceivedAPDUBufferMaxLength)
            {
               CMemoryCopy(p7816Connection->pReceivedAPDUBuffer, p7816Connection->pAllocatedReceivedAPDUBuffer, p7816Connection->nResponseIndex);
               CMemoryFree(p7816Connection->pAllocatedReceivedAPDUBuffer);
               p7816Connection->pAllocatedReceivedAPDUBuffer = null;
               p7816Connection->nAllocatedReceivedAPDUBufferMaxLength = 0;

               /* Provide the card response */
               PDFCPostContext3(
                  &p7816Connection->sCallbackContext,
                  P_DFC_TYPE_ISO_7816_4,
                  p7816Connection->nResponseIndex,
                  W_SUCCESS );
            }
            else
            {
               /* Provide the card response */
               PDFCPostContext3(
                  &p7816Connection->sCallbackContext,
                  P_DFC_TYPE_ISO_7816_4,
                  p7816Connection->nResponseIndex,
                  W_ERROR_BUFFER_TOO_SHORT );
            }
         }

         goto return_function;
   }

   goto return_function;

send_error:

   /* Store the error code */
   p7816Connection->bIsExchangePending = false;

   PDebugError("static_P7816ExchangeDataCompleted: sending error %s", PUtilTraceError(nError));

   if (p7816Connection->pCurrentLogicalChannel != null)
   {
      if (p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer)
      {
         CMemoryFree(p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer);
         p7816Connection->pCurrentLogicalChannel->pAllocatedReceivedAPDUBuffer = null;
         p7816Connection->pCurrentLogicalChannel->nAllocatedReceivedAPDUBufferMaxLength = 0;
      }
   }
   else
   {
      if (p7816Connection->pAllocatedReceivedAPDUBuffer != null)
      {
         CMemoryFree(p7816Connection->pAllocatedReceivedAPDUBuffer);
         p7816Connection->pAllocatedReceivedAPDUBuffer = null;
         p7816Connection->nAllocatedReceivedAPDUBufferMaxLength = 0;
      }
   }

   /* Send the error */
   for(nChannelIndex = 0; nChannelIndex < P_7816_MAX_LOGICAL_CHANNEL; nChannelIndex++)
   {
      tP7816LogicalChannel* pLogicalChannel = &p7816Connection->aLogicalChannelArray[nChannelIndex];

      if(pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_CLOSE_PENDING)
      {
         /* Close anyway */
         W_HANDLE hLogicalChannel = pLogicalChannel->hLogicalChannel;
         pLogicalChannel->hLogicalChannel = W_NULL_HANDLE;
         pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_UNUSED;
         PHandleClose(pContext, hLogicalChannel);

         PDFCPostContext2(
            &p7816Connection->sCallbackContext,
            P_DFC_TYPE_ISO_7816_4,
            nError );

         goto return_function;
      }
      else if((pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_OPEN_PENDING)
      || (pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_CLOSE_ERROR_PENDING))
      {
         pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_UNUSED;

         PDFCPostContext3(
            &p7816Connection->sCallbackContext,
            P_DFC_TYPE_ISO_7816_4,
            W_NULL_HANDLE,
            nError );

         goto return_function;
      }
      else if(pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_SELECT_AID_PENDING)
      {
         pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_CLOSE_ERROR_PENDING;
         pLogicalChannel->nCloseError = nError;

         p7816Connection->aReaderToCardBuffer[0] = 0x00; /* Class is computed later */
         p7816Connection->aReaderToCardBuffer[1] = P_7816_4_INS_MANAGE_CHANNEL;
         p7816Connection->aReaderToCardBuffer[2] = 0x80; /* P1 -> close channel id in class byte */
         p7816Connection->aReaderToCardBuffer[3] = (uint8_t)pLogicalChannel->nChannelId; /* P2 */
         p7816Connection->nSendAPDUBufferLength = 4;

         (void)static_P7816ComputeClassByte(&p7816Connection->aReaderToCardBuffer[0], (uint8_t)pLogicalChannel->nChannelId);

         /* Store the callback buffer */
         p7816Connection->pReceivedAPDUBuffer            = p7816Connection->aInternalCardToReaderBuffer;

         /* Store the connection information */
         p7816Connection->bIsExchangePending             = true;
         p7816Connection->nReceivedAPDUBufferMaxLength   = P_7816_4_BUFFER_MAX_SIZE;

         p7816Connection->pCurrentLogicalChannel = pLogicalChannel;
         static_P7816ExchangeData(pContext, p7816Connection, null);

         goto return_function;
      }
   }

   PDFCPostContext3(
      &p7816Connection->sCallbackContext,
      P_DFC_TYPE_ISO_7816_4,
      0,
      nError );

return_function:

   PHandleDecrementReferenceCount(pContext, p7816Connection);
}

/**
 * @brief   Stores the ATR.
 *
 * @param[in]  p7816Connection  The 7816-4 connection.
 *
 * @param[in]  pBuffer The buffer with the ATR extra data.
 *
 * @param[in]  nLength The buffer length.
 **/
static void static_P7816StoreATR(
            t7816Connection* p7816Connection,
            const uint8_t* pBuffer,
            uint8_t nLength )
{
   uint8_t nXor;
   uint32_t i;

   p7816Connection->aATR[0] = 0x3B;
   p7816Connection->aATR[1] = 0x80 | nLength; /* T0 */
   p7816Connection->aATR[2] = 0x80;
   p7816Connection->aATR[3] = 0x01;

   /* Copy the historical bytes */
   if ( nLength != 0 )
   {
      /* Header + Historical bytes + TCK */
      CDebugAssert(nLength < (P_7816_4_ATR_MAX_LENGTH -4 -1));

      /* Store the higher layer response as historical bytes */
      CMemoryCopy(
         &p7816Connection->aATR[4],
         pBuffer, nLength );
   }
   p7816Connection->nATRLength = 4 + nLength + 1;

   /* Calculate the Exclusive-OR, starting at "T0" position (skip header 0x3B) */

   for ( i=1, nXor=0 ; i < (uint32_t)(p7816Connection->nATRLength -1) ; i++ )
   {
      nXor ^= p7816Connection->aATR[i];
   }

   p7816Connection->aATR[p7816Connection->nATRLength -1] = nXor;

   PDebugTrace("static_P7816StoreATR: ATR");
   PDebugTraceBuffer( p7816Connection->aATR, p7816Connection->nATRLength );
}

/* See Header file */
void P7816CreateConnection(
            tContext* pContext,
            W_HANDLE hUserConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nConnectionProperty )
{
   tDFCCallbackContext sCallbackContext;
   t7816Connection* p7816Connection;
   W_ERROR nError;
   uint32_t nChannelIndex;

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Create the 7816-4 buffer */
   p7816Connection = (t7816Connection*)CMemoryAlloc( sizeof(t7816Connection) );
   if ( p7816Connection == null )
   {
      PDebugError("static_P7816CreateConnection: p7816Connection == null");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }
   CMemoryFill(p7816Connection, 0, sizeof(t7816Connection));

   /* Add the 7816-4 structure */
   if ( ( nError = PHandleAddHeir(
                     pContext,
                     hUserConnection,
                     p7816Connection,
                     P_HANDLE_TYPE_7816_CONNECTION ) ) != W_SUCCESS )
   {
      PDebugError("static_P7816CreateConnection: could not add the 7816-4 buffer");
      CMemoryFree(p7816Connection);
      goto return_error;
   }

   /* Store the connection handle */
   p7816Connection->hConnection = hUserConnection;

   for(nChannelIndex = 0; nChannelIndex < P_7816_MAX_LOGICAL_CHANNEL; nChannelIndex++)
   {
      p7816Connection->aLogicalChannelArray[nChannelIndex].nStatus = P_7816_CHANNEL_STATUS_UNUSED;
      p7816Connection->aLogicalChannelArray[nChannelIndex].p7816Connection = p7816Connection;
   }

   /* Compute the ATR */
   if(PHandleCheckProperty(pContext, hUserConnection, W_PROP_ISO_14443_4_A) == W_SUCCESS)
   {
      tW14Part4ConnectionInfo sInfo;

      nError = P14Part4GetConnectionInfo(pContext, hUserConnection, &sInfo);
      if(nError != W_SUCCESS)
      {
         PDebugError("static_P7816CreateConnection: could not get the connection properties type A 4");
         goto return_error;
      }

      static_P7816StoreATR(p7816Connection,
            sInfo.sW14TypeA.aApplicationData,
            sInfo.sW14TypeA.nApplicationDataLength );
   }
   else if(PHandleCheckProperty(pContext, hUserConnection, W_PROP_ISO_14443_4_B) == W_SUCCESS)
   {
      tW14Part3ConnectionInfo sInfo;
      uint8_t aHistoricalBytes[8];

      nError = P14Part3GetConnectionInfo(pContext, hUserConnection, &sInfo);
      if(nError != W_SUCCESS)
      {
         PDebugError("static_P7816CreateConnection: could not get the connection properties type B 3");
         goto return_error;
      }

      aHistoricalBytes[0] = sInfo.sW14TypeB.aATQB[5];
      aHistoricalBytes[1] = sInfo.sW14TypeB.aATQB[6];
      aHistoricalBytes[2] = sInfo.sW14TypeB.aATQB[7];
      aHistoricalBytes[3] = sInfo.sW14TypeB.aATQB[8];

      aHistoricalBytes[4] = sInfo.sW14TypeB.aATQB[9];
      aHistoricalBytes[5] = sInfo.sW14TypeB.aATQB[10];
      aHistoricalBytes[6] = sInfo.sW14TypeB.aATQB[11];

      aHistoricalBytes[7] = sInfo.sW14TypeB.nMBLI_CID & 0xF0;

      static_P7816StoreATR(p7816Connection, aHistoricalBytes, 8 );
   }
   else
   {
      /* No info available */
      static_P7816StoreATR(p7816Connection, null, 0 );
   }

   nError = W_SUCCESS;

return_error:

   if(nError != W_SUCCESS)
   {
      PDebugError("P7816CreateConnection sending error %s", PUtilTraceError(nError));
   }

   PDFCPostContext2(
      &sCallbackContext,
      P_DFC_TYPE_ISO_7816_4,
      nError );
}

/* See Client API Specifications */
W_ERROR P7816GetATRSize(
            tContext* pContext,
            W_HANDLE hConnection,
            uint32_t* pnSize )
{
   t7816Connection* p7816Connection;
   W_ERROR nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_7816_CONNECTION, (void**)&p7816Connection);

   if ( nError == W_SUCCESS )
   {
      /* Check the parameters */
      if ( pnSize == null )
      {
         PDebugError("P7816GetATRSize: W_ERROR_BAD_PARAMETER");
         nError = W_ERROR_BAD_PARAMETER;
      }
      else
      {
         *pnSize = p7816Connection->nATRLength;
      }
   }
   else
   {
      PDebugError("P7816GetATRSize: could not get p7816Connection buffer");
      if ( pnSize != null )
      {
         *pnSize = 0;
      }
   }

   return nError;
}

/* See Client API Specifications */
W_ERROR P7816GetATR(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pBuffer,
            uint32_t nBufferMaxLength,
            uint32_t* pnActualLength)
{
   t7816Connection* p7816Connection;
   W_ERROR nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_7816_CONNECTION, (void**)&p7816Connection);

   if ( pnActualLength != null )
   {
      *pnActualLength = 0;
   }

   if ( nError != W_SUCCESS )
   {
      PDebugError("P7816GetATR: could not get p7816Connection buffer");
   }
   else if (( pBuffer == null )
         || ( pnActualLength == null ) )
   {
      PDebugError(" P7816GetATR: W_ERROR_BAD_PARAMETER");
      nError = W_ERROR_BAD_PARAMETER;
   }
   else if ( nBufferMaxLength < p7816Connection->nATRLength)
   {
      PDebugError(
         "P7816GetATR: Buffer to small (0x%08X)",
         *pnActualLength );
      nError = W_ERROR_BUFFER_TOO_SHORT;
   }
   else
   {
      *pnActualLength = p7816Connection->nATRLength;
      CMemoryCopy(pBuffer, p7816Connection->aATR, p7816Connection->nATRLength);
   }

   return nError;
}

/**
 * Computes the class byte from a source value and a channel number.
 *
 * @param[inout]  pnClass  A pointer on a value containing the original CLASS byte
 *                and set with the updated value.
 *
 * @param[in]     nChannelId  The logical channel identifier.
 *
 * @return  The error code.
 **/
static W_ERROR static_P7816ComputeClassByte(uint8_t* pnClass, uint8_t nChannelId)
{
   uint8_t nClass = 0x00;
   uint8_t nSecureMessaging = 0x00;
   uint8_t nSourceClass = *pnClass;

   if(nSourceClass == 0x94)
   {
      /* Special case for the Calypso application */
      if(nChannelId != 0)
      {
         PDebugError("P7816ExchangeAPDU: CLASS byte 0x94 can only be used with the base logical channel");
         return W_ERROR_RF_COMMUNICATION;
      }

      return W_SUCCESS;
   }
   else if((nSourceClass & 0x40) == 0x00)
   {
      if(nChannelId > 19)
      {
         PDebugError("P7816ExchangeAPDU: Invalid channel value");
         return W_ERROR_RF_COMMUNICATION;
      }

      nSecureMessaging = (nSourceClass & 0x0C) >> 2;
   }
   else
   {
      if(nChannelId > 19)
      {
         PDebugError("P7816ExchangeAPDU: Invalid channel value");
         return W_ERROR_RF_COMMUNICATION;
      }

      nSecureMessaging = (nSourceClass & 0x20) >> 5;
      if(nSecureMessaging != 0)
      {
         nSecureMessaging = 2;
      }
   }

   if(nChannelId <= 3)
   {
      nClass = (nSourceClass & 0x90) | (nSecureMessaging << 2) | nChannelId;
   }
   else
   {
      if((nSecureMessaging != 0) && (nSecureMessaging != 2))
      {
         PDebugError("P7816ExchangeAPDU: This type of secure messaging is not supported with channel >= 4");
         return W_ERROR_RF_COMMUNICATION;
      }

      if(nSecureMessaging == 2)
      {
         nSecureMessaging = 1;
      }

      CDebugAssert(nChannelId <= 19);
      nClass = 0x40 | (nSourceClass & 0x90) | (nSecureMessaging << 5) | (nChannelId - 4);
   }

   *pnClass = nClass;
   return W_SUCCESS;
}

/* See Client API Specifications */
void P7816ExchangeAPDU(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericDataCallbackFunction* pCallback,
            void* pCallbackParameter,
            const uint8_t* pSendAPDUBuffer,
            uint32_t nSendAPDUBufferLength,
            uint8_t* pReceivedAPDUBuffer,
            uint32_t nReceivedAPDUBufferMaxLength,
            W_HANDLE* phOperation )
{
   t7816Connection* p7816Connection;
   tP7816LogicalChannel* pLogicalChannel = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDFCFillCallbackContext(
                  pContext,
                  (tDFCCallback*)pCallback,
                  pCallbackParameter,
                  &sCallbackContext );

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_7816_CONNECTION, (void**)&p7816Connection);
   if ( nError != W_SUCCESS )
   {
      p7816Connection = null;
      nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_7816_LOGICAL_CHANNEL, (void**)&pLogicalChannel);
      if ( nError != W_SUCCESS )
      {
         goto return_error;
      }
      p7816Connection = pLogicalChannel->p7816Connection;
   }

   /* Check the parameters */
   if (  (  ( pSendAPDUBuffer == null )
         && ( nSendAPDUBufferLength != 0 ) )
      || (  ( pSendAPDUBuffer != null )
         && ( nSendAPDUBufferLength == 0 ) )
      || (  ( pReceivedAPDUBuffer == null )
         && ( nReceivedAPDUBufferMaxLength != 0 ) )
      || (  ( pReceivedAPDUBuffer != null )
         && ( nReceivedAPDUBufferMaxLength == 0 ) ) )
   {
      PDebugError("P7816ExchangeAPDU: bad buffer parameter");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* Filetering the MANAGE CHANNEL APDU */
   if(nSendAPDUBufferLength >= 2)
   {
      if(pSendAPDUBuffer[1] == P_7816_4_INS_MANAGE_CHANNEL)
      {
         PDebugError("P7816ExchangeAPDU: MANAGE CHANNEL APDU rejected");
         nError = W_ERROR_RF_COMMUNICATION;
         goto return_error;
      }
   }

   if(pLogicalChannel != null)
   {
      /* Filetering the SELECT AID APDU */
      if(nSendAPDUBufferLength >= 3)
      {
         if((pSendAPDUBuffer[1] == P_7816_4_INS_SELECT)
         && (pSendAPDUBuffer[2] == P_7816_4_INS_SELECT_AID_P1))
         {
            PDebugError("P7816ExchangeAPDU: SELECT AID APDU rejected");
            nError = W_ERROR_RF_COMMUNICATION;
            goto return_error;
         }
      }

      if(pLogicalChannel->nStatus != P_7816_CHANNEL_STATUS_OPEN)
      {
         PDebugError("P7816ExchangeAPDU: Error bad logical channel status");
         nError = W_ERROR_BAD_STATE;
         goto return_error;
      }
   }

   /* Check if an operation is still pending */
   if ( p7816Connection->bIsExchangePending != false )
   {
      PDebugError("P7816ExchangeAPDU: Error send APDU pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   if (nSendAPDUBufferLength > P_7816_4_BUFFER_MAX_SIZE)
   {
      PDebugError("P7816ExchangeAPDU: Error APDU too long");
      nError = W_ERROR_BUFFER_TOO_LARGE;
      goto return_error;
   }

   if (pLogicalChannel != null)
   {
      if (pLogicalChannel->pAllocatedReceivedAPDUBuffer != null)
      {
         PDebugError("P7816ExchangeAPDU : new APDU exchange without retreive of the pending DATA. last APDU response is LOST");
         CMemoryFree(pLogicalChannel->pAllocatedReceivedAPDUBuffer);
         pLogicalChannel->pAllocatedReceivedAPDUBuffer = null;
         pLogicalChannel->nAllocatedReceivedAPDUBufferMaxLength = 0;
      }

      pLogicalChannel->nResponseIndex = 0;
   }
   else
   {
      if (p7816Connection->pAllocatedReceivedAPDUBuffer != null)
      {
         PDebugError("P7816ExchangeAPDU : new APDU exchange without retreive of the pending DATA. last APDU response is LOST");
         CMemoryFree(p7816Connection->pAllocatedReceivedAPDUBuffer);
         p7816Connection->pAllocatedReceivedAPDUBuffer = null;
         p7816Connection->nAllocatedReceivedAPDUBufferMaxLength = 0;
      }

      p7816Connection->nResponseIndex = 0;
   }

   p7816Connection->pCurrentLogicalChannel = pLogicalChannel;

   CMemoryCopy(p7816Connection->aReaderToCardBuffer, pSendAPDUBuffer, nSendAPDUBufferLength);
   p7816Connection->nSendAPDUBufferLength = nSendAPDUBufferLength;

   if(nSendAPDUBufferLength >= 1)
   {
      uint32_t nChannelId = 0;
      if(pLogicalChannel != null)
      {
         nChannelId = pLogicalChannel->nChannelId;
      }
      nError = static_P7816ComputeClassByte(&p7816Connection->aReaderToCardBuffer[0], (uint8_t)nChannelId);

      if ( nError != W_SUCCESS )
      {
         goto return_error;
      }
   }

   /* Store the callback context */
   p7816Connection->sCallbackContext               = sCallbackContext;
   p7816Connection->bIsExchangePending             = true;

   /* Store the callback buffer */
   p7816Connection->pReceivedAPDUBuffer            = pReceivedAPDUBuffer;
   p7816Connection->nReceivedAPDUBufferMaxLength   = nReceivedAPDUBufferMaxLength;

   static_P7816ExchangeData(pContext, p7816Connection, phOperation);

   return;

return_error:

   PDebugError("P7816ExchangeAPDU: returning error %s", PUtilTraceError(nError));
   PDFCPostContext3(
      &sCallbackContext,
      P_DFC_TYPE_ISO_7816_4,
      0,
      nError );
}

W_ERROR P7816GetResponseAPDUData(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t *  pReceivedAPDUBuffer,
            uint32_t   nReceivedAPDUBufferMaxLength,
            uint32_t * pnReceivedAPDUActualLength )
{
   t7816Connection* p7816Connection;
   tP7816LogicalChannel* pLogicalChannel = null;
   uint8_t * pBuffer;
   uint32_t  nLength;

   W_ERROR nError;

   if (pnReceivedAPDUActualLength != null)
   {
      * pnReceivedAPDUActualLength = 0;
   }

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_7816_CONNECTION, (void**)&p7816Connection);

   if ( nError != W_SUCCESS )
   {
      nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_7816_LOGICAL_CHANNEL, (void**)&pLogicalChannel);

      if ( nError != W_SUCCESS )
      {
         return (nError);
      }
   }

   if ((pReceivedAPDUBuffer == null) || (pnReceivedAPDUActualLength == null))
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   if (p7816Connection != null)
   {
      if (p7816Connection->pAllocatedReceivedAPDUBuffer == null)
      {
         /* no data pending, return W_SUCCESS,
            the pnReceivedAPDUActualLength is set to zero */
         return W_SUCCESS;
      }

      pBuffer = p7816Connection->pAllocatedReceivedAPDUBuffer;
      nLength = p7816Connection->nResponseIndex;
   }
   else
   {
      pBuffer = pLogicalChannel->pAllocatedReceivedAPDUBuffer;
      nLength = pLogicalChannel->nResponseIndex;

      if (pBuffer == null)
      {
         /* no data pending, return W_SUCCESS,
            the pnReceivedAPDUActualLength is set to zero */
         return W_SUCCESS;
      }
   }

   if (nReceivedAPDUBufferMaxLength < nLength)
   {
      * pnReceivedAPDUActualLength = nLength;
      return W_ERROR_BUFFER_TOO_SHORT;
   }

   CMemoryCopy(pReceivedAPDUBuffer, pBuffer, nLength);

   * pnReceivedAPDUActualLength = nLength;

   if (p7816Connection != null)
   {
      CMemoryFree(p7816Connection->pAllocatedReceivedAPDUBuffer);
      p7816Connection->pAllocatedReceivedAPDUBuffer = null;
      p7816Connection->nAllocatedReceivedAPDUBufferMaxLength = 0;
   }
   else
   {
      CMemoryFree(pLogicalChannel->pAllocatedReceivedAPDUBuffer);
      pLogicalChannel->pAllocatedReceivedAPDUBuffer = null;
      pLogicalChannel->nAllocatedReceivedAPDUBufferMaxLength = 0;
   }

   return W_SUCCESS;
}



/* See Client API Specifications */
W_ERROR W7816ExchangeAPDUSync(
            W_HANDLE hConnection,
            const uint8_t* pSendAPDUBuffer,
            uint32_t nSendAPDUBufferLength,
            uint8_t* pReceivedAPDUBuffer,
            uint32_t nReceivedAPDUBufferMaxLength,
            uint32_t* pnReceivedAPDUActualLength )
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("W7816ExchangeAPDUSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      W7816ExchangeAPDU(
            hConnection,
            PBasicGenericSyncCompletionUint32,
            &param,
            pSendAPDUBuffer,
            nSendAPDUBufferLength,
            pReceivedAPDUBuffer,
            nReceivedAPDUBufferMaxLength,
            null  );
   }

   return PBasicGenericSyncWaitForResultUint32(&param, pnReceivedAPDUActualLength);
}

/* See Client API Specifications */
void P7816OpenLogicalChannel(
                  tContext* pContext,
                  W_HANDLE hConnection,
                  tPBasicGenericHandleCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  const uint8_t* pAID,
                  uint32_t nAIDLength,
                  W_HANDLE* phOperation)
{
   tDFCCallbackContext sCallbackContext;
   t7816Connection* p7816Connection = null;
   W_ERROR nError;
   uint32_t nChannelIndex;
   tP7816LogicalChannel* pLogicalChannel = null;

   PDFCFillCallbackContext(
                  pContext,
                  (tDFCCallback*)pCallback,
                  pCallbackParameter,
                  &sCallbackContext );

   if((pAID == null) || (nAIDLength == 0) || (nAIDLength >= 50))
   {
      PDebugError("P7816OpenLogicalChannel: Bad parameters");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   nError = PReaderUserGetConnectionObject(pContext, hConnection, P_HANDLE_TYPE_7816_CONNECTION, (void**)&p7816Connection);
   if ( nError != W_SUCCESS )
   {
      PDebugError("P7816OpenLogicalChannel: Bad connection handle");
      goto return_error;
   }

   /* Check if an operation is still pending */
   if ( p7816Connection->bIsExchangePending != false )
   {
      PDebugError("P7816OpenLogicalChannel: Error send APDU pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   for(nChannelIndex = 0; nChannelIndex < P_7816_MAX_LOGICAL_CHANNEL; nChannelIndex++)
   {
      pLogicalChannel = &p7816Connection->aLogicalChannelArray[nChannelIndex];
      if(pLogicalChannel->nStatus == P_7816_CHANNEL_STATUS_UNUSED)
      {
         break;
      }
   }

   if(pLogicalChannel->nStatus != P_7816_CHANNEL_STATUS_UNUSED)
   {
      PDebugError("P7816OpenLogicalChannel: No logical channel available");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_OPEN_PENDING;

   p7816Connection->aReaderToCardBuffer[0] = 0x00; /* Use class 0x00 */
   p7816Connection->aReaderToCardBuffer[1] = P_7816_4_INS_MANAGE_CHANNEL;
   p7816Connection->aReaderToCardBuffer[2] = 0x00; /* P1 -> for opening a logical channel to be numbered in the response data field */
   p7816Connection->aReaderToCardBuffer[3] = 0x00; /* P2 */
   p7816Connection->aReaderToCardBuffer[4] = 0x01; /* Le */

   p7816Connection->nSendAPDUBufferLength = 5;

   /* Store the callback context */
   p7816Connection->sCallbackContext               = sCallbackContext;
   /* Store the callback buffer */
   p7816Connection->pReceivedAPDUBuffer            = p7816Connection->aInternalCardToReaderBuffer;

   /* Store the connection information */
   pLogicalChannel->nResponseIndex                 = 0;
   p7816Connection->bIsExchangePending             = true;
   p7816Connection->nReceivedAPDUBufferMaxLength   = P_7816_4_BUFFER_MAX_SIZE;

   p7816Connection->pAID = pAID;
   p7816Connection->nAIDLength = nAIDLength;

   p7816Connection->pCurrentLogicalChannel = pLogicalChannel;
   static_P7816ExchangeData(pContext, p7816Connection, phOperation);

   return;

return_error:

   PDebugError("P7816OpenLogicalChannel: returning error %s", PUtilTraceError(nError));
   PDFCPostContext3(
      &sCallbackContext,
      P_DFC_TYPE_ISO_7816_4,
      W_NULL_HANDLE,
      nError );
}

/* See Client API Specifications */
W_ERROR W7816OpenLogicalChannelSync(
                  W_HANDLE hConnection,
                  const uint8_t* pAID,
                  uint32_t nAIDLength,
                  W_HANDLE* phLogicalChannel)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("W7816OpenLogicalChannelSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      W7816OpenLogicalChannel(
            hConnection,
            PBasicGenericSyncCompletionHandle,
            &param,
            pAID,
            nAIDLength,
            null  );
   }

   return PBasicGenericSyncWaitForResultHandle(&param, phLogicalChannel);
}

/* See Client API Specifications */
void P7816CloseLogicalChannel(
                  tContext* pContext,
                  W_HANDLE hLogicalChannel,
                  tPBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter)
{
   t7816Connection* p7816Connection = null;
   tP7816LogicalChannel* pLogicalChannel = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError;

   PDFCFillCallbackContext(
                  pContext,
                  (tDFCCallback*)pCallback,
                  pCallbackParameter,
                  &sCallbackContext );

   nError = PReaderUserGetConnectionObject(pContext, hLogicalChannel, P_HANDLE_TYPE_7816_LOGICAL_CHANNEL, (void**)&pLogicalChannel);
   if ( nError != W_SUCCESS )
   {
      goto return_error;
   }

   p7816Connection = pLogicalChannel->p7816Connection;

   if(pLogicalChannel->nStatus != P_7816_CHANNEL_STATUS_OPEN)
   {
      PDebugError("P7816CloseLogicalChannel: Error bad logical channel status");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   /* Check if an operation is still pending */
   if ( p7816Connection->bIsExchangePending != false )
   {
      PDebugError("P7816CloseLogicalChannel: Error send APDU pending");
      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   p7816Connection->aReaderToCardBuffer[0] = 0x00; /* Class is computed later */
   p7816Connection->aReaderToCardBuffer[1] = P_7816_4_INS_MANAGE_CHANNEL;
   p7816Connection->aReaderToCardBuffer[2] = 0x80; /* P1 -> close channel id in class byte */
   p7816Connection->aReaderToCardBuffer[3] = (uint8_t)pLogicalChannel->nChannelId; /* P2 */

   p7816Connection->nSendAPDUBufferLength = 4;

   (void)static_P7816ComputeClassByte(&p7816Connection->aReaderToCardBuffer[0], (uint8_t)pLogicalChannel->nChannelId);

   /* Store the callback context */
   p7816Connection->sCallbackContext               = sCallbackContext;
   /* Store the callback buffer */
   p7816Connection->pReceivedAPDUBuffer            = p7816Connection->aInternalCardToReaderBuffer;

   /* Store the connection information */
   pLogicalChannel->nResponseIndex                 = 0;
   p7816Connection->bIsExchangePending             = true;
   p7816Connection->nReceivedAPDUBufferMaxLength   = P_7816_4_BUFFER_MAX_SIZE;

   pLogicalChannel->nStatus = P_7816_CHANNEL_STATUS_CLOSE_PENDING;
   p7816Connection->pCurrentLogicalChannel = pLogicalChannel;
   static_P7816ExchangeData(pContext, p7816Connection, null);
   return;

return_error:

   PDebugError("P7816CloseLogicalChannel: returning error %s", PUtilTraceError(nError));
   PDFCPostContext2(
      &sCallbackContext,
      P_DFC_TYPE_ISO_7816_4,
      nError );
}

/* See Client API Specifications */
W_ERROR W7816CloseLogicalChannelSync(
                  W_HANDLE hLogicalChannel)
{
   tPBasicGenericSyncParameters param;

   PDebugTrace("W7816CloseLogicalChannelSync");

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      W7816CloseLogicalChannel(
            hLogicalChannel,
            PBasicGenericSyncCompletion,
            &param );
   }

   return PBasicGenericSyncWaitForResult(&param);
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

