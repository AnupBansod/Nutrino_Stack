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
   Contains the implementation of some utility functions
*******************************************************************************/

#include "wme_context.h"

/* See header file */
void PUtilWriteUint16ToBigEndianBuffer(
         uint16_t nValue,
         uint8_t* pBuffer)
{
   pBuffer[0] = (uint8_t)((nValue >> 8) & 0xFF);
   pBuffer[1] = (uint8_t)(nValue & 0xFF);
}

/* See header file */
uint16_t PUtilReadUint16FromBigEndianBuffer(
         const uint8_t* pBuffer)
{
   return (uint16_t)((((uint16_t)pBuffer[0]) << 8) | ((uint16_t)pBuffer[1]));
}

/* See header file */
void PUtilWriteUint32ToBigEndianBuffer(
         uint32_t nValue,
         uint8_t* pBuffer)
{
   pBuffer[0] = (uint8_t)((nValue >> 24) & 0xFF);
   pBuffer[1] = (uint8_t)((nValue >> 16) & 0xFF);
   pBuffer[2] = (uint8_t)((nValue >> 8) & 0xFF);
   pBuffer[3] = (uint8_t)(nValue & 0xFF);
}

/* See header file */
uint32_t PUtilReadUint32FromBigEndianBuffer(
         const uint8_t* pBuffer)
{
   return (uint32_t)((((uint32_t)pBuffer[0]) << 24)
   | (((uint32_t)pBuffer[1]) << 16)
   | (((uint32_t)pBuffer[2]) << 8)
   | ((uint32_t)pBuffer[3]));
}

/* See header file */
uint32_t PUtilReadUint32FromLittleEndianBuffer(
         const uint8_t* pBuffer)
{
   return (uint32_t)((((uint32_t)pBuffer[3]) << 24)
   | (((uint32_t)pBuffer[2]) << 16)
   | (((uint32_t)pBuffer[1]) << 8)
   | ((uint32_t)pBuffer[0]));
}

/******************************************************************************

   Utf-8 (8-bit UCS/Unicode Transformation Format) ISO 10646
   ---------------------------------------------------------

                              0zzzzzzz (7)          [   00..7F   ] (Ascii)
                     110yyyyy 10zzzzzz (5+6=11)     [   80..7FF  ]
            1110xxxx 10yyyyyy 10zzzzzz (4+6+6=16)   [  800..FFFF ]


   Utf-16 - (16-bit Unicode Transformation Format) ISO 10646
   ---------------------------------------------------------

                [0000..FFFF] 64Ko available
                (!) surrogate not impleted [D800..DFFF]
                Little Endian specified by NDEF (NFC forum)

******************************************************************************/

/* See header file */
uint32_t PUtilConvertUTF16ToUTF8(
                  uint8_t* pDestUtf8,
                  const tchar* pSourceUtf16,
                  uint32_t nSourceCharLength )
{
   uint32_t nIndex = 0;
   uint16_t v;
   uint32_t i;

   for (i = 0; i < nSourceCharLength; i++)
   {
      v = pSourceUtf16[i];

      if ( v < 128 )
      {
         /* [0xxx-xxxx] */
         if ( pDestUtf8 != null )
         {
            *pDestUtf8++ = (uint8_t)v;          /* 00000000-0xxxxxx */
         }
         nIndex++;
      }
      else if ( v < 2048 )
      {
         /* [110x-xxxx][10xx-xxxx] */
         if ( pDestUtf8 != null )
         {
            *pDestUtf8++ = (uint8_t)(0xC0 | (v>>6));       /* 00000xxx-xx...... */
            *pDestUtf8++ = (uint8_t)(0x80 | (v&0x3F));     /* 00000...-..xxxxxx */
         }
         nIndex += 2;
      }
      else
      {
         /* [1110-xxxx][10xx-xxxx][10xx-xxxx] */
         if ( pDestUtf8 != null )
         {
            *pDestUtf8++ = (uint8_t)(0xE0 | ((v&0xF000)>>12)); /* xxxx....-........ */
            *pDestUtf8++ = (uint8_t)(0x80 | ((v&0x0FC0)>>6));  /* ....xxxx-xx...... */
            *pDestUtf8++ = (uint8_t)(0x80 | (v&0x3F));         /* ........-..xxxxxx */
         }
         nIndex += 3;
      }
   }

   return nIndex;
}

/* See header file */
uint32_t PUtilConvertUTF8ToUTF16(
                  tchar* pDestUtf16,
                  const uint8_t* pSourceUtf8,
                  uint32_t nSourceLength )
{
   uint32_t i;
   uint16_t nValue;
   uint32_t nIndex = 0;

   for (i=0; i<nSourceLength; i++)
   {
      if ( ((*pSourceUtf8)&0x80) == 0x00 )
      {
         /* [0xxx-xxxx] */
         nValue = *pSourceUtf8++;      /* 0xxx-xxxx */
      }
      else
      {
         switch ( (*pSourceUtf8) & 0xF0 )
         {
         case 0xE0:
            /* [1110-xxxx][10xx-xxxx][10xx-xxxx] */
            if ( ( (i+2) < nSourceLength )
               && ( (*(pSourceUtf8+1)&0xC0) == 0x80 )
               && ( (*(pSourceUtf8+2)&0xC0) == 0x80 ))
            {
               nValue =  ( (uint16_t)*(pSourceUtf8++)&0x0f ) << (12);/* ....-xxxx */
               nValue |= ( (uint16_t)*(pSourceUtf8++)&0x3f ) << 6;   /* ..xx-xxxx */
               nValue |=   (uint16_t)*(pSourceUtf8++)&0x3f;          /* ..xx-xxxx */
               if ( nValue < 2048 )
               {
                  goto return_error;
               }
               i += 2;
               break;
            }
            goto return_error;

         case 0xC0:
         case 0xD0:
               /* [110x-xxxx][10xx-xxxx] */
            if ( ( (i+1) < nSourceLength )
               && ( (*(pSourceUtf8+1)&0xC0) == 0x80 ))
            {
               nValue =  ( (uint16_t)*(pSourceUtf8++)&0x1f ) << 6;/* ...x-xxxx */
               nValue |=   (uint16_t)*(pSourceUtf8++)&0x3f;       /* ..xx-xxxx */
               if ( nValue < 128 )
               {
                  goto return_error;
               }
               ++i;
               break;
            }
            goto return_error;

         default:
            goto return_error;
         }
      }

      if ( pDestUtf16 != null )
      {
         ((uint16_t*)pDestUtf16)[ nIndex ] = nValue;
      }
      ++nIndex;

   } /* END loop */

   return nIndex;

return_error:

   return 0;
}


static bool static_PUtilIsMachineLittleEndian(void)
{
   tchar      nCar = 0x1234;
   uint8_t  * p = (uint8_t * ) & nCar;

   return (*p == 0x12) ? false : true;
}


/* see header */
void PUtilConvertUTF16ToUTF16BE(
                  tchar* pDestUtf16,
                  const tchar* pSourceUtf16,
                  uint32_t nSourceLength )
{
   tchar nCurrentChar;

   if (static_PUtilIsMachineLittleEndian() == true)
   {
      while (nSourceLength-- != 0)
      {
         nCurrentChar = * pSourceUtf16++ ;

         * pDestUtf16++ =  ((nCurrentChar & 0x00FF) << 8) | ((nCurrentChar >> 8) & 0x00FF);
      }
   }
   else
   {
      if (pDestUtf16 != pSourceUtf16)
      {
         CMemoryMove(pDestUtf16, pSourceUtf16, nSourceLength * sizeof(tchar));
      }
   }
}

/* see header */
void PUtilConvertUTF16BEToUTF16(
                  tchar* pDestUtf16,
                  const tchar* pSourceUtf16,
                  uint32_t nSourceLength )
{

   tchar nCurrentChar;

   if (static_PUtilIsMachineLittleEndian() == true)
   {
      while (nSourceLength-- != 0)
      {
         nCurrentChar = * pSourceUtf16++ ;

         * pDestUtf16++ = ((nCurrentChar & 0x00FF) << 8) | ((nCurrentChar >> 8) & 0x00FF);
      }
   }
   else
   {
      if (pDestUtf16 != pSourceUtf16)
      {
         CMemoryMove(pDestUtf16, pSourceUtf16, nSourceLength * sizeof(tchar));
      }
   }
}


/* See header file */
uint32_t PUtilWriteHexaUint8(
               tchar* pStringBuffer,
               uint8_t nValue)
{
   *pStringBuffer = (nValue >> 4) + '0';
   if(*pStringBuffer > '9')
   {
      *pStringBuffer = *pStringBuffer - '0' - 10 + 'A';
   }
   pStringBuffer++;

   *pStringBuffer = (nValue & 0x0F) + '0';
   if(*pStringBuffer > '9')
   {
      *pStringBuffer = *pStringBuffer - '0' - 10 + 'A';
   }

   return 2;
}

/* See header file */
uint32_t PUtilWriteHexaUint32(
               tchar* pStringBuffer,
               uint32_t nValue)
{
   pStringBuffer += PUtilWriteHexaUint8(pStringBuffer, (uint8_t)((nValue >> 24) & 0xFF));
   pStringBuffer += PUtilWriteHexaUint8(pStringBuffer, (uint8_t)((nValue >> 16) & 0xFF));
   pStringBuffer += PUtilWriteHexaUint8(pStringBuffer, (uint8_t)((nValue >> 8) & 0xFF));
   PUtilWriteHexaUint8(pStringBuffer, (uint8_t)(nValue & 0xFF));

   return 8;
}

/* See header file */
uint32_t PUtilWriteDecimalUint32(
               tchar* pStringBuffer,
               uint32_t nValue)
{
   uint8_t aBuffer[10];
   uint32_t nLength = 0;
   uint32_t nPos;

   do
   {
      aBuffer[nLength++] = (uint8_t)(nValue % 10);
      nValue /= 10;
   }
   while(nValue != 0);

   for(nPos = 1; nPos <= nLength; nPos++)
   {
      *pStringBuffer++ = aBuffer[nLength - nPos] + '0';
   }

   return nLength;
}

/* See header file */
uint32_t PUtilStringLength(
               const tchar* pString )
{
   const tchar *pNext;

   if (pString == null)
   {
      return 0;
   }
   for (pNext = pString; *pNext; ++pNext);

   return pNext-pString;
}

/* See header file */
sint32_t PUtilStringCompare(
               const tchar* pString1,
               const tchar* pString2 )
{
  while (*pString1 == *pString2)
  {
      if (*pString1 == 0)
      {
         return 0;
      }
      pString1++;
      pString2++;
  }

  return *pString1 - *pString2;
}

/* See header file */
sint32_t PUtilStringNCompare(
               const tchar * pString1,
               const tchar * pString2,
               uint32_t      nLength)
{
   while ((nLength != 0) && (*pString1 == *pString2))
   {
      if (*pString1 == 0)
      {
         return 0;
      }
      pString1++;
      pString2++;
      nLength--;
  }

   if (nLength > 0)
   {
      return *pString1 - *pString2;
   }
   else
   {
      return 0;
   }
}


/* See header file */
sint32_t PUtilMixedStringCompare(
               const tchar* pUnicodeString,
               uint32_t nUnicodeLength,
               const char* pASCIIString )
{
   uint32_t nIndex;

   for(nIndex = 0; nIndex < nUnicodeLength; nIndex++)
   {
      tchar ac = (tchar)pASCIIString[nIndex];
      tchar uc = pUnicodeString[nIndex];

      if(ac != uc)
      {
         return uc - ac;
      }

      if(uc == 0)
      {
         if(nIndex == nUnicodeLength - 1)
         {
            return 0;
         }

         return 1;
      }
   }

   return 0 - pASCIIString[nIndex];
}

/* See header file */
tchar* PUtilStringCopy(
               tchar* pBuffer,
               uint32_t* pPos,
               const tchar* pString)
{
   uint32_t nCursor = *pPos;
   tchar c;

   while( (c = *pString++) != 0)
   {
      pBuffer[nCursor++] = c;
   }

   pBuffer[nCursor] = 0;

   *pPos = nCursor;

   return &pBuffer[nCursor];
}

/* -----------------------------------------------------------------------------
      Trace Functions
----------------------------------------------------------------------------- */

#ifdef P_TRACE_ACTIVE

/* See header file */
uint32_t PUtilLogUint8(
         char* pTraceBuffer,
         uint32_t nValue)
{
   uint8_t nDigit = (uint8_t)((nValue >> 4) & 0x0F);

   if(nDigit <= 9)
   {
      *pTraceBuffer++ = nDigit + '0';
   }
   else
   {
      *pTraceBuffer++ = nDigit - 10 + 'A';
   }

   nDigit = (uint8_t)(nValue & 0x0F);

   if(nDigit <= 9)
   {
      *pTraceBuffer = nDigit + '0';
   }
   else
   {
      *pTraceBuffer = nDigit - 10 + 'A';
   }

   return 2;
}

/* See header file */
uint32_t PUtilLogUint16(
                  char* pTraceBuffer,
                  uint32_t nValue)
{
    uint32_t nPos = 0;
    nPos +=PUtilLogUint8(&pTraceBuffer[nPos], ((nValue & 0x0000FF00) >> 8));
    nPos +=PUtilLogUint8(&pTraceBuffer[nPos],  (nValue & 0x000000FF));
    return nPos;
}

/* See header file */
uint32_t PUtilLogUint32(
                  char* pTraceBuffer,
                  uint32_t nValue)
{
    uint32_t nPos = 0;
    nPos +=PUtilLogUint8(&pTraceBuffer[nPos], ((nValue & 0xFF000000) >> 24));
    nPos +=PUtilLogUint8(&pTraceBuffer[nPos], ((nValue & 0x00FF0000) >> 16));
    nPos +=PUtilLogUint8(&pTraceBuffer[nPos], ((nValue & 0x0000FF00) >> 8));
    nPos +=PUtilLogUint8(&pTraceBuffer[nPos],  (nValue & 0x000000FF));
    return nPos;
}

/* See header file */
uint32_t PUtilLogArray(
         char* pTraceBuffer,
         uint8_t* pBuffer,
         uint32_t nLength)
{
   uint32_t nPos = 0;
   uint32_t i;

   if(nLength != 0)
   {
      PUtilTraceASCIIStringCopy(pTraceBuffer, &nPos, "(len=0x00)");

      (void)PUtilLogUint8(&pTraceBuffer[7], nLength);

      for(i = 0; i < nLength; i++)
      {
         pTraceBuffer[nPos++] = ' ';
         nPos += PUtilLogUint8(&pTraceBuffer[nPos], pBuffer[i]);
      }
   }

   pTraceBuffer[nPos] = 0;

   return nPos;
}

/* See header file */
void PUtilTraceASCIIStringCopy(
            char* pBuffer,
            uint32_t* pPos,
            const char* pString)
{
   uint32_t nCursor = *pPos;
   char c;

   while( (c = *pString++) != 0)
   {
      pBuffer[nCursor++] = c;
   }

   pBuffer[nCursor] = 0;

   *pPos = nCursor;
}

/* See header file */
const char* PUtilTraceBool(
         bool bValue)
{
   return bValue?"true":"false";
}

#define P_CODE_2_STRING(X) \
         case X: \
            return #X;

