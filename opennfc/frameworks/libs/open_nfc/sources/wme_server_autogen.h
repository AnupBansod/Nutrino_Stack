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

/*******************************************************************************
 File auto-generated with the autogen.exe tool - Do not modify manually
 The autogen.exe binary tool, the generation scripts and the files used
 for the source of the generation are available under Apache License, Version 2.0
 ******************************************************************************/

#ifndef __WME_SERVER_AUTOGEN_H
#define __WME_SERVER_AUTOGEN_H

#ifdef P_CONFIG_CLIENT_SERVER

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_DRIVER)

/* -----------------------------------------------------------------------------
      P14P3DriverExchangeData()
----------------------------------------------------------------------------- */

#define P_Idenfier_P14P3DriverExchangeData 0

typedef struct __tMessage_in_P14P3DriverExchangeData
{
   W_HANDLE hDriverConnection;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
   bool bCheckResponseCRC;
} tMessage_in_P14P3DriverExchangeData;

typedef struct __tMessage_out_P14P3DriverExchangeData
{
   W_HANDLE value;
} tMessage_out_P14P3DriverExchangeData;

typedef union __tMessage_in_out_P14P3DriverExchangeData
{
   tMessage_in_P14P3DriverExchangeData in;
   tMessage_out_P14P3DriverExchangeData out;
} tMessage_in_out_P14P3DriverExchangeData;

/* -----------------------------------------------------------------------------
      P14P3DriverSetTimeout()
----------------------------------------------------------------------------- */

#define P_Idenfier_P14P3DriverSetTimeout 1

typedef struct __tMessage_in_P14P3DriverSetTimeout
{
   W_HANDLE hConnection;
   uint32_t nTimeout;
} tMessage_in_P14P3DriverSetTimeout;

typedef struct __tMessage_out_P14P3DriverSetTimeout
{
   W_ERROR value;
} tMessage_out_P14P3DriverSetTimeout;

typedef union __tMessage_in_out_P14P3DriverSetTimeout
{
   tMessage_in_P14P3DriverSetTimeout in;
   tMessage_out_P14P3DriverSetTimeout out;
} tMessage_in_out_P14P3DriverSetTimeout;

/* -----------------------------------------------------------------------------
      P14P4DriverExchangeData()
----------------------------------------------------------------------------- */

#define P_Idenfier_P14P4DriverExchangeData 2

typedef struct __tMessage_in_P14P4DriverExchangeData
{
   W_HANDLE hDriverConnection;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
   bool bSendNAD;
   uint8_t nNAD;
} tMessage_in_P14P4DriverExchangeData;

typedef struct __tMessage_out_P14P4DriverExchangeData
{
   W_HANDLE value;
} tMessage_out_P14P4DriverExchangeData;

typedef union __tMessage_in_out_P14P4DriverExchangeData
{
   tMessage_in_P14P4DriverExchangeData in;
   tMessage_out_P14P4DriverExchangeData out;
} tMessage_in_out_P14P4DriverExchangeData;

/* -----------------------------------------------------------------------------
      P14P4DriverSetTimeout()
----------------------------------------------------------------------------- */

#define P_Idenfier_P14P4DriverSetTimeout 3

typedef struct __tMessage_in_P14P4DriverSetTimeout
{
   W_HANDLE hConnection;
   uint32_t nTimeout;
} tMessage_in_P14P4DriverSetTimeout;

typedef struct __tMessage_out_P14P4DriverSetTimeout
{
   W_ERROR value;
} tMessage_out_P14P4DriverSetTimeout;

typedef union __tMessage_in_out_P14P4DriverSetTimeout
{
   tMessage_in_P14P4DriverSetTimeout in;
   tMessage_out_P14P4DriverSetTimeout out;
} tMessage_in_out_P14P4DriverSetTimeout;

/* -----------------------------------------------------------------------------
      P15P3DriverExchangeData()
----------------------------------------------------------------------------- */

#define P_Idenfier_P15P3DriverExchangeData 4

typedef struct __tMessage_in_P15P3DriverExchangeData
{
   W_HANDLE hConnection;
   tP15P3DriverExchangeDataCompleted * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
} tMessage_in_P15P3DriverExchangeData;

typedef union __tMessage_in_out_P15P3DriverExchangeData
{
   tMessage_in_P15P3DriverExchangeData in;
} tMessage_in_out_P15P3DriverExchangeData;

/* -----------------------------------------------------------------------------
      P15P3DriverSetTimeout()
----------------------------------------------------------------------------- */

#define P_Idenfier_P15P3DriverSetTimeout 5

typedef struct __tMessage_in_P15P3DriverSetTimeout
{
   W_HANDLE hConnection;
   uint32_t nTimeout;
} tMessage_in_P15P3DriverSetTimeout;

typedef struct __tMessage_out_P15P3DriverSetTimeout
{
   W_ERROR value;
} tMessage_out_P15P3DriverSetTimeout;

typedef union __tMessage_in_out_P15P3DriverSetTimeout
{
   tMessage_in_P15P3DriverSetTimeout in;
   tMessage_out_P15P3DriverSetTimeout out;
} tMessage_in_out_P15P3DriverSetTimeout;

/* -----------------------------------------------------------------------------
      PBasicDriverCancelOperation()
----------------------------------------------------------------------------- */

#define P_Idenfier_PBasicDriverCancelOperation 6

typedef struct __tMessage_in_PBasicDriverCancelOperation
{
   W_HANDLE hOperation;
} tMessage_in_PBasicDriverCancelOperation;

typedef union __tMessage_in_out_PBasicDriverCancelOperation
{
   tMessage_in_PBasicDriverCancelOperation in;
} tMessage_in_out_PBasicDriverCancelOperation;

/* -----------------------------------------------------------------------------
      PBasicDriverGetVersion()
----------------------------------------------------------------------------- */

#define P_Idenfier_PBasicDriverGetVersion 7

typedef struct __tMessage_in_PBasicDriverGetVersion
{
   void * pBuffer;
   uint32_t nBufferSize;
} tMessage_in_PBasicDriverGetVersion;

typedef struct __tMessage_out_PBasicDriverGetVersion
{
   W_ERROR value;
} tMessage_out_PBasicDriverGetVersion;

typedef union __tMessage_in_out_PBasicDriverGetVersion
{
   tMessage_in_PBasicDriverGetVersion in;
   tMessage_out_PBasicDriverGetVersion out;
} tMessage_in_out_PBasicDriverGetVersion;

/* -----------------------------------------------------------------------------
      PBPrimeDriverExchangeData()
----------------------------------------------------------------------------- */

#define P_Idenfier_PBPrimeDriverExchangeData 8

typedef struct __tMessage_in_PBPrimeDriverExchangeData
{
   W_HANDLE hDriverConnection;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
} tMessage_in_PBPrimeDriverExchangeData;

typedef struct __tMessage_out_PBPrimeDriverExchangeData
{
   W_HANDLE value;
} tMessage_out_PBPrimeDriverExchangeData;

typedef union __tMessage_in_out_PBPrimeDriverExchangeData
{
   tMessage_in_PBPrimeDriverExchangeData in;
   tMessage_out_PBPrimeDriverExchangeData out;
} tMessage_in_out_PBPrimeDriverExchangeData;

/* -----------------------------------------------------------------------------
      PBPrimeDriverSetTimeout()
----------------------------------------------------------------------------- */

#define P_Idenfier_PBPrimeDriverSetTimeout 9

typedef struct __tMessage_in_PBPrimeDriverSetTimeout
{
   W_HANDLE hConnection;
   uint32_t nTimeout;
} tMessage_in_PBPrimeDriverSetTimeout;

typedef struct __tMessage_out_PBPrimeDriverSetTimeout
{
   W_ERROR value;
} tMessage_out_PBPrimeDriverSetTimeout;

typedef union __tMessage_in_out_PBPrimeDriverSetTimeout
{
   tMessage_in_PBPrimeDriverSetTimeout in;
   tMessage_out_PBPrimeDriverSetTimeout out;
} tMessage_in_out_PBPrimeDriverSetTimeout;

/* -----------------------------------------------------------------------------
      PCacheConnectionDriverRead()
----------------------------------------------------------------------------- */

#define P_Idenfier_PCacheConnectionDriverRead 10

