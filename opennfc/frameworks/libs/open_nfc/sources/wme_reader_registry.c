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
   Contains the reader registry implementation.
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( REG )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/**
 * @brief   Creates a primary connection.
 *
 * The errors restuned in the callback function are the following:
 *    - W_SUCCESS in case of success.
 *    - W_ERROR_CONNECTION_COMPATIBILITY The card is not compliant with
 *      the requested connection.
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
 * @param[in]  nConnectionProperty  The connection property.
 *
 * @param[in]  pBuffer  The buffer containing the detection data.
 *
 * @param[in]  nLength  The length in bytes of the detection data.
 **/
typedef void tPReaderUserCreatePrimaryConnection(
            tContext* pContext,
            W_HANDLE hUserConnection,
            W_HANDLE hDriverConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nConnectionProperty,
            const uint8_t* pBuffer,
            uint32_t nLength );

/**
 * @brief   Creates a secondary connection.
 *
 * The errors restuned in the callback function are the following:
 *    - W_SUCCESS in case of success.
 *    - W_ERROR_CONNECTION_COMPATIBILITY The card is not compliant with
 *      the requested connection.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameterThe callback parameter.
 *
 * @param[in]  nConnectionProperty  The connection property.
 **/
typedef void tPReaderUserCreateSecondaryConnection(
            tContext* pContext,
            W_HANDLE hUserConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nConnectionProperty );

/**
 * @brief   Removes a secondary connection.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The connection handle.
 **/
typedef void tPReaderUserRemoveSecondaryConnection(
            tContext* pContext,
            W_HANDLE hUserConnection );

/** The connection type */
typedef const struct __tPReaderUserType
{
   uint8_t nConnectionProperty;
   uint32_t nDriverProtocol;
   tPReaderUserCreatePrimaryConnection* pCreatePrimaryConnection;
   tPReaderUserCreateSecondaryConnection* pCreateSecondaryConnection;
   tPReaderUserRemoveSecondaryConnection* pRemoveSecondaryConnection;
} tPReaderUserType;

