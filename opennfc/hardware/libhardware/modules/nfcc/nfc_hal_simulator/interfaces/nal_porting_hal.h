/*
 * Copyright (c) 2009-2010 Inside Secure, All Rights Reserved.
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

  This header file contains the functions to implement when porting Open NFC
  on the device. For the porting, refer the porting reference manual.

*******************************************************************************/

#ifndef __NAL_PORTING_HAL_H
#define __NAL_PORTING_HAL_H

#include "nal_porting_config.h"
#include "nal_porting_types.h"
#include "nal_porting_inline.h"

/*******************************************************************************
  Communication Constants
*******************************************************************************/

#define P_COM_TYPE_NFCC_SHDLC_RXTX         0
#define P_COM_TYPE_NFCC_SHDLC_I2C          1
#define P_COM_TYPE_NFCC_SHDLC_SPI          2
#define P_COM_TYPE_NFCC_SHDLC_TCPIP        3
#define P_COM_TYPE_NFC_HAL_SHDLC_RXTX      4
#define P_COM_TYPE_NFC_HAL_SHDLC_I2C       5
#define P_COM_TYPE_NFC_HAL_SHDLC_SPI       6
#define P_COM_TYPE_NFC_HAL_SHDLC_TCPIP     7
#define P_COM_TYPE_NFC_HAL_TCPIP           8
#define P_COM_TYPE_NFC_HAL_SHDLC_DIRECT    9

/*******************************************************************************
  Reset Constants
*******************************************************************************/

#define P_RESET_BOOT       0
#define P_RESET_WAKEUP     1

/*******************************************************************************
  Creation functions
*******************************************************************************/

struct __tNALInstance;

typedef struct __tNALInstance tNALInstance;

tNALInstance * CNALPreCreate(
         void * pPortingConfig);

bool CNALPostCreate(
         tNALInstance * pNALInstance,
         void * pNALVoidContext);

void CNALPreDestroy(
         tNALInstance * pNALInstance);

void CNALPostDestroy(
         tNALInstance * pNALInstance);

/*******************************************************************************
  Timer Functions
*******************************************************************************/
struct __tNALTimerInstance;

typedef struct __tNALTimerInstance tNALTimerInstance;

tNALTimerInstance* CNALTimerCreate(void* pPortingConfig);

void CNALTimerDestroy(
         tNALTimerInstance* pTimer );

uint32_t CNALTimerGetCurrentTime(
         tNALTimerInstance* pTimer );

void CNALTimerSet(
         tNALTimerInstance* pTimer,
         uint32_t nAbsoluteTime );

bool CNALTimerIsTimerElapsed(
         tNALTimerInstance* pTimer );

void CNALTimerCancel(
         tNALTimerInstance* pTimer );

/*******************************************************************************
  Communication Port Functions
*******************************************************************************/
struct __tNALComInstance;

typedef struct __tNALComInstance tNALComInstance;

tNALComInstance* CNALComCreate(
         void* pPortingConfig,
         uint32_t* pnType );

void CNALComDestroy(
         tNALComInstance* pComPort );


uint32_t CNALComReadBytes(
         tNALComInstance* pComPort,
         uint8_t* pReadBuffer,
         uint32_t nBufferLength);


uint32_t CNALComWriteBytes(
         tNALComInstance* pComPort,
         uint8_t* pBuffer,
         uint32_t nBufferLength );


/*******************************************************************************
  Reset Functions
*******************************************************************************/

void CNALResetNFCController(
            void* pPortingConfig,
            uint32_t nResetType );

bool CNALResetIsPending(
            void* pPortingConfig );

/*******************************************************************************
  Synchronization functions
*******************************************************************************/

void CNALSyncTriggerEventPump(
            void* pPortingConfig );

#endif /* __NAL_PORTING_HAL_H */
