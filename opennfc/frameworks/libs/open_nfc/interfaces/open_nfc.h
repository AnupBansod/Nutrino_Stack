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

  This header file contains the Client API functions.

*******************************************************************************/

#ifndef __OPEN_NFC_H
#define __OPEN_NFC_H

#ifdef __cplusplus
extern "C" {
#endif

/* If not included form a test implementation */
#ifndef P_IN_TEST_IMPLEMENTATION
#include "porting_types.h"
#endif /* #ifndef P_IN_TEST_IMPLEMENTATION */

/* API Prefix is empty by default */
#ifndef OPEN_NFC_CLIENT_API
#define OPEN_NFC_CLIENT_API
#endif

/*
   Indicators meaning:

         "I"  : Input  { User   -> Driver }
         "O"  : Output { Driver -> User   }
         "A"  : Asynchronous
         "W"  : Word { not a buffer, but a 32-bit value; typically used for handles }
         "2"  : second buffer qualifier { 0, 1, or 2 buffers in Open NFC API }
         "L"  : * in "PBL" or "PBL2" : 'length' parameter qualifier
                * in IOAL" : means "L"eave buffer after asynchronous notification, might be reused later.
*/
#define OPEN_NFC_BUF1_I
#define OPEN_NFC_BUF1_O
#define OPEN_NFC_BUF1_IO
#define OPEN_NFC_BUF1_IW
#define OPEN_NFC_BUF1_OW
#define OPEN_NFC_BUF1_IOW
#define OPEN_NFC_BUF1_IA
#define OPEN_NFC_BUF1_OA
#define OPEN_NFC_BUF1_IOA
#define OPEN_NFC_BUF1_IOAL
#define OPEN_NFC_BUF1_IWA
#define OPEN_NFC_BUF1_OWA
#define OPEN_NFC_BUF1_IOWA

#define OPEN_NFC_BUF2_I
#define OPEN_NFC_BUF2_O
#define OPEN_NFC_BUF2_IO
#define OPEN_NFC_BUF2_IW
#define OPEN_NFC_BUF2_OW
#define OPEN_NFC_BUF2_IOW
#define OPEN_NFC_BUF2_IA
#define OPEN_NFC_BUF2_OA
#define OPEN_NFC_BUF2_IOA
#define OPEN_NFC_BUF2_IWA
#define OPEN_NFC_BUF2_OWA
#define OPEN_NFC_BUF2_IOWA

#define OPEN_NFC_BUF3_I
#define OPEN_NFC_BUF3_OW

#define OPEN_NFC_BUF1_LENGTH
#define OPEN_NFC_BUF2_LENGTH
#define OPEN_NFC_BUF3_LENGTH

#define OPEN_NFC_BUF1_OFFSET
#define OPEN_NFC_BUF2_OFFSET
#define OPEN_NFC_BUF3_OFFSET

#define OPEN_NFC_USER_CALLBACK
#define OPEN_NFC_USER_EVENT_HANDLER

/*******************************************************************************
  Basic Types
*******************************************************************************/

typedef uint32_t W_HANDLE;

#define W_NULL_HANDLE  ((W_HANDLE)0)

typedef uint32_t W_ERROR;

/*******************************************************************************
  Error codes
*******************************************************************************/

#define W_SUCCESS                            0x00000000

#define W_ERROR_VERSION_NOT_SUPPORTED        0x00000001

#define W_ERROR_ITEM_NOT_FOUND               0x00000002

#define W_ERROR_BUFFER_TOO_SHORT             0x00000003

#define W_ERROR_PERSISTENT_DATA              0x00000004

#define W_ERROR_NO_EVENT                     0x00000005

#define W_ERROR_WAIT_CANCELLED               0x00000006

#define W_ERROR_UICC_COMMUNICATION           0x00000007

#define W_ERROR_BAD_HANDLE                   0x00000008

#define W_ERROR_EXCLUSIVE_REJECTED           0x00000009

#define W_ERROR_SHARE_REJECTED               0x0000000A

#define W_ERROR_BAD_PARAMETER                0x0000000B

#define W_ERROR_RF_PROTOCOL_NOT_SUPPORTED    0x0000000C

#define W_ERROR_CONNECTION_COMPATIBILITY     0x0000000D

#define W_ERROR_BUFFER_TOO_LARGE             0x0000000E

#define W_ERROR_INDEX_OUT_OF_RANGE           0x0000000F

#define W_ERROR_OUT_OF_RESOURCE              0x00000010

#define W_ERROR_BAD_TAG_FORMAT               0x00000011

#define W_ERROR_BAD_NDEF_FORMAT              0x00000012

#define W_ERROR_NDEF_UNKNOWN                 0x00000013

#define W_ERROR_LOCKED_TAG                   0x00000014

#define W_ERROR_TAG_FULL                     0x00000015

#define W_ERROR_CANCEL                       0x00000016

#define W_ERROR_TIMEOUT                      0x00000017

#define W_ERROR_TAG_DATA_INTEGRITY           0x00000018

#define W_ERROR_NFC_HAL_COMMUNICATION        0x00000019

#define W_ERROR_NCI_COMMUNICATION W_ERROR_NFC_HAL_COMMUNICATION /* Deprecated */

#define W_ERROR_WRONG_RTD                    0x0000001A

#define W_ERROR_TAG_WRITE                    0x0000001B

#define W_ERROR_BAD_NFCC_MODE                0x0000001C

#define W_ERROR_TOO_MANY_HANDLERS            0x0000001D

#define W_ERROR_BAD_STATE                    0x0000001E

#define W_ERROR_BAD_FIRMWARE_FORMAT          0x0000001F

#define W_ERROR_BAD_FIRMWARE_SIGNATURE       0x00000020

#define W_ERROR_DURING_HARDWARE_BOOT         0x00000021

#define W_ERROR_DURING_FIRMWARE_BOOT         0x00000022

#define W_ERROR_FEATURE_NOT_SUPPORTED        0x00000023

#define W_ERROR_CLIENT_SERVER_PROTOCOL       0x00000024

#define W_ERROR_FUNCTION_NOT_SUPPORTED       0x00000025

#define W_ERROR_TAG_NOT_LOCKABLE             0x00000026

#define W_ERROR_ITEM_LOCKED                  0x00000027

#define W_ERROR_SYNC_OBJECT                  0x00000028

#define W_ERROR_RETRY                        0x00000029

#define W_ERROR_DRIVER                       0x0000002A

#define W_ERROR_MISSING_INFO                 0x0000002B

#define W_ERROR_P2P_CLIENT_REJECTED          0x0000002C

#define W_ERROR_NFCC_COMMUNICATION           0x0000002D

#define W_ERROR_RF_COMMUNICATION             0x0000002E

#define W_ERROR_BAD_FIRMWARE_VERSION         0x0000002F

#define W_ERROR_HETEROGENEOUS_DATA           0x00000030

#define W_ERROR_CLIENT_SERVER_COMMUNICATION  0x00000031

#define W_ERROR_SECURITY                     0x00000032

/*******************************************************************************
  Priority levels
*******************************************************************************/

#define W_PRIORITY_EXCLUSIVE                 0xFF

#define W_PRIORITY_NO_ACCESS                 0x00

#define W_PRIORITY_MINIMUM                   0x01

#define W_PRIORITY_MAXIMUM                   0x0A

/*******************************************************************************
  Connection properties
*******************************************************************************/

#define W_PROP_ISO_14443_3_A                 0x01
#define W_PROP_ISO_14443_4_A                 0x02
#define W_PROP_ISO_14443_3_B                 0x03
#define W_PROP_ISO_14443_4_B                 0x04
#define W_PROP_ISO_15693_3                   0x05
#define W_PROP_ISO_15693_2                   0x06
#define W_PROP_ISO_7816_4                    0x07
#define W_PROP_BPRIME                        0x08

#define W_PROP_NFC_TAG_TYPE_1                0x10
#define W_PROP_NFC_TAG_TYPE_2                0x11
#define W_PROP_NFC_TAG_TYPE_3                0x12
#define W_PROP_NFC_TAG_TYPE_4_A              0x13
#define W_PROP_NFC_TAG_TYPE_4_B              0x14
#define W_PROP_NFC_TAG_TYPE_5                0x15
#define W_PROP_NFC_TAG_TYPE_6                0x16

#define W_PROP_TYPE1_CHIP                    0x20
#define W_PROP_JEWEL                         0x21
#define W_PROP_TOPAZ                         0x22
#define W_PROP_TOPAZ_512                     0x23
#define W_PROP_FELICA                        0x24
#define W_PROP_PICOPASS_2K                   0x25
#define W_PROP_PICOPASS_32K                  0x26
#define W_PROP_ICLASS_2K                     0x27
#define W_PROP_ICLASS_16K                    0x28
#define W_PROP_MIFARE_UL                     0x29
#define W_PROP_MIFARE_UL_C                   0x2A
#define W_PROP_MIFARE_MINI                   0x2B
#define W_PROP_MIFARE_1K                     0x2C
#define W_PROP_MIFARE_4K                     0x2D
#define W_PROP_MIFARE_DESFIRE_D40            0x2E
#define W_PROP_MIFARE_DESFIRE_EV1_2K         0x2F
#define W_PROP_MIFARE_DESFIRE_EV1_4K         0x30
#define W_PROP_MIFARE_DESFIRE_EV1_8K         0x31
#define W_PROP_MIFARE_PLUS_X_2K              0x32
#define W_PROP_MIFARE_PLUS_X_4K              0x33
#define W_PROP_MIFARE_PLUS_S_2K              0x34
#define W_PROP_MIFARE_PLUS_S_4K              0x35
#define W_PROP_TI_TAGIT                      0x36
#define W_PROP_ST_LRI_512                    0x37
#define W_PROP_ST_LRI_2K                     0x38
#define W_PROP_NXP_ICODE                     0x39
#define W_PROP_MY_D_MOVE                     0x3A
#define W_PROP_MY_D_NFC                      0x3B

#define W_PROP_SECURE_ELEMENT                0x50
#define W_PROP_VIRTUAL_TAG                   0x51

/* for internal use only */
#define W_PROP_P2P_TARGET                    0x60
#define W_PROP_ISO_7816_4_A                  0x61
#define W_PROP_ISO_7816_4_B                  0x62
#define W_PROP_NFC_TAG_TYPE_2_GENERIC        0x63

/*******************************************************************************
  NFC Controller Properties
*******************************************************************************/

#define W_NFCC_PROP_LIBRARY_VERSION              0x01
#define W_NFCC_PROP_LIBRARY_IMPL                 0x02
#define W_NFCC_PROP_LOADER_VERSION               0x03
#define W_NFCC_PROP_FIRMWARE_VERSION             0x04
#define W_NFCC_PROP_HARDWARE_VERSION             0x05
#define W_NFCC_PROP_HARDWARE_SN                  0x06

#define W_NFCC_PROP_NFC_HAL_VERSION              0x20

#define W_NFCC_PROP_SE_NUMBER                    0x21

#define W_NFCC_PROP_READER_ISO_14443_A_MAX_RATE  0x22
#define W_NFCC_PROP_READER_ISO_14443_A_INPUT_SIZE 0x23
#define W_NFCC_PROP_READER_ISO_14443_B_MAX_RATE  0x24
#define W_NFCC_PROP_READER_ISO_14443_B_INPUT_SIZE 0x25
#define W_NFCC_PROP_CARD_ISO_14443_A_MAX_RATE    0x26
#define W_NFCC_PROP_CARD_ISO_14443_B_MAX_RATE    0x27
#define W_NFCC_PROP_STANDBY_TIMEOUT              0x28

#define W_NFCC_PROP_BATTERY_LOW_SUPPORTED        0x40
#define W_NFCC_PROP_BATTERY_OFF_SUPPORTED        0x41
#define W_NFCC_PROP_STANDBY_SUPPORTED            0x42

#define W_NFCC_PROP_CARD_ISO_14443_A_CID         0x50
#define W_NFCC_PROP_CARD_ISO_14443_A_NAD         0x51
#define W_NFCC_PROP_CARD_ISO_14443_B_CID         0x52
#define W_NFCC_PROP_CARD_ISO_14443_B_NAD         0x53
#define W_NFCC_PROP_READER_ISO_14443_A_CID       0x54
#define W_NFCC_PROP_READER_ISO_14443_A_NAD       0x55
#define W_NFCC_PROP_READER_ISO_14443_B_CID       0x56
#define W_NFCC_PROP_READER_ISO_14443_B_NAD       0x57

#define W_NFCC_PROP_P2P                          0x60

/* NFC Controller modes */
#define W_NFCC_MODE_BOOT_PENDING             0x00
#define W_NFCC_MODE_MAINTENANCE              0x01
#define W_NFCC_MODE_NO_FIRMWARE              0x02
#define W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED   0x03
#define W_NFCC_MODE_NOT_RESPONDING           0x04
#define W_NFCC_MODE_LOADER_NOT_SUPPORTED     0x05
#define W_NFCC_MODE_ACTIVE                   0x06
#define W_NFCC_MODE_SWITCH_TO_STANDBY        0x07
#define W_NFCC_MODE_STANDBY                  0x08
#define W_NFCC_MODE_SWITCH_TO_ACTIVE         0x09

#ifndef P_OPEN_NFC_CONSTANTS_ONLY

/*******************************************************************************
  Basic Functions
*******************************************************************************/

#ifndef DO_NOT_USE_LWRAP_UNICODE

#if defined __linux__
#define LWRAP_UNICODE
#endif

#endif /* #ifndef DO_NOT_USE_LWRAP_UNICODE */

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWBasicGenericCompletionFunction(
               void* pCallbakParameter );

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWBasicGenericCallbackFunction(
               void* pCallbackParameter,
               W_ERROR nResult );

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWBasicGenericDataCallbackFunction(
               void* pCallbackParameter,
               uint32_t nDataLength,
               W_ERROR nResult );
/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWBasicGenericHandleCallbackFunction(
               void* pCallbackParameter,
               W_HANDLE hHandle,
               W_ERROR nResult );

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWBasicGenericEventHandler(
               void* pHandlerParameter,
               W_ERROR nEventCode );

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWBasicGenericEventHandler2(
               void* pHandlerParameter,
               W_ERROR nEventCode1,
               W_ERROR nEventCode2 );

#if defined (LWRAP_UNICODE)
   /* In User File; Force replacement of original functions with a Linux Wrapper Function */
#define WBasicInit                      LWRAP_WBasicInit
#else
   /* In Linux Wrapper source file; do NOT change any original definition */
/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), THREAD_SAFE
 **/
OPEN_NFC_CLIENT_API W_ERROR WBasicInit(
                  const tchar* pVersionString );

#endif

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), THREAD_SAFE
 **/
OPEN_NFC_CLIENT_API void WBasicTerminate( void );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WBasicGetConnectionPropertyNumber(
                  W_HANDLE hConnection,
                  OPEN_NFC_BUF1_OW uint32_t* pnPropertyNumber );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WBasicGetConnectionProperties(
                  W_HANDLE hConnection,
                  OPEN_NFC_BUF1_O uint8_t* pPropertyArray,
                  OPEN_NFC_BUF1_LENGTH uint32_t nArrayLength );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WBasicCheckConnectionProperty(
                  W_HANDLE hConnection,
                  uint8_t nPropertyIdentifier );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 * @keyword  JNI(org.opennfc.nfctag.MethodAutogen)
 **/
OPEN_NFC_CLIENT_API void WBasicCloseHandle(
                  W_HANDLE hHandle );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WBasicCloseHandleSafe(
                  W_HANDLE hHandle,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WBasicCloseHandleSafeSync(
                  W_HANDLE hHandle );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WBasicCancelOperation(
                  W_HANDLE hOperation );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), THREAD_SAFE
 **/
OPEN_NFC_CLIENT_API W_ERROR WBasicPumpEvent(
                  bool bWait );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), THREAD_SAFE
 **/
OPEN_NFC_CLIENT_API void WBasicStopEventLoop( void );

/**
 * @keyword  P_FUNCTION, THREAD_SAFE
 **/
OPEN_NFC_CLIENT_API void WBasicExecuteEventLoop( void );

#define W_TEST_SERVER_RESPONSE_MAX_LENGTH  500

/**
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WBasicTestExchangeMessage(
                  tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t* pSendMessageBuffer,
                  uint32_t nSendMessageBufferLength,
                  uint8_t* pReceivedMessageBuffer );

/*******************************************************************************
  Reader Mode:
  - Reader Registry Functions
*******************************************************************************/

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WReaderIsPropertySupported(
                  uint8_t nPropertyIdentifier );

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWReaderCardDetectionHandler(
                  void* pHandlerParameter,
                  W_HANDLE hConnection,
                  W_ERROR nError );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WReaderListenToCardDetection(
                  OPEN_NFC_USER_EVENT_HANDLER tWReaderCardDetectionHandler* pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  OPEN_NFC_BUF1_I const uint8_t* pConnectionPropertyArray,
                  OPEN_NFC_BUF1_LENGTH uint32_t nPropertyNumber,
                  OPEN_NFC_BUF2_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WReaderHandlerWorkPerformed(
                  W_HANDLE hConnection,
                  bool bGiveToNextListener,
                  bool bCardApplicationMatch );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WReaderPreviousApplicationMatch(
                  W_HANDLE hConnection);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WReaderSetPulsePeriod(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint32_t nPulsePeriod );

OPEN_NFC_CLIENT_API W_ERROR WReaderSetPulsePeriodSync(
                  uint32_t nPulsePeriod );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), DRIVER_API, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WReaderGetPulsePeriod(
                  OPEN_NFC_BUF1_OW uint32_t* pnTimeout );

#define W_READER_ERROR_UNKNOWN      0x00
#define W_READER_ERROR_COLLISION    0x01
#define W_READER_MULTIPLE_DETECTION 0x02

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), DRIVER_API, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WReaderErrorEventRegister(
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler *pHandler,
                  void *pHandlerParameter,
                  uint8_t nEventType,
                  bool bCardDetectionRequested,
                  OPEN_NFC_BUF1_OW W_HANDLE* phRegistryHandle );

/*******************************************************************************
  Reader Mode:
  - ISO 14443-3 Functions
*******************************************************************************/

/* Predefined here because used in reader structures */
#define W_EMUL_APPLICATION_DATA_MAX_LENGTH         252
#define W_EMUL_HIGHER_LAYER_DATA_MAX_LENGTH        245
#define W_EMUL_HIGHER_LAYER_RESPONSE_MAX_LENGTH    253

typedef struct
{
   uint8_t  nUIDLength;
   uint8_t  aUID[10];
   uint16_t nATQA;
   uint8_t  nSAK;
}tW14Part3ConnectionInfoTypeA;

typedef struct
{
   uint8_t  aATQB[12];
   uint8_t  nAFI;
   bool     bIsCIDSupported;
   bool     bIsNADSupported;
   uint32_t nCardInputBufferSize;
   uint32_t nReaderInputBufferSize;
   uint32_t nBaudRate;
   uint32_t nTimeout;
   uint8_t  aHigherLayerData[W_EMUL_HIGHER_LAYER_DATA_MAX_LENGTH];
   uint8_t  nHigherLayerDataLength;
   uint8_t  aHigherLayerResponse[W_EMUL_HIGHER_LAYER_RESPONSE_MAX_LENGTH];
   uint8_t  nHigherLayerResponseLength;
   uint8_t  nMBLI_CID;
}tW14Part3ConnectionInfoTypeB;

typedef union
{
   tW14Part3ConnectionInfoTypeA sW14TypeA;
   tW14Part3ConnectionInfoTypeB sW14TypeB;
}tW14Part3ConnectionInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W14Part3GetConnectionInfo(
                  W_HANDLE hConnection,
                  tW14Part3ConnectionInfo* p14Part3ConnectionInfo );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W14Part3SetTimeout(
                  W_HANDLE hConnection,
                  uint32_t nTimeout );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W14Part3ListenToCardDetectionTypeB(
                  OPEN_NFC_USER_EVENT_HANDLER tWReaderCardDetectionHandler* pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  uint8_t nAFI,
                  bool bUseCID,
                  uint8_t nCID,
                  uint32_t nBaudRate,
                  OPEN_NFC_BUF1_I const uint8_t* pHigherLayerDataBuffer,
                  OPEN_NFC_BUF1_LENGTH uint8_t nHigherLayerDataLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W14Part3ExchangeData(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pReaderToCardBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nReaderToCardBufferLength,
                  OPEN_NFC_BUF2_OA uint8_t* pCardToReaderBuffer,
                  OPEN_NFC_BUF2_LENGTH uint32_t nCardToReaderBufferMaxLength,
                  OPEN_NFC_BUF3_OW W_HANDLE* phOperation );

OPEN_NFC_CLIENT_API W_ERROR W14Part3ExchangeDataSync(
                  W_HANDLE hConnection,
                  const uint8_t* pReaderToCardBuffer,
                  uint32_t nReaderToCardBufferLength,
                  uint8_t* pCardToReaderBuffer,
                  uint32_t nCardToReaderBufferMaxLength,
                  uint32_t* pnCardToReaderBufferActualLength );

/*******************************************************************************
  Reader Mode:
  - B PRIME Functions
*******************************************************************************/

#define P_B_PRIME_SIZE_MAX_APGEN  4
#define P_B_PRIME_SIZE_MIN_APGEN  3

#define P_B_PRIME_SIZE_MAX_REPGEN 32

#define P_B_PRIME_LENGTH_UID      7


typedef struct
{
   uint8_t nREPGENLength;
   uint8_t aREPGEN[16];
   uint32_t nTimeout;

}  tWBPrimeConnectionInfo;


/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WBPrimeGetConnectionInfo(
                  W_HANDLE hConnection,
                  tWBPrimeConnectionInfo* pBPrimeConnectionInfo );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WBPrimeSetTimeout(
                  W_HANDLE hConnection,
                  uint32_t nTimeout );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WBPrimeListenToCardDetection(
                  OPEN_NFC_USER_EVENT_HANDLER tWReaderCardDetectionHandler* pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  OPEN_NFC_BUF1_I const uint8_t* pAPGENBuffer,
                  OPEN_NFC_BUF1_LENGTH uint8_t nAPGENLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WBPrimeExchangeData(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pReaderToCardBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nReaderToCardBufferLength,
                  OPEN_NFC_BUF2_OA uint8_t* pCardToReaderBuffer,
                  OPEN_NFC_BUF2_LENGTH uint32_t nCardToReaderBufferMaxLength,
                  OPEN_NFC_BUF3_OW W_HANDLE* phOperation );

OPEN_NFC_CLIENT_API W_ERROR WBPrimeExchangeDataSync(
                  W_HANDLE hConnection,
                  const uint8_t* pReaderToCardBuffer,
                  uint32_t nReaderToCardBufferLength,
                  uint8_t* pCardToReaderBuffer,
                  uint32_t nCardToReaderBufferMaxLength,
                  uint32_t* pnCardToReaderBufferActualLength );



/*******************************************************************************
  Reader Mode:
  - ISO 14443-4 Functions
*******************************************************************************/

typedef struct
{
   bool     bIsCIDSupported;
   bool     bIsNADSupported;
   uint8_t  nCID;
   uint8_t  nNAD;
   uint8_t  aApplicationData[W_EMUL_APPLICATION_DATA_MAX_LENGTH];
   uint8_t  nApplicationDataLength;
   uint32_t nCardInputBufferSize;
   uint32_t nReaderInputBufferSize;
   uint8_t  nFWI_SFGI;
   uint32_t nTimeout;
   uint8_t  nDataRateMaxDiv;
   uint32_t nBaudRate;
}tW14Part4ConnectionInfoTypeA;

typedef struct
{
   uint8_t nNAD;
}tW14Part4ConnectionInfoTypeB;

typedef union
{
   tW14Part4ConnectionInfoTypeA sW14TypeA;
   tW14Part4ConnectionInfoTypeB sW14TypeB;
}tW14Part4ConnectionInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W14Part4GetConnectionInfo(
                  W_HANDLE hConnection,
                  tW14Part4ConnectionInfo* p14Part4ConnectionInfo );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W14Part4SetNAD(
                  W_HANDLE hConnection,
                  uint8_t nNAD );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W14Part4ListenToCardDetectionTypeA(
                  OPEN_NFC_USER_EVENT_HANDLER tWReaderCardDetectionHandler* pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  bool bUseCID,
                  uint8_t nCID,
                  uint32_t nBaudRate,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W14Part4ListenToCardDetectionTypeB(
                  OPEN_NFC_USER_EVENT_HANDLER tWReaderCardDetectionHandler* pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  uint8_t nAFI,
                  bool bUseCID,
                  uint8_t nCID,
                  uint32_t nBaudRate,
                  OPEN_NFC_BUF1_I const uint8_t* pHigherLayerDataBuffer,
                  OPEN_NFC_BUF1_LENGTH uint8_t  nHigherLayerDataLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W14Part4ExchangeData(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pReaderToCardBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nReaderToCardBufferLength,
                  OPEN_NFC_BUF2_OA uint8_t* pCardToReaderBuffer,
                  OPEN_NFC_BUF2_LENGTH uint32_t nCardToReaderBufferMaxLength,
                  OPEN_NFC_BUF3_OW W_HANDLE* phOperation );

OPEN_NFC_CLIENT_API W_ERROR W14Part4ExchangeDataSync(
                  W_HANDLE hConnection,
                  const uint8_t* pReaderToCardBuffer,
                  uint32_t nReaderToCardBufferLength,
                  uint8_t* pCardToReaderBuffer,
                  uint32_t nCardToReaderBufferMaxLength,
                  uint32_t* pnCardToReaderBufferActualLength );

/*******************************************************************************
  Reader Mode:
  - ISO 7816-4 Functions
*******************************************************************************/

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W7816GetATRSize(
                  W_HANDLE hConnection,
                  OPEN_NFC_BUF1_OW uint32_t* pnSize );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W7816GetATR(
                  W_HANDLE hConnection,
                  OPEN_NFC_BUF1_O uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferMaxLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualLength );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W7816ExchangeAPDU(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pSendAPDUBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nSendAPDUBufferLength,
                  OPEN_NFC_BUF2_OA uint8_t* pReceivedAPDUBuffer,
                  OPEN_NFC_BUF2_LENGTH uint32_t nReceivedAPDUBufferMaxLength,
                  OPEN_NFC_BUF3_OW W_HANDLE* phOperation );

OPEN_NFC_CLIENT_API W_ERROR W7816ExchangeAPDUSync(
                  W_HANDLE hConnection,
                  const uint8_t* pSendAPDUBuffer,
                  uint32_t nSendAPDUBufferLength,
                  uint8_t* pReceivedAPDUBuffer,
                  uint32_t nReceivedAPDUBufferMaxLength,
                  uint32_t* pnReceivedAPDUActualLength );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/

OPEN_NFC_CLIENT_API W_ERROR W7816GetResponseAPDUData(
                  W_HANDLE hConnection,
                  OPEN_NFC_BUF1_O uint8_t* pReceivedAPDUBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nReceivedAPDUBufferMaxLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnReceivedAPDUActualLength );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W7816OpenLogicalChannel(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericHandleCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pAID,
                  OPEN_NFC_BUF1_LENGTH uint32_t nAIDLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR W7816OpenLogicalChannelSync(
                  W_HANDLE hConnection,
                  const uint8_t* pAID,
                  uint32_t nAIDLength,
                  W_HANDLE* phLogicalChannel);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W7816CloseLogicalChannel(
                  W_HANDLE hLogicalChannel,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR W7816CloseLogicalChannelSync(
                  W_HANDLE hLogicalChannel);

/*******************************************************************************
  Reader Mode:
  - ISO 15693 Support Functions
*******************************************************************************/

#define  W_15_ATTRIBUTE_ACTION_SET_AFI     0x01
#define  W_15_ATTRIBUTE_ACTION_LOCK_AFI    0x02
#define  W_15_ATTRIBUTE_ACTION_SET_DSFID   0x04
#define  W_15_ATTRIBUTE_ACTION_LOCK_DSFID  0x08
#define  W_15_ATTRIBUTE_ACTION_LOCK_TAG    0x10

typedef struct
{
   uint8_t  UID[8];
   bool  bIsAFISupported;
   bool  bIsAFILocked;
   bool  bIsDSFIDSupported;
   bool  bIsDSFIDLocked;
   uint8_t  nAFI;
   uint8_t  nDSFID;
   uint8_t  nSectorSize;
   uint16_t  nSectorNumber;
} tW15ConnectionInfo;

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W15ListenToCardDetection (
                  OPEN_NFC_USER_EVENT_HANDLER tWReaderCardDetectionHandler* pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  uint8_t nAFI,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W15GetConnectionInfo (
                  W_HANDLE hConnection,
                  tW15ConnectionInfo* pConnectionInfo) ;

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W15SetTagSize(
                  W_HANDLE hConnection,
                  uint16_t nSectorNumber,
                  uint8_t nSectorSize );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR W15IsWritable (
                  W_HANDLE hConnection,
                  uint8_t nSectorIndex);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W15SetAttribute (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction *pCallback,
                  void* pCallbackParameter,
                  uint8_t nActions,
                  uint8_t nAFI,
                  uint8_t nDSFID,
                  OPEN_NFC_BUF1_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR W15SetAttributeSync (
                  W_HANDLE hConnection,
                  uint8_t nActions,
                  uint8_t nAFI,
                  uint8_t nDSFID);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W15Read (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  OPEN_NFC_BUF2_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR W15ReadSync (
                  W_HANDLE hConnection,
                  uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void W15Write (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  bool bLockSectors,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR W15WriteSync (
                  W_HANDLE hConnection,
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength,
                  bool bLockSectors);

/*******************************************************************************
  Reader Mode:
  - Picopass Functions
*******************************************************************************/

typedef struct
{
   uint8_t  UID[8];
   uint16_t  nSectorSize;
   uint16_t  nSectorNumber;
} tWPicoConnectionInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WPicoGetConnectionInfo (
                  W_HANDLE hConnection,
                  tWPicoConnectionInfo* pConnectionInfo);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WPicoIsWritable (
                  W_HANDLE hConnection);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WPicoRead (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  OPEN_NFC_BUF2_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WPicoReadSync (
                  W_HANDLE hConnection,
                  uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WPicoWrite (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  bool bLockCard,
                  OPEN_NFC_BUF2_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WPicoWriteSync (
                  W_HANDLE hConnection,
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength,
                  bool bLockCard);


/*******************************************************************************
  Reader Mode:
  - Mifare Functions
*******************************************************************************/

/* Mifare access rights */
#define W_MIFARE_UL_READ_OK                         0
#define W_MIFARE_UL_READ_AUTHENTICATED              1
#define W_MIFARE_UL_WRITE_OK                        0
#define W_MIFARE_UL_WRITE_AUTHENTICATED             1
#define W_MIFARE_UL_WRITE_LOCKED                    2

typedef struct
{
   uint8_t  UID[7];
   uint16_t  nSectorSize;
   uint16_t  nSectorNumber;
} tWMifareConnectionInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WMifareGetConnectionInfo (
                  W_HANDLE hConnection,
                  tWMifareConnectionInfo* pConnectionInfo);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMifareRead (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  OPEN_NFC_BUF2_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WMifareReadSync (
                  W_HANDLE hConnection,
                  uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMifareWrite (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  bool bLockSectors,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WMifareWriteSync (
                  W_HANDLE hConnection,
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength,
                  bool bLockSectors);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WMifareULForceULC(
                  W_HANDLE hConnection);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMifareULFreezeDataLockConfiguration(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction * pCallback,
                  void * pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WMifareULFreezeDataLockConfigurationSync(
                  W_HANDLE hConnection);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMifareULRetrieveAccessRights (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction *pCallback,
                  void *pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WMifareULRetrieveAccessRightsSync (
                  W_HANDLE hConnection);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WMifareULGetAccessRights (
                  W_HANDLE hConnection,
                  uint32_t nOffset,
                  uint32_t nLength,
                  OPEN_NFC_BUF1_OW uint32_t *pnRights);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMifareULCSetAccessRights (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction *pCallback,
                  void *pCallbackParameter,
                  OPEN_NFC_BUF1_I const uint8_t * pKey,
                  OPEN_NFC_BUF1_LENGTH uint32_t nKeyLength,
                  uint8_t nThreshold,
                  uint32_t nRights,
                  bool bLockConfiguration);

OPEN_NFC_CLIENT_API W_ERROR WMifareULCSetAccessRightsSync (
                  W_HANDLE hConnection,
                  const uint8_t * pKey,
                  uint32_t nKeyLength,
                  uint8_t nThreshold,
                  uint32_t nRights,
                  bool bLockConfiguration);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMifareULCAuthenticate (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction *pCallback,
                  void *pCallbackParameter,
                  OPEN_NFC_BUF1_I const uint8_t* pKey,
                  OPEN_NFC_BUF1_LENGTH uint32_t nKeyLength);

OPEN_NFC_CLIENT_API W_ERROR WMifareULCAuthenticateSync (
                  W_HANDLE hConnection,
                  const uint8_t* pKey,
                  uint32_t nKeyLength);

/*******************************************************************************
  Reader Mode:
  - My-d Functions
*******************************************************************************/

/* My-d move configuration bits */
/* If set, indicates the my-d move card configuration is locked and can not been changed */
#define W_MY_D_MOVE_CONFIG_LOCKED                  0x01
/* If set, indicates the write password is active for write commands for all blocks starting from address 0x10 */
#define W_MY_D_MOVE_WRITE_PASSWORD_ENABLED         0x02
/* If set, indicates the read and write password is active for read, write and decrement commands for all blocks starting from address 0x10 */
#define W_MY_D_MOVE_READ_WRITE_PASSWORD_ENABLED    0x04
/* If set, indicates blocks 0x22 and 0x23 are configured as a 16 bits counter */
#define W_MY_D_MOVE_16BITS_COUNTER_ENABLED         0x80

typedef struct
{
   uint8_t  UID[7];
   uint16_t nSectorSize;
   uint16_t nSectorNumber;
} tWMyDConnectionInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WMyDGetConnectionInfo (
                  W_HANDLE hConnection,
                  tWMyDConnectionInfo* pConnectionInfo);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMyDRead (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  OPEN_NFC_BUF2_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WMyDReadSync (
                  W_HANDLE hConnection,
                  uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMyDWrite (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  bool bLockSectors,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WMyDWriteSync (
                  W_HANDLE hConnection,
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength,
                  bool bLockSectors);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMyDMoveFreezeDataLockConfiguration(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction * pCallback,
                  void * pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WMyDMoveFreezeDataLockConfigurationSync(
                  W_HANDLE hConnection);

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWMyDMoveGetConfigurationCompleted(
                  void *pCallbackParameter,
                  W_ERROR nError,
                  uint8_t nStatusByte,
                  uint8_t nPasswordRetryCounter);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMyDMoveGetConfiguration (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWMyDMoveGetConfigurationCompleted *  pCallback,
                  void *  pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WMyDMoveGetConfigurationSync (
                  W_HANDLE hConnection,
                  uint8_t * pnStatusByte,
                  uint8_t * pnPasswordRetryCounter);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMyDMoveSetConfiguration (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction * pCallback,
                  void * pCallbackParameter,
                  uint8_t nStatusByte,
                  uint8_t nPasswordRetryCounter,
                  uint32_t nPassword,
                  bool bLockConfiguration);

OPEN_NFC_CLIENT_API W_ERROR WMyDMoveSetConfigurationSync (
                  W_HANDLE hConnection,
                  uint8_t nStatusByte,
                  uint8_t nPasswordRetryCounter,
                  uint32_t nPassword,
                  bool bLockConfiguration);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WMyDMoveAuthenticate (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction *pCallback,
                  void *pCallbackParameter,
                  uint32_t nPassword);

OPEN_NFC_CLIENT_API W_ERROR WMyDMoveAuthenticateSync (
                  W_HANDLE hConnection,
                  uint32_t nPassword);

/*******************************************************************************
  Reader Mode:
  - Type 1 Chip Functions
*******************************************************************************/

typedef struct
{
   uint8_t   UID[7];
   uint16_t  nBlockSize;
   uint16_t  nBlockNumber;
   uint8_t   aHeaderRom[2];
} tWType1ChipConnectionInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WType1ChipGetConnectionInfo (
                  W_HANDLE hConnection,
                  tWType1ChipConnectionInfo* pConnectionInfo);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WType1ChipIsWritable (
                  W_HANDLE hConnection);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WType1ChipRead (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  OPEN_NFC_BUF2_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WType1ChipReadSync (
                  W_HANDLE hConnection,
                  uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WType1ChipWrite (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pBuffer,
                  uint32_t nOffset,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  bool bLockBlocks,
                  OPEN_NFC_BUF2_OW W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WType1ChipWriteSync (
                  W_HANDLE hConnection,
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nLength,
                  bool bLockSectors);

/*******************************************************************************
  Reader Mode:
  - FeliCa Functions
*******************************************************************************/

typedef struct
{
   uint8_t aManufacturerID[8];
   uint8_t aManufacturerParameter[8];
   uint16_t nSystemCode;
} tWFeliCaConnectionInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WFeliCaGetConnectionInfo (
                  W_HANDLE hConnection,
                  tWFeliCaConnectionInfo* pConnectionInfo);


/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WFeliCaExchangeData(
      W_HANDLE hUserConnection,
      tWBasicGenericDataCallbackFunction * pCallback,
      void * pCallbackParameter,
      const uint8_t * pReaderToCardBuffer,
      uint32_t nReaderToCardBufferLength,
      uint8_t * pCardToReaderBuffer,
      uint32_t nCardToReaderBufferMaxLength,
      W_HANDLE * phOperation);
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WFeliCaRead (
                  W_HANDLE hConnection,
                  tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t* pBuffer,
                  uint32_t nLength,
                  uint8_t  nNumberOfService,
                  const uint16_t* pServiceCodeList,
                  uint8_t  nNumberOfBlocks,
                  const uint8_t* pBlockList,
                  W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WFeliCaReadSync (
                  W_HANDLE hConnection,
                  uint8_t* pBuffer,
                  uint32_t nLength,
                  uint8_t  nNumberOfService,
                  const uint16_t* pServiceCodeList,
                  uint8_t  nNumberOfBlocks,
                  const uint8_t* pBlockList);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WFeliCaWrite (
                  W_HANDLE hConnection,
                  tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  const uint8_t* pBuffer,
                  uint32_t nLength,
                  uint8_t  nNumberOfService,
                  const uint16_t* pServiceCodeList,
                  uint8_t  nNumberOfBlocks,
                  const uint8_t* pBlockList,
                  W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WFeliCaWriteSync (
                  W_HANDLE hConnection,
                  const uint8_t* pBuffer,
                  uint32_t nLength,
                  uint8_t nNumberOfService,
                  const uint16_t* pServiceCodeList,
                  uint8_t nNumberOfBlocks,
                  const uint8_t* pBlockList);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WFeliCaListenToCardDetection (
                  tWReaderCardDetectionHandler *pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  uint16_t nSystemCode,
                  W_HANDLE* phEventRegistry);


/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WFeliCaSelectSystem (
                  W_HANDLE hConnection,
			         tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  const uint8_t	nIndexSubSystem);

OPEN_NFC_CLIENT_API W_ERROR WFeliCaSelectSystemSync (
                  W_HANDLE hConnection,
                  const uint8_t	nIndexSubSystem);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WFeliCaSelectCard (
                  W_HANDLE hConnection,
			         tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  const tWFeliCaConnectionInfo* pFeliCaConnectionInfo);

OPEN_NFC_CLIENT_API W_ERROR WFeliCaSelectCardSync (
                  W_HANDLE hConnection,
                  const tWFeliCaConnectionInfo* pFeliCaConnectionInfo);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WFeliCaGetCardNumber(
                  W_HANDLE hConnection,
                  uint32_t* pnCardNumber);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WFeliCaGetCardList(
                  W_HANDLE hConnection,
						tWFeliCaConnectionInfo* aFeliCaConnectionInfos,
                  const uint32_t nArraySize);

/*******************************************************************************
  Peer2Peer Functions
*******************************************************************************/

typedef struct
{
   uint16_t        nLocalLTO;
   uint16_t        nLocalMIU;
   bool            bActivateInitiator;

} tWP2PConfiguration;


/**
 * @keyword P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WP2PGetConfiguration(
                  tWP2PConfiguration * pConfiguration);

/**
 * @keyword P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WP2PSetConfiguration(
                  const tWP2PConfiguration * pConfiguration);

/**
 * @keyword TEST_VECTOR, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API void WP2PEstablishLink(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericHandleCallbackFunction * pEstablishmentCallback,
                  void * pEstablishmentCallbackParameter,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction * pReleaseCallback,
                  void * pReleaseCallbackParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE *phOperation);

/**
 * @keyword  P_FUNCTION, DRIVER_API
 **/
W_HANDLE WP2PEstablishLinkDriver1(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericHandleCallbackFunction* pEstablishmentCallback,
                  void* pEstablishmentCallbackParameter);

/**
 * @keyword  P_FUNCTION, DRIVER_API
 **/
void WP2PEstablishLinkDriver2(
                  W_HANDLE hLink,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pReleaseCallback,
                  void* pReleaseCallbackParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE * phOperation);

typedef struct
{
  uint8_t         nAgreedLLCPVersion;
  uint8_t         nRemoteLLCPVersion;
  uint16_t        nRemoteWKS;
  uint32_t        nRemoteMIU;
  uint32_t        nRemoteLTO;
  uint32_t        nBaudRate;
  bool            bIsInitiator;

} tWP2PLinkProperties;

/**
 * @keyword P_FUNCTION, TEST_VECTOR
 **/

OPEN_NFC_CLIENT_API W_ERROR WP2PGetLinkProperties(
                  W_HANDLE hLink,
                  tWP2PLinkProperties * pProperties
);

#define W_P2P_TYPE_CLIENT           1
#define W_P2P_TYPE_SERVER           2
#define W_P2P_TYPE_CLIENT_SERVER    3
#define W_P2P_TYPE_CONNECTIONLESS   4

#if defined (LWRAP_UNICODE)
#define WP2PCreateSocket          LWRAP_WP2PCreateSocket
#else
/**
 * @keyword P_FUNCTION, TEST_VECTOR, JNI(org.opennfc.MethodAutogen)
 **/
OPEN_NFC_CLIENT_API W_ERROR WP2PCreateSocket(
                  uint8_t nType,
                  const tchar * pServiceURI,
                  uint8_t nSAP,
                  OPEN_NFC_BUF1_OW W_HANDLE * phSocket);
#endif

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API void WP2PConnect(
                  W_HANDLE hSocket,
                  W_HANDLE hLink,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction * pEstablishmentCallback,
                  void * pEstablishmentCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WP2PConnectSync(
                  W_HANDLE hSocket,
                  W_HANDLE hLink);

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API void WP2PShutdown(
                  W_HANDLE  hSocket,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction * pReleaseCallback,
                  void * pReleaseCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WP2PShutdownSync(
                  W_HANDLE    hSocket);

#define W_P2P_LOCAL_MIU                   0
#define W_P2P_LOCAL_RW                    1
#define W_P2P_CONNECTION_ESTABLISHED      2
#define W_P2P_DATA_AVAILABLE              3
#define W_P2P_LOCAL_SAP                   4
#define W_P2P_REMOTE_SAP                  5
#define W_P2P_REMOTE_MIU                  6
#define W_P2P_REMOTE_RW                   7
#define W_P2P_IS_SERVER                   8

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API W_ERROR WP2PGetSocketParameter(
                  W_HANDLE   hSocket,
                  uint32_t   nParameter,
                  OPEN_NFC_BUF1_OW uint32_t * pnValue);

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen), DRIVER_API
 **/

OPEN_NFC_CLIENT_API W_ERROR WP2PSetSocketParameter(
                  W_HANDLE    hSocket,
                  uint32_t    nParameter,
                  uint32_t    nValue);

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API void WP2PRead(
                  W_HANDLE hSocket,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericDataCallbackFunction * pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t* pReceptionBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nReceptionBufferLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WP2PReadSync(
                  W_HANDLE hSocket,
                  uint8_t* pReceptionBuffer,
                  uint32_t nReceptionBufferLength,
                  uint32_t* pnDataLength );

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API void WP2PWrite(
                  W_HANDLE hSocket,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pSendBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nSendBufferLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);


OPEN_NFC_CLIENT_API W_ERROR WP2PWriteSync(
                  W_HANDLE hSocket,
                  const uint8_t* pSendBuffer,
                  uint32_t nSendBufferLength );

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API void WP2PSendTo(
                  W_HANDLE hSocket,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t nSAP,
                  OPEN_NFC_BUF1_IA const uint8_t* pSendBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nSendBufferLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WP2PSendToSync(
                  W_HANDLE hSocket,
                  uint8_t nSAP,
                  const uint8_t* pSendBuffer,
                  uint32_t nSendBufferLength);

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/

typedef void tWP2PRecvFromCompleted(
                  void* pCallbackParameter,
                  uint32_t nDataLength,
                  W_ERROR nError,
                  uint8_t nSAP );

/**
 * @keyword TEST_VECTOR, P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 **/

OPEN_NFC_CLIENT_API void WP2PRecvFrom(
                  W_HANDLE hSocket,
                  OPEN_NFC_USER_CALLBACK tWP2PRecvFromCompleted* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t * pReceptionBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nReceptionBufferLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WP2PRecvFromSync (
                  W_HANDLE hSocket,
                  uint8_t* pReceptionBuffer,
                  uint32_t nReceptionBufferLength,
                  uint8_t *pnSAP,
                  uint32_t *pnDataLength);

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWP2PURILookupCompleted(
                  void* pCallbackParameter,
                  uint8_t nDSAP,
                  W_ERROR nError );

#if defined (LWRAP_UNICODE)
#define WP2PURILookup       LWRAP_WP2PURILookup
#else
/**
 * @keyword  JNI(org.opennfc.MethodAutogen), TEST_VECTOR, P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WP2PURILookup(
                  W_HANDLE hLink,
                  OPEN_NFC_USER_CALLBACK tWP2PURILookupCompleted* pCallback,
                  void* pCallbackParameter,
                  const tchar* pServiceURI);
#endif

/*******************************************************************************
  NDEF Functions
*******************************************************************************/

#define W_NDEF_TNF_EMPTY            0x00
#define W_NDEF_TNF_WELL_KNOWN       0x01
#define W_NDEF_TNF_MEDIA            0x02
#define W_NDEF_TNF_ABSOLUTE_URI     0x03
#define W_NDEF_TNF_EXTERNAL         0x04
#define W_NDEF_TNF_UNKNOWN          0x05
#define W_NDEF_TNF_UNCHANGED        0x06
#define W_NDEF_TNF_ANY_TYPE         0x08

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_HANDLE WNDEFGetNextMessage(
                  W_HANDLE hMessage);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API uint32_t WNDEFGetRecordNumber(
                  W_HANDLE hMessage );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_HANDLE WNDEFGetRecord(
                  W_HANDLE hMessage,
                  uint32_t nIndex );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFSetRecord(
                  W_HANDLE hMessage,
                  uint32_t nIndex,
                  W_HANDLE hRecord );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFInsertRecord(
                  W_HANDLE hMessage,
                  uint32_t nIndex,
                  W_HANDLE hRecord );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFRemoveRecord(
                  W_HANDLE hMessage,
                  uint32_t nIndex );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetEnclosedMessage(
                  W_HANDLE hRecord,
                  OPEN_NFC_BUF1_OW W_HANDLE* phMessage );

#if defined (LWRAP_UNICODE)
#define WNDEFCheckType                  LWRAP_WNDEFCheckType
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WNDEFCheckType(
                  W_HANDLE hRecord,
                  uint8_t nTNF,
                  const tchar* pTypeString);
#endif

#define W_NDEF_INFO_TNF                0
#define W_NDEF_INFO_TYPE_STRING_ASCII  1
#define W_NDEF_INFO_TYPE_STRING_PRINT  2
#define W_NDEF_INFO_IDENTIFIER_UTF8    3
#define W_NDEF_INFO_IDENTIFIER_PRINT   4
#define W_NDEF_INFO_PAYLOAD            5
#define W_NDEF_INFO_BUFFER             6

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetRecordInfo(
                  W_HANDLE hRecord,
                  uint32_t nInfoType,
                  OPEN_NFC_BUF1_OW uint32_t* pnValue);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetRecordInfoBuffer(
                  W_HANDLE hRecord,
                  uint32_t nInfoType,
                  OPEN_NFC_BUF1_O uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualLength);

#if defined (LWRAP_UNICODE)
#define WNDEFGetTypeString              LWRAP_WNDEFGetTypeString
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetTypeString(
                  W_HANDLE hRecord,
                  OPEN_NFC_BUF1_O tchar* pTypeStringBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nTypeStringBufferLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualLength );
#endif

#if defined (LWRAP_UNICODE )
#define WNDEFCheckIdentifier            LWRAP_WNDEFCheckIdentifier
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WNDEFCheckIdentifier(
                  W_HANDLE hRecord,
                  const tchar* pIdentifierString);
#endif

#if defined (LWRAP_UNICODE)
#define WNDEFGetIdentifierString        LWRAP_WNDEFGetIdentifierString
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetIdentifierString(
                  W_HANDLE hRecord,
                  OPEN_NFC_BUF1_O tchar* pIdentifierStringBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nIdentifierStringBufferLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualLength );
#endif

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetPayloadPointer(
                  W_HANDLE hRecord,
                  uint8_t** ppBuffer );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFCreateNewMessage(
                  OPEN_NFC_BUF1_OW W_HANDLE* phMessage );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API uint32_t WNDEFGetMessageLength(
                  W_HANDLE hMessage );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetMessageContent(
                  W_HANDLE hMessage,
                  OPEN_NFC_BUF1_O uint8_t* pMessageBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nMessageBufferLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualLength );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFBuildMessage(
                  OPEN_NFC_BUF1_I const uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phMessage);

#if defined (LWRAP_UNICODE)
#define WNDEFCheckRecordType           LWRAP_WNDEFCheckRecordType
#else
/**
 * @keyword  TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WNDEFCheckRecordType(
                  uint8_t nTNF,
                  const tchar* pTypeString);
#endif



#if defined (LWRAP_UNICODE)
#define WNDEFCompareRecordType           LWRAP_WNDEFCompareRecordType
#else
/**
 * @keyword  TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WNDEFCompareRecordType(
                  uint8_t nTNF1,
                  const tchar* pTypeString1,
                  uint8_t nTNF2,
                  const tchar* pTypeString2);
#endif

#if defined (LWRAP_UNICODE)
#define WNDEFCreateRecord           LWRAP_WNDEFCreateRecord
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFCreateRecord(
                  uint8_t nTNF,
                  const tchar* pTypeString,
                  OPEN_NFC_BUF1_I const uint8_t* pPayloadBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nPayloadLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phRecord );
#endif

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFBuildRecord(
                  OPEN_NFC_BUF1_I const uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phRecord);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFSetRecordInfo(
                  W_HANDLE hRecord,
                  uint32_t nInfoType,
                  OPEN_NFC_BUF1_I const uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength);

#if defined (LWRAP_UNICODE)
#define WNDEFSetIdentifierString           LWRAP_WNDEFSetIdentifierString
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFSetIdentifierString(
                  W_HANDLE hRecord,
                  const tchar* pIdentifierString);
#endif

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFAppendRecord(
                  W_HANDLE hMessage,
                  W_HANDLE hRecord);

#if defined (LWRAP_UNICODE)
#define WNDEFCreateNestedMessageRecord           LWRAP_WNDEFCreateNestedMessageRecord
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFCreateNestedMessageRecord(
                  uint8_t nTNF,
                  const tchar* pTypeString,
                  W_HANDLE hNestedMessage,
                  OPEN_NFC_BUF1_OW W_HANDLE* phRecord);
#endif

/*******************************************************************************
  NFC Tag Functions
*******************************************************************************/

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWNDEFReadMessageCompleted(
                  void* pCallbackParameter,
                  W_HANDLE hMessage,
                  W_ERROR nError );

#if defined (LWRAP_UNICODE)
#define WNDEFReadMessageOnAnyTag        LWRAP_WNDEFReadMessageOnAnyTag
#else
/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNDEFReadMessageOnAnyTag(
                  OPEN_NFC_USER_CALLBACK tWNDEFReadMessageCompleted* pCallback,
                  void* pCallbackParameter,
                  uint8_t nPriority,
                  uint8_t nTNF,
                  const tchar* pTypeString,
                  OPEN_NFC_BUF1_OW W_HANDLE* phRegistry);
#endif

#if defined (LWRAP_UNICODE)
#define WNDEFReadMessage               LWRAP_WNDEFReadMessage
#else
/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNDEFReadMessage(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWNDEFReadMessageCompleted* pCallback,
                  void* pCallbackParameter,
                  uint8_t nTNF,
                  const tchar* pTypeString,
                  OPEN_NFC_BUF1_OW W_HANDLE* phOperation);
#endif

#if defined (LWRAP_UNICODE)
#define WNDEFReadMessageSync            LWRAP_WNDEFReadMessageSync
#else
OPEN_NFC_CLIENT_API W_ERROR WNDEFReadMessageSync(
                  W_HANDLE hConnection,
                  uint8_t nTNF,
                  const tchar* pTypeString,
                  W_HANDLE* phMessage);
#endif

typedef struct
{
   uint8_t nTagType;
   uint8_t nSerialNumberLength;
   bool bIsLocked;
   bool bIsLockable;
   uint32_t nFreeSpaceSize;
   uint8_t aSerialNumber[10];
} tNDEFTagInfo;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNDEFGetTagInfo(
                  W_HANDLE hConnection,
                  tNDEFTagInfo* pTagInfo );

#define W_NDEF_ACTION_BIT_FORMAT_BLANK_TAG        0x01
#define W_NDEF_ACTION_BIT_FORMAT_NON_NDEF_TAG     0x02
#define W_NDEF_ACTION_BIT_FORMAT_ALL              0x03
#define W_NDEF_ACTION_BIT_ERASE                   0x04
#define W_NDEF_ACTION_BIT_CHECK_WRITE             0x08
#define W_NDEF_ACTION_BIT_LOCK                    0x10

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNDEFWriteMessageOnAnyTag(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t nPriority,
                  W_HANDLE hMessage,
                  uint32_t nActionMask,
                  OPEN_NFC_BUF1_OW W_HANDLE* phOperation);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNDEFWriteMessage(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  W_HANDLE hMessage,
                  uint32_t nActionMask,
                  OPEN_NFC_BUF1_OW W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WNDEFWriteMessageSync(
                  W_HANDLE hConnection,
                  W_HANDLE hMessage,
                  uint32_t nActionMask);

/*******************************************************************************
  RTD Functions
*******************************************************************************/

#define W_RTD_TEXT_MATCH_1             0x01
#define W_RTD_TEXT_PARTIAL_MATCH_1     0x02
#define W_RTD_TEXT_MATCH_2             0x03
#define W_RTD_TEXT_PARTIAL_MATCH_2     0x04
#define W_RTD_TEXT_NO_MATCH_FOUND      0x00

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WRTDIsTextRecord(
                  W_HANDLE hRecord );

#if defined (LWRAP_UNICODE )
#define WRTDTextGetLanguage             LWRAP_WRTDTextGetLanguage
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDTextGetLanguage(
                  W_HANDLE hRecord,
                  OPEN_NFC_BUF1_O tchar* pLanguageBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength );
#endif

#if defined (LWRAP_UNICODE)
#define WRTDTextLanguageMatch           LWRAP_WRTDTextLanguageMatch
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API uint8_t WRTDTextLanguageMatch(
                  W_HANDLE hRecord,
                  const tchar* pLanguage1,
                  const tchar* pLanguage2 );
#endif

#if defined (LWRAP_UNICODE)
#define WRTDTextFind                    LWRAP_WRTDTextFind
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDTextFind(
                  W_HANDLE hMessage,
                  const tchar* pLanguage1,
                  const tchar* pLanguage2,
                  W_HANDLE* phRecord,
                  uint8_t* pnMatch );
#endif

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API uint32_t WRTDTextGetLength(
                  W_HANDLE hRecord );

#if defined (LWRAP_UNICODE)
#define WRTDTextGetValue                LWRAP_WRTDTextGetValue
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDTextGetValue(
                  W_HANDLE hRecord,
                  OPEN_NFC_BUF1_O tchar* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength );
#endif

#if defined (LWRAP_UNICODE)
#define WRTDTextAddRecord               LWRAP_WRTDTextAddRecord
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDTextAddRecord(
                  W_HANDLE hMessage,
                  const tchar* pLanguage,
                  bool bUseUtf8,
                  const tchar* pText,
                  uint32_t nTextLength );
#endif

#if defined (LWRAP_UNICODE)
#define WRTDTextCreateRecord               LWRAP_WRTDTextCreateRecord
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDTextCreateRecord(
                  const tchar* pLanguage,
                  bool bUseUtf8,
                  const tchar* pText,
                  uint32_t nTextLength,
                  OPEN_NFC_BUF1_OW W_HANDLE* phRecord );
#endif

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WRTDIsURIRecord(
                  W_HANDLE hRecord );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API uint32_t WRTDURIGetLength(
                  W_HANDLE hRecord );

#if defined (LWRAP_UNICODE)
#define WRTDURIGetValue                 LWRAP_WRTDURIGetValue
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDURIGetValue(
                  W_HANDLE hRecord,
                  OPEN_NFC_BUF1_O tchar* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength );
#endif

#if defined (LWRAP_UNICODE)
#define WRTDURIAddRecord                LWRAP_WRTDURIAddRecord
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDURIAddRecord(
                  W_HANDLE hMessage,
                  const tchar* pURI );
#endif

#if defined (LWRAP_UNICODE)
#define WRTDURICreateRecord                LWRAP_WRTDURICreateRecord
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WRTDURICreateRecord(
                  const tchar* pURI,
                  OPEN_NFC_BUF1_OW W_HANDLE* phRecord );
#endif

/*******************************************************************************
  Tag simulation Functions
*******************************************************************************/

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WVirtualTagCreate(
                  uint8_t nTagType,
                  OPEN_NFC_BUF1_I const uint8_t* pIdentifier,
                  OPEN_NFC_BUF1_LENGTH uint32_t nIdentifierLength,
                  uint32_t nMaximumMessageLength,
                  OPEN_NFC_BUF2_OW W_HANDLE* phVirtualTag );

#define W_VIRTUAL_TAG_EVENT_SELECTION           0
#define W_VIRTUAL_TAG_EVENT_READER_LEFT         1
#define W_VIRTUAL_TAG_EVENT_READER_READ_ONLY    2
#define W_VIRTUAL_TAG_EVENT_READER_WRITE        3

/**
 * @keyword  JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WVirtualTagStart(
                  W_HANDLE hVirtualTag,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCompletionCallback,
                  void* pCompletionCallbackParameter,
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler* pEventCallback,
                  void* pEventCallbackParameter,
                  bool bReadOnly );

OPEN_NFC_CLIENT_API W_ERROR WVirtualTagStartSync(
                  W_HANDLE hVirtualTag,
                  tWBasicGenericEventHandler* pEventCallback,
                  void* pEventCallbackParameter,
                  bool bReadOnly );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WVirtualTagStop(
                  W_HANDLE hVirtualTag,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCompletionCallback,
                  void* pCallbackParameter );

OPEN_NFC_CLIENT_API W_ERROR WVirtualTagStopSync(
                  W_HANDLE hVirtualTag );

/*******************************************************************************
  Card Emulation Functions
*******************************************************************************/

#define W_EMUL_LLCP_PARAMETER_MAX_LENGTH           128

typedef struct
{
   uint8_t nSAK;
   uint16_t nATQA;
   uint8_t nFWI_SFGI;
   uint8_t nDataRateMax;
   bool bSetCIDSupport;
   uint8_t nCID;
   uint8_t nNAD;
   uint8_t nUIDLength;
   uint8_t UID[10];
   uint32_t nApplicationDataLength;
   uint8_t aApplicationData[W_EMUL_APPLICATION_DATA_MAX_LENGTH];
} tWEmul14AConnectionInfo;

typedef struct
{
   uint8_t nAFI;
   uint32_t nATQB;
   bool bSetCIDSupport;
   uint8_t nCID;
   uint8_t nNAD;
   uint8_t nPUPILength;
   uint8_t PUPI[4];
   uint32_t nHigherLayerResponseLength;
   uint8_t aHigherLayerResponse[W_EMUL_HIGHER_LAYER_RESPONSE_MAX_LENGTH];
} tWEmul14BConnectionInfo;

typedef struct
{
   uint8_t nTimeOutRTX;
   uint8_t nLLCPParameterSize;
   uint8_t aLLCPParameter[W_EMUL_LLCP_PARAMETER_MAX_LENGTH];

} tWEmulP2PTargetConnectionInfo;

typedef struct
{
   uint8_t nCardType;

   union
   {
      tWEmul14AConnectionInfo s14A;
      tWEmul14BConnectionInfo s14B;
      tWEmulP2PTargetConnectionInfo sP2PTarget;

   } sCardInfo;
} tWEmulConnectionInfo;

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API bool WEmulIsPropertySupported(
                  uint8_t nPropertyIdentifier );

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWEmulCommandReceived(
                  void* pCallbackParameter,
                  uint32_t nDataLength );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WEmulGetMessageData(
                  W_HANDLE hHandle,
                  OPEN_NFC_BUF1_O uint8_t* pDataBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nDataLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualDataLength );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WEmulSendAnswer(
                  W_HANDLE hDriverConnection,
                  OPEN_NFC_BUF1_I const uint8_t* pDataBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nDataLength );

#define W_EMUL_EVENT_SELECTION      2
#define W_EMUL_EVENT_DEACTIVATE     3

/**
 * @keyword  TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WEmulOpenConnection(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pOpenCallback,
                  void* pOpenCallbackParameter,
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler* pEventCallback,
                  void* pEventCallbackParameter,
                  OPEN_NFC_USER_EVENT_HANDLER tWEmulCommandReceived* pCommandCallback,
                  void* pCommandCallbackParameter,
                  tWEmulConnectionInfo* pEmulConnectionInfo,
                  OPEN_NFC_BUF1_OW W_HANDLE* phHandle);

/**
 * @keyword  P_FUNCTION, DRIVER_API
 **/
void WEmulOpenConnectionDriver1(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pOpenCallback,
                  void* pOpenCallbackParameter,
                  OPEN_NFC_BUF1_I const tWEmulConnectionInfo* pEmulConnectionInfo,
                  OPEN_NFC_BUF1_LENGTH uint32_t nSize,
                  OPEN_NFC_BUF2_OW W_HANDLE* phHandle);

typedef void tWEmulDriverCommandReceived(
                  void* pCallbackParameter,
                  uint32_t nDataLength );

typedef void tWEmulDriverEventReceived(
                  void *pCallbackParameter,
                  uint32_t nEventCode );

/**
 * @keyword  P_FUNCTION, DRIVER_API
 **/
void WEmulOpenConnectionDriver2(
                  W_HANDLE hHandle,
                  OPEN_NFC_USER_EVENT_HANDLER tWEmulDriverEventReceived* pEventCallback,
                  void* pEventCallbackParameter );

/**
 * @keyword  P_FUNCTION, DRIVER_API
 **/
void WEmulOpenConnectionDriver3(
                  W_HANDLE hHandle,
                  OPEN_NFC_USER_EVENT_HANDLER tWEmulDriverCommandReceived* pCommandCallback,
                  void* pCommandCallbackParameter );

OPEN_NFC_CLIENT_API W_ERROR WEmulOpenConnectionSync(
                  tWBasicGenericEventHandler* pEventCallback,
                  void* pEventCallbackParameter,
                  tWEmulCommandReceived* pCommandCallback,
                  void* pCommandCallbackParameter,
                  tWEmulConnectionInfo* pEmulConnectionInfo,
                  W_HANDLE* phHandle );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WEmulClose(
                  W_HANDLE hHandle,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WEmulCloseSync(
                  W_HANDLE hHandle);

/*******************************************************************************
  Secure Element Functions
*******************************************************************************/

#define W_SE_FLAG_COMMUNICATION                    0x01
#define W_SE_FLAG_END_OF_TRANSACTION_NOTIFICATION  0x02
#define W_SE_FLAG_REMOVABLE                        0x04
#define W_SE_FLAG_HOT_PLUG                         0x08

#define W_SE_DESCRIPTION_LENGTH  32

typedef struct
{
   uint32_t nSlotIdentifier;
   tchar aDescription[W_SE_DESCRIPTION_LENGTH];
   uint32_t nSupportedProtocols;
   bool bIsPresent;
   uint32_t nCapabilities;
   uint32_t nVolatilePolicy;
   uint32_t nPersistentPolicy;

} tSEInfo;


/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WSEGetInfo(
                  uint32_t nSlotIdentifier,
                  tSEInfo* pSEInfo );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WSEOpenConnection(
                  uint32_t nSlotIdentifier,
                  bool bForce,
                  tWBasicGenericHandleCallbackFunction* pCallback,
                  void* pCallbackParameter );

OPEN_NFC_CLIENT_API W_ERROR WSEOpenConnectionSync(
                  uint32_t nSlotIdentifier,
                  bool bForce,
                  W_HANDLE* phConnection);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WSEMonitorEndOfTransaction(
                  uint32_t nSlotIdentifier,
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler2* pHandler,
                  void* pHandlerParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);

#define W_SE_EVENT_INSERTED   1
#define W_SE_EVENT_REMOVED   0

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WSEMonitorHotPlugEvents(
                  uint32_t nSlotIdentifier,
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler2* pHandler,
                  void* pHandlerParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API uint32_t WSEGetTransactionAID(
                  uint32_t nSlotIdentifier,
                  OPEN_NFC_BUF1_O uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WSESetPolicy(
                  uint32_t nSlotIdentifier,
                  uint32_t nStorageType,
                  uint32_t nProtocols,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WSESetPolicySync(
                  uint32_t nSlotIdentifier,
                  uint32_t nStorageType,
                  uint32_t nProtocols);

/*******************************************************************************
  UICC Functions
*******************************************************************************/

#define  W_UICC_LINK_STATUS_NO_CONNECTION       0
#define  W_UICC_LINK_STATUS_NO_UICC             1
#define  W_UICC_LINK_STATUS_ERROR               2
#define  W_UICC_LINK_STATUS_INITIALIZATION      3
#define  W_UICC_LINK_STATUS_ACTIVE              4

typedef void tWUICCGetSlotInfoCompleted(
                  void* pCallbackParameter,
                  uint32_t nSWPLinkStatus,
                  W_ERROR nError );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WUICCGetSlotInfo(
                  OPEN_NFC_USER_CALLBACK tWUICCGetSlotInfoCompleted* pCallback,
                  void* pCallbackParameter );

OPEN_NFC_CLIENT_API W_ERROR WUICCGetSlotInfoSync(
                  uint32_t* pnSWPLinkStatus);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WUICCActivateSWPLine(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WUICCActivateSWPLineSync( void );

typedef void tWUICCMonitorTransactionEventHandler(
                  void* pHandlerParameter );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WUICCMonitorTransactionEvent(
                  OPEN_NFC_USER_EVENT_HANDLER tWUICCMonitorTransactionEventHandler* pHandler,
                  void* pHandlerParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API uint32_t WUICCGetTransactionEventAID(
                  OPEN_NFC_BUF1_O uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength);

typedef void tWUICCMonitorConnectivityEventHandler(
                  void* pHandlerParameter,
                  uint8_t nMessageCode,
                  uint32_t nDataLength );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WUICCGetConnectivityEventParameter(
                  OPEN_NFC_BUF1_O uint8_t* pDataBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualDataLength);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WUICCMonitorConnectivityEvent(
                  OPEN_NFC_USER_EVENT_HANDLER tWUICCMonitorConnectivityEventHandler* pHandler,
                  void* pHandlerParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);


typedef struct
{
   uint8_t nReaderISO14443_4_A_Priority;
   uint8_t nReaderISO14443_4_B_Priority;
   uint8_t nCardISO14443_4_A_Priority;
   uint8_t nCardISO14443_4_B_Priority;
   uint8_t nCardBPrime_Priority;
   uint8_t nCardFeliCa_Priority;

} tWUICCAccessPolicy;

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WUICCSetAccessPolicy(
                  uint32_t nStorageType,
                  const tWUICCAccessPolicy* pAccessPolicy,
                  tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WUICCSetAccessPolicySync(
                  uint32_t nStorageType,
                  const tWUICCAccessPolicy* pAccessPolicy);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WUICCGetAccessPolicy(
                  uint32_t nStorageType,
                  tWUICCAccessPolicy* pAccessPolicy);

/*******************************************************************************
  NFC Controller Functions
*******************************************************************************/

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerReset(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint32_t nMode );

#if defined (LWRAP_UNICODE)
#define WNFCControllerGetFirmwareProperty               LWRAP_WNFCControllerGetFirmwareProperty
#else
/**
 * @keyword  JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerGetFirmwareProperty(
                  OPEN_NFC_BUF1_I const uint8_t* pUpdateBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nUpdateBufferLength,
                  uint8_t nPropertyIdentifier,
                  OPEN_NFC_BUF2_O tchar* pValueBuffer,
                  OPEN_NFC_BUF2_LENGTH uint32_t nBufferLength,
                  OPEN_NFC_BUF3_OW uint32_t* pnValueLength );
#endif

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerFirmwareUpdate(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pUpdateBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nUpdateBufferLength,
                  uint32_t nMode );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API uint32_t WNFCControllerFirmwareUpdateState( void );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API uint32_t WNFCControllerGetMode( void );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerMonitorException(
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler* pHandler,
                  void* pHandlerParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);

/**
 * @keyword  JNI(org.opennfc.MethodAutogen)
 **/
typedef void tWNFCControllerSelfTestCompleted(
                  void* pCallbackParameter,
                  W_ERROR nError,
                  uint32_t nResult );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerSelfTest(
                  OPEN_NFC_USER_CALLBACK tWNFCControllerSelfTestCompleted* pCallback,
                  void* pCallbackParameter );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerProductionTest(
                  OPEN_NFC_BUF1_IA const uint8_t* pParameterBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nParameterBufferLength,
                  OPEN_NFC_BUF2_OA uint8_t* pResultBuffer,
                  OPEN_NFC_BUF2_LENGTH uint32_t nResultBufferLength,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter);

#define  W_NFCC_STORAGE_VOLATILE    1
#define  W_NFCC_STORAGE_PERSISTENT  2
#define  W_NFCC_STORAGE_BOTH        (W_NFCC_STORAGE_VOLATILE | W_NFCC_STORAGE_PERSISTENT)

#define W_NFCC_PROTOCOL_CARD_ISO_14443_4_A      0x00000001
#define W_NFCC_PROTOCOL_CARD_ISO_14443_4_B      0x00000002
#define W_NFCC_PROTOCOL_CARD_ISO_14443_3_A      0x00000004
#define W_NFCC_PROTOCOL_CARD_ISO_14443_3_B      0x00000008
#define W_NFCC_PROTOCOL_CARD_ISO_15693_3        0x00000010
#define W_NFCC_PROTOCOL_CARD_ISO_15693_2        0x00000020
#define W_NFCC_PROTOCOL_CARD_FELICA             0x00000040
#define W_NFCC_PROTOCOL_CARD_P2P_TARGET         0x00000080
#define W_NFCC_PROTOCOL_CARD_TYPE_1_CHIP        0x00000100
#define W_NFCC_PROTOCOL_CARD_MIFARE_CLASSIC     0x00000200
#define W_NFCC_PROTOCOL_CARD_BPRIME             0x00000400


#define W_NFCC_PROTOCOL_READER_ISO_14443_4_A    0x00010000
#define W_NFCC_PROTOCOL_READER_ISO_14443_4_B    0x00020000
#define W_NFCC_PROTOCOL_READER_ISO_14443_3_A    0x00040000
#define W_NFCC_PROTOCOL_READER_ISO_14443_3_B    0x00080000
#define W_NFCC_PROTOCOL_READER_ISO_15693_3      0x00100000
#define W_NFCC_PROTOCOL_READER_ISO_15693_2      0x00200000
#define W_NFCC_PROTOCOL_READER_FELICA           0x00400000
#define W_NFCC_PROTOCOL_READER_P2P_INITIATOR    0x00800000
#define W_NFCC_PROTOCOL_READER_TYPE_1_CHIP      0x01000000
#define W_NFCC_PROTOCOL_READER_MIFARE_CLASSIC   0x02000000
#define W_NFCC_PROTOCOL_READER_BPRIME           0x04000000

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerSetRFLock(
                  uint32_t nLockSet,
                  bool bReaderLock,
                  bool bCardLock,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WNFCControllerSetRFLockSync(
                  uint32_t nLockSet,
                  bool bReaderLock,
                  bool bCardLock );

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerGetRFLock(
                  uint32_t nLockSet,
                  OPEN_NFC_BUF1_OW bool* pbReaderLock,
                  OPEN_NFC_BUF2_OW bool* pbCardLock );

#if defined (LWRAP_UNICODE)
#define WNFCControllerGetProperty               LWRAP_WNFCControllerGetProperty
#else
/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR, THREAD_SAFE
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerGetProperty(
                  uint8_t nPropertyIdentifier,
                  OPEN_NFC_BUF1_O tchar* pValueBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnValueLength );
#endif

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerGetBooleanProperty(
                  uint8_t nPropertyIdentifier,
                  bool* pbValue );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerGetIntegerProperty(
                  uint8_t nPropertyIdentifier,
                  uint32_t* pnValue );

#define W_NFCC_RF_ACTIVITY_INACTIVE            0x00
#define W_NFCC_RF_ACTIVITY_DETECTION           0x01
#define W_NFCC_RF_ACTIVITY_ACTIVE              0x02

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerGetRFActivity(
                  OPEN_NFC_BUF1_OW uint8_t* pnReaderState,
                  OPEN_NFC_BUF2_OW uint8_t* pnCardState,
                  OPEN_NFC_BUF3_OW uint8_t* pnP2PState);


#define W_NFCC_EVENT_FIELD_ON   1
#define W_NFCC_EVENT_FIELD_OFF   0

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerMonitorFieldEvents(
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler* pHandler,
                  void* pHandlerParameter,
                  OPEN_NFC_BUF1_OW W_HANDLE* phEventRegistry);
/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerSwitchStandbyMode(
                  bool bStandbyOn );

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerSwitchToRawMode(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerRegisterRawListener(
                  OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericDataCallbackFunction* pReceiveMessageEventHandler,
                  void* pHandlerParameter);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WNFCControllerWriteRawMessage(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_I const uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength);
/**
 * @keyword  P_FUNCTION, TEST_VECTOR, DRIVER_API
 **/
OPEN_NFC_CLIENT_API W_ERROR WNFCControllerGetRawMessageData(
                  OPEN_NFC_BUF1_O uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nBufferLength,
                  OPEN_NFC_BUF2_OW uint32_t* pnActualLength);

/*******************************************************************************
  Bluetooth Pairing Functions
*******************************************************************************/
#define W_BT_PAIRING_TYPE_TAG 0
#define W_BT_PAIRING_TYPE_READER 1
#define W_BT_PAIRING_TYPE_P2P 2
#define W_BT_PAIRING_TYPE_P2P_SLAVE 3
#define W_BT_PAIRING_TYPE_P2P_MASTER 4

#define W_BT_PAIRING_AUTH_PIN 0


typedef struct
{
   uint8_t aBD_ADDR[6];
   uint8_t aBTDeviceClass[3];
   uint32_t nAuthenticationDataType;
   uint32_t nAuthenticationDataSize;
   uint8_t aAuthenticationData[16];

} tWBTPairingInfo;

typedef void tWBTPairingStartCompleted(
                  void* pCallbackParameter,
                  uint32_t nRemoteDeviceNameLength,
                  bool bSetBTMasterMode,
                  W_ERROR nError);

#if defined (LWRAP_UNICODE)
#define WBTPairingStart                      LWRAP_WBTPairingStart
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WBTPairingStart(
                  uint32_t nMode,
                  const tWBTPairingInfo* pLocalInfo,
                  const tchar* pLocalDeviceName,
                  tWBTPairingStartCompleted* pCallback,
                  void* pCallbackParameter,
                  W_HANDLE* phOperation);

#endif

#if defined (LWRAP_UNICODE)
#define WBTPairingGetRemoteDeviceInfo          LWRAP_WBTPairingGetRemoteDeviceInfo
#else
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WBTPairingGetRemoteDeviceInfo(
                  W_HANDLE hOperation,
                  tWBTPairingInfo* pRemoteInfo,
                  tchar* pRemoteDeviceNameBuffer,
                  uint32_t nRemoteDeviceNameBufferLength);

#endif

/*******************************************************************************
  Wi-Fi Pairing Functions
*******************************************************************************/

#define W_WIFI_PAIRING_PASSWORD_TOKEN  0
#define W_WIFI_PAIRING_CONFIGURATION_TOKEN 1
#define W_WIFI_PAIRING_TOUCHING_DEVICE  2


typedef struct
{
   uint8_t aPublicKeyHashData[20];
   uint16_t nPasswordId;

} tWWIFIOOBDevicePasswordInfo;

typedef void tWWIFIEnrolleePairingStartCompleted(
                  void*    pCallbackParameter,
                  uint8_t  nMode,
                  uint32_t nCredentialLength,
                  uint32_t nDevicePasswordLength,
                  W_ERROR  nError);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WWIFIEnrolleePairingStart(
                  const uint8_t* pMessageM1,
                  uint32_t nMessageM1Length,
                  tWWIFIEnrolleePairingStartCompleted *pCallback,
                  void *pCallbackParameter,
                  W_HANDLE *phOperation);

   /* In Linux Wrapper source file; do NOT change any original definition */
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WWIFIGetOOBDevicePasswordInfo(
                  W_HANDLE hOperation,
                  tWWIFIOOBDevicePasswordInfo *pOOBDevicePasswordInfo,
                  uint8_t *pVersion,
                  uint8_t *pDevicePasswordBuffer,
                  uint32_t nDevicePasswordLength);

   /* In Linux Wrapper source file; do NOT change any original definition */
/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WWIFIGetCredentialInfo(
                  W_HANDLE hOperation,
                  uint8_t *pVersion,
                  uint8_t *pCredentialBuffer,
                  uint32_t nCredentialLength);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WWIFIPairingGetMessageM(
                  W_HANDLE hOperation,
                  uint8_t *pMessageMBuffer,
                  uint32_t nMessageMLength);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API uint32_t WWIFIPairingGetMessageMLength(
                  W_HANDLE hOperation);

/**
 * @keyword  TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WWIFIAccessPointPairingStart(
                  tWBasicGenericCallbackFunction *pCallback,
                  void *pCallbackParameter,
                  tWBasicGenericEventHandler *pM1EventReadCallback,
                  void *pM1EventReadCallbackParameter,
                  W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WWIFIAccessPointPairingStartSync(
                  tWBasicGenericEventHandler *pM1EventReadCallback,
                  void *pM1EventReadCallbackParameter);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WWIFIPairingWriteMessageM2(
                  W_HANDLE hOperation,
                  const uint8_t * pMessageM2Buffer,
                  uint32_t nMessageM2Length);

/**
 * @keyword  P_FUNCTION, TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WWIFIAccessPointConfigureToken(
                  uint8_t nVersion,
                  const uint8_t* pCredentialBuffer,
                  uint32_t nCredentialLength,
                  tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  W_HANDLE* phOperation);

OPEN_NFC_CLIENT_API W_ERROR WWIFIAccessPointConfigureTokenSync(
                  uint8_t nVersion,
                  const uint8_t* pCredentialBuffer,
                  uint32_t nCredentialLength);

/**
 * @keyword  TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API void WWIFIInfoPairingTransmit(
                  tWBasicGenericCallbackFunction *pCallback,
                  void *pCallbackParameter,
                  tWBasicGenericEventHandler *pEventWriteCallback,
                  void *pEventWriteCallbackParameter,
                  W_HANDLE *phOperation);

OPEN_NFC_CLIENT_API W_ERROR WWIFIInfoPairingTransmitSync(
                  tWBasicGenericEventHandler *pEventWriteCallback,
                  void *pEventWriteCallbackParameter);

/*******************************************************************************
  Helper Functions fo the J-Edition
*******************************************************************************/

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 **/
typedef void tWJedSendEvent(
                  void* pCallbackParameter,
                  uint32_t nIsolate,
                  uint32_t nEventCode,
                  uint32_t nParam2,
                  uint32_t nParam3,
                  uint32_t nParam4);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedInitialize(
                  uint32_t nKeepAliveConnectionTimeout,
                  uint32_t nPushRegistryTimeout,
                  uint8_t nReaderPriority,
                  tWJedSendEvent* pSendEventFunction,
                  void* pCallbackParameter );

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WJedTerminate(void);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedGetNFCCProperty(
                  const tchar* pPropertyName,
                  tchar* pPropertyValueBuffer,
                  uint32_t nPropertyValueBufferLength,
                  uint32_t* pnPropertyValueLength);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_HANDLE WJedListenToCardDetection(
                  uint32_t nIsolate,
                  uint32_t nListenerId,
                  const uint8_t* pProperties,
                  uint32_t nPropertyNumber);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedReadOnAnyTag(
                  uint32_t nIsolate,
                  bool bRegister,
                  uint32_t nListenerId,
                  uint8_t nTNF,
                  const tchar* pTypeString);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedAddHandle(
                  W_HANDLE hHandle,
                  uint32_t nIsolate);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedCheckHandle(
                  W_HANDLE hHandle,
                  uint32_t* pnIsolate);

#define WJED_UICC_SLOT    0
#define WJED_SE_SLOT      1

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedMonitorEndOfTransaction(
                  uint32_t nIsolate,
                  bool bRegister,
                  uint32_t nSlotId);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 **/
typedef void tWJedPushRegistryEventHandler(
                  void* pHandlerParameter,
                  uint32_t nApplicationIdentifier);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedPushRegistryAddEntry(
                  tWJedPushRegistryEventHandler* pEventHandler,
                  void* pHandlerParameter,
                  uint32_t nApplicationIdentifier,
                  const uint8_t* pURLString);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedPushRegistryCheckEntry(
                  const uint8_t* pURLString);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedPushRegistryRemoveEntry(
                  uint32_t nApplicationIdentifier,
                  const uint8_t* pURLString);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedGetInfo(
                  W_HANDLE hHandle,
                  uint32_t nInfoType,
                  uint32_t* pnValue);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedGetInfoBuffer(
                  W_HANDLE hHandle,
                  uint32_t nInfoType,
                  uint8_t* pBuffer,
                  uint32_t nBufferLength,
                  uint32_t* pnActualLength);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedRetrieveConnection(
                  W_HANDLE hConnection);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WJedCloseHandle(
                  W_HANDLE hHandle);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WJedReaderHandlerWorkPerformed(
                  W_HANDLE hConnection,
                  bool bGiveToNextListener,
                  bool bCardApplicationMatch );

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WJedReleaseApplication(
                  uint32_t nIsolate);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_HANDLE WJedCreateBufferObject(
                  uint32_t nIsolate,
                  uint32_t nParameter,
                  uint32_t nLength,
                  uint8_t** ppContent);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedGetBufferContent(
                  W_HANDLE hBufferObject,
                  uint32_t* pnIsolate,
                  uint32_t* pnParameter,
                  uint8_t** ppContent,
                  uint32_t* pnLength);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WJed14Part3ExchangeData(
                  W_HANDLE hConnection,
                  tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  const uint8_t* pReaderToCardBuffer,
                  uint32_t nReaderToCardBufferLength,
                  uint8_t* pCardToReaderBuffer,
                  uint32_t nCardToReaderBufferMaxLength,
                  W_HANDLE* phOpeartion,
                  bool bCheckResponseCRC);

/**
 * @ifdef P_INCLUDE_J_EDITION_HELPER
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API W_ERROR WJedGetP2PPushSocket(
                  const tchar* pPushEntry,
                  W_HANDLE* phSocket);

/*******************************************************************************
  Helper Functions for the Jupiter SE
*******************************************************************************/

/**
 * @ifdef P_INCLUDE_JUPITER_SE
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WJupiterSEApplyPolicy(
                  uint32_t nSlotIdentifier,
                  tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter);

OPEN_NFC_CLIENT_API W_ERROR WJupiterSEApplyPolicySync(
                  uint32_t nSlotIdentifier);

/**
 * @ifdef P_INCLUDE_JUPITER_SE
 *
 * @keyword  P_FUNCTION
 **/
OPEN_NFC_CLIENT_API void WJupiterSEGetAID(
                  uint32_t nSlotIdentifier,
                  tWBasicGenericDataCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t* pBuffer,
                  uint32_t nBufferLength);

OPEN_NFC_CLIENT_API W_ERROR WJupiterSEGetAIDSync(
                  uint32_t nSlotIdentifier,
                  uint8_t* pBuffer,
                  uint32_t nBufferLength,
                  uint32_t* pnActualBufferLength);

/*******************************************************************************
  Security Function
*******************************************************************************/

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen), TEST_VECTOR
 **/
OPEN_NFC_CLIENT_API W_ERROR WSecurityAuthenticate(
         OPEN_NFC_BUF1_I const uint8_t* pApplicationDataBuffer,
         OPEN_NFC_BUF1_LENGTH uint32_t nApplicationDataBufferLength );

/*******************************************************************************
  Linux Unicode Wrapper functions
*******************************************************************************/

#if defined (LWRAP_UNICODE)
#include "open_nfc_wrap.h"   /* Linux Unicode Wrapper functions */
#endif

#endif /* #ifndef P_OPEN_NFC_CONSTANTS_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* __OPEN_NFC_H */
