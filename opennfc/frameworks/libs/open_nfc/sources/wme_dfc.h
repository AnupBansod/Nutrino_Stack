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

#ifndef __WME_DFC_H
#define __WME_DFC_H

/*******************************************************************************
   Contains the declaration of the internal DFC function
*******************************************************************************/

/* The DFC queue instance - Internal structure - Do not use directly */
struct __tDFCElement;
typedef struct __tDFCQueue
{
   uint32_t nFirstDFC;
   uint32_t nDFCNumber;
   uint32_t nDFCQueueSize;

   struct __tDFCElement* pDFCElementList;

#if (((P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)) && defined(P_INCLUDE_JAVA_API))

   uint32_t nNextToCall;
   uint32_t nNextToAdd;
   uint32_t nElementNumber;
   struct __tDFCElement* pExternalList;

#endif /* (P_CONFIG_USER || P_CONFIG_MONOLITHIC) && P_INCLUDE_JAVA_API */

} tDFCQueue;

typedef void tDFCCallback( void );

typedef struct __tDFCExternal
{
   tDFCCallback* pFunction;
   void* pParameter;
} tDFCExternal;

/* The DFC callback context - Internal structure - Do not use directly */
typedef struct __tDFCCallbackContext
{
   tDFCCallback* pFunction;
   void* pParameter;
   tContext* pContext;
   bool bIsExternal;

} tDFCCallbackContext;

/**
 * Fills a callback context.
 *
 * @param[in]   pContext  The context pointer received in parameter.
 *
 * @param[in]   pCallbackFunction  The callback function.
 *
 * @param[in]   pCallbackParameter  The callback parameter.
 *
 * @param[out]  pCallbackContext  The callback context to fill.
 **/
void PDFCFillCallbackContext(
         tContext* pContext,
         tDFCCallback* pCallbackFunction,
         void* pCallbackParameter,
         tDFCCallbackContext* pCallbackContext );

/**
 * Flushes the specified call.
 *
 * @param[in]   pCallbackContext  The callback context.
 **/
void PDFCFlushCall(
            tDFCCallbackContext* pCallbackContext);

/**
 * Posts a differed function call in the queue.
 *
 * INTERNAL FUNCTION - DO NOT CALL DIRECTLY.
 *
 * Use the function PDFCPostContext<X>().
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  nFlags  The flags. Contains the number of parameters (1, 2, 3)
 *             and the type of the call.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 **/
void PDFCPostInternalContext3(
            tDFCCallbackContext* pCallbackContext,
            void* pType,
            uint32_t nFlags,
            void* pParam2,
            uint32_t nParam3);

/**
 * Posts a differed function call in the queue.
 *
 * INTERNAL FUNCTION - DO NOT CALL DIRECTLY.
 *
 * Use the function PDFCPostContext<X>().
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  nFlags  The flags. Contains the number of parameters (4, 5, 6).
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 *
 * @param[in]  nParam5  The fifth parameter.
 *
 * @param[in]  nParam6  The sixth parameter.
 **/
void PDFCPostInternalContext6(
            tDFCCallbackContext* pCallbackContext,
            void* pType,
            uint32_t nFlags,
            void* pParam2,
            uint32_t nParam3,
            uint32_t nParam4,
            uint32_t nParam5,
            uint32_t nParam6);

#if (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* In monolithic config, the driver callback context is equal to the user callback context */
#define tDFCDriverCC tDFCCallbackContext

#define PDFCDriverFlushCall( pDriverCC ) \
            PDFCFlushCall( &(pDriverCC) )

#define PDFCDriverFillCallbackContext   PDFCFillCallbackContext

#define tDFCDriverCCReference  tDFCCallbackContext

#define PDFCDriverPostCC1( pDriverCC, pType ) \
            PDFCPostInternalContext3( &(pDriverCC), (pType), 1, 0, 0 )

#define PDFCDriverPostCC2( pDriverCC, pType, pParam2 ) \
            PDFCPostInternalContext3( &(pDriverCC), (pType), 2, \
               (void*)(pParam2), 0 )

