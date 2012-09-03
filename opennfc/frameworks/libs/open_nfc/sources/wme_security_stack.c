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
   Contains the implementation of the security stack functions
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( SECSTACK )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#include "wme_security_stack_acl_manager.h"

/** The maximum of logical channels managed by the security Stack */
#define P_SECSTACK_MAX_LOGICAL_CHANNELS 4

/** The AID of the PKCS#15 application */
static const uint8_t P_SECSTACK_PKCS15_AID[]=
{
   0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43,
   0x53, 0x2D, 0x31, 0x35
};

/** The expected major version of the PKCS#15 application */
#define P_SECSTACK_PKCS15_MAJOR_VERSION 0x01

/** The expected minor version of the PKCS#15 application */
#define P_SECSTACK_PKCS15_MINOR_VERSION 0x00

/** The length of the EF(SE-LastUpdate) file */
#define P_SECSTACK_LASTUPDATE_LENGTH 11

/** Structure used by the security stack for each Secure Element */
struct __tSecurityStackInstance
{
   /** The identifier of this Secure Element */
   uint32_t nIdentifier;
   /** The default principal */
   const uint8_t* pDefaultPrincipal;
   /** The length of the default principal */
   uint32_t nDefaultPrincipalLength;
   /** The function used to send an APDU command to the SE */
   tPBasicExchangeData* pExchangeApduFunction;
   /** The handle to the SE */
   W_HANDLE hConnection;
   /** Whether the PKCS#15 application is present on the Secure Element */
   bool bIsACFPresent;
   /** The date&time of the current ACL */
   uint8_t aACLTime[((P_SECSTACK_LASTUPDATE_LENGTH+3) & ~3)]; /* Aligned on 32-bit boundary */
   /** The current ACL (or null, if the ACL has not been read successfully) */
   tSecurityStackACL* pACL;
   /** The list of the currently selected application AID */
   struct
   {
      /** Length of the AID (0 if the AID is not available) */
      uint32_t nAIDLength;
      /** AID of the application selected on this logical channel */
      uint8_t aAID[P_SECSTACK_AID_MAX_LENGTH];
   }  sCurrent[P_SECSTACK_MAX_LOGICAL_CHANNELS];
   /** The last SELECT[AID] or MANAGE CHANNEL[close] AID command sent to the PKCS#15 application*/
   struct
   {
      /** The last APDU command */
      uint8_t aLastCommand[24];
   }  sCandidate;
   /** The ACF reading context */
   struct
   {
      /** Whether the ACL may have been modified and need to be read again */
      bool bTouched;
      /** The error to be returned (once the ACL has been read) */
      uint32_t nError;
      /** The current state of the state machine */
      uint32_t nState;
      /** The callback to be called (once the ACL has been read) */
      tDFCCallbackContext sCallbackContext;
      /** The logical channel on which the PKCS#15 application is selected */
      uint32_t nChannelId;
      /** The buffer containing the APDU command sent to the SE */
      uint8_t aReaderToCardBuffer[20];
      /** The buffer containing the APDU response returned by the SE */
      uint8_t aCardToReaderBuffer[(256+2)+2]; /* 2 padding bytes */
      /** The number of bytes returned by the SE */
      uint32_t nCardToReaderBufferLength;
      /** The contents of the EF(SE-LastUpdate) file */
      uint8_t aSELastUpdate[((P_SECSTACK_LASTUPDATE_LENGTH+3) & ~3)];
      /** The contents of the EF(SE-ACF) file */
      uint8_t* pSEACF;
      /** The size (in bytes) of the EF(SE-ACF) file */
      uint32_t nSEACFLength;
      /** The number of bytes currently read from the EF(SE-ACF) file */
      uint32_t nSEACFReadLength;
   }  readAcf;
};

/* See header file */
tSecurityStackInstance* PSecurityStackCreateInstance(
      tContext* pContext,
      uint32_t nIdentifier,
      const uint8_t* pDefaultPrincipal,
      uint32_t nDefaultPrincipalLength )
{
   tSecurityStackInstance* pSecurityStackInstance;

   PDebugTrace("PSecurityStackCreateInstance()");

   if ((pSecurityStackInstance = (tSecurityStackInstance*)CMemoryAlloc(sizeof(tSecurityStackInstance) )) == null)
   {
      PDebugError("PSecurityStackCreateInstance: Cannot allocate the structure");
      return (tSecurityStackInstance*)null;
   }

   CMemoryFill( pSecurityStackInstance, 0, sizeof(tSecurityStackInstance) );

   pSecurityStackInstance->nIdentifier = nIdentifier;
   pSecurityStackInstance->pDefaultPrincipal = pDefaultPrincipal;
   pSecurityStackInstance->nDefaultPrincipalLength = nDefaultPrincipalLength;

   /* The ACL must be read asap */
   pSecurityStackInstance->readAcf.bTouched = true;

   return pSecurityStackInstance;
}

/* See header file */
void PSecurityStackDestroyInstance(
      tContext* pContext,
      tSecurityStackInstance* pSecurityStackInstance )
{
   PDebugTrace("PSecurityStackDestroyInstance()");

   if (pSecurityStackInstance != null)
   {
      if (pSecurityStackInstance->pACL != null)
      {
         PSecurityStackFreeACL(pSecurityStackInstance->pACL);
         pSecurityStackInstance->pACL = (tSecurityStackACL*)null;
      }

      CMemoryFree(pSecurityStackInstance);
   }
}


/**
 * Extracts the channel number from the class byte and updates the class byte by removing the logical channel bits.
 *
 * @param[in]     nClass   The class byte.
 *
 * @param[inout]  pnClass  A pointer to the updated class byte. May be null.
 *
 * @param[in]     pnChannelId  A pointer to the extracted logical channel identifier. May be null.
 *
 * @return W_SUCCESS.
 **/
