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
   Contains the implementation of the NFCC Boot and Download functions
*******************************************************************************/

#define P_MODULE  P_MODULE_DEC( NFCC_B )

#include "wme_context.h"

#define  NFCC_EVENT_INITIAL                  0
#define  NFCC_EVENT_SERVICE_CONNECTED        1
#define  NFCC_EVENT_COMMAND_EXECUTED         2
#define  NFCC_EVENT_TIMEOUT_ELAPSED          3

#define  NFCC_STATUS_PENDING_CONNECTED       0x0100
#define  NFCC_STATUS_PENDING_EXECUTED        0x0200
#define  NFCC_STATUS_PENDING_CANCEL          0x0400

#define  NFCC_BOOT_STATE_INIT                         0x0000
#define  NFCC_BOOT_STATE_GET_HARDWARE_INFO            0x0001
#define  NFCC_BOOT_STATE_GET_FIRMWARE_INFO            0x0002
#define  NFCC_BOOT_STATE_GET_PERSISTENT_POLICY        0x0003
#define  NFCC_BOOT_STATE_GET_PERSISTENT_MEMORY        0x0004
#define  NFCC_BOOT_STATE_SET_PULSE                    0x0005
#define  NFCC_BOOT_STATE_START_SWP_1                  0x0006
#define  NFCC_BOOT_STATE_START_SWP_2                  0x0007
#define  NFCC_BOOT_STATE_SET_CURRENT_POLICY_DONE      0x0008
#define  NFCC_BOOT_STATE_SET_PERSISTENT_POLICY_DONE   0x0009
#define  NFCC_BOOT_STATE_START_SWP_3                  0x000A
#define  NFCC_BOOT_STATE_START_SE                     0x000B
#define  NFCC_BOOT_STATE_WAIT_FOR_SWP_BOOT            0x000C
#define  NFCC_BOOT_STATE_FINAL                        0x000D

#define  NFCC_UPDATE_STATE_LOAD_CHUNK          0x0010
#define  NFCC_UPDATE_STATE_FINAL               0x0011

/* The update firmware block size */
#define NFCC_UPDATE_BLOCK_SIZE               200

#define  NFCC_BOOT_RESET_LIMIT               3

/* -----------------------------------------------------------------------------

   Boot state Machine

----------------------------------------------------------------------------- */


#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

static void static_PNFCControllerFirmwareUpdateDriverCompleted(
      tContext* pContext,
      void * pCallbackParameter,
      W_ERROR nResult )
{
   tDFCCallbackContext * pCallbackContext = (tDFCCallbackContext *) pCallbackParameter;

#if (P_BUILD_CONFIG == P_CONFIG_USER)
   W_ERROR nError;
#endif


#if (P_BUILD_CONFIG == P_CONFIG_USER)

    nError = PNFCControllerUserReadInfo(pContext);

    if (nError != W_SUCCESS)
    {
       nResult = nError;
    }
#endif

   PDFCPostContext2(pCallbackContext, P_DFC_TYPE_NFCC, nResult);

   CMemoryFree(pCallbackContext);
}



/* See Client API Specifications */
void PNFCControllerFirmwareUpdate(
                  tContext* pContext,
                  tPBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  const uint8_t* pUpdateBuffer,
                  uint32_t nUpdateBufferLength,
                  uint32_t nMode )
{
   tDFCCallbackContext * pCallbackContext;
   W_ERROR nError;

   pCallbackContext = CMemoryAlloc(sizeof(tDFCCallbackContext));

   if (pCallbackContext != null)
   {
      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, pCallbackContext);

      PNFCControllerFirmwareUpdateDriver(pContext, static_PNFCControllerFirmwareUpdateDriverCompleted, pCallbackContext, pUpdateBuffer, nUpdateBufferLength, nMode);

      nError = PContextGetLastIoctlError(pContext);

      if (nError != W_SUCCESS)
      {
         PDFCPostContext2(pCallbackContext, P_DFC_TYPE_NFCC, nError);
         return;
      }
   }
   else
   {
      tDFCCallbackContext sCallbackContext;

      PDFCFillCallbackContext(pContext, (tDFCCallback *) pCallback, pCallbackParameter, &sCallbackContext);
      PDFCPostContext2(&sCallbackContext, P_DFC_TYPE_NFCC, W_ERROR_OUT_OF_RESOURCE);
   }
}

#endif

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* The Persistent Memory Default value */
static const uint8_t g_aPersistentMemoryDefaultValue[] =
{
   'O', 'N', 'F', 'C', 0x30, 0x00,    /* Header */
   W_PRIORITY_NO_ACCESS,              /* UICC has no access to the reader ISO 14443 4 A */
   W_PRIORITY_NO_ACCESS               /* UICC has no access to the reader ISO 14443 4 B */
};

static const uint8_t g_aDefaultPulseParameter[] =
{
   (NAL_PAR_DETECT_PULSE_DEFAULT_VALUE >> 8) & 0xFF,
   NAL_PAR_DETECT_PULSE_DEFAULT_VALUE & 0xFF
};


/* See header file */
void PNFCControllerWritePersistentMemoryBuffer(
            uint8_t* pBuffer,
            tNFCControllerPolicyParameters* pPolicyParameter)
{
   CMemoryCopy(pBuffer,
      g_aPersistentMemoryDefaultValue, sizeof(g_aPersistentMemoryDefaultValue));

   pBuffer[6] = pPolicyParameter->nUICCReaderISO14443_4_A_Priority;
   pBuffer[7] = pPolicyParameter->nUICCReaderISO14443_4_B_Priority;
}

static void static_PNFCControllerBootMachine(
         tContext*      pContext,
         uint32_t       nEvent,
         uint8_t*       pBuffer,
         uint32_t       nLength,
         W_ERROR nError );

static P_INLINE void static_PNFCControllerBootMachineDefault(
         tContext*      pContext,
         uint32_t       nEvent)
{
   static_PNFCControllerBootMachine(pContext, nEvent, null, 0, W_SUCCESS);
}

/**
 * Service   C O N N E C T E D   Callback
 */
static void static_PNFCControllerBootServiceConnectCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError )
{
   static_PNFCControllerBootMachine(pContext, NFCC_EVENT_SERVICE_CONNECTED, null, 0, nError);
}

/**  Get Parameter callback function */
static void static_PNFCControllerBootGetParameterCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            uint32_t nLength,
            W_ERROR nError)
{
   static_PNFCControllerBootMachine(
      pContext, NFCC_EVENT_COMMAND_EXECUTED,
      ((tNFCController*)pCallbackParameter)->aNALDataBuffer, nLength, nError);
}

/**  Set Parameter callback function */
static void static_PNFCControllerBootSetParameterCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   static_PNFCControllerBootMachine(
      pContext, NFCC_EVENT_COMMAND_EXECUTED,
      null, 0, nError);
}

/**  SE Initialization callback function */
static void static_PNFCControllerBootSEInitializationCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_ERROR nError)
{
   static_PNFCControllerBootMachine(
      pContext, NFCC_EVENT_COMMAND_EXECUTED,
      null, 0, nError);
}

/**
 * Time Out ELAPSED Callback
 */
static void static_PNFCControllerBootTimeoutElapsed(
               tContext* pContext,
               void* pCallbackParameter)
{
   static_PNFCControllerBootMachineDefault(pContext, NFCC_EVENT_TIMEOUT_ELAPSED);
}