/* See header file */
const char* PUtilTraceError(
         W_ERROR nError)
{
   switch(nError)
   {
      P_CODE_2_STRING(W_SUCCESS)
      P_CODE_2_STRING(W_ERROR_VERSION_NOT_SUPPORTED)
      P_CODE_2_STRING(W_ERROR_ITEM_NOT_FOUND)
      P_CODE_2_STRING(W_ERROR_BUFFER_TOO_SHORT)
      P_CODE_2_STRING(W_ERROR_PERSISTENT_DATA)
      P_CODE_2_STRING(W_ERROR_NO_EVENT)
      P_CODE_2_STRING(W_ERROR_WAIT_CANCELLED)
      P_CODE_2_STRING(W_ERROR_UICC_COMMUNICATION)
      P_CODE_2_STRING(W_ERROR_BAD_HANDLE)
      P_CODE_2_STRING(W_ERROR_EXCLUSIVE_REJECTED)
      P_CODE_2_STRING(W_ERROR_SHARE_REJECTED)
      P_CODE_2_STRING(W_ERROR_BAD_PARAMETER)
      P_CODE_2_STRING(W_ERROR_RF_PROTOCOL_NOT_SUPPORTED)
      P_CODE_2_STRING(W_ERROR_CONNECTION_COMPATIBILITY)
      P_CODE_2_STRING(W_ERROR_BUFFER_TOO_LARGE)
      P_CODE_2_STRING(W_ERROR_INDEX_OUT_OF_RANGE)
      P_CODE_2_STRING(W_ERROR_OUT_OF_RESOURCE)
      P_CODE_2_STRING(W_ERROR_BAD_TAG_FORMAT)
      P_CODE_2_STRING(W_ERROR_BAD_NDEF_FORMAT)
      P_CODE_2_STRING(W_ERROR_NDEF_UNKNOWN)
      P_CODE_2_STRING(W_ERROR_LOCKED_TAG)
      P_CODE_2_STRING(W_ERROR_TAG_FULL)
      P_CODE_2_STRING(W_ERROR_CANCEL)
      P_CODE_2_STRING(W_ERROR_TIMEOUT)
      P_CODE_2_STRING(W_ERROR_TAG_DATA_INTEGRITY)
      P_CODE_2_STRING(W_ERROR_NFC_HAL_COMMUNICATION)
      P_CODE_2_STRING(W_ERROR_WRONG_RTD)
      P_CODE_2_STRING(W_ERROR_TAG_WRITE)
      P_CODE_2_STRING(W_ERROR_BAD_NFCC_MODE)
      P_CODE_2_STRING(W_ERROR_TOO_MANY_HANDLERS)
      P_CODE_2_STRING(W_ERROR_BAD_STATE)
      P_CODE_2_STRING(W_ERROR_BAD_FIRMWARE_FORMAT)
      P_CODE_2_STRING(W_ERROR_BAD_FIRMWARE_SIGNATURE)
      P_CODE_2_STRING(W_ERROR_DURING_HARDWARE_BOOT)
      P_CODE_2_STRING(W_ERROR_DURING_FIRMWARE_BOOT)
      P_CODE_2_STRING(W_ERROR_FEATURE_NOT_SUPPORTED)
      P_CODE_2_STRING(W_ERROR_CLIENT_SERVER_PROTOCOL)
      P_CODE_2_STRING(W_ERROR_FUNCTION_NOT_SUPPORTED)
      P_CODE_2_STRING(W_ERROR_TAG_NOT_LOCKABLE)
      P_CODE_2_STRING(W_ERROR_ITEM_LOCKED)
      P_CODE_2_STRING(W_ERROR_SYNC_OBJECT)
      P_CODE_2_STRING(W_ERROR_RETRY)
      P_CODE_2_STRING(W_ERROR_DRIVER )
      P_CODE_2_STRING(W_ERROR_MISSING_INFO)
      P_CODE_2_STRING(W_ERROR_P2P_CLIENT_REJECTED)
      P_CODE_2_STRING(W_ERROR_NFCC_COMMUNICATION)
      P_CODE_2_STRING(W_ERROR_RF_COMMUNICATION)
      P_CODE_2_STRING(W_ERROR_BAD_FIRMWARE_VERSION)
      P_CODE_2_STRING(W_ERROR_HETEROGENEOUS_DATA)
      P_CODE_2_STRING(W_ERROR_CLIENT_SERVER_COMMUNICATION)
      P_CODE_2_STRING(W_ERROR_SECURITY)

      default:
         return "*** Unknown Error ***";
   }
}

/* See header file */
const char* PUtilTracePriority(
         W_ERROR nPriority)
{
   switch(nPriority)
   {
         P_CODE_2_STRING(W_PRIORITY_NO_ACCESS)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 1)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 2)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 3)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 4)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 5)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 6)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 7)
         P_CODE_2_STRING(W_PRIORITY_MINIMUM + 8)
         P_CODE_2_STRING(W_PRIORITY_MAXIMUM)
         P_CODE_2_STRING(W_PRIORITY_EXCLUSIVE)
         P_CODE_2_STRING(W_PRIORITY_SE)
         P_CODE_2_STRING(W_PRIORITY_SE_FORCED)
      default:
         return "*** Unknown Priority ***";
   }
}

/* See header file */
const char* PUtilTraceCardProtocol(
         tContext* pContext,
         uint32_t nProtocol)
{
   uint8_t nIndex;
   uint32_t nPos = 0;
   char* pTraceBuffer = PContextGetCardProtocolTraceBuffer(pContext);

   static const uint32_t aProtocolFlag[] = {
      W_NFCC_PROTOCOL_CARD_ISO_14443_4_A,
      W_NFCC_PROTOCOL_CARD_ISO_14443_4_B,
      W_NFCC_PROTOCOL_CARD_ISO_14443_3_A,
      W_NFCC_PROTOCOL_CARD_ISO_14443_3_B,
      W_NFCC_PROTOCOL_CARD_ISO_15693_3,
      W_NFCC_PROTOCOL_CARD_ISO_15693_2,
      W_NFCC_PROTOCOL_CARD_FELICA,
      W_NFCC_PROTOCOL_CARD_P2P_TARGET,
      W_NFCC_PROTOCOL_CARD_TYPE_1_CHIP,
      W_NFCC_PROTOCOL_CARD_MIFARE_CLASSIC,
      W_NFCC_PROTOCOL_CARD_BPRIME};

   static const char* aProtocolName[] = {
      "14-4A ", "14-4B ", "14-3A ", "14-3B ", "15-3 ", "15-2 ",
      "FELICA ", "P2P ", "T1 ", "MIFCLA ", "BP "};

   for(nIndex = 0; nIndex < (sizeof(aProtocolFlag)/sizeof(uint32_t)); nIndex++)
   {
      if((nProtocol & aProtocolFlag[nIndex]) != 0)
      {
         PUtilTraceASCIIStringCopy(pTraceBuffer, &nPos, aProtocolName[nIndex]);
      }
   }

   if(nPos != 0)
   {
      nPos--;
   }
   else
   {
      PUtilTraceASCIIStringCopy(pTraceBuffer, &nPos, "none");
   }

   pTraceBuffer[nPos] = 0;

   return pTraceBuffer;
}

/* See header file */
const char* PUtilTraceReaderProtocol(
         tContext* pContext,
         uint32_t nProtocol)
{
   uint8_t nIndex;
   uint32_t nPos = 0;
   char* pTraceBuffer = PContextGetReaderProtocolTraceBuffer(pContext);

   static const uint32_t aProtocolFlag[] = {
      W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_B,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_B,
      W_NFCC_PROTOCOL_READER_ISO_15693_3,
      W_NFCC_PROTOCOL_READER_ISO_15693_2,
      W_NFCC_PROTOCOL_READER_FELICA,
      W_NFCC_PROTOCOL_READER_P2P_INITIATOR,
      W_NFCC_PROTOCOL_READER_TYPE_1_CHIP,
      W_NFCC_PROTOCOL_READER_MIFARE_CLASSIC,
      W_NFCC_PROTOCOL_READER_BPRIME};

   static const char* aProtocolName[] = {
      "14-4A ", "14-4B ", "14-3A ", "14-3B ", "15-3 ", "15-2 ",
      "FELICA ", "P2P ", "T1 ", "MIFCLA ", "BP "};

   for(nIndex = 0; nIndex < (sizeof(aProtocolFlag)/sizeof(uint32_t)); nIndex++)
   {
      if((nProtocol & aProtocolFlag[nIndex]) != 0)
      {
         PUtilTraceASCIIStringCopy(pTraceBuffer, &nPos, aProtocolName[nIndex]);
      }
   }

   if(nPos != 0)
   {
      nPos--;
   }
   else
   {
      PUtilTraceASCIIStringCopy(pTraceBuffer, &nPos, "none");
   }

   pTraceBuffer[nPos] = 0;

   return pTraceBuffer;
}

/* See header file */
const char* PUtilTraceConnectionProperty(
         uint8_t nProtocol)
{
   switch(nProtocol)
   {
      P_CODE_2_STRING(W_PROP_ISO_14443_3_A)
      P_CODE_2_STRING(W_PROP_ISO_14443_4_A)
      P_CODE_2_STRING(W_PROP_ISO_14443_3_B)
      P_CODE_2_STRING(W_PROP_ISO_14443_4_B)
      P_CODE_2_STRING(W_PROP_ISO_15693_3)
      P_CODE_2_STRING(W_PROP_ISO_15693_2)
      P_CODE_2_STRING(W_PROP_ISO_7816_4)
      P_CODE_2_STRING(W_PROP_BPRIME)

      P_CODE_2_STRING(W_PROP_NFC_TAG_TYPE_1)
      P_CODE_2_STRING(W_PROP_NFC_TAG_TYPE_2)
      P_CODE_2_STRING(W_PROP_NFC_TAG_TYPE_3)
      P_CODE_2_STRING(W_PROP_NFC_TAG_TYPE_4_A)
      P_CODE_2_STRING(W_PROP_NFC_TAG_TYPE_4_B)
      P_CODE_2_STRING(W_PROP_NFC_TAG_TYPE_5)
      P_CODE_2_STRING(W_PROP_NFC_TAG_TYPE_6)

      P_CODE_2_STRING(W_PROP_TYPE1_CHIP)
      P_CODE_2_STRING(W_PROP_JEWEL)
      P_CODE_2_STRING(W_PROP_TOPAZ)
      P_CODE_2_STRING(W_PROP_TOPAZ_512)
      P_CODE_2_STRING(W_PROP_FELICA)
      P_CODE_2_STRING(W_PROP_PICOPASS_2K)
      P_CODE_2_STRING(W_PROP_PICOPASS_32K)
      P_CODE_2_STRING(W_PROP_ICLASS_2K)
      P_CODE_2_STRING(W_PROP_ICLASS_16K)
      P_CODE_2_STRING(W_PROP_MY_D_MOVE)
      P_CODE_2_STRING(W_PROP_MY_D_NFC)
      P_CODE_2_STRING(W_PROP_MIFARE_UL)
      P_CODE_2_STRING(W_PROP_MIFARE_UL_C)
      P_CODE_2_STRING(W_PROP_MIFARE_MINI)
      P_CODE_2_STRING(W_PROP_MIFARE_1K)
      P_CODE_2_STRING(W_PROP_MIFARE_4K)
      P_CODE_2_STRING(W_PROP_MIFARE_DESFIRE_D40)
      P_CODE_2_STRING(W_PROP_MIFARE_DESFIRE_EV1_2K)
      P_CODE_2_STRING(W_PROP_MIFARE_DESFIRE_EV1_4K)
      P_CODE_2_STRING(W_PROP_MIFARE_DESFIRE_EV1_8K)
      P_CODE_2_STRING(W_PROP_MIFARE_PLUS_X_2K)
      P_CODE_2_STRING(W_PROP_MIFARE_PLUS_X_4K)
      P_CODE_2_STRING(W_PROP_MIFARE_PLUS_S_2K)
      P_CODE_2_STRING(W_PROP_MIFARE_PLUS_S_4K)
      P_CODE_2_STRING(W_PROP_TI_TAGIT)
      P_CODE_2_STRING(W_PROP_ST_LRI_512)
      P_CODE_2_STRING(W_PROP_ST_LRI_2K)
      P_CODE_2_STRING(W_PROP_NXP_ICODE)

      P_CODE_2_STRING(W_PROP_SECURE_ELEMENT)
      P_CODE_2_STRING(W_PROP_VIRTUAL_TAG)

      P_CODE_2_STRING(W_PROP_P2P_TARGET)
      P_CODE_2_STRING(W_PROP_ISO_7816_4_A)
      P_CODE_2_STRING(W_PROP_ISO_7816_4_B)

      default:
         return "*** Unknown Property ***";
   }
}

#undef P_CODE_2_STRING

#else /* P_TRACE_ACTIVE */

   /* @note: this is needeed by Microsoft Visual C++ 2010 Express ; even if functions are not used (always called inside PDebugTrace() */

/* See header file */
const char* PUtilTraceBool(
         bool bValue)
{
   return null;
}

/* See header file */
const char* PUtilTraceError(
         W_ERROR nError)
{
   return null;
}

/* See header file */
const char* PUtilTracePriority(
         W_ERROR nPriority)
{
   return null;
}

/* See header file */
const char* PUtilTraceCardProtocol(
         tContext* pContext,
         uint32_t nProtocol)
{
   return null;
}

/* See header file */
const char* PUtilTraceReaderProtocol(
         tContext* pContext,
         uint32_t nProtocol)
{
   return null;
}

/* See header file */
const char* PUtilTraceConnectionProperty(
         uint8_t nProtocol)
{
   return null;
}

#endif /* #ifdef P_TRACE_ACTIVE */

