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

#ifndef __WME_UICC_H
#define __WME_UICC_H


#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

typedef struct __tUICCUserInstance
{
   tDFCCallbackContext  sCallbackContext;
   bool                 bInUse;

} tUICCUserInstance;

/**
 * @brief   Creates a UICC user instance
 *
 * @pre  Only one UICC user instance is created at a given time.
 *
 * @param[in]  pUICCUserInstance  The UICC user instance to initialize.
 **/
void PUICCUserCreate(
         tUICCUserInstance* pUICCUserInstance );

/**
 * @brief   Destroyes a UICC user instance.
 *
 * If the instance is already destroyed, the function does nothing and returns.
 *
 * @post  PUICCUserDesroy() does not return any error. The caller should always
 *        assume that the UICC user instance is destroyed after this call.
 *
 * @post  The caller should never re-use the UICC user instance value.
 *
 * @param[in]  pUICCUserInstance  The test engine instance to destroy.
 **/
void PUICCUserDestroy(
         tUICCUserInstance* pUICCUserInstance );


#endif


#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/*******************************************************************************
   Contains the declaration of the UICC functions
*******************************************************************************/

/*
 * The maximum length in bytes of the connectivity data,
 * as defined in the ETSI specification.
 */
#define P_UICC_MAX_CONNECTIVITY_DATA_LENGTH  255

/*
 * The maximum length in bytes of the connectivity AID,
 * as defined in the ETSI specification.
 */
#define P_UICC_MAX_CONNECTIVITY_AID_LENGTH  16

/* Declare a UICC instance */
typedef struct __tUICCInstance
{
   /* The pending operation flags */
   uint32_t nPendingFlags;

   /* WUICCGetAccessPolicy() & WUICCSetAccessPolicy() operation */
   tWUICCAccessPolicy sPersistentAccessPolicy;
   tWUICCAccessPolicy sVolatileAccessPolicy;
   tDFCDriverCCReference pSetAccessPolicyDriverCC;
   tWUICCAccessPolicy sNewAccessPolicy;
   /* Flag indicating if the UICC information is built */
   bool bUICCInfoIsBuilt;

   /* PUICCGetSlotInfo() operation */
   tDFCDriverCCReference pGetSlotInfoDriverCC;
   tNALServiceOperation sGetSlotInfoOperation;
   uint8_t aGetSlotInfoBuffer[1];

   /* The PUICCMonitorConnectivityEvent() operation */
   tHandleObjectHeader sMonitorConnectivityRegistry;
   tDFCDriverCCReference pMonitorConnectivityDriverCC;
   tNALServiceOperation sMonitorConnectivityOperation;
   uint32_t nConnectivityDataLength;
   uint8_t aConnectivityDataBuffer[P_UICC_MAX_CONNECTIVITY_DATA_LENGTH];

   /* The WUICCMonitorTransactionEvent operation */
   tHandleObjectHeader sMonitorTransactionEventRegistry;
   tDFCDriverCCReference pMonitorTransactionEventDriverCC;

   /* The WUICCGetTransactionEventAID operation */
   uint32_t nLastAIDLength;
   uint8_t aLastAID[P_UICC_MAX_CONNECTIVITY_AID_LENGTH];

   /* The PUICCActivateSWPLine operation */
   tNALServiceOperation  sActivateSWPLineOperation;
   tDFCDriverCCReference pEndOfActivateSWPLineDriverCC;
   bool bActivateSWPLineInProgress;

} tUICCInstance;

/**
 * @brief   Creates a UICC instance.
 *
 * @pre  Only one UICCinstance is created at a given time.
 *
 * @param[in]  pUICCInstance The UICC instance to initialize.
 **/
void PUICCCreate(
      tUICCInstance* pUICCInstance );

/**
 * @brief   Destroyes a UICC instance.
 *
 * If the instance is already destroyed, the function does nothing and returns.
 *
 * @post  PUICCDestroy() does not return any error. The caller should always
 *        assume that the UICC instance is destroyed after this call.
 *
 * @post  The caller should never re-use the UICC instance value.
 *
 * @param[in]  pUICCInstance  The UICC instance to destroy.
 **/
void PUICCDestroy(
      tUICCInstance* pUICCInstance );


/* Define a mask with all the UICC card protocols */
#define P_UICC_PROTOCOL_CARD_ALL (\
      W_NFCC_PROTOCOL_CARD_ISO_14443_4_A | \
      W_NFCC_PROTOCOL_CARD_ISO_14443_4_B | \
      W_NFCC_PROTOCOL_CARD_BPRIME | \
      W_NFCC_PROTOCOL_CARD_FELICA )

#endif /* P_CONFIG_DRIVER ||P_CONFIG_MONOLITHIC */

#endif /* __WME_UICC_H */