/* The list of the reader properties (See Client API Specifications) */
static tPReaderUserType g_aPReaderUserTypeArray[] = {
   {  W_PROP_ISO_14443_3_A,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      P14P3UserCreateConnection,
      null,
      null
   },
#ifdef P_READER_14P4_STANDALONE_SUPPORT
   {  W_PROP_ISO_14443_4_A,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
      P14P4UserCreateConnection,
      null,
      null
   },
#else
   {  W_PROP_ISO_14443_4_A,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A, /* Warning: use ISO Part 3 */
      null,
      P14P4CreateConnection,
      null
   },
#endif /* P_READER_14P4_STANDALONE_SUPPORT */
   {  W_PROP_ISO_14443_3_B,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_B,
      P14P3UserCreateConnection,
      null,
      null
   },
#ifdef P_READER_14P4_STANDALONE_SUPPORT
   {  W_PROP_ISO_14443_4_B,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_B,
      P14P4UserCreateConnection,
      null,
      null
   },
#else
   {  W_PROP_ISO_14443_4_B,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_B, /* Warning: use ISO Part 3 */
      null,
      P14P4CreateConnection,
      null
   },
#endif /* P_READER_14P4_STANDALONE_SUPPORT */
   {  W_PROP_ISO_15693_3,
      W_NFCC_PROTOCOL_READER_ISO_15693_3,
      P15P3UserCreateConnection,
      null,
      null
   },
   {
      W_PROP_NXP_ICODE,
      W_NFCC_PROTOCOL_READER_ISO_15693_3,
      null,
      P15P3UserCreateSecondaryConnection,
      P15P3UserRemoveSecondaryConnection,
   },
   {
      W_PROP_TI_TAGIT,
      W_NFCC_PROTOCOL_READER_ISO_15693_3,
      null,
      P15P3UserCreateSecondaryConnection,
      P15P3UserRemoveSecondaryConnection,
   },
   {
      W_PROP_ST_LRI_512,
      W_NFCC_PROTOCOL_READER_ISO_15693_3,
      null,
      P15P3UserCreateSecondaryConnection,
      P15P3UserRemoveSecondaryConnection,
   },
   {
      W_PROP_ST_LRI_2K,
      W_NFCC_PROTOCOL_READER_ISO_15693_3,
      null,
      P15P3UserCreateSecondaryConnection,
      P15P3UserRemoveSecondaryConnection,
   },

   {  W_PROP_NFC_TAG_TYPE_1,
      W_NFCC_PROTOCOL_READER_TYPE_1_CHIP,
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
   {  W_PROP_NFC_TAG_TYPE_2,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
   {  W_PROP_NFC_TAG_TYPE_3,
      W_NFCC_PROTOCOL_READER_FELICA,
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
#ifdef P_READER_14P4_STANDALONE_SUPPORT
   {  W_PROP_NFC_TAG_TYPE_4_A,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
   {  W_PROP_NFC_TAG_TYPE_4_B,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_B,
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
#else
   {  W_PROP_NFC_TAG_TYPE_4_A,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,  /* Warning: use ISO Part 3 */
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
   {  W_PROP_NFC_TAG_TYPE_4_B,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_B,  /* Warning: use ISO Part 3 */
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
#endif
   {  W_PROP_NFC_TAG_TYPE_5,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_B,
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
   {  W_PROP_NFC_TAG_TYPE_6,
      W_NFCC_PROTOCOL_READER_ISO_15693_3,
      null,
      PNDEFCreateConnection,
      PNDEFRemoveConnection
   },
   {  W_PROP_TYPE1_CHIP,
      W_NFCC_PROTOCOL_READER_TYPE_1_CHIP,
      PType1ChipUserCreateConnection,
      null,
      null
   },
   {
      W_PROP_JEWEL,
      W_NFCC_PROTOCOL_READER_TYPE_1_CHIP,
      null,
      PType1UserCreateSecondaryConnection,
      PType1UserRemoveSecondaryConnection,
   },
   {
      W_PROP_TOPAZ,
      W_NFCC_PROTOCOL_READER_TYPE_1_CHIP,
      null,
      PType1UserCreateSecondaryConnection,
      PType1UserRemoveSecondaryConnection,
   },
   {
      W_PROP_TOPAZ_512,
      W_NFCC_PROTOCOL_READER_TYPE_1_CHIP,
      null,
      PType1UserCreateSecondaryConnection,
      PType1UserRemoveSecondaryConnection,
   },
   {  W_PROP_FELICA,
      W_NFCC_PROTOCOL_READER_FELICA,
      PFeliCaUserCreateConnection,
      null,
      null
   },
   {  W_PROP_PICOPASS_2K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_B,
      null,
      PPicoCreateConnection,
      PPicoRemoveConnection
   },
   {  W_PROP_PICOPASS_32K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_B,
      null,
      PPicoCreateConnection,
      PPicoRemoveConnection
   },
   {  W_PROP_ICLASS_2K,
      0,
      null,
      null,
      null
   },
   {  W_PROP_ICLASS_16K,
      0,
      null,
      null,
      null
   },
   {  W_PROP_MIFARE_UL,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },
   {  W_PROP_MIFARE_MINI,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },
   {  W_PROP_MIFARE_1K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },
   {  W_PROP_MIFARE_4K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },
   {  W_PROP_MIFARE_PLUS_S_2K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },
   {  W_PROP_MIFARE_PLUS_X_2K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },
   {  W_PROP_MIFARE_PLUS_S_4K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },
   {  W_PROP_MIFARE_PLUS_X_4K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMifareCreateConnection3A,
      PMifareRemoveConnection3A
   },

#ifdef P_READER_14P4_STANDALONE_SUPPORT
   {  W_PROP_MIFARE_DESFIRE_D40,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
      null,
      PMifareCreateConnection4A,
      null
   },
   {  W_PROP_MIFARE_DESFIRE_EV1_2K,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
      null,
      PMifareCreateConnection4A,
      null
   },
   {  W_PROP_MIFARE_DESFIRE_EV1_4K,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
      null,
      PMifareCreateConnection4A,
      null
   },
   {  W_PROP_MIFARE_DESFIRE_EV1_8K,
      W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
      null,
      PMifareCreateConnection4A,
      null
   },
#else
   {  W_PROP_MIFARE_DESFIRE_D40,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,  /* Warning: use ISO Part 3 */
      null,
      PMifareCreateConnection4A,
      null
   },
   {  W_PROP_MIFARE_DESFIRE_EV1_2K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,  /* Warning: use ISO Part 3 */
      null,
      PMifareCreateConnection4A,
      null
   },
   {  W_PROP_MIFARE_DESFIRE_EV1_4K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,  /* Warning: use ISO Part 3 */
      null,
      PMifareCreateConnection4A,
      null
   },
   {  W_PROP_MIFARE_DESFIRE_EV1_8K,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,  /* Warning: use ISO Part 3 */
      null,
      PMifareCreateConnection4A,
      null
   },

#endif
   {  W_PROP_ISO_7816_4_A,
      0,
      null,
      P7816CreateConnection,
      null
   },
   {  W_PROP_ISO_7816_4_B,
      0,
      null,
      P7816CreateConnection,
      null
   },
   {
      W_PROP_NFC_TAG_TYPE_2_GENERIC,
      0,
      null,
      PNDEF2GenCreateConnection,
      PNDEF2GenRemoveConnection
   },
   {
      W_PROP_BPRIME,
      W_NFCC_PROTOCOL_READER_BPRIME,
      PBPrimeUserCreateConnection,
      null,
      null
   },
   {  W_PROP_MY_D_MOVE,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMyDCreateConnection,
      PMyDRemoveConnection
   },
   {  W_PROP_MY_D_NFC,
      W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
      null,
      PMyDCreateConnection,
      PMyDRemoveConnection
   }
};

static const uint8_t g_aPReaderUserPropertyChain[] = {

   W_PROP_TYPE1_CHIP,    W_PROP_JEWEL,             0,
   W_PROP_TYPE1_CHIP,    W_PROP_TOPAZ,             W_PROP_NFC_TAG_TYPE_1,   0,
   W_PROP_TYPE1_CHIP,    W_PROP_TOPAZ_512,         W_PROP_NFC_TAG_TYPE_1,   0,
   W_PROP_TYPE1_CHIP,    W_PROP_NFC_TAG_TYPE_1,    0,
   W_PROP_FELICA,        W_PROP_NFC_TAG_TYPE_3,    0,
   W_PROP_ISO_14443_3_A, W_PROP_NFC_TAG_TYPE_2_GENERIC, W_PROP_MIFARE_UL, W_PROP_NFC_TAG_TYPE_2,  0,
   W_PROP_ISO_14443_3_A, W_PROP_NFC_TAG_TYPE_2_GENERIC, W_PROP_MY_D_MOVE, W_PROP_NFC_TAG_TYPE_2,  0,
   W_PROP_ISO_14443_3_A, W_PROP_NFC_TAG_TYPE_2_GENERIC, W_PROP_MY_D_NFC, W_PROP_NFC_TAG_TYPE_2,  0,
   W_PROP_ISO_14443_3_A, W_PROP_MY_D_NFC, W_PROP_NFC_TAG_TYPE_2,  0,
   W_PROP_ISO_14443_3_A, W_PROP_NFC_TAG_TYPE_2_GENERIC, W_PROP_NFC_TAG_TYPE_2,  0,
   W_PROP_ISO_14443_3_A, W_PROP_MIFARE_MINI,       0,
   W_PROP_ISO_14443_3_A, W_PROP_MIFARE_1K,         0,
   W_PROP_ISO_14443_3_A, W_PROP_MIFARE_4K,         0,
   W_PROP_ISO_14443_3_A, W_PROP_MIFARE_PLUS_S_2K,  0,
   W_PROP_ISO_14443_3_A, W_PROP_MIFARE_PLUS_X_2K,  0,
   W_PROP_ISO_14443_3_A, W_PROP_MIFARE_PLUS_S_4K,  0,
   W_PROP_ISO_14443_3_A, W_PROP_MIFARE_PLUS_X_4K,  0,

#ifdef P_READER_14P4_STANDALONE_SUPPORT

   W_PROP_ISO_14443_4_A, W_PROP_ISO_7816_4_A,      W_PROP_MIFARE_DESFIRE_D40, W_PROP_NFC_TAG_TYPE_4_A, 0,
   W_PROP_ISO_14443_4_A, W_PROP_ISO_7816_4_A,      W_PROP_MIFARE_DESFIRE_EV1_2K, W_PROP_NFC_TAG_TYPE_4_A, 0,
   W_PROP_ISO_14443_4_A, W_PROP_ISO_7816_4_A,      W_PROP_MIFARE_DESFIRE_EV1_4K, W_PROP_NFC_TAG_TYPE_4_A, 0,
   W_PROP_ISO_14443_4_A, W_PROP_ISO_7816_4_A,      W_PROP_MIFARE_DESFIRE_EV1_8K, W_PROP_NFC_TAG_TYPE_4_A, 0,

   W_PROP_ISO_14443_4_A, W_PROP_ISO_7816_4_A,      W_PROP_NFC_TAG_TYPE_4_A, 0,
#else
   W_PROP_ISO_14443_3_A, W_PROP_ISO_14443_4_A,     W_PROP_ISO_7816_4_A,    W_PROP_MIFARE_DESFIRE_D40, W_PROP_NFC_TAG_TYPE_4_A, 0,
   W_PROP_ISO_14443_3_A, W_PROP_ISO_14443_4_A,     W_PROP_ISO_7816_4_A,    W_PROP_MIFARE_DESFIRE_EV1_2K, W_PROP_NFC_TAG_TYPE_4_A, 0,
   W_PROP_ISO_14443_3_A, W_PROP_ISO_14443_4_A,     W_PROP_ISO_7816_4_A,    W_PROP_MIFARE_DESFIRE_EV1_4K, W_PROP_NFC_TAG_TYPE_4_A, 0,
   W_PROP_ISO_14443_3_A, W_PROP_ISO_14443_4_A,     W_PROP_ISO_7816_4_A,    W_PROP_MIFARE_DESFIRE_EV1_8K, W_PROP_NFC_TAG_TYPE_4_A, 0,
   W_PROP_ISO_14443_3_A, W_PROP_ISO_14443_4_A,     W_PROP_ISO_7816_4_A,    W_PROP_NFC_TAG_TYPE_4_A, 0,
#endif /* P_READER_14P4_STANDALONE_SUPPORT */


   W_PROP_ISO_15693_3,   W_PROP_NXP_ICODE,         W_PROP_NFC_TAG_TYPE_6, 0,
   W_PROP_ISO_15693_3,   W_PROP_ST_LRI_512,        W_PROP_NFC_TAG_TYPE_6, 0,
   W_PROP_ISO_15693_3,   W_PROP_ST_LRI_2K,         W_PROP_NFC_TAG_TYPE_6, 0,
   W_PROP_ISO_15693_3,   W_PROP_TI_TAGIT,          W_PROP_NFC_TAG_TYPE_6, 0,
   W_PROP_ISO_15693_3,   W_PROP_NFC_TAG_TYPE_6,    0,

   W_PROP_ISO_14443_3_B, W_PROP_PICOPASS_2K,       W_PROP_NFC_TAG_TYPE_5,  0,
   W_PROP_ISO_14443_3_B, W_PROP_PICOPASS_32K,      W_PROP_NFC_TAG_TYPE_5,  0,

#ifdef P_READER_14P4_STANDALONE_SUPPORT
   W_PROP_ISO_14443_4_B, W_PROP_ISO_7816_4_B,      W_PROP_NFC_TAG_TYPE_4_B, 0,
#else
   W_PROP_ISO_14443_3_B, W_PROP_ISO_14443_4_B,     W_PROP_ISO_7816_4_B,      W_PROP_NFC_TAG_TYPE_4_B, 0,
#endif /* P_READER_14P4_STANDALONE_SUPPORT */
};

/* List properties that are internal and not visible from API (WBasicGetConnectionProperties, etc...) */
static const uint8_t g_aPReaderUserInternalProperties[] = {
   W_PROP_NFC_TAG_TYPE_2_GENERIC
};

#define P_READER_INTERNAL_PROPERTY_NUMBER (sizeof(g_aPReaderUserInternalProperties) / sizeof(uint8_t))

/* Maximum number of properties */
#define P_READER_PROPERTY_MAX_NUMBER  \
   (sizeof(g_aPReaderUserTypeArray) / sizeof(tPReaderUserType))

/* Declare a reader registry structure */
typedef struct __tReaderUserListener
{
   /* Connection registry handle */
   tHandleObjectHeader sObjectHeader;

   uint8_t  aPropertyArray[P_READER_PROPERTY_MAX_NUMBER];
   uint32_t nPropertyNumber;

   /* Driver listener handle */
   W_HANDLE hDriverListener;

   /* Response buffer */
   uint8_t aCardToReaderBuffer[NAL_MESSAGE_MAX_LENGTH];

   /* Callback information */
   tDFCCallbackContext sCallbackContext;

} tReaderUserListener;

/* Declare a reader connection structure */
typedef struct __tReaderUserConnection
{
   /* Connection object registry */
   tHandleObjectHeader sObjectHeader;
   /* Registry handle */
   W_HANDLE hDriverConnection;

   /* The listener */
   tReaderUserListener* pUserListener;

   /* State machine variables for the connection build  */
   uint8_t nCurrentProperty;
   uint8_t nNextProperty;
   uint8_t nListIndex;
   bool bConnectionMatch;

   bool bPreviousCardApplicationMatch;

} tReaderUserConnection;

/**
 * @brief   Destroyes a user connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PReaderUserConnectionDestroy(
            tContext* pContext,
            void* pObject )
{
   tReaderUserConnection* pUserConnection = (tReaderUserConnection*)pObject;

   PDebugTrace("static_PReaderUserConnectionDestroy");

   /* Commit the user cache, if needed */
   PCacheConnectionUserCommit(pContext);

   /* Close driver handle */
   PHandleClose( pContext, pUserConnection->hDriverConnection );

   /* Decrement the reference count of the listener object  */
   PHandleDecrementReferenceCount(pContext, pUserConnection->pUserListener);

   CMemoryFree( pUserConnection );

   return P_HANDLE_DESTROY_DONE;
}

tHandleType g_sReaderUserConnection = {
   static_PReaderUserConnectionDestroy, null, null, null, null };

#define P_HANDLE_TYPE_READER_USER_CONNECTION (&g_sReaderUserConnection)

/* Destroy registry callback */
static uint32_t static_PReaderUserListenerDestroy(
            tContext* pContext,
            void* pObject );

tHandleType g_sReaderUserListener = { static_PReaderUserListenerDestroy, null, null, null, null };

#define P_HANDLE_TYPE_READER_USER_LISTENER (&g_sReaderUserListener)

/**
 * @brief   Destroyes a reader listener object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PReaderUserListenerDestroy(
            tContext* pContext,
            void* pObject )
{
   tReaderUserListener* pUserListener = (tReaderUserListener*)pObject;

   PDebugTrace("static_PReaderUserListenerDestroy");

   /* Free the registry buffer */
   if ( pUserListener != null )
   {
      PReaderDriverSetWorkPerformedAndClose(pContext, pUserListener->hDriverListener);

/*       PHandleClose( pContext, pUserListener->hDriverListener ); */

      CMemoryFree( pUserListener );
   }

   return P_HANDLE_DESTROY_DONE;
}

/** See header file */
void* PReaderUserConvertPointerToConnection(
                  tContext* pContext,
                  void* pPointer,
                  tHandleType* pExpectedType,
                  W_HANDLE* phUserConnection)
{
   W_ERROR nError;
   void* pObject;

   CDebugAssert(pPointer != null);

   *phUserConnection = PUtilConvertPointerToHandle(pPointer);

   nError = PHandleGetObject(pContext, *phUserConnection, pExpectedType, &pObject);

   /* Check the result */
   if ( ( nError != W_SUCCESS ) || ( pObject == null ) )
   {
      *phUserConnection = W_NULL_HANDLE;
      pObject = null;
   }

   return pObject;
}

/**
 * @brief  Looks for a property in the resquested listener list.
 *
 * @param[in] nConnectionProperty  The connection property to find.
 *
 * @param[in] pPropertyArray  The requested property array.
 *
 * @param[in] nPropertyNumber  The number of requested property.
 *
 * @return  true if the property is found, false otherwise.
 **/
static P_INLINE bool static_PReaderUserSearchPropertyInRequestedList(
                  uint8_t nConnectionProperty,
                  const uint8_t* pPropertyArray,
                  uint32_t nPropertyNumber)
{
   uint32_t nPos;

   for(nPos = 0; nPos < nPropertyNumber; nPos++)
   {
      if(nConnectionProperty == pPropertyArray[nPos])
      {
         return true;
      }
   }

   return false;
}

/**
 * @brief  Looks for a property in the chain list.
 *
 * @param[inout]   pnListIndex  The index of the first list updated with the index
 *                 of the list where is found the property.
 *
 * @param[in]      nConncetionProperty  The connection property.
 *
 * @return   The index following the property in the chain list
 *           or -1 if the property is not found.
 **/
static sint32_t static_PReaderUserLookInChainList(
                  uint8_t* pnListIndex,
                  uint8_t nConncetionProperty)
{
   uint8_t nListIndex = 0;
   uint32_t nPos = 0;
   uint8_t nValue;

   while(nListIndex < *pnListIndex)
   {
      while(g_aPReaderUserPropertyChain[nPos++] != 0) {}

      if(nPos == sizeof(g_aPReaderUserPropertyChain))
      {
         return -1;
      }

      nListIndex++;
   }

   for(;;)
   {
      nValue = g_aPReaderUserPropertyChain[nPos++];
      if(nValue == 0)
      {
         if(nPos == sizeof(g_aPReaderUserPropertyChain))
         {
            return -1;
         }
         nListIndex++;
      }
      else
      {
         if(nValue == nConncetionProperty)
         {
            break;
         }
      }
   }

   *pnListIndex = nListIndex;
   return nPos;
}

/**
 * @brief   Removes a connection.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The connection handle.
 *
 * @param[in]  nConnectionProperty  The connection property.
 **/
static void static_PReaderUserRemoveConnection(
                  tContext* pContext,
                  W_HANDLE hUserConnection,
                  uint8_t nConnectionProperty )
{
   uint32_t nIndex;

   for(nIndex = 0; nIndex < P_READER_PROPERTY_MAX_NUMBER; nIndex++)
   {
      if(g_aPReaderUserTypeArray[nIndex].nConnectionProperty == nConnectionProperty)
      {
         if(g_aPReaderUserTypeArray[nIndex].pRemoveSecondaryConnection != null)
         {
            g_aPReaderUserTypeArray[nIndex].pRemoveSecondaryConnection(
               pContext, hUserConnection );
         }
         return;
      }
   }
   CDebugAssert(false);
}

/**
 * @brief   Adds a connection.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The connection handle.
 *
 * @param[in]  pCallback  The callback function.
 *
 * @param[in]  pCallbackParameterThe callback parameter.
 *
 * @param[in]  nConnectionProperty  The connection property.
 **/
static void static_PReaderUserAddConnection(
                  tContext* pContext,
                  W_HANDLE hUserConnection,
                  tPBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t nConnectionProperty )
{
   uint32_t nIndex;

   for(nIndex = 0; nIndex < P_READER_PROPERTY_MAX_NUMBER; nIndex++)
   {
      if(g_aPReaderUserTypeArray[nIndex].nConnectionProperty == nConnectionProperty)
      {
         CDebugAssert(g_aPReaderUserTypeArray[nIndex].pCreateSecondaryConnection != null);
         g_aPReaderUserTypeArray[nIndex].pCreateSecondaryConnection(
            pContext, hUserConnection, pCallback, pCallbackParameter, nConnectionProperty );
         return;
      }
   }
   CDebugAssert(false);
}

/**
 * @brief   Receives the completion of a connection creation initiated by a create connection call.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pCallbackParameter  The callback parameter specified for the call.
 *
 * @param[in]  nError  The error code:
 *               - W_SUCCESS in case of success.
 *               - W_ERROR_CONNECTION_COMPATIBILITY The card is not compliant with the requested connection.
 **/
static void static_PReaderUserCreateConnectionCompleted(
                  tContext* pContext,
                  void* pCallbackParameter,
                  W_ERROR nError )
{
   /* Algorithm

   Variables:
      CURRENT  current property
      NEXT  next property
      LIST#  chain list index
      MATCH  connection match flag

   Initial values:
      CURRENT = 0
      NEXT = initial property
      LIST# = 0
      MATCH = false

   If an error is returned =>  LIST# ++
   else (success)
   {
      CURRENT = NEXT
      NEXT = 0
      LIST# = 0
   }

   if CURRENT==0 => failure, no event, stop

   if CURRENT is in listener requested list => MATCH = true

   Loop
   {
      Look for CURRENT in the chain lists from LIST# (included)

      If not found
      {
         If MATCH == true => Send success event, stop
         Else => failure, no event, stop
      }
      Else If CURRENT is the last of the current chain list
      {
         LIST# ++
      }
      Else
      {
         Look if another property in the requested list is in the remaining
         properties of the current chain list
         If not found
         {
            LIST# ++
         }
         Else
         {
            If following property in the current chain list == NEXT
            {
               LIST# ++
            }
            Else
            {
               Try the detection of the following property in the current chain list
               Set NEXT with the following property
               break loop to wait event
            }
         }
      }
   } End Loop

   */
   W_HANDLE hUserConnection;
   tReaderUserConnection* pUserConnection  = (tReaderUserConnection*)PReaderUserConvertPointerToConnection(
      pContext, pCallbackParameter, P_HANDLE_TYPE_READER_USER_CONNECTION, &hUserConnection);
   tReaderUserListener* pUserListener = pUserConnection->pUserListener;
   sint32_t nPos;

   PDebugTrace("static_PReaderUserCreateConnectionCompleted");

   /* Check if the create connection was successful */
   if ( nError == W_SUCCESS )
   {
      uint8_t aPropertyArray[15];

      /* Check if the property set by the Create Property function is equal or different
         from the property requested */
      nError = PHandleGetProperties( pContext, hUserConnection, aPropertyArray, sizeof(aPropertyArray) );
      if(nError != W_SUCCESS)
      {
         PDebugError("static_PReaderUserCreateConnectionCompleted: PHandleGetProperties() in error");
         goto return_error;
      }

      if(aPropertyArray[0] == pUserConnection->nNextProperty)
      {
         PDebugTrace( "static_PReaderUserCreateConnectionCompleted: Success for %s",
            PUtilTraceConnectionProperty(pUserConnection->nNextProperty) );
      }
      else
      {
         if (aPropertyArray[0] != W_PROP_ISO_7816_4)
         {

            PDebugTrace( "static_PReaderUserCreateConnectionCompleted: Success for %s replacing %s",
               PUtilTraceConnectionProperty(aPropertyArray[0]), PUtilTraceConnectionProperty(pUserConnection->nNextProperty) );

            pUserConnection->nNextProperty = aPropertyArray[0];
         }
         else
         {
             PDebugTrace( "static_PReaderUserCreateConnectionCompleted: Success for %s",
                PUtilTraceConnectionProperty(pUserConnection->nNextProperty));
         }

      }

      pUserConnection->nCurrentProperty = pUserConnection->nNextProperty;
      pUserConnection->nNextProperty = 0;
      /* Keep bConnectionMatch */
      pUserConnection->nListIndex = 0;
   }
   else
   {
      if ( nError == W_ERROR_CONNECTION_COMPATIBILITY )
      {
         /* Remove the faulty object form the connection */
         static_PReaderUserRemoveConnection( pContext, hUserConnection,
                  pUserConnection->nNextProperty );
         /* Keep nCurrentProperty */
         /* Keep nNextProperty */
         /* Keep bConnectionMatch */
         pUserConnection->nListIndex++;
      }
      else
      {
         PDebugError( "static_PReaderUserCreateConnectionCompleted: receive error %s for %s",
            PUtilTraceError(nError), PUtilTraceConnectionProperty(pUserConnection->nNextProperty) );

         goto return_error;
      }
   }

   if(pUserConnection->nCurrentProperty == 0)
   {
      PDebugError( "static_PReaderUserCreateConnectionCompleted: Error in the first step");
      CDebugAssert(pUserConnection->bConnectionMatch == false);
      PHandleClose( pContext, hUserConnection );
      return;
   }

   /* Look for the property in the requested listener list */
   if(static_PReaderUserSearchPropertyInRequestedList(
         pUserConnection->nCurrentProperty,
         pUserListener->aPropertyArray, pUserListener->nPropertyNumber) != false)
   {
      PDebugTrace("static_PReaderUserCreateConnectionCompleted: Find a match for %s",
         PUtilTraceConnectionProperty(pUserConnection->nCurrentProperty));
      pUserConnection->bConnectionMatch = true;
   }


   while((nPos = static_PReaderUserLookInChainList(
      &pUserConnection->nListIndex, pUserConnection->nCurrentProperty)) >= 0)
   {
      sint32_t nPos2 = nPos;

      /* If the current property is the last of the chain list */
      while(g_aPReaderUserPropertyChain[nPos2] != 0)
      {
         /* Look if another property in the requested list is in the remaining
         properties of the current chain list */
         if(static_PReaderUserSearchPropertyInRequestedList(
               g_aPReaderUserPropertyChain[nPos2],
               pUserListener->aPropertyArray, pUserListener->nPropertyNumber) != false)
         {
            if(g_aPReaderUserPropertyChain[nPos] == pUserConnection->nNextProperty)
            {
               break;
            }
            else
            {
               /* Try the detection of the next property */
               static_PReaderUserAddConnection(
                  pContext,
                  hUserConnection,
                  static_PReaderUserCreateConnectionCompleted,
                  PUtilConvertHandleToPointer(hUserConnection),
                  g_aPReaderUserPropertyChain[nPos] );

               /* Set the next property */
               pUserConnection->nNextProperty = g_aPReaderUserPropertyChain[nPos];

               return;
            }
         }

         nPos2++;
      }

      pUserConnection->nListIndex++;
   }

   if(pUserConnection->bConnectionMatch != false)
   {
      /* Call the callback related to this handle */
      PDFCPostContext3(
         &pUserListener->sCallbackContext,
         P_DFC_TYPE_READER,
         hUserConnection,
         W_SUCCESS );
   }
   else
   {
      PHandleClose( pContext, hUserConnection );
   }

   return;

return_error:

   /* Call the callback related to this handle */
   PDFCPostContext3(
      &pUserListener->sCallbackContext,
      P_DFC_TYPE_READER,
      W_NULL_HANDLE,
      nError );

   PHandleClose( pContext, hUserConnection );
}

/**
 * @brief   Creates the connection at reader level.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pUserListener  The reader registry structure.
 *
 * @param[in]  hDriverConnection  The driver connection handle.
 *
 * @param[in]  nProperty  The property number.
 *
 * @param[in]  bPreviousCardApplicationMatch  The card application match flag.
 *
 * @param[out] phUserConnection  The connection handle.
 *
 * @return  The result code.
 **/
static W_ERROR static_PReaderUserCreateConnection(
            tContext* pContext,
            tReaderUserListener* pUserListener,
            W_HANDLE hDriverConnection,
            uint8_t nProperty,
            bool bPreviousCardApplicationMatch,
            W_HANDLE* phUserConnection)
{
   tReaderUserConnection* pUserConnection;
   W_HANDLE hUserConnection;
   W_ERROR nError;

   PDebugTrace("static_PReaderUserCreateConnection");

   /* Create the reader buffer */
   pUserConnection = (tReaderUserConnection*)CMemoryAlloc( sizeof(tReaderUserConnection) );
   if ( pUserConnection == null )
   {
      PDebugError("static_PReaderUserCreateConnection: pUserConnection == null");
      return W_ERROR_OUT_OF_RESOURCE;
   }
   CMemoryFill(pUserConnection, 0, sizeof(tReaderUserConnection));

   /* Register the reader connection structure */
   if ( ( nError = PHandleRegister(
                        pContext,
                        pUserConnection,
                        P_HANDLE_TYPE_READER_USER_CONNECTION,
                        &hUserConnection) ) != W_SUCCESS )
   {
      PDebugError("static_PReaderUserCreateConnection: error on PHandleRegister %s",
         PUtilTraceError(nError) );
      CMemoryFree(pUserConnection);
      return nError;
   }

   /* Store the driver connection handle */
   pUserConnection->hDriverConnection = hDriverConnection;

   /* Set the user listener */
   pUserConnection->pUserListener = pUserListener;

   /* Set the current value of the application match flag */
   pUserConnection->bPreviousCardApplicationMatch = bPreviousCardApplicationMatch;

   /* Increment the reference count of the listener object */
   PHandleIncrementReferenceCount(pUserListener);

   /* Initalize the state machine variables */
   pUserConnection->nCurrentProperty = 0;
   pUserConnection->nNextProperty = nProperty;
   pUserConnection->nListIndex = 0;
   pUserConnection->bConnectionMatch = false;

   /* Return the connection */
   *phUserConnection = hUserConnection;

   return W_SUCCESS;
}

/** See tPReaderDriverRegisterCompleted **/
static void static_PReaderUserCardDetectionHandler(
                  tContext* pContext,
                  void* pCallbackParameter,
                  uint32_t nDriverProtocol,
                  W_HANDLE hDriverConnection,
                  uint32_t nLength,
                  bool bPreviousCardApplicationMatch )
{
   tReaderUserListener* pUserListener = (tReaderUserListener*)pCallbackParameter;
   W_HANDLE hUserConnection = W_NULL_HANDLE;
   W_ERROR nError = W_SUCCESS;
   uint8_t nProperty = 0;
   uint32_t nIndex;
   tPReaderUserCreatePrimaryConnection* pCreatePrimaryConnection;

   uint8_t* pBuffer = pUserListener->aCardToReaderBuffer;

   PDebugTrace("static_PReaderUserCardDetectionHandler()" );
   PDebugTraceBuffer( pBuffer, nLength );

   /* Register the different connection property buffers */
   pCreatePrimaryConnection = null;
   for(nIndex = 0; nIndex < P_READER_PROPERTY_MAX_NUMBER; nIndex++)
   {
      if(g_aPReaderUserTypeArray[nIndex].pCreatePrimaryConnection != null)
      {
         if((g_aPReaderUserTypeArray[nIndex].nDriverProtocol & nDriverProtocol) != 0)
         {
            pCreatePrimaryConnection = g_aPReaderUserTypeArray[nIndex].pCreatePrimaryConnection;
            nProperty = g_aPReaderUserTypeArray[nIndex].nConnectionProperty;
            break;
         }
      }
   }

   if(pCreatePrimaryConnection == null)
   {
      PDebugError(
         "static_PReaderUserCardDetectionHandler: wrong driver protocol %s",
         PUtilTraceReaderProtocol(pContext, nDriverProtocol) );
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* Get the connection handle */
   nError = static_PReaderUserCreateConnection(
            pContext,
            pUserListener,
            hDriverConnection,
            nProperty,
            bPreviousCardApplicationMatch,
            &hUserConnection );

   if ( nError != W_SUCCESS )
   {
      PDebugError("static_PReaderUserCardDetectionHandler: static_PReaderUserCreateConnection");
      goto return_error;
   }

   /* Update the User Connection Cache*/
   PCacheConnectionUserUpdate(pContext);

   /* Register the different connection property buffers */
   pCreatePrimaryConnection(
            pContext,
            hUserConnection,
            hDriverConnection,
            static_PReaderUserCreateConnectionCompleted,
            PUtilConvertHandleToPointer(hUserConnection),
            nProperty,
            pBuffer, nLength );

   return;

return_error:

   PDebugError(
         "static_PReaderUserCardDetectionHandler: sending error %s",
         PUtilTraceError(nError) );

   /* Call the callback related to this handle */
   PDFCPostContext3(
      &pUserListener->sCallbackContext,
      P_DFC_TYPE_READER,
      W_NULL_HANDLE,
      nError );

   /* If the connection handle exists */
   PHandleClose(pContext, hUserConnection );
}

/**
 * @brief   Checks a connection property and returns the corresponding driver protocol value.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  nConnectionProperty  The connection property to check.
 *
 * @return  The protocol value or zero if the property is not supported.
 **/
static uint32_t static_PReaderUserCheckProperty(
         tContext* pContext,
         uint8_t nConnectionProperty)
{
   uint32_t nDriverProtocol = 0;
   uint32_t nIndex;
   bool bFound = false;

   for(nIndex = 0; nIndex < P_READER_PROPERTY_MAX_NUMBER; nIndex++)
   {
      if(g_aPReaderUserTypeArray[nIndex].nConnectionProperty == nConnectionProperty)
      {
         nDriverProtocol = g_aPReaderUserTypeArray[nIndex].nDriverProtocol;
         bFound = true;
         break;
      }
   }

   if(bFound == false)
   {
      PDebugWarning("static_PReaderUserCheckProperty: Illegal Property 0x%02X (%s)",
          nConnectionProperty, PUtilTraceConnectionProperty(nConnectionProperty));

      CDebugAssert(nDriverProtocol == 0);
   }
   else
   {
      if(PReaderIsPropertySupported(pContext, nConnectionProperty) == false)
      {
         PDebugWarning("static_PReaderUserCheckProperty: Unsupported Property %s",
             PUtilTraceConnectionProperty(nConnectionProperty));

         nDriverProtocol = 0;
      }
   }

   return nDriverProtocol;
}

/**
 * @brief   Checks the property array values and builds the driver protocol value.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pSourcePropertyArray  The source property array.
 *
 * @param[in]  nSourcePropertyNumber  The number of properties in the source property array.
 *
 * @param[out] pDestPropertyArray  The destination property array.
 *
 * @param[out] pnDestPropertyNumber  A pointer on a variable valued with the
 *             number of properties in the destination property array.
 *
 * @param[out] pnDriverProtocol  A pointer on a variable valued with the driver protocols.
 *
 * @return  W_SUCCESS if the property is supported, an error otherwise.
 **/
static W_ERROR static_PReaderUserCheckPropertyArray(
            tContext* pContext,
            const uint8_t* pSourcePropertyArray,
            uint32_t nSourcePropertyNumber,
            uint8_t* pDestPropertyArray,
            uint32_t* pnDestPropertyNumber,
            uint32_t* pnDriverProtocol )
{
   uint32_t nDestPropertyNumber = 0;
   uint32_t nDriverProtocol = 0;

   CDebugAssert(pDestPropertyArray != null);
   CDebugAssert(pnDestPropertyNumber != null);
   CDebugAssert(pnDriverProtocol != null);

   if(((pSourcePropertyArray == null) && (nSourcePropertyNumber != 0))
   || ((pSourcePropertyArray != null) && (nSourcePropertyNumber == 0)))
   {
      PDebugError("static_PReaderUserCheckPropertyArray: Bad null value for the property array or the length");
      *pnDestPropertyNumber = 0;
      *pnDriverProtocol = 0;
      return W_ERROR_BAD_PARAMETER;
   }

   if(pSourcePropertyArray == null)
   {
      PDebugTrace("static_PReaderUserCheckPropertyArray: Registration for every protocol");
      nSourcePropertyNumber = P_READER_PROPERTY_MAX_NUMBER;
   }

   CDebugAssert(nSourcePropertyNumber != 0);

   do
   {
      uint8_t nConnectionProperty;

      if(pSourcePropertyArray != null)
      {
         nConnectionProperty = pSourcePropertyArray[--nSourcePropertyNumber];
      }
      else
      {
         nConnectionProperty = g_aPReaderUserTypeArray[--nSourcePropertyNumber].nConnectionProperty;

         if ((nConnectionProperty == W_PROP_ISO_7816_4_A) || (nConnectionProperty == W_PROP_ISO_7816_4_B))
         {
            continue;
         }
      }

      if(nConnectionProperty == W_PROP_ISO_7816_4)
      {
         /* Ignore this property */
      }
      else if(static_PReaderUserSearchPropertyInRequestedList(
                  nConnectionProperty, pDestPropertyArray, nDestPropertyNumber) != false)
      {
         PDebugWarning("static_PReaderUserCheckPropertyArray: The property %s is already in the list",
               PUtilTraceConnectionProperty(nConnectionProperty));
      }
      else
      {
         uint32_t nTempProtocol = static_PReaderUserCheckProperty(pContext, nConnectionProperty);

         if(nTempProtocol != 0)
         {
            nDriverProtocol |= nTempProtocol;
            pDestPropertyArray[nDestPropertyNumber++] = nConnectionProperty;

            PDebugTrace("static_PReaderUserCheckPropertyArray: Adding the property %s",
               PUtilTraceConnectionProperty(nConnectionProperty));
         }

         /* Special case ISO 7816 */
          if((nConnectionProperty == W_PROP_ISO_14443_4_A)
         || (nConnectionProperty == W_PROP_ISO_14443_4_B))
         {
            if(static_PReaderUserSearchPropertyInRequestedList(
                        W_PROP_ISO_7816_4, pDestPropertyArray, nDestPropertyNumber) == false)
            {
               pDestPropertyArray[nDestPropertyNumber++] = W_PROP_ISO_7816_4;
               PDebugTrace("static_PReaderUserCheckPropertyArray: Spontaneously adding the property W_PROP_ISO_7816_4");
            }

            if (nConnectionProperty == W_PROP_ISO_14443_4_A)
            {
               pDestPropertyArray[nDestPropertyNumber++] = W_PROP_ISO_7816_4_A;
               PDebugTrace("static_PReaderUserCheckPropertyArray: Spontaneously adding the property W_PROP_ISO_7816_4_A");

            }
            else
            {
               pDestPropertyArray[nDestPropertyNumber++] = W_PROP_ISO_7816_4_B;
               PDebugTrace("static_PReaderUserCheckPropertyArray: Spontaneously adding the property W_PROP_ISO_7816_4_B");
            }

         }
      }
   } while(nSourcePropertyNumber > 0);

   if(nDriverProtocol == 0)
   {
      PDebugWarning("static_PReaderUserCheckPropertyArray: No protocol remaining for the registration");
   }
   else
   {
      PDebugTrace("static_PReaderUserCheckPropertyArray: The driver protocols are %s",
            PUtilTraceReaderProtocol(pContext, nDriverProtocol));
   }

   *pnDestPropertyNumber = nDestPropertyNumber;
   *pnDriverProtocol = nDriverProtocol;
   return W_SUCCESS;
}

/* See PReaderListenToCardDetection */
W_ERROR PReaderUserListenToCardDetection(
            tContext* pContext,
            tPReaderCardDetectionHandler *pHandler,
            void *pHandlerParameter,
            uint8_t nPriority,
            const uint8_t* pConnectionPropertyArray,
            uint32_t nPropertyNumber,
            const void* pDetectionConfigurationBuffer,
            uint32_t nDetectionConfigurationBufferLength,
            W_HANDLE *phEventRegistry )
{
   tReaderUserListener* pUserListener = null;
   W_ERROR nError;
   W_HANDLE hUserListenerHandle = W_NULL_HANDLE;
   uint32_t nDriverProtocol;

   PDebugTrace("PReaderUserListenToCardDetection()");

   /* Check the parameters */
   if ( phEventRegistry == null )
   {
      PDebugError("PReaderUserListenToCardDetection: phEventRegistry == null");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ( ((pConnectionPropertyArray == null) && (nPropertyNumber != 0)) ||
        ((pConnectionPropertyArray != null) && (nPropertyNumber == 0)))
   {
      PDebugError("PReaderUserListenToCardDetection: inconsistency between pDetectionConfigurationBuffer and  nDetectionConfigurationBufferLength");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* Create the reader structure */
   pUserListener = (tReaderUserListener*)CMemoryAlloc( sizeof(tReaderUserListener) );
   if ( pUserListener == null )
   {
      PDebugError("PReaderUserListenToCardDetection: pUserListener == null");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }
   CMemoryFill(pUserListener, 0, sizeof(tReaderUserListener));

   pUserListener->hDriverListener = W_NULL_HANDLE;

   /* Check the property array */
   if ( ( nError = static_PReaderUserCheckPropertyArray(
                     pContext, pConnectionPropertyArray, nPropertyNumber,
                     pUserListener->aPropertyArray,
                     &pUserListener->nPropertyNumber,
                     &nDriverProtocol ) ) != W_SUCCESS )
   {
      PDebugWarning("PReaderUserListenToCardDetection: unknown/unsupported property");
      goto return_error;
   }

   /* Get a reader handle */
   if ( ( nError = PHandleRegister(
                     pContext,
                     pUserListener,
                     P_HANDLE_TYPE_READER_USER_LISTENER,
                     &hUserListenerHandle) ) != W_SUCCESS )
   {
      PDebugError("PReaderUserListenToCardDetection: error on PHandleRegister()");
      goto return_error;
   }

   /* Store the reader information */
   *phEventRegistry = hUserListenerHandle;

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pHandler,
      pHandlerParameter,
      &pUserListener->sCallbackContext );

   /*use aCardToReaderBuffer to copy parameters*/
   if((pDetectionConfigurationBuffer != null) && (nDetectionConfigurationBufferLength != 0))
   {
      CMemoryCopy(
         pUserListener->aCardToReaderBuffer,
         pDetectionConfigurationBuffer, nDetectionConfigurationBufferLength);
   }
   else
   {
      /* reset the buffer*/
      pUserListener->aCardToReaderBuffer[0] = 0;
   }

   /* Register with the driver, if the protocol list is non-empty */
   if (nDriverProtocol != 0)
   {
      /* Register the protocol(s) */
      nError = PReaderDriverRegister(
                  pContext,
                  static_PReaderUserCardDetectionHandler,
                  pUserListener,
                  nPriority,
                  nDriverProtocol,
                  nDetectionConfigurationBufferLength,
                  pUserListener->aCardToReaderBuffer,
                  NAL_MESSAGE_MAX_LENGTH,
                  &pUserListener->hDriverListener );

      /* Check the result */
      if ( nError != W_SUCCESS )
      {
         PDebugError(
            "PReaderUserListenToCardDetection: PReaderDriverRegister() returned an error");
         goto return_error;
      }
   }

   return W_SUCCESS;

return_error:

   PDebugError("PReaderUserListenToCardDetection: Returning %s",
      PUtilTraceError(nError) );

   if ( phEventRegistry != null )
   {
      *phEventRegistry = W_NULL_HANDLE;
   }

   if(hUserListenerHandle != W_NULL_HANDLE)
   {
      /* Close the user registry */
      PHandleClose( pContext, hUserListenerHandle );
   }
   else
   {
      if (pUserListener != null)
      {
         PHandleClose( pContext, pUserListener->hDriverListener );

         CMemoryFree( pUserListener );
      }
   }

   return nError;
}

/* See Client API Specifications */
W_ERROR PReaderListenToCardDetection(
                  tContext * pContext,
                  tPReaderCardDetectionHandler* pHandler,
                  void* pHandlerParameter,
                  uint8_t nPriority,
                  const uint8_t* pConnectionPropertyArray,
                  uint32_t nPropertyNumber,
                  W_HANDLE* phEventRegistry)
{
   PDebugTrace("PReaderListenToCardDetection()");

   /* Do not allow user to specific priority of the SE */

   if ((nPriority == W_PRIORITY_SE) || (nPriority == W_PRIORITY_SE_FORCED))
   {
      return W_ERROR_BAD_PARAMETER;
   }

   return PReaderUserListenToCardDetection(
            pContext,
            pHandler, pHandlerParameter,
            nPriority,
            pConnectionPropertyArray, nPropertyNumber,
            null, 0,
            phEventRegistry );
}

/* See Client API Specifications */
void PReaderHandlerWorkPerformed(
         tContext * pContext,
         W_HANDLE hUserConnection,
         bool bGiveToNextListener,
         bool bCardApplicationMatch )
{
   tReaderUserConnection* pUserConnection;
   W_ERROR nError = PHandleGetObject(pContext, hUserConnection, P_HANDLE_TYPE_READER_USER_CONNECTION, (void**)&pUserConnection);

   PDebugTrace("PReaderHandlerWorkPerformed: hUserConnection=0x%08X", hUserConnection);

   /* If the connection has been found */
   if ( ( nError == W_SUCCESS ) && ( pUserConnection != null ) )
   {
      /* Commit the user cache, if needed */
      PCacheConnectionUserCommit(pContext);

      /* Call the driver function */
      PReaderDriverWorkPerformed(
         pContext,
         pUserConnection->hDriverConnection,
         bGiveToNextListener,
         bCardApplicationMatch );

      /* @todo what can we do if the IOCTL failed */

      /* hDriverConnection is closed */
      pUserConnection->hDriverConnection = W_NULL_HANDLE;

      /* Close handle */
      PHandleClose(pContext, hUserConnection );
   }
   else
   {
      PDebugError("PReaderHandlerWorkPerformed: could not get pUserConnection buffer");
   }
}

/* See Client API Specifications */
bool PReaderIsPropertySupported(
         tContext * pContext,
         uint8_t nPropertyIdentifier )
{
   bool bValue;
   tNFCControllerInfo* pNFCControllerInfo = PContextGetNFCControllerInfo( pContext );
   uint32_t nProtocols = pNFCControllerInfo->nProtocolCapabilities;
   uint32_t nCapabilities = pNFCControllerInfo->nFirmwareCapabilities;

   switch(nPropertyIdentifier)
   {
   case W_PROP_ISO_14443_3_B:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_ISO_14443_3_B) != 0)?true:false;
      break;
   case W_PROP_ISO_14443_4_B:
   case W_PROP_NFC_TAG_TYPE_4_B:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_ISO_14443_4_B) != 0)?true:false;
      break;
   case W_PROP_NFC_TAG_TYPE_6:
   case W_PROP_ISO_15693_3:
   case W_PROP_TI_TAGIT:
   case W_PROP_ST_LRI_512:
   case W_PROP_ST_LRI_2K:
   case W_PROP_NXP_ICODE:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_ISO_15693_3) != 0)?true:false;
      break;
   case W_PROP_NFC_TAG_TYPE_1:
   case W_PROP_TYPE1_CHIP:
   case W_PROP_JEWEL:
   case W_PROP_TOPAZ:
   case W_PROP_TOPAZ_512:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_TYPE_1_CHIP) != 0)?true:false;
      break;
   case W_PROP_NFC_TAG_TYPE_3:
   case W_PROP_FELICA:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_FELICA) != 0)?true:false;
      break;
   case W_PROP_NFC_TAG_TYPE_5:
   case W_PROP_PICOPASS_2K:
   case W_PROP_PICOPASS_32K:
      if((nProtocols & W_NFCC_PROTOCOL_READER_ISO_14443_3_B) != 0)
      {
         bValue = ((nCapabilities & NAL_CAPA_READER_ISO_14443_B_PICO) != 0)?true:false;
      }
      else
      {
         bValue = false;
      }
      break;
   case W_PROP_ISO_15693_2:
   case W_PROP_ICLASS_2K:
   case W_PROP_ICLASS_16K:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_ISO_15693_2) != 0)?true:false;
      break;
   case W_PROP_ISO_14443_3_A:
   case W_PROP_NFC_TAG_TYPE_2:
   case W_PROP_NFC_TAG_TYPE_2_GENERIC:
   case W_PROP_MY_D_MOVE:
   case W_PROP_MY_D_NFC:
   case W_PROP_MIFARE_UL:
   case W_PROP_MIFARE_UL_C:
   case W_PROP_MIFARE_MINI:
   case W_PROP_MIFARE_1K:
   case W_PROP_MIFARE_4K:
   case W_PROP_MIFARE_PLUS_X_2K:
   case W_PROP_MIFARE_PLUS_X_4K:
   case W_PROP_MIFARE_PLUS_S_2K:
   case W_PROP_MIFARE_PLUS_S_4K:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_ISO_14443_3_A) != 0)?true:false;
      break;
   case W_PROP_ISO_14443_4_A:
   case W_PROP_NFC_TAG_TYPE_4_A:
   case W_PROP_MIFARE_DESFIRE_D40:
   case W_PROP_MIFARE_DESFIRE_EV1_2K:
   case W_PROP_MIFARE_DESFIRE_EV1_4K:
   case W_PROP_MIFARE_DESFIRE_EV1_8K:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_ISO_14443_4_A) != 0)?true:false;
      break;
   case W_PROP_BPRIME:
      bValue = ((nProtocols & W_NFCC_PROTOCOL_READER_BPRIME) != 0)?true:false;
      break;
   /* Special values not usable with detection filter */
   case W_PROP_ISO_7816_4:
      bValue = (((nProtocols & W_NFCC_PROTOCOL_READER_ISO_14443_4_A) != 0)
             || ((nProtocols & W_NFCC_PROTOCOL_READER_ISO_14443_4_B) != 0))?true:false;;
      break;
   case W_PROP_SECURE_ELEMENT:
   case W_PROP_VIRTUAL_TAG:
   case W_PROP_P2P_TARGET:
   case W_PROP_ISO_7816_4_A:
   case W_PROP_ISO_7816_4_B:
   default:
      PDebugError("PReaderIsPropertySupported: Wrong property value %08X", nPropertyIdentifier);
      return false;
   }

   return bValue;
}

/* See Client API Specifications */
bool PReaderPreviousApplicationMatch(
         tContext * pContext,
         W_HANDLE hUserConnection)
{
   tReaderUserConnection* pUserConnection;
   W_ERROR nError = PHandleGetObject(pContext, hUserConnection, P_HANDLE_TYPE_READER_USER_CONNECTION, (void**)&pUserConnection);

   PDebugTrace("PReaderPreviousApplicationMatch: hUserConnection=0x%08X", hUserConnection);

   if ( ( nError == W_SUCCESS ) && ( pUserConnection != null ) )
   {
      return pUserConnection->bPreviousCardApplicationMatch;
   }

   PDebugError("PReaderPreviousApplicationMatch: Bad handle value");

   return false;
}

/* See Client API Specifications */
W_ERROR WReaderSetPulsePeriodSync(
                  uint32_t nPulsePeriod )
{
   tPBasicGenericSyncParameters param;

   if(PBasicGenericSyncPrepare(&param) != false)
   {
      WReaderSetPulsePeriod(PBasicGenericSyncCompletion, &param, nPulsePeriod);
   }

   return PBasicGenericSyncWaitForResult(&param);
}

/* See header file */
W_ERROR PReaderUserGetConnectionObject(
            tContext* pContext,
            W_HANDLE hUserConnection,
            tHandleType* pExpectedType,
            void** ppObject)
{
   W_ERROR nError;
   uint32_t nPropertyNumber;

   if(hUserConnection == W_NULL_HANDLE)
   {
      return W_ERROR_BAD_HANDLE;
   }

   nError = PHandleGetObject(pContext, hUserConnection, pExpectedType, ppObject);

   if ( ( nError == W_SUCCESS ) && ( *ppObject != null ) )
   {
      return W_SUCCESS;
   }

   nError = PHandleGetPropertyNumber( pContext, hUserConnection, &nPropertyNumber );
   if (  ( nError == W_SUCCESS )
      && ( nPropertyNumber != 0 ) )
   {
      return W_ERROR_CONNECTION_COMPATIBILITY;
   }
   else
   {
      return W_ERROR_BAD_HANDLE;
   }
}

/* See header file */
bool PReaderUserIsPropertyVisible(uint8_t nProperty)
{
   uint32_t i;

   for (i = 0; i < P_READER_INTERNAL_PROPERTY_NUMBER; i++)
   {
      if (nProperty == g_aPReaderUserInternalProperties[i])
      {
         return false;
      }
   }

   return true;
}

/* See API Specification */
uint8_t PReaderUserGetNbCardDetected(tContext * pContext)
{
   return (uint8_t)PReaderDriverGetNbCardDetected(pContext);
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */


