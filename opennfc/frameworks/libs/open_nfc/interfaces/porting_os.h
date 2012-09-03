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

  This header file contains the functions to implement to port Open NFC
  on the device OS. For the porting, refer the documentation on the
  function detailed description and the porting process.

*******************************************************************************/

#ifndef __PORTING_OS_H
#define __PORTING_OS_H

#ifndef  P_CONFIG_DRIVER
/* Define the build configuration for the monolithic porting */
#  define P_CONFIG_MONOLITHIC  1
/* Define the build configuration for the driver porting */
#  define P_CONFIG_DRIVER  2
/* Define the build configuration for the user porting */
#  define P_CONFIG_USER    3
/* Define the build configuration for the client porting */
#  define P_CONFIG_CLIENT    4
/* Define the build configuration for the server porting */
#  define P_CONFIG_SERVER    5
#endif /* #ifndef  P_CONFIG_DRIVER */

#include "porting_config.h"
#include "porting_types.h"
#include "porting_inline.h"

/*******************************************************************************
  Memory Functions
*******************************************************************************/

#ifndef CMemoryAlloc
void* CMemoryAlloc( uint32_t nSize );
#endif /* CMemoryAlloc */

#ifndef CMemoryFree
void CMemoryFree( void* pBuffer );
#endif /* CMemoryFree */

#ifndef CMemoryGetStatistics
void CMemoryGetStatistics(
               uint32_t* pnCurrentAllocation,
               uint32_t* pnPeakAllocation );
#endif /* CMemoryGetStatistics */

#ifndef CMemoryResetStatistics
void CMemoryResetStatistics( void );
#endif /* CMemoryResetStatistics */

#ifndef CMemoryCopy
void* CMemoryCopy( void* pDestination, const void* pSource, uint32_t nLength );
#endif /* CMemoryCopy */

#ifndef CMemoryMove
void* CMemoryMove( void* pDestination, const void* pSource, uint32_t nLength );
#endif /* CMemoryMove */

#ifndef CMemoryFill
void CMemoryFill( void* pBuffer, uint8_t nValue, uint32_t nLength );
#endif /* CMemoryFill */

#ifndef CMemoryCompare
sint32_t CMemoryCompare( const void* pBuffer1, const void* pBuffer2, uint32_t nLength );
#endif /* CMemoryCompare */

/*******************************************************************************
  Debug Functions
*******************************************************************************/

#ifndef CDebugAssert
#define CDebugAssert( cond ) \
   if(!(cond)) CDebugAssertFaild(#cond)
#endif /* CDebugAssert */

static P_INLINE void PDebugTraceNone(const char* pMessage, ...) {}

#ifndef P_TRACE_ACTIVE
#define PDebugTrace while(0) PDebugTraceNone
#define PDebugLog while(0) PDebugTraceNone
#define PDebugWarning while(0) PDebugTraceNone
#define PDebugError while(0) PDebugTraceNone
#define PDebugTraceBuffer(pBuffer, nLength) while(0) {}
#define PDebugLogBuffer(pBuffer, nLength) while(0) {}
#define PDebugWarningBuffer(pBuffer, nLength) while(0) {}
#define PDebugErrorBuffer(pBuffer, nLength) while(0) {}
#else /* P_TRACE_ACTIVE */

/* The trace levels */
#define P_TRACE_TRACE      1
#define P_TRACE_LOG        2
#define P_TRACE_WARNING    3
#define P_TRACE_ERROR      4
#define P_TRACE_NONE       5

/* See Functional Specifications Document */
void CDebugPrintTrace(
            const char* pTag,
            uint32_t nTraceLevel,
            const char* pMessage,
            va_list list);

/* See Functional Specifications Document */
void CDebugPrintTraceBuffer(
            const char* pTag,
            uint32_t nTraceLevel,
            const uint8_t* pBuffer,
            uint32_t nLength);

#ifdef P_MODULE
#  define P_MODULE_DEC(X) P_TRACE_LEVEL_##X
#  if P_MODULE == P_TRACE_TRACE
#     define P_TRACE_LEVEL P_TRACE_TRACE
#  elif P_MODULE == P_TRACE_LOG
#     define P_TRACE_LEVEL P_TRACE_LOG
#  elif P_MODULE == P_TRACE_WARNING
#     define P_TRACE_LEVEL P_TRACE_WARNING
#  elif P_MODULE == P_TRACE_ERROR
#     define P_TRACE_LEVEL P_TRACE_ERROR
#  elif P_MODULE == P_TRACE_NONE
#     define P_TRACE_LEVEL P_TRACE_NONE
#  else
#     define P_TRACE_LEVEL P_TRACE_LEVEL_DEFAULT
#  endif
#  undef P_MODULE_DEC
#  define P_MODULE_DEC(X) #X
#  define P_TRACE_TAG P_MODULE
#else /* P_MODULE */

#  define P_TRACE_TAG  "?????"
#  define P_TRACE_LEVEL P_TRACE_LEVEL_DEFAULT
#endif /* P_MODULE */

#if P_TRACE_LEVEL <= P_TRACE_TRACE

#ifdef __GNUC__
static void PDebugTrace( const char* pMessage, ... ) __attribute__ ((format (printf, 1, 2))) __attribute__ ((unused)) ;
#endif

static void PDebugTrace( const char* pMessage, ... )
{
   va_list list;
   va_start( list, pMessage );
   CDebugPrintTrace(P_TRACE_TAG, P_TRACE_TRACE, pMessage, list );
   va_end( list );
}
static P_INLINE void PDebugTraceBuffer(const uint8_t* pBuffer, uint32_t nLength)
{
   CDebugPrintTraceBuffer(P_TRACE_TAG, P_TRACE_TRACE, pBuffer, nLength );
}
#else
#define PDebugTrace while(0) PDebugTraceNone
#define PDebugTraceBuffer(pBuffer, nLength) while(0) {}
#endif

#if P_TRACE_LEVEL <= P_TRACE_LOG

#ifdef __GNUC__
static void PDebugLog( const char* pMessage, ... ) __attribute__ ((format (printf, 1, 2))) __attribute__ ((unused)) ;
#endif

static void PDebugLog( const char* pMessage, ... )
{
   va_list list;
   va_start( list, pMessage );
   CDebugPrintTrace(P_TRACE_TAG, P_TRACE_LOG, pMessage, list );
   va_end( list );
}
static P_INLINE void PDebugLogBuffer(const uint8_t* pBuffer, uint32_t nLength)
{
   CDebugPrintTraceBuffer(P_TRACE_TAG, P_TRACE_LOG, pBuffer, nLength );
}
#else
#define PDebugLog while(0) PDebugTraceNone
#define PDebugLogBuffer(pBuffer, nLength) while(0) {}
#endif

#if P_TRACE_LEVEL <= P_TRACE_WARNING

#ifdef __GNUC__
static void PDebugWarning( const char* pMessage, ... ) __attribute__ ((format (printf, 1, 2))) __attribute__ ((unused)) ;
#endif

static void PDebugWarning( const char* pMessage, ... )
{
   va_list list;
   va_start( list, pMessage );
   CDebugPrintTrace(P_TRACE_TAG, P_TRACE_WARNING, pMessage, list );
   va_end( list );
}
static P_INLINE void PDebugWarningBuffer(const uint8_t* pBuffer, uint32_t nLength)
{
   CDebugPrintTraceBuffer(P_TRACE_TAG, P_TRACE_WARNING, pBuffer, nLength );
}
#else
#define PDebugWarning while(0) PDebugTraceNone
#define PDebugWarningBuffer(pBuffer, nLength) while(0) {}
#endif

#if P_TRACE_LEVEL <= P_TRACE_ERROR

#ifdef __GNUC__
static void PDebugError( const char* pMessage, ... ) __attribute__ ((format (printf, 1, 2))) __attribute__ ((unused)) ;
#endif

static void PDebugError( const char* pMessage, ... )
{
   va_list list;
   va_start( list, pMessage );
   CDebugPrintTrace(P_TRACE_TAG, P_TRACE_ERROR, pMessage, list );
   va_end( list );
}
static P_INLINE void PDebugErrorBuffer(const uint8_t* pBuffer, uint32_t nLength)
{
   CDebugPrintTraceBuffer(P_TRACE_TAG, P_TRACE_ERROR, pBuffer, nLength );
}
#else
#define PDebugError while(0) PDebugTraceNone
#define PDebugErrorBuffer(pBuffer, nLength) while(0) {}
#endif

#undef P_MODULE_DEC
#undef P_TRACE_TAG

#endif /* P_TRACE_ACTIVE */

/*******************************************************************************
  Critical Section Functions
*******************************************************************************/

#ifndef CSyncCreateCriticalSection
void CSyncCreateCriticalSection(
            P_SYNC_CS* phCriticalSection);
#endif /* #ifndef CSyncCreateCriticalSection */

#ifndef CSyncEnterCriticalSection
void CSyncEnterCriticalSection(
            P_SYNC_CS* phCriticalSection );
#endif /* #ifndef CSyncEnterCriticalSection */

#ifndef CSyncLeaveCriticalSection
void CSyncLeaveCriticalSection(
            P_SYNC_CS* phCriticalSection );
#endif /* #ifndef CSyncLeaveCriticalSection */

#ifndef CSyncDestroyCriticalSection
void CSyncDestroyCriticalSection(
            P_SYNC_CS* phCriticalSection );
#endif /* #ifndef CSyncDestroyCriticalSection */

/*******************************************************************************
  Wait Object Functions
*******************************************************************************/

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#ifndef CSyncCreateWaitObject
bool CSyncCreateWaitObject(
            P_SYNC_WAIT_OBJECT* phWaitObject);
#endif /* #ifndef CSyncCreateWaitObject */

#ifndef CSyncWaitForObject
void CSyncWaitForObject(
            P_SYNC_WAIT_OBJECT* phWaitObject );
#endif /* #ifndef CSyncWaitForObject */

#ifndef CSyncSignalWaitObject
void CSyncSignalWaitObject(
            P_SYNC_WAIT_OBJECT* phWaitObject );
#endif /* #ifndef CSyncSignalWaitObject */

#ifndef CSyncDestroyWaitObject
void CSyncDestroyWaitObject(
            P_SYNC_WAIT_OBJECT* phWaitObject );
#endif /* #ifndef CSyncDestroyWaitObject */

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

/*******************************************************************************
  Semaphore Functions
*******************************************************************************/

#if P_BUILD_CONFIG == P_CONFIG_DRIVER

#ifndef CSyncCreateSemaphore
void CSyncCreateSemaphore(
               P_SYNC_SEMAPHORE* phSemaphore );
#endif /* #ifndef CSyncCreateSemaphore */

#ifndef CSyncDestroySemaphore
void CSyncDestroySemaphore(
               P_SYNC_SEMAPHORE* phSemaphore );
#endif /* #ifndef CSyncDestroySemaphore */

#ifndef CSyncIncrementSemaphore
void CSyncIncrementSemaphore(
               P_SYNC_SEMAPHORE* phSemaphore );
#endif /* #ifndef CSyncIncrementSemaphore */

#ifndef CSyncWaitSemaphore
bool CSyncWaitSemaphore(
               P_SYNC_SEMAPHORE* phSemaphore );
#endif /* #ifndef CSyncWaitSemaphore */

#endif /* P_CONFIG_DRIVER */

/*******************************************************************************
  Memory Mapping Functions
*******************************************************************************/

#if P_BUILD_CONFIG == P_CONFIG_DRIVER

void* CSyncMapUserBuffer(
               P_SYNC_BUFFER* phBuffer,
               void* pUserBuffer,
               uint32_t nLength,
               uint32_t nType);

void CSyncCopyToUserBuffer(
               P_SYNC_BUFFER* phBuffer,
               void* pUserBuffer,
               void* pKernelBuffer,
               uint32_t nBufferLength);

void CSyncUnmapUserBuffer(
               P_SYNC_BUFFER* phBuffer,
               void* pUserBuffer,
               void* pKernelBuffer,
               uint32_t nBufferLength);

#endif /* P_CONFIG_DRIVER */

/*******************************************************************************
  Trigger Event Pump Function
*******************************************************************************/

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

void CSyncTriggerEventPump(
            void* pPortingConfig);

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */

/*******************************************************************************
  Security Functions
*******************************************************************************/

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

typedef struct __tUserAuthenticationData tUserAuthenticationData;
typedef struct __tUserIdentity tUserIdentity;

bool CSecurityCreateAuthenticationData(
            const tUserIdentity* pUserIdentity,
            const uint8_t* pApplicationDataBuffer,
            uint32_t nApplicationDataBufferLength,
            const tUserAuthenticationData** ppAuthenticationData);

void CSecurityDestroyAuthenticationData(
            const tUserAuthenticationData* pAuthenticationData);

bool CSecurityCheckIdentity(
            const tUserIdentity* pUserIdentity,
            const tUserAuthenticationData* pAuthenticationData,
            const uint8_t* pPrincipalBuffer,
            uint32_t nPrincipalBufferLength);

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */

#endif /* __PORTING_OS_H */