/**
 * Time Out SWP Callback
 */
static void static_PNFCControllerBootTimeoutSWP(
               tContext* pContext,
               void* pCallbackParameter)
{
   static_PNFCControllerBootMachineDefault(pContext, NFCC_EVENT_COMMAND_EXECUTED);
}

/**
 * Execute Get Parameter & start timer
 */
static void static_PNFCControllerBootGetParameter(
               tContext* pContext,
               tNFCController* pNFCController,
               uint8_t nServiceIdentifier,
               uint8_t nParameterCode )
{
   PNALServiceGetParameter(
         pContext,
         nServiceIdentifier,
         &pNFCController->sBootServiceOperation,
         nParameterCode,
         pNFCController->aNALDataBuffer, sizeof(pNFCController->aNALDataBuffer),
         static_PNFCControllerBootGetParameterCompleted, pNFCController);

   PMultiTimerSet( pContext, TIMER_T13_NFCC_BOOT, TIMER_T13_TIMEOUT_BOOT,
      &static_PNFCControllerBootTimeoutElapsed, null );
}


/**
 * Execute Set Parameter & start timer
 */
static void static_PNFCControllerBootSetParameter(
               tContext* pContext,
               tNFCController* pNFCController,
               uint8_t nServiceIdentifier,
               uint8_t nParameterCode,
               const uint8_t* pBuffer,
               uint32_t  nLength )
{
   PNALServiceSetParameter(
      pContext,
      nServiceIdentifier,
      &pNFCController->sBootServiceOperation,
      nParameterCode,
      pBuffer, nLength,
      static_PNFCControllerBootSetParameterCompleted, pNFCController );

   PMultiTimerSet( pContext, TIMER_T13_NFCC_BOOT, TIMER_T13_TIMEOUT_BOOT,
      &static_PNFCControllerBootTimeoutElapsed, null );
}

/**
 * Pre-reset the layer above and bellow this component.
 *
 * @param[in]  pContext  The context.
 **/
static void static_PNFCControllerPreReset(
            tContext* pContext )
{

   /* Makes sure the reader registry is disconnected */
   PReaderDriverDisconnect(pContext);

   /* Pre-reset the NFC HAL stack */
   PNALServicePreReset( pContext );
}

/**
 * NFC Controller Boot Complete
 */
static void static_PNFCControllerBootComplete(
            tContext* pContext,
            uint32_t nMode,
            W_ERROR nError )
{
   tNFCController* pNFCController = PContextGetNFCController( pContext );
   pNFCController->nState = NFCC_BOOT_STATE_START_SWP_2;

   PNALServiceSetVariable(pContext, NAL_PARAM_MODE, nMode);

   /* No communication after these modes */
   if ( (nMode == W_NFCC_MODE_NOT_RESPONDING) || (nMode==W_NFCC_MODE_LOADER_NOT_SUPPORTED) )
   {
      static_PNFCControllerPreReset( pContext );
   }

   pNFCController->pUpdateBuffer = null;
   pNFCController->nUpdateBufferLength = 0;
   pNFCController->nUpdateBufferIndex = 0;

   if(pNFCController->bInitialReset == false)
   {
      if(pNFCController->nUpdateError != W_SUCCESS)
      {
         nError = pNFCController->nUpdateError;
      }
      PDFCDriverPostCC2( pNFCController->pBootDriverCC, P_DFC_TYPE_NFCC, nError );
   }
   else
   {
      if(nError != W_SUCCESS)
      {
         PDebugError("static_PNFCControllerBootComplete: Error received %s",
            PUtilTraceError(nError));
      }

      if(pNFCController->pResetCallback != null)
      {
         /* Direct call since we are in a callback */
         pNFCController->pResetCallback(pNFCController->pCompletionCallbackParameter, nMode);
      }

      pNFCController->bInitialReset = false;
   }

   if((nMode == W_NFCC_MODE_ACTIVE) && (nError == W_SUCCESS))
   {
      tUICCInstance* pUICCInstance = PContextGetUICCInstance(pContext);

      /* reset the UICC policy cache */
      pUICCInstance->bUICCInfoIsBuilt = false;
   }
}

/* See header file */
static void static_PNFCControllerPerformInitialReset(
         tContext* pContext,
         uint32_t nMode)
{
   tNFCController* pNFCController = PContextGetNFCController( pContext );
   tNFCControllerInfo* pNFCControllerInfo = PContextGetNFCControllerInfo(pContext);

   CMemoryFill(pNFCControllerInfo, 0, sizeof(tNFCControllerInfo));

   pNFCController->nTargetMode = nMode;
   pNFCController->nResetCount = 0;

   static_PNFCControllerPreReset( pContext );

   static_PNFCControllerBootMachineDefault( pContext, NFCC_EVENT_INITIAL );
}

/* See header file */
void PNFCControllerPerformInitialReset(
               tContext* pContext,
               tPNFCControllerInitialResetCompletionCallback* pCompletionCallback,
               void* pCompletionCallbackParameter,
               bool bForceReset )
{
   tNFCController* pNFCController = PContextGetNFCController( pContext );

   pNFCController->bInitialReset = true;
   pNFCController->pResetCallback = pCompletionCallback;
   pNFCController->pCompletionCallbackParameter = pCompletionCallbackParameter;

   pNFCController->bForceReset = bForceReset;

   static_PNFCControllerPerformInitialReset(pContext,
         W_NFCC_MODE_ACTIVE);
}

/**
 * Converts the FSDI value into a FSD value.
 *
 * See ISO/IEC 14443-4 Chapter 5.1
 *
 * @param[in]  nFSDI  The FSDI value.
 *
 * @return the FSD value in bytes.
 **/
static uint32_t static_PNFCControllerFSDIToFSD(
               uint32_t nFSDI)
{
   uint32_t nFSD;

   switch(nFSDI)
   {
      case 0:
         nFSD = 16;
         break;
      case 1:
         nFSD = 24;
         break;
      case 2:
         nFSD = 32;
         break;
      case 3:
         nFSD = 40;
         break;
      case 4:
         nFSD = 48;
         break;
      case 5:
         nFSD = 64;
         break;
      case 6:
         nFSD = 96;
         break;
      case 7:
         nFSD = 128;
         break;
      case 8:
      default:
         nFSD = 256;
         break;
   }

   return nFSD;
}

/** @see  PNALServiceExecuteCommand **/
static void static_PNFCControllerBootExecuteCommandCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            uint32_t nLength,
            W_ERROR nError,
            uint32_t nReceptionCounter)
{
   static_PNFCControllerBootMachine(
      pContext, NFCC_EVENT_COMMAND_EXECUTED,
      null, 0, nError);
}

/**
 * NFC Controller Boot Machine State
 */