typedef struct __tMessage_in_PCacheConnectionDriverRead
{
   tCacheConnectionInstance * pCacheConnection;
   uint32_t nSize;
} tMessage_in_PCacheConnectionDriverRead;

typedef struct __tMessage_out_PCacheConnectionDriverRead
{
   W_ERROR value;
} tMessage_out_PCacheConnectionDriverRead;

typedef union __tMessage_in_out_PCacheConnectionDriverRead
{
   tMessage_in_PCacheConnectionDriverRead in;
   tMessage_out_PCacheConnectionDriverRead out;
} tMessage_in_out_PCacheConnectionDriverRead;

/* -----------------------------------------------------------------------------
      PCacheConnectionDriverWrite()
----------------------------------------------------------------------------- */

#define P_Idenfier_PCacheConnectionDriverWrite 11

typedef struct __tMessage_in_PCacheConnectionDriverWrite
{
   const tCacheConnectionInstance * pCacheConnection;
   uint32_t nSize;
} tMessage_in_PCacheConnectionDriverWrite;

typedef struct __tMessage_out_PCacheConnectionDriverWrite
{
   W_ERROR value;
} tMessage_out_PCacheConnectionDriverWrite;

typedef union __tMessage_in_out_PCacheConnectionDriverWrite
{
   tMessage_in_PCacheConnectionDriverWrite in;
   tMessage_out_PCacheConnectionDriverWrite out;
} tMessage_in_out_PCacheConnectionDriverWrite;

/* -----------------------------------------------------------------------------
      PContextDriverGenerateRandom()
----------------------------------------------------------------------------- */

#define P_Idenfier_PContextDriverGenerateRandom 12

typedef struct __tMessage_out_PContextDriverGenerateRandom
{
   uint32_t value;
} tMessage_out_PContextDriverGenerateRandom;

typedef union __tMessage_in_out_PContextDriverGenerateRandom
{
   tMessage_out_PContextDriverGenerateRandom out;
} tMessage_in_out_PContextDriverGenerateRandom;

/* -----------------------------------------------------------------------------
      PContextDriverGetMemoryStatistics()
----------------------------------------------------------------------------- */

#define P_Idenfier_PContextDriverGetMemoryStatistics 13

typedef struct __tMessage_in_PContextDriverGetMemoryStatistics
{
   tContextDriverMemoryStatistics * pStatistics;
   uint32_t nSize;
} tMessage_in_PContextDriverGetMemoryStatistics;

typedef union __tMessage_in_out_PContextDriverGetMemoryStatistics
{
   tMessage_in_PContextDriverGetMemoryStatistics in;
} tMessage_in_out_PContextDriverGetMemoryStatistics;

/* -----------------------------------------------------------------------------
      PContextDriverResetMemoryStatistics()
----------------------------------------------------------------------------- */

#define P_Idenfier_PContextDriverResetMemoryStatistics 14

/* -----------------------------------------------------------------------------
      PDFCDriverInterruptEventLoop()
----------------------------------------------------------------------------- */

#define P_Idenfier_PDFCDriverInterruptEventLoop 15

/* -----------------------------------------------------------------------------
      PDFCDriverStopEventLoop()
----------------------------------------------------------------------------- */

#define P_Idenfier_PDFCDriverStopEventLoop 16

/* -----------------------------------------------------------------------------
      PEmulCloseDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulCloseDriver 17

typedef struct __tMessage_in_PEmulCloseDriver
{
   W_HANDLE hHandle;
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PEmulCloseDriver;

typedef union __tMessage_in_out_PEmulCloseDriver
{
   tMessage_in_PEmulCloseDriver in;
} tMessage_in_out_PEmulCloseDriver;

/* -----------------------------------------------------------------------------
      PEmulGetMessageData()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulGetMessageData 18

typedef struct __tMessage_in_PEmulGetMessageData
{
   W_HANDLE hHandle;
   uint8_t * pDataBuffer;
   uint32_t nDataLength;
   uint32_t * pnActualDataLength;
} tMessage_in_PEmulGetMessageData;

typedef struct __tMessage_out_PEmulGetMessageData
{
   W_ERROR value;
} tMessage_out_PEmulGetMessageData;

typedef union __tMessage_in_out_PEmulGetMessageData
{
   tMessage_in_PEmulGetMessageData in;
   tMessage_out_PEmulGetMessageData out;
} tMessage_in_out_PEmulGetMessageData;

/* -----------------------------------------------------------------------------
      PEmulOpenConnectionDriver1()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulOpenConnectionDriver1 19

typedef struct __tMessage_in_PEmulOpenConnectionDriver1
{
   tPBasicGenericCallbackFunction * pOpenCallback;
   void * pOpenCallbackParameter;
   const tWEmulConnectionInfo * pEmulConnectionInfo;
   uint32_t nSize;
   W_HANDLE * phHandle;
} tMessage_in_PEmulOpenConnectionDriver1;

typedef union __tMessage_in_out_PEmulOpenConnectionDriver1
{
   tMessage_in_PEmulOpenConnectionDriver1 in;
} tMessage_in_out_PEmulOpenConnectionDriver1;

/* -----------------------------------------------------------------------------
      PEmulOpenConnectionDriver1Ex()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulOpenConnectionDriver1Ex 20

typedef struct __tMessage_in_PEmulOpenConnectionDriver1Ex
{
   tPBasicGenericCallbackFunction * pOpenCallback;
   void * pOpenCallbackParameter;
   const tWEmulConnectionInfo * pEmulConnectionInfo;
   uint32_t nSize;
   W_HANDLE * phHandle;
} tMessage_in_PEmulOpenConnectionDriver1Ex;

typedef union __tMessage_in_out_PEmulOpenConnectionDriver1Ex
{
   tMessage_in_PEmulOpenConnectionDriver1Ex in;
} tMessage_in_out_PEmulOpenConnectionDriver1Ex;

/* -----------------------------------------------------------------------------
      PEmulOpenConnectionDriver2()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulOpenConnectionDriver2 21

typedef struct __tMessage_in_PEmulOpenConnectionDriver2
{
   W_HANDLE hHandle;
   tPEmulDriverEventReceived * pEventCallback;
   void * pEventCallbackParameter;
} tMessage_in_PEmulOpenConnectionDriver2;

typedef union __tMessage_in_out_PEmulOpenConnectionDriver2
{
   tMessage_in_PEmulOpenConnectionDriver2 in;
} tMessage_in_out_PEmulOpenConnectionDriver2;

/* -----------------------------------------------------------------------------
      PEmulOpenConnectionDriver2Ex()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulOpenConnectionDriver2Ex 22

typedef struct __tMessage_in_PEmulOpenConnectionDriver2Ex
{
   W_HANDLE hHandle;
   tPEmulDriverEventReceived * pEventCallback;
   void * pEventCallbackParameter;
} tMessage_in_PEmulOpenConnectionDriver2Ex;

typedef union __tMessage_in_out_PEmulOpenConnectionDriver2Ex
{
   tMessage_in_PEmulOpenConnectionDriver2Ex in;
} tMessage_in_out_PEmulOpenConnectionDriver2Ex;

/* -----------------------------------------------------------------------------
      PEmulOpenConnectionDriver3()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulOpenConnectionDriver3 23

typedef struct __tMessage_in_PEmulOpenConnectionDriver3
{
   W_HANDLE hHandle;
   tPEmulDriverCommandReceived * pCommandCallback;
   void * pCommandCallbackParameter;
} tMessage_in_PEmulOpenConnectionDriver3;

typedef union __tMessage_in_out_PEmulOpenConnectionDriver3
{
   tMessage_in_PEmulOpenConnectionDriver3 in;
} tMessage_in_out_PEmulOpenConnectionDriver3;

/* -----------------------------------------------------------------------------
      PEmulOpenConnectionDriver3Ex()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulOpenConnectionDriver3Ex 24

typedef struct __tMessage_in_PEmulOpenConnectionDriver3Ex
{
   W_HANDLE hHandle;
   tPEmulDriverCommandReceived * pCommandCallback;
   void * pCommandCallbackParameter;
} tMessage_in_PEmulOpenConnectionDriver3Ex;

typedef union __tMessage_in_out_PEmulOpenConnectionDriver3Ex
{
   tMessage_in_PEmulOpenConnectionDriver3Ex in;
} tMessage_in_out_PEmulOpenConnectionDriver3Ex;

/* -----------------------------------------------------------------------------
      PEmulSendAnswer()
----------------------------------------------------------------------------- */

#define P_Idenfier_PEmulSendAnswer 25

typedef struct __tMessage_in_PEmulSendAnswer
{
   W_HANDLE hDriverConnection;
   const uint8_t * pDataBuffer;
   uint32_t nDataLength;
} tMessage_in_PEmulSendAnswer;

typedef struct __tMessage_out_PEmulSendAnswer
{
   W_ERROR value;
} tMessage_out_PEmulSendAnswer;

typedef union __tMessage_in_out_PEmulSendAnswer
{
   tMessage_in_PEmulSendAnswer in;
   tMessage_out_PEmulSendAnswer out;
} tMessage_in_out_PEmulSendAnswer;

/* -----------------------------------------------------------------------------
      PFeliCaDriverExchangeData()
----------------------------------------------------------------------------- */

#define P_Idenfier_PFeliCaDriverExchangeData 26

typedef struct __tMessage_in_PFeliCaDriverExchangeData
{
   W_HANDLE hDriverConnection;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
} tMessage_in_PFeliCaDriverExchangeData;

typedef union __tMessage_in_out_PFeliCaDriverExchangeData
{
   tMessage_in_PFeliCaDriverExchangeData in;
} tMessage_in_out_PFeliCaDriverExchangeData;

/* -----------------------------------------------------------------------------
      PFeliCaDriverGetCardList()
----------------------------------------------------------------------------- */

#define P_Idenfier_PFeliCaDriverGetCardList 27

typedef struct __tMessage_in_PFeliCaDriverGetCardList
{
   W_HANDLE hDriverConnection;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
} tMessage_in_PFeliCaDriverGetCardList;

typedef union __tMessage_in_out_PFeliCaDriverGetCardList
{
   tMessage_in_PFeliCaDriverGetCardList in;
} tMessage_in_out_PFeliCaDriverGetCardList;

