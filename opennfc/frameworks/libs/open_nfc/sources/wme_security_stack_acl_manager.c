/*
 * Copyright (c) 2011 Inside Secure, All Rights Reserved.
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
   Contains the implementation of the security stack ACL manager functions
*******************************************************************************/

#define P_MODULE  P_MODULE_DEC( SECSTACK )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#include "wme_security_stack_acl_manager.h"

/* ASN.1 tags (1-byte long) */
#define P_TAG(_class, _porc, _number)\
  ((uint8_t)(((_class) & 0xC0)|((_porc) & 0x20)|((_number) & 0x1F)))

#define P_TAG_UNIVERSAL ((uint8_t)0x00)
#define P_TAG_CONTEXT_SPECIFIC ((uint8_t)0x80)
#define P_TAG_PRIMITIVE ((uint8_t)0x00)
#define P_TAG_CONSTRUCTED ((uint8_t)0x20)
#define P_TAG_INTEGER ((uint8_t)0x02)
#define P_TAG_OCTET_STRING ((uint8_t)0x04)
#define P_TAG_SEQUENCE ((uint8_t)0x10)

#define P_TAG_EMPTY ((uint8_t)0x00)

#define P_TAG_UNIVERSAL_INTEGER\
  P_TAG(P_TAG_UNIVERSAL, P_TAG_PRIMITIVE, P_TAG_INTEGER)

#define P_TAG_UNIVERSAL_OCTET_STRING\
  P_TAG(P_TAG_UNIVERSAL, P_TAG_PRIMITIVE, P_TAG_OCTET_STRING)

#define P_TAG_UNIVERSAL_SEQUENCE\
  P_TAG(P_TAG_UNIVERSAL, P_TAG_CONSTRUCTED, P_TAG_SEQUENCE)

/** FID of the EF(SE-LastUpdate) file */
#define P_SECSTACK_FID_EF_SE_LASTUPDATE ((uint16_t)0x5035)

/** FID of the EF(SE-ACF) file */
#define P_SECSTACK_FID_EF_SE_ACF ((uint16_t)0x5036)

/** Length of a Principal ID */
#define P_SECSTACK_PRINCIPALID_LENGTH 20

/** Length of an APDU permission */
#define P_SECSTACK_APDU_PERMISSION_LENGTH 4

/* Allowed value ranges */
#define P_SECSTACK_ACE_INDEX_MIN 0x0000
#define P_SECSTACK_ACE_INDEX_MAX 0x7FFF
#define P_SECSTACK_ACE_LENGTH_MIN 0x0000
#define P_SECSTACK_ACE_LENGTH_MAX 0x7FFF

/*
 * The following data types are used to build an internal representation
 * of the ACL read from the EF(SE-ACF) file.
 */

/** Structure for an ACIE (Access Control Index Entry) */
typedef struct __tACIE
{
   /** The pointer to the next ACIE */
   struct __tACIE* pNextACIE;
   /** The length of the AID (0 indicates that the AID is not present) */
   uint32_t nAID;
   /** The AID value */
   uint8_t aAID[P_SECSTACK_AID_MAX_LENGTH];
   /** The pointer to the ACE associated with this ACIE */
   struct __tACE* pACE;
}  tACIE;

/** Structure for a Principal identifier */
typedef struct __tPrincipal
{
   /** The 20-byte SHA-1 hash of the Principal */
   uint8_t id[P_SECSTACK_PRINCIPALID_LENGTH];
}  tPrincipal;

/** Structure for an APDU permission */
typedef struct __tPermission
{
   /** The APDU header (4 bytes) */
   uint8_t apduHeader[P_SECSTACK_APDU_PERMISSION_LENGTH];
   /** The APDU mask (4 bytes) */
   uint8_t apduMask[P_SECSTACK_APDU_PERMISSION_LENGTH];
}  tPermission;

/** Structure for an ACE (Access Control Entry) */
typedef struct __tACE
{
   /** The pointer to the next ACE */
   struct __tACE* pNextACE;
   /** The index (offset) of the ACE with the EF(SE-ACF) file */
   uint32_t nIndex;
   /** The length of the ACE with the EF(SE-ACF) file */
   uint32_t nLength;
   /** The number of Principals */
   uint32_t nPrincipals;
   /** The array of Principals (dynamically allocated) */
   tPrincipal* pPrincipals;
   /** The number of Permissions */
   uint32_t nPermissions;
   /** The array of Permissions (dynamically allocated) */
   tPermission* pPermissions;
}  tACE;

/** Structure for an ACL (Access Control List) */
struct __tSecurityStackACL
{
   /** Pointer to the first ACIE */
   tACIE* pFirstACIE;
   /** Pointer to the first ACE */
   tACE* pFirstACE;
};