static W_ERROR static_PSecurityStackExtractChannelId
(
   uint8_t nClass,
   uint8_t* pnClass,
   uint32_t* pnChannelId
)
{
   uint32_t nChannelId = 0;

   if ((nClass & 0x80) != 0x00)
   {
      /* Proprietary class byte (this includes the Calypso-specific 0x94 value) */
      goto return_result;
   }

   if ((nClass & 0x40) != 0x00)
   {
      /* New format */
      nChannelId = 4 + (nClass & 0x0F);

      nClass = (nClass & 0x90) | ((nClass & 0x20) >> 2);
   }
   else
   {
      /* Old format */
      nChannelId = (nClass & 0x03);

      nClass = (nClass & 0xFC);
   }

return_result:

   if (pnClass != null)
   {
      *pnClass = nClass;
   }

   if (pnChannelId != null)
   {
      *pnChannelId = nChannelId;
   }

   return W_SUCCESS;
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
static W_ERROR static_PSecurityStackComputeClassByte
(
   uint8_t* pnClass,
   uint8_t nChannelId
)
{
   uint8_t nClass = 0x00;
   uint8_t nSecureMessaging = 0x00;
   uint8_t nSourceClass = *pnClass;

   if (nSourceClass == 0x94)
   {
      /* Special case for the Calypso application */
      if (nChannelId != 0)
      {
         PDebugError("static_PSecurityStackComputeClassByte: CLASS byte 0x94 can only be used with the base logical channel");
         return W_ERROR_RF_COMMUNICATION;
      }

      return W_SUCCESS;
   }
   else if ((nSourceClass & 0x40) == 0x00)
   {
      if (nChannelId > 19)
      {
         PDebugError("static_PSecurityStackComputeClassByte: Invalid channel value");
         return W_ERROR_RF_COMMUNICATION;
      }

      nSecureMessaging = (nSourceClass & 0x0C) >> 2;
   }
   else
   {
      if (nChannelId > 19)
      {
         PDebugError("static_PSecurityStackComputeClassByte: Invalid channel value");
         return W_ERROR_RF_COMMUNICATION;
      }

      nSecureMessaging = (nSourceClass & 0x20) >> 5;
      if (nSecureMessaging != 0)
      {
         nSecureMessaging = 2;
      }
   }

   if (nChannelId <= 3)
   {
      nClass = (nSourceClass & 0x90) | (nSecureMessaging << 2) | nChannelId;
   }
   else
   {
      if ((nSecureMessaging != 0) && (nSecureMessaging != 2))
      {
         PDebugError("static_PSecurityStackComputeClassByte: This type of secure messaging is not supported with channel >= 4");
         return W_ERROR_RF_COMMUNICATION;
      }

      if (nSecureMessaging == 2)
      {
         nSecureMessaging = 1;
      }

      CDebugAssert(nChannelId <= 19);
      nClass = 0x40 | (nSourceClass & 0x90) | (nSecureMessaging << 5) | (nChannelId - 4);
   }

   *pnClass = nClass;
   return W_SUCCESS;
}

/** The CLA byte for ISO/IEC 7816-4 compliant commands */
#define P_SECSTACK_CLA_ISO7816 ((uint8_t)0x00)

/** The INS byte of the SELECT command */
#define P_SECSTACK_INS_SELECT ((uint8_t)0xA4)

/** The P1 byte of the SELECT[AID] command */
#define P_SECSTACK_P1_SELECT_AID ((uint8_t)0x04)

/** The P2 byte of the SELECT[AID] command */
#define P_SECSTACK_P2_SELECT_AID ((uint8_t)0x00)

/** The INS byte of the READ BINARY command */
#define P_SECSTACK_INS_READ_BINARY ((uint8_t)0xB0)

/** The INS byte of the UPDATE BINARY command */
#define P_SECSTACK_INS_UPDATE_BINARY ((uint8_t)0xD6)

/** The INS byte of the MANAGE CHANNEL command */
#define P_SECSTACK_INS_MANAGE_CHANNEL ((uint8_t)0x70)

/** The P1 byte of the MANAGE CHANNEL[open] command */
#define P_SECSTACK_P1_MANAGE_CHANNEL_OPEN ((uint8_t)0x00)

/** The P1 byte of the MANAGE CHANNEL[close] command */
#define P_SECSTACK_P1_MANAGE_CHANNEL_CLOSE ((uint8_t)0x80)

/** The INS byte of the GET RESPONSE command */
#define P_SECSTACK_INS_GET_RESPONSE ((uint8_t)0xC0)

/**
 * @brief Processes APDU commands sent to the PKCS#15 application.
 *
 * This function intercepts all APDU commands sent to the SE and checks whether this is an UPDATE BINARY
 * command sent to the PKCS#15 application. If so, the PKCS#15 application data will be read again from
 * the PSecurityStackNotifyEndofConnection function.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
 *
 * @param[in]  pReaderToCardBuffer  A pointer on the buffer containing the data to send to the card.
 *             This value may be null if nReaderToCardBufferLength is set to zero.
 *
 * @param[in]  nReaderToCardBufferLength  The length in bytes of the data to send to the card.
 *             This value may be zero.
 */
static void static_PSecurityStackProcessPKCS15Command
(
   tSecurityStackInstance* pSecurityStackInstance,
   const uint8_t* pReaderToCardBuffer,
   uint32_t nReaderToCardBufferLength
)
{
   uint32_t nChannelId;

   CDebugAssert(pSecurityStackInstance != null);

   if (pSecurityStackInstance->readAcf.bTouched)
      { return; }

   if ((nReaderToCardBufferLength < 5) || (pReaderToCardBuffer == null))
      { return; }

   if (pReaderToCardBuffer[1] != P_SECSTACK_INS_UPDATE_BINARY)
      { return; }

   if (static_PSecurityStackExtractChannelId(pReaderToCardBuffer[0], (uint8_t*)null, &nChannelId) != W_SUCCESS)
     { return; }

   if (nChannelId >= P_SECSTACK_MAX_LOGICAL_CHANNELS)
     { return; }

   if (pSecurityStackInstance->sCurrent[nChannelId].nAIDLength != sizeof(P_SECSTACK_PKCS15_AID))
      { return; }

   if (CMemoryCompare(pSecurityStackInstance->sCurrent[nChannelId].aAID, P_SECSTACK_PKCS15_AID, sizeof(P_SECSTACK_PKCS15_AID)) != 0)
      { return; }

   pSecurityStackInstance->readAcf.bTouched = true;
   return;
}

/* See header file */
W_ERROR PSecurityStackFilterApdu(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            const uint8_t* pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength )
{
   W_ERROR nError = W_ERROR_SECURITY;

   PDebugTrace("PSecurityStackFilterApdu()");

   if ((pSecurityStackInstance == null) || (pReaderToCardBuffer == null))
   {
	   PDebugError("PSecurityStackFilterApdu: Invalid parameter");
	   return W_ERROR_BAD_PARAMETER;
   }

   if (pSecurityStackInstance->bIsACFPresent == false)
   {
      PDebugTrace("PSecurityStackFilterApdu: There is no PKCS#15 application. Access is granted");
	   return W_SUCCESS;
   }

   /* Reset the candidate context */
   CMemoryFill(&pSecurityStackInstance->sCandidate, 0, sizeof(pSecurityStackInstance->sCandidate));

   /* Is it a SELECT[AID] command with Lc=0? */
   if ( (nReaderToCardBufferLength == 5) &&
        (pReaderToCardBuffer[1] == P_SECSTACK_INS_SELECT) &&
        (pReaderToCardBuffer[2] == P_SECSTACK_P1_SELECT_AID) &&
        (pReaderToCardBuffer[4] == 0))
   {
	   PDebugTrace("PSecurityStackFilterApdu: Filtering SELECT[AID] command with Lc=0");

      CMemoryCopy(pSecurityStackInstance->sCandidate.aLastCommand, pReaderToCardBuffer, nReaderToCardBufferLength);

      return W_SUCCESS;
   }

   /* Is it a MANAGE CHANNEL[close] command */
   if ( ((nReaderToCardBufferLength == 4) || (nReaderToCardBufferLength == 5)) && /* P3 present or not */
        (pReaderToCardBuffer[1] == P_SECSTACK_INS_MANAGE_CHANNEL) &&
        (pReaderToCardBuffer[2] == P_SECSTACK_P1_MANAGE_CHANNEL_CLOSE))
   {
	   PDebugTrace("PSecurityStackFilterApdu: Filtering MANAGE CHANNEL[close] command");

      CMemoryCopy(pSecurityStackInstance->sCandidate.aLastCommand, pReaderToCardBuffer, nReaderToCardBufferLength);

      return W_SUCCESS;
   }

   /*
    * Is it a command sent to the PKCS15 application?
    */
   static_PSecurityStackProcessPKCS15Command(pSecurityStackInstance, pReaderToCardBuffer, nReaderToCardBufferLength);

   /*
    * If the caller is the default principal, access is granted
    */
   if (PSecurityManagerDriverCheckIdentity(
      pContext,
      pSecurityStackInstance->pDefaultPrincipal,
      pSecurityStackInstance->nDefaultPrincipalLength) == W_SUCCESS)
   {
	   PDebugTrace("PSecurityStackFilterApdu: The caller is the default principal. Access is granted.");
      return W_SUCCESS;
   }

   /* The passed APDU must be at least 4-byte long */
   if (nReaderToCardBufferLength < 4)
   {
	   PDebugError("PSecurityStackFilterApdu: Malformed APDU command (too short)");
	   return W_ERROR_BAD_PARAMETER;
   }

   /* Is it a SELECT[AID] command? */
   if ((pReaderToCardBuffer[1] == P_SECSTACK_INS_SELECT) && (pReaderToCardBuffer[2] == P_SECSTACK_P1_SELECT_AID))
   {
      /* Lc should be in range 5 to 16 */
      if ((nReaderToCardBufferLength < 5) ||
          (pReaderToCardBuffer[4] > (nReaderToCardBufferLength - 5 + 1)) || /* +1 in case Le is present */
          (pReaderToCardBuffer[4] < P_SECSTACK_AID_MIN_LENGTH) ||
          (pReaderToCardBuffer[4] > P_SECSTACK_AID_MAX_LENGTH))
      {
   	   PDebugError("PSecurityStackFilterApdu: Malformed SELECT[AID] APDU command (AID too short or too long)");
         return W_ERROR_SECURITY;
      }

      /* Check AID */
      nError = PSecurityStackCheckACL(pContext, pSecurityStackInstance->pACL,
         pReaderToCardBuffer+5, pReaderToCardBuffer[4],
         (const uint8_t*)null
      );

      if (nError != W_SUCCESS)
      {
   	   PDebugError("PSecurityStackFilterApdu: Rejected SELECT[AID] APDU command");
         return W_ERROR_SECURITY;
      }

      /* Remember last command */
      CMemoryCopy(pSecurityStackInstance->sCandidate.aLastCommand, pReaderToCardBuffer, nReaderToCardBufferLength);

      return W_SUCCESS;
   }
   else if ((pReaderToCardBuffer[1] == P_SECSTACK_INS_MANAGE_CHANNEL) && (pReaderToCardBuffer[2] == P_SECSTACK_P1_MANAGE_CHANNEL_CLOSE))
   {
      /* This is a MANAGE CHANNEL[close] command */

      /* Remember last command */
      CMemoryCopy(pSecurityStackInstance->sCandidate.aLastCommand, pReaderToCardBuffer, nReaderToCardBufferLength);

      return W_SUCCESS;
   }
   else
   {
      /* This is not a SELECT[AID] not MANAGE CHANNEL[close] command */

      /* Note: We ignore MANAGE CHANNEL[open] and GET RESPONSE commands */

      if ((pReaderToCardBuffer[1] != P_SECSTACK_INS_MANAGE_CHANNEL) && (pReaderToCardBuffer[1] != P_SECSTACK_INS_GET_RESPONSE))
      {
         uint32_t nChannelId = 0;

         uint8_t aAPDUHeader[4];
         CMemoryCopy(aAPDUHeader, pReaderToCardBuffer, sizeof(aAPDUHeader));
         static_PSecurityStackExtractChannelId(aAPDUHeader[0], &aAPDUHeader[0], &nChannelId);

         if (nChannelId >= P_SECSTACK_MAX_LOGICAL_CHANNELS)
         {
   	      PDebugError("PSecurityStackFilterApdu: Rejected APDU command (unsupported logical channel)");
            return W_ERROR_SECURITY;
         }

         nError = PSecurityStackCheckACL(pContext, pSecurityStackInstance->pACL,
            pSecurityStackInstance->sCurrent[nChannelId].aAID,
            pSecurityStackInstance->sCurrent[nChannelId].nAIDLength,
            aAPDUHeader
         );

         if (nError != W_SUCCESS)
         {
   	      PDebugError("PSecurityStackFilterApdu: Rejected APDU command");
            return W_ERROR_SECURITY;
         }
      }
   }

   return W_SUCCESS;
}

static W_ERROR static_PSecurityStackParseAnswerToSelectTLV
(
   const uint8_t** ppBuffer,
   uint32_t* pnBufferLength,
   uint16_t nExpectedTag,
   uint8_t nExpectedLength,
   const uint8_t** ppValue,
   bool bLastTag
);

/**
 * @brief Extracts the AID of the SE from the answer (FCI) returned by the SE.
 *
 * This function is called when the SE is selected by a SELECT command with an empty AID.
 * Only the Jupiter SE is supported for now.
 *
 * @param[in]  pBuffer  Pointer to the buffer containing the FCI.
 *
 * @param[in]  nBufferLength  The length in bytes of the FCI.
 *
 * @param[out] ppAID  A pointer to the pointer to the extracted AID
 *
 * @param[out] pnAIDLength  A pointer to the length of the extracted AID
 *
 * @return One of the following error codes:
 *          - W_SUCCESS  The AID has been successfully extracted.
 *          - W_ERROR_BAD_PARAMETER The FCI is malformed.
 */
static W_ERROR static_PSecurityStackGetApplicationAID
(
   const uint8_t* pBuffer,
   uint32_t nBufferLength,
   const uint8_t** ppAID,
   uint32_t* pnAIDLength
)
{
   uint32_t nError;
   const uint8_t* pValue;

   /*
    * The Jupiter SE returns the following answer to SELECT.
    * This is the only supported SE for now.
    * 6F 10             FCI template
    *    84 08 xx-xx    PKCS#15 application AID = A0 00 00 00 03 00 00 00
    *    A5 04          Proprietary data
    *       9F-65 01 FF ???
    */

   /* Parse '6F' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0x6F, 0x10, &pValue, true);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackGetApplicationAID: Bad '6F' tag");
      return W_ERROR_BAD_PARAMETER;
   }

   /* Parse '84' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0x84, 0x08, &pValue, false);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackGetApplicationAID: Bad '84' tag");
      return W_ERROR_BAD_PARAMETER;
   }

   *ppAID = pValue;
   *pnAIDLength = 8;

   return W_SUCCESS;
}

/* See header file */
W_ERROR PSecurityStackNotifyResponseApdu(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            const uint8_t* pCardToReaderBuffer,
            uint32_t nCardToReaderBufferLength,
            W_ERROR nError )
{
   PDebugTrace("PSecurityStackNotifyResponseApdu()");

   if ((pSecurityStackInstance == null) || (pCardToReaderBuffer == null))
   {
	   PDebugError("PSecurityStackNotifyResponseApdu: Invalid parameter");
	   return W_ERROR_BAD_PARAMETER;
   }

   if (pSecurityStackInstance->bIsACFPresent == false)
   {
      PDebugLog("PSecurityStackNotifyResponseApdu: The PKCS#15 application is not present");
      return W_SUCCESS;
   }

   if ((nError == W_SUCCESS) && (nCardToReaderBufferLength >= 2))
   {
      if (pSecurityStackInstance->sCandidate.aLastCommand[1] == P_SECSTACK_INS_SELECT)
      {
         /* This was a SELECT[AID] command */

         if ((nCardToReaderBufferLength > 2) || /* R-DATA is present */
             ((pCardToReaderBuffer[0] != 0x6A) && (pCardToReaderBuffer[1] != 0x82))) /* File not found */
         {
            uint32_t nChannelId;
            static_PSecurityStackExtractChannelId(pSecurityStackInstance->sCandidate.aLastCommand[0], (uint8_t*)null, &nChannelId);

            if (nChannelId < P_SECSTACK_MAX_LOGICAL_CHANNELS)
            {
               if (pSecurityStackInstance->sCandidate.aLastCommand[4] == 0x00)
               {
                  /* This was a SELECT[AID] command with Lc=0 */

                  const uint8_t* pAID = (const uint8_t*)null;
                  uint32_t nAIDLength = 0;

                  if ( (static_PSecurityStackGetApplicationAID(pCardToReaderBuffer, nCardToReaderBufferLength-2, &pAID, &nAIDLength) == W_SUCCESS)
                       &&
                       ( (PSecurityManagerDriverCheckIdentity(
                           pContext,
                           pSecurityStackInstance->pDefaultPrincipal,
                           pSecurityStackInstance->nDefaultPrincipalLength) == W_SUCCESS)
                         ||
                         (PSecurityStackCheckACL(
                              pContext,
                              pSecurityStackInstance->pACL,
                              pAID, nAIDLength,
                              (uint8_t*)null) == W_SUCCESS)
                       )
                     )
                  {
                     pSecurityStackInstance->sCurrent[nChannelId].nAIDLength = nAIDLength;
                     CMemoryCopy(pSecurityStackInstance->sCurrent[nChannelId].aAID, pAID, nAIDLength);
                  }
                  else
                  {
                     pSecurityStackInstance->sCurrent[nChannelId].nAIDLength = 0;
                     CMemoryFill(pSecurityStackInstance->sCurrent[nChannelId].aAID, 0, sizeof(pSecurityStackInstance->sCurrent[nChannelId].aAID));
                  }
               }
               else
               {
                  /* This was a SELECT[AID] command with Lc in range 5..16*/
                  CDebugAssert((pSecurityStackInstance->sCandidate.aLastCommand[4] >= P_SECSTACK_AID_MIN_LENGTH) &&
                               (pSecurityStackInstance->sCandidate.aLastCommand[4] <= P_SECSTACK_AID_MAX_LENGTH) );

                  pSecurityStackInstance->sCurrent[nChannelId].nAIDLength = pSecurityStackInstance->sCandidate.aLastCommand[4];
                  CMemoryCopy(pSecurityStackInstance->sCurrent[nChannelId].aAID, pSecurityStackInstance->sCandidate.aLastCommand + 5, pSecurityStackInstance->sCandidate.aLastCommand[4]);
               }
            }
         }

      }
      else if (pSecurityStackInstance->sCandidate.aLastCommand[1] == P_SECSTACK_INS_MANAGE_CHANNEL)
      {
         /* This was a MANAGE CHANNEL[close] command */
         uint32_t nChannelId;
         static_PSecurityStackExtractChannelId(pSecurityStackInstance->sCandidate.aLastCommand[0], (uint8_t*)null, &nChannelId);

         if (nChannelId < P_SECSTACK_MAX_LOGICAL_CHANNELS)
         {
            pSecurityStackInstance->sCurrent[nChannelId].nAIDLength = 0;
            CMemoryFill(pSecurityStackInstance->sCurrent[nChannelId].aAID, 0, sizeof(pSecurityStackInstance->sCurrent[nChannelId].aAID));
         }

      }
   }

   /* Reset candidate context */
   CMemoryFill(&pSecurityStackInstance->sCandidate, 0, sizeof(pSecurityStackInstance->sCandidate));

   return W_SUCCESS;
}


/** The event indicating that the state machine must be started */
#define P_EVENT_INITIAL                    0
/** The event indicating that the state machine has received the answer of the last command */
#define P_EVENT_COMMAND_EXECUTED           1
/* The status indicating that a command is pending */
#define P_STATUS_PENDING_EXECUTED          0x0200
/** The state machine is stopped */
#define P_STATE_IDLE                       0x0000
/** The state machine must send the MANAGE CHANNEL [open] command */
#define P_STATE_MANAGE_CHANNEL_OPEN        0x0001
/** The state machine must send the SELECT command to select the PKCS#15 application */
#define P_STATE_SELECT_PKCS15_APPLICATION  0x0002
/** The state machine must send the first READ BINARY command */
#define P_STATE_READ_EF_SE_ACF_FIRST       0x0003
/** The state machine must send subsequent READ BINARY command */
#define P_STATE_READ_EF_SE_ACF_NEXT        0x0004
/** The state machine must send the MANAGE CHANNEL [close] command */
#define P_STATE_MANAGE_CHANNEL_CLOSE       0x0005
/** The state machine must return a notification to the caller */
#define P_STATE_FINAL                      0x0006

static void static_PSecurityStackLoadAcfStateMachine
(
   tContext* pContext,
   tSecurityStackInstance* pSecurityStackInstance,
   uint32_t nEvent,
   W_ERROR nError
);

static void static_PSecurityStackExchangeApdu
(
   tContext* pContext,
   tSecurityStackInstance* pSecurityStackInstance,
   const uint8_t* pReaderToCardBuffer,
   uint32_t nReaderToCardBufferLength
);

/**
 * @brief Extracts the next TLV from the answer to the SELECT command sent to the PKCS#15 application.
 *
 * @param[inout]  ppBuffer  A pointer to the pointer to the next available data to be parsed
 *                This pointer is updated in case a TLV has been successfully parsed.
 *
 * @param[inout]  pnBufferLength  A pointer to the length of the next available data to be parsed.
 *                This length is updated in case a TLV has been successfully parsed.
 *
 * @param[in]  nExpectedTag  The expected tag
 *
 * @param[in]  nExpectedLength  The expected length in bytes of the tag
 *
 * @param[out]  ppValue  A pointer to the tag value
 *
 * @param[in]  bLastTag  Whether this must be the last tag
 *
 * @return One of the following error codes:
 *          - W_SUCCESS  The TLV has been successfully parsed and a pointer to the value is returned.
 *          - W_ERROR_BAD_PARAMETER  The passed data are malformed.
 */
static W_ERROR static_PSecurityStackParseAnswerToSelectTLV
(
   const uint8_t** ppBuffer,
   uint32_t* pnBufferLength,
   uint16_t nExpectedTag,
   uint8_t nExpectedLength,
   const uint8_t** ppValue,
   bool bLastTag
)
{
   const uint8_t* pBuffer;
   uint32_t nBufferLength;

   CDebugAssert(ppBuffer != null);
   CDebugAssert(pnBufferLength != null);
   CDebugAssert(ppValue != null);

   pBuffer = *ppBuffer;
   nBufferLength = *pnBufferLength;

   /* Parse tag */
   if ((nExpectedTag & 0xFF00) == 0x0000)
   {
      /* 1-byte tag */
      if ((nBufferLength < 1) || (pBuffer[0] != (uint8_t)nExpectedTag))
      {
         return W_ERROR_BAD_PARAMETER;
      }

      pBuffer++;
      nBufferLength--;
   }
   else
   {
      /* 2-byte tag */
      if ((nBufferLength < 2) || (pBuffer[0] != (uint8_t)(nExpectedTag>>8)) || (pBuffer[1] != (uint8_t)nExpectedTag))
      {
         return W_ERROR_BAD_PARAMETER;
      }

      pBuffer += 2;
      nBufferLength -= 2;
   }

   /* Parse length (1-byte length is assumed) */
   if ((nBufferLength < 1) || (pBuffer[0] != nExpectedLength))
   {
      return W_ERROR_BAD_PARAMETER;
   }

   pBuffer++;
   nBufferLength--;

   /* Check whether this is (optionally) actually the last tag */
   if (bLastTag && (nBufferLength != nExpectedLength))
   {
      return W_ERROR_BAD_PARAMETER;
   }

   /* Remember the pointer to the value */
   if (ppValue != null)
   {
      *ppValue = pBuffer;
   }

   if (bLastTag)
   {
      /* Return pointer to the tag value */
      *ppBuffer = pBuffer;
      *pnBufferLength = nBufferLength;
   }
   else
   {
      /* Return pointer to the next tag-length-value */
      *ppBuffer = pBuffer + nExpectedLength;
      *pnBufferLength = nBufferLength - nExpectedLength;
   }

   return W_SUCCESS;
}

/**
 * @brief Parses the answer (FCI) to the SELECT command sent to the PKCS#15 application.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
 *
 * @return One of the following error codes:
 *          - W_SUCCESS  The FCI has been successfully parsed.
 *          - W_ERROR_CONNECTION_COMPATIBILITY  The FCI is malformed.
 *          - W_ERROR_OUT_OF_RESOURCE  Out of memory.
 */
static W_ERROR static_PSecurityStackParseAnswerToSelect
(
   tSecurityStackInstance* pSecurityStackInstance
)
{
   uint32_t nError;

   const uint8_t* pBuffer = pSecurityStackInstance->readAcf.aCardToReaderBuffer;
   uint32_t nBufferLength = pSecurityStackInstance->readAcf.nCardToReaderBufferLength - 2; /* remove SW */
   const uint8_t* pValue;

   /*
    * The answer to the SELECT[AID] command is formatted as:
    * 6F 2F                FCI template
    *    84 0C xx-xx       PKCS#15 application AID
    *    A5 1F             Proprietary data
    *       9F-18 02 xx-xx PKCS#15 Application Version Number
    *       BF-0C 17       FCI Issuer Discretionary Data
    *          51 11 xx.xx Contents of the EF(SE-LastUpdate) file
    *          52 02 xx.xx Size of the EF(SE-ACF) file
    */

   /* Parse '6F' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0x6F, 0x2F, &pValue, true);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad '6F' tag");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Parse '84' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0x84, 0x0C, &pValue, false);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad '84' tag");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Parse 'A5' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0xA5, 0x1F, &pValue, true);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad 'A5' tag");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Parse '9F08' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0x9F08, 0x02, &pValue, false);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad '9F08' tag");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* The version must be 0x01 0x00 */
   if ((pValue[0] != P_SECSTACK_PKCS15_MAJOR_VERSION) || (pValue[1] != P_SECSTACK_PKCS15_MINOR_VERSION))
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad version number");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Parse 'BF0C' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0xBF0C, 0x17, &pValue, true);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad 'BF0C' tag");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }

   /* Parse '51' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0x51, 0x11, &pValue, false);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad '51' tag");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   else
   {
      /* Copy the EF(SE-LastUpdate) file contents */
      CMemoryCopy(pSecurityStackInstance->readAcf.aSELastUpdate, pValue, P_SECSTACK_LASTUPDATE_LENGTH);
   }

   /* Parse '52' tag */
   nError = static_PSecurityStackParseAnswerToSelectTLV(&pBuffer, &nBufferLength, 0x52, 0x02, &pValue, true);
   if (nError != W_SUCCESS)
   {
	   PDebugError("static_PSecurityStackParseAnswerToSelect: Bad '52' tag");
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   else
   {
      if (pSecurityStackInstance->readAcf.pSEACF != null)
      {
         CMemoryFree(pSecurityStackInstance->readAcf.pSEACF);
         pSecurityStackInstance->readAcf.pSEACF = (uint8_t*)null;
      }

      pSecurityStackInstance->readAcf.nSEACFLength = (256*pValue[0]+pValue[1]);

      if ((pSecurityStackInstance->readAcf.pSEACF = CMemoryAlloc(pSecurityStackInstance->readAcf.nSEACFLength)) == null)
      {
         pSecurityStackInstance->readAcf.nSEACFLength = 0;
         return W_ERROR_OUT_OF_RESOURCE;
      }

      CMemoryFill(pSecurityStackInstance->readAcf.pSEACF, 0, pSecurityStackInstance->readAcf.nSEACFLength);
   }

   /* Allocate memory for the EF(SE-ACF) file */

   return W_SUCCESS;
}

/**
 * @brief Processes the notification sent when an APDU exchange is completed.
 *
 * This function processes the 61xx and 6Cxx status words. In case of the 61xx, a GET RESPONSE
 * command is sent. In case of 6Cxx, the command is reissued with the right P3.
 *
 * Otherwise, the state machine entry point is called.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pCallbackParameter  A blind parameter transmitted to the callback function.
 *
 * @param[in]  nDataLength  The length in bytes of returned data.
 *
 * @param[in]  nError  An error code.
 */
static void static_PSecurityStackExchangeApduCompleted
(
   tContext* pContext,
   void* pCallbackParameter,
   uint32_t nDataLength,
   W_ERROR nError
)
{
   tSecurityStackInstance* pSecurityStackInstance = (tSecurityStackInstance*)pCallbackParameter;

   if (nError == W_SUCCESS)
   {
      const uint8_t* pData = pSecurityStackInstance->readAcf.aCardToReaderBuffer+pSecurityStackInstance->readAcf.nCardToReaderBufferLength;

      if (nDataLength < 2)
      {
         nError = W_ERROR_RF_COMMUNICATION;
         goto return_error;
      }

      /* Update the number of received bytes */
      pSecurityStackInstance->readAcf.nCardToReaderBufferLength += nDataLength;

      /* Handle 61xx status word */
      if (pData[nDataLength - 2] == 0x61)
      {
         uint8_t aReaderToCardBuffer[5];
         aReaderToCardBuffer[0] = P_SECSTACK_CLA_ISO7816;
         aReaderToCardBuffer[1] = P_SECSTACK_INS_GET_RESPONSE;
         aReaderToCardBuffer[2] = 0x00;
         aReaderToCardBuffer[3] = 0x00;
         aReaderToCardBuffer[4] = pData[nDataLength - 1];

         /* Forget the 61xx status word */
         pSecurityStackInstance->readAcf.nCardToReaderBufferLength -= 2;

         /* Send the GET RESPONSE command */
         static_PSecurityStackExchangeApdu(pContext, pSecurityStackInstance,
            aReaderToCardBuffer, sizeof(aReaderToCardBuffer)
         );
         return;
      }

      /* Handle 6Cxx status word (just in case, this should never occur) */
      if (pData[nDataLength - 2] == 0x6C)
      {
         uint8_t aReaderToCardBuffer[5];
         aReaderToCardBuffer[0] = pSecurityStackInstance->readAcf.aReaderToCardBuffer[0];
         aReaderToCardBuffer[1] = pSecurityStackInstance->readAcf.aReaderToCardBuffer[1];
         aReaderToCardBuffer[2] = pSecurityStackInstance->readAcf.aReaderToCardBuffer[2];
         aReaderToCardBuffer[3] = pSecurityStackInstance->readAcf.aReaderToCardBuffer[3];
         aReaderToCardBuffer[4] = pData[nDataLength - 1];

         /* Resend the command with the right P3=Le */
         static_PSecurityStackExchangeApdu(pContext, pSecurityStackInstance,
            aReaderToCardBuffer, sizeof(aReaderToCardBuffer)
         );
         return;
      }
   }

return_error:

   static_PSecurityStackLoadAcfStateMachine(pContext, pSecurityStackInstance, P_EVENT_COMMAND_EXECUTED, nError);
}

/**
 * @brief Sends an APDU command to the PKCS#15 application
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
 *
 * @param[in]  pReaderToCardBuffer  A pointer on the buffer containing the data to send to the card.
 *             This value may be null if nReaderToCardBufferLength is set to zero.
 *
 * @param[in]  nReaderToCardBufferLength  The length in bytes of the data to send to the card.
 *             This value may be zero.
 */
static void static_PSecurityStackExchangeApdu
(
   tContext* pContext,
   tSecurityStackInstance* pSecurityStackInstance,
   const uint8_t* pReaderToCardBuffer,
   uint32_t nReaderToCardBufferLength
)
{
   CDebugAssert(pContext != null);
   CDebugAssert(pSecurityStackInstance != null);
   CDebugAssert(pReaderToCardBuffer != null);
   CDebugAssert(nReaderToCardBufferLength <= sizeof(pSecurityStackInstance->readAcf.aReaderToCardBuffer));

   /* Copy APDU command and adjust CLA byte */
   CMemoryCopy(pSecurityStackInstance->readAcf.aReaderToCardBuffer,
      pReaderToCardBuffer, nReaderToCardBufferLength
   );
   static_PSecurityStackComputeClassByte(pSecurityStackInstance->readAcf.aReaderToCardBuffer,
      (uint8_t)pSecurityStackInstance->readAcf.nChannelId
   );

   /* No response bytes received yet */
   pSecurityStackInstance->readAcf.nCardToReaderBufferLength=0;

   /* Send APDU command */
   pSecurityStackInstance->pExchangeApduFunction(pContext,
      pSecurityStackInstance->hConnection,
      static_PSecurityStackExchangeApduCompleted, pSecurityStackInstance,
      /* C-APDU */
      pSecurityStackInstance->readAcf.aReaderToCardBuffer,
      nReaderToCardBufferLength,
      /* R-APDU */
      pSecurityStackInstance->readAcf.aCardToReaderBuffer+pSecurityStackInstance->readAcf.nCardToReaderBufferLength,
      sizeof(pSecurityStackInstance->readAcf.aCardToReaderBuffer)-pSecurityStackInstance->readAcf.nCardToReaderBufferLength,
      (W_HANDLE*)null
   );
}