#define PDFCDriverPostCC3( pDriverCC, pType, pParam2, nParam3 ) \
            PDFCPostInternalContext3( &(pDriverCC), (pType), 3, \
               (void*)(pParam2), (uint32_t)(nParam3) )

#define PDFCDriverPostCC4( pDriverCC, pType, pParam2, nParam3, nParam4 ) \
            PDFCPostInternalContext6( &(pDriverCC), (pType), 4, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), 0, 0 )

#define PDFCDriverPostCC5( pDriverCC, pType, pParam2, nParam3, nParam4, nParam5 ) \
            PDFCPostInternalContext6( &(pDriverCC), (pType), 5, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), 0 )

#define PDFCDriverPostCC6( pDriverCC, pType, pParam2, nParam3, nParam4, nParam5, nParam6 ) \
            PDFCPostInternalContext6( &(pDriverCC), (pType), 6, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), (uint32_t)(nParam6) )

/**
 * Gets the address of the caller buffer.
 *
 * The address of the caller buffer should not be used to access the buffer data.
 *
 * @param[in]   pDriverCC  The driver callback context.
 *
 * @param[in]   pKernelBuffer  The local buffer address.
 *
 * @return   The address of the caller buffer.
 **/
#define PDFCDriverGetCallerBufferAddress( pDriverCC, pKernelBuffer ) \
               ((void*)pKernelBuffer)

/**
 * Sets/resets the context user instance.
 *
 * If pDriverCC is defined, the context user instance is set accodingly.
 * If pDriverCC is null, the context user instance is set to null.
 *
 * @param[in]   pContext  The context.
 *
 * @param[in]   pDriverCC  The driver callback context.
 **/
#define PDFCDriverSetCurrentUserInstance( pContext, pDriverCC )

#endif /* P_CONFIG_MONOLITHIC */

#if (P_BUILD_CONFIG == P_CONFIG_USER)

/**
 * Pumps the dfc calls comming from the driver.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  bWait  If set to true, the function waits until an event is
 *             received or WBasicStopEventLoop() is called. If set to false and
 *             there is no event in the event queue, the function returns
 *             immediately with the code W_ERROR_NO_EVENT.
 *
 * @return     One of the following error codes:
 *               - W_SUCCESS  An event was removed from the event queue and the
 *                 corresponding handler function is executed.
 *               - W_ERROR_NO_EVENT  The parameter bWait is set to false and
 *                 there is no event in the event queue.
 *               - W_ERROR_WAIT_CANCELLED  The function WBasicStopEventLoop()
 *                 was called.
 *               - others  If any other error occurred.
 **/
W_ERROR PDFCDriverPumpEvent(
            tContext * pContext,
            bool bWait );

#endif /* P_CONFIG_USER  */

#if (P_BUILD_CONFIG == P_CONFIG_USER)

#ifdef P_CONFIG_CLIENT_SERVER

/**
 * @brief  Calls a server function.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  nCode  The command code.
 *
 * @param[in-out]  pParamInOut  The input/output parameters, may be null.
 *
 * @param[in]  nSizeIn  The size in bytes of the input parameters.
 *
 * @param[in]  pBuffer1  The input buffer 1, may be null.
 *
 * @param[in]  nBuffer1Length  The length in bytes of the input buffer 1.
 *
 * @param[in]  pBuffer2  The input buffer 2, may be null.
 *
 * @param[in]  nBuffer2Length  The length in bytes of the input buffer 2.
 *
 * @param[in]  nSizeOut  The size in bytes of the output parameters.
 *
 * @return  The error returned by the communication functions.
 **/
W_ERROR PDFCClientCallFunction(
            tContext * pContext,
            uint8_t nCode,
            void* pParamInOut,
            uint32_t nSizeIn,
            const void* pBuffer1,
            uint32_t nBuffer1Length,
            const void* pBuffer2,
            uint32_t nBuffer2Length,
            uint32_t nSizeOut);

#endif /* #ifdef P_CONFIG_CLIENT_SERVER */

#endif /* P_CONFIG_USER  */

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER)

struct __tDFCDriverCallbackContext;

/* typedef struct __tDFCDriverCallbackContext* tDFCCallbackContext; */

typedef struct __tDFCDriverCallbackContext tDFCDriverCC;

typedef tDFCDriverCC* tDFCDriverCCReference;

#define PDFCDriverPostCC1( pDriverCC, pType ) \
            PDFCDriverInternalPostCC3( pContext, &(pDriverCC), (pType), 1, 0, 0 )

#define PDFCDriverPostCC2( pDriverCC, pType, pParam2 ) \
            PDFCDriverInternalPostCC3( pContext, &(pDriverCC), (pType), 2, \
               (void*)(pParam2), 0 )

#define PDFCDriverPostCC3( pDriverCC, pType, pParam2, nParam3 ) \
            PDFCDriverInternalPostCC3( pContext, &(pDriverCC), (pType), 3, \
               (void*)(pParam2), (uint32_t)(nParam3) )

#define PDFCDriverPostCC4( pDriverCC, pType, pParam2, nParam3, nParam4 ) \
            PDFCDriverInternalPostCC6( pContext, &(pDriverCC), (pType), 4, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), 0, 0 )

#define PDFCDriverPostCC5( pDriverCC, pType, pParam2, nParam3, nParam4, nParam5 ) \
            PDFCDriverInternalPostCC6( pContext, &(pDriverCC), (pType), 5, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), 0 )

#define PDFCDriverPostCC6( pDriverCC, pType, pParam2, nParam3, nParam4, nParam5, nParam6 ) \
            PDFCDriverInternalPostCC6( pContext, &(pDriverCC), (pType), 6, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), (uint32_t)(nParam6) )

#define PDFCDriverFlushCall(pDriverCC) \
               PDFCDriverInternalFlushCall(pContext, pDriverCC)

/**
 * Fluses the DFC of the current client.
 *
 * @param[in]  pContext   The context.
 **/
void PDFCDriverFlushClient(
         tContext* pContext );

/**
 * Pumps the dfc calls comming from the driver.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pParams  The parameter list.
 *
 * @return     One of the following error codes:
 *               - W_SUCCESS  An event was removed from the event queue and the
 *                 corresponding handler function is executed.
 *               - W_ERROR_NO_EVENT  The parameter bWait is set to false and
 *                 there is no event in the event queue.
 *               - W_ERROR_WAIT_CANCELLED  The function WBasicStopEventLoop()
 *                 was called.
 *               - others  If any other error occurred.
 **/
W_ERROR PDFCDriverPumpEvent(
            tContext * pContext,
            void* pParams );

/**
 * Allocates a callback context.
 *
 * @param[in]  pContext   The context.
 *
 * @return The callback context or null if there is not enough resources.
 **/
tDFCDriverCC* PDFCDriverAllocateCC(
         tContext* pContext );

/**
 * Allocates a callback context for an external callback function.
 *
 * @param[in]   pContext   The context.
 *
 * @param[in]   pCallbackFunction  The callback function.
 *
 * @param[in]   pCallbackParameter  The callback parameter.
 *
 * @return The callback context or null if there is not enough resources.
 **/
tDFCDriverCC* PDFCDriverAllocateCCExternal(
         tContext* pContext,
         tDFCCallback* pCallbackFunction,
         void* pCallbackParameter);

/**
 * Allocates a callback context for an internal callback function.
 *
 * @param[in]   pContext   The context.
 *
 * @param[in]   pCallbackFunction  The callback function.
 *
 * @param[in]   pCallbackParameter  The callback parameter.
 *
 * @return The callback context or null if there is not enough resources.
 **/
tDFCDriverCC* PDFCDriverAllocateCCFunction(
         tContext* pContext,
         tDFCCallback* pCallbackFunction,
         void* pCallbackParameter);

/**
 * Allocates a callback context for an external event handler function.
 *
 * @param[in]   pContext   The context.
 *
 * @param[in]   pCallbackFunction  The callback function.
 *
 * @param[in]   pCallbackParameter  The callback parameter.
 *
 * @return The callback context or null if there is not enough resources.
 **/
tDFCDriverCC* PDFCDriverAllocateCCExternalEvent(
         tContext* pContext,
         tDFCCallback* pCallbackFunction,
         void* pCallbackParameter);

/**
 * Allocates a callback context for an internal event handler function.
 *
 * @param[in]   pContext   The context.
 *
 * @param[in]   pCallbackFunction  The callback function.
 *
 * @param[in]   pCallbackParameter  The callback parameter.
 *
 * @return The callback context or null if there is not enough resources.
 **/
tDFCDriverCC* PDFCDriverAllocateCCFunctionEvent(
         tContext* pContext,
         tDFCCallback* pCallbackFunction,
         void* pCallbackParameter);

/**
 * Fills a driver callback context.
 *
 * @param[in]   pContext  The context pointer received in parameter.
 *
 * @param[in]   pCallbackFunction  The callback function.
 *
 * @param[in]   pCallbackParameter  The callback parameter.
 *
 * @param[out]  ppDriverCC  The driver callback context to fill.
 **/
#define PDFCDriverFillCallbackContext( pContext, pCallbackFunction, pCallbackParameter, ppDriverCC ) \
         { CDebugAssert(pCallbackFunction == null); \
           *(ppDriverCC) = (tDFCDriverCC*)(pCallbackParameter); }

/**
 * Gets the address of the caller buffer.
 *
 * The address of the caller buffer should not be used to access the buffer data.
 *
 * @param[in]   pCallbackContext  The callback context.
 *
 * @param[in]   pKernelBuffer  The local buffer address.
 *
 * @return   The address of the caller buffer.
 **/
void* PDFCDriverGetCallerBufferAddress(
         tDFCDriverCC* pDriverCC,
         void* pKernelBuffer );

/**
 * Sets/resets the context user instance.
 *
 * If pDriverCC is defined, the context user instance is set accodingly.
 * If pDriverCC is null, the context user instance is set to null.
 *
 * @param[in]   pContext  The context.
 *
 * @param[in]   pDriverCC  The driver callback context.
 **/
void PDFCDriverSetCurrentUserInstance(
         tContext* pContext,
         tDFCDriverCC* pDriverCC );

/**
 * Registers a user buffer in the driver callback context.
 *
 * @param[in]  pDriverCC  The driver callback context.
 *
 * @param[in]  pUserBuffer The pointer on the user buffer.
 *
 * @param[in]  nBufferSize  The buffer size in bytes.
 *
 * @param[in]  nType  The buffer type.
 *
 * @return  The kernel address of the buffer.
 *          The value is null if the user buffer is null or empty.
 *          The value is 1 if an error of mapping is detected.
 **/
void* PDFCDriverRegisterUserBuffer(
         tDFCDriverCC* pDriverCC,
         void* pUserBuffer,
         uint32_t nBufferSize,
         uint32_t nType );

/**
 * Registers the user address of a word in the driver callback context.
 *
 * @param[in]  pDriverCC  The driver callback context.
 *
 * @param[in]  pUserWord The pointer on the user word.
 *
 * @param[in]  nType  The buffer type.
 *
 * @return  The kernel address of the buffer.
 *          The value is null if the user buffer is null.
 *          The value is 1 if an error of mapping is detected.
 **/
uint32_t* PDFCDriverRegisterUserWordBuffer(
         tDFCDriverCC* pDriverCC,
         void* pUserWord,
         uint32_t nType );

/**
 * Synchronizes the content of the kernel buffer with the content of the user buffer.
 *
 * @param[in]  pDriverCC  The driver callback context.
 **/
void PDFCDriverSynchronizeUserBuffer(
         tDFCDriverCC* pDriverCC );

/**
 * Frees a driver callback context.
 *
 * @param[in]  pDriverCC  The driver callback context.
 **/
void PDFCDriverFreeCC(
         tDFCDriverCC* pDriverCC );

/**
 * Flushes the specified call.
 *
 * @param[in]   pDriverCC  The driver callback context.
 **/

void PDFCDriverInternalFlushCall(
            tContext * pContext,
            tDFCDriverCC* pDriverCC);

/**
 * Checks if the specified DFC is still present in the user instance
 *
 * @param[in]   pInstance  The user instance
 * @param[in]   pDriverCC  The driver callback context.
 *
 * @return true if the driver DFC still exists.
 **/

bool PDFCDriverCheckDriverDFCInUserInstance(
      tUserInstance * pInstance,
      tDFCDriverCC * pDriverCC);

/**
 * Posts a differed function call in the queue.
 *
 * INTERNAL FUNCTION - DO NOT CALL DIRECTLY.
 *
 * Use the function PDFCDriverPostCC<X>().
 *
 * @param[in]  pDriverCC   The driver callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  nFlags  The flags. Contains the number of parameters (1, 2, 3)
 *             and the type of the call.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 **/
void PDFCDriverInternalPostCC3(
            tContext * pContext,
            tDFCDriverCC** ppDriverCC,
            void* pType,
            uint32_t nFlags,
            void* pParam2,
            uint32_t nParam3);

/**
 * Posts a differed function call in the queue.
 *
 * INTERNAL FUNCTION - DO NOT CALL DIRECTLY.
 *
 * Use the function PDFCDriverPostCC<X>().
 *
 * @param[in]  pDriverCC   The driver callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  nFlags  The flags. Contains the number of parameters (4, 5, 6).
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 *
 * @param[in]  nParam5  The fifth parameter.
 *
 * @param[in]  nParam6  The sixth parameter.
 **/
void PDFCDriverInternalPostCC6(
            tContext * pContext,
            tDFCDriverCC ** ppDriverCC,
            void* pType,
            uint32_t nFlags,
            void* pParam2,
            uint32_t nParam3,
            uint32_t nParam4,
            uint32_t nParam5,
            uint32_t nParam6);

#endif /* P_CONFIG_DRIVER */

/* The call types */
#define P_DFC_TYPE_ISO_14443_3    ((void*)6)
#define P_DFC_TYPE_ISO_14443_4    ((void*)7)
#define P_DFC_TYPE_TEST_ENGINE    ((void*)8)
#define P_DFC_TYPE_SECURE_ELEMENT ((void*)9)
#define P_DFC_TYPE_NFCC           ((void*)10)
#define P_DFC_TYPE_NDEF           ((void*)11)
#define P_DFC_TYPE_ISO_7816_4     ((void*)12)
#define P_DFC_TYPE_PICO           ((void*)13)
#define P_DFC_TYPE_ISO_15693_DRIVER ((void*)14)
#define P_DFC_TYPE_ISO_15693_USER ((void*)15)
#define P_DFC_TYPE_SMART_CACHE    ((void*)16)
#define P_DFC_TYPE_READER         ((void*)17)
#define P_DFC_TYPE_MIFARE         ((void*)18)
#define P_DFC_TYPE_VIRTUAL_TAG    ((void*)19)
#define P_DFC_TYPE_ISO_EMUL       ((void*)20)
#define P_DFC_TYPE_BT_PAIR        ((void*)21)
#define P_DFC_TYPE_WIFI_PAIR      ((void*)22)
#define P_DFC_TYPE_TYPE5          ((void*)23)
#define P_DFC_TYPE_TYPE1_CHIP     ((void*)24)
#define P_DFC_TYPE_FELICA         ((void*)25)
#define P_DFC_TYPE_NFC_HAL_SERVICE ((void*)26)
#define P_DFC_TYPE_UICC           ((void*)27)
#define P_DFC_TYPE_P2P            ((void*)29)
#define P_DFC_TYPE_FORMAT1        ((void*)30)
#define P_DFC_TYPE_FORMAT2        ((void*)31)
#define P_DFC_TYPE_FORMAT5        ((void*)32)
#define P_DFC_TYPE_FORMAT6        ((void*)33)
#define P_DFC_TYPE_WJED           ((void*)34)
#define P_DFC_TYPE_TYPE3          ((void*)35)
#define P_DFC_TYPE_MY_D           ((void*)36)
#define P_DFC_TYPE_B_PRIME        ((void*)37)
#define P_DFC_TYPE_SECURITY       ((void*)38)
#define P_DFC_TYPE_HANDLE         ((void*)39)

/**
 * @brief Creates a DFC queue instance.
 *
 * @pre  Only one DFC queue instance is created at a given time.
 *
 * @param[out]  pDFCQueue  The DFC queue instance to initialize.
 *
 * @return  true if the DFC queue is initialized, false otherwise.
 **/
bool PDFCCreate(
         tDFCQueue* pDFCQueue );

/**
 * @brief Destroyes a DFC queue instance.
 *
 * If the instance is already destroyed, the function does nothing and returns.
 *
 * @post  Every pending DFC is cancelled.
 *
 * @post  PDFCDestroy() does not return any error. The caller should always
 *        assume that the DFC queue instance is destroyed after this call.
 *
 * @post  The caller should never re-use the DFC queue instance value.
 *
 * @param[in]  pDFCQueue  The DFC queue to destroy.
 **/
void PDFCDestroy(
         tDFCQueue* pDFCQueue );


/**
 * Posts a differed function call in the queue.
 *
 * INTERNAL FUNCTION - DO NOT CALL DIRECTLY.
 *
 * Use the function PDFCPost<X>().
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  void* pType  The type of call.
 *
 * @param[in]  nFlags  The flags. Contains the number of parameters (0, 1).
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 **/
void PDFCPostInternal(
            tContext* pContext,
            void* pType,
            uint32_t nFlags,
            tDFCCallback* pFunction,
            void* pParam1);

/**
 * Posts a differed function call in the queue.
 *
 * INTERNAL FUNCTION - DO NOT CALL DIRECTLY.
 *
 * Use the function PDFCPost<X>().
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  nFlags  The flags. Contains the number of parameters (2, 3 ,4, 5, 6).
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 *
 * @param[in]  nParam5  The fifth parameter.
 *
 * @param[in]  nParam6  The sixth parameter.
 **/
void PDFCPostInternal6(
            tContext* pContext,
            void* pType,
            uint32_t nFlags,
            tDFCCallback* pFunction,
            void* pParam1,
            void* pParam2,
            uint32_t nParam3,
            uint32_t nParam4,
            uint32_t nParam5,
            uint32_t nParam6);


/**
 * Posts a differed function call with no parameter.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pFunction  The callback function.
 **/
#define PDFCPost0( pContext, pType, pFunction ) \
            PDFCPostInternal( pContext, (pType), 0, (tDFCCallback*)(pFunction), \
               null )

/**
 * Posts a differed function call with 1 parameter.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 **/
#define PDFCPost1( pContext, pType, pFunction, pParam1 ) \
            PDFCPostInternal( pContext, (pType), 1, (tDFCCallback*)(pFunction), \
               (void*)(pParam1) )

/**
 * Posts a differed function call with 1 parameters.
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 **/
#define PDFCPostContext1( pCallbackContext, pType ) \
            PDFCPostInternalContext3( pCallbackContext, (pType), 1, 0, 0 )

/**
 * Posts a differed function call with 2 parameters.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 *
 * @param[in]  pParam2  The second parameter.
 **/
#define PDFCPost2( pContext, pType, pFunction, pParam1, pParam2 ) \
            PDFCPostInternal6( pContext, (pType), 2, (tDFCCallback*)(pFunction), \
               (void*)(pParam1), (void*)(pParam2), 0, 0, 0, 0 )

/**
 * Posts a differed function call with 2 parameters.
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pParam2  The second parameter.
 **/
#define PDFCPostContext2( pCallbackContext, pType, pParam2 ) \
            PDFCPostInternalContext3( pCallbackContext, (pType), 2, \
               (void*)(pParam2), 0 )

/**
 * Posts a differed function call with 3 parameters.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 **/
#define PDFCPost3( pContext, pType, pFunction, pParam1, pParam2, nParam3 ) \
            PDFCPostInternal6( pContext, (pType), 3, (tDFCCallback*)(pFunction), \
               (void*)(pParam1), (void*)(pParam2), (uint32_t)(nParam3), 0, 0, 0 )

/**
 * Posts a differed function call with 3 parameters.
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 **/
#define PDFCPostContext3( pCallbackContext, pType, pParam2, nParam3 ) \
            PDFCPostInternalContext3( pCallbackContext, (pType), 3, \
               (void*)(pParam2), (uint32_t)(nParam3) )

/**
 * Posts a differed function call with 4 parameters.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 **/
#define PDFCPost4( pContext, pType, pFunction, pParam1, pParam2, nParam3, nParam4 ) \
            PDFCPostInternal6( pContext, (pType), 4, (tDFCCallback*)(pFunction), \
               (void*)(pParam1), (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), 0, 0 )

/**
 * Posts a differed function call with 4 parameters.
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 **/
#define PDFCPostContext4( pCallbackContext, pType, pParam2, nParam3, nParam4 ) \
            PDFCPostInternalContext6( pCallbackContext, (pType), 4, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), 0, 0 )

/**
 * Posts a differed function call with 5 parameters.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 *
 * @param[in]  nParam5  The fifth parameter.
 **/
#define PDFCPost5( pContext, pType, pFunction, pParam1, pParam2, nParam3, nParam4, nParam5 ) \
            PDFCPostInternal6( pContext, (pType), 5, (tDFCCallback*)(pFunction), \
               (void*)(pParam1), (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), 0 )

/**
 * Posts a differed function call with 5 parameters.
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 *
 * @param[in]  nParam5  The fifth parameter.
 **/
#define PDFCPostContext5( pCallbackContext, pType, pParam2, nParam3, nParam4, nParam5 ) \
            PDFCPostInternalContext6( pCallbackContext, (pType), 5, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), 0 )

/**
 * Posts a differed function call with 6 parameters.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pFunction  The callback function.
 *
 * @param[in]  pParam1  The first parameter.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 *
 * @param[in]  nParam5  The fifth parameter.
 *
 * @param[in]  nParam6  The sixth parameter.
 **/
#define PDFCPost6( pContext, pType, pFunction, pParam1, pParam2, nParam3, nParam4, nParam5, nParam6 ) \
            PDFCPostInternal6( pContext, (pType), 6, (tDFCCallback*)(pFunction), \
               (void*)(pParam1), (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), (uint32_t)(nParam6) )

/**
 * Posts a differed function call with 6 parameters.
 *
 * @param[in]  pCallbackContext   The callback context.
 *
 * @param[in]  pType  The type of the call.
 *
 * @param[in]  pParam2  The second parameter.
 *
 * @param[in]  nParam3  The third parameter.
 *
 * @param[in]  nParam4  The fourth parameter.
 *
 * @param[in]  nParam5  The fifth parameter.
 *
 * @param[in]  nParam6  The sixth parameter.
 **/
#define PDFCPostContext6( pCallbackContext, pType, pParam2, nParam3, nParam4, nParam5, nParam6 ) \
            PDFCPostInternalContext6( pCallbackContext, (pType), 6, \
               (void*)(pParam2), (uint32_t)(nParam3), \
               (uint32_t)(nParam4), (uint32_t)(nParam5), (uint32_t)(nParam6) )

/**
 * Flushes the DFC of a given type.
 *
 * @param[in]  pContext   The context.
 *
 * @param[in]  pType  The type of the DFC to remove from the queue.
 **/
void PDFCFlush(
         tContext* pContext,
         void* pType );

/**
 * Executes the pending calls in the DFC queue.
 *
 * The function returns when the DFC queue is empty.
 *
 * @param[in]  pContext   The context.
 *
 * @return  true if at least a call is performed. false if no call is perfomed.
 **/
bool PDFCPump(
         tContext* pContext );

#endif /* __WME_DFC_H */