/** Checks whether an ACE has already been parsed */
static W_ERROR static_PSecurityStackLookupACE
(
   tSecurityStackACL* pACL,
   uint32_t nIndex,
   uint32_t nLength,
   tACE** ppACE
)
{
   tACE* pACE;

   if ((pACL == null) || (ppACE == null))
   {
      return W_ERROR_BAD_PARAMETER;
   }

   for (pACE = pACL->pFirstACE; pACE != null; pACE = pACE->pNextACE)
   {
      if ((pACE->nIndex == nIndex) && (pACE->nLength == nLength))
      {
         *ppACE = pACE;
         return W_SUCCESS;
      }
   }

   return W_ERROR_ITEM_NOT_FOUND;
}

/** Reads the next TLV from the EF(SE-ACF) file */
static W_ERROR static_PSecurityStackGetNextTLV
(
   const uint8_t** ppBuffer,
   uint32_t* pnBuffer,
   uint8_t* pnTag,
   uint32_t* pnLength,
   const uint8_t** ppValue
)
{
   const uint8_t* pBuffer = (uint8_t*)null;
   uint32_t nBuffer = 0;
   uint8_t nTag = P_TAG_EMPTY;
   uint32_t nLength = 0;
   const uint8_t* pValue = (uint8_t*)null;

   if ((ppBuffer == null) || (pnBuffer == null))
   {
      return W_ERROR_BAD_PARAMETER;
   }

   pBuffer = *ppBuffer;
   nBuffer = *pnBuffer;

   /* Parse tag (assume it 1-byte long) */
   if (nBuffer < 1)
   {
      PDebugError("static_PSecurityStackGetNextTLV: Not enough data (tag field)");
      return W_ERROR_BAD_PARAMETER;
   }

   nTag = pBuffer[0];
   pBuffer++;
   nBuffer--;

   /* Parse the length field (1, 2 or 3 bytes) */
   if (nBuffer < 1)
   {
      if (nTag == P_TAG_EMPTY)
      {
         nLength = 0;
         pValue = (uint8_t*)null;

         goto return_result;
      }

      PDebugError("static_PSecurityStackGetNextTLV: Not enough data (length field)");
      return W_ERROR_BAD_PARAMETER;
   }

   nLength = pBuffer[0];
   pBuffer++;
   nBuffer--;

   if (nLength>=0x80) /* Bit b8 set to 1 ? */
   {
      if ((nLength == 0x81) && (nBuffer >= 1))
      {
         nLength = pBuffer[0];
         pBuffer++;
         nBuffer--;
      }
      else if ((nLength == 0x82) && (nBuffer >=2 ))
      {
         nLength = ((256*pBuffer[0])+pBuffer[1]);
         pBuffer += 2;
         nBuffer -= 2;
      }
      else
      {
         PDebugError("static_PSecurityStackGetNextTLV: Invalid length field");
         return W_ERROR_BAD_PARAMETER;
      }
   }

   /* Check that the value field still fits in the buffer */
   if (nBuffer < nLength)
   {
      PDebugError("static_PSecurityStackGetNextTLV: Not enough data (value field)");
      return W_ERROR_BAD_PARAMETER;
   }

   pValue = pBuffer;
   pBuffer += nLength;
   nBuffer -= nLength;

return_result:

   if (pnTag != null)
   {
      *pnTag = nTag;
   }

   if (pnLength != null)
   {
      *pnLength = nLength;
   }

   if (ppValue != null)
   {
      *ppValue = pValue;
   }

   *ppBuffer = pBuffer;
   *pnBuffer = nBuffer;

   return W_SUCCESS;
}

/** Parses a DER-encoded integer */
static W_ERROR static_PSecurityStackParseInteger
(
   const uint8_t* pBuffer,
   uint32_t nBuffer,
   sint32_t* pnValue,
   sint32_t nValueMin,
   sint32_t nValueMax
)
{
   sint32_t nValue = 0;

   if ((pBuffer == null) || (nBuffer < 1) || (nBuffer > 4))
   {
      return W_ERROR_BAD_PARAMETER;
   }

   /* Initialize with sign-bit extension */
   nValue = (sint8_t)pBuffer[0];
   pBuffer++;
   nBuffer--;

   while (nBuffer > 0)
   {
      nValue = (nValue << 8) + pBuffer[0];
      pBuffer++;
      nBuffer--;
   }

   if ((nValue < nValueMin) || (nValue > nValueMax))
   {
      PDebugError("static_PSecurityStackParseInteger: Invalid value (not in allowed range)");
      return W_ERROR_BAD_PARAMETER;
   }

   if (pnValue != null)
   {
      *pnValue = nValue;
   }

   return W_SUCCESS;
}

/** Parses a list of Principals */
static W_ERROR static_PSecurityStackParseSequenceOfPrincipals
(
  const uint8_t* pBuffer,
  uint32_t nBuffer,
  uint32_t* pnPrincipals,
  tPrincipal** ppPrincipals
)
{
   W_ERROR nError = W_SUCCESS;

   uint32_t nPrincipals = 0;

   /*
    * The sequence of "Principal" is a list of:
    * - 81 - [1] endEntityID (OCTET STRING)
    */
   uint8_t nTag = P_TAG_EMPTY;
   uint32_t nLength = 0;
   const uint8_t* pValue = (uint8_t*)null;

   while(nBuffer > 0)
   {
      /* Get the next tag (which must be a context-specific OCTET STRING) */
      nError = static_PSecurityStackGetNextTLV(
        &pBuffer, &nBuffer,
        &nTag, &nLength, &pValue
      );
      if (nError != W_SUCCESS)
         { break; }

      if (nTag != P_TAG(P_TAG_CONTEXT_SPECIFIC, P_TAG_PRIMITIVE, 1))
      {
         PDebugError("static_PSecurityStackParseSequenceOfPrincipals: Unexpected tag");
         nError = W_ERROR_BAD_PARAMETER;
         break;
      }

      if (nLength != P_SECSTACK_PRINCIPALID_LENGTH)
      {
         PDebugError("static_PSecurityStackParseSequenceOfPrincipals: Invalid PrincipalID length");
         nError = W_ERROR_BAD_PARAMETER;
         break;
      }

      if ((ppPrincipals != null) && (pnPrincipals != null))
      {
         if (nPrincipals >= (*pnPrincipals))
         {
            /* There is not enough space in the passed array */
            PDebugError("static_PSecurityStackParseSequenceOfPrincipals: Buffer too short");
            return W_ERROR_BUFFER_TOO_SHORT;
         }

         CMemoryCopy((*ppPrincipals)[nPrincipals].id, pValue, nLength);
      }

      nPrincipals++;
   }

   if (pnPrincipals != null)
   {
      *pnPrincipals = nPrincipals;
   }

   return nError;
}

/** Parses a list of APDU permissions */
static W_ERROR static_PSecurityStackParseSequenceOfAPDUMaskPermissions
(
  const uint8_t* pBuffer,
  uint32_t nBuffer,
  uint32_t* pnPermissions,
  tPermission** ppPermissions
)
{
   W_ERROR nError = W_SUCCESS;

   uint32_t nPermissions = 0;

   /*
    * The sequence of "APDUMaskPermission" is a list of:
    * - A0 - [0] APDUMaskPermission (SEQUENCE)
    * -    04 - apduHeader (OCTET STRING)
    * -    04 - mask (OCTET STRING)
    */
   uint8_t nTag = P_TAG_EMPTY;
   uint32_t nLength = 0;
   const uint8_t* pValue = (uint8_t*)null;

   while(nBuffer > 0)
   {
      const uint8_t* pAPDUMaskPermission = (uint8_t*)null;
      uint32_t nAPDUMaskPermission = 0;
      const uint8_t* pAPDUHeader = (uint8_t*)null;
      const uint8_t* pAPDUMask = (uint8_t*)null;

      /* Get the next tag (which must be a context-specific SEQUENCE) */
      nError = static_PSecurityStackGetNextTLV(
        &pBuffer, &nBuffer,
        &nTag, &nLength, &pValue
      );
      if (nError != W_SUCCESS)
         { break; }

      if (nTag != P_TAG(P_TAG_CONTEXT_SPECIFIC, P_TAG_CONSTRUCTED, 0))
      {
         PDebugError("static_PSecurityStackParseSequenceOfAPDUMaskPermissions: Unexpected tag");
         nError = W_ERROR_BAD_PARAMETER;
         break;
      }

      /* Parse the {apduHeader, mask} fields */
      pAPDUMaskPermission = pValue;
      nAPDUMaskPermission = nLength;

      /* Get the next tag (which must be an OCTET STRING for apduHeader) */
      nError = static_PSecurityStackGetNextTLV(
        &pAPDUMaskPermission, &nAPDUMaskPermission,
        &nTag, &nLength, &pValue
      );
      if (nError != W_SUCCESS)
         { return nError; }

      if ((nTag != P_TAG_UNIVERSAL_OCTET_STRING) && (nLength != P_SECSTACK_APDU_PERMISSION_LENGTH))
      {
         PDebugError("static_PSecurityStackParseSequenceOfAPDUMaskPermissions: Invalid value (apduHeader)");
         nError = W_ERROR_BAD_PARAMETER;
         break;
      }

      pAPDUHeader = pValue;

      /* Get the next tag (which must be an OCTET STRING for mask) */
      nError = static_PSecurityStackGetNextTLV(
        &pAPDUMaskPermission, &nAPDUMaskPermission,
        &nTag, &nLength, &pValue
      );
      if (nError != W_SUCCESS)
         { return nError; }

      if ((nTag != P_TAG_UNIVERSAL_OCTET_STRING) && (nLength != P_SECSTACK_APDU_PERMISSION_LENGTH))
      {
         PDebugError("static_PSecurityStackParseSequenceOfAPDUMaskPermissions: Invalid value (mask)");
         nError = W_ERROR_BAD_PARAMETER;
         break;
      }

      pAPDUMask = pValue;

      /* No data must follow */
      if (nAPDUMaskPermission != 0)
      {
         PDebugError("static_PSecurityStackParseSequenceOfAPDUMaskPermissions: Invalid value");
         nError = W_ERROR_BAD_PARAMETER;
         break;
      }

      if ((ppPermissions != null) && (pnPermissions != null))
      {
         if (nPermissions >= (*pnPermissions))
         {
            /* There is not enough space in the passed array */
            return W_ERROR_BUFFER_TOO_SHORT;
         }

         CMemoryCopy((*ppPermissions)[nPermissions].apduHeader,
           pAPDUHeader, P_SECSTACK_APDU_PERMISSION_LENGTH
         );
         CMemoryCopy((*ppPermissions)[nPermissions].apduMask,
           pAPDUMask, P_SECSTACK_APDU_PERMISSION_LENGTH
         );
      }

      nPermissions++;
   }

   if (pnPermissions != null)
   {
      *pnPermissions = nPermissions;
   }

   return nError;
}

/** Parses an ACE */
static W_ERROR static_PSecurityStackParseACE
(
  tSecurityStackACL* pACL,
  uint32_t nACEIndex,
  uint32_t nACELength,
  const uint8_t* pBuffer, /* ACE data buffer */
  uint32_t nBuffer, /* ACE data length */
  tACE** ppACE
)
{
   W_ERROR nError = W_SUCCESS;

   uint32_t nPrincipals = 0;
   tPrincipal* pPrincipals = (tPrincipal*)null;
   uint32_t nPermissions = 0;
   tPermission* pPermissions = (tPermission*)null;
   tACE* pACE = (tACE*)null;

   /*
    * The ACIE is a SEQUENCE formated as:
    * - 30 SEQUENCE
    * -    A0 - [0] Principals (SEQUENCE OF) OPTIONAL
    * -       81 - [1] endEntityID (OCTET STRING)
    * -       n times
    * -    A1 - [1] Permissions (SEQUENCE OF) OPTIONAL
    * -       A0 - [0] APDUMaskPermission (SEQUENCE)
    * -          04 - apduHeader (OCTET STRING)
    * -          04 - mask (OCTET STRING)
    * -       n times
    */
   uint8_t nTag = P_TAG_EMPTY;
   uint32_t nLength = 0;
   const uint8_t* pValue = (uint8_t*)null;

   /* Get the first tag (which must be a SEQUENCE) */
   nError = static_PSecurityStackGetNextTLV(
     &pBuffer, &nBuffer,
     &nTag, &nLength, &pValue
   );
   if (nError != W_SUCCESS)
      { return nError; }

   /* Check that this a SEQUENCE and that data don't follow */
   if (nTag != P_TAG_UNIVERSAL_SEQUENCE)
   {
      PDebugError("static_PSecurityStackParseACE: Unexpected tag (must be a SEQUENCE)");
      return W_ERROR_BAD_PARAMETER;
   }

   if (nBuffer != 0)
   {
      PDebugError("static_PSecurityStackParseACE: Unexpected value (too much data)");
      return W_ERROR_BAD_PARAMETER;
   }

   pBuffer = pValue;
   nBuffer = nLength;

   /* Get the next tag */
   nError = static_PSecurityStackGetNextTLV(
     &pBuffer, &nBuffer,
     &nTag, &nLength, &pValue
   );
   if (nError != W_SUCCESS)
      { return nError; }

   /* Is the next tag the SEQUENCE OF "Principal" ? */
   if (nTag == P_TAG(P_TAG_CONTEXT_SPECIFIC, P_TAG_CONSTRUCTED, 0))
   {
      if (nLength != 0)
      {
         /* The SEQUENCE of "Principal" is not empty */
         nError = static_PSecurityStackParseSequenceOfPrincipals(
           pValue, nLength, &nPrincipals, (tPrincipal**)null
         );
         if (nError != W_SUCCESS)
            { goto return_result; }

         pPrincipals = (tPrincipal*)CMemoryAlloc(nPrincipals*sizeof(tPrincipal));
         if (pPrincipals == null)
         {
            PDebugError("static_PSecurityStackParseACE: Unable to allocate memory");
            nError = W_ERROR_OUT_OF_RESOURCE;
            goto return_result;
         }

         nError = static_PSecurityStackParseSequenceOfPrincipals(
           pValue, nLength, &nPrincipals, &pPrincipals
         );
         if (nError != W_SUCCESS)
            { goto return_result; }
      }

      /* Get the next tag (which should the SEQUENCE of "Permission") */
      nError = static_PSecurityStackGetNextTLV(
        &pBuffer, &nBuffer,
        &nTag, &nLength, &pValue
      );
      if (nError != W_SUCCESS)
         { goto return_result; }
   }

   /* Is the next tag the SEQUENCE OF "Permission" ? */
   if (nTag == P_TAG(P_TAG_CONTEXT_SPECIFIC, P_TAG_CONSTRUCTED, 1))
   {
      if (nLength != 0)
      {
         /* The SEQUENCE of "Principal" is not empty */
         nError = static_PSecurityStackParseSequenceOfAPDUMaskPermissions(
           pValue, nLength, &nPermissions, (tPermission**)null
         );
         if (nError != W_SUCCESS)
            { goto return_result; }

         pPermissions = (tPermission*)CMemoryAlloc(nPermissions*sizeof(tPermission));
         if (pPermissions == null)
         {
            PDebugError("static_PSecurityStackParseACE: Unable to allocate memory");
            nError = W_ERROR_OUT_OF_RESOURCE;
            goto return_result;
         }

         nError = static_PSecurityStackParseSequenceOfAPDUMaskPermissions(
           pValue, nLength, &nPermissions, &pPermissions
         );
         if (nError != W_SUCCESS)
            { goto return_result; }
      }
   }

   /* No data must follow */
   if (nBuffer != 0)
   {
      PDebugError("static_PSecurityStackParseACE: Too much data");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_result;
   }

   /*
    * At this point, the ACE entry has been successfully parsed.
    */

   /* Create the ACE */
   if ((pACE = (tACE*)CMemoryAlloc(sizeof(tACE))) == null)
   {
      PDebugError("static_PSecurityStackParseACE: Unable to allocate memory");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_result;
   }

   CMemoryFill(pACE, 0, sizeof(*pACE));
   pACE->pNextACE = (tACE*)null;
   pACE->nIndex = nACEIndex;
   pACE->nLength = nACELength;
   pACE->nPrincipals = nPrincipals;
   pACE->pPrincipals = pPrincipals;
   pACE->nPermissions = nPermissions;
   pACE->pPermissions = pPermissions;

   *ppACE = pACE;

   return W_SUCCESS;

return_result:

   if (nError != W_SUCCESS)
   {
      if (pPrincipals != null)
      {
         CMemoryFree(pPrincipals);
         pPrincipals = (tPrincipal*)null;
      }

      if (pPermissions != null)
      {
         CMemoryFree(pPermissions);
         pPermissions = (tPermission*)null;
      }
   }

   return nError;
}

/** Parses an ACIE */
static W_ERROR static_PSecurityStackParseACIE
(
  tSecurityStackACL* pACL,
  const uint8_t* pACLBuffer,
  uint32_t nACLBuffer,
  const uint8_t* pBuffer, /* ACIE data buffer */
  uint32_t nBuffer  /* ACIE data length */
)
{
   W_ERROR nError = W_SUCCESS;

   uint32_t nAID = 0;
   const uint8_t* pAID = (uint8_t*)null;

   uint32_t nACEIndex = 0;
   uint32_t nACELength = 0;
   tACE* pACE = (tACE*)null;

   tACIE* pACIE = (tACIE*)null;

   /*
    * The ACIE is a SEQUENCE that contains the following fields:
    * - 04 aid    (OCTET STRING) OPTIONAL
    *   30 SEQUENCE
    * -    04 - path   (OCTET STRING) -- must be '5036'
    * -    02 - index  (INTEGER)
    * -    80 - [0] length (INTEGER)
    */
   uint8_t nTag = P_TAG_EMPTY;
   uint32_t nLength = 0;
   const uint8_t* pValue = (uint8_t*)null;

   /* Get the first tag */
   nError = static_PSecurityStackGetNextTLV(
     &pBuffer, &nBuffer,
     &nTag, &nLength, &pValue
   );
   if (nError != W_SUCCESS)
      { return nError; }

   /* Is the next tag an octet string, ie. the (optional) AID ? */
   if (nTag == P_TAG_UNIVERSAL_OCTET_STRING)
   {
      /* The AID length must be between 5 and 16 */
      if ((nLength < 5) || (nLength > 16))
      {
         PDebugError("static_PSecurityStackParseACIE: Invalid AID length");
         return W_ERROR_BAD_PARAMETER;
      }

      /* Remember AID */
      nAID = nLength;
      pAID = pValue;

      /* Get the next tag (which must be a SEQUENCE, ie. '30') */
      nError = static_PSecurityStackGetNextTLV(
        &pBuffer, &nBuffer,
        &nTag, &nLength, &pValue
      );
      if (nError != W_SUCCESS)
         { return nError; }
   }

   /* Is the next tag a SEQUENCE of {path, index, length} ? */
   if (nTag != P_TAG_UNIVERSAL_SEQUENCE)
   {
      PDebugError("static_PSecurityStackParseACIE: Unexpected tag (must be a SEQUENCE)");
      return W_ERROR_BAD_PARAMETER;
   }

   /* The SEQUENCE tag must be the last tag as well */
   if (nBuffer != 0)
   {
      PDebugError("static_PSecurityStackParseACIE: Too much data");
      return W_ERROR_BAD_PARAMETER;
   }

   /* Parse the SEQUENCE tag */
   pBuffer = pValue;
   nBuffer = nLength;

   /* Get the first tag of the SEQUENCE of {path, index, length}, ie. path */
   nError = static_PSecurityStackGetNextTLV(
     &pBuffer, &nBuffer,
     &nTag, &nLength, &pValue
   );
   if (nError != W_SUCCESS)
      { return nError; }

   /* Check tag 'path' that must contain the FID of the EF(SE-ACF) file, ie. '5036' */
   if ((nTag != P_TAG_UNIVERSAL_OCTET_STRING) || (nLength != 2))
   {
      PDebugError("static_PSecurityStackParseACIE: Invalid path tag or length");
      return W_ERROR_BAD_PARAMETER;
   }

   /* The FID must be '5036' */
   nError = static_PSecurityStackParseInteger(pValue, nLength, (sint32_t*)null, P_SECSTACK_FID_EF_SE_ACF, P_SECSTACK_FID_EF_SE_ACF);
   if (nError != W_SUCCESS)
   {
      PDebugError("static_PSecurityStackParseACIE: Invalid path value");
      return nError;
   }

   /* Get the second tag of the SEQUENCE of {path, index, length}, ie. index */
   nError = static_PSecurityStackGetNextTLV(
     &pBuffer, &nBuffer,
     &nTag, &nLength, &pValue
   );
   if (nError != W_SUCCESS)
      { return nError; }

   if (nTag != P_TAG_UNIVERSAL_INTEGER)
   {
      PDebugError("static_PSecurityStackParseACIE: Unexpected tag for the index field");
      return W_ERROR_BAD_PARAMETER;
   }

   /* The ACE index must be in range 0 to 32767 */
   nError = static_PSecurityStackParseInteger(pValue, nLength, (sint32_t*)&nACEIndex, P_SECSTACK_ACE_INDEX_MIN, P_SECSTACK_ACE_INDEX_MAX);
   if (nError != W_SUCCESS)
      { return nError; }

   /* Get the second tag of the SEQUENCE of {path, index, length}, ie. length */
   nError = static_PSecurityStackGetNextTLV(
     &pBuffer, &nBuffer,
     &nTag, &nLength, &pValue
   );
   if (nError != W_SUCCESS)
      { return nError; }

   if (nTag != P_TAG(P_TAG_CONTEXT_SPECIFIC, P_TAG_PRIMITIVE, 0))
   {
      PDebugError("static_PSecurityStackParseACIE: Unexpected tag for the length field");
      return W_ERROR_BAD_PARAMETER;
   }

   /* The ACE length must be in range 0 to 32767 */
   nError = static_PSecurityStackParseInteger(pValue, nLength, (sint32_t*)&nACELength, P_SECSTACK_ACE_LENGTH_MIN, P_SECSTACK_ACE_LENGTH_MAX);
   if (nError != W_SUCCESS)
      { return nError; }

   /* No data must follow */
   if (nBuffer != 0)
   {
      PDebugError("static_PSecurityStackParseACIE: Too much data");
      return W_ERROR_BAD_PARAMETER;
   }

   /*
    * At this point, the ACIE entry has been successfully parsed.
    */

   /* Check whether the ACE has already been parsed */

   nError = static_PSecurityStackLookupACE(pACL,
      nACEIndex, nACELength,
      &pACE
   );
   if ((nError != W_SUCCESS) && (nError != W_ERROR_ITEM_NOT_FOUND))
      { return nError; }

   if (nError == W_ERROR_ITEM_NOT_FOUND)
   {
      /* Check that ACE index and length are not out of bounds */
      if ((nACEIndex + nACELength) > nACLBuffer)
      {
         PDebugError("static_PSecurityStackParseACIE: index/length are out of bounds");
         return W_ERROR_BAD_PARAMETER;
      }

      nError = static_PSecurityStackParseACE(pACL,
        nACEIndex, nACELength,
        pACLBuffer+nACEIndex, nACELength,
        &pACE
      );
      if (nError != W_SUCCESS)
         { return nError; }

      /* Append ACE to the ACL data structure */
      if (pACL->pFirstACE == null)
      {
         pACL->pFirstACE = pACE;
      }
      else
      {
         tACE* pLastACE;
         for(pLastACE = pACL->pFirstACE; pLastACE->pNextACE != null; pLastACE = pLastACE->pNextACE);
         pLastACE->pNextACE = pACE;
      }
   }

   /* Create ACIE */
   if ((pACIE = (tACIE*)CMemoryAlloc(sizeof(tACIE))) == null)
   {
      PDebugError("static_PSecurityStackParseACIE: Unable to allocate memory");
      return W_ERROR_OUT_OF_RESOURCE;
   }

   CMemoryFill(pACIE, 0, sizeof(*pACIE));
   if (pAID != null)
   {
      pACIE->nAID = nAID;
      CMemoryCopy(pACIE->aAID, pAID, nAID);
   }
   pACIE->pACE = pACE;

   /* Append ACIE to the ACL data structure */
   if (pACL->pFirstACIE == null)
   {
      pACL->pFirstACIE = pACIE;
   }
   else
   {
      tACIE* pLastACIE;
      for(pLastACIE = pACL->pFirstACIE; pLastACIE->pNextACIE != null; pLastACIE = pLastACIE->pNextACIE);
      pLastACIE->pNextACIE = pACIE;
   }

   return nError;
}