/**
 * @brief Reads next available data from the EF(SE-ACF) file of the PKCS#15 application.
 *
 * This command sends a READ BINARY command to the PKCS#15 application.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
 */
static void static_PSecurityStackReadAcf
(
   tContext* pContext,
   tSecurityStackInstance* pSecurityStackInstance
)
{
   uint8_t aReaderToCardBuffer[5];
   aReaderToCardBuffer[0] = P_SECSTACK_CLA_ISO7816;
   aReaderToCardBuffer[1] = P_SECSTACK_INS_READ_BINARY;
   aReaderToCardBuffer[2] = (uint8_t)(pSecurityStackInstance->readAcf.nSEACFReadLength >> 8);
   aReaderToCardBuffer[3] = (uint8_t)(pSecurityStackInstance->readAcf.nSEACFReadLength);
   aReaderToCardBuffer[4] = 0x00; /* Le */

   static_PSecurityStackExchangeApdu(pContext, pSecurityStackInstance,
      aReaderToCardBuffer, sizeof(aReaderToCardBuffer)
   );
}

/**
 * @brief Implements the state machine for reading the PKCS#15 application data.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
 *
 * @param[in]  nEvent  An event (P_EVENT_INITIAL or P_EVENT_COMMAND_EXECUTED).
 *
 * @param[in]  nError  An error code.
 *
 */
static void static_PSecurityStackLoadAcfStateMachine
(
   tContext* pContext,
   tSecurityStackInstance* pSecurityStackInstance,
   uint32_t nEvent,
   W_ERROR nError
)
{
   switch ( nEvent )
   {
      case P_EVENT_INITIAL:
         pSecurityStackInstance->readAcf.nError = W_SUCCESS;
         pSecurityStackInstance->readAcf.nState = P_STATE_MANAGE_CHANNEL_OPEN;
         break;

      case P_EVENT_COMMAND_EXECUTED:
         pSecurityStackInstance->readAcf.nState &= ~P_STATUS_PENDING_EXECUTED;
         break;
   }

   if (nError == W_ERROR_TIMEOUT) /* The SE is not responding */
   {
      goto return_error;
   }
   else if (nError != W_SUCCESS)
   {
      /* Close the logical channel (if need be) */
      switch(pSecurityStackInstance->readAcf.nState)
      {
         case P_STATE_READ_EF_SE_ACF_FIRST:
         case P_STATE_READ_EF_SE_ACF_NEXT:
            pSecurityStackInstance->readAcf.nError = nError;
            pSecurityStackInstance->readAcf.nState = P_STATE_MANAGE_CHANNEL_CLOSE;
            break;
      }
   }

   switch(pSecurityStackInstance->readAcf.nState)
   {
      case P_STATE_MANAGE_CHANNEL_OPEN:
      {
         /* Send a MANAGE CHANNEL [open] command */
         uint8_t aReaderToCardBuffer[5];
         aReaderToCardBuffer[0] = P_SECSTACK_CLA_ISO7816;
         aReaderToCardBuffer[1] = P_SECSTACK_INS_MANAGE_CHANNEL;
         aReaderToCardBuffer[2] = P_SECSTACK_P1_MANAGE_CHANNEL_OPEN;
         aReaderToCardBuffer[3] = 0x00;
         aReaderToCardBuffer[4] = 0x01;  /* Le */

         /* The MANAGE CHANNEL command is sent on the basic logical channel */
         pSecurityStackInstance->readAcf.nChannelId = 0;

         static_PSecurityStackExchangeApdu(pContext, pSecurityStackInstance,
            aReaderToCardBuffer, sizeof(aReaderToCardBuffer)
          );

         pSecurityStackInstance->readAcf.nState = P_STATE_SELECT_PKCS15_APPLICATION | P_STATUS_PENDING_EXECUTED;
         return;
      }

      case P_STATE_SELECT_PKCS15_APPLICATION:
      {
         uint8_t aReaderToCardBuffer[5+sizeof(P_SECSTACK_PKCS15_AID)];

         /* Check answer to the MANAGE CHANNEL [open] command */
         if (pSecurityStackInstance->readAcf.nCardToReaderBufferLength != 3)
         {
            nError = W_ERROR_RF_COMMUNICATION;
            goto return_error;
         }

         aReaderToCardBuffer[0] = P_SECSTACK_CLA_ISO7816;
         aReaderToCardBuffer[1] = P_SECSTACK_INS_SELECT;
         aReaderToCardBuffer[2] = P_SECSTACK_P1_SELECT_AID;
         aReaderToCardBuffer[3] = P_SECSTACK_P2_SELECT_AID;
         aReaderToCardBuffer[4] = sizeof(P_SECSTACK_PKCS15_AID);
         CMemoryCopy(aReaderToCardBuffer+5, P_SECSTACK_PKCS15_AID, sizeof(P_SECSTACK_PKCS15_AID));

         /* Extract the channel ID from the R-DATA field */
         pSecurityStackInstance->readAcf.nChannelId = pSecurityStackInstance->readAcf.aCardToReaderBuffer[0];

         /* Send the SELECT[AID] command */
         static_PSecurityStackExchangeApdu(pContext, pSecurityStackInstance,
            aReaderToCardBuffer, sizeof(aReaderToCardBuffer)
          );

         pSecurityStackInstance->readAcf.nState = P_STATE_READ_EF_SE_ACF_FIRST | P_STATUS_PENDING_EXECUTED;
         return;
      }

      case P_STATE_READ_EF_SE_ACF_FIRST:
      {
         /* Check answer to the SELECT[AID] command */
         if (pSecurityStackInstance->readAcf.nCardToReaderBufferLength < 2)
         {
            nError = W_ERROR_RF_COMMUNICATION;
            goto return_error;
         }

         /* Is the PKCS#15 application present in the Secure element? */
         if ( (pSecurityStackInstance->readAcf.nCardToReaderBufferLength == 2) &&
              (pSecurityStackInstance->readAcf.aCardToReaderBuffer[0] == 0x6A) &&
              (pSecurityStackInstance->readAcf.aCardToReaderBuffer[1] == 0x82) ) /* File no found */
         {
            /* The PKCS#15 application is not present in the SE */

            if (pSecurityStackInstance->bIsACFPresent != false)
            {
               /* The PKCS#15 application was previously present, but is no longer present */
               pSecurityStackInstance->readAcf.nError = W_ERROR_SECURITY;
            }

			pSecurityStackInstance->readAcf.bTouched = false;
            pSecurityStackInstance->bIsACFPresent = false;
            goto close_logical_channel;
         }


         /* The PKCS#15 application is present! */
         pSecurityStackInstance->bIsACFPresent = true;

         /* Parse the answer to the SELECT[AID] command */
         if ((nError = static_PSecurityStackParseAnswerToSelect(pSecurityStackInstance)) != W_SUCCESS)
         {
            /* Setting this error code will delete the current ACL */
            pSecurityStackInstance->readAcf.nError = W_ERROR_SECURITY;
            goto close_logical_channel;
         }

         /* Check whether the LastUpdate date&time changed since the last read */
         if ((pSecurityStackInstance->pACL != null) &&
            (CMemoryCompare(pSecurityStackInstance->aACLTime, pSecurityStackInstance->readAcf.aSELastUpdate, P_SECSTACK_LASTUPDATE_LENGTH) == 0))
         {
            /* The timestamp has not changed - No need to read the ACL again */
            PDebugLog("PSecurityStackLoadAcf: The ACL has not changed. No need to read it again.");

            goto close_logical_channel;
         }

         /* No data have been read yet */
         pSecurityStackInstance->readAcf.nSEACFReadLength = 0;

         /* Read next data */
         static_PSecurityStackReadAcf(pContext, pSecurityStackInstance);

         pSecurityStackInstance->readAcf.nState = P_STATE_READ_EF_SE_ACF_NEXT | P_STATUS_PENDING_EXECUTED;
         return;
      }

      case P_STATE_READ_EF_SE_ACF_NEXT:
      {
         if (pSecurityStackInstance->readAcf.nCardToReaderBufferLength < 2)
         {
            nError = W_ERROR_RF_COMMUNICATION;
            goto return_error;
         }

         /* Copy returned bytes */
         CMemoryCopy(pSecurityStackInstance->readAcf.pSEACF+pSecurityStackInstance->readAcf.nSEACFReadLength,
            pSecurityStackInstance->readAcf.aCardToReaderBuffer,
            pSecurityStackInstance->readAcf.nCardToReaderBufferLength - 2
         );
         pSecurityStackInstance->readAcf.nSEACFReadLength += pSecurityStackInstance->readAcf.nCardToReaderBufferLength - 2;

         /* Check whether all bytes have been read */
         if (pSecurityStackInstance->readAcf.nSEACFReadLength < pSecurityStackInstance->readAcf.nSEACFLength)
         {
            /* Read next data */
            static_PSecurityStackReadAcf(pContext, pSecurityStackInstance);

            pSecurityStackInstance->readAcf.nState = P_STATE_READ_EF_SE_ACF_NEXT | P_STATUS_PENDING_EXECUTED;
            return;
         }
         else
         {
            tSecurityStackACL* pACL = (tSecurityStackACL*)null;

            /* ACL data have been read */
            pSecurityStackInstance->readAcf.bTouched = false;

            if ((nError = PSecurityStackParseACL(pSecurityStackInstance->readAcf.pSEACF, pSecurityStackInstance->readAcf.nSEACFLength, &pACL)) != W_SUCCESS)
            {
               pSecurityStackInstance->readAcf.nError = W_ERROR_SECURITY;
            }
            else
            {
                tSecurityStackACL* pOldACL = pSecurityStackInstance->pACL;

                /* Install the new ACL */
                pSecurityStackInstance->pACL = pACL;

                /* Remember the date&time of this ACL */
                CMemoryCopy(pSecurityStackInstance->aACLTime, pSecurityStackInstance->readAcf.aSELastUpdate, P_SECSTACK_LASTUPDATE_LENGTH);

                /* Free old ACL */
                if (pOldACL != null)
                {
                   PSecurityStackFreeACL(pOldACL);
                   pOldACL = (tSecurityStackACL*)null;
                }
            }

         }

         /* FALL THROUGH */
      }

close_logical_channel:

      case P_STATE_MANAGE_CHANNEL_CLOSE:
      {
         /* Send a MANAGE CHANNEL [close] command */
         uint8_t aReaderToCardBuffer[4];
         aReaderToCardBuffer[0] = P_SECSTACK_CLA_ISO7816;
         aReaderToCardBuffer[1] = P_SECSTACK_INS_MANAGE_CHANNEL;
         aReaderToCardBuffer[2] = P_SECSTACK_P1_MANAGE_CHANNEL_CLOSE;
         aReaderToCardBuffer[3] = (uint8_t)pSecurityStackInstance->readAcf.nChannelId;

         static_PSecurityStackExchangeApdu(pContext, pSecurityStackInstance,
            aReaderToCardBuffer, sizeof(aReaderToCardBuffer)
          );

         pSecurityStackInstance->readAcf.nState = P_STATE_FINAL | P_STATUS_PENDING_EXECUTED;
         return;
      }

      case P_STATE_FINAL:
      {
         /* Check answer to the MANAGE CHANNEL[close] command - Errors are ignored though */
         if (pSecurityStackInstance->readAcf.nCardToReaderBufferLength == 2)
         {
            uint16_t nSW = (uint16_t)((256 * pSecurityStackInstance->readAcf.aCardToReaderBuffer[0]) + pSecurityStackInstance->readAcf.aCardToReaderBuffer[1]);

            if (nSW != 0x9000)
            {
               PDebugWarning("PSecurityStackLoadAcf: MANAGE CHANNEL[close] returned 0x%04X instead of 0x9000", nSW);
            }
         }

         nError = pSecurityStackInstance->readAcf.nError;
         goto return_error;
      }
   }

   nError = W_ERROR_SECURITY;
   /* FALL THROUGH */

return_error:

   /* Free working memory */
   if (pSecurityStackInstance->readAcf.pSEACF != null)
   {
      CMemoryFree(pSecurityStackInstance->readAcf.pSEACF);
      pSecurityStackInstance->readAcf.pSEACF = (uint8_t*)null;

      pSecurityStackInstance->readAcf.nSEACFLength = 0;
      pSecurityStackInstance->readAcf.nSEACFReadLength = 0;
   }

   /* Free current ACL in case of error */
   if ((nError != W_SUCCESS) && (pSecurityStackInstance->pACL != null))
   {
      PSecurityStackFreeACL(pSecurityStackInstance->pACL);
      pSecurityStackInstance->pACL = (tSecurityStackACL*)null;
   }

   pSecurityStackInstance->readAcf.nState = P_STATE_IDLE;
   PDFCPostContext2(&pSecurityStackInstance->readAcf.sCallbackContext, P_DFC_TYPE_SECURITY, nError);
}

