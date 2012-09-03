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
#define P_MODULE P_MODULE_DEC( NDEF_FORMAT6 )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#define P_COMMAND_TYPE6_MAX_LENGTH   8192 /* up 32 bytes for one block * up to 256 blocks */

typedef struct __tType6CommandInfo
{
   /* List of commands */
   uint8_t nOffset;
   uint8_t nLength;
   uint8_t pCommand[P_COMMAND_TYPE6_MAX_LENGTH];
} tType6CommandInfo;

typedef struct t15693Format
{
   /* Callback context */
   tDFCCallbackContext        sCallbackContext;

} t15693Format;

const uint8_t g_TagItFormat[] = {
      /* CC file */     0x4E, 0x44, 0x45, 0x10, 0x03, 0x3F, 0x00, 0x3E,
      /* NDEF file */   0x00, 0x00 };


const uint8_t g_ICodeFormat[] = {
      /* CC file */     0x4E, 0x44, 0x45, 0x10, 0x03, 0x1B, 0x00, 0x1A,
      /* NDEF file */   0x00, 0x00  };

const uint8_t g_LRI512Format[] = {
      /* CC file */     0x4E, 0x44, 0x45, 0x10, 0x03, 0x0F, 0x00, 0x0E,
      /* NDEF file */   0x00, 0x00 };


static void static_W15693WriteCompleted (
            tContext* pContext,
            void *pCallbackParameter,
            W_ERROR nError )
{
   /* Get the command table address */
   t15693Format* p15693Format = (t15693Format*)pCallbackParameter;

   PDebugTrace("static_W15693WriteCompleted: nError=0x%02x",nError);

   PDFCPostContext2( &p15693Format->sCallbackContext, P_DFC_TYPE_FORMAT6, nError );
   CMemoryFree(p15693Format);
}

void PNDEFFormatNDEFType6(tContext* pContext,
                          W_HANDLE hConnection,
                          tPBasicGenericCallbackFunction *pCallback,
                          void *pCallbackParameter,
                          uint8_t nTypeTag)
{
   t15693Format* p15693Format = null;
   tDFCCallbackContext sCallbackContext;

   PDebugTrace("PNDEFFormatNDEFType6");

   /* Get the connection property number to check if it exists */
   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   p15693Format = (t15693Format*)CMemoryAlloc( sizeof(t15693Format) );

   if ( p15693Format == null )
   {
      PDebugError("PNDEFFormatNDEFType6: p15693Format == null");
      PDFCPostContext2( &sCallbackContext, P_DFC_TYPE_FORMAT6, W_ERROR_OUT_OF_RESOURCE );
      return;
   }

   /* Store the connection information */
   p15693Format->sCallbackContext   = sCallbackContext;

   switch (nTypeTag)
   {
      case W_PROP_TI_TAGIT :

         P15Write(pContext,
                     hConnection,
                     static_W15693WriteCompleted,
                     p15693Format,
                     g_TagItFormat,
                     0,
                     sizeof(g_TagItFormat),
                     false,
                     null );
         break;

      case W_PROP_NXP_ICODE :

         P15Write(pContext,
                     hConnection,
                     static_W15693WriteCompleted,
                     p15693Format,
                     g_ICodeFormat,
                     0,
                     sizeof(g_ICodeFormat),
                     false,
                     null );
         break;

      case W_PROP_ST_LRI_512 :

         P15Write(pContext,
                     hConnection,
                     static_W15693WriteCompleted,
                     p15693Format,
                     g_LRI512Format,
                     0,
                     sizeof(g_LRI512Format),
                     false,
                     null );
         break;
   }
}
#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