/* See header file */
W_ERROR PSecurityStackParseACL
(
   const uint8_t* pBuffer,
   uint32_t nBuffer,
   tSecurityStackACL** ppACL
)
{
   W_ERROR nError = W_SUCCESS;

   const uint8_t* pACLBuffer = pBuffer;
   uint32_t nACLBuffer = nBuffer;

   tSecurityStackACL* pACL = (tSecurityStackACL*)null;

   if (pBuffer == null)
   {
      return W_ERROR_BAD_PARAMETER;
   }

   if ((pACL=(tSecurityStackACL*)CMemoryAlloc(sizeof(tSecurityStackACL))) == null)
   {
      PDebugError("PSecurityStackParseACL: Unable to allocate memory");
      return W_ERROR_OUT_OF_RESOURCE;
   }

   CMemoryFill(pACL, 0, sizeof(*pACL));
   pACL->pFirstACE = (tACE*)null;
   pACL->pFirstACIE = (tACIE*)null;

   /*
    * The ACL is a list of:
    * - tags '30' containing an ACIE
    * - tags '00' (Empty element) containing the ACE
    */
   while(nBuffer > 0)
   {
      uint8_t nTag = P_TAG_EMPTY;
      uint32_t nLength = 0;
      const uint8_t* pValue = (uint8_t*)null;

      nError = static_PSecurityStackGetNextTLV(&pBuffer, &nBuffer,
        &nTag, &nLength, &pValue
      );
      if (nError != W_SUCCESS)
         { break; }

      /* Skip Empty elements */
      if (nTag == P_TAG_EMPTY)
         { continue; }

      /* Check that the next tag is a SEQUENCE (containing an ACIE) */
      if (nTag != P_TAG_UNIVERSAL_SEQUENCE)
      {
         PDebugError("PSecurityStackParseACL: Unexpected tag");
         nError = W_ERROR_BAD_PARAMETER;
         break;
      }

      nError = static_PSecurityStackParseACIE(pACL,
         pACLBuffer, nACLBuffer,
         pValue, nLength
      );
      if (nError != W_SUCCESS)
         { break; }
   }

   if (nError != W_SUCCESS)
   {
      if (pACL != null)
      {
         PSecurityStackFreeACL(pACL);
         pACL = (tSecurityStackACL*)null;
      }
   }
   else
   {
      *ppACL = pACL;
   }

   return nError;
}

