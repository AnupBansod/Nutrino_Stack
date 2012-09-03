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

#ifndef __WME_14443_3_H
#define __WME_14443_3_H

/*******************************************************************************
   Contains the declaration of the 14443-3 functions
*******************************************************************************/

/* Maximum buffer size for input and output (from reader) as defined in ISO 14443 */
/* this is also the maximum frame size with CRC */
#define P_14443_3_BUFFER_MAX_SIZE          256
#define P_14443_3_FSD_CODE_MAX_VALUE       8
/* Frame buffer maximum size:
   The frame maximum size for ISO 14443-3 A or B is 256 bytes including a 2-bytes CRC */
#define P_14443_3_FRAME_MAX_SIZE     (P_14443_3_BUFFER_MAX_SIZE - 2)

/* Default timeout value used for ISO 14443 A Part 3 */
/* In part 3, no timeout is exchanged, the default value is always used */
#define P_14443_3_A_DEFAULT_TIMEOUT             0x0B    /* 618 ms */

/**
 * Detection Configuration structure for Type A
 *
 * Placed here because referenced in Part 4.
 */
typedef struct __tP14P3DetectionConfigurationTypeA
{
   bool bUseCID;
   uint8_t nCID;
   uint32_t nBaudRate;

} tP14P3DetectionConfigurationTypeA;

/**
 * Detection Configuration structure for Type B
 *
 * Placed here because referenced in Part 4.
 */
typedef struct __tP14P3DetectionConfigurationTypeB
{
   bool bUseCID;
   uint8_t nCID;
   uint8_t nAFI;
   uint32_t nBaudRate;
   uint8_t nHigherLayerDataLength;
   uint8_t aHigherLayerData[W_EMUL_HIGHER_LAYER_DATA_MAX_LENGTH];

} tP14P3DetectionConfigurationTypeB;

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/**
 * @brief   Create the connection at 14443-3 level.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The user connection handle.
 *
 * @param[in]  hDriverConnection  The driver connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameterThe callback parameter.
 *
 * @param[in]  nProtocol  The protocol type.
 *
 * @param[in]  pBuffer  The buffer containing the activate result.
 *
 * @param[in]  nLength  The length of the buffer.
 **/
void P14P3UserCreateConnection(
            tContext* pContext,
            W_HANDLE hUserConnection,
            W_HANDLE hDriverConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nProtocol,
            const uint8_t* pBuffer,
            uint32_t nLength );

/* Same as P14Part3ExchangeData()
   with an flag to acivate the CRC checking in the response */
void P14P3UserExchangeData(
            tContext* pContext,
            W_HANDLE hConnection,
            tPBasicGenericDataCallbackFunction* pCallback,
            void* pCallbackParameter,
            const uint8_t* pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength,
            uint8_t* pCardToReaderBuffer,
            uint32_t nCardToReaderBufferMaxLength,
            W_HANDLE* phOperation,
            bool bCheckResponseCRC );

/**
 * @brief   Checks if a card is compliant with 14443-4.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 **/
W_ERROR P14P3UserCheckPart4(
            tContext* pContext,
            W_HANDLE hConnection );

/**
 * @brief   Checks if a card is compliant with Mifare.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pUID  The Mifare UID.
 *
 * @param[in]  pnUIDLength  The Mifare UID length.
 *
 * @param[in]  nType  The Mifare type (UL, 1K, 4K, Desfire).
 **/
W_ERROR P14P3UserCheckMifare(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pUID,
            uint8_t* pnUIDLength,
            uint8_t* pnType );

/**
 * @brief   Checks if a card is compliant with My-d.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pUID  The My-d UID.
 *
 * @param[in]  pnUIDLength  The My-d UID length.
 *
 * @param[in]  nType  The My-d type (Move, NFC).
 **/
W_ERROR P14P3UserCheckMyD(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pUID,
            uint8_t* pnUIDLength,
            uint8_t* pnType );

/**
 * @brief   Checks if a card is propably compliant with Type2 generic (ATQA = 0044 and SAK = 0).
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pnSAK  The SAK.
 *
 * @param[in]  pUID  The Mifare UID.
 *
 * @param[in]  pnUIDLength  The Mifare UID length.
 **/
W_ERROR P14P3UserCheckNDEF2Gen(
            tContext* pContext,
            W_HANDLE hConnection,
#if 0 /* RFU */
            uint8_t* pnATQA,
            uint8_t* pnSAK,
#endif
            uint8_t* pUID,
            uint8_t* pnUIDLength);

/**
 * @brief   Gets the card serial number.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hConnection  The connection handle.
 *
 * @param[in]  pSerialNumber  The card serial number.
 *
 * @param[in]  pnLength  The Mifare UID length.
 **/
W_ERROR P14P3UserCheckPico(
            tContext* pContext,
            W_HANDLE hConnection,
            uint8_t* pSerialNumber,
            uint8_t* pnLength );

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */


#ifdef P_READER_14P4_STANDALONE_SUPPORT

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

W_ERROR P14P3DriverSetDetectionConfigurationTypeB(
                     tContext* pContext,
                     uint8_t* pCommandBuffer,
                     uint32_t* pnCommandBufferLength,
                     const uint8_t* pDetectionConfigurationBuffer,
                     uint32_t nDetectionConfigurationLength);

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */

#endif /* P_READER_14P4_STANDALONE_SUPPORT */

#endif /* __WME_14443_3_H */