/* -----------------------------------------------------------------------------
      PHandleCheckPropertyDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PHandleCheckPropertyDriver 28

typedef struct __tMessage_in_PHandleCheckPropertyDriver
{
   W_HANDLE hObject;
   uint8_t nPropertyValue;
} tMessage_in_PHandleCheckPropertyDriver;

typedef struct __tMessage_out_PHandleCheckPropertyDriver
{
   W_ERROR value;
} tMessage_out_PHandleCheckPropertyDriver;

typedef union __tMessage_in_out_PHandleCheckPropertyDriver
{
   tMessage_in_PHandleCheckPropertyDriver in;
   tMessage_out_PHandleCheckPropertyDriver out;
} tMessage_in_out_PHandleCheckPropertyDriver;

/* -----------------------------------------------------------------------------
      PHandleCloseDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PHandleCloseDriver 29

typedef struct __tMessage_in_PHandleCloseDriver
{
   W_HANDLE hObject;
} tMessage_in_PHandleCloseDriver;

typedef union __tMessage_in_out_PHandleCloseDriver
{
   tMessage_in_PHandleCloseDriver in;
} tMessage_in_out_PHandleCloseDriver;

/* -----------------------------------------------------------------------------
      PHandleCloseSafeDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PHandleCloseSafeDriver 30

typedef struct __tMessage_in_PHandleCloseSafeDriver
{
   W_HANDLE hObject;
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PHandleCloseSafeDriver;

typedef union __tMessage_in_out_PHandleCloseSafeDriver
{
   tMessage_in_PHandleCloseSafeDriver in;
} tMessage_in_out_PHandleCloseSafeDriver;

/* -----------------------------------------------------------------------------
      PHandleGetCountDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PHandleGetCountDriver 31

typedef struct __tMessage_out_PHandleGetCountDriver
{
   uint32_t value;
} tMessage_out_PHandleGetCountDriver;

typedef union __tMessage_in_out_PHandleGetCountDriver
{
   tMessage_out_PHandleGetCountDriver out;
} tMessage_in_out_PHandleGetCountDriver;

/* -----------------------------------------------------------------------------
      PHandleGetPropertiesDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PHandleGetPropertiesDriver 32

typedef struct __tMessage_in_PHandleGetPropertiesDriver
{
   W_HANDLE hObject;
   uint8_t * pPropertyArray;
   uint32_t nPropertyArrayLength;
} tMessage_in_PHandleGetPropertiesDriver;

typedef struct __tMessage_out_PHandleGetPropertiesDriver
{
   W_ERROR value;
} tMessage_out_PHandleGetPropertiesDriver;

typedef union __tMessage_in_out_PHandleGetPropertiesDriver
{
   tMessage_in_PHandleGetPropertiesDriver in;
   tMessage_out_PHandleGetPropertiesDriver out;
} tMessage_in_out_PHandleGetPropertiesDriver;

/* -----------------------------------------------------------------------------
      PHandleGetPropertyNumberDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PHandleGetPropertyNumberDriver 33

typedef struct __tMessage_in_PHandleGetPropertyNumberDriver
{
   W_HANDLE hObject;
   uint32_t * pnPropertyNumber;
} tMessage_in_PHandleGetPropertyNumberDriver;

typedef struct __tMessage_out_PHandleGetPropertyNumberDriver
{
   W_ERROR value;
} tMessage_out_PHandleGetPropertyNumberDriver;

typedef union __tMessage_in_out_PHandleGetPropertyNumberDriver
{
   tMessage_in_PHandleGetPropertyNumberDriver in;
   tMessage_out_PHandleGetPropertyNumberDriver out;
} tMessage_in_out_PHandleGetPropertyNumberDriver;

/* -----------------------------------------------------------------------------
      PMultiTimerCancelDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PMultiTimerCancelDriver 34

typedef struct __tMessage_in_PMultiTimerCancelDriver
{
   uint32_t nTimerIdentifier;
} tMessage_in_PMultiTimerCancelDriver;

typedef union __tMessage_in_out_PMultiTimerCancelDriver
{
   tMessage_in_PMultiTimerCancelDriver in;
} tMessage_in_out_PMultiTimerCancelDriver;

/* -----------------------------------------------------------------------------
      PMultiTimerSetDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PMultiTimerSetDriver 35

typedef struct __tMessage_in_PMultiTimerSetDriver
{
   uint32_t nTimerIdentifier;
   uint32_t nAbsoluteTimeout;
   tPBasicGenericCompletionFunction * pCallbackFunction;
   void * pCallbackParameter;
} tMessage_in_PMultiTimerSetDriver;

typedef union __tMessage_in_out_PMultiTimerSetDriver
{
   tMessage_in_PMultiTimerSetDriver in;
} tMessage_in_out_PMultiTimerSetDriver;

/* -----------------------------------------------------------------------------
      PNALServiceDriverGetCurrentTime()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNALServiceDriverGetCurrentTime 36

typedef struct __tMessage_out_PNALServiceDriverGetCurrentTime
{
   uint32_t value;
} tMessage_out_PNALServiceDriverGetCurrentTime;

typedef union __tMessage_in_out_PNALServiceDriverGetCurrentTime
{
   tMessage_out_PNALServiceDriverGetCurrentTime out;
} tMessage_in_out_PNALServiceDriverGetCurrentTime;

/* -----------------------------------------------------------------------------
      PNALServiceDriverGetProtocolStatistics()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNALServiceDriverGetProtocolStatistics 37

typedef struct __tMessage_in_PNALServiceDriverGetProtocolStatistics
{
   tNALProtocolStatistics * pStatistics;
   uint32_t nSize;
} tMessage_in_PNALServiceDriverGetProtocolStatistics;

typedef union __tMessage_in_out_PNALServiceDriverGetProtocolStatistics
{
   tMessage_in_PNALServiceDriverGetProtocolStatistics in;
} tMessage_in_out_PNALServiceDriverGetProtocolStatistics;

/* -----------------------------------------------------------------------------
      PNALServiceDriverResetProtocolStatistics()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNALServiceDriverResetProtocolStatistics 38

/* -----------------------------------------------------------------------------
      PNFCControllerDriverGetRFActivity()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerDriverGetRFActivity 39

typedef struct __tMessage_out_PNFCControllerDriverGetRFActivity
{
   uint32_t value;
} tMessage_out_PNFCControllerDriverGetRFActivity;

typedef union __tMessage_in_out_PNFCControllerDriverGetRFActivity
{
   tMessage_out_PNFCControllerDriverGetRFActivity out;
} tMessage_in_out_PNFCControllerDriverGetRFActivity;

/* -----------------------------------------------------------------------------
      PNFCControllerDriverGetRFLock()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerDriverGetRFLock 40

typedef struct __tMessage_in_PNFCControllerDriverGetRFLock
{
   uint32_t nLockSet;
} tMessage_in_PNFCControllerDriverGetRFLock;

typedef struct __tMessage_out_PNFCControllerDriverGetRFLock
{
   uint32_t value;
} tMessage_out_PNFCControllerDriverGetRFLock;

typedef union __tMessage_in_out_PNFCControllerDriverGetRFLock
{
   tMessage_in_PNFCControllerDriverGetRFLock in;
   tMessage_out_PNFCControllerDriverGetRFLock out;
} tMessage_in_out_PNFCControllerDriverGetRFLock;

/* -----------------------------------------------------------------------------
      PNFCControllerDriverReadInfo()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerDriverReadInfo 41

typedef struct __tMessage_in_PNFCControllerDriverReadInfo
{
   void * pBuffer;
   uint32_t nBufferSize;
} tMessage_in_PNFCControllerDriverReadInfo;

typedef struct __tMessage_out_PNFCControllerDriverReadInfo
{
   W_ERROR value;
} tMessage_out_PNFCControllerDriverReadInfo;

typedef union __tMessage_in_out_PNFCControllerDriverReadInfo
{
   tMessage_in_PNFCControllerDriverReadInfo in;
   tMessage_out_PNFCControllerDriverReadInfo out;
} tMessage_in_out_PNFCControllerDriverReadInfo;

/* -----------------------------------------------------------------------------
      PNFCControllerFirmwareUpdateDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerFirmwareUpdateDriver 42

typedef struct __tMessage_in_PNFCControllerFirmwareUpdateDriver
{
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pUpdateBuffer;
   uint32_t nUpdateBufferLength;
   uint32_t nMode;
} tMessage_in_PNFCControllerFirmwareUpdateDriver;

typedef union __tMessage_in_out_PNFCControllerFirmwareUpdateDriver
{
   tMessage_in_PNFCControllerFirmwareUpdateDriver in;
} tMessage_in_out_PNFCControllerFirmwareUpdateDriver;

/* -----------------------------------------------------------------------------
      PNFCControllerFirmwareUpdateState()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerFirmwareUpdateState 43

typedef struct __tMessage_out_PNFCControllerFirmwareUpdateState
{
   uint32_t value;
} tMessage_out_PNFCControllerFirmwareUpdateState;

typedef union __tMessage_in_out_PNFCControllerFirmwareUpdateState
{
   tMessage_out_PNFCControllerFirmwareUpdateState out;
} tMessage_in_out_PNFCControllerFirmwareUpdateState;

/* -----------------------------------------------------------------------------
      PNFCControllerGetMode()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerGetMode 44

typedef struct __tMessage_out_PNFCControllerGetMode
{
   uint32_t value;
} tMessage_out_PNFCControllerGetMode;

typedef union __tMessage_in_out_PNFCControllerGetMode
{
   tMessage_out_PNFCControllerGetMode out;
} tMessage_in_out_PNFCControllerGetMode;

/* -----------------------------------------------------------------------------
      PNFCControllerGetRawMessageData()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerGetRawMessageData 45

typedef struct __tMessage_in_PNFCControllerGetRawMessageData
{
   uint8_t * pBuffer;
   uint32_t nBufferLength;
   uint32_t * pnActualLength;
} tMessage_in_PNFCControllerGetRawMessageData;

typedef struct __tMessage_out_PNFCControllerGetRawMessageData
{
   W_ERROR value;
} tMessage_out_PNFCControllerGetRawMessageData;

typedef union __tMessage_in_out_PNFCControllerGetRawMessageData
{
   tMessage_in_PNFCControllerGetRawMessageData in;
   tMessage_out_PNFCControllerGetRawMessageData out;
} tMessage_in_out_PNFCControllerGetRawMessageData;

/* -----------------------------------------------------------------------------
      PNFCControllerIsActive()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerIsActive 46

typedef struct __tMessage_out_PNFCControllerIsActive
{
   bool value;
} tMessage_out_PNFCControllerIsActive;

typedef union __tMessage_in_out_PNFCControllerIsActive
{
   tMessage_out_PNFCControllerIsActive out;
} tMessage_in_out_PNFCControllerIsActive;

/* -----------------------------------------------------------------------------
      PNFCControllerMonitorException()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerMonitorException 47

typedef struct __tMessage_in_PNFCControllerMonitorException
{
   tPBasicGenericEventHandler * pHandler;
   void * pHandlerParameter;
   W_HANDLE * phEventRegistry;
} tMessage_in_PNFCControllerMonitorException;

typedef struct __tMessage_out_PNFCControllerMonitorException
{
   W_ERROR value;
} tMessage_out_PNFCControllerMonitorException;

typedef union __tMessage_in_out_PNFCControllerMonitorException
{
   tMessage_in_PNFCControllerMonitorException in;
   tMessage_out_PNFCControllerMonitorException out;
} tMessage_in_out_PNFCControllerMonitorException;

/* -----------------------------------------------------------------------------
      PNFCControllerMonitorFieldEvents()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerMonitorFieldEvents 48

typedef struct __tMessage_in_PNFCControllerMonitorFieldEvents
{
   tPBasicGenericEventHandler * pHandler;
   void * pHandlerParameter;
   W_HANDLE * phEventRegistry;
} tMessage_in_PNFCControllerMonitorFieldEvents;

typedef struct __tMessage_out_PNFCControllerMonitorFieldEvents
{
   W_ERROR value;
} tMessage_out_PNFCControllerMonitorFieldEvents;

typedef union __tMessage_in_out_PNFCControllerMonitorFieldEvents
{
   tMessage_in_PNFCControllerMonitorFieldEvents in;
   tMessage_out_PNFCControllerMonitorFieldEvents out;
} tMessage_in_out_PNFCControllerMonitorFieldEvents;

/* -----------------------------------------------------------------------------
      PNFCControllerProductionTestDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerProductionTestDriver 49

typedef struct __tMessage_in_PNFCControllerProductionTestDriver
{
   const uint8_t * pParameterBuffer;
   uint32_t nParameterBufferLength;
   uint8_t * pResultBuffer;
   uint32_t nResultBufferLength;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PNFCControllerProductionTestDriver;

typedef union __tMessage_in_out_PNFCControllerProductionTestDriver
{
   tMessage_in_PNFCControllerProductionTestDriver in;
} tMessage_in_out_PNFCControllerProductionTestDriver;

/* -----------------------------------------------------------------------------
      PNFCControllerRegisterRawListener()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerRegisterRawListener 50

typedef struct __tMessage_in_PNFCControllerRegisterRawListener
{
   tPBasicGenericDataCallbackFunction * pReceiveMessageEventHandler;
   void * pHandlerParameter;
} tMessage_in_PNFCControllerRegisterRawListener;

typedef struct __tMessage_out_PNFCControllerRegisterRawListener
{
   W_ERROR value;
} tMessage_out_PNFCControllerRegisterRawListener;

typedef union __tMessage_in_out_PNFCControllerRegisterRawListener
{
   tMessage_in_PNFCControllerRegisterRawListener in;
   tMessage_out_PNFCControllerRegisterRawListener out;
} tMessage_in_out_PNFCControllerRegisterRawListener;

/* -----------------------------------------------------------------------------
      PNFCControllerResetDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerResetDriver 51

typedef struct __tMessage_in_PNFCControllerResetDriver
{
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
   uint32_t nMode;
} tMessage_in_PNFCControllerResetDriver;

typedef union __tMessage_in_out_PNFCControllerResetDriver
{
   tMessage_in_PNFCControllerResetDriver in;
} tMessage_in_out_PNFCControllerResetDriver;

/* -----------------------------------------------------------------------------
      PNFCControllerSelfTestDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerSelfTestDriver 52

typedef struct __tMessage_in_PNFCControllerSelfTestDriver
{
   tPNFCControllerSelfTestCompleted * pCallback;
   void * pCallbackParameter;
} tMessage_in_PNFCControllerSelfTestDriver;

typedef union __tMessage_in_out_PNFCControllerSelfTestDriver
{
   tMessage_in_PNFCControllerSelfTestDriver in;
} tMessage_in_out_PNFCControllerSelfTestDriver;

/* -----------------------------------------------------------------------------
      PNFCControllerSetRFLockDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerSetRFLockDriver 53

typedef struct __tMessage_in_PNFCControllerSetRFLockDriver
{
   uint32_t nLockSet;
   bool bReaderLock;
   bool bCardLock;
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PNFCControllerSetRFLockDriver;

typedef union __tMessage_in_out_PNFCControllerSetRFLockDriver
{
   tMessage_in_PNFCControllerSetRFLockDriver in;
} tMessage_in_out_PNFCControllerSetRFLockDriver;

/* -----------------------------------------------------------------------------
      PNFCControllerSwitchStandbyMode()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerSwitchStandbyMode 54

typedef struct __tMessage_in_PNFCControllerSwitchStandbyMode
{
   bool bStandbyOn;
} tMessage_in_PNFCControllerSwitchStandbyMode;

typedef struct __tMessage_out_PNFCControllerSwitchStandbyMode
{
   W_ERROR value;
} tMessage_out_PNFCControllerSwitchStandbyMode;

typedef union __tMessage_in_out_PNFCControllerSwitchStandbyMode
{
   tMessage_in_PNFCControllerSwitchStandbyMode in;
   tMessage_out_PNFCControllerSwitchStandbyMode out;
} tMessage_in_out_PNFCControllerSwitchStandbyMode;

/* -----------------------------------------------------------------------------
      PNFCControllerSwitchToRawModeDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerSwitchToRawModeDriver 55

typedef struct __tMessage_in_PNFCControllerSwitchToRawModeDriver
{
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PNFCControllerSwitchToRawModeDriver;

typedef union __tMessage_in_out_PNFCControllerSwitchToRawModeDriver
{
   tMessage_in_PNFCControllerSwitchToRawModeDriver in;
} tMessage_in_out_PNFCControllerSwitchToRawModeDriver;

/* -----------------------------------------------------------------------------
      PNFCControllerWriteRawMessageDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PNFCControllerWriteRawMessageDriver 56

typedef struct __tMessage_in_PNFCControllerWriteRawMessageDriver
{
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pBuffer;
   uint32_t nLength;
} tMessage_in_PNFCControllerWriteRawMessageDriver;

typedef union __tMessage_in_out_PNFCControllerWriteRawMessageDriver
{
   tMessage_in_PNFCControllerWriteRawMessageDriver in;
} tMessage_in_out_PNFCControllerWriteRawMessageDriver;

/* -----------------------------------------------------------------------------
      PP2PConnectDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PConnectDriver 57

typedef struct __tMessage_in_PP2PConnectDriver
{
   W_HANDLE hSocket;
   W_HANDLE hLink;
   tPBasicGenericCallbackFunction * pEstablishmentCallback;
   void * pEstablishmentCallbackParameter;
} tMessage_in_PP2PConnectDriver;

typedef union __tMessage_in_out_PP2PConnectDriver
{
   tMessage_in_PP2PConnectDriver in;
} tMessage_in_out_PP2PConnectDriver;

/* -----------------------------------------------------------------------------
      PP2PCreateSocketDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PCreateSocketDriver 58

typedef struct __tMessage_in_PP2PCreateSocketDriver
{
   uint8_t nType;
   const tchar * pServiceURI;
   uint32_t nSize;
   uint8_t nSAP;
   W_HANDLE * phSocket;
} tMessage_in_PP2PCreateSocketDriver;

typedef struct __tMessage_out_PP2PCreateSocketDriver
{
   W_ERROR value;
} tMessage_out_PP2PCreateSocketDriver;

typedef union __tMessage_in_out_PP2PCreateSocketDriver
{
   tMessage_in_PP2PCreateSocketDriver in;
   tMessage_out_PP2PCreateSocketDriver out;
} tMessage_in_out_PP2PCreateSocketDriver;

/* -----------------------------------------------------------------------------
      PP2PEstablishLinkDriver1()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PEstablishLinkDriver1 59

typedef struct __tMessage_in_PP2PEstablishLinkDriver1
{
   tPBasicGenericHandleCallbackFunction * pEstablishmentCallback;
   void * pEstablishmentCallbackParameter;
} tMessage_in_PP2PEstablishLinkDriver1;

typedef struct __tMessage_out_PP2PEstablishLinkDriver1
{
   W_HANDLE value;
} tMessage_out_PP2PEstablishLinkDriver1;

typedef union __tMessage_in_out_PP2PEstablishLinkDriver1
{
   tMessage_in_PP2PEstablishLinkDriver1 in;
   tMessage_out_PP2PEstablishLinkDriver1 out;
} tMessage_in_out_PP2PEstablishLinkDriver1;

/* -----------------------------------------------------------------------------
      PP2PEstablishLinkDriver1Wrapper()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PEstablishLinkDriver1Wrapper 60

typedef struct __tMessage_in_PP2PEstablishLinkDriver1Wrapper
{
   tPBasicGenericHandleCallbackFunction * pEstablishmentCallback;
   void * pEstablishmentCallbackParameter;
} tMessage_in_PP2PEstablishLinkDriver1Wrapper;

typedef struct __tMessage_out_PP2PEstablishLinkDriver1Wrapper
{
   W_HANDLE value;
} tMessage_out_PP2PEstablishLinkDriver1Wrapper;

typedef union __tMessage_in_out_PP2PEstablishLinkDriver1Wrapper
{
   tMessage_in_PP2PEstablishLinkDriver1Wrapper in;
   tMessage_out_PP2PEstablishLinkDriver1Wrapper out;
} tMessage_in_out_PP2PEstablishLinkDriver1Wrapper;

/* -----------------------------------------------------------------------------
      PP2PEstablishLinkDriver2()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PEstablishLinkDriver2 61

typedef struct __tMessage_in_PP2PEstablishLinkDriver2
{
   W_HANDLE hLink;
   tPBasicGenericCallbackFunction * pReleaseCallback;
   void * pReleaseCallbackParameter;
   W_HANDLE * phOperation;
} tMessage_in_PP2PEstablishLinkDriver2;

typedef union __tMessage_in_out_PP2PEstablishLinkDriver2
{
   tMessage_in_PP2PEstablishLinkDriver2 in;
} tMessage_in_out_PP2PEstablishLinkDriver2;

/* -----------------------------------------------------------------------------
      PP2PEstablishLinkDriver2Wrapper()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PEstablishLinkDriver2Wrapper 62

typedef struct __tMessage_in_PP2PEstablishLinkDriver2Wrapper
{
   W_HANDLE hLink;
   tPBasicGenericCallbackFunction * pReleaseCallback;
   void * pReleaseCallbackParameter;
   W_HANDLE * phOperation;
} tMessage_in_PP2PEstablishLinkDriver2Wrapper;

typedef union __tMessage_in_out_PP2PEstablishLinkDriver2Wrapper
{
   tMessage_in_PP2PEstablishLinkDriver2Wrapper in;
} tMessage_in_out_PP2PEstablishLinkDriver2Wrapper;

/* -----------------------------------------------------------------------------
      PP2PGetConfigurationDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PGetConfigurationDriver 63

typedef struct __tMessage_in_PP2PGetConfigurationDriver
{
   tWP2PConfiguration * pConfiguration;
   uint32_t nSize;
} tMessage_in_PP2PGetConfigurationDriver;

typedef struct __tMessage_out_PP2PGetConfigurationDriver
{
   W_ERROR value;
} tMessage_out_PP2PGetConfigurationDriver;

typedef union __tMessage_in_out_PP2PGetConfigurationDriver
{
   tMessage_in_PP2PGetConfigurationDriver in;
   tMessage_out_PP2PGetConfigurationDriver out;
} tMessage_in_out_PP2PGetConfigurationDriver;

/* -----------------------------------------------------------------------------
      PP2PGetLinkPropertiesDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PGetLinkPropertiesDriver 64

typedef struct __tMessage_in_PP2PGetLinkPropertiesDriver
{
   W_HANDLE hLink;
   tWP2PLinkProperties * pProperties;
   uint32_t nSize;
} tMessage_in_PP2PGetLinkPropertiesDriver;

typedef struct __tMessage_out_PP2PGetLinkPropertiesDriver
{
   W_ERROR value;
} tMessage_out_PP2PGetLinkPropertiesDriver;

typedef union __tMessage_in_out_PP2PGetLinkPropertiesDriver
{
   tMessage_in_PP2PGetLinkPropertiesDriver in;
   tMessage_out_PP2PGetLinkPropertiesDriver out;
} tMessage_in_out_PP2PGetLinkPropertiesDriver;

/* -----------------------------------------------------------------------------
      PP2PGetSocketParameterDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PGetSocketParameterDriver 65

typedef struct __tMessage_in_PP2PGetSocketParameterDriver
{
   W_HANDLE hSocket;
   uint32_t nParameter;
   uint32_t * pnValue;
} tMessage_in_PP2PGetSocketParameterDriver;

typedef struct __tMessage_out_PP2PGetSocketParameterDriver
{
   W_ERROR value;
} tMessage_out_PP2PGetSocketParameterDriver;

typedef union __tMessage_in_out_PP2PGetSocketParameterDriver
{
   tMessage_in_PP2PGetSocketParameterDriver in;
   tMessage_out_PP2PGetSocketParameterDriver out;
} tMessage_in_out_PP2PGetSocketParameterDriver;

/* -----------------------------------------------------------------------------
      PP2PReadDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PReadDriver 66

typedef struct __tMessage_in_PP2PReadDriver
{
   W_HANDLE hConnection;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
   uint8_t * pReceptionBuffer;
   uint32_t nReceptionBufferLength;
   W_HANDLE * phOperation;
} tMessage_in_PP2PReadDriver;

typedef union __tMessage_in_out_PP2PReadDriver
{
   tMessage_in_PP2PReadDriver in;
} tMessage_in_out_PP2PReadDriver;

/* -----------------------------------------------------------------------------
      PP2PRecvFromDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PRecvFromDriver 67

typedef struct __tMessage_in_PP2PRecvFromDriver
{
   W_HANDLE hSocket;
   tPP2PRecvFromCompleted * pCallback;
   void * pCallbackParameter;
   uint8_t * pReceptionBuffer;
   uint32_t nReceptionBufferLength;
   W_HANDLE * phOperation;
} tMessage_in_PP2PRecvFromDriver;

typedef union __tMessage_in_out_PP2PRecvFromDriver
{
   tMessage_in_PP2PRecvFromDriver in;
} tMessage_in_out_PP2PRecvFromDriver;

/* -----------------------------------------------------------------------------
      PP2PSendToDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PSendToDriver 68

typedef struct __tMessage_in_PP2PSendToDriver
{
   W_HANDLE hSocket;
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
   uint8_t nSAP;
   const uint8_t * pSendBuffer;
   uint32_t nSendBufferLength;
   W_HANDLE * phOperation;
} tMessage_in_PP2PSendToDriver;

typedef union __tMessage_in_out_PP2PSendToDriver
{
   tMessage_in_PP2PSendToDriver in;
} tMessage_in_out_PP2PSendToDriver;

/* -----------------------------------------------------------------------------
      PP2PSetConfigurationDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PSetConfigurationDriver 69

typedef struct __tMessage_in_PP2PSetConfigurationDriver
{
   const tWP2PConfiguration * pConfiguration;
   uint32_t nSize;
} tMessage_in_PP2PSetConfigurationDriver;

typedef struct __tMessage_out_PP2PSetConfigurationDriver
{
   W_ERROR value;
} tMessage_out_PP2PSetConfigurationDriver;

typedef union __tMessage_in_out_PP2PSetConfigurationDriver
{
   tMessage_in_PP2PSetConfigurationDriver in;
   tMessage_out_PP2PSetConfigurationDriver out;
} tMessage_in_out_PP2PSetConfigurationDriver;

/* -----------------------------------------------------------------------------
      PP2PSetSocketParameter()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PSetSocketParameter 70

typedef struct __tMessage_in_PP2PSetSocketParameter
{
   W_HANDLE hSocket;
   uint32_t nParameter;
   uint32_t nValue;
} tMessage_in_PP2PSetSocketParameter;

typedef struct __tMessage_out_PP2PSetSocketParameter
{
   W_ERROR value;
} tMessage_out_PP2PSetSocketParameter;

typedef union __tMessage_in_out_PP2PSetSocketParameter
{
   tMessage_in_PP2PSetSocketParameter in;
   tMessage_out_PP2PSetSocketParameter out;
} tMessage_in_out_PP2PSetSocketParameter;

/* -----------------------------------------------------------------------------
      PP2PShutdownDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PShutdownDriver 71

typedef struct __tMessage_in_PP2PShutdownDriver
{
   W_HANDLE hSocket;
   tPBasicGenericCallbackFunction * pReleaseCallback;
   void * pReleaseCallbackParameter;
} tMessage_in_PP2PShutdownDriver;

typedef union __tMessage_in_out_PP2PShutdownDriver
{
   tMessage_in_PP2PShutdownDriver in;
} tMessage_in_out_PP2PShutdownDriver;

/* -----------------------------------------------------------------------------
      PP2PURILookupDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PURILookupDriver 72

typedef struct __tMessage_in_PP2PURILookupDriver
{
   W_HANDLE hLink;
   tPP2PURILookupCompleted * pCallback;
   void * pCallbackParameter;
   const tchar * pServiceURI;
   uint32_t nSize;
} tMessage_in_PP2PURILookupDriver;

typedef union __tMessage_in_out_PP2PURILookupDriver
{
   tMessage_in_PP2PURILookupDriver in;
} tMessage_in_out_PP2PURILookupDriver;

/* -----------------------------------------------------------------------------
      PP2PWriteDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PP2PWriteDriver 73

typedef struct __tMessage_in_PP2PWriteDriver
{
   W_HANDLE hConnection;
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pSendBuffer;
   uint32_t nSendBufferLength;
   W_HANDLE * phOperation;
} tMessage_in_PP2PWriteDriver;

typedef union __tMessage_in_out_PP2PWriteDriver
{
   tMessage_in_PP2PWriteDriver in;
} tMessage_in_out_PP2PWriteDriver;

/* -----------------------------------------------------------------------------
      PReaderDriverGetNbCardDetected()
----------------------------------------------------------------------------- */

#define P_Idenfier_PReaderDriverGetNbCardDetected 74

typedef struct __tMessage_out_PReaderDriverGetNbCardDetected
{
   uint8_t value;
} tMessage_out_PReaderDriverGetNbCardDetected;

typedef union __tMessage_in_out_PReaderDriverGetNbCardDetected
{
   tMessage_out_PReaderDriverGetNbCardDetected out;
} tMessage_in_out_PReaderDriverGetNbCardDetected;

/* -----------------------------------------------------------------------------
      PReaderDriverRegister()
----------------------------------------------------------------------------- */

#define P_Idenfier_PReaderDriverRegister 75

typedef struct __tMessage_in_PReaderDriverRegister
{
   tPReaderDriverRegisterCompleted * pCallback;
   void * pCallbackParameter;
   uint8_t nPriority;
   uint32_t nRequestedProtocolsBF;
   uint32_t nDetectionConfigurationLength;
   uint8_t * pBuffer;
   uint32_t nBufferMaxLength;
   W_HANDLE * phListenerHandle;
} tMessage_in_PReaderDriverRegister;

typedef struct __tMessage_out_PReaderDriverRegister
{
   W_ERROR value;
} tMessage_out_PReaderDriverRegister;

typedef union __tMessage_in_out_PReaderDriverRegister
{
   tMessage_in_PReaderDriverRegister in;
   tMessage_out_PReaderDriverRegister out;
} tMessage_in_out_PReaderDriverRegister;

/* -----------------------------------------------------------------------------
      PReaderDriverSetWorkPerformedAndClose()
----------------------------------------------------------------------------- */

#define P_Idenfier_PReaderDriverSetWorkPerformedAndClose 76

typedef struct __tMessage_in_PReaderDriverSetWorkPerformedAndClose
{
   W_HANDLE hDriverListener;
} tMessage_in_PReaderDriverSetWorkPerformedAndClose;

typedef struct __tMessage_out_PReaderDriverSetWorkPerformedAndClose
{
   W_ERROR value;
} tMessage_out_PReaderDriverSetWorkPerformedAndClose;

typedef union __tMessage_in_out_PReaderDriverSetWorkPerformedAndClose
{
   tMessage_in_PReaderDriverSetWorkPerformedAndClose in;
   tMessage_out_PReaderDriverSetWorkPerformedAndClose out;
} tMessage_in_out_PReaderDriverSetWorkPerformedAndClose;

/* -----------------------------------------------------------------------------
      PReaderDriverWorkPerformed()
----------------------------------------------------------------------------- */

#define P_Idenfier_PReaderDriverWorkPerformed 77

typedef struct __tMessage_in_PReaderDriverWorkPerformed
{
   W_HANDLE hConnection;
   bool bGiveToNextListener;
   bool bCardApplicationMatch;
} tMessage_in_PReaderDriverWorkPerformed;

typedef struct __tMessage_out_PReaderDriverWorkPerformed
{
   W_ERROR value;
} tMessage_out_PReaderDriverWorkPerformed;

typedef union __tMessage_in_out_PReaderDriverWorkPerformed
{
   tMessage_in_PReaderDriverWorkPerformed in;
   tMessage_out_PReaderDriverWorkPerformed out;
} tMessage_in_out_PReaderDriverWorkPerformed;

/* -----------------------------------------------------------------------------
      PReaderErrorEventRegister()
----------------------------------------------------------------------------- */

#define P_Idenfier_PReaderErrorEventRegister 78

typedef struct __tMessage_in_PReaderErrorEventRegister
{
   tPBasicGenericEventHandler * pHandler;
   void * pHandlerParameter;
   uint8_t nEventType;
   bool bCardDetectionRequested;
   W_HANDLE * phRegistryHandle;
} tMessage_in_PReaderErrorEventRegister;

typedef struct __tMessage_out_PReaderErrorEventRegister
{
   W_ERROR value;
} tMessage_out_PReaderErrorEventRegister;

typedef union __tMessage_in_out_PReaderErrorEventRegister
{
   tMessage_in_PReaderErrorEventRegister in;
   tMessage_out_PReaderErrorEventRegister out;
} tMessage_in_out_PReaderErrorEventRegister;

/* -----------------------------------------------------------------------------
      PReaderGetPulsePeriod()
----------------------------------------------------------------------------- */

#define P_Idenfier_PReaderGetPulsePeriod 79

typedef struct __tMessage_in_PReaderGetPulsePeriod
{
   uint32_t * pnTimeout;
} tMessage_in_PReaderGetPulsePeriod;

typedef struct __tMessage_out_PReaderGetPulsePeriod
{
   W_ERROR value;
} tMessage_out_PReaderGetPulsePeriod;

typedef union __tMessage_in_out_PReaderGetPulsePeriod
{
   tMessage_in_PReaderGetPulsePeriod in;
   tMessage_out_PReaderGetPulsePeriod out;
} tMessage_in_out_PReaderGetPulsePeriod;

/* -----------------------------------------------------------------------------
      PReaderSetPulsePeriodDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PReaderSetPulsePeriodDriver 80

typedef struct __tMessage_in_PReaderSetPulsePeriodDriver
{
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
   uint32_t nPulsePeriod;
} tMessage_in_PReaderSetPulsePeriodDriver;

typedef union __tMessage_in_out_PReaderSetPulsePeriodDriver
{
   tMessage_in_PReaderSetPulsePeriodDriver in;
} tMessage_in_out_PReaderSetPulsePeriodDriver;

/* -----------------------------------------------------------------------------
      PSecurityManagerDriverAuthenticate()
----------------------------------------------------------------------------- */

#define P_Idenfier_PSecurityManagerDriverAuthenticate 81

typedef struct __tMessage_in_PSecurityManagerDriverAuthenticate
{
   const uint8_t * pApplicationDataBuffer;
   uint32_t nApplicationDataBufferLength;
} tMessage_in_PSecurityManagerDriverAuthenticate;

typedef struct __tMessage_out_PSecurityManagerDriverAuthenticate
{
   W_ERROR value;
} tMessage_out_PSecurityManagerDriverAuthenticate;

typedef union __tMessage_in_out_PSecurityManagerDriverAuthenticate
{
   tMessage_in_PSecurityManagerDriverAuthenticate in;
   tMessage_out_PSecurityManagerDriverAuthenticate out;
} tMessage_in_out_PSecurityManagerDriverAuthenticate;

/* -----------------------------------------------------------------------------
      PSEDriverGetInfo()
----------------------------------------------------------------------------- */

#define P_Idenfier_PSEDriverGetInfo 82

typedef struct __tMessage_in_PSEDriverGetInfo
{
   uint32_t nSlotIdentifier;
   tSEInfo * pSEInfo;
   uint32_t nSize;
} tMessage_in_PSEDriverGetInfo;

typedef struct __tMessage_out_PSEDriverGetInfo
{
   W_ERROR value;
} tMessage_out_PSEDriverGetInfo;

typedef union __tMessage_in_out_PSEDriverGetInfo
{
   tMessage_in_PSEDriverGetInfo in;
   tMessage_out_PSEDriverGetInfo out;
} tMessage_in_out_PSEDriverGetInfo;

/* -----------------------------------------------------------------------------
      PSEDriverOpenConnection()
----------------------------------------------------------------------------- */

#define P_Idenfier_PSEDriverOpenConnection 83

typedef struct __tMessage_in_PSEDriverOpenConnection
{
   uint32_t nSlotIdentifier;
   bool bForce;
   tPBasicGenericHandleCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PSEDriverOpenConnection;

typedef union __tMessage_in_out_PSEDriverOpenConnection
{
   tMessage_in_PSEDriverOpenConnection in;
} tMessage_in_out_PSEDriverOpenConnection;

/* -----------------------------------------------------------------------------
      PSEGetTransactionAID()
----------------------------------------------------------------------------- */

#define P_Idenfier_PSEGetTransactionAID 84

typedef struct __tMessage_in_PSEGetTransactionAID
{
   uint32_t nSlotIdentifier;
   uint8_t * pBuffer;
   uint32_t nBufferLength;
} tMessage_in_PSEGetTransactionAID;

typedef struct __tMessage_out_PSEGetTransactionAID
{
   uint32_t value;
} tMessage_out_PSEGetTransactionAID;

typedef union __tMessage_in_out_PSEGetTransactionAID
{
   tMessage_in_PSEGetTransactionAID in;
   tMessage_out_PSEGetTransactionAID out;
} tMessage_in_out_PSEGetTransactionAID;

/* -----------------------------------------------------------------------------
      PSEMonitorEndOfTransaction()
----------------------------------------------------------------------------- */

#define P_Idenfier_PSEMonitorEndOfTransaction 85

typedef struct __tMessage_in_PSEMonitorEndOfTransaction
{
   uint32_t nSlotIdentifier;
   tPBasicGenericEventHandler2 * pHandler;
   void * pHandlerParameter;
   W_HANDLE * phEventRegistry;
} tMessage_in_PSEMonitorEndOfTransaction;

typedef struct __tMessage_out_PSEMonitorEndOfTransaction
{
   W_ERROR value;
} tMessage_out_PSEMonitorEndOfTransaction;

typedef union __tMessage_in_out_PSEMonitorEndOfTransaction
{
   tMessage_in_PSEMonitorEndOfTransaction in;
   tMessage_out_PSEMonitorEndOfTransaction out;
} tMessage_in_out_PSEMonitorEndOfTransaction;

/* -----------------------------------------------------------------------------
      PSEMonitorHotPlugEvents()
----------------------------------------------------------------------------- */

#define P_Idenfier_PSEMonitorHotPlugEvents 86

typedef struct __tMessage_in_PSEMonitorHotPlugEvents
{
   uint32_t nSlotIdentifier;
   tPBasicGenericEventHandler2 * pHandler;
   void * pHandlerParameter;
   W_HANDLE * phEventRegistry;
} tMessage_in_PSEMonitorHotPlugEvents;

typedef struct __tMessage_out_PSEMonitorHotPlugEvents
{
   W_ERROR value;
} tMessage_out_PSEMonitorHotPlugEvents;

typedef union __tMessage_in_out_PSEMonitorHotPlugEvents
{
   tMessage_in_PSEMonitorHotPlugEvents in;
   tMessage_out_PSEMonitorHotPlugEvents out;
} tMessage_in_out_PSEMonitorHotPlugEvents;

/* -----------------------------------------------------------------------------
      PSESetPolicyDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PSESetPolicyDriver 87

typedef struct __tMessage_in_PSESetPolicyDriver
{
   uint32_t nSlotIdentifier;
   uint32_t nStorageType;
   uint32_t nProtocols;
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PSESetPolicyDriver;

typedef union __tMessage_in_out_PSESetPolicyDriver
{
   tMessage_in_PSESetPolicyDriver in;
} tMessage_in_out_PSESetPolicyDriver;

/* -----------------------------------------------------------------------------
      PType1ChipDriverExchangeData()
----------------------------------------------------------------------------- */

#define P_Idenfier_PType1ChipDriverExchangeData 88

typedef struct __tMessage_in_PType1ChipDriverExchangeData
{
   W_HANDLE hDriverConnection;
   tPBasicGenericDataCallbackFunction * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
} tMessage_in_PType1ChipDriverExchangeData;

typedef struct __tMessage_out_PType1ChipDriverExchangeData
{
   W_HANDLE value;
} tMessage_out_PType1ChipDriverExchangeData;

typedef union __tMessage_in_out_PType1ChipDriverExchangeData
{
   tMessage_in_PType1ChipDriverExchangeData in;
   tMessage_out_PType1ChipDriverExchangeData out;
} tMessage_in_out_PType1ChipDriverExchangeData;

/* -----------------------------------------------------------------------------
      PUICCActivateSWPLineDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCActivateSWPLineDriver 89

typedef struct __tMessage_in_PUICCActivateSWPLineDriver
{
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PUICCActivateSWPLineDriver;

typedef union __tMessage_in_out_PUICCActivateSWPLineDriver
{
   tMessage_in_PUICCActivateSWPLineDriver in;
} tMessage_in_out_PUICCActivateSWPLineDriver;

/* -----------------------------------------------------------------------------
      PUICCDriverGetAccessPolicy()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCDriverGetAccessPolicy 90

typedef struct __tMessage_in_PUICCDriverGetAccessPolicy
{
   uint32_t nStorageType;
   tWUICCAccessPolicy * pAccessPolicy;
   uint32_t nSize;
} tMessage_in_PUICCDriverGetAccessPolicy;

typedef struct __tMessage_out_PUICCDriverGetAccessPolicy
{
   W_ERROR value;
} tMessage_out_PUICCDriverGetAccessPolicy;

typedef union __tMessage_in_out_PUICCDriverGetAccessPolicy
{
   tMessage_in_PUICCDriverGetAccessPolicy in;
   tMessage_out_PUICCDriverGetAccessPolicy out;
} tMessage_in_out_PUICCDriverGetAccessPolicy;

/* -----------------------------------------------------------------------------
      PUICCDriverSetAccessPolicy()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCDriverSetAccessPolicy 91

typedef struct __tMessage_in_PUICCDriverSetAccessPolicy
{
   uint32_t nStorageType;
   const tWUICCAccessPolicy * pAccessPolicy;
   uint32_t nSize;
   tPBasicGenericCallbackFunction * pCallback;
   void * pCallbackParameter;
} tMessage_in_PUICCDriverSetAccessPolicy;

typedef union __tMessage_in_out_PUICCDriverSetAccessPolicy
{
   tMessage_in_PUICCDriverSetAccessPolicy in;
} tMessage_in_out_PUICCDriverSetAccessPolicy;

/* -----------------------------------------------------------------------------
      PUICCGetConnectivityEventParameter()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCGetConnectivityEventParameter 92

typedef struct __tMessage_in_PUICCGetConnectivityEventParameter
{
   uint8_t * pDataBuffer;
   uint32_t nBufferLength;
   uint32_t * pnActualDataLength;
} tMessage_in_PUICCGetConnectivityEventParameter;

typedef struct __tMessage_out_PUICCGetConnectivityEventParameter
{
   W_ERROR value;
} tMessage_out_PUICCGetConnectivityEventParameter;

typedef union __tMessage_in_out_PUICCGetConnectivityEventParameter
{
   tMessage_in_PUICCGetConnectivityEventParameter in;
   tMessage_out_PUICCGetConnectivityEventParameter out;
} tMessage_in_out_PUICCGetConnectivityEventParameter;

/* -----------------------------------------------------------------------------
      PUICCGetSlotInfoDriver()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCGetSlotInfoDriver 93

typedef struct __tMessage_in_PUICCGetSlotInfoDriver
{
   tPUICCGetSlotInfoCompleted * pCallback;
   void * pCallbackParameter;
} tMessage_in_PUICCGetSlotInfoDriver;

typedef union __tMessage_in_out_PUICCGetSlotInfoDriver
{
   tMessage_in_PUICCGetSlotInfoDriver in;
} tMessage_in_out_PUICCGetSlotInfoDriver;

/* -----------------------------------------------------------------------------
      PUICCGetTransactionEventAID()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCGetTransactionEventAID 94

typedef struct __tMessage_in_PUICCGetTransactionEventAID
{
   uint8_t * pBuffer;
   uint32_t nBufferLength;
} tMessage_in_PUICCGetTransactionEventAID;

typedef struct __tMessage_out_PUICCGetTransactionEventAID
{
   uint32_t value;
} tMessage_out_PUICCGetTransactionEventAID;

typedef union __tMessage_in_out_PUICCGetTransactionEventAID
{
   tMessage_in_PUICCGetTransactionEventAID in;
   tMessage_out_PUICCGetTransactionEventAID out;
} tMessage_in_out_PUICCGetTransactionEventAID;

/* -----------------------------------------------------------------------------
      PUICCMonitorConnectivityEvent()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCMonitorConnectivityEvent 95

typedef struct __tMessage_in_PUICCMonitorConnectivityEvent
{
   tPUICCMonitorConnectivityEventHandler * pHandler;
   void * pHandlerParameter;
   W_HANDLE * phEventRegistry;
} tMessage_in_PUICCMonitorConnectivityEvent;

typedef struct __tMessage_out_PUICCMonitorConnectivityEvent
{
   W_ERROR value;
} tMessage_out_PUICCMonitorConnectivityEvent;

typedef union __tMessage_in_out_PUICCMonitorConnectivityEvent
{
   tMessage_in_PUICCMonitorConnectivityEvent in;
   tMessage_out_PUICCMonitorConnectivityEvent out;
} tMessage_in_out_PUICCMonitorConnectivityEvent;

/* -----------------------------------------------------------------------------
      PUICCMonitorTransactionEvent()
----------------------------------------------------------------------------- */

#define P_Idenfier_PUICCMonitorTransactionEvent 96

typedef struct __tMessage_in_PUICCMonitorTransactionEvent
{
   tPUICCMonitorTransactionEventHandler * pHandler;
   void * pHandlerParameter;
   W_HANDLE * phEventRegistry;
} tMessage_in_PUICCMonitorTransactionEvent;

typedef struct __tMessage_out_PUICCMonitorTransactionEvent
{
   W_ERROR value;
} tMessage_out_PUICCMonitorTransactionEvent;

typedef union __tMessage_in_out_PUICCMonitorTransactionEvent
{
   tMessage_in_PUICCMonitorTransactionEvent in;
   tMessage_out_PUICCMonitorTransactionEvent out;
} tMessage_in_out_PUICCMonitorTransactionEvent;

#define P_DRIVER_FUNCTION_COUNT 97

#endif /* P_CONFIG_USER || P_CONFIG_DRIVER */

#endif /* #ifdef P_CONFIG_CLIENT_SERVER */


#endif /* #ifdef __WME_SERVER_AUTOGEN_H */