/* See header file */
W_ERROR PSecurityStackFreeACL
(
   tSecurityStackACL* pACL
)
{
   tACE* pACE;
   tACIE* pACIE;

   if (pACL == null)
   {
      return W_ERROR_BAD_PARAMETER;
   }

   /* Free the (linked) list of ACIE */
   pACIE = pACL->pFirstACIE;
   while(pACIE != null)
   {
      tACIE* pNextACIE = pACIE->pNextACIE;
      CMemoryFree(pACIE);
      pACIE = pNextACIE;
   }

   /* Free the (linked) list of ACE */
   pACE = pACL->pFirstACE;
   while(pACE != null)
   {
      tACE* pNextACE = pACE->pNextACE;

      if (pACE->pPrincipals != null)
      {
         CMemoryFree(pACE->pPrincipals);
         pACE->pPrincipals = (tPrincipal*)null;
      }

      if (pACE->pPermissions != null)
      {
         CMemoryFree(pACE->pPermissions);
         pACE->pPermissions = (tPermission*)null;
      }

      pACE = pNextACE;
   }

   /* Free the ACL itself */
   CMemoryFree(pACL);

   return W_SUCCESS;
}

/** The AID of the PKCS#15 application */
static const uint8_t P_SECSTACK_PKCS15_AID[]=
{
   0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43,
   0x53, 0x2D, 0x31, 0x35
};

/* See header file */
W_ERROR PSecurityStackCheckACL(
   tContext* pContext,
   tSecurityStackACL* pACL,
   const uint8_t* pAID,
   uint32_t nAID,
   const uint8_t* pAPDUHeader)
{
   tACIE* pACIE = (tACIE*)null;

   PDebugTrace("PSecurityStackCheckACL()");

   if ((pAID == null) || (nAID < P_SECSTACK_AID_MIN_LENGTH) || (nAID > P_SECSTACK_AID_MAX_LENGTH))
   {
      PDebugError("PSecurityStackCheckACL: Invalid AID parameter");
      return W_ERROR_BAD_PARAMETER;
   }

   PDebugTrace("PSecurityStackCheckACL: AID");
   PDebugTraceBuffer(pAID, nAID);
   PDebugTrace("PSecurityStackCheckACL: APDU");
   PDebugTraceBuffer(pAPDUHeader, 4);

   /* Manage built-in access conditions */

/*@todo*/
#undef P_INCLUDE_JUPITER_SE
#define P_INCLUDE_JUPITER_SE
#ifdef P_INCLUDE_JUPITER_SE
{
   /* Jupiter AID */
   static const uint8_t P_SECSTACK_JUPITER_AID[] =
   {
      0xF0, 0x69, 0x6E, 0x63, 0x6C, 0x53, 0x45, 0x53, 0x65, 0x74, 0x74, 0x69,
      0x6E, 0x67, 0x73
   };

   /* Allow the selection and all APDU commands sent to the Jupiter applet */
   if ((nAID == sizeof(P_SECSTACK_JUPITER_AID)) && (CMemoryCompare(P_SECSTACK_JUPITER_AID, pAID, nAID) == 0))
   {
      return W_SUCCESS;
   }
}
#endif

   if (pACL == null)
   {
      PDebugError("PSecurityStackCheckACL: Invalid ACL parameter");
      return W_ERROR_SECURITY;
   }

   /*
    * Scan all ACIE until a matching permission is found.
    */
   for(pACIE = pACL->pFirstACIE; pACIE != null; pACIE = pACIE->pNextACIE)
   {
      /*
       * If the AID of the ACIE is empty, then the corresponding ACE applies.
       */
      if (pACIE->nAID != 0)
      {
         /* Compare AID */
         if ((pACIE->nAID == nAID) && (CMemoryCompare(pACIE->aAID, pAID, nAID) == 0))
         {
            /* AID matches */
            goto found_aid;
         }

         /* AID does not match. Skip to next ACIE */
         continue;
      }

   found_aid:

      /*
       * If the list of principals is empty, then permissions apply.
       */
      if (pACIE->pACE->nPrincipals != 0)
      {
         /* Look for a matching principal ID */
         uint32_t i;
         for(i = 0; i < pACIE->pACE->nPrincipals; i++)
         {
            if (PSecurityManagerDriverCheckIdentity(
               pContext,
               pACIE->pACE->pPrincipals[i].id,
               P_SECSTACK_PRINCIPALID_LENGTH) == W_SUCCESS)
            {
               /* Principal ID matches */
               goto found_principal;
            }
         }

         /* Principal ID does not match. Skip to next ACIE */
         continue;
      }

   found_principal:

      /*
       * If the 'pAPDUHeader' parameter is null (which is used to check wheter the AID and/or
       * Principal are referred to from the ACL), we're done.
       *
       * If the list of permissions is empty, then access is granted.
       */
      if ((pAPDUHeader != null) && (pACIE->pACE->nPermissions != 0))
      {
         /* Look for a matching permission */
         uint32_t i;
         for(i = 0; i < pACIE->pACE->nPermissions; i++)
         {
            tPermission* pPermission = &pACIE->pACE->pPermissions[i];

            uint8_t aMaskedAPDUHeader[P_SECSTACK_APDU_PERMISSION_LENGTH]; /* Length is assumed to be 4 though !!! */
            aMaskedAPDUHeader[0] = (uint8_t)(pAPDUHeader[0] & pPermission->apduMask[0]);
            aMaskedAPDUHeader[1] = (uint8_t)(pAPDUHeader[1] & pPermission->apduMask[1]);
            aMaskedAPDUHeader[2] = (uint8_t)(pAPDUHeader[2] & pPermission->apduMask[2]);
            aMaskedAPDUHeader[3] = (uint8_t)(pAPDUHeader[3] & pPermission->apduMask[3]);

            if (CMemoryCompare(aMaskedAPDUHeader, pPermission->apduHeader, P_SECSTACK_APDU_PERMISSION_LENGTH) == 0)
            {
               /* Permission matches */
               goto found_permission;
            }
         }

         /* Permission does not match. Skip to next ACIE */
         continue;
      }

   found_permission:

      PDebugTrace("PSecurityStackCheckACL: Access is granted");
      return W_SUCCESS;
   }

   PDebugTrace("PSecurityStackCheckACL: Access is denied");
   return W_ERROR_SECURITY;
}

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */
