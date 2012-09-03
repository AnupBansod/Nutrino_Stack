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
   Contains the NDEF API implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( NDEF )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#include "wme_ndef.h"

const tchar g_aNDEFWellKnownPrefix[]      = {   'u', 'r', 'n', ':', 'n', 'f', 'c', ':', 'w', 'k', 't', ':', 0x00 };
const uint32_t g_nNDEFWellKnownPrefixLengh   = 12;
const tchar g_aNDEFExternalPrefix[]       = {   'u', 'r', 'n', ':', 'n', 'f', 'c', ':', 'e', 'x', 't', ':', 0x00 };
const uint32_t   g_nNDEFExternalPrefixLengh    = 12;

static uint32_t static_PNDEFDestroyNDEFMessage(
         tContext* pContext,
         void* pObject );

static uint32_t static_PNDEFDestroyNDEFRecord(
         tContext* pContext,
         void* pObject );

tHandleType g_sNDEFType   = { static_PNDEFDestroyNDEFMessage, null, null, null, null };
tHandleType g_sNDEFRecord = { static_PNDEFDestroyNDEFRecord, null, null, null, null };


static uint32_t static_PNDEFGetRecordNumber(tNDEFMessage * pMessage);

uint32_t PNDEFParseBuffer(
         const uint8_t* pBuffer,
         uint32_t nBufferSize,
         uint32_t  *pnTypeOffset,
         uint32_t  *pnTypeLength,
         uint32_t  *pnIDOffset,
         uint32_t  *pnIDLength,
         uint32_t  *pnPayloadOffset,
         uint32_t *pnPayloadLength)
{
   /*    +--+--+--+--++--+--+--+--+
    *    |  |  |  |SR||IL|  TNF   |
    *    +--+--+--+--++--+--------+
    *    |    TYPE length (1)     |
    *    +------------------------+
    *    :  PAYLOAD length (1/4)  : SR=0:[4] SR=1:[1]
    *    +------------------------+
    *    :    ID length (0/1)     : IL=0:[0] IL=1:[1]
    *    +------------------------+
    *    :        TYPE[]          : [TYPE length]
    *    +------------------------+
    *    :          ID[]          : [ID length]
    *    +------------------------+
    *    :                        :
    *    :     PAYLOAD[]          :
    *    :                        :
    *    +------------------------+
    */

   uint32_t  nTypeOffset;
   uint32_t  nTypeLength;
   uint32_t  nIDOffset;
   uint32_t  nIDLength;
   uint32_t  nPayloadOffset;
   uint32_t nPayloadLength;
   uint8_t nTNF;
   uint32_t i;
   uint32_t nOffset = 2;

   PDebugTrace("PNDEFParseBuffer");

   if ( nBufferSize < 3 )
   {
      return 0;
   }
   nTNF = pBuffer[0];
   nTypeLength = pBuffer[1];

   /* Short Record ? */
   if ( (nTNF&0x10) != 0 )
   {
      nPayloadLength = pBuffer[nOffset++];
   }
   else
   {
      if ( nBufferSize < (nOffset+4) )
      {
         return 0;
      }
      nPayloadLength = 0;
      for (i=0; i<4 ;i++)
      {
         nPayloadLength <<= 8;
         nPayloadLength |= (uint32_t)pBuffer[ nOffset++ ];
      }
   }

   /* Identifier ? */
   if ( (nTNF&0x08) != 0 )
   {
      if ( nBufferSize < (nOffset+1) )
      {
         return 0;
      }
      nIDLength = pBuffer[ nOffset++ ];
   }
   else
   {
      nIDLength = 0;
   }

   nTypeOffset = (nTypeLength>0) ? nOffset : 0;
   nOffset += nTypeLength;

   nIDOffset   = (nIDLength>0) ? nOffset : 0;
   nOffset += nIDLength;

   nPayloadOffset = nOffset;
   if ( nOffset + nPayloadLength > nBufferSize )
   {
      return 0;
   }

   nTNF &= 0x07;

   switch(nTNF)
   {
      case W_NDEF_TNF_EMPTY:
         if((nIDLength != 0) || (nPayloadLength != 0) || (nTypeLength != 0))
         {
            return 0;
         }
         break;
      case W_NDEF_TNF_WELL_KNOWN:
      case W_NDEF_TNF_MEDIA:
      case W_NDEF_TNF_ABSOLUTE_URI:
      case W_NDEF_TNF_EXTERNAL:
         if(nTypeLength == 0)
         {
            return 0;
         }
         break;
      case W_NDEF_TNF_UNKNOWN:
      case W_NDEF_TNF_UNCHANGED:
         if(nTypeLength != 0)
         {
            return 0;
         }
         break;
      default:
         return 0;
   }

   /* Check the type value */
   for(i = 0; i < nTypeLength; i++)
   {
      uint8_t nValue = pBuffer[nTypeOffset + i];
      if((nValue < 0x20) || (nValue > 0x7E))
      {
         return 0;
      }
   }

   /* Check the identifier value */
   /* This checking seems uncompliant with some usage */
   /*
   for(i = 0; i < nIDLength; i++)
   {
      uint8_t nValue = pBuffer[nIDOffset + i];
      if((nValue < 0x20) || (nValue > 0x7E))
      {
         return 0;
      }
   }
   */

   if ( pnTypeOffset != null )
   {
      *pnTypeOffset = nTypeOffset;
   }
   if ( pnTypeLength != null )
   {
      *pnTypeLength = nTypeLength;
   }
   if ( pnIDOffset != null )
   {
      *pnIDOffset = nIDOffset;
   }
   if ( pnIDLength != null )
   {
      *pnIDLength = nIDLength;
   }
   if ( pnPayloadOffset )
   {
      *pnPayloadOffset = nPayloadOffset;
   }
   if ( pnPayloadLength )
   {
      *pnPayloadLength = nPayloadLength;
   }

   nOffset += nPayloadLength;
   return nOffset;
}


/** \brief Returns a pointer on the first occurence of a character in an ASCII string
  *
  * @param[in] pString The string to look into.
  *
  * @param[in] c The character to look for.
  *
  * @return The address of the first occurentce of the character if found
  *         null if the character was not found
  *
  */

static const tchar * static_PNDEFUtilASCIIStringChr(
   const tchar * pString,
   const tchar   c)
{
   CDebugAssert(pString != null);

   while (* pString != 0)
   {
      if (* pString == c)
      {
         return (pString);
      }

      pString++;
   }

   return null;
}

/* ****************************************************************************************************************
    Character type checking functions
   **************************************************************************************************************** */

/** \brief Checks if the provided character belongs to printable URI allowed characters
  *
  * @param[in] nCar  The character
  *
  * return true if the character is a valid printable URI character, false otherwise.
  *
  */

const tchar g_aPrintableURIChars[] =
   {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    ':', '/', '?', '#', '[', ']', '@', '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=', '-', '.', '_', '~' };


static bool static_PNDEFUtilCharacterIsPrintableURI(tchar nCar)
{
  uint32_t i;

   for (i=0; i< sizeof(g_aPrintableURIChars) / sizeof(g_aPrintableURIChars[0]); i++)
   {
      if (g_aPrintableURIChars[i] == nCar)
      {
         return (true);
      }
   }

   return (false);
}

/** \brief Checks if the provided character is an hexadecimal digit
  *
  * @param[in] nCar  The character
  *
  * return true if the character is [0-9] [A-F], false otherwise.
  *
  */

const tchar g_aHexaChars[]         =  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static bool static_PNDEFUtilCharacterIsHexa(tchar nCar)
{
   uint32_t i;

   for (i=0; i<sizeof(g_aHexaChars) / sizeof(g_aHexaChars[0]); i++)
   {
      if (g_aHexaChars[i] == nCar)
      {
         return (true);
      }
   }

   return (false);
}


/* ****************************************************************************************************************
    String type checking functions
   **************************************************************************************************************** */

/** @brief Checks if the provided zero ended UTF-16 string uses only character in the range [0x0000 0x00FF]
  *
  * @param[in] pString  The string
  *
  * return true if the UTF-16 string uses only character in the range [0x0000 0x00FF]
  *        false otherwise.
  *
  */

static bool static_PNDEFUtilStringIs8BitsCompatible(const tchar* pString )
{
   CDebugAssert(pString != null);

   PDebugTrace("static_PNDEFUtilStringIs8BitsCompatible %p", pString);

   while (* pString != 0)
   {
      if (* pString > 255)
      {
         return (false);
      }

      pString++;
   }

   return (true);
}

#if 0 /* @todo */

/** \brief Check if the provided zero ended UTF-16 string uses only character in the range [0x0000 0x007F]
  *
  * @param[in] pString  The string
  *
  * return true if the UTF-16 string uses only character in the range [0x0000 0x0000]
  *        false if the string is not encoded using character set [0x0000 0x007F]
  *
  */

static bool static_PNDEFUtilStringIsASCIICompatible(const tchar * pString)
{
   CDebugAssert(pString != null);

   while (* pString != 0)
   {
      if (*pString > 0x007F)
      {
         return (false);
      }

      pString++;
   }

   return (true);
}

#endif


/** @brief Checks if the provided zero ended UTF-16 string uses only character in the range [0x0020 0x007E]
  *
  * @param[in] pString  The string
  *
  * return true if the UTF-16 string uses only character in the range [0x0020 0x007E]
  *        false otherwise.
  *
  */
#if 0 /* @todo */

static bool static_PNDEFUtilStringIsPrintableASCIICompatible(const tchar * pString)
{
   CDebugAssert(pString != null);

   PDebugTrace("static_PNDEFUtilStringIsPrintableASCIICompatible");

   while (* pString != 0)
   {
      if ((*pString < 0x0020) || (*pString > 0x007E))
      {
         return (false);
      }

      pString++;
   }

   return (true);
}

#endif

static bool static_PNDEFUtilStringIsWellKnownCompatible(const tchar * pString)
{
   tchar nCar;
   CDebugAssert(pString != null);

   PDebugTrace("static_PNDEFUtilStringIsWellKnownCompatible");

   /* skip the 'urn:nfc:wkt: if present */

   if (PUtilStringNCompare(pString, g_aNDEFWellKnownPrefix, g_nNDEFWellKnownPrefixLengh) == 0)
   {
      pString += g_nNDEFWellKnownPrefixLengh;
   }

   /* The WKT-type must have at least one local or global char */

   if (*pString == 0)
   {
      PDebugWarning("static_PNDEFUtilStringIsWellKnownCompatible : WKT-type must have at least one local or global");
      return false;
   }

   /* If the fisrt character is other or reserved return FALSE */

   nCar = *pString++;
   if (!(((nCar >= 'A') && (nCar <= 'Z'))  ||
        ((nCar >= 'a') && (nCar <= 'z'))  ||
        ((nCar >= '0') && (nCar <= '9')) ))
   {
      PDebugWarning("static_PNDEFUtilStringIsWellKnownCompatible : invalid first character in WKT-type part");
      return false;
   }

   while ((nCar = *pString++) != 0)
   {
      if (static_PNDEFUtilCharacterIsPrintableURI(nCar) == false)
      {
         return false;
      }

      if ((nCar == '%' ) || (nCar == '/') || (nCar == '?') || (nCar == '#') || (nCar == '~'))
      {
         return false;
      }
   }

   return (true);
}


static bool static_PNDEFUtilStringIsExternalCompatible(const tchar * pString)
{
   bool bDNSPartFound = false;
   tchar nCar;


   /* skip the 'urn:nfc:ext: if present */

   if (PUtilStringNCompare(pString, g_aNDEFExternalPrefix, g_nNDEFExternalPrefixLengh) == 0)
   {
      pString += g_nNDEFExternalPrefixLengh;
   }

   /* check the DNS part */

   for (nCar = * pString++; nCar != ':';  nCar = * pString++)
   {
      if (nCar == 0)
      {
         PDebugWarning("bool_PNDEFUtilStringIsExternalCompatible : reached end of string when processing DNS part");
         return false;
      }

      if (!(((nCar >= 'A') && (nCar <= 'Z'))  ||
           ((nCar >= 'a') && (nCar <= 'z'))  ||
           ((nCar >= '0') && (nCar <= '9'))  ||
            (nCar == '.')                    ||
            (nCar == '-')))
      {
         PDebugWarning("bool_PNDEFUtilStringIsExternalCompatible : invalid character in DNS part");
         return false;
      }

      bDNSPartFound = true;
   }

   if (bDNSPartFound == false)
   {
      PDebugWarning("bool_PNDEFUtilStringIsExternalCompatible : empty DNS part");
      return false;
   }

   /* here, the DNS part has been processed, and the : has been skipped */

   if (* pString == 0)
   {
      PDebugTrace("bool_PNDEFUtilStringIsExternalCompatible : empty name part is forbidden");
      return false;
   }

   /* Check the name part */

   while ((nCar = *pString++) != 0)
   {
      if (static_PNDEFUtilCharacterIsPrintableURI(nCar) == false)
      {
         return false;
      }

      if ((nCar == '%' ) || (nCar == '/') || (nCar == '?') || (nCar == '#') || (nCar == '~'))
      {
         return false;
      }
   }

   /* all is fine */
   return (true);
}

/** \brief Checks if the provided zero ended UTF-16 string uses the printable URI format
  *
  * @param[in] pInString  The string
  *
  * return  true / false
  *
  */

bool PNDEFUtilStringIsPrintableURI(const tchar * pString)
{
   bool     bInEscapement     = false;
   uint32_t nEscapementLength = 0;
   tchar    nCar;

   CDebugAssert(pString != null);

   PDebugTrace("PNDEFUtilStringIsPrintableURI");

   while ((nCar = * pString++) != 0)
   {
      if (bInEscapement == false)
      {
         if (nCar != '%')
         {
            if (static_PNDEFUtilCharacterIsPrintableURI(nCar) == false)
               return false;
         }
         else
         {
            bInEscapement = true;
            nEscapementLength = (* pString == 'u') ? 4 : 2;
         }
      }
      else
      {
         if ((nEscapementLength == 4) && (nCar == 'u'))
            continue;

         if (static_PNDEFUtilCharacterIsHexa(nCar) == false)
         {
            return (false);
         }

         if (--nEscapementLength == 0)
         {
            bInEscapement = false;
         }
      }
   }

   return (bInEscapement == false) ? true : false;
}


/* ****************************************************************************************************************
    String conversion functions
   **************************************************************************************************************** */


/** \brief Converts a 8-bits compatible zero ended UTF16 string into a byte array
  *
  * @param[in] pInString  The string
  *
  * @param[out] pOutBuffer The destination byte array
  *
  * return The length of the ASCII byte array
  *
  */

static uint32_t static_PNDEFUtilConvert8BitCompatibleStringToByteArray(const tchar * pInString, uint8_t * pOutBuffer)
{
   uint32_t nLength = 0;

   CDebugAssert(pInString != null);

   PDebugTrace("static_PNDEFUtilConvert8BitCompatibleStringToByteArray");

   while (* pInString != 0)
   {
      if (pOutBuffer != null)
      {
         * pOutBuffer ++ = (uint8_t) * pInString++;
      }
      nLength++;
   }

   return (nLength);
}

/** \brief Converts a byte array into a 8 bit compatible UTF16 string
  *
  * @param[out] pString   The destination string
  *
  * @param[in] pInBuffer  The source buffer
  *
  * @param[in] nBufferLength  The source buffer length
  *
  * return  The length of the string
  */

uint32_t PNDEFUtilConvertByteArrayTo8BitCompatibleString(tchar * pOutString, const uint8_t * pInBuffer, uint32_t nBufferLength)
{
   uint32_t nLength = 0;

   PDebugTrace("PNDEFUtilConvertByteArrayTo8BitCompatibleString");

   if ((pInBuffer == null) || (nBufferLength == 0))
   {
      return 0;
   }

   while (nBufferLength--)
   {
      if (pOutString != null)
      {
         * pOutString++ = * pInBuffer++;
      }
      nLength++;
   }

   /* Add the NUL character */
   if (pOutString != null)
   {
      * pOutString = 0;
   }

   return (nLength);
}

/** \brief Converts a UTF16 buffer into a printable URI UTF-16 string
  *
  * @param[out] pString   The destination string
  *
  * @param[in] pInBuffer  The source buffer
  *
  * @param[in] nBufferLength  The source buffer length
  *
  * @return  The length of the string
  */

uint32_t PUtilConvertUTF16BufferToPrintableString(
   tchar       * pDestString,
   const tchar * pInBuffer,
   uint32_t      nBufferLength)
{
   tchar    nCar;
   uint32_t nDestLength = 0;
   uint32_t    i;

   PDebugTrace("PUtilConvertUTF16BufferToPrintableString");

   if (pInBuffer == null)
   {
      return 0;
   }

   for (i=0; i<nBufferLength; i++)
   {
      nCar = * pInBuffer++;

      if (nCar > 255)
      {
         /* must be encoded using %uXXXX format */

         if (pDestString != null)
         {
            *pDestString++ = '%';
            *pDestString++ = 'u';
            *pDestString++ = g_aHexaChars[(nCar >> 12) & 0x0F];
            *pDestString++ = g_aHexaChars[(nCar >> 8)  & 0x0F];
            *pDestString++ = g_aHexaChars[(nCar >> 4)  & 0x0F];
            *pDestString++ = g_aHexaChars[(nCar)       & 0x0F];
         }

         nDestLength += 6;
      }
      else
      {
         if (static_PNDEFUtilCharacterIsPrintableURI(nCar) == true)
         {
            if (pDestString != null)
            {
               *pDestString++ = nCar;
            }

            nDestLength++;
         }
         else
         {
            if (pDestString != null)
            {

               /* must be encoded using %XX format */

               *pDestString++ = '%';
               *pDestString++ = g_aHexaChars[(nCar >> 4)  & 0x0F];
               *pDestString++ = g_aHexaChars[(nCar)       & 0x0F];
            }

            nDestLength += 3;
         }
      }
   }

   /* Add the NUL character */
   if (pDestString != 0)
   {
      * pDestString = 0;
   }

   return (nDestLength);
}




/** \brief Converts a zero-ended UTF16 string into a printable URI UTF-16 string
  *
  * @param[out] tchar         * pDestString,   The destination string
  *
  * @param[in] pInBuffer  The source buffer
  *
  * @param[in] nBufferLength  The source buffer length
  *
  * return  The length of the string
  */


uint32_t PUtilConvertStringToPrintableString(
   tchar         * pDestString,
   const tchar   * pSourceString
   )
{
   tchar    nCar;
   uint32_t nDestLength = 0;

   PDebugTrace("PUtilConvertStringToPrintableString");

   if (pSourceString == null)
   {
      return 0;
   }

   while (* pSourceString != 0)
   {
      nCar = * pSourceString++;

      if (nCar > 255)
      {
         /* must be encoded using %uXXXX format */

         if (pDestString != null)
         {
            *pDestString++ = '%';
            *pDestString++ = 'u';
            *pDestString++ = g_aHexaChars[(nCar >> 12) & 0x0F];
            *pDestString++ = g_aHexaChars[(nCar >> 8)  & 0x0F];
            *pDestString++ = g_aHexaChars[(nCar >> 4)  & 0x0F];
            *pDestString++ = g_aHexaChars[(nCar)       & 0x0F];
         }

         nDestLength += 6;
      }
      else
      {
         if (static_PNDEFUtilCharacterIsPrintableURI(nCar) == true)
         {
            if (pDestString != null)
            {
               *pDestString++ = nCar;
            }

            nDestLength++;
         }
         else
         {
            if (pDestString != null)
            {

               /* must be encoded using %XX format */

               *pDestString++ = '%';
               *pDestString++ = g_aHexaChars[(nCar >> 4)  & 0x0F];
               *pDestString++ = g_aHexaChars[(nCar)       & 0x0F];
            }

            nDestLength += 3;
         }
      }
   }

   /* Add the NUL character */
   if (pDestString != 0)
   {
      * pDestString = 0;
   }

   return (nDestLength);
}


uint32_t static_PNDEFUtilConvertPrintableUTF16ToUTF16(
   tchar         * pDestString,
   const tchar   * pSourceString
   )
{
   bool     bInEscapement     = false;
   uint32_t nEscapementLength = 0;
   uint32_t nDestLength = 0;
   tchar    nCar, nOutCar = 0;

   PDebugTrace("static_PNDEFUtilConvertPrintableUTF16ToUTF16");

   CDebugAssert(pSourceString != null);

   while ((nCar = * pSourceString++) != 0)
   {
      if (bInEscapement == false)
      {
         if (nCar != '%')
         {
            if (pDestString != null)
            {
               * pDestString++ = nCar;
            }
            nDestLength++;
         }
         else
         {
            bInEscapement = true;
            nEscapementLength = (* pSourceString == 'u') ? 4 : 2;
            nOutCar = 0;
         }
      }
      else
      {
         nOutCar = nOutCar * 16;

         if ((nEscapementLength == 4) && (nCar == 'u'))
            continue;

         if ((nCar >= '0') && (nCar <= '9'))
         {
            nOutCar += nCar -'0';
         }
         else
         {
            nOutCar += nCar - 'A' + 10;
         }

         if (--nEscapementLength == 0)
         {
            if (pDestString != null)
            {
               * pDestString++ = nOutCar;
            }

            nDestLength++;

            bInEscapement = false;
         }
      }
   }

   /* Add the NUL character */
   if (pDestString != null)
   {
      * pDestString = 0;
   }

   return nDestLength;

}

/** \brief Convert a printable string into a printable URI string
  *
  * @param[in] pDestString The destination string
  *
  * @param[in] pBuffer The byte array
  *
  * @return The size of the destination string
  *
  */

uint32_t PNDEFUtilConvertPrintableUTF16ToUTF8(
   uint8_t    * pOutBuffer,
   const tchar      * pInstring)
{
   tchar     * pTempString;
   uint32_t    nLength;

   PDebugTrace("PNDEFUtilConvertPrintableUTF16ToUTF8");

   nLength = static_PNDEFUtilConvertPrintableUTF16ToUTF16(null, pInstring);

   pTempString = CMemoryAlloc((nLength+1) * sizeof(tchar));

   if (pTempString == null)
   {
      return (0);
   }

   static_PNDEFUtilConvertPrintableUTF16ToUTF16(pTempString, pInstring);

   nLength = PUtilConvertUTF16ToUTF8(pOutBuffer, pTempString, nLength);

   CMemoryFree(pTempString);
   return (nLength);
}

/** \brief Convert a byte array into a printable URI string
  *
  * @param[in] pDestString The destination string
  *
  * @param[in] pBuffer The byte array
  *
  * @param[in] nBufferLength The byte array length
  *
  * @return The size of the destination string
  *
  */

static uint32_t static_PNDEFUtilConvertByteArrayToPrintableURIString(
   tchar         * pDestString,
   const uint8_t * pBuffer,
   uint32_t        nBufferLength)
{
   tchar *  pTempString;
   uint32_t nSize;

   PDebugTrace("static_PNDEFUtilConvertByteArrayToPrintableURI");

   if (nBufferLength == 0)
   {
      return 0;
   }

   nSize = PNDEFUtilConvertByteArrayTo8BitCompatibleString(null, pBuffer, nBufferLength);

   pTempString = CMemoryAlloc((nSize + 1) * sizeof(tchar));

   if (pTempString == null)
   {
      return 0;
   }

   PNDEFUtilConvertByteArrayTo8BitCompatibleString(pTempString, pBuffer, nBufferLength);

   nSize = PUtilConvertStringToPrintableString(pDestString, pTempString);

   CMemoryFree(pTempString);

   return (nSize);
}

/** \brief Convert a byte array into a printable URI string
  *
  * @param[in] pDestString The destination string
  *
  * @param[in] pBuffer The byte array
  *
  * @param[in] nBufferLength The byte array length
  *
  * @return The size of the destination string
  *
  */

static uint32_t static_PNDEFUtilConvertUTF8BufferToPrintableURIString(
      tchar         * pDestString,
      const uint8_t * pBuffer,
      uint32_t        nBufferLength)
{
   tchar    * pTempString;
   uint32_t   nSize;

   PDebugTrace("static_PNDEFUtilConvertUTF8StringToPrintableURI");

   if (nBufferLength == 0)
   {
      return 0;
   }

   CDebugAssert(pBuffer != null);

   nSize = PUtilConvertUTF8ToUTF16(null, pBuffer, nBufferLength);

   if (nSize == 0)
   {
      return 0;
   }

   pTempString = CMemoryAlloc(nSize * sizeof(tchar));

   if (pTempString == null)
   {
      return 0;
   }

   nSize = PUtilConvertUTF8ToUTF16(pTempString, pBuffer, nBufferLength);

   nSize = PUtilConvertUTF16BufferToPrintableString(pDestString, pTempString, nSize);

   CMemoryFree(pTempString);

   return (nSize);
}

/* ****************************************************************************************************************
    String comparison functions
   **************************************************************************************************************** */

/** \brief Performs an insensitive string comparison between first n characters of two ASCII strings
  *
  * @param[in] pString1 The first string
  *
  * @param[in] pString2 The second string
  *
  * @param[in] nLength The number of character to compare
  *
  * @return 0 if the string are equal
  *         a negative value if first string is lesser than the second string in alphabetic order
  *         a positive value if the first string is greater than the second string in alphabetic order
  *
  */

sint32_t PUtilASCIIStringCaseInsensitiveNCompare(
   const tchar * pString1,
   const tchar * pString2,
   uint32_t      nLength)
{
   tchar nCar1, nCar2;

   PDebugTrace("PUtilASCIIStringCaseInsensitiveNCompare");

   CDebugAssert(pString1 != null);
   CDebugAssert(pString2 != null);

   while (nLength--)
   {
      nCar1 = * pString1;
      nCar2 = * pString2;

      if ((nCar1 >= 'a') && (nCar1 <= 'z'))
      {
         nCar1 = 'A' + nCar1 - 'a';
      }

      if ((nCar2 >= 'a') && (nCar2 <= 'z'))
      {
         nCar2 = 'A' + nCar2 - 'a';
      }

      if (nCar1 != nCar2)
      {
         return (* pString1 - * pString2);
      }

      pString1++;
      pString2++;
   }

   return (0);
}

/** \brief Performs an insensitive string comparison between two ASCII strings
  *
  * @param[in] pString1 The first string
  *
  * @param[in] pString2 The second string
  *
  * @param[in] nLength The number of character to compare
  *
  * @return 0 if the string are equal
  *         a negative value if first string is lesser than the second string in alphabetic order
  *         a positive value if the first string is greater than the second string in alphabetic order
  *
  */

sint32_t PUtilASCIIStringCaseInsensitiveCompare(
   const tchar * pString1,
   const tchar * pString2)
{
   tchar nCar1, nCar2;

   PDebugTrace("PUtilASCIIStringCaseInsensitiveCompare");

   CDebugAssert(pString1 != null);
   CDebugAssert(pString2 != null);

   while ((* pString1 != 0) || (* pString2 != 0))
   {
      nCar1 = * pString1;
      nCar2 = * pString2;

      if ((nCar1 >= 'a') && (nCar1 <= 'z'))
      {
         nCar1 = 'A' + nCar1 - 'a';
      }

      if ((nCar2 >= 'a') && (nCar2 <= 'z'))
      {
         nCar2 = 'A' + nCar2 - 'a';
      }

      if (nCar1 != nCar2)
      {
         return (* pString1 - * pString2);
      }

      pString1++;
      pString2++;
   }

   return (0);
}

/** \brief Performs a sensitive string comparison between first n characters of two ASCII strings
  *
  * @param[in] pString1 The first string
  *
  * @param[in] pString2 The second string
  *
  * @param[in] nLength The number of character to compare
  *
  * @return 0 if the string are equal
  *         a negative value if first string is lesser than the second string in alphabetic order
  *         a positive value if the first string is greater than the second string in alphabetic order
  *
  */

sint32_t PUtilAsciiStringCaseSensitiveNCompare(
   const tchar * pString1,
   const tchar * pString2,
   uint32_t      nLength)
{
   PDebugTrace("PUtilAsciiStringCaseSensitiveNCompare");

   CDebugAssert(pString1 != null);
   CDebugAssert(pString2 != null);

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

/** \brief Performs a sensitive string comparison between two ASCII strings
  *
  * @param[in] pString1 The first string
  *
  * @param[in] pString2 The second string
  *
  * @param[in] nLength The number of character to compare
  *
  * @return 0 if the string are equal
  *         a negative value if first string is lesser than the second string in alphabetic order
  *         a positive value if the first string is greater than the second string in alphabetic order
  *
  */

sint32_t PUtilAsciiStringCaseSensitiveCompare(
   const tchar * pString1,
   const tchar * pString2)
{

   PDebugTrace("PUtilAsciiStringCaseSensitiveCompare");

   CDebugAssert(pString1 != null);
   CDebugAssert(pString2 != null);

   /* both record type names shall be identical. The comparison is case-sensitive. */

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

/**
 * @brief Initialises pMessage pointer from message handle
 *
 * @param[in]  pContext The context
 *
 * @param[in]  hMessage The message handle
 *
 * @param[out]  ppMessage Address of message pointer to be initialized
 *
 * @return W_SUCCESS on success, other value on failure.
 */

static W_ERROR static_PNDEFGetMessageObject(
   tContext * pContext,
   W_HANDLE   hMessage,
   tNDEFMessage ** ppMessage)
{
   void * pTemp;
   W_ERROR nError;

   PDebugTrace("static_PNDEFGetMessageObject");

   if (hMessage == W_NULL_HANDLE)
   {
      return (W_ERROR_BAD_HANDLE);
   }

   nError = PHandleGetObject( pContext, hMessage, P_HANDLE_TYPE_NDEF_MESSAGE, &pTemp);

   if (nError != W_SUCCESS)
   {
      PDebugTrace("static_PNDEFGetMessageObject : PHandleGetObject() %s", PUtilTraceError(nError));
   }

   if (ppMessage != null)
   {
      * ppMessage = pTemp;
   }

   return (nError);
}

/* See header */
W_ERROR PNDEFGetRecordObject(
   tContext * pContext,
   W_HANDLE   hRecord,
   tNDEFRecord ** ppRecord)
{
   W_ERROR nError;
   void * pTemp;

   PDebugTrace("PNDEFGetRecordObject");

   if (hRecord == W_NULL_HANDLE)
   {
      return (W_ERROR_BAD_HANDLE);
   }

   nError = PHandleGetObject( pContext, hRecord, P_HANDLE_TYPE_NDEF_RECORD, &pTemp);

   if (nError != W_SUCCESS)
   {
      PDebugTrace("PNDEFGetRecordObject : PHandleGetObject() %s", PUtilTraceError(nError));
   }

   if (ppRecord != null)
   {
      * ppRecord = pTemp;
   }

   return (nError);
}

/**
 * @brief Initialises pMessage and pRecord pointers from message and record handles
 *
 * @param[in]  pContext The context
 *
 * @param[in]  hMessage The message handle
 *
 * @param[out]  ppMessage Address of message pointer to be initialized
 *
 * @param[in]  hRecord The record handle
 *
 * @param[out]  ppRecord Address of record pointer to be initialized
 *
 * @return W_SUCCESS on success, other value on failure.
 */

static W_ERROR static_PNDEFGetMessageAndRecordObjects(
   tContext * pContext,
   W_HANDLE   hMessage, tNDEFMessage ** ppMessage,
   W_HANDLE   hRecord,  tNDEFRecord  ** ppRecord)
{
   W_ERROR nError;

   PDebugTrace("static_PNDEFGetMessageAndRecordObjects");

   if ((nError = static_PNDEFGetMessageObject(pContext, hMessage, ppMessage)) != W_SUCCESS)
   {
      return (nError);
   }

   return PNDEFGetRecordObject( pContext, hRecord, ppRecord);
}


/**
 * @brief Frees a NDEF record object
 *
 * @param[in]  pNDEFRecord The record to be freed
 *
 */


static void static_PNDEFRecordFree(
   tNDEFRecord * pNDEFRecord)
{
   PDebugTrace("static_PNDEFRecordFree");

   if (pNDEFRecord != null)
   {
      if (pNDEFRecord->pRecordBuffer != null)
         CMemoryFree(pNDEFRecord->pRecordBuffer);

      CMemoryFree(pNDEFRecord);
   }
}

/**
 * @brief Clones a  NDEF record object
 *
 * @param[in]  pContext The context
 *
 * @param[in]  hRecord  The record handle
 *
 * @param[out] phNewRecord The address of a handle to be initialised
 *
 * @return W_SUCCESS on success, other value on failure.
 */

static W_ERROR static_PNDEFRecordClone(
   tContext * pContext,
   W_HANDLE   hRecord,
   W_HANDLE * phNewRecord)
{
   W_ERROR       nError;
   tNDEFRecord * pOldRecord;
   tNDEFRecord * pNewRecord = null;

   PDebugTrace("static_PNDEFRecordClone");

   CDebugAssert(phNewRecord != null);

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, &pOldRecord)) != W_SUCCESS)
   {
      goto error;
   }

   if ((pNewRecord = CMemoryAlloc(sizeof(*pNewRecord))) == null)
   {
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto error;
   }

   CMemoryFill(pNewRecord, 0, sizeof(* pNewRecord));

   pNewRecord->nRecordSize = pOldRecord->nRecordSize;
   if ((pNewRecord->pRecordBuffer = CMemoryAlloc(pNewRecord->nRecordSize)) == null)
   {
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto error;
   }

   /* copy the buffer contents */
   CMemoryCopy(pNewRecord->pRecordBuffer, pOldRecord->pRecordBuffer, pNewRecord->nRecordSize);

   pNewRecord->nIdentifierOffset = pOldRecord->nIdentifierOffset;
   pNewRecord->nIdentifierSize = pOldRecord->nIdentifierSize;

   pNewRecord->nTypeOffset = pOldRecord->nTypeOffset;
   pNewRecord->nTypeSize = pOldRecord->nTypeSize;

   pNewRecord->nPayloadOffset = pOldRecord->nPayloadOffset;
   pNewRecord->nPayloadSize = pOldRecord->nPayloadSize;

   if ((nError = PHandleRegister( pContext, pNewRecord, P_HANDLE_TYPE_NDEF_RECORD, phNewRecord)) != W_SUCCESS)
   {
      goto error;
   }

   return (W_SUCCESS);

error:

   * phNewRecord = W_NULL_HANDLE;
   static_PNDEFRecordFree(pNewRecord);
   return (nError);
}



/* See Client API */
W_ERROR PNDEFCreateRecord(
         tContext* pContext,
         uint8_t nTNF,
         const tchar* pTypeString,
         const uint8_t* pPayloadBuffer,
         uint32_t nPayloadLength,
         W_HANDLE* phRecord )
{
   tNDEFRecord * pRecord = null;
   W_ERROR       nError = W_SUCCESS;
   uint8_t       byteHeader;
   uint32_t      nTypeLength;
   uint32_t      nRecordSize;
   uint32_t      nRecordOffset;

   /* Check Parameters */

   PDebugTrace("PNDEFCreateRecord");

   if (phRecord != null)
   {
      * phRecord = W_NULL_HANDLE;
   }

   if ((phRecord == null) || ((pPayloadBuffer == null) && (nPayloadLength != 0)) || ((pPayloadBuffer != null) && (nPayloadLength == 0)))
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   /* TYPE */
   if (pTypeString != null)
   {
      switch (nTNF)
      {
         case W_NDEF_TNF_WELL_KNOWN :

            if (static_PNDEFUtilStringIsWellKnownCompatible(pTypeString) == false)
            {
               PDebugError("PNDEFCreateRecord : pTypeString must used only characters in the range [0x20-0x7E]");
               return W_ERROR_BAD_PARAMETER;
            }
            break;

         case W_NDEF_TNF_EXTERNAL :

            if (static_PNDEFUtilStringIsExternalCompatible(pTypeString) == false)
            {
               PDebugError("PNDEFCreateRecord : pTypeString must used only characters in the range [0x20-0x7E]");
               return W_ERROR_BAD_PARAMETER;
            }
            break;

         default:

            /* For other types, the character range is limited to [0x0000-0x00FF] (8 bit compatible) */

            if (static_PNDEFUtilStringIs8BitsCompatible(pTypeString) == false)
            {
               PDebugError("PNDEFCreateRecord : pTypeString must used only characters in the range [0x00-0xFF]");
               return W_ERROR_BAD_PARAMETER;
            }
            break;
      }


      /* More processing according to the NDEF type */

      switch (nTNF)
      {
         case W_NDEF_TNF_WELL_KNOWN :

            /* remove the "urn:nfc:wkt:" prefix if present */

            if (PUtilStringNCompare(pTypeString, g_aNDEFWellKnownPrefix, g_nNDEFWellKnownPrefixLengh) == 0)
            {
               pTypeString += g_nNDEFWellKnownPrefixLengh;
            }

            break;

         case W_NDEF_TNF_EXTERNAL :

            /* remove the "urn:nfc:ext:" prefix if present */

            if (PUtilStringNCompare(pTypeString, g_aNDEFExternalPrefix, g_nNDEFExternalPrefixLengh) == 0)
            {
               pTypeString += g_nNDEFExternalPrefixLengh;
            }
            break;

         case W_NDEF_TNF_ABSOLUTE_URI :

            if (PNDEFUtilStringIsPrintableURI(pTypeString) == false)
            {
               PDebugError("PNDEFCreateRecord : pTypeString must contain a printable URI");
               return W_ERROR_BAD_PARAMETER;
            }
            break;
      }

      /* Ensure the length of the type string fits in a byte */

      if ( (nTypeLength=PUtilStringLength(pTypeString)) > 255 )
      {
         PDebugError("PNDEFCreateRecord : pTypeString length must be <= 255");
         return W_ERROR_BAD_PARAMETER;
      }
   }
   else
   {
      nTypeLength = 0;
   }


   /* Check Type Name Format (TNF) */

   switch ( nTNF )
   {
      case W_NDEF_TNF_EMPTY:

         if ( (nTypeLength!=0) ||(nPayloadLength!=0) )
         {
            return W_ERROR_BAD_PARAMETER;
         }
         break;

      case W_NDEF_TNF_WELL_KNOWN:
      case W_NDEF_TNF_MEDIA:
      case W_NDEF_TNF_ABSOLUTE_URI:
      case W_NDEF_TNF_EXTERNAL:

         if ( nTypeLength == 0 )
         {
            return W_ERROR_BAD_PARAMETER;
         }

         break;

      case W_NDEF_TNF_UNKNOWN:
      case W_NDEF_TNF_UNCHANGED:

         if ( nTypeLength != 0 )
         {
            return W_ERROR_BAD_PARAMETER;
         }
         break;

      default:
         return W_ERROR_BAD_PARAMETER;
   }

   /* Every thing is OK, create the NDEF record */

   byteHeader = nTNF & 0x07;

   if (nPayloadLength >= 256)
   {
      /* long payload, payload size is stored on 4 bytes */

      nRecordSize = 6 + nTypeLength + nPayloadLength;
   }
   else
   {
      byteHeader |= 0x10;        /* Set SR: Short Record */
      nRecordSize = 3 + nTypeLength + nPayloadLength;
   }

   /**
    * Construct new record according to input parameters
    */

   pRecord = (tNDEFRecord *)CMemoryAlloc(sizeof(tNDEFRecord));

   if ( pRecord == null)
   {
      return W_ERROR_OUT_OF_RESOURCE;
   }

   CMemoryFill(pRecord, 0, sizeof(*pRecord));

   pRecord->nRecordSize = nRecordSize;
   pRecord->pRecordBuffer = (uint8_t *)CMemoryAlloc(nRecordSize);

   if (pRecord->pRecordBuffer == null )
   {
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto error_cleanup;
   }

   /* Fills in the record buffer */

   nRecordOffset = 0;

   /* Copy HEADER byte */
   pRecord->pRecordBuffer[nRecordOffset++] = byteHeader | 0xC0;
   /* Copy TYPE Length */
   pRecord->pRecordBuffer[nRecordOffset++] = (uint8_t)nTypeLength;
   /* Copy PAYLOAD Length */
   if ( nPayloadLength < 256 )
   {
      pRecord->pRecordBuffer[nRecordOffset++] = (uint8_t)nPayloadLength;
   }
   else
   {
      pRecord->pRecordBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0xFF000000) >> 24);
      pRecord->pRecordBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0x00FF0000) >> 16);
      pRecord->pRecordBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0x0000FF00) >>  8);
      pRecord->pRecordBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0x000000FF) >>  0);
   }

   /* Copy TYPE content */

   pRecord->nTypeOffset = nRecordOffset;
   pRecord->nTypeSize = nTypeLength;

   if (pRecord->nTypeSize != 0)
   {
      nRecordOffset += static_PNDEFUtilConvert8BitCompatibleStringToByteArray(pTypeString, pRecord->pRecordBuffer + nRecordOffset);
   }

   /* NO ID */

   pRecord->nIdentifierOffset = nRecordOffset;
   pRecord->nIdentifierSize = 0;

   /* Copy PAYLOAD */

   pRecord->nPayloadOffset = nRecordOffset;
   pRecord->nPayloadSize   = nPayloadLength;

   if( nPayloadLength > 0)
   {
      CMemoryCopy( &pRecord->pRecordBuffer[nRecordOffset], pPayloadBuffer, nPayloadLength);
      nRecordOffset += nPayloadLength;
   }


   /*Register new record handle*/
   nError = PHandleRegister( pContext, pRecord, P_HANDLE_TYPE_NDEF_RECORD, phRecord);

   if (nError != W_SUCCESS)
   {
      goto error_cleanup;
   }

   return (W_SUCCESS);

error_cleanup:

   static_PNDEFRecordFree(pRecord);
   return (nError);
}

/* See Client API */
W_ERROR PNDEFBuildRecord(
         tContext* pContext,
         const uint8_t* pBuffer,
         uint32_t nBufferLength,
         W_HANDLE* phRecord)
{
   W_ERROR  nError;
   uint32_t nResult;
   uint32_t nTypeOffset;
   uint32_t nTypeLength;
   uint32_t nIDOffset;
   uint32_t nIDLength;
   uint32_t nPayloadOffset;
   uint32_t nPayloadLength;
   tNDEFRecord * pRecord = null;

   PDebugTrace("PNDEFBuildRecord");

   if ((pBuffer == null) || (nBufferLength == 0) || (phRecord == null))
   {
      PDebugError("PNDEFBuildRecord : BAD PARAMETER");
      nError = W_ERROR_BAD_PARAMETER;
      goto error_cleanup;
   }

   /* Parse the the buffer */
   nResult = PNDEFParseBuffer(pBuffer, nBufferLength, &nTypeOffset, & nTypeLength, & nIDOffset, & nIDLength, & nPayloadOffset, & nPayloadLength);

   if (nResult == 0)
   {
      PDebugError("PNDEFBuildRecord : PNDEFParseBuffer failed");
      nError = W_ERROR_BAD_NDEF_FORMAT;
      goto error_cleanup;
   }

   if ((pRecord = CMemoryAlloc(sizeof(* pRecord))) == null)
   {
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto error_cleanup;
   }

   CMemoryFill(pRecord, 0, sizeof(* pRecord));


   pRecord->pRecordBuffer = CMemoryAlloc(nResult);

   if (pRecord->pRecordBuffer == null)
   {
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto error_cleanup;
   }

   pRecord->nRecordSize = nResult;
   CMemoryCopy(pRecord->pRecordBuffer, pBuffer, nResult);

   pRecord->nTypeOffset = nTypeOffset;
   pRecord->nTypeSize = nTypeLength;

   pRecord->nIdentifierOffset = nIDOffset;
   pRecord->nIdentifierSize = nIDLength;

   pRecord->nPayloadOffset = nPayloadOffset;
   pRecord->nPayloadSize = nPayloadLength;

   /*Register new record handle*/

   nError = PHandleRegister( pContext, pRecord, P_HANDLE_TYPE_NDEF_RECORD, phRecord);

   if (nError != W_SUCCESS)
   {
      goto error_cleanup;
   }

   return (W_SUCCESS);

error_cleanup :

   if (phRecord != null)
   {
      * phRecord = W_NULL_HANDLE;
   }

   static_PNDEFRecordFree(pRecord);
   return nError;
}

/* See Client API */
W_ERROR PNDEFSetRecordInfo(
         tContext* pContext,
         W_HANDLE hRecord,
         uint32_t nInfoType,
         const uint8_t* pBuffer,
         uint32_t nBufferLength)
{
   tNDEFRecord   * pRecord;
   W_ERROR         nError;
   uint8_t       * pTempBuffer = null;
   uint32_t        nRequiredSize;
   uint32_t        nPayloadLength;
   const uint8_t * pPayload;
   uint32_t        nIdLength;
   uint32_t        nRecordOffset;
   const uint8_t * pIdentifier;

   PDebugTrace("PNDEFSetRecordInfo");

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, &pRecord)) != W_SUCCESS)
   {
      goto error;
   }

   if ( ((pBuffer == null) && (nBufferLength != 0)) ||
        ((pBuffer != null) && (nBufferLength == 0)))
   {
      PDebugError ("PNDEFSetRecordInfo : pBuffer / nBufferLength is invalid");
      nError = W_ERROR_BAD_PARAMETER;
      goto error;
   }

   nRequiredSize = pRecord->nRecordSize;

   switch (nInfoType)
   {
      case W_NDEF_INFO_IDENTIFIER_UTF8 :
         {
            uint32_t nRecordTNF = pRecord->pRecordBuffer[0] & 0x07;

            /* specific case for NDEF_EMPTY
               it is not allowed to add an identifier to this record */

            if (nRecordTNF == W_NDEF_TNF_EMPTY) {

               if (pBuffer != null)
               {
                  PDebugError ("PNDEFSetRecordInfo : setting an identifier into an NDEF EMPTY is not allowed");
                  nError = W_ERROR_BAD_NDEF_FORMAT;
                  goto error;
               }
            }

            if (pRecord->nIdentifierSize != 0)
            {
               nRequiredSize -= 1 + pRecord->nIdentifierSize;       /* remove ID length and ID payload from the current record size */
            }

            if (pBuffer != null)
            {
               nRequiredSize += 1 + nBufferLength;                   /* add ID length and ID payload to the record size */
            }

            nPayloadLength = pRecord->nPayloadSize;
            pPayload       = & pRecord->pRecordBuffer[pRecord->nPayloadOffset];

            nIdLength   = nBufferLength;
            pIdentifier =  pBuffer;
         }
         break;

      case W_NDEF_INFO_PAYLOAD:
         {
            uint32_t nRecordTNF = pRecord->pRecordBuffer[0] & 0x07;

            /* specific case for NDEF_EMPTY
               it is not allowed to add an identifier to this record */

            if (nRecordTNF == W_NDEF_TNF_EMPTY) {

               if (pBuffer != null)
               {
                  PDebugError ("PNDEFSetRecordInfo : setting an identifier into an NDEF EMPTY is not allowed");
                  nError = W_ERROR_BAD_NDEF_FORMAT;
                  goto error;
               }
            }

            if (pRecord->nPayloadSize >= 256)
            {
               nRequiredSize -= 4 + pRecord->nPayloadSize;   /* remove payload length and current payload from the current record size */
            }
            else
            {
               nRequiredSize -= 1 + pRecord->nPayloadSize;   /* remove payload length and current payload from the current record size (Short Record variant) */
            }

            if (nBufferLength >= 256)
            {
               nRequiredSize += 4 + nBufferLength;
            }
            else
            {
               nRequiredSize += 1 + nBufferLength;
            }

            nPayloadLength = nBufferLength;
            pPayload = pBuffer;

            nIdLength = pRecord->nIdentifierSize;
            pIdentifier  = & pRecord->pRecordBuffer[pRecord->nIdentifierOffset];
         }
         break;

      default :

         PDebugError ("PNDEFSetRecordInfo : nInfoType is invalid");
         nError = W_ERROR_BAD_PARAMETER;
         goto error;
   }

   /* Rebuild the new record */

   pTempBuffer = CMemoryAlloc(nRequiredSize);

   if (pTempBuffer == null)
   {
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto error;
   }

   nRecordOffset = 0;

   /* Copy HEADER byte */
   pTempBuffer[nRecordOffset++] = pRecord->pRecordBuffer[0];

   /* Copy TYPE Length */
   pTempBuffer[nRecordOffset++] = pRecord->pRecordBuffer[1];

   /* Copy PAYLOAD Length */
   if ( nPayloadLength < 256 )
   {
      pTempBuffer[nRecordOffset++] = (uint8_t)nPayloadLength;

      /* Set SR in the header */
      pTempBuffer[0] |= 0x10;
   }
   else
   {
      pTempBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0xFF000000) >> 24);
      pTempBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0x00FF0000) >> 16);
      pTempBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0x0000FF00) >>  8);
      pTempBuffer[nRecordOffset++] = (uint8_t)((nPayloadLength & 0x000000FF) >>  0);

      /* Reset SR in the header */
      pTempBuffer[0] &= ~0x10;
   }

   /* Set ID length */
   if (nIdLength != 0)
   {
      pTempBuffer[nRecordOffset++] = (uint8_t) nIdLength;

      /* Set IL in the header */
      pTempBuffer[0] |= 0x08;
   }
   else
   {
      /* Reset IL in the header */
      pTempBuffer[0] &= ~0x08;
   }

   /* Copy TYPE */
   CMemoryCopy(&pTempBuffer[nRecordOffset], pRecord->pRecordBuffer + pRecord->nTypeOffset, pRecord->nTypeSize);
   pRecord->nTypeOffset = nRecordOffset;
   nRecordOffset += pRecord->nTypeSize;

   /* Copy ID */

   pRecord->nIdentifierOffset = nRecordOffset;
   pRecord->nIdentifierSize = nIdLength;

   if (nIdLength != 0)
   {
      CMemoryCopy(&pTempBuffer[nRecordOffset], pIdentifier, nIdLength);
      nRecordOffset += nIdLength;
   }

   /* Copy Payload */

   pRecord->nPayloadOffset = nRecordOffset;
   pRecord->nPayloadSize = nPayloadLength;

   if (nPayloadLength != 0)
   {
      CMemoryCopy(&pTempBuffer[nRecordOffset], pPayload, nPayloadLength);
      nRecordOffset += nPayloadLength;
   }

   /* Update the record */

   CMemoryFree(pRecord->pRecordBuffer);

   pRecord->nRecordSize = nRequiredSize;
   pRecord->pRecordBuffer = pTempBuffer;

   return (W_SUCCESS);

error:

   if (pTempBuffer != null)
   {
      CMemoryFree(pTempBuffer);
   }

   return (nError);
}

/* See Client API */
W_ERROR PNDEFSetIdentifierString(
         tContext* pContext,
         W_HANDLE hRecord,
         const tchar* pIdentifierString)
{
   W_ERROR  nError;
   uint32_t nUTF8Length;
   uint8_t  * pBuffer = null;

   PDebugTrace("PNDEFSetIdentifierString");

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, null)) != W_SUCCESS)
   {
      return (nError);
   }

   if (pIdentifierString != null)
   {
      uint32_t nLength;

      nLength = PUtilStringLength(pIdentifierString);

      if (nLength == 0)
      {
         PDebugError("PNDEFSetIdentifierString : pIdentifierString length is zero");
         return W_ERROR_BAD_PARAMETER;
      }

      if (PNDEFUtilStringIsPrintableURI(pIdentifierString) == false)
      {
         PDebugError("PNDEFSetIdentifierString : pTypeString be use printable URI format");
         return W_ERROR_BAD_PARAMETER;
      }

      /* Convert the UTF16 printable URI in UTF8 */
      nUTF8Length = PNDEFUtilConvertPrintableUTF16ToUTF8(null, pIdentifierString);

      if ((pBuffer = CMemoryAlloc(nUTF8Length)) == null)
      {
         PDebugError("PNDEFSetIdentifierString : CMemoryAlloc failed");
         return (W_ERROR_OUT_OF_RESOURCE);
      }

      nUTF8Length = PNDEFUtilConvertPrintableUTF16ToUTF8(pBuffer, pIdentifierString);
   }
   else
   {
      pBuffer     = null;
      nUTF8Length = 0;
   }

   nError = PNDEFSetRecordInfo(pContext, hRecord, W_NDEF_INFO_IDENTIFIER_UTF8, pBuffer, nUTF8Length);

   CMemoryFree(pBuffer);
   return (nError);
}

/**
 * Check if a message does not contain a record with the same ID
 *
 * param[in] pContext the context
 *
 * param[in] hMessage the message handle
 *
 * param[in] hRecord the record handle
 *
 * param[in] nExcludedRecordIdx if not set to 0xFFFFFFFF, do not check this index
 */

static W_ERROR static_PNDEFCheckRecordUnicity(
   tContext * pContext,
   W_HANDLE   hMessage,
   W_HANDLE   hRecord,
   uint32_t   nExcludedRecordIdx)
{
   tNDEFMessage          * pMessage;
   tNDEFRecord           * pRecord;
   tNDEFRecordHandleList * pCurrentRecordHandleItem;
   W_ERROR                 nError;
   tchar                 * pPrintableIdentifierString = null;
   tchar                 * pPrintableIdentifierString2 = null;
   uint32_t                nSize;
   uint32_t                nIndex;

   PDebugTrace("static_PNDEFCheckRecordUnicity");

   if ((nError = static_PNDEFGetMessageAndRecordObjects(pContext, hMessage, &pMessage, hRecord, &pRecord)) != W_SUCCESS)
   {
      PDebugTrace("static_PNDEFCheckRecordUnicity : static_PNDEFGetMessageAndRecordObjects %s", PUtilTraceError(nError));
      return (nError);
   }

   if ((nError = PNDEFGetRecordInfo(pContext, hRecord,  W_NDEF_INFO_IDENTIFIER_PRINT, & nSize)) != W_SUCCESS)
   {
      goto end;
   }

   if (nSize == 0)
   {
      return (W_SUCCESS);
   }

   pPrintableIdentifierString = CMemoryAlloc((nSize+1)* sizeof(tchar));

   if ((PNDEFGetIdentifierString(pContext, hRecord, pPrintableIdentifierString, (nSize+1),& nSize)) != W_SUCCESS)
   {
      goto end;
   }

   nIndex = 0;

   for (pCurrentRecordHandleItem = pMessage->pRecordHandleListBegin; pCurrentRecordHandleItem != null; pCurrentRecordHandleItem = pCurrentRecordHandleItem->pNext)
   {
      /* do not check the excluded record idx */
      if (nIndex++ == nExcludedRecordIdx)
      {
         PDebugTrace("static_PNDEFCheckRecordUnicity : skipping record index %d", nExcludedRecordIdx);
         continue;
      }

      if ((nError = PNDEFGetRecordInfo(pContext, pCurrentRecordHandleItem->hRecord,  W_NDEF_INFO_IDENTIFIER_PRINT, & nSize)) != W_SUCCESS)
      {
         goto end;
      }

      if (nSize == 0)
         continue;

      CMemoryFree(pPrintableIdentifierString2);
      pPrintableIdentifierString2 = CMemoryAlloc((nSize+1)* sizeof(tchar));

      if ((nError = PNDEFGetIdentifierString(pContext, pCurrentRecordHandleItem->hRecord, pPrintableIdentifierString2, (nSize+1),& nSize)) != W_SUCCESS)
      {
         break;
      }

      if (PUtilStringCompare(pPrintableIdentifierString, pPrintableIdentifierString2) == 0)
      {
         /* this identifier string is already present in the message */
         PDebugError("static_PNDEFCheckRecordUnicity : duplicate identifier string");
         nError = W_ERROR_BAD_NDEF_FORMAT;
         break;
      }

   }

end:
   CMemoryFree(pPrintableIdentifierString);
   CMemoryFree(pPrintableIdentifierString2);

   return (nError);
}