static void static_PNFCControllerBootMachine(
               tContext* pContext,
               uint32_t nEvent,
               uint8_t* pNALDataBuffer,
               uint32_t nLength,
               W_ERROR nError )
{
   tNFCController* pNFCController = PContextGetNFCController( pContext );
   tNFCControllerInfo* pNFCControllerInfo = PContextGetNFCControllerInfo(pContext);
   uint32_t nSWPBootTimeout = P_UICC_SWP_BOOT_TIMEOUT;

   if(pNALDataBuffer != null)
   {
      CDebugAssert(pNALDataBuffer == pNFCController->aNALDataBuffer);
   }
   else
   {
      pNALDataBuffer = pNFCController->aNALDataBuffer;
   }

   PMultiTimerCancel( pContext, TIMER_T13_NFCC_BOOT );

   switch ( nEvent )
   {
   case NFCC_EVENT_INITIAL:
      pNFCController->nState = NFCC_BOOT_STATE_INIT;
      PNALServiceSetVariable(pContext, NAL_PARAM_MODE, W_NFCC_MODE_BOOT_PENDING);
      pNFCController->bBootedForSEandUICC = false;
      break;

   case NFCC_EVENT_SERVICE_CONNECTED:

      pNFCController->nState &= ~NFCC_STATUS_PENDING_CONNECTED;
      break;

   case NFCC_EVENT_COMMAND_EXECUTED:

      if ( (pNFCController->nState & NFCC_STATUS_PENDING_CANCEL) != 0x00 )
      {
         /* Boot stopped */
         PDebugError("PNFCControllerBoot: NFC Controller not responding.");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_NOT_RESPONDING, W_ERROR_TIMEOUT );
         return;
      }

      pNFCController->nState &= ~NFCC_STATUS_PENDING_EXECUTED;
      break;

   case NFCC_EVENT_TIMEOUT_ELAPSED:

      if ( pNFCController->nResetCount < NFCC_BOOT_RESET_LIMIT )
      {
         ++pNFCController->nResetCount;

         PNFCControllerNotifyException(pContext);

         static_PNFCControllerPreReset( pContext );

         pNFCController->nState = NFCC_BOOT_STATE_INIT;
         PNALServiceSetVariable(pContext, NAL_PARAM_MODE, W_NFCC_MODE_BOOT_PENDING);
         pNFCController->bBootedForSEandUICC = false;
      }
      else
      {
         if ( (pNFCController->nState & NFCC_STATUS_PENDING_EXECUTED) != 0x00 )
         {
            pNFCController->nState &= ~NFCC_STATUS_PENDING_EXECUTED;
            pNFCController->nState |= NFCC_STATUS_PENDING_CANCEL;

            PNALServiceCancelOperation(
               pContext,
               &pNFCController->sBootServiceOperation );

            return;
         }
         /* Boot stopped */
         PDebugError("PNFCControllerBoot: NFC Controller not responding.");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_NOT_RESPONDING, W_ERROR_TIMEOUT );
         return;
      }
      break;
   }

   switch ( pNFCController->nState )
   {
   case  NFCC_BOOT_STATE_INIT:

      PNALServiceConnect(
         pContext,
         NAL_SIGNAL_RESET,
         static_PNFCControllerBootServiceConnectCompleted, null );

      pNFCController->nState = NFCC_BOOT_STATE_GET_HARDWARE_INFO | NFCC_STATUS_PENDING_CONNECTED;
      PMultiTimerSet( pContext, TIMER_T13_NFCC_BOOT, TIMER_T13_TIMEOUT_BOOT,
         &static_PNFCControllerBootTimeoutElapsed, null );
      return;

   case  NFCC_BOOT_STATE_GET_HARDWARE_INFO:

      /* Check connect result */
      if ( nError != W_SUCCESS )
      {
         PDebugError("PNFCControllerBoot: NFC HAL Connect returns %s", PUtilTraceError(nError));

         if(nError != W_ERROR_DURING_FIRMWARE_BOOT)
         {
            /* Boot stopped */
            static_PNFCControllerBootComplete( pContext,
                  W_NFCC_MODE_NOT_RESPONDING, W_SUCCESS );
            return;
         }
      }

      /**
       * Get the hardware info parameter
       */
      static_PNFCControllerBootGetParameter(
         pContext,
         pNFCController,
         NAL_SERVICE_ADMIN,
         NAL_PAR_HARDWARE_INFO);

      pNFCController->nState = NFCC_BOOT_STATE_GET_FIRMWARE_INFO | NFCC_STATUS_PENDING_EXECUTED;
      return;

   case  NFCC_BOOT_STATE_GET_FIRMWARE_INFO:
   {
      uint32_t nIndex = 0;
      uint32_t nSlotIdentifier;

      if ( (nError == W_SUCCESS) && (nLength < 1) )
      {
         PDebugError("PNFCControllerBoot: Buffer too short");
         nError = W_ERROR_NFC_HAL_COMMUNICATION;
      }

      /* Check connect result */
      if ( nError != W_SUCCESS )
      {
         PDebugError("PNFCControllerBoot: NFC HAL get hardware info returns %s", PUtilTraceError(nError));

         /* Boot stopped */
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_NOT_RESPONDING, W_SUCCESS );
         return;
      }

      if(pNALDataBuffer[nIndex++] != NAL_VERSION)
      {
         PDebugError("PNFCControllerBoot: Loader NFC HAL version not supported");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_LOADER_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      if(nLength != NAL_PAR_HARDWARE_INFO_SIZE)
      {
         PDebugError("PNFCControllerBoot: Buffer too short");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_LOADER_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      CMemoryCopy(pNFCControllerInfo->aHardwareVersion, &pNALDataBuffer[nIndex], NAL_HARDWARE_TYPE_STRING_SIZE);
      nIndex += NAL_HARDWARE_TYPE_STRING_SIZE;
      PDebugLog("Hardware Type: %s", pNFCControllerInfo->aHardwareVersion);

      CMemoryCopy(pNFCControllerInfo->aHardwareSerialNumber, &pNALDataBuffer[nIndex], NAL_HARDWARE_SERIAL_NUMBER_STRING_SIZE);
      nIndex += NAL_HARDWARE_SERIAL_NUMBER_STRING_SIZE;
      PDebugLog("Hardware Serial Number: %s", pNFCControllerInfo->aHardwareSerialNumber);

      CMemoryCopy(pNFCControllerInfo->aLoaderVersion, &pNALDataBuffer[nIndex], NAL_LOADER_DESCRIPTION_STRING_SIZE);
      nIndex += NAL_LOADER_DESCRIPTION_STRING_SIZE;
      PDebugLog("Loader Version: %s", pNFCControllerInfo->aLoaderVersion);

      /* Check the presence of the firmware */
      if(pNALDataBuffer[nIndex++] == 0x00)
      {
         PDebugLog("No Firmware");
         static_PNFCControllerBootComplete( pContext, W_NFCC_MODE_NO_FIRMWARE, W_SUCCESS );
         return;
      }
      else
      {
         PDebugLog("Firmware Present");
      }

      pNFCControllerInfo->nSENumber = pNALDataBuffer[nIndex++];
      if(pNFCControllerInfo->nSENumber > NAL_MAXIMUM_SE_NUMBER)
      {
         PDebugError("PNFCControllerBoot: Too many SE declared (%d)", pNFCControllerInfo->nSENumber);
         nError = W_ERROR_NFC_HAL_COMMUNICATION;
      }

      for(nSlotIdentifier = 0; nSlotIdentifier < pNFCControllerInfo->nSENumber; nSlotIdentifier++)
      {
         uint16_t nNALCapabilities;
         uint32_t nCapabilities = 0;
         uint32_t nProtocols;

         CMemoryCopy(pNFCControllerInfo->aSEInfoArray[nSlotIdentifier].aDescription, &pNALDataBuffer[nIndex], NAL_SE_DESCRIPTION_STRING_SIZE);
         nIndex += NAL_SE_DESCRIPTION_STRING_SIZE;
         PDebugLog("SE #%d Description: %s", nSlotIdentifier, pNFCControllerInfo->aSEInfoArray[nSlotIdentifier].aDescription);

         nNALCapabilities = PNALReadUint16FromBuffer(&pNALDataBuffer[nIndex]);
         nIndex += 2;
         if((nNALCapabilities & NAL_SE_FLAG_END_OF_TRANSACTION_NOTIFICATION) != 0)
         {
            nCapabilities |= W_SE_FLAG_END_OF_TRANSACTION_NOTIFICATION;
         }
         if((nNALCapabilities & NAL_SE_FLAG_REMOVABLE) != 0)
         {
            nCapabilities |= W_SE_FLAG_REMOVABLE;
         }
         if((nNALCapabilities & NAL_SE_FLAG_HOT_PLUG) != 0)
         {
            nCapabilities |= W_SE_FLAG_HOT_PLUG;
         }

         nProtocols = PNALReadReaderProtocols(&pNALDataBuffer[nIndex]);
         if(nProtocols != 0)
         {
            nCapabilities |= W_SE_FLAG_COMMUNICATION;
         }
         pNFCControllerInfo->aSEInfoArray[nSlotIdentifier].nCapabilities = nCapabilities;
         PDebugLog("SE #%d Capabilities : 0x%08X", nSlotIdentifier, nCapabilities);

         nProtocols |= PNALReadCardProtocols(&pNALDataBuffer[nIndex + 2]);
         pNFCControllerInfo->aSEInfoArray[nSlotIdentifier].nProtocols = nProtocols;
         nIndex += 4;
         PDebugLog("SE #%d Host Interface : %s", nSlotIdentifier, PUtilTraceReaderProtocol(pContext, nProtocols));
         PDebugLog("SE #%d RF Interface : %s", nSlotIdentifier, PUtilTraceCardProtocol(pContext, nProtocols));
      }

      /**
       * Get the firmware info parameter
       */
      static_PNFCControllerBootGetParameter(
         pContext,
         pNFCController,
         NAL_SERVICE_ADMIN,
         NAL_PAR_FIRMWARE_INFO);

      pNFCController->nState = NFCC_BOOT_STATE_GET_PERSISTENT_POLICY | NFCC_STATUS_PENDING_EXECUTED;
      return;
   }

   case  NFCC_BOOT_STATE_GET_PERSISTENT_POLICY:
   {
      uint32_t nIndex = 0;

      if ( (nError == W_SUCCESS) && (nLength < 1) )
      {
         PDebugError("PNFCControllerBoot: Buffer too short");
         nError = W_ERROR_NFC_HAL_COMMUNICATION;
      }

      /* Check connect result */
      if ( nError != W_SUCCESS )
      {
         PDebugError("PNFCControllerBoot: NFC HAL get firmware info returns %s", PUtilTraceError(nError));

         /* Boot stopped */
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      pNFCControllerInfo->nNALVersion = pNALDataBuffer[nIndex++];
      if(pNFCControllerInfo->nNALVersion != NAL_VERSION)
      {
         PDebugError("PNFCControllerBoot: Firmware NFC HAL version not supported");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      if(nLength != NAL_PAR_FIRMWARE_INFO_SIZE)
      {
         PDebugError("PNFCControllerBoot: invalid buffer length");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      CMemoryCopy(pNFCControllerInfo->aFirmwareVersion, &pNALDataBuffer[nIndex], NAL_FIRMWARE_DESCRIPTION_STRING_SIZE);
      nIndex += NAL_FIRMWARE_DESCRIPTION_STRING_SIZE;
      PDebugLog("Firmware Version: %s", pNFCControllerInfo->aFirmwareVersion);

      pNFCControllerInfo->nProtocolCapabilities = PNALReadCardProtocols(&pNALDataBuffer[nIndex]);
      nIndex += 2;
      pNFCControllerInfo->nProtocolCapabilities |= PNALReadReaderProtocols(&pNALDataBuffer[nIndex]);
      nIndex += 2;
      PDebugLog("Reader Capabilities : %s", PUtilTraceReaderProtocol(pContext, pNFCControllerInfo->nProtocolCapabilities));
      PDebugLog("Card Capabilities : %s", PUtilTraceCardProtocol(pContext, pNFCControllerInfo->nProtocolCapabilities));

      pNFCControllerInfo->nFirmwareCapabilities = PNALReadUint16FromBuffer(&pNALDataBuffer[nIndex]);
      nIndex += 2;
      PDebugLog("Firmware Capabilities : 0x%04X", pNFCControllerInfo->nFirmwareCapabilities);

      pNFCControllerInfo->nReaderISO14443_A_MaxRate = pNALDataBuffer[nIndex++];
      PDebugLog("Reader ISO 14443 A Max Rate : %d", pNFCControllerInfo->nReaderISO14443_A_MaxRate);

      pNFCControllerInfo->nReaderISO14443_A_InputSize = static_PNFCControllerFSDIToFSD(pNALDataBuffer[nIndex++]);
      PDebugLog("Reader ISO 14443 A Input Size : %d", pNFCControllerInfo->nReaderISO14443_A_InputSize);

      pNFCControllerInfo->nReaderISO14443_B_MaxRate = pNALDataBuffer[nIndex++];
      PDebugLog("Reader ISO 14443 B Max Rate : %d", pNFCControllerInfo->nReaderISO14443_B_MaxRate);

      pNFCControllerInfo->nReaderISO14443_B_InputSize = static_PNFCControllerFSDIToFSD(pNALDataBuffer[nIndex++]);
      PDebugLog("Reader ISO 14443 B Input Size : %d", pNFCControllerInfo->nReaderISO14443_B_InputSize);

      pNFCControllerInfo->nCardISO14443_A_MaxRate = pNALDataBuffer[nIndex++];
      PDebugLog("Card ISO 14443 A Max Rate : %d", pNFCControllerInfo->nCardISO14443_A_MaxRate);

      pNFCControllerInfo->nCardISO14443_B_MaxRate = pNALDataBuffer[nIndex++];
      PDebugLog("Card ISO 14443 B Max Rate : %d", pNFCControllerInfo->nCardISO14443_B_MaxRate);

      pNFCControllerInfo->nAutoStandbyTimeout = PNALReadUint16FromBuffer(&pNALDataBuffer[nIndex]);
      nIndex += 2;
      PDebugLog("Auto standby timeout : %d", pNFCControllerInfo->nAutoStandbyTimeout);

      CDebugAssert(nIndex == NAL_PAR_FIRMWARE_INFO_SIZE);
      /**
       * Get the persistent info parameter
       */
      static_PNFCControllerBootGetParameter(
         pContext,
         pNFCController,
         NAL_SERVICE_ADMIN,
         NAL_PAR_PERSISTENT_POLICY);

      pNFCController->nState = NFCC_BOOT_STATE_GET_PERSISTENT_MEMORY | NFCC_STATUS_PENDING_EXECUTED;
      return;
   }

   case  NFCC_BOOT_STATE_GET_PERSISTENT_MEMORY:
      {
         uint16_t nFlags;
         tNFCControllerPolicyParameters* pPolicy = &pNFCController->sPolicyMonitor.sPersistent;

         if ( (nError == W_SUCCESS) && (nLength != NAL_POLICY_SIZE) )
         {
            PDebugError("PNFCControllerBoot: Invalid length");
            nError = W_ERROR_NFC_HAL_COMMUNICATION;
         }

         /* Check connect result */
         if ( nError != W_SUCCESS )
         {
            PDebugError("PNFCControllerBoot: NFC HAL get persistent policy returns %s", PUtilTraceError(nError));

            /* Boot stopped */
            static_PNFCControllerBootComplete( pContext,
                  W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
            return;
         }

         pPolicy->nUICCProtocolPolicy = PNALReadCardProtocols(&pNALDataBuffer[0]);
         pPolicy->nUICCProtocolPolicy |= PNALReadReaderProtocols(&pNALDataBuffer[2]);

         PDebugLog("Persistent UICC Card Policy : %s", PUtilTraceCardProtocol(pContext, pPolicy->nUICCProtocolPolicy));
         PDebugLog("Persistent UICC Reader Policy : %s", PUtilTraceReaderProtocol(pContext, pPolicy->nUICCProtocolPolicy));

         nFlags = PNALReadUint16FromBuffer(&pNALDataBuffer[4]);

         pPolicy->bReaderRFLock = ((nFlags & NAL_POLICY_FLAG_READER_LOCK) != 0)?false:true;
         pPolicy->bCardRFLock = ((nFlags & NAL_POLICY_FLAG_CARD_LOCK) != 0)?false:true;

         if(pPolicy->bReaderRFLock)
         {
            PDebugLog("Persistent Reader RF: locked");
         }
         else
         {
            PDebugLog("Persistent Reader RF: active");
         }

         if(pPolicy->bCardRFLock)
         {
            PDebugLog("Persistent Card RF: locked");
         }
         else
         {
            PDebugLog("Persistent Card RF: active");
         }

         if((nFlags & NAL_POLICY_FLAG_SE_MASK) == NAL_POLICY_FLAG_SE_OFF)
         {
            PDebugLog("Persistent SE Position: OFF");
            pPolicy->sSEPolicy.nSESwitchPosition = P_SE_SWITCH_OFF;
         }
         else if((nFlags & NAL_POLICY_FLAG_SE_MASK) == NAL_POLICY_FLAG_RF_INTERFACE)
         {
            PDebugLog("Persistent SE Position: RF Interface");
            pPolicy->sSEPolicy.nSESwitchPosition = P_SE_SWITCH_RF_INTERFACE;
         }
         else /* Host Interface is not supported for the persistent policy */
         {
            PDebugError("PNFCControllerBoot: Error in the SE Position");
            static_PNFCControllerBootComplete( pContext,
                  W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
            return;
         }

         pPolicy->sSEPolicy.nSlotIdentifier = (nFlags & 0x0030) >> 4;
         PDebugLog("Persistent SE Slot: %d", pPolicy->sSEPolicy.nSlotIdentifier);

         pPolicy->sSEPolicy.nSEProtocolPolicy = PNALReadCardProtocols(&pNALDataBuffer[6]);
         PDebugLog("Persistent SE Card Policy : %s", PUtilTraceCardProtocol(pContext, pPolicy->sSEPolicy.nSEProtocolPolicy));

         nError = PNFCControllerCheckPersistentPolicy(pContext);
         if(nError != W_SUCCESS)
         {
            PDebugError("PNFCControllerBoot: Error %s in the card emulation policy", PUtilTraceError(nError));
            static_PNFCControllerBootComplete( pContext,
                  W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
            return;
         }
      }

      static_PNFCControllerBootSetParameter(
               pContext, pNFCController,
               NAL_SERVICE_ADMIN, NAL_PAR_DETECT_PULSE,
               g_aDefaultPulseParameter, sizeof(g_aDefaultPulseParameter));

      pNFCController->nState = NFCC_BOOT_STATE_SET_PULSE | NFCC_STATUS_PENDING_EXECUTED;
      return;

   case NFCC_BOOT_STATE_SET_PULSE :

      if(pNFCController->bForceReset == false)
      {
         /* Get the persistent info parameter */
         static_PNFCControllerBootGetParameter(
            pContext, pNFCController,
            NAL_SERVICE_ADMIN, NAL_PAR_PERSISTENT_MEMORY);

         pNFCController->nState = NFCC_BOOT_STATE_START_SWP_2 | NFCC_STATUS_PENDING_EXECUTED;
      }
      else
      {
         /* Set the persistent info parameter */
         static_PNFCControllerBootSetParameter(
            pContext, pNFCController,
            NAL_SERVICE_ADMIN, NAL_PAR_PERSISTENT_MEMORY,
            g_aPersistentMemoryDefaultValue, sizeof(g_aPersistentMemoryDefaultValue));

         pNFCController->nState = NFCC_BOOT_STATE_START_SWP_1 | NFCC_STATUS_PENDING_EXECUTED;
      }

      return;

   case  NFCC_BOOT_STATE_START_SWP_1:

      if ( nError != W_SUCCESS )
      {
         PDebugError("PNFCControllerBoot: Cannot write the persistent memory: %s", PUtilTraceError(nError));

         /* Boot stopped */
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      /* Simulate a successful read operation */
      pNFCController->nState = NFCC_BOOT_STATE_START_SWP_2 | NFCC_STATUS_PENDING_EXECUTED;
      CMemoryCopy(pNALDataBuffer,
         g_aPersistentMemoryDefaultValue, sizeof(g_aPersistentMemoryDefaultValue));

      static_PNFCControllerBootMachine( pContext,
               NFCC_EVENT_COMMAND_EXECUTED,
               pNALDataBuffer, sizeof(g_aPersistentMemoryDefaultValue),
               W_SUCCESS );
      return;

   case  NFCC_BOOT_STATE_START_SWP_2:

      if ( (nError == W_SUCCESS) && (nLength != sizeof(g_aPersistentMemoryDefaultValue)) )
      {
         PDebugError("PNFCControllerBoot: Invalid length");
         nError = W_ERROR_NFC_HAL_COMMUNICATION;
      }

      /* Check the result */
      if ( nError != W_SUCCESS )
      {
         PDebugError("PNFCControllerBoot: NFC HAL get persistent memory returns %s", PUtilTraceError(nError));

         /* Boot stopped */
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      if(CMemoryCompare(pNALDataBuffer, g_aPersistentMemoryDefaultValue, 6) != 0)
      {
         if(pNFCController->bForceReset == false)
         {
            PDebugWarning("PNFCControllerBoot: The persistent memory is not initialized");
            /* Set the persistent info parameter */
            static_PNFCControllerBootSetParameter(
               pContext, pNFCController,
               NAL_SERVICE_ADMIN, NAL_PAR_PERSISTENT_MEMORY,
               g_aPersistentMemoryDefaultValue, sizeof(g_aPersistentMemoryDefaultValue));

            pNFCController->bForceReset = true;
            pNFCController->nState = NFCC_BOOT_STATE_START_SWP_1 | NFCC_STATUS_PENDING_EXECUTED;
         }
         else
         {
            PDebugError("PNFCControllerBoot: Error in the persitent memory content");
            /* Boot stopped */
            static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         }
         return;
      }

      /* Reset the force persistent flag (if it is set) */
      pNFCController->bForceReset = false;

      pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority = pNALDataBuffer[6];
      pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority = pNALDataBuffer[7];

      if((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority != W_PRIORITY_NO_ACCESS)
         && ((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority < W_PRIORITY_MINIMUM)
         || (pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority > W_PRIORITY_MAXIMUM)))
      {
         PDebugError("PNFCControllerBoot: bad UICC reader A priority");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      if((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority != W_PRIORITY_NO_ACCESS)
      && ((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority < W_PRIORITY_MINIMUM)
      || (pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority > W_PRIORITY_MAXIMUM)))
      {
         PDebugError("PNFCControllerBoot: bad UICC reader B priority");
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      PDebugLog("UICC Reader ISO 14443 4 A Priority: %s",
         PUtilTracePriority(pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority));
      PDebugLog("UICC Reader ISO 14443 4 B Priority: %s",
         PUtilTracePriority(pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority));

      /* check consistency between priority and access policy */

      if ( ((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority == W_PRIORITY_NO_ACCESS) &&
               (pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy & W_NFCC_PROTOCOL_READER_ISO_14443_4_A) != 0)
        || ((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority != W_PRIORITY_NO_ACCESS) &&
               (pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy & W_NFCC_PROTOCOL_READER_ISO_14443_4_A) == 0)
        || ((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority == W_PRIORITY_NO_ACCESS) &&
               (pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy & W_NFCC_PROTOCOL_READER_ISO_14443_4_B) != 0)
        || ((pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority != W_PRIORITY_NO_ACCESS) &&
               (pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy & W_NFCC_PROTOCOL_READER_ISO_14443_4_B) == 0))
      {
         /* Inconsistency detected between the reader priority stored in the persistent memory and UICC persistent policy
            This can occur if a crash occured during last set policy operation.

            Since the reader priorities are stored in the persistent memory before the persistent policy, we set the
            persistent and current policy according to the reader priority values */

         PDebugWarning("Inconsistency between reader priority and persistent policy");

         if (pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_A_Priority == W_PRIORITY_NO_ACCESS)
               pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy &= ~W_NFCC_PROTOCOL_READER_ISO_14443_4_A;
         else
               pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy |= W_NFCC_PROTOCOL_READER_ISO_14443_4_A;


         if (pNFCController->sPolicyMonitor.sPersistent.nUICCReaderISO14443_4_B_Priority == W_PRIORITY_NO_ACCESS)
            pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy &= ~W_NFCC_PROTOCOL_READER_ISO_14443_4_B;
         else
            pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy |= W_NFCC_PROTOCOL_READER_ISO_14443_4_B;

         /* restore the current policy */

         PNALWriteCardProtocols(pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy, &pNFCController->aNALDataBuffer[0]);
         PNALWriteReaderProtocols(pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy, &pNFCController->aNALDataBuffer[2]);

         {
            uint16_t nFlags = 0;
            nFlags |= pNFCController->sPolicyMonitor.sPersistent.bReaderRFLock  == false ? NAL_POLICY_FLAG_READER_LOCK : 0;
            nFlags |= pNFCController->sPolicyMonitor.sPersistent.bCardRFLock  == false ? NAL_POLICY_FLAG_CARD_LOCK : 0;

            switch (pNFCController->sPolicyMonitor.sPersistent.sSEPolicy.nSESwitchPosition)
            {
               case P_SE_SWITCH_OFF    : nFlags |= NAL_POLICY_FLAG_SE_OFF;  break;
               case P_SE_SWITCH_RF_INTERFACE   : nFlags |= NAL_POLICY_FLAG_RF_INTERFACE; break;
               case P_SE_SWITCH_FORCED_HOST_INTERFACE : nFlags |= NAL_POLICY_FLAG_FORCED_HOST_INTERFACE; break;
               case P_SE_SWITCH_HOST_INTERFACE : nFlags |= NAL_POLICY_FLAG_HOST_INTERFACE; break;
            }

            nFlags |= pNFCController->sPolicyMonitor.sPersistent.sSEPolicy.nSlotIdentifier << 4;

            PNALWriteUint16ToBuffer(nFlags, &pNFCController->aNALDataBuffer[4]);
         }

         PNALWriteCardProtocols(pNFCController->sPolicyMonitor.sPersistent.sSEPolicy.nSEProtocolPolicy, &pNFCController->aNALDataBuffer[6]);

         static_PNFCControllerBootSetParameter(pContext,pNFCController, NAL_SERVICE_ADMIN , NAL_PAR_POLICY, pNFCController->aNALDataBuffer, NAL_POLICY_SIZE);
         pNFCController->nState = NFCC_BOOT_STATE_SET_CURRENT_POLICY_DONE | NFCC_STATUS_PENDING_EXECUTED;

         return;
      }

      /* if all is OK, walk through NFCC_BOOT_STATE_START_SWP_3 */

   case  NFCC_BOOT_STATE_START_SWP_3:

      /* Initializes the reader registry */
      nError = PReaderDriverConnect(pContext, pNFCControllerInfo->nProtocolCapabilities);

      if(nError != W_SUCCESS)
      {
         PDebugError("PNFCControllerBoot: PReaderDriverConnect() in error %s",
            PUtilTraceError(nError));

         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );

         return;
      }

      /* Set the pre boot value to activate the NFCC functions needed for the boot of the SEs */
      pNFCController->bBootedForSEandUICC = true;

      /* Copy the persistent policy into the volatile policy */
      pNFCController->sPolicyMonitor.sVolatile = pNFCController->sPolicyMonitor.sPersistent;

      /* set the new policy to the same value as the active fields */
      pNFCController->sPolicyMonitor.sNewPersistent = pNFCController->sPolicyMonitor.sPersistent;
      pNFCController->sPolicyMonitor.sNewVolatile   = pNFCController->sPolicyMonitor.sVolatile;

      /* Initialize the Secure Element(s) */
      PSEDriverInitialize(
                  pContext,
                  static_PNFCControllerBootSEInitializationCompleted,
                  null);

      pNFCController->nState = NFCC_BOOT_STATE_START_SE | NFCC_STATUS_PENDING_EXECUTED;
      return;

   case NFCC_BOOT_STATE_START_SE:

      /* Check connect result */
      if ( nError != W_SUCCESS )
      {
         PDebugError("PNFCControllerBoot: SE Initalization returns %s", PUtilTraceError(nError));

         PDebugWarning("PNFCControllerBoot: Continue boot procedure");
      }

      if(nSWPBootTimeout == 0)
      {
         pNFCController->nState = NFCC_BOOT_STATE_FINAL | NFCC_STATUS_PENDING_EXECUTED;

         /* End of the boot procedure */
         static_PNFCControllerBootComplete( pContext, pNFCController->nTargetMode, W_SUCCESS );
         return;
      }

      /* Request the establishment of the SWP communication */
      PNALServiceExecuteCommand(
         pContext,
         NAL_SERVICE_UICC,
         &pNFCController->sBootServiceOperation,
         NAL_CMD_UICC_START_SWP,
         null, 0,
         null, 0,
         static_PNFCControllerBootExecuteCommandCompleted,
         pNFCController );

      PMultiTimerSet( pContext, TIMER_T13_NFCC_BOOT, TIMER_T13_TIMEOUT_BOOT,
         &static_PNFCControllerBootTimeoutElapsed, null );

      pNFCController->nState = NFCC_BOOT_STATE_WAIT_FOR_SWP_BOOT | NFCC_STATUS_PENDING_EXECUTED;
      return;


   case  NFCC_BOOT_STATE_SET_CURRENT_POLICY_DONE :

      /* check if the set of the current policy succeed */

      /* restore the persistent policy */
      PNALWriteCardProtocols(pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy, &pNFCController->aNALDataBuffer[0]);
      PNALWriteReaderProtocols(pNFCController->sPolicyMonitor.sPersistent.nUICCProtocolPolicy, &pNFCController->aNALDataBuffer[2]);

      {
         uint16_t nFlags = 0;
         nFlags |= pNFCController->sPolicyMonitor.sPersistent.bReaderRFLock  == false ? NAL_POLICY_FLAG_READER_LOCK : 0;
         nFlags |= pNFCController->sPolicyMonitor.sPersistent.bCardRFLock  == false ? NAL_POLICY_FLAG_CARD_LOCK : 0;

         switch (pNFCController->sPolicyMonitor.sPersistent.sSEPolicy.nSESwitchPosition)
         {
            case P_SE_SWITCH_OFF    : nFlags |= NAL_POLICY_FLAG_SE_OFF;  break;
            case P_SE_SWITCH_RF_INTERFACE   : nFlags |= NAL_POLICY_FLAG_RF_INTERFACE; break;
            case P_SE_SWITCH_FORCED_HOST_INTERFACE : nFlags |= NAL_POLICY_FLAG_FORCED_HOST_INTERFACE; break;
            case P_SE_SWITCH_HOST_INTERFACE : nFlags |= NAL_POLICY_FLAG_HOST_INTERFACE; break;
         }

         nFlags |= pNFCController->sPolicyMonitor.sPersistent.sSEPolicy.nSlotIdentifier << 4;

         PNALWriteUint16ToBuffer(nFlags, &pNFCController->aNALDataBuffer[4]);
      }

      PNALWriteCardProtocols(pNFCController->sPolicyMonitor.sPersistent.sSEPolicy.nSEProtocolPolicy, &pNFCController->aNALDataBuffer[6]);

      static_PNFCControllerBootSetParameter(pContext,pNFCController, NAL_SERVICE_ADMIN, NAL_PAR_PERSISTENT_POLICY, pNFCController->aNALDataBuffer, NAL_POLICY_SIZE);
      pNFCController->nState = NFCC_BOOT_STATE_SET_PERSISTENT_POLICY_DONE | NFCC_STATUS_PENDING_EXECUTED;

      break;

   case NFCC_BOOT_STATE_SET_PERSISTENT_POLICY_DONE :

      /* check if the set persistent policy succeeded */

      /* ok, go back to NFCC_BOOT_STATE_START_SWP_3 state */
      pNFCController->nState = NFCC_BOOT_STATE_START_SWP_3 | NFCC_STATUS_PENDING_EXECUTED;

      static_PNFCControllerBootMachine( pContext, NFCC_EVENT_COMMAND_EXECUTED, null, 0, W_SUCCESS );
      return;

   case  NFCC_BOOT_STATE_WAIT_FOR_SWP_BOOT:

      /* Check the result */
      if ( nError != W_SUCCESS )
      {
         PDebugError("PNFCControllerBoot: NFC HAL start SWP returns %s", PUtilTraceError(nError));

         /* Boot stopped */
         static_PNFCControllerBootComplete( pContext,
               W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED, W_SUCCESS );
         return;
      }

      CDebugAssert(nSWPBootTimeout != 0);
      PMultiTimerSet( pContext, TIMER_T13_NFCC_BOOT, nSWPBootTimeout,
         &static_PNFCControllerBootTimeoutSWP, null );

      pNFCController->nState = NFCC_BOOT_STATE_FINAL | NFCC_STATUS_PENDING_EXECUTED;
      return;

   case NFCC_BOOT_STATE_FINAL:

      CDebugAssert(pNFCController->nState == NFCC_BOOT_STATE_FINAL);

      /* End of the boot procedure */
      static_PNFCControllerBootComplete( pContext, pNFCController->nTargetMode, W_SUCCESS );
      return;

   default:

      PDebugError("PNFCControllerBoot: Unknown machine state !");

      static_PNFCControllerBootComplete( pContext,
         W_NFCC_MODE_NOT_RESPONDING, W_ERROR_BAD_STATE );

      return;
   }
}


/**
 * NFC Controller Boot Reset
 */

/* See Client API Specifications */
void PNFCControllerResetDriver(
         tContext* pContext,
         tPBasicGenericCallbackFunction* pCallback,
         void* pCallbackParameter,
         uint32_t nMode )
{
   W_ERROR nError = W_SUCCESS;
   tNFCController* pNFCController = PContextGetNFCController( pContext );

   if( PNALServiceGetVariable(pContext, NAL_PARAM_MODE) == W_NFCC_MODE_BOOT_PENDING )
   {
      nError = W_ERROR_BAD_NFCC_MODE;
   }
   else if ( (nMode!=W_NFCC_MODE_ACTIVE) && (nMode!=W_NFCC_MODE_MAINTENANCE) )
   {
      nError = W_ERROR_BAD_PARAMETER;
   }

   if ( nError == W_SUCCESS )
   {
      PNFCControllerNotifyException(pContext);

      CDebugAssert(pNFCController->bInitialReset == false);

      PDFCDriverFillCallbackContext( pContext,
            (tDFCCallback*)pCallback, pCallbackParameter,
            &pNFCController->pBootDriverCC );

      pNFCController->nUpdateError = W_SUCCESS;

      static_PNFCControllerPerformInitialReset(pContext, nMode);
    }
   else
   {
      tDFCDriverCCReference  pDriverCC;

      PDFCDriverFillCallbackContext( pContext,
            (tDFCCallback*)pCallback, pCallbackParameter,
            &pDriverCC );

      PDFCDriverPostCC2( pDriverCC, P_DFC_TYPE_NFCC, nError );
   }
}

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */


/* -----------------------------------------------------------------------------

   Firmware Update State Machine

----------------------------------------------------------------------------- */

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

static void static_PNFCControllerUpdateMachine(
         tContext* pContext,
         uint32_t nEvent,
         uint32_t nLength,
         W_ERROR nError );

static P_INLINE void static_PNFCControllerUpdateMachineDefault(
         tContext* pContext,
         uint32_t nEvent)
{
   static_PNFCControllerUpdateMachine(pContext, nEvent, 0, W_SUCCESS);
}

/**
 * Command   E X E C U T E D   Callback
 */
static void static_PNFCControllerUpdateExecuteCommandCompleted(
         tContext* pContext,
         void* pCallbackParameter,
         uint32_t nLength,
         W_ERROR nError,
         uint32_t nReceptionCounter)
{
   static_PNFCControllerUpdateMachine(
      pContext, NFCC_EVENT_COMMAND_EXECUTED, nLength, nError);
}

/**
 * Firmware Update Machine State
 */
static void static_PNFCControllerUpdateMachine(
         tContext* pContext,
         uint32_t nEvent,
         uint32_t nLength,
         W_ERROR nError )
{
   tNFCController* pNFCController = PContextGetNFCController( pContext );
   tNFCControllerInfo* pNFCControllerInfo;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PNFCControllerUpdateMachine: Receiving error %s",
         PUtilTraceError(nError));
   }

   switch ( nEvent )
   {
   case NFCC_EVENT_INITIAL:

      pNFCController->nUpdateBufferIndex = 0;
      pNFCController->nState = NFCC_UPDATE_STATE_LOAD_CHUNK;
      CDebugAssert(nError == W_SUCCESS);
      break;

   case NFCC_EVENT_COMMAND_EXECUTED:
      if (nError == W_ERROR_BAD_PARAMETER)
      {
         /* Update stopped */
         PDebugError("static_PNFCControllerUpdateMachine: Error of firmware format.");
         nError = W_ERROR_BAD_FIRMWARE_FORMAT;
      }
      else if ( ((pNFCController->nState & NFCC_STATUS_PENDING_CANCEL) != 0x00 )
      || (nError == W_ERROR_TIMEOUT))
      {
         /* Update stopped */
         PDebugError("static_PNFCControllerUpdateMachine: NFCC not repsonding.");
         nError = W_ERROR_TIMEOUT;
      }
      pNFCController->nState &= ~NFCC_STATUS_PENDING_EXECUTED;
      break;

   case NFCC_EVENT_TIMEOUT_ELAPSED:

      if ( (pNFCController->nState & NFCC_STATUS_PENDING_EXECUTED) != 0x00 )
      {
         pNFCController->nState &= ~NFCC_STATUS_PENDING_EXECUTED;
         pNFCController->nState |= NFCC_STATUS_PENDING_CANCEL;

         PNALServiceCancelOperation(pContext, &pNFCController->sBootServiceOperation);
         return;
      }

      /* Update stopped */
      PDebugError("static_PNFCControllerUpdateMachine: NFCC not repsonding.");
      nError = W_ERROR_TIMEOUT;
      break;
   }

   if ((nError == W_SUCCESS) && ( pNFCController->nState == NFCC_UPDATE_STATE_LOAD_CHUNK ))
   {
      pNFCController->nState = NFCC_UPDATE_STATE_LOAD_CHUNK | NFCC_STATUS_PENDING_EXECUTED;

      /* Content of the NAL_CMD_UPDATE_FIRMWARE :
            [0-3] Pointer to the buffer that contains the whole configuration buffer
            [4-7] Length of the buffer
      */

      PUtilWriteUint32ToBigEndianBuffer((uint32_t) pNFCController->pUpdateBuffer, pNFCController->aNALDataBuffer);
      PUtilWriteUint32ToBigEndianBuffer(pNFCController->nUpdateBufferLength, pNFCController->aNALDataBuffer + 4);

      PNALServiceExecuteCommand(
         pContext,
         NAL_SERVICE_ADMIN,
         &pNFCController->sBootServiceOperation,
         NAL_CMD_UPDATE_FIRMWARE,
         pNFCController->aNALDataBuffer, 8,
         null, 0,
         static_PNFCControllerUpdateExecuteCommandCompleted, pNFCController );

      pNFCController->nState = NFCC_UPDATE_STATE_FINAL | NFCC_STATUS_PENDING_EXECUTED;

      return;
   }

   PDebugError("static_PNFCControllerUpdateMachine: Firmware Update returns %s",
      PUtilTraceError(nError));

   pNFCController->nUpdateError = nError;

   pNFCController->pUpdateBuffer = null;
   pNFCController->nUpdateBufferLength = 0;
   pNFCController->nUpdateBufferIndex = 0;

   /* Reset the NFCC */
   pNFCControllerInfo = PContextGetNFCControllerInfo( pContext );

   CDebugAssert( (pNFCController->nTargetMode == W_NFCC_MODE_ACTIVE) || (pNFCController->nTargetMode == W_NFCC_MODE_MAINTENANCE) );
   pNFCController->nResetCount = 0;
   CMemoryFill(pNFCControllerInfo, 0, sizeof(tNFCControllerInfo));

   static_PNFCControllerPreReset( pContext );

   /* Call the boot state machine */
   static_PNFCControllerBootMachineDefault( pContext, NFCC_EVENT_INITIAL );
}

/* See Client API Specifications */
void PNFCControllerFirmwareUpdateDriver(
                  tContext* pContext,
                  tPBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  const uint8_t* pUpdateBuffer,
                  uint32_t nUpdateBufferLength,
                  uint32_t nMode )
{
   tNFCController * pNFCController = PContextGetNFCController( pContext );
   W_ERROR          nError = W_SUCCESS;
   uint32_t nInitalMode = PNALServiceGetVariable(pContext, NAL_PARAM_MODE);

   if ( (nInitalMode != W_NFCC_MODE_NO_FIRMWARE) &&
        (nInitalMode != W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED) &&
        (nInitalMode != W_NFCC_MODE_MAINTENANCE) )
   {
      nError = W_ERROR_BAD_NFCC_MODE;
   }
   else if ( (nMode != W_NFCC_MODE_ACTIVE) && (nMode != W_NFCC_MODE_MAINTENANCE) )
   {
      PDebugError("PNFCControllerFirmwareUpdate: Wrong nMode value");
      nError = W_ERROR_BAD_PARAMETER;
   }
   else if ( (pUpdateBuffer == null) || (nUpdateBufferLength <= NAL_FIRMWARE_HEADER_SIZE))
   {
      PDebugError("PNFCControllerFirmwareUpdate: pUpdateBuffer is null or too short");
      nError = W_ERROR_BAD_PARAMETER;
   }
   /* Check the Magic Number */
   else if (PUtilReadUint32FromLittleEndianBuffer(pUpdateBuffer) != NAL_FIRMWARE_FORMAT_MAGIC_NUMBER)
   {
      PDebugError("PNFCControllerFirmwareUpdate: Wrong format for the firmware");
      nError = W_ERROR_BAD_FIRMWARE_SIGNATURE;
   }
   else if ((pNFCController->pUpdateBuffer != null) ||(pNFCController->nUpdateBufferLength != 0))
   {
      PDebugError("PNFCControllerFirmwareUpdate: An update is already pending");
      nError = W_ERROR_BAD_STATE;
   }

   if ( nError == W_SUCCESS )
   {
      PDFCDriverFillCallbackContext(
            pContext,
            (tDFCCallback*)pCallback,
            pCallbackParameter,
            &pNFCController->pBootDriverCC );

      pNFCController->nUpdateError = W_SUCCESS;
      PNALServiceSetVariable(pContext, NAL_PARAM_MODE, W_NFCC_MODE_BOOT_PENDING);
      pNFCController->nTargetMode = nMode;

      pNFCController->pUpdateBuffer       = (uint8_t*)pUpdateBuffer;
      pNFCController->nUpdateBufferLength = nUpdateBufferLength;

      static_PNFCControllerUpdateMachineDefault(pContext, NFCC_EVENT_INITIAL);
   }
   else
   {
      tDFCDriverCCReference  pDriverCC;

      PDebugError("PNFCControllerFirmwareUpdate: Error %s", PUtilTraceError(nError));

      PDFCDriverFillCallbackContext( pContext,
            (tDFCCallback*)pCallback, pCallbackParameter,
            &pDriverCC );

      PDFCDriverPostCC2(pDriverCC, P_DFC_TYPE_NFCC, nError);
   }
}

/* See Client API Specifications */
uint32_t PNFCControllerFirmwareUpdateState(
         tContext* pContext )
{
   return PNALServiceGetVariable(pContext, NAL_PARAM_FIRMWARE_UPDATE);
}

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */

