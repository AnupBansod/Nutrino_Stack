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
   Contains the NDEF Type API implementation.
*******************************************************************************/
#define P_MODULE P_MODULE_DEC( NDEF_FORMAT2 )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

typedef struct tCallbackFormat
{
   /* Callback context */
   tDFCCallbackContext        sCallbackContext;

} tCallbackFormat;

/* Mifare UL */
const uint8_t g_MifareULFormat[] = { /* CC */               0xE1, 0x10, 0x06, 0x00,
                                     /* NDEF TLV */         0x03, 0x00,
                                     /* TERMINATOR TLV */   0xFE };

/* Mifare UL-C */
const uint8_t g_MifareULCFormat[] = { /* CC */               0xE1, 0x10, 0x12, 0x00,
                                     /* NDEF TLV */         0x03, 0x00,
                                     /* TERMINATOR TLV */   0xFE };

/* My-d move */
const uint8_t g_MyDMoveFormat[]  = { /* CC */               0xE1, 0x10, 0x10, 0x00,
                                     /* NDEF TLV */         0x03, 0x00,
                                     /* TERMINATOR TLV */   0xFE };


const uint8_t g_MyDNFCFormatSLER16P[] = { /* CC */               0xE1, 0x10, 0x7E, 0x00,
                                          /* NDEF TLV */         0x03, 0xFF, 0x00, 0x00,
                                          /* TERMINATOR TLV */   0xFE };

/* My-d NFC */
const uint8_t g_MyDNFCFormatSLER32P[] = { /* CC */               0xE1, 0x10, 0xFE, 0x00,
                                          /* NDEF TLV */         0x03, 0xFF, 0x00, 0x00,
                                          /* TERMINATOR TLV */   0xFE };


/* write operation completed callback */
static void static_PNDEFFormatNDEFType2WriteCompleted (
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError )
{
   tCallbackFormat* pCallbackFormat = (tCallbackFormat*)pCallbackParameter;

   PDebugTrace("static_WType2MifareWriteCompleted nError = 0x%02x", nError);

   PDFCPostContext2( &(pCallbackFormat->sCallbackContext), P_DFC_TYPE_FORMAT2, nError );
   CMemoryFree(pCallbackFormat);
}

/** See header */
void PNDEFFormatNDEFType2(tContext* pContext,
                          W_HANDLE hConnection,
                          tPBasicGenericCallbackFunction *pCallback,
                          void *pCallbackParameter,
                          uint8_t nTagType)
{
   tCallbackFormat* pCallbackFormat = null;
   tDFCCallbackContext sCallbackContext;
   tWMyDConnectionInfo sConnectionInfo;
   W_ERROR nError;

   PDebugTrace("PNDEFFormatNDEFType2");

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   pCallbackFormat = (tCallbackFormat*)CMemoryAlloc( sizeof(tCallbackFormat) );

   if ( pCallbackFormat == null )
   {
      PDebugError("PNDEFFormatNDEFType2: pCallbackFormat == null");
      PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_FORMAT2, W_ERROR_OUT_OF_RESOURCE );
      return;
   }
   /* Store the connection information */
   pCallbackFormat->sCallbackContext    = sCallbackContext;

   switch (nTagType)
   {
      case W_PROP_MIFARE_UL :
         PNDEF2GenWrite(pContext,hConnection, static_PNDEFFormatNDEFType2WriteCompleted, pCallbackFormat, g_MifareULFormat, 12, sizeof(g_MifareULFormat),  null );
         break;

      case W_PROP_MIFARE_UL_C :
         PNDEF2GenWrite(pContext,hConnection, static_PNDEFFormatNDEFType2WriteCompleted, pCallbackFormat, g_MifareULCFormat, 12, sizeof(g_MifareULCFormat), null );
         break;

      case W_PROP_MY_D_MOVE :
         PNDEF2GenWrite(pContext,hConnection, static_PNDEFFormatNDEFType2WriteCompleted, pCallbackFormat, g_MyDMoveFormat, 12, sizeof(g_MyDMoveFormat), null );
         break;

      case W_PROP_MY_D_NFC :

         nError = PMyDGetConnectionInfo(pContext, hConnection, &sConnectionInfo);
         if (nError != W_SUCCESS)
         {
            CMemoryFree(pCallbackFormat);
            PDebugError("PNDEFFormatNDEFType2: PMyDGetConnectionInfo failed");
            PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_FORMAT2, nError );
            return;

         }
         if (sConnectionInfo.nSectorNumber == 256)
         {
            PNDEF2GenWrite(pContext,hConnection, static_PNDEFFormatNDEFType2WriteCompleted, pCallbackFormat, g_MyDNFCFormatSLER16P, 12, sizeof(g_MyDNFCFormatSLER16P), null );
         }
         else if (sConnectionInfo.nSectorNumber == 512)
         {
            PNDEF2GenWrite(pContext,hConnection, static_PNDEFFormatNDEFType2WriteCompleted, pCallbackFormat, g_MyDNFCFormatSLER32P, 12, sizeof(g_MyDNFCFormatSLER32P), null );
         }
         else
         {
            CMemoryFree(pCallbackFormat);
            PDebugError("PNDEFFormatNDEFType2: unknown my-d NFC model");
            PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_FORMAT2, nError );
            return;
         }
         break;
   }
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