W_ERROR static_PNDEFInsertRecord(
                  tContext* pContext,
                  W_HANDLE hMessage,
                  uint32_t nIndex,
                  W_HANDLE hRecord,
                  bool     bAllowAppend);

/* See Client API */
W_ERROR PNDEFAppendRecord(
         tContext* pContext,
         W_HANDLE hMessage,
         W_HANDLE hRecord)
{
   tNDEFMessage          * pMessage;
   tNDEFRecord           * pRecord;
   uint32_t                nRecordNb;
   W_ERROR                 nError;

   PDebugTrace("PNDEFAppendRecord");

   if ((nError = static_PNDEFGetMessageAndRecordObjects(pContext, hMessage, &pMessage, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   if ((nError = static_PNDEFCheckRecordUnicity(pContext, hMessage, hRecord, (uint32_t) ~0)) != W_SUCCESS)
   {
      return (nError);
   }

   nRecordNb = static_PNDEFGetRecordNumber(pMessage);

   nError = static_PNDEFInsertRecord(pContext, hMessage, nRecordNb, hRecord, true);

   return (nError);
}

/* See Client API */
W_ERROR PNDEFCreateNestedMessageRecord(
                  tContext* pContext,
                  uint8_t nTNF,
                  const tchar* pTypeString,
                  W_HANDLE hNestedMessage,
                  W_HANDLE* phRecord)
{
   uint8_t * pBuffer = null;
   uint32_t  nLength;
   W_ERROR   nError;

   PDebugTrace("PNDEFCreateNestedMessageRecord");

   if (phRecord == null)
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto error_cleanup;
   }

   pBuffer = null;
   nLength = 0;

   if (hNestedMessage != W_NULL_HANDLE)
   {
      /* check the validity ot the provided hNestedHandle */

      if ((nError = static_PNDEFGetMessageObject(pContext, hNestedMessage, null)) != W_SUCCESS)
      {
         goto error_cleanup;
      }

      if (PNDEFGetRecordNumber(pContext, hNestedMessage) != 0)
      {
         /* retrieve the whole content of the message to be put in the record */

         if ((nLength = PNDEFGetMessageLength(pContext, hNestedMessage)) == 0)
         {
            nError = W_ERROR_BAD_HANDLE;
            goto error_cleanup;
         }


         if ((pBuffer = CMemoryAlloc(nLength)) == null)
         {
            nError =  W_ERROR_OUT_OF_RESOURCE;
            goto error_cleanup;
         }

         if ((nError = PNDEFGetMessageContent(pContext, hNestedMessage, pBuffer, nLength, &nLength)) != W_SUCCESS)
         {
            goto error_cleanup;
         }

         /* create a new record with payload corresponding to the message contents */
      }
   }

   if ((nError = PNDEFCreateRecord(pContext, nTNF, pTypeString, pBuffer, nLength, phRecord)) != W_SUCCESS)
   {
      goto error_cleanup;
   }

   CMemoryFree(pBuffer);

   return (W_SUCCESS);

error_cleanup:

   if (phRecord != null)
   {
      * phRecord = W_NULL_HANDLE;
   }
   CMemoryFree(pBuffer);

   return (nError);
}

uint32_t PNDEFGetRecordLength(
         const uint8_t* pBuffer,
         uint32_t nBufferSize)
{
   uint32_t  nTypeLength;
   uint32_t  nIDLength;
   uint32_t nPayloadLength;
   uint32_t nOffset = 2;

   PDebugTrace("PNDEFGetRecordLength");

   if( nBufferSize < 3 )
   {
      PDebugError("PNDEFGetRecordLength: buffer too short");
      return 0;
   }

   nTypeLength = pBuffer[1];

   /* Short Record ? */
   if ( (pBuffer[0]&0x10) != 0 )
   {
      nPayloadLength = pBuffer[nOffset++];
   }
   else
   {
      uint32_t i;
      if( nBufferSize < (nOffset+4) )
      {
         PDebugError("PNDEFGetRecordLength: buffer too short (not SR record)");
         return 0;
      }

      nPayloadLength = 0;
      for (i=0; i<4 ;i++)
      {
         nPayloadLength <<= 8;
         nPayloadLength |= (uint32_t)pBuffer[ nOffset++ ];
      }
   }

   /* Identifier ? */
   if ( (pBuffer[0]&0x08) != 0 )
   {
      if( nBufferSize < (nOffset+1) )
      {
         PDebugError("PNDEFGetRecordLength: buffer too short (for getting ID Length)");
         return 0;
      }
      nIDLength = pBuffer[ nOffset++ ];
   }
   else
   {
      nIDLength = 0;
   }

   nOffset = nOffset + nTypeLength + nIDLength + nPayloadLength;

   if( nOffset > nBufferSize )
   {
      PDebugError("PNDEFGetRecordLength: buffer too short (0x%x) OR wrong length (0x%x)", nBufferSize, nOffset);
      return 0;
   }

   return nOffset;
}

uint32_t PNDEFParseSafeBuffer(
         uint8_t* pBuffer,
         uint32_t nBufferSize,
         uint32_t  *pnTypeOffset,
         uint32_t  *pnTypeLength,
         uint32_t  *pnIDOffset,
         uint32_t  *pnIDLength,
         uint32_t  *pnPayloadOffset,
         uint32_t *pnPayloadLength)
{
   uint32_t  nTypeOffset;
   uint32_t  nTypeLength;
   uint32_t  nIDOffset;
   uint32_t  nIDLength;
   uint32_t  nPayloadOffset;
   uint32_t nPayloadLength;

   uint32_t nOffset = 2;

   PDebugTrace("PNDEFParseSafeBuffer");

   CDebugAssert( nBufferSize >= 3 );

   nTypeLength = pBuffer[1];

   /* Short Record ? */
   if ( (pBuffer[0]&0x10) != 0 )
   {
      nPayloadLength = pBuffer[nOffset++];
   }
   else
   {
      uint32_t i;
      CDebugAssert( nBufferSize >= (nOffset+4) );
      nPayloadLength = 0;
      for (i=0; i<4 ;i++)
      {
         nPayloadLength <<= 8;
         nPayloadLength |= (uint32_t)pBuffer[ nOffset++ ];
      }
   }

   /* Identifier ? */
   if ( (pBuffer[0]&0x08) != 0 )
   {
      CDebugAssert( nBufferSize >= (nOffset+1) );
      nIDLength = pBuffer[ nOffset++ ];
   }
   else
   {
      nIDLength = 0;
   }

   nTypeOffset = (nTypeLength>0) ? nOffset : 0;
   nOffset += nTypeLength;

   nIDOffset   = (nIDLength>0) ? nOffset : 0;
   nOffset += nIDLength;

   nPayloadOffset = nOffset;

   if ( pnTypeOffset != null )
   {
      *pnTypeOffset = nTypeOffset;
   }
   if ( pnTypeLength != null )
   {
      *pnTypeLength = nTypeLength;
   }
   if ( pnIDOffset != null )
   {
      *pnIDOffset = nIDOffset;
   }
   if ( pnIDLength != null )
   {
      *pnIDLength = nIDLength;
   }
   if ( pnPayloadOffset )
   {
      *pnPayloadOffset = nPayloadOffset;
   }
   if ( pnPayloadLength )
   {
      *pnPayloadLength = nPayloadLength;
   }

   CDebugAssert( nOffset + nPayloadLength <= nBufferSize );

   return nOffset;
}


/* Destroy a NDEF message object */
static uint32_t static_PNDEFDestroyNDEFMessage(
         tContext* pContext,
         void* pObject )
{
   tNDEFMessage* pMessage = (tNDEFMessage*)pObject;
   tNDEFRecordHandleList* pHandleElement;
   tNDEFRecordHandleList* pNextElement;
   tNDEFRecord* pRecord;
   W_ERROR nError;

   W_HANDLE      hCurrentMessage;
   tNDEFMessage* pCurrentMessage;


   PDebugTrace("static_PNDEFDestroyNDEFMessage");

   pHandleElement = pMessage->pRecordHandleListBegin;
   while (pHandleElement != null)
   {
      nError = PHandleGetObject(pContext, pHandleElement->hRecord,
         P_HANDLE_TYPE_NDEF_RECORD, (void**)&pRecord);

      if ((nError == W_SUCCESS) && (pRecord != null))
      {
         PHandleClose(pContext, pHandleElement->hRecord);
      }

      pNextElement = pHandleElement->pNext;
      CMemoryFree(pHandleElement);
      pHandleElement = pNextElement;
   }

   /* walk through the  linked message list to decrement their ref count */

   hCurrentMessage = pMessage->hMessageNext;

   while (hCurrentMessage != W_NULL_HANDLE)
   {
      if ((static_PNDEFGetMessageObject(pContext, hCurrentMessage, &pCurrentMessage)) == W_SUCCESS)
      {
         hCurrentMessage = pCurrentMessage->hMessageNext;
         PHandleDecrementReferenceCount(pContext, pCurrentMessage);
      }
      else
      {
         CDebugAssert(0);
      }
   }

   CMemoryFree(pMessage);

   return P_HANDLE_DESTROY_DONE;
}


/* Destroy a NDEF record object */
static uint32_t static_PNDEFDestroyNDEFRecord(
         tContext* pContext,
         void* pObject )
{
   tNDEFRecord* pRecord = (tNDEFRecord*)pObject;

   PDebugTrace("static_PNDEFDestroyNDEFRecord");

   static_PNDEFRecordFree(pRecord);

   return P_HANDLE_DESTROY_DONE;
}

static uint32_t static_PNDEFGetRecordNumber(tNDEFMessage * pMessage)
{
   CDebugAssert(pMessage != null);

   PDebugTrace("static_PNDEFGetRecordNumber");

   return (pMessage->nRecordCount);
}


/* See Client API Specifications */
uint32_t PNDEFGetRecordNumber(
                  tContext* pContext,
                  W_HANDLE hMessage )
{
   tNDEFMessage * pMessage;
   PDebugTrace("PNDEFGetRecordNumber");

   if (static_PNDEFGetMessageObject(pContext, hMessage, &pMessage) == W_SUCCESS)
   {
      return (static_PNDEFGetRecordNumber(pMessage));
   }

   return (0);
}

uint32_t PUtilConvertASCIIToUTF16(
   tchar          * pDestUTF16,
   const uint8_t  * pSourceASCII,
   uint32_t         nSourceLength )
{
   uint32_t         i;

   PDebugTrace("PUtilConvertASCIIToUTF16");

   if (pDestUTF16 == null)
   {
      return (nSourceLength);
   }

   /* ASCII to UTF16 conversion */
   for (i=0; i<nSourceLength; i++)
   {
      * pDestUTF16++ = * pSourceASCII++;
   }

   return (nSourceLength);
}


/* See Client API Specifications */
bool PNDEFCheckType(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  uint8_t nTNF,
                  const tchar* pTypeString)
{
   tNDEFRecord * pRecord;
   uint32_t      nRecordTNF;
   tchar       * pRecordTypeString = null;
   uint32_t      nLength;
   bool          bResult;

   PDebugTrace("PNDEFCheckType");

   if (PNDEFGetRecordObject(pContext, hRecord, &pRecord) != W_SUCCESS)
   {
      return (false);
   }

   if (nTNF == W_NDEF_TNF_ANY_TYPE)
   {
      return (false);
   }

   nRecordTNF = pRecord->pRecordBuffer[0] & 0x07;

   if (pRecord->nTypeSize != 0) {

      nLength = PUtilConvertASCIIToUTF16(null, pRecord->pRecordBuffer + pRecord->nTypeOffset, pRecord->nTypeSize);

      pRecordTypeString = CMemoryAlloc((nLength+1) * sizeof(tchar));

      if (pRecordTypeString == null)
      {
         /* out of resource, return false */
         PDebugError("PNDEFCheckType : CMemoryAlloc failed");
         return (false);
      }

      PUtilConvertASCIIToUTF16(pRecordTypeString, pRecord->pRecordBuffer + pRecord->nTypeOffset, pRecord->nTypeSize);

      /* Add ending NUL character */
      pRecordTypeString[nLength] = 0x00;
   }

   bResult = WNDEFCompareRecordType(nTNF, pTypeString, (uint8_t) nRecordTNF, pRecordTypeString);

   /* Free the pRecordTypeString */

   if (pRecordTypeString != null)
   {
      CMemoryFree(pRecordTypeString);
   }

   return (bResult);
}

uint32_t PUTilConvertUTF8URIToPrintableUTF16URI(
   tchar          * pDestString,
   const uint8_t  * pSourceBuffer,
   uint32_t         nSourceLength)
{
   uint32_t         nLength;
   tchar          * pUTF16URI;

   PDebugTrace("PUTilConvertUTF8URIToPrintableUTF16URI");

   /* The source buffer contains the URI in UTF8, first convert in to UTF-16 */

   if ((nLength = PUtilConvertUTF8ToUTF16(null, pSourceBuffer, nSourceLength)) == 0)
   {
      return (0);
   }

   if ((pUTF16URI = CMemoryAlloc(nLength* sizeof(tchar))) == null)
   {
      return (0);
   }

   nLength = PUtilConvertUTF8ToUTF16(pUTF16URI, pSourceBuffer, nSourceLength);

   /* now pUTF16URI contains the URI in UTF16, convert it in a UT16 printable URI */

   nLength = PUtilConvertStringToPrintableString(pDestString, pUTF16URI);

   CMemoryFree(pUTF16URI);

   return (nLength);
}


uint32_t PUTilConvertASCIIURIToPrintableUT16URI(
      tchar          * pDestString,
      const uint8_t  * pSourceBuffer,
      uint32_t         nSourceLength)
{
   tchar    * pUTF16URI;
   uint32_t   i;
   uint32_t   nLength;

   PDebugTrace("PUTilConvertASCIIURIToPrintableUT16URI");

   if ((pUTF16URI = CMemoryAlloc(nSourceLength * sizeof (tchar))) == null)
   {
      return (0);
   }

   /* convert the ASCII into UTF16 */

   for (i=0; i<nSourceLength; i++)
   {
      pUTF16URI[i] = pSourceBuffer[i];
   }

   nLength = PUtilConvertStringToPrintableString(pDestString, pUTF16URI);

   CMemoryFree(pUTF16URI);

   return (nLength);
}



/* See Client API Specifications */
W_ERROR PNDEFGetRecordInfo(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  uint32_t nInfoType,
                  uint32_t* pnValue)
{
   W_ERROR       nError;
   tNDEFRecord * pRecord;

   PDebugTrace("PNDEFGetRecordInfo");

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, &pRecord)) != W_SUCCESS)
   {
      goto error;
   }

   if (pnValue == null)
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto error;
   }

   switch (nInfoType)
   {
      case W_NDEF_INFO_TNF :
         /* The type name format (TNF) */
         * pnValue = pRecord->pRecordBuffer[0] & 0x07;
         break;

      case W_NDEF_INFO_TYPE_STRING_ASCII :
         /* The length in bytes of the type string encoded in ASCII */
         * pnValue = pRecord->nTypeSize;
         break;

      case W_NDEF_INFO_TYPE_STRING_PRINT :
         /* The length in characters of the type string in printable format  */
         * pnValue = static_PNDEFUtilConvertByteArrayToPrintableURIString(null, pRecord->pRecordBuffer + pRecord->nTypeOffset, pRecord->nTypeSize);
         break;

      case W_NDEF_INFO_IDENTIFIER_UTF8 :
         /* The length in bytes of the record identifier encoded in UTF8 URI */
         * pnValue = pRecord->nIdentifierSize;
         break;

      case W_NDEF_INFO_IDENTIFIER_PRINT :
         /* The length in characters of the record identifier encoded in printable URI  */
         * pnValue = static_PNDEFUtilConvertUTF8BufferToPrintableURIString(null, pRecord->pRecordBuffer + pRecord->nIdentifierOffset, pRecord->nIdentifierSize);
         break;

      case W_NDEF_INFO_PAYLOAD :
         /* The length in bytes of the record payload */
         * pnValue = pRecord->nPayloadSize;
         break;

      case W_NDEF_INFO_BUFFER :
         /* The length in bytes of the record buffer */
         * pnValue = pRecord->nRecordSize;
         break;

      default :
         PDebugTrace("PNDEFGetRecordInfo  : invalid nInfoType %d", nInfoType);
         nError = W_ERROR_BAD_PARAMETER;
         goto error;
   }

   return (W_SUCCESS);

error:

   if (pnValue != null)
   {
      * pnValue = 0;
   }

   return nError;
}

/* See Client API Specifications */
W_ERROR PNDEFGetRecordInfoBuffer(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  uint32_t nInfoType,
                  uint8_t* pBuffer,
                  uint32_t nBufferLength,
                  uint32_t* pnActualLength)
{
   W_ERROR       nError;
   tNDEFRecord * pRecord;

   PDebugTrace("PNDEFGetRecordInfoBuffer");

   if (pnActualLength != null)
   {
      * pnActualLength = 0;
   }

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   if ((pBuffer == null) || (pnActualLength == null))
   {
      PDebugError("PNDEFGetRecordInfoBuffer : pBuffer or  pnActualLength is null");
      return (W_ERROR_BAD_PARAMETER);
   }

   nError = W_ERROR_BUFFER_TOO_SHORT;

   switch (nInfoType)
   {
      case W_NDEF_INFO_TYPE_STRING_ASCII :

         if (nBufferLength >= pRecord->nTypeSize)
         {
            CMemoryCopy(pBuffer, pRecord->pRecordBuffer + pRecord->nTypeOffset, pRecord->nTypeSize);
            * pnActualLength = pRecord->nTypeSize;
            return W_SUCCESS;
         }
         break;

      case W_NDEF_INFO_IDENTIFIER_UTF8 :

         if (nBufferLength >= pRecord->nIdentifierSize)
         {
            CMemoryCopy(pBuffer, pRecord->pRecordBuffer + pRecord->nIdentifierOffset, pRecord->nIdentifierSize);
            * pnActualLength = pRecord->nIdentifierSize;
            return W_SUCCESS;
         }
         break;

      case W_NDEF_INFO_PAYLOAD :

         if (nBufferLength >= pRecord->nPayloadSize)
         {
            CMemoryCopy(pBuffer, pRecord->pRecordBuffer + pRecord->nPayloadOffset, pRecord->nPayloadSize);
            * pnActualLength = pRecord->nPayloadSize;
            return W_SUCCESS;
         }
         break;

      case W_NDEF_INFO_BUFFER :

         if (nBufferLength >= pRecord->nRecordSize)
         {
            CMemoryCopy(pBuffer, pRecord->pRecordBuffer, pRecord->nRecordSize);
            pBuffer[0] |= 0xC0; /* force MB and ME as requested in the Client API */
            *pnActualLength = pRecord->nRecordSize;
            return W_SUCCESS;
         }
         break;

      default :

         PDebugTrace("PNDEFGetRecordInfo  : invalid nInfoType %d", nInfoType);
         nError = W_ERROR_BAD_PARAMETER;
         break;
   }

   return (nError);
}

/* See Client API Specifications */
W_ERROR PNDEFGetEnclosedMessage(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  W_HANDLE* phMessage )
{
   tNDEFRecord* pRecord = null;
   W_ERROR nError;
   uint32_t nPayloadLength;
   uint32_t  nPayloadOffset;

   PDebugTrace("PNDEFGetEnclosedMessage");

   if (phMessage == null)
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   * phMessage = W_NULL_HANDLE;

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   if ( ( PNDEFParseSafeBuffer( pRecord->pRecordBuffer, pRecord->nRecordSize,
      null,null, null,null, &nPayloadOffset,&nPayloadLength ) == 0 )
      || ( nPayloadOffset == 0 )
      || ( nPayloadLength == 0 ) )
   {
      *phMessage = W_NULL_HANDLE;
      return W_ERROR_BAD_NDEF_FORMAT;
   }

   if ( W_SUCCESS == PNDEFBuildMessage( pContext, &pRecord->pRecordBuffer[nPayloadOffset], nPayloadLength, phMessage ) )
   {
      return W_SUCCESS;
   }
   else
   {
      *phMessage = W_NULL_HANDLE;
      return W_ERROR_BAD_NDEF_FORMAT;
   }
}


/* See Client API Specifications */
W_ERROR PNDEFGetPayloadPointer(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  uint8_t** ppBuffer )
{
   tNDEFRecord* pRecord = null;
   W_ERROR      nError;
   uint32_t nPayloadOffset = 0;
   uint32_t nPayloadLen = 0;

   PDebugTrace("PNDEFGetPayloadPointer");

   if (ppBuffer == null)
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   *ppBuffer = null;

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, &pRecord)) != W_SUCCESS)
   {
      return nError;
   }

   if ( PNDEFParseSafeBuffer( pRecord->pRecordBuffer, pRecord->nRecordSize,
      null, null, null, null, &nPayloadOffset, &nPayloadLen ) == 0)
   {
      *ppBuffer = null;
      return W_ERROR_BAD_HANDLE;
   }

   if (nPayloadLen == 0)
   {
      *ppBuffer = null;
   }
   else
   {
      *ppBuffer = &pRecord->pRecordBuffer[ nPayloadOffset ];
   }

   return W_SUCCESS;
}


/* See Client API Specifications */
W_ERROR PNDEFCreateNewMessage(
                  tContext* pContext,
                  W_HANDLE* phMessage )
{
   W_ERROR       nError;
   tNDEFMessage* pMessage;

   PDebugTrace("PNDEFCreateNewMessage");

   if (phMessage == null)
   {
      PDebugError("PNDEFCreateNewMessage : phMessage == null");
      return W_ERROR_BAD_PARAMETER;
   }

   pMessage = (tNDEFMessage*)CMemoryAlloc(sizeof(tNDEFMessage));

   if (pMessage == null)
   {
      PDebugError("PNDEFCreateNewMessage : CMemoryAlloc failed");

      *phMessage = W_NULL_HANDLE;
      return W_ERROR_OUT_OF_RESOURCE;
   }

   CMemoryFill( pMessage, 0, sizeof(tNDEFMessage));

   if ((nError = PHandleRegister( pContext, pMessage, P_HANDLE_TYPE_NDEF_MESSAGE, phMessage)) != W_SUCCESS)
   {
      PDebugError("PNDEFCreateNewMessage : PHandleRegister failed %s", PUtilTraceError(nError));

      CMemoryFree(pMessage);
      *phMessage = W_NULL_HANDLE;
      return W_ERROR_OUT_OF_RESOURCE;
   }

   return W_SUCCESS;
}

/* See Client API Specifications */
uint32_t PNDEFGetMessageLength(
                  tContext* pContext,
                  W_HANDLE hMessage )
{
   tNDEFMessage* pMessage = null;

   PDebugTrace("PNDEFGetMessageLength");

   if (static_PNDEFGetMessageObject(pContext, hMessage, &pMessage) != W_SUCCESS)
   {
      return 0;
   }

   return pMessage->nMessageSize;
}

/* See Client API Specifications */
W_ERROR PNDEFGetMessageContent(
                  tContext* pContext,
                  W_HANDLE hMessage,
                  uint8_t* pMessageBuffer,
                  uint32_t nMessageBufferLength,
                  uint32_t* pnActualLength )
{
   tNDEFMessage* pMessage = null;
   tNDEFRecordHandleList* pRecordList = 0;
   W_ERROR nError;

   PDebugTrace("PNDEFGetMessageContent");

   if (pnActualLength == null)
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   * pnActualLength = 0;

   if (pMessageBuffer == null)
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   if ((nError = static_PNDEFGetMessageObject(pContext, hMessage, &pMessage)) != W_SUCCESS)
   {
      return (nError);
   }

   if (nMessageBufferLength < pMessage->nMessageSize)
   {
      return W_ERROR_BUFFER_TOO_SHORT;
   }

   pRecordList = pMessage->pRecordHandleListBegin;

   while (pRecordList != null)
   {
      CMemoryCopy(
         pMessageBuffer + (*pnActualLength),
         pRecordList->pNDEFRecord->pRecordBuffer,
         pRecordList->pNDEFRecord->nRecordSize );

      *pnActualLength += pRecordList->pNDEFRecord->nRecordSize;
      pRecordList = pRecordList->pNext;
   }

   return W_SUCCESS;
}

/* See Client API Specifications */
W_ERROR PNDEFBuildMessage(
                  tContext* pContext,
                  const uint8_t* pBuffer,
                  uint32_t nBufferLength,
                  W_HANDLE* phMessage)
{
   W_HANDLE      hMessage = W_NULL_HANDLE;
   W_HANDLE      hRecord  = W_NULL_HANDLE;
   uint32_t      nIndex;
   uint32_t      nRecordCount;
   uint32_t      nRecordLength;
   W_ERROR       nError;

   PDebugTrace("PNDEFBuildMessage");

   /* Check parameters */

   if (phMessage == null)
   {
      PDebugError("PNDEFBuildMessage : phMessage == null");
      nError = W_ERROR_BAD_PARAMETER;
      goto error_cleanup;
   }

   if ((pBuffer == null) || (nBufferLength == 0))
   {
      PDebugError("PNDEFBuildMessage : pBuffer /  nBufferLength invalid");
      nError =  W_ERROR_BAD_PARAMETER;
      goto error_cleanup;
   }

   /* Ok, create a new message */

   if ((nError = PNDEFCreateNewMessage(pContext, &hMessage)) != W_SUCCESS)
   {
      PDebugError("PNDEFBuildMessage : PNDEFCreateNewMessage failed %s", PUtilTraceError(nError));
      goto error_cleanup;
   }

   * phMessage = hMessage;

   nRecordCount = 0;
   nIndex       = 0;

   for (;;)
   {
      /* Check the MB, ME flag consistency */

      if (nRecordCount == 0)
      {
         if ((pBuffer[nIndex] & 0x80) == 0)
         {
            PDebugError("PNDEFBuildMessage : first message does not have MB bit set !");
            nError = W_ERROR_BAD_NDEF_FORMAT;
            goto error_cleanup;
         }
      }
      else
      {
         if ((pBuffer[nIndex] & 0x80) != 0)
         {
            PDebugError("PNDEFBuildMessage : message have MB bit set !");
            nError = W_ERROR_BAD_NDEF_FORMAT;
            goto error_cleanup;
         }
      }

      nError = PNDEFBuildRecord(pContext, pBuffer + nIndex, nBufferLength - nIndex, &hRecord);

      if (nError != W_SUCCESS)
      {
         PDebugError("PNDEFBuildMessage : PNDEFBuildRecord failed %s", PUtilTraceError(nError));
         goto error_cleanup;
      }

      nError = PNDEFGetRecordInfo(pContext, hRecord, W_NDEF_INFO_BUFFER, &nRecordLength);

      if (nError != W_SUCCESS)
      {
           PDebugError("PNDEFBuildMessage : PNDEFGetRecordInfo failed %s", PUtilTraceError(nError));
           goto error_cleanup;
      }

      /* Add the record in the message */
      nError = PNDEFAppendRecord(pContext, hMessage, hRecord);

      if (nError != W_SUCCESS)
      {
         PDebugError("PNDEFBuildMessage : PNDEFGetRecordInfo failed %s", PUtilTraceError(nError));
         goto error_cleanup;
      }

      /* the record has been added and is no longer needed */

      PHandleClose(pContext, hRecord);
      hRecord = W_NULL_HANDLE;

      /* Prepare for the next record processing */

      nRecordCount++;
      if ((pBuffer[nIndex] & 0x40) != 0)
      {
         /* we've processed the last record of the message
            ignore the bytes in the buffer after the end of the message */
         return (W_SUCCESS);
      }

      nIndex += nRecordLength;

      if (nIndex == nBufferLength)
      {
         /* we've reached the end of the buffer without ME bit */
         PDebugError("PNDEFBuildMessage : Cannot find the last message with the ME bit set !");
         nError = W_ERROR_BAD_NDEF_FORMAT;
         goto error_cleanup;
      }
   }

error_cleanup :

   if (hRecord != W_NULL_HANDLE)
   {
      PHandleClose(pContext, hRecord);
   }

   if (hMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, hMessage);
   }

   if (phMessage != null)
   {
      * phMessage = W_NULL_HANDLE;
   }

   return (nError);
}

/* See Client API Specifications */
W_ERROR PNDEFGetTypeString(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  tchar* pTypeStringBuffer,
                  uint32_t nTypeStringBufferLength,
                  uint32_t* pnActualLength )
{
   W_ERROR       nError;
   tNDEFRecord * pRecord;
   uint32_t      nSize;

   PDebugTrace("PNDEFGetTypeString");

   if (pnActualLength != null)
   {
      * pnActualLength = 0;
   }

   if (( nError = PNDEFGetRecordObject(pContext, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   if (pnActualLength == null)
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   if ((pTypeStringBuffer == null) || (nTypeStringBufferLength ==0))
   {
      return W_ERROR_BAD_PARAMETER;
   }

   nSize = static_PNDEFUtilConvertByteArrayToPrintableURIString(null, pRecord->pRecordBuffer + pRecord->nTypeOffset, pRecord->nTypeSize);

   if (nTypeStringBufferLength < nSize)
   {
      return (W_ERROR_BUFFER_TOO_SHORT);
   }

   * pnActualLength = static_PNDEFUtilConvertByteArrayToPrintableURIString(pTypeStringBuffer, pRecord->pRecordBuffer + pRecord->nTypeOffset, pRecord->nTypeSize);

   return W_SUCCESS;

}

/* See Client API Specifications */
bool PNDEFCheckIdentifier(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  const tchar* pIdentifierString)
{
   tNDEFRecord *pRecord = null;
   tchar       *pTemp;
   uint32_t     nSize;
   bool         ret;

   PDebugTrace("PNDEFCheckIdentifier");

   if (pIdentifierString == null)
   {
      PDebugTrace("PNDEFCheckIdentifier : pIdentifierString == null");
      return false;
   }

   if (PNDEFGetRecordObject(pContext, hRecord, &pRecord) != W_SUCCESS)
   {
      return false;
   }

   if (PNDEFUtilStringIsPrintableURI(pIdentifierString) == false)
   {
      PDebugError("PNDEFCheckIdentifier : pTypeString is not a printable URI");
      return false;
   }

   nSize = static_PNDEFUtilConvertUTF8BufferToPrintableURIString(null, pRecord->pRecordBuffer + pRecord->nIdentifierOffset, pRecord->nIdentifierSize);
   pTemp = CMemoryAlloc((nSize + 1) * sizeof(tchar));

   static_PNDEFUtilConvertUTF8BufferToPrintableURIString(pTemp, pRecord->pRecordBuffer + pRecord->nIdentifierOffset, pRecord->nIdentifierSize);

   if (PUtilStringCompare(pIdentifierString, pTemp) == 0)
   {
      ret = true;
   }
   else
   {
      ret = false;
   }

   CMemoryFree(pTemp);

   return (ret);

}

/* See Client API Specifications */
W_ERROR PNDEFGetIdentifierString(
                  tContext* pContext,
                  W_HANDLE hRecord,
                  tchar* pIdentifierStringBuffer,
                  uint32_t nIdentifierStringBufferLength,
                  uint32_t* pnActualLength )
{
   tNDEFRecord * pRecord;
   W_ERROR       nError;
   uint32_t      nSize;

   PDebugTrace("PNDEFGetIdentifierString");

   if (pnActualLength == null)
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   * pnActualLength = 0;

   if ((pIdentifierStringBuffer == null) || (nIdentifierStringBufferLength == 0))
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   if ((nError = PNDEFGetRecordObject(pContext, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   nSize = static_PNDEFUtilConvertUTF8BufferToPrintableURIString(null, pRecord->pRecordBuffer + pRecord->nIdentifierOffset, pRecord->nIdentifierSize);

   if (nIdentifierStringBufferLength < nSize + 1)
   {
      PDebugError("PNDEFGetIdentifierString : buffer too short");
      return (W_ERROR_BUFFER_TOO_SHORT);
   }

   nSize = static_PNDEFUtilConvertUTF8BufferToPrintableURIString(pIdentifierStringBuffer, pRecord->pRecordBuffer + pRecord->nIdentifierOffset, pRecord->nIdentifierSize);
   pIdentifierStringBuffer[nSize] = 0x0000;

   * pnActualLength = nSize;

   return (W_SUCCESS);
}

/* See the Client API Specifications */
W_HANDLE PNDEFGetRecord(
                  tContext* pContext,
                  W_HANDLE hMessage,
                  uint32_t nIndex )
{
   tNDEFMessage            * pMessage;
   uint32_t                  nItemNumber;
   W_ERROR                   nError;
   tNDEFRecordHandleList   * pCurrentItem;
   W_HANDLE                  hNewRecord;

   PDebugTrace("PNDEFGetRecord");

   if ((nError = static_PNDEFGetMessageObject(pContext, hMessage, & pMessage)) != W_SUCCESS)
   {
      return (W_NULL_HANDLE);
   }

   nItemNumber = static_PNDEFGetRecordNumber(pMessage);

   if (nIndex >= nItemNumber)
   {
      PDebugError("PNDEFGetRecord : nIndex is invalid");
      return (W_NULL_HANDLE);
   }

   pCurrentItem = pMessage->pRecordHandleListBegin;

   while (nIndex--)
   {
      CDebugAssert(pCurrentItem != null);
      pCurrentItem = pCurrentItem->pNext;
   }

   CDebugAssert(pCurrentItem != null);

   /* duplicate the record found */

   if ((nError = static_PNDEFRecordClone(pContext, pCurrentItem->hRecord, &hNewRecord)) != W_SUCCESS)
   {
      PDebugError("PNDEFGetRecord : static_PNDEFRecordClone failed %s'", PUtilTraceError(nError));
   }

   return (hNewRecord);
}

/* See the Client API Specifications */
W_ERROR PNDEFSetRecord(
                  tContext* pContext,
                  W_HANDLE hMessage,
                  uint32_t nIndex,
                  W_HANDLE hRecord )
{
   tNDEFMessage * pMessage;
   tNDEFRecord  * pRecord;
   W_ERROR        nError;

   PDebugTrace("PNDEFSetRecord");

   if ((nError = static_PNDEFGetMessageAndRecordObjects(pContext, hMessage, &pMessage, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   if (nIndex >= static_PNDEFGetRecordNumber(pMessage))
   {
      PDebugError("PNDEFSetRecord : invalid nIndex");
      return (W_ERROR_BAD_PARAMETER);
   }

   if ((nError = static_PNDEFCheckRecordUnicity(pContext, hMessage, hRecord, nIndex)) != W_SUCCESS)
   {
      return (nError);
   }

   /* The Set record operation is done in two passes :
      - first insert the new record at the nIndex position
      - then remove the previous record (now at the nIndex+1) position
    */

   nError = static_PNDEFInsertRecord(pContext, hMessage, nIndex, hRecord, false);

   if (nError == W_SUCCESS)
   {
      nError = PNDEFRemoveRecord(pContext, hMessage, nIndex+1);

      CDebugAssert(nError == W_SUCCESS);
   }

   return (nError);
}


/* See the Client API Specifications */
W_ERROR static_PNDEFInsertRecord(
                  tContext* pContext,
                  W_HANDLE hMessage,
                  uint32_t nIndex,
                  W_HANDLE hRecord,
                  bool     bAllowAppend)
{
   tNDEFMessage * pMessage;
   tNDEFRecord  * pRecord;
   W_ERROR        nError;
   tNDEFRecordHandleList * pNewItem, * pPreviousItem;

   PDebugTrace("static_PNDEFInsertRecord");

   if ((nError = static_PNDEFGetMessageAndRecordObjects(pContext, hMessage, &pMessage, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   if (bAllowAppend == false)
   {
      if (nIndex >= pMessage->nRecordCount)
      {
         PDebugError("PNDEFInsertRecord : bad nIndex value");
         return (W_ERROR_BAD_PARAMETER);
      }
   }
   else
   {
      if (nIndex > pMessage->nRecordCount)
      {
         PDebugError("PNDEFInsertRecord : bad nIndex value");
         return (W_ERROR_BAD_PARAMETER);
      }
   }

   pNewItem = (tNDEFRecordHandleList *)CMemoryAlloc(sizeof(tNDEFRecordHandleList));

   if (pNewItem == null)
   {
      PDebugError("PNDEFAppendRecord : out of resource");
      return (W_ERROR_OUT_OF_RESOURCE);
   }

   /* create a clone of the record since the content of the message must not be changed if the record is modified */
   nError = static_PNDEFRecordClone(pContext, hRecord, & pNewItem->hRecord);

   if (nError != W_SUCCESS)
   {
      PDebugError("PNDEFAppendRecord : static_PNDEFRecordClone failed");
      return (nError);
   }

   pNewItem->pNext = null;
   PHandleGetObject( pContext, pNewItem->hRecord, P_HANDLE_TYPE_NDEF_RECORD, (void **)& pRecord);

   pNewItem->pNDEFRecord  = pRecord;

   /* reset the Message Begin and Message End flags in the record */

   pRecord->pRecordBuffer[0] &= 0x3F;

   if (nIndex == 0)     /* insert in first position */
   {
      pRecord->pRecordBuffer[0] |= 0x80;           /* First record -> Set Message Begin flag */

      if (pMessage->pRecordHandleListBegin == null)
      {
         pRecord->pRecordBuffer[0] |= 0x40;        /* Last record -> Set Message End flag */

         pMessage->pRecordHandleListBegin = pNewItem;
         pMessage->pRecordHandleListEnd = pNewItem;
      }
      else
      {
         /* Reset Message Begin flag from previous head */

         pMessage->pRecordHandleListBegin->pNDEFRecord->pRecordBuffer[0] &= ~0x80;

         pNewItem->pNext = pMessage->pRecordHandleListBegin;
         pMessage->pRecordHandleListBegin = pNewItem;
      }
   }
   else if (nIndex == pMessage->nRecordCount)  /* insert at the last position */
   {
      pRecord->pRecordBuffer[0] |= 0x40;           /* Last record -> Set Message End flag */


      /* Reset Message End flag in the previous tail */

      pMessage->pRecordHandleListEnd->pNDEFRecord->pRecordBuffer[0] &= ~0x40;

      pMessage->pRecordHandleListEnd->pNext = pNewItem;
      pMessage->pRecordHandleListEnd = pNewItem;
   }
   else  /* insert in the middle of the list */
   {
      pPreviousItem = pMessage->pRecordHandleListBegin;

      while (nIndex-- != 1) {

         CDebugAssert(pPreviousItem != null);
         pPreviousItem = pPreviousItem->pNext;
      }

      /* pPreviousItem points on the previous record */

      pNewItem->pNext = pPreviousItem->pNext;
      pPreviousItem->pNext = pNewItem;
   }

   pMessage->nRecordCount++;
   pMessage->nMessageSize += pRecord->nRecordSize;
   return W_SUCCESS;
}

/* See the Client API Specifications */
W_ERROR PNDEFInsertRecord(
                  tContext* pContext,
                  W_HANDLE hMessage,
                  uint32_t nIndex,
                  W_HANDLE hRecord
                  )
{
   tNDEFMessage * pMessage;
   tNDEFRecord  * pRecord;
   W_ERROR        nError;

   PDebugTrace("PNDEFInsertRecord");

   if ((nError = static_PNDEFGetMessageAndRecordObjects(pContext, hMessage, &pMessage, hRecord, &pRecord)) != W_SUCCESS)
   {
      return (nError);
   }

   if ((nError = static_PNDEFCheckRecordUnicity(pContext, hMessage, hRecord, (uint32_t) ~0)) != W_SUCCESS)
   {
      return (nError);
   }

   return static_PNDEFInsertRecord(pContext, hMessage, nIndex, hRecord, false);
}

/* See the Client API Specifications */
W_ERROR PNDEFRemoveRecord(tContext* pContext,
                          W_HANDLE  hMessage,
                          uint32_t  nIndex)
{
   tNDEFMessage * pMessage;
   W_ERROR        nError;
   uint32_t       i;

   tNDEFRecordHandleList * pPreviousRecord;
   tNDEFRecordHandleList * pCurrentRecord;

   PDebugTrace("PNDEFRemoveRecord %d", nIndex);

   if ((nError = static_PNDEFGetMessageObject(pContext, hMessage, &pMessage)) != W_SUCCESS)
   {
      return (nError);
   }

   if (nIndex >= pMessage->nRecordCount)
   {
      return (W_ERROR_BAD_PARAMETER);
   }

   if (nIndex == 0)
   {
      /* removing the first record */
      pCurrentRecord = pMessage->pRecordHandleListBegin;

      pMessage->pRecordHandleListBegin = pMessage->pRecordHandleListBegin->pNext;

      if (pMessage->pRecordHandleListBegin == null)
      {
         pMessage->pRecordHandleListEnd = null;
      }

      /* ensure the MB is set in the new head */
      if (pMessage->pRecordHandleListBegin != null)
      {
         pMessage->pRecordHandleListBegin->pNDEFRecord->pRecordBuffer[0] |= 0x80;
      }

   }
   else
   {
      pPreviousRecord = null;
      pCurrentRecord = pMessage->pRecordHandleListBegin;

      for (i=0; i<nIndex;i++)
      {
         pPreviousRecord = pCurrentRecord;
         pCurrentRecord = pCurrentRecord->pNext;
      }

      pPreviousRecord->pNext = pCurrentRecord->pNext;

      if (pPreviousRecord->pNext == null)
      {
         pMessage->pRecordHandleListEnd = pPreviousRecord;
         pPreviousRecord->pNDEFRecord->pRecordBuffer[0] |= 0x40;
      }
   }

   pMessage->nRecordCount--;
   pMessage->nMessageSize = pMessage->nMessageSize - pCurrentRecord->pNDEFRecord->nRecordSize;

   PHandleClose(pContext, pCurrentRecord->hRecord);
   CMemoryFree(pCurrentRecord);

   return(W_SUCCESS);
}

/* See Header file */
W_ERROR PNDEFSetNextMessage(
            tContext* pContext,
            W_HANDLE hMessage,
            W_HANDLE hMessageNext )
{
   tNDEFMessage* pMessage;
   W_ERROR nError;

   PDebugTrace("PNDEFSetNextMessage");

   if (W_NULL_HANDLE == hMessage)
   {
      return W_ERROR_BAD_HANDLE;
   }

   if ((nError = static_PNDEFGetMessageObject(pContext, hMessage, &pMessage)) != W_SUCCESS)
   {
      return (nError);
   }


   pMessage->hMessageNext = hMessageNext;
   return W_SUCCESS;
}

/* See Client API Specifications */
W_HANDLE PNDEFGetNextMessage(
            tContext* pContext,
            W_HANDLE hMessage)
{
   tNDEFMessage* pMessage;
   W_HANDLE      hCurrentMessage;
   tNDEFMessage* pCurrentMessage;


   PDebugTrace("PNDEFGetNextMessage");

   if ((static_PNDEFGetMessageObject(pContext, hMessage, &pMessage)) != W_SUCCESS)
   {
      return (W_NULL_HANDLE);
   }

   /* walk through the linked messages to increment theih ref counter */

   hCurrentMessage = pMessage->hMessageNext;

   while (hCurrentMessage != W_NULL_HANDLE)
   {
      if ((static_PNDEFGetMessageObject(pContext, hCurrentMessage, &pCurrentMessage)) == W_SUCCESS)
      {
         PHandleIncrementReferenceCount(pCurrentMessage);
      }
      else
      {
         CDebugAssert(0);
      }

      hCurrentMessage = pCurrentMessage->hMessageNext;
   }

   return pMessage->hMessageNext;
}

/* See Client API Specifications */
bool WNDEFCheckRecordType(
                  uint8_t nTNF,
                  const tchar* pTypeString)
{
   PDebugTrace("WNDEFCheckRecordType");

   switch (nTNF)
   {
      case W_NDEF_TNF_EMPTY:
      case W_NDEF_TNF_UNKNOWN:
      case W_NDEF_TNF_UNCHANGED:

         /* for these TNF values, the pTypeString must be null */

         return (pTypeString == null) ? true : false;

      case W_NDEF_TNF_WELL_KNOWN:
      case W_NDEF_TNF_MEDIA:
      case W_NDEF_TNF_ABSOLUTE_URI:
      case W_NDEF_TNF_EXTERNAL:

         if (pTypeString == null)
         {
            return false;
         }
      break;

      default :
         return false;

   }

   switch (nTNF)
   {
      case W_NDEF_TNF_MEDIA :
      case W_NDEF_TNF_ABSOLUTE_URI :
         return PNDEFUtilStringIsPrintableURI(pTypeString);

      case W_NDEF_TNF_WELL_KNOWN :
         return static_PNDEFUtilStringIsWellKnownCompatible(pTypeString);

      case W_NDEF_TNF_EXTERNAL :
         return static_PNDEFUtilStringIsExternalCompatible(pTypeString);
   }

   return false;
}

/** \brief Compares two Well-known TNF records
  *
  * @param[in] pString1 The string corresponding to the first TNF record
  *
  * @param[in] pString2 The string corresponding to the second TNF record
  *
  * @return 0 if the TNF records are equal
  *
  */

static sint32_t static_PNDEFWellKnownURICompare(
   const tchar * pString1,
   const tchar * pString2)
{

   PDebugTrace("static_PNDEFWellKnownURICompare");

   if ((pString1 == null) || (pString2 == null))
   {
      return -1;
   }

   /* Before performing the comparison, the prefix urn:nfc:wkt: is removed from the type names, if present */

   if (PUtilAsciiStringCaseSensitiveNCompare(pString1, g_aNDEFWellKnownPrefix, g_nNDEFWellKnownPrefixLengh) == 0)
   {
      pString1 += g_nNDEFWellKnownPrefixLengh;
   }

   if (PUtilAsciiStringCaseSensitiveNCompare(pString2, g_aNDEFWellKnownPrefix, g_nNDEFWellKnownPrefixLengh) == 0)
   {
      pString2 += g_nNDEFWellKnownPrefixLengh;
   }

   /* Then perform the comparison (case sensitive) */

   return (PUtilAsciiStringCaseSensitiveCompare(pString1, pString2));
}


/** \brief Compares two Media TNF records
  *
  * @param[in] pString1 The string corresponding to the first TNF record
  *
  * @param[in] pString2 The string corresponding to the second TNF record
  *
  * @return 0 if the TNF records are equal
  *
  */

static sint32_t static_PNDEFMediaURICompare(
   const tchar * pString1,
   const tchar * pString2)
{
   const tchar * pString;
   sint32_t nResult;
   uint32_t nComparisonLength1 = 0;
   uint32_t nComparisonLength2 = 0;

   PDebugTrace("static_PNDEFMediaURICompare");

   if ((pString1 == null) || (pString2 == null))
   {
      return -1;
   }

   /* The comparison must be done till the first occurence of '; */

   pString = static_PNDEFUtilASCIIStringChr(pString1, L';');

   if (pString != null)
   {
      nComparisonLength1 = pString - pString1;
   }
   else
   {
      nComparisonLength1 = PUtilStringLength(pString1);
   }

   pString = static_PNDEFUtilASCIIStringChr(pString2, L';');

   if (pString != null)
   {
      nComparisonLength2 = pString - pString2;
   }
   else
   {
      nComparisonLength2 = PUtilStringLength(pString2);
   }

   if (nComparisonLength1 == nComparisonLength2)
   {
      /* Perform the comparison until the ';' */
      nResult = PUtilASCIIStringCaseInsensitiveNCompare(pString1, pString2, nComparisonLength1);
   }
   else
   {
      /* the two sub-strings to compare do not have the same length, they differ ! */
      nResult = -1;
   }

   return (nResult);
}


/**
  * \brief Normalizes an absolute URI (lower scheme and host part)
  *
  * @param[in/out] pString The absolute URI to be normalized
  */


static void static_PNEFNormalizeAbsoluteURI(
   tchar * pString)
{
   tchar    nCar;
   tchar *  pHost;

   PDebugTrace("static_PNEFNormalizeAbsoluteURI");
   CDebugAssert(pString != null);

   /* lower the scheme part */
   for (nCar = * pString; nCar != ':'; nCar = * ++pString)
   {
      if (nCar == 0)
      {
         return;
      }

      if ((nCar >= 'A') && (nCar <= 'Z'))
      {
         nCar = nCar - 'A' + 'a';
         * pString = nCar;
      }
   }

   /* check the presence of "//" */

   if ((* ++pString != '/') || (* ++pString != '/'))
   {
      /* no // found, return */
      return;

   }

   /* authority : [user@]host[:port] */
   pHost = ++pString;

   for (nCar = * pString; (nCar != 0) && (nCar != '/') && (nCar != ':'); nCar = * ++pString)
   {
      if (nCar == '@')
      {
         pHost = pString+1;
         break;
      }
   }

   /* pHost now points on the start of the host address */

   pString = pHost;

   for (nCar = * pString; (nCar != 0) && (nCar != '/') && (nCar != ':'); nCar = * ++pString)
   {
      if ((nCar >= 'A') && (nCar <= 'Z'))
      {
         nCar = nCar - 'A' + 'a';
         * pString = nCar;
      }
   }
}


/** \brief Compares two absolute URI TNF records
  *
  * @param[in] pString1 The string corresponding to the first TNF record
  *
  * @param[in] pString2 The string corresponding to the second TNF record
  *
  * @return 0 if the TNF records are equal
  *
  */

static sint32_t static_PNDEFAbsoluteURICompare(
   const tchar * pString1,
   const tchar * pString2)
{

   uint32_t nSize1;
   uint32_t nSize2;
   tchar * pTempString1 = null;
   tchar * pTempString2 = null;
   sint32_t nRes;

   PDebugTrace("static_PNDEFAbsoluteURICompare");

   if ((pString1 == null) || (pString2 == null))
   {
      return -1;
   }

   nSize1 = PUtilConvertStringToPrintableString(null, pString1);
   nSize2 = PUtilConvertStringToPrintableString(null, pString2);

   if (nSize1 != nSize2)
   {
      return -1;
   }

   if (nSize1 == 0)
   {
      return 0;
   }

   pTempString1 = CMemoryAlloc((nSize1+1) * sizeof(tchar));
   pTempString2 = CMemoryAlloc((nSize2+1) * sizeof(tchar));

   if ((pTempString1 == null) || (pTempString1 == null))
   {
      PDebugError("static_PNDEFAbsoluteURICompare : out of memory");
      nRes = -1;
      goto end;
   }

   PUtilConvertStringToPrintableString(pTempString1, pString1);
   PUtilConvertStringToPrintableString(pTempString2, pString2);

   static_PNEFNormalizeAbsoluteURI(pTempString1);
   static_PNEFNormalizeAbsoluteURI(pTempString2);

   nRes = PUtilAsciiStringCaseSensitiveCompare(pTempString1, pTempString2);

end:
   CMemoryFree(pTempString1);
   CMemoryFree(pTempString2);

   return (nRes);
}

/** \brief Compares two external TNF records
  *
  * @param[in] pString1 The string corresponding to the first TNF record
  *
  * @param[in] pString2 The string corresponding to the second TNF record
  *
  * @return 0 if the TNF records are equal
  *
  */

static sint32_t static_PNDEFExternalURICompare(
   const tchar * pString1,
   const tchar * pString2)
{
   PDebugTrace("static_PNDEFExternalURICompare");

   if ((pString1 == null) || (pString2 == null))
   {
      return -1;
   }

   /* Before performing the comparison, the prefix urn:nfc:ext: is removed from the type names, if present */

   if (PUtilAsciiStringCaseSensitiveNCompare(pString1, g_aNDEFExternalPrefix, g_nNDEFExternalPrefixLengh) == 0)
   {
      pString1 += g_nNDEFWellKnownPrefixLengh;
   }

   if (PUtilAsciiStringCaseSensitiveNCompare(pString2, g_aNDEFExternalPrefix, g_nNDEFExternalPrefixLengh) == 0)
   {
      pString2 += g_nNDEFWellKnownPrefixLengh;
   }

   /* Then perform the comparison (case insensitive) */

   return (PUtilASCIIStringCaseInsensitiveCompare(pString1, pString2));
}



/* See Client API Specifications */
bool WNDEFCompareRecordType(
                  uint8_t nTNF1,
                  const tchar* pTypeString1,
                  uint8_t nTNF2,
                  const tchar* pTypeString2)
{

   PDebugTrace("WNDEFCompareRecordType");

   /* comparison with W_NDEF_TNF_ANY_TYPE is always successfull,
      but we ensure the associated pTypeString is null */

   if ( ((nTNF1 == W_NDEF_TNF_ANY_TYPE) && (pTypeString1 != null)) ||
        ((nTNF2 == W_NDEF_TNF_ANY_TYPE) && (pTypeString2 != null)))
   {
      return false;
   }

   if ((nTNF1 == W_NDEF_TNF_ANY_TYPE) || (nTNF2 == W_NDEF_TNF_ANY_TYPE))
   {
      return (true);
   }

   /* comparison between different TNF values is always unsuccessfull */
   if (nTNF1 != nTNF2)
   {
      return (false);
   }

   switch (nTNF1)
   {
      case W_NDEF_TNF_EMPTY :
      case W_NDEF_TNF_UNKNOWN :
      case W_NDEF_TNF_UNCHANGED :

         /* For this records, the Type strings must be null */

         PDebugTrace("pTypeString1 %p - pTypeString2 %p", pTypeString1, pTypeString2);

         return ((pTypeString1 == null) && (pTypeString2 == null)) ? true : false ;

      case W_NDEF_TNF_WELL_KNOWN :

         PDebugTrace("WNDEFCompareRecordType: comparing W_NDEF_TNF_WELL_KNOWN");
         return (static_PNDEFWellKnownURICompare(pTypeString1, pTypeString2) == 0) ? true : false;

      case W_NDEF_TNF_MEDIA :

         PDebugTrace("WNDEFCompareRecordType: comparing W_NDEF_TNF_MEDIA");
         return (static_PNDEFMediaURICompare(pTypeString1, pTypeString2) == 0) ? true : false;

      case W_NDEF_TNF_ABSOLUTE_URI :

         PDebugTrace("WNDEFCompareRecordType: comparing W_NDEF_TNF_ABSOLUTE_URI");
         return (static_PNDEFAbsoluteURICompare(pTypeString1, pTypeString2) == 0) ? true : false;

      case W_NDEF_TNF_EXTERNAL :

         PDebugTrace("WNDEFCompareRecordType: comparing W_NDEF_TNF_EXTERNAL");
         return (static_PNDEFExternalURICompare(pTypeString1, pTypeString2) == 0) ? true : false;

      default:

         PDebugTrace("WNDEFCompareRecordType: comparing unknown");
         return (false);
   }
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