/* See header file */
void PSecurityStackLoadAcf(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            tPBasicExchangeData* pExchangeApduFunction,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter )
{
   tDFCCallbackContext sCallbackContext;
   uint32_t nError = W_SUCCESS;

   PDebugTrace("PSecurityStackLoadAcf()");

   if ((pSecurityStackInstance == null))
   {
	   PDebugError("PSecurityStackLoadAcf: Invalid parameter");

      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if (pSecurityStackInstance->readAcf.nState != P_STATE_IDLE)
   {
	   PDebugError("PSecurityStackLoadAcf: An ACF loading is already ongoing");

      nError = W_ERROR_BAD_STATE;
      goto return_error;
   }

   pSecurityStackInstance->pExchangeApduFunction = pExchangeApduFunction;
   pSecurityStackInstance->hConnection = hConnection;

   PDFCFillCallbackContext(pContext, (tDFCCallback*)pCallback, pCallbackParameter,
      &pSecurityStackInstance->readAcf.sCallbackContext
   );

   /* Start reading the ACL */
   static_PSecurityStackLoadAcfStateMachine(pContext, pSecurityStackInstance, P_EVENT_INITIAL, W_SUCCESS);
   return;

return_error:

   PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &sCallbackContext);
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_SECURITY, nError);
}

/* See header file */
void PSecurityStackNotifyEndofConnection(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            tPBasicExchangeData* pExchangeApduFunction,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter )
{
   tDFCCallbackContext sCallbackContext;
   uint32_t nError = W_SUCCESS;

   PDebugTrace("PSecurityStackNotifyEndofConnection()");

   if (pSecurityStackInstance == null)
   {
	   PDebugError("PSecurityStackNotifyEndofConnection: Invalid parameter");

      nError = W_ERROR_BAD_PARAMETER;
      goto return_result;
   }

   if (!pSecurityStackInstance->readAcf.bTouched)
   {
      goto return_result;
   }

   /* Re-read the ACL */
   PSecurityStackLoadAcf(pContext, pSecurityStackInstance, pExchangeApduFunction, hConnection, pCallback, pCallbackParameter);
   return;

return_result:

   PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &sCallbackContext);
   PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_SECURITY, nError);
}

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */
