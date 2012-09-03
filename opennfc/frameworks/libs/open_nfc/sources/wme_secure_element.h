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

#ifndef __WME_SECURE_ELEMENT_H
#define __WME_SECURE_ELEMENT_H

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/*******************************************************************************
   Contains the declaration of the secure element functions
*******************************************************************************/

/* SE switch position values */
#define P_SE_SWITCH_OFF                      0
#define P_SE_SWITCH_RF_INTERFACE             1
#define P_SE_SWITCH_FORCED_HOST_INTERFACE    2
#define P_SE_SWITCH_HOST_INTERFACE           3

/* The maximum length of the AID list in bytes */
#define P_SE_MAX_AID_LENGTH   280

/* Declare a secure element slot */
typedef struct __tSESlot
{
   tHandleObjectHeader sObjectHeader;

   /* Second object header for the security stack destroy hook */
   tHandleObjectHeader sObjectHeaderSecurityStackDestroyHook;

   /* The Secure Element information */
   tSEInfo sConstantInfo;

   /* The secure element protocol */
   uint32_t nConstantProtocols;

   /* The pending operation flags */
   uint32_t nPendingFlags;

   /* The WSEMonitorEndOfTransaction operation */
   tHandleObjectHeader sEndOfTransactionRegistry;
   tDFCDriverCCReference pEndOfTransactionHandlerDriverCC;

   /* The WSEMonitorHotPlugEvents operation */
   tHandleObjectHeader sHotPlugRegistry;
   tDFCDriverCCReference pHotPlugHandlerDriverCC;

   /* Internal communication operation */
   tDFCDriverCCReference pOpenConnectionDriverCC;
   tDFCCallbackContext sOpenConnectionCC;
   uint8_t aOpenConnectionDetectionBuffer[50]; /* Receive the detection frame for the buffer */
   W_HANDLE hOpenConnectionListenner;
   W_ERROR nOpenConnectionFirstError;
   bool bFirstDestroyCalled;
   bool bOpenConnectionForce;
   tDFCCallbackContext sDestroyCallback;
   W_HANDLE hDriverConnectionWeak;

   /* The Security Stack instance */
   tSecurityStackInstance* pSecurityStackInstance;

   W_HANDLE hInitializeHostConnection;
   W_ERROR nInitializeHostConnectionError;

   /* The WSEGetTransactionAID operation */
   uint32_t nLastAIDLength;
   uint8_t aLastAID[P_SE_MAX_AID_LENGTH];

} tSESlot;

/* Declare a secure element instance */
typedef struct __tSEInstance
{
   /* Flag indicating if the Secure Element information is built */
   bool bSEInfoIsBuilt;

   tNALServiceOperation sEndOfTransactionOperation;
   tNALServiceOperation sHotPlugOperation;

   uint32_t nGlobalPendingFlags;

   /* The WSESetPolicy operations */
   tDFCDriverCCReference pSetPolicyDriverCC;

   /* The policy to set after the host communication */
   tNFCControllerSEPolicy sAfterComPolicy;

   /* The policy currently requested */
   tNFCControllerSEPolicy sNewPolicy;

   /* The inialize callback parameters */
   tDFCCallbackContext sInitializeCallback;

   /* The current SE identifier used during initialization */
   uint32_t nSEIndentifierBeingInitialized;

   uint32_t nSENumber;
   tSESlot aSlotArray[NAL_MAXIMUM_SE_NUMBER];
} tSEInstance;

/**
 * @brief   Creates a secure element instance.
 *
 * @pre  Only one secure element instance is created at a given time.
 *
 * @param[in]  pSEInstance The secure element instance to initialize.
 **/
void PSECreate(
      tSEInstance* pSEInstance );



/**
 * @brief   Destroyes a secure element instance.
 *
 * If the instance is already destroyed, the function does nothing and returns.
 *
 * @post  PSEDestroy() does not return any error. The caller should always
 *        assume that the secure element instance is destroyed after this call.
 *
 * @post  The caller should never re-use the secure element instance value.
 *
 * @param[in]  pSEInstance  The secure element instance to destroy.
 **/
void PSEDestroy(
      tSEInstance* pSEInstance );

/**
 * @brief   Resets the data in the secure element instance.
 *
 * @param[in]  pContext  The current context.
 **/
void PSEDriverResetData(
      tContext* pContext );

/**
 * @brief  Initializes the Secure Element(s)
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pCallback  A pointer on the callback function.
 *
 * @param[in]  pCallbackParameter  A blind parameter transmitted to the callback function.
 **/
void PSEDriverInitialize(
            tContext* pContext,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter);

/**
 * Filters an APDU value before sending it to the Secure Element.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pReaderToCardBuffer  A pointer on the buffer containing the data to send to the card.
 *             This value may be null if nReaderToCardBufferLength is set to zero.
 *
 * @param[in]  nReaderToCardBufferLength  The length in bytes of the data to send to the card.
 *             This value may be zero.
 *
 * @return One of the following error code:
 *          - W_SUCCESS  The APDU can be sent to the Secure Element.
 *          - W_ERROR_BAD_STATE An exchange operation is already pending for this Secure Element.
 *          - W_ERROR_SECURITY  The APDU is not allowed by the access policy of the Secure Element.
 **/
W_ERROR PSEFilterApdu(
            tContext* pContext,
            const uint8_t* pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength );

/**
 * Notifies the response APDU of the Secure Element.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pCardToReaderBuffer  A pointer on the buffer with the data returned by the card.
 *
 * @param[in]  nCardToReaderBufferLength  The length in bytes of the buffer pCardToReaderBuffer.
 *
 * @param[in]  nError  W_SUCCESS in case of successful exchange, or an error code
 *             to notify an exchnage failure.
 *
 * @return W_SUCCESS or another error code.
 **/
W_ERROR PSENotifyResponseApdu(
            tContext* pContext,
            const uint8_t* pCardToReaderBuffer,
            uint32_t nCardToReaderBufferLength,
            W_ERROR nError );

#endif /* P_CONFIG_DRIVER ||P_CONFIG_MONOLITHIC */

#endif /* __WME_SECURE_ELEMENT_H */
