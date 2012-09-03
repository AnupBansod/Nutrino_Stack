/*
 * Copyright (c) 2007-2011 Inside Secure, All Rights Reserved.
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

#define P_MODULE  P_MODULE_DEC( PROTO )

#include "nfc_hal_binding.h"

/* Reader Mode */
#define P_NFC_HAL_BINDING_READER_CONFIG_SIZE_ISO_14443_4_A        4
#define P_NFC_HAL_BINDING_READER_CONFIG_MIN_SIZE_ISO_14443_3_B    5
#define P_NFC_HAL_BINDING_READER_CONFIG_MAX_SIZE_ISO_14443_3_B    250
#define P_NFC_HAL_BINDING_READER_CONFIG_SIZE_ISO_15_3             1
#define P_NFC_HAL_BINDING_READER_CONFIG_SIZE_FELICA               2

/* Card Mode */
#define P_NFC_HAL_BINDING_CARD_CONFIG_MIN_SIZE_ISO_14443_4_A      16
#define P_NFC_HAL_BINDING_CARD_CONFIG_MAX_SIZE_ISO_14443_4_A      (16 + 252)
#define P_NFC_HAL_BINDING_CARD_CONFIG_MIN_SIZE_ISO_14443_4_B      11
#define P_NFC_HAL_BINDING_CARD_CONFIG_MAX_SIZE_ISO_14443_4_B      (11 + 253)

#define P_NFC_HAL_BINDING_CARD_CONFIG_UID_SIZE                    0x0A
#define P_NFC_HAL_BINDING_CARD_CONFIG_ATQB_SIZE                   0x0B

/* P2P Mode */
#define P_NFC_HAL_BINDING_P2P_TARGET_RTX_CONFIG_PARAMETERS_SIZE   0x02

#define P_NFC_HAL_BINDING_READER_CONFIG_MIN_SIZE_B_PRIME          3
#define P_NFC_HAL_BINDING_READER_CONFIG_MAX_SIZE_B_PRIME          4

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_POLICY(
               const uint8_t* pBuffer,
               uint32_t nLength,
               bool* pbIsReaderLocked,
               bool* pbIsCardLocked,
               uint16_t* pnUICCCardPolicy,
               uint16_t* pnUICCReaderPolicy,
               uint32_t* pnSESwitchPosition,
               uint32_t* pnSESlotIdentifier,
               uint16_t* pnSECardPolicy,
               uint32_t* pnBattOff)
{
   uint16_t nFlags = 0;

   if (nLength != NAL_POLICY_SIZE)
   {
      return NAL_RES_BAD_LENGTH;;
   }

   if((pbIsReaderLocked != null)
   || (pbIsCardLocked != null)
   || (pnSESwitchPosition != null)
   || (pnSESlotIdentifier != null))
   {
      nFlags = static_PNALBindingReadUint16FromNALBuffer(&pBuffer[4]);
   }

   if(pbIsReaderLocked != null)
   {
      *pbIsReaderLocked = ((nFlags & NAL_POLICY_FLAG_READER_LOCK) == 0)?true:false;
   }

   if(pbIsCardLocked != null)
   {
      *pbIsCardLocked = ((nFlags & NAL_POLICY_FLAG_CARD_LOCK) == 0)?true:false;
   }

   if(pnUICCCardPolicy != null)
   {
      *pnUICCCardPolicy = static_PNALBindingReadUint16FromNALBuffer(&pBuffer[0]);
   }

   if(pnUICCReaderPolicy != null)
   {
      *pnUICCReaderPolicy = static_PNALBindingReadUint16FromNALBuffer(&pBuffer[2]);
   }

   if(pnSESwitchPosition != null)
   {
      *pnSESwitchPosition = nFlags & NAL_POLICY_FLAG_SE_MASK;
   }

   if(pnSESlotIdentifier != null)
   {
      *pnSESlotIdentifier = (nFlags & NAL_POLICY_FLAG_SE_ID_MASK) >> 4;
   }

   if (pnBattOff != null)
   {
      * pnBattOff = (nFlags & NAL_POLICY_FLAG_BATT_OFF_MASK);
   }

   if(pnSECardPolicy != null)
   {
      uint16_t nSECardPolicy;

      nSECardPolicy = static_PNALBindingReadUint16FromNALBuffer(&pBuffer[6]);

      switch (nSECardPolicy)
      {
         /* only the following combination are allowed */
         case 0:
         case NAL_PROTOCOL_CARD_ISO_14443_4_A:
         case NAL_PROTOCOL_CARD_ISO_14443_4_A | NAL_PROTOCOL_CARD_MIFARE_CLASSIC:
         case NAL_PROTOCOL_CARD_ISO_14443_4_A | NAL_PROTOCOL_CARD_ISO_15693_2:
         case NAL_PROTOCOL_CARD_ISO_14443_4_A | NAL_PROTOCOL_CARD_MIFARE_CLASSIC | NAL_PROTOCOL_CARD_ISO_15693_2:
         case NAL_PROTOCOL_CARD_ISO_14443_4_B:
         case NAL_PROTOCOL_CARD_ISO_14443_4_B | NAL_PROTOCOL_CARD_ISO_15693_2:
         case NAL_PROTOCOL_CARD_ISO_15693_2:
            break;

         default:
            /* invalid policy */
            return NAL_RES_FEATURE_NOT_SUPPORTED;
      }

      *pnSECardPolicy = nSECardPolicy;
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_POLICY(
               uint8_t* pBuffer,
               bool bIsReaderLocked,
               bool bIsCardLocked,
               uint16_t nUICCCardPolicy,
               uint16_t nUICCReaderPolicy,
               uint32_t nSESwitchPosition,
               uint32_t nSESlotIdentifier,
               uint16_t nSECardPolicy)
{
   uint16_t nFlags = 0;
   uint32_t nIndex = 0;

   static_PNALBindingWriteUint16ToNALBuffer(nUICCCardPolicy, pBuffer + nIndex);
   nIndex += 2;

   static_PNALBindingWriteUint16ToNALBuffer(nUICCReaderPolicy, pBuffer + nIndex);
   nIndex += 2;

   if (bIsReaderLocked == false)
   {
      nFlags |= NAL_POLICY_FLAG_READER_LOCK;
   }

   if (bIsCardLocked == false)
   {
      nFlags |= NAL_POLICY_FLAG_CARD_LOCK;
   }

   CNALDebugAssert((nSESwitchPosition | NAL_POLICY_FLAG_SE_MASK) == NAL_POLICY_FLAG_SE_MASK);
   nFlags |= nSESwitchPosition;

   CNALDebugAssert((nSESlotIdentifier | NAL_POLICY_FLAG_SE_ID_MASK) == NAL_POLICY_FLAG_SE_ID_MASK);
   nFlags |= nSESlotIdentifier;

   static_PNALBindingWriteUint16ToNALBuffer(nFlags, pBuffer + nIndex);
   nIndex += 2;

   static_PNALBindingWriteUint16ToNALBuffer(nSECardPolicy, pBuffer + nIndex);
   nIndex += 2;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_HARDWARE_INFO(
               uint8_t* pBuffer,
               const tNALParHardwareInfo *pNALParHardwareInfo)
{
   uint32_t nIndex = 0;
   uint32_t nBytePos, nTempPos;

   pBuffer[nIndex++] = NAL_VERSION;

   if(pNALParHardwareInfo->nSEType == P_NFC_HAL_SE_NONE)
   {
      CNALMemoryCopy(&pBuffer[nIndex], "Microread v", 11);
      PNALUtilWriteVersion(&pBuffer[nIndex + 11], pNALParHardwareInfo->aHardwareVersion);
   }
   else
   {
      CNALMemoryCopy(&pBuffer[nIndex], "Securead v", 10);
      PNALUtilWriteVersion(&pBuffer[nIndex + 10], pNALParHardwareInfo->aHardwareVersion);
   }

   nIndex += NAL_HARDWARE_TYPE_STRING_SIZE;

   nTempPos = nIndex;
   for(nBytePos = 0; nBytePos < P_NFC_HAL_HARDWARE_SERIAL_NUMBER_SIZE; nBytePos++)
   {
      nTempPos += PNALUtilWriteHexaUint8(
         &pBuffer[nTempPos], pNALParHardwareInfo->aHardwareSerialNumber[nBytePos]);
   }
   nIndex += NAL_HARDWARE_SERIAL_NUMBER_STRING_SIZE;

   CNALMemoryCopy(&pBuffer[nIndex], "Loader v", 8);
   PNALUtilWriteVersion(&pBuffer[nIndex + 8], pNALParHardwareInfo->aLoaderVersion);
   nIndex += NAL_LOADER_DESCRIPTION_STRING_SIZE;

   pBuffer[nIndex++] = pNALParHardwareInfo->nFirmwareStatus;

   CNALMemoryFill(pBuffer + nIndex, 0, 4 * (NAL_SE_DESCRIPTION_STRING_SIZE + 2 + 2 + 2));

   if(pNALParHardwareInfo->nSEType != P_NFC_HAL_SE_NONE)
   {
      uint16_t value;

      pBuffer[nIndex++] = 1; /* one SE */

      /* Hardcoding of the Jupiter features */
      CNALMemoryCopy(&pBuffer[nIndex], "SLE97-144SE", 11);
      nIndex += NAL_SE_DESCRIPTION_STRING_SIZE;

      /* Capabilities */
      value = NAL_SE_FLAG_END_OF_TRANSACTION_NOTIFICATION;
      static_PNALBindingWriteUint16ToNALBuffer(value, &pBuffer[nIndex]);
      nIndex += 2;

      /* Host Interface */
      value = NAL_PROTOCOL_READER_ISO_14443_4_B;
      static_PNALBindingWriteUint16ToNALBuffer(value, &pBuffer[nIndex]);
      nIndex += 2;

      /* RF Interface */
      value = NAL_PROTOCOL_CARD_ISO_14443_4_A | NAL_PROTOCOL_CARD_ISO_14443_4_B | NAL_PROTOCOL_CARD_ISO_15693_2 | NAL_PROTOCOL_CARD_MIFARE_CLASSIC;
      static_PNALBindingWriteUint16ToNALBuffer(value, &pBuffer[nIndex]);
      nIndex += 2;
   }

   return 0xFB;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_FIRMWARE_INFO(
               uint8_t* pBuffer,
               const tNALParFirmwareInfo *pNALParFirmwareInfo,
               uint16_t nAutoStandbyTimeout)
{
   uint32_t nIndex = 0;
   uint32_t nPos = 0;
   uint16_t nVersion;

   /* Build the NFC HAL payload */
   pBuffer[nIndex++] = NAL_VERSION;

   CNALMemoryCopy(&pBuffer[nIndex], "Firmware v", 10);
   nPos += 10;

   nPos += PNALUtilWriteVersion(&pBuffer[nIndex + nPos], pNALParFirmwareInfo->aFirmwareVersion);
   CNALMemoryCopy(&pBuffer[nIndex + nPos], " (", 2);
   nPos += 2;

   /* ROM family */
   switch (pNALParFirmwareInfo->aFirmwareTrackingId[0])
   {
      case 5 :
         CNALMemoryCopy(&pBuffer[nIndex + nPos], "3.0 ", 4);
         break;

      case 6:
         CNALMemoryCopy(&pBuffer[nIndex + nPos], "3.2 ", 4);
         break;

      case 7:
         CNALMemoryCopy(&pBuffer[nIndex + nPos], "3.4 ", 4);
         break;

      default:
         CNALMemoryCopy(&pBuffer[nIndex + nPos], "??? ", 4);
         break;
   }

   nPos += 4;

   /* Destination */
   CNALMemoryCopy(&pBuffer[nIndex + nPos], & pNALParFirmwareInfo->aFirmwareTrackingId[1], 2);
   nPos += 2;
   pBuffer[nIndex + nPos++] = ' ';

   /* Delivery */
   nPos += PNALUtilWriteHexaUint8(&pBuffer[nIndex + nPos], pNALParFirmwareInfo->aFirmwareTrackingId[3]);
   pBuffer[nIndex + nPos++] = ' ';

   /* Trunk */
   nVersion = static_PNALBindingReadUint16FromHCIBuffer(& pNALParFirmwareInfo->aFirmwareTrackingId[4]);
   nPos += PNALUtilWriteDecimalUint32(&pBuffer[nIndex + nPos], nVersion);

   pBuffer[nIndex + nPos++] = ')';
   pBuffer[nIndex + nPos++ ] = 0;
   nIndex += NAL_FIRMWARE_DESCRIPTION_STRING_SIZE;

   CNALMemoryCopy(
     &pBuffer[nIndex],
     pNALParFirmwareInfo->aCardProtocolCapabilities,
     P_NFC_HAL_BINDING_PROTOCOL_CAPABILITIES_SIZE);
   nIndex += P_NFC_HAL_BINDING_PROTOCOL_CAPABILITIES_SIZE;

   CNALMemoryCopy(
     &pBuffer[nIndex],
     pNALParFirmwareInfo->aReaderProtocolCapabilities,
     P_NFC_HAL_BINDING_PROTOCOL_CAPABILITIES_SIZE);
   nIndex += P_NFC_HAL_BINDING_PROTOCOL_CAPABILITIES_SIZE;

   CNALMemoryCopy(
     &pBuffer[nIndex],
     pNALParFirmwareInfo->aFirmwareCapabilities,
     P_NFC_HAL_BINDING_FIRMWARE_CAPABILITIES_SIZE);
   nIndex += P_NFC_HAL_BINDING_FIRMWARE_CAPABILITIES_SIZE;

   pBuffer[nIndex++] = pNALParFirmwareInfo->nReaderISOADataRateMax;
   pBuffer[nIndex++] = pNALParFirmwareInfo->nReaderISOAInputBufferSize;
   pBuffer[nIndex++] = pNALParFirmwareInfo->nReaderISOBDataRateMax;
   pBuffer[nIndex++] = pNALParFirmwareInfo->nReaderISOBInputBufferSize;
   pBuffer[nIndex++] = pNALParFirmwareInfo->nCardISOADataRateMax;
   pBuffer[nIndex++] = pNALParFirmwareInfo->nCardISOBDataRateMax;

   static_PNALBindingWriteUint16ToNALBuffer( nAutoStandbyTimeout,
                                             &pBuffer[nIndex]);
   nIndex += 2;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_DETECT_PULSE(
               const uint8_t* pBuffer,
               uint32_t nLength,
               uint16_t* pnCardDetectPulse)
{
   if (nLength != NAL_PAR_DETECT_PULSE_SIZE)
   {
      return NAL_RES_BAD_LENGTH;
   }

   * pnCardDetectPulse = static_PNALBindingReadUint16FromNALBuffer(pBuffer);
   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_PERSISTENT_MEMORY(
                        const uint8_t  *pBuffer,
                        uint32_t nLength,
                        uint8_t **ppPersistentStorage)
{
   if (nLength != NAL_PERSISTENT_MEMORY_SIZE)
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      * ppPersistentStorage = (uint8_t *) pBuffer;
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_PERSISTENT_MEMORY(
                        uint8_t  *pBuffer,
                        const uint8_t aPersistentStorage[NAL_PERSISTENT_MEMORY_SIZE])

{
   if (aPersistentStorage != null)
   {
      CNALMemoryCopy(pBuffer, aPersistentStorage, NAL_PERSISTENT_MEMORY_SIZE);
   }

   return NAL_PERSISTENT_MEMORY_SIZE;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_READER_CONFIG_14_A_4(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t* pnMaxDataRate,
                        bool* pbIsCID,
                        uint8_t* pnCID,
                        uint8_t* pnFSD)
{
   if (nLength != P_NFC_HAL_BINDING_READER_CONFIG_SIZE_ISO_14443_4_A)
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      uint32_t nIndex = 0;

      if(pnMaxDataRate != null)
      {
         *pnMaxDataRate = pBuffer[nIndex];
      }
      nIndex++;

      if(pbIsCID != null)
      {
         *pbIsCID = (pBuffer[nIndex] == 1)?true:false;
      }
      nIndex++;

      if(pnCID != null)
      {
         *pnCID = pBuffer[nIndex];
      }
      nIndex++;

      if(pnFSD != null)
      {
         *pnFSD = pBuffer[nIndex];
      }
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_READER_CONFIG_14_B_3(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t* pnMaxDataRate,
                        uint8_t* pnAFI,
                        bool* pbIsCID,
                        uint8_t* pnCID,
                        uint8_t* pnFSD,
                        uint8_t** pHigherLayerData,
                        uint32_t* nHigherLayerDataLength)
{
   if ((nLength < P_NFC_HAL_BINDING_READER_CONFIG_MIN_SIZE_ISO_14443_3_B) ||
       (nLength > P_NFC_HAL_BINDING_READER_CONFIG_MAX_SIZE_ISO_14443_3_B))
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      uint32_t nIndex = 0;

      if(pnMaxDataRate != null)
      {
         *pnMaxDataRate = pBuffer[nIndex];
      }
      nIndex++;

      if(pnAFI != null)
      {
         *pnAFI = pBuffer[nIndex];
      }
      nIndex++;

      if(pbIsCID != null)
      {
         *pbIsCID = (pBuffer[nIndex] == 1)?true:false;
      }
      nIndex++;

      if(pnCID != null)
      {
         *pnCID = pBuffer[nIndex];
      }
      nIndex++;

      if(pnFSD != null)
      {
         *pnFSD = pBuffer[nIndex];
      }
      nIndex++;

      if(pHigherLayerData != null)
      {
         *pHigherLayerData = (uint8_t *) &pBuffer[nIndex];
         *nHigherLayerDataLength = (nLength - P_NFC_HAL_BINDING_READER_CONFIG_MIN_SIZE_ISO_14443_3_B);
      }
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_READER_CONFIG_15_3(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t* pnAFI)
{
   if (nLength < P_NFC_HAL_BINDING_READER_CONFIG_SIZE_ISO_15_3)
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      if(pnAFI != null)
      {
         *pnAFI = pBuffer[0];
      }
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_READER_CONFIG_FELICA(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint16_t* pnSystemCode)
{
   if (nLength != P_NFC_HAL_BINDING_READER_CONFIG_SIZE_FELICA)
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      if(pnSystemCode != null)
      {
         * pnSystemCode = static_PNALBindingReadUint16FromNALBuffer(pBuffer);
      }
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_READER_CONFIG_B_PRIME(
                        const uint8_t* pBuffer,
                        uint32_t nLength)
{
   if( (nLength >= P_NFC_HAL_BINDING_READER_CONFIG_MIN_SIZE_B_PRIME)
    && (nLength <= P_NFC_HAL_BINDING_READER_CONFIG_MAX_SIZE_B_PRIME))
   {
      return NAL_RES_OK;
   }

   return NAL_RES_BAD_LENGTH;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_CARD_CONFIG_14_A_4(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t* pnUID,
                        uint8_t* pnUIDLen,
                        uint8_t* pnATQAMsb,
                        uint8_t* pnTO,
                        uint8_t* pnTA,
                        uint8_t* pnTB,
                        uint8_t* pnTC,
                        uint8_t** pnApplicationData,
                        uint32_t* pnApplicationDataLength)
{
   if ((nLength < P_NFC_HAL_BINDING_CARD_CONFIG_MIN_SIZE_ISO_14443_4_A) ||
       (nLength > P_NFC_HAL_BINDING_CARD_CONFIG_MAX_SIZE_ISO_14443_4_A))
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      uint32_t nIndex = 0;
      if(pnUID != null)
      {
         CNALMemoryCopy(pnUID, pBuffer, P_NFC_HAL_BINDING_CARD_CONFIG_UID_SIZE);
      }
      nIndex += P_NFC_HAL_BINDING_CARD_CONFIG_UID_SIZE;

      if(pnUIDLen != null)
      {
         *pnUIDLen = pBuffer[nIndex];
      }
      nIndex++;

      if(pnATQAMsb != null)
      {
         *pnATQAMsb = pBuffer[nIndex];
      }
      nIndex++;

      if(pnTO != null)
      {
         *pnTO = pBuffer[nIndex];
      }
      nIndex++;

      if(pnTA != null)
      {
         *pnTA = pBuffer[nIndex];
      }
      nIndex++;

      if(pnTB != null)
      {
         *pnTB = pBuffer[nIndex];
      }
      nIndex++;

      if(pnTC != null)
      {
         *pnTC = pBuffer[nIndex];
      }
      nIndex++;

      if(pnApplicationData != null)
      {
         *pnApplicationData = (uint8_t *) &pBuffer[nIndex];
         *pnApplicationDataLength = (nLength - P_NFC_HAL_BINDING_CARD_CONFIG_MIN_SIZE_ISO_14443_4_A);
      }
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_CARD_CONFIG_14_B_4(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t* pATQB,
                        uint8_t** ppHigherLayerResponseData,
                        uint32_t* pnHigherLayerResponseDataLength)
{
   if ((nLength < P_NFC_HAL_BINDING_CARD_CONFIG_MIN_SIZE_ISO_14443_4_B) ||
       (nLength > P_NFC_HAL_BINDING_CARD_CONFIG_MAX_SIZE_ISO_14443_4_B))
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      uint32_t nIndex = 0;

      if(pATQB != null)
      {
         CNALMemoryCopy(pATQB, pBuffer, P_NFC_HAL_BINDING_CARD_CONFIG_ATQB_SIZE);
      }
      nIndex += P_NFC_HAL_BINDING_CARD_CONFIG_ATQB_SIZE;

      if(ppHigherLayerResponseData != null)
      {
         *ppHigherLayerResponseData = (uint8_t *) &pBuffer[nIndex];
         *pnHigherLayerResponseDataLength = (nLength - P_NFC_HAL_BINDING_CARD_CONFIG_ATQB_SIZE);
      }
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_P2P_INITIATOR_LINK_PARAMETERS(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t** pParameters,
                        uint32_t * pnParametersLength)

{
   * pParameters = (uint8_t *) pBuffer;
   * pnParametersLength = nLength;

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_P2P_TARGET_PARAMETERS(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t** pParameters,
                        uint32_t * pnParametersLength)
{
   * pParameters = (uint8_t *) pBuffer;
   * pnParametersLength = nLength;

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_PAR_P2P_TARGET_RTX(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        uint8_t* pParameters)
{
   if (nLength != P_NFC_HAL_BINDING_P2P_TARGET_RTX_CONFIG_PARAMETERS_SIZE)
   {
      return NAL_RES_BAD_LENGTH;
   }
   else
   {
      pParameters = (uint8_t *) pBuffer;
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_UICC_SWP(
                        uint8_t* pBuffer,
                        uint32_t nStatus)
{
   pBuffer[0] = (uint8_t) nStatus;

   return 1;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_UICC_READER_PROTOCOLS(
                        uint8_t* pBuffer,
                        bool bDetect,
                        uint16_t nOpenProtocols,
                        uint8_t nDataRateMax,
                        uint8_t nAFI,
                        const uint8_t* pnHigherLayerData,
                        uint32_t nHigherLayerDataLength)
{
   uint32_t nIndex = 0;

   pBuffer[nIndex++] = (bDetect==true)?1:0;

   static_PNALBindingWriteUint16ToNALBuffer(nOpenProtocols, &pBuffer[nIndex]);
   nIndex += 2;

   pBuffer[nIndex++] = nDataRateMax;
   pBuffer[nIndex++] = nAFI;

   if(pnHigherLayerData != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pnHigherLayerData, nHigherLayerDataLength);
      nIndex += nHigherLayerDataLength;
   }

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_UICC_CARD_PROTOCOLS(
                        uint8_t* pBuffer,
                        uint16_t nOpenProtocols,
                        uint8_t nCardA4Mode,
                        const uint8_t* pnUID,
                        uint8_t nUIDLength,
                        uint8_t nSAK,
                        uint16_t nATQA,
                        const uint8_t* pnApplicationData,
                        uint8_t nFWI_SFGI,
                        uint8_t nCIDSupport,
                        uint8_t nDataRateMax,
                        uint8_t nCardB4Mode,
                        uint32_t nPUPI,
                        uint8_t nAFI,
                        uint32_t nATQB,
                        const uint8_t* pnHigherLayerResponse)
{
   uint32_t nIndex = 0;

   static_PNALBindingWriteUint16ToNALBuffer(nOpenProtocols, &pBuffer[nIndex]);
   nIndex += 2;

   pBuffer[nIndex++] = nCardA4Mode;

   if (pnUID != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pnUID, 10);
   }
   nIndex += 10;

   pBuffer[nIndex++] = nUIDLength;
   pBuffer[nIndex++] = nSAK;
   static_PNALBindingWriteUint16ToNALBuffer(nATQA, &pBuffer[nIndex]);
   nIndex += 2;

   if (pnApplicationData != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pnApplicationData, 20);
   }
   nIndex += 20;

   pBuffer[nIndex++] = nFWI_SFGI;
   pBuffer[nIndex++] = nCIDSupport;
   pBuffer[nIndex++] = nDataRateMax;
   pBuffer[nIndex++] = nCardB4Mode;

   static_PNALBindingWriteUint32ToNALBuffer(nPUPI, &pBuffer[nIndex]);
   nIndex += 4;

   pBuffer[nIndex++] = nAFI;

   static_PNALBindingWriteUint32ToNALBuffer(nATQB, &pBuffer[nIndex]);
   nIndex += 4;

   if (pnHigherLayerResponse != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pnHigherLayerResponse, 20);
   }
   nIndex += 20;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_PAR_SE_PRESENT(
                        uint8_t* pBuffer,
                        uint8_t nParameter)
{
   uint32_t nIndex = 0;

   pBuffer[nIndex++] = nParameter;

   return nIndex;
}

/* ************************************************************************** */
/*                             Event Codes                                    */
/* ************************************************************************** */

/* See header file */
NFC_HAL_INTERNAL uint8_t PNALProtocolParse_NAL_EVT_STANDBY_MODE(
                        const uint8_t* pBuffer,
                        uint32_t nLength,
                        bool* pbStandbyFlag)
{
   if (nLength != 1)
   {
      return NAL_RES_BAD_LENGTH;
   }

   if(pbStandbyFlag != null)
   {
      *pbStandbyFlag = pBuffer[0];
   }

   return NAL_RES_OK;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_COLLISION(
                        uint8_t  * pBuffer,
                        const uint16_t aReaderProtocols[],
                        uint32_t   nReaderProtocols)
{
   uint32_t i;
   uint32_t nIndex = 0;

   for (i=0; i<nReaderProtocols; i++)
   {
      static_PNALBindingWriteUint16ToNALBuffer(aReaderProtocols[i], pBuffer + nIndex);
      nIndex += 2;
   }

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_DISCOVERED_14_A_3(
                        uint8_t* pBuffer,
                        uint16_t nATQA,
                        uint8_t nSAK,
                        const uint8_t* pUID,
                        uint32_t nUIDLength)
{
   uint32_t nIndex = 0;

   static_PNALBindingWriteUint16ToNALBuffer(nATQA, &pBuffer[nIndex]);
   nIndex += 2;

   pBuffer[nIndex++] = nSAK;

   if (pUID != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pUID, nUIDLength);
   }
   nIndex += nUIDLength;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_DISCOVERED_14_A_4(
                        uint8_t* pBuffer,
                        uint16_t nATQA,
                        uint8_t  nSAK,
                        const uint8_t* pATS,
                        uint32_t nATSLength,
                        const uint8_t* pApplicationData,
                        uint32_t nApplicationDataLength,
                        const uint8_t* pUID,
                        uint32_t nUIDLength)
{
   uint32_t nIndex = 0;

   static_PNALBindingWriteUint16ToNALBuffer(nATQA, &pBuffer[nIndex]);
   nIndex += 2;

   pBuffer[nIndex++] = nSAK;

   if (pATS != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pATS, nATSLength);
      pBuffer[nIndex] = (uint8_t) nATSLength;
   }
   pBuffer[nIndex] = (uint8_t) (pBuffer[nIndex] + nApplicationDataLength);
   nIndex += nATSLength;

   if (pApplicationData != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pApplicationData, nApplicationDataLength);
   }
   nIndex += nApplicationDataLength;

   if (pUID != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pUID, nUIDLength);
   }
   nIndex += nUIDLength;

   return (nIndex);
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_DISCOVERED_14_B_3(
                        uint8_t * pBuffer,
                        uint8_t  aATQB[P_NFC_HAL_BINDING_CARD_CONFIG_ATQB_SIZE],
                        const uint8_t * pATTRIB,
                        uint32_t nATTRIBLength)
{
   uint32_t nIndex = 0;

   CNALMemoryCopy(&pBuffer[nIndex], aATQB, P_NFC_HAL_BINDING_CARD_CONFIG_ATQB_SIZE);
   nIndex += P_NFC_HAL_BINDING_CARD_CONFIG_ATQB_SIZE;

   if (pATTRIB != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pATTRIB, nATTRIBLength);
   }
   nIndex += nATTRIBLength;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_DISCOVERED_15_3(
                        uint8_t* pBuffer,
                        uint8_t nFlag,
                        uint8_t nDSFID,
                        uint8_t aUID[8])
{
   uint32_t nIndex = 0;

   pBuffer[nIndex++] = nFlag;
   pBuffer[nIndex++] = nDSFID;

   CNALMemoryCopy(&pBuffer[nIndex], aUID, 8);
   nIndex += 8;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_DISCOVERED_TYPE1(
                        uint8_t* pBuffer,
                        const uint8_t aUID[4],
                        const uint8_t aHR[2],
                        uint16_t nATQA)
{
   uint32_t nIndex = 0;

   CNALMemoryCopy(&pBuffer[nIndex], aUID, 4);
   nIndex += 4;

   CNALMemoryCopy(&pBuffer[nIndex], aHR, 2);
   nIndex += 2;

   static_PNALBindingWriteUint16ToNALBuffer(nATQA, &pBuffer[nIndex]);
   nIndex += 2;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_DISCOVERED_FELICA(
                        uint8_t* pBuffer,
                        uint8_t  nIdPm[16],
                        uint16_t nSystemCode)
{
   uint32_t nIndex = 0;

   CNALMemoryCopy(&pBuffer[nIndex], nIdPm, 16);
   nIndex += 16;

   static_PNALBindingWriteUint16ToNALBuffer(nSystemCode, &pBuffer[nIndex]);
   nIndex += 2;

   return nIndex;
}

NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_READER_TARGET_DISCOVERED_BPRIME(
                        uint8_t* pBuffer,
                        uint8_t* pAPGENBuffer,
                        uint32_t nAPGENLength
                        )
{
   CNALMemoryCopy(pBuffer, pAPGENBuffer, nAPGENLength);
   return nAPGENLength;
}

NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_UICC_DETECTION_REQUEST (
                        uint8_t* pBuffer,
                        bool bUICC,
                        uint16_t nReaderProtocolOpened,
                        uint8_t nDataRateMax,
                        uint8_t nAFI,
                        const uint8_t* pnHigherLayerDara,
                        uint32_t nHigherLayerDaraLength)
{
   uint32_t nIndex = 0;

   pBuffer[nIndex++] = (bUICC==true)? 1:0;

   static_PNALBindingWriteUint16ToNALBuffer(nReaderProtocolOpened, &pBuffer[nIndex]);
   nIndex += 2;

   pBuffer[nIndex++] = nDataRateMax;
   pBuffer[nIndex++] = nAFI;

   if (pnHigherLayerDara != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pnHigherLayerDara, nHigherLayerDaraLength);
   }
   nIndex += nHigherLayerDaraLength;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_CARD_SELECTED_14_A_4(
                        uint8_t* pBuffer,
                        uint8_t nDataRate,
                        uint8_t nCID,
                        uint8_t * pUID,
                        uint32_t nUIDLength)
{
   uint32_t nIndex = 0;

   pBuffer[nIndex++] = nDataRate;
   pBuffer[nIndex++] = nCID;

   CNALMemoryCopy(&pBuffer[nIndex], pUID, nUIDLength);
   nIndex += P_NFC_HAL_BINDING_CARD_CONFIG_UID_SIZE;

   pBuffer[nIndex++] = (uint8_t) nUIDLength;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_CARD_SELECTED_14_B_4(
                        uint8_t* pBuffer,
                        uint8_t nAFI,
                        uint8_t nDataRate,
                        uint8_t nCID,
                        uint8_t nFrameSize,
                        uint8_t aPUPI[4],
                        const uint8_t* pnHigherLayerData,
                        uint32_t nHigherLayerDataLength)
{
   uint32_t nIndex = 0;

   pBuffer[nIndex++] = nAFI;
   pBuffer[nIndex++] = nDataRate;
   pBuffer[nIndex++] = nCID;
   pBuffer[nIndex++] = nFrameSize;

   CNALMemoryCopy(&pBuffer[nIndex], aPUPI, 4);
   nIndex += 4;

   if (pnHigherLayerData != null)
   {
      CNALMemoryCopy(&pBuffer[nIndex], pnHigherLayerData, nHigherLayerDataLength);
   }
   nIndex += nHigherLayerDataLength;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocol_NAL_EVT_CARD_SEND_DATA(
                        uint8_t* pBuffer,
                        const uint8_t* pSendData,
                        uint32_t nSendDataLength)
{
   if ((nSendDataLength > 0) && (pSendData != null))
   {
      CNALMemoryCopy(&pBuffer[0], pSendData, nSendDataLength);
   }

   return nSendDataLength;
}

/* See header file */
NFC_HAL_INTERNAL void PNALProtocolFormat_NAL_EVT_CARD_END_OF_TRANSACTION(
                        uint8_t* pBuffer,
                        bool bDeSelection)
{
   pBuffer[0] = (bDeSelection==true)?1:0;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_SE_CARD_EOT(
                        uint8_t* pBuffer,
                        uint16_t nCardProtocolBitField,
                        const uint8_t* pAID,
                        uint32_t nAIDLength)
{
   uint32_t nIndex = 0;

   static_PNALBindingWriteUint16ToNALBuffer(nCardProtocolBitField, &pBuffer[nIndex]);
   nIndex += 2;

   if ((pAID != null) && (nAIDLength > 0))
   {
      CNALMemoryCopy(&pBuffer[nIndex], pAID, nAIDLength);
   }
   nIndex += nAIDLength;

   return nIndex;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_P2P_TARGET_DISCOVERED(
                        uint8_t* pBuffer,
                        const uint8_t* pLinkParameters,
                        uint32_t nLinkLength)
{
   if ((pLinkParameters != null) && (nLinkLength > 0))
   {
      CNALMemoryCopy(&pBuffer[0], pLinkParameters, nLinkLength);
   }

   return nLinkLength;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_P2P_INITIATOR_DISCOVERED(
                        uint8_t* pBuffer,
                        const uint8_t * pLinkParameters,
                        uint32_t nLinkLength)
{
   if ((pLinkParameters != null) && (nLinkLength > 0))
   {
      CNALMemoryCopy(&pBuffer[0], pLinkParameters, nLinkLength);
   }

   return nLinkLength;
}

/* See header file */
NFC_HAL_INTERNAL uint32_t PNALProtocolFormat_NAL_EVT_UICC_CONNECTIVITY(
                        uint8_t* pBuffer,
                        const uint8_t* pPayload,
                        uint32_t nPayloadLength)
{
   if ((pPayload != null) && (nPayloadLength > 0))
   {
      CNALMemoryCopy(&pBuffer[0], pPayload, nPayloadLength);
   }
   return nPayloadLength;
}

/* See header file */
NFC_HAL_INTERNAL void PNALProtocolFormat_NAL_EVT_RF_FIELD(
                        uint8_t* pBuffer,
                        bool bExternalRfField)
{
   pBuffer[0] = (bExternalRfField==true)?1:0;
}

/* See header file */
NFC_HAL_INTERNAL void PNALProtocolFormat_NAL_EVT_SE_HOT_PLUG(
                        uint8_t* pBuffer,
                        bool bIsSeInserted)
{
   pBuffer[0] = (bIsSeInserted==true)?1:0;
}

