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

#define P_MODULE P_MODULE_DEC( NDEFA )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/*******************************************************************************

   Common function

*******************************************************************************/

/* Array listing every types of tags */
static const uint8_t g_aTagTypeArray[] = {
      W_PROP_NFC_TAG_TYPE_1,
      W_PROP_NFC_TAG_TYPE_2,
      W_PROP_NFC_TAG_TYPE_3,
      W_PROP_NFC_TAG_TYPE_4_A, W_PROP_NFC_TAG_TYPE_4_B,
      W_PROP_NFC_TAG_TYPE_5,
      W_PROP_NFC_TAG_TYPE_6};


static const uint8_t g_aTagAndChipTypeArray[] = {

      W_PROP_NFC_TAG_TYPE_1,
      W_PROP_TYPE1_CHIP,         /* we are able to format typer 1 chips */

      W_PROP_NFC_TAG_TYPE_2,
      W_PROP_MIFARE_UL,          /* we are able to format mifare UL chip */
      W_PROP_MY_D_MOVE,          /* we are able to format My-d move chip */
      W_PROP_MY_D_NFC,           /* we are able to format My-d NFC chip */

      W_PROP_NFC_TAG_TYPE_3,     /* no format possible for type 3 */


      W_PROP_NFC_TAG_TYPE_4_A,   /* no format possible for type 4 */
      W_PROP_NFC_TAG_TYPE_4_B,

      W_PROP_NFC_TAG_TYPE_5,
      W_PROP_PICOPASS_2K,        /* we are able to format PicoPass chips */
      W_PROP_PICOPASS_32K,

      W_PROP_NFC_TAG_TYPE_6,
      W_PROP_ISO_15693_3         /* we are able to format some ISO 15693 chips */
};

/*******************************************************************************

   PNDEFWriteMessageOnAnyTag()

*******************************************************************************/

/* Declare a write NDEF on any tag structure */
typedef struct __tNDEFWriteOnAnyTagOperation
{
   /* Registry handle */
   W_HANDLE  hRegistryHandle;

   /* Callback context */
   tDFCCallbackContext  sCallbackContext;

   /* Write operation handle */
   W_HANDLE  hWriteOnAnyTagOperation;

   /* NDEF connection */
   W_HANDLE hNDEFConnection;

   /* Command state */
   uint8_t   nCommandState;

   /* NDEF message to write */
   W_HANDLE   hMessage;

   /* write parameters */
   uint32_t nActionMask;


} tNDEFWriteOnAnyTagOperation;

/* Write on any tag status */
#define P_NDEF_WRITE_ANY_STATE_NOT_INIT            0
#define P_NDEF_WRITE_ANY_STATE_DETECTION_PENDING   1
#define P_NDEF_WRITE_ANY_STATE_WRITE_PENDING       2

/** See WReaderListenToCardDetection */
static void static_PNDEFWriteOnAnyTagWriteCompleted(
            tContext* pContext,
            void * pHandlerParameter,
            W_ERROR nError )
{
   tNDEFWriteOnAnyTagOperation* pWriteOnAnyTagOperation =
      (tNDEFWriteOnAnyTagOperation*)pHandlerParameter;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFWriteOnAnyTagWriteCompleted: receive %s",
         PUtilTraceError(nError));
   }

   if(pWriteOnAnyTagOperation->hRegistryHandle != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pWriteOnAnyTagOperation->hRegistryHandle);
   }

   if(pWriteOnAnyTagOperation->hNDEFConnection != W_NULL_HANDLE)
   {
      /* notify the completion of the operation */
      PReaderHandlerWorkPerformed(
         pContext,
         pWriteOnAnyTagOperation->hNDEFConnection,
         true, ( nError == W_SUCCESS )?true:false );
   }

   if(pWriteOnAnyTagOperation->hWriteOnAnyTagOperation != W_NULL_HANDLE)
   {
      PBasicSetOperationCompleted(pContext, pWriteOnAnyTagOperation->hWriteOnAnyTagOperation);
      PHandleClose(pContext, pWriteOnAnyTagOperation->hWriteOnAnyTagOperation);
   }

   if(pWriteOnAnyTagOperation->hMessage != W_NULL_HANDLE)
   {
      PHandleClose(pContext, pWriteOnAnyTagOperation->hMessage);
   }

   PDFCPostContext2(
      &pWriteOnAnyTagOperation->sCallbackContext,
      P_DFC_TYPE_NDEF,
      nError );

   CMemoryFree(pWriteOnAnyTagOperation);
}

/**
 *
 */

static void static_PNDEFWriteOnAnyTagRightsRetrieved(
   tContext * pContext,
   void     * pCallbackParameteter,
   W_ERROR    nError)
{
   tNDEFWriteOnAnyTagOperation* pWriteOnAnyTagOperation = pCallbackParameteter;
   W_HANDLE hNDEFOperation = W_NULL_HANDLE;

   if (nError == W_SUCCESS)
   {
      PNDEFWriteMessage(
         pContext,
         pWriteOnAnyTagOperation->hNDEFConnection,
         static_PNDEFWriteOnAnyTagWriteCompleted,
         pWriteOnAnyTagOperation,
         pWriteOnAnyTagOperation->hMessage,
         pWriteOnAnyTagOperation->nActionMask,
         &hNDEFOperation );

      if (hNDEFOperation != W_NULL_HANDLE)
      {
         if((nError = PBasicAddSubOperationAndClose(
            pContext, pWriteOnAnyTagOperation->hWriteOnAnyTagOperation, hNDEFOperation)) != W_SUCCESS)
         {
            PDebugError(
            "static_PNDEFWriteOnAnyTagDetectionHandler: error returned by PBasicAddSubOperationAndClose()");
            goto return_error;
         }
      }

      pWriteOnAnyTagOperation->nCommandState = P_NDEF_WRITE_ANY_STATE_WRITE_PENDING;
   }

   return;

return_error:

   PHandleClose(pContext, hNDEFOperation);

   static_PNDEFWriteOnAnyTagWriteCompleted(pContext, pWriteOnAnyTagOperation, nError);

}

/** See WReaderListenToCardDetection */
static void static_PNDEFWriteOnAnyTagDetectionHandler(
            tContext* pContext,
            void * pHandlerParameter,
            W_HANDLE hConnection,
            W_ERROR nError )
{
   tNDEFWriteOnAnyTagOperation* pWriteOnAnyTagOperation =
      (tNDEFWriteOnAnyTagOperation*)pHandlerParameter;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFWriteOnAnyTagDetectionHandler: receive %s",
         PUtilTraceError(nError));
      goto return_error;
   }

   if(PBasicGetOperationState(pContext, pWriteOnAnyTagOperation->hWriteOnAnyTagOperation)
      == P_OPERATION_STATE_CANCELLED)
   {
      PDebugError("static_PNDEFWriteOnAnyTagDetectionHandler: operation cancelled");
      nError = W_ERROR_CANCEL;
      goto return_error;
   }

   /* Stop the detection */
   CDebugAssert(pWriteOnAnyTagOperation->hRegistryHandle != W_NULL_HANDLE);
   PHandleClose(pContext, pWriteOnAnyTagOperation->hRegistryHandle);
   pWriteOnAnyTagOperation->hRegistryHandle = W_NULL_HANDLE;

   /* Store the connection */
   pWriteOnAnyTagOperation->hNDEFConnection = hConnection;

   /*
    * For Mifare UL-C, retreive the access rights prior to be able to write into them
    */

   if (PBasicCheckConnectionProperty(pContext, hConnection, W_PROP_MIFARE_UL_C) == W_SUCCESS)
   {
      PMifareULRetrieveAccessRights(pContext, hConnection, static_PNDEFWriteOnAnyTagRightsRetrieved, pWriteOnAnyTagOperation);
   }
   else
   {
      static_PNDEFWriteOnAnyTagRightsRetrieved(pContext, pWriteOnAnyTagOperation, W_SUCCESS);
   }

   return;

return_error:

   static_PNDEFWriteOnAnyTagWriteCompleted(pContext, pWriteOnAnyTagOperation, nError);
}

/** See tHandleCancelOperation */
static void static_PNDEFWriteOnAnyTagCancelOperation(
         tContext* pContext,
         void* pCancelParameter,
         bool bIsClosing)
{
   tNDEFWriteOnAnyTagOperation* pWriteOnAnyTagOperation =
      (tNDEFWriteOnAnyTagOperation*)pCancelParameter;

   if(pWriteOnAnyTagOperation->nCommandState == P_NDEF_WRITE_ANY_STATE_DETECTION_PENDING)
   {
      PHandleClose(pContext, pWriteOnAnyTagOperation->hRegistryHandle);
      pWriteOnAnyTagOperation->hRegistryHandle = W_NULL_HANDLE;

      /* Post the call to avoid concurrency issues */
      PDFCPost2( pContext, P_DFC_TYPE_NDEF, static_PNDEFWriteOnAnyTagWriteCompleted,
         pWriteOnAnyTagOperation, W_ERROR_CANCEL );
   }
}

/* See Client API Specifications */
void PNDEFWriteMessageOnAnyTag(
            tContext* pContext,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter,
            uint8_t nPriority,
            W_HANDLE hMessage,
            uint32_t nActionMask,
            W_HANDLE* phOperation )
{
   tNDEFWriteOnAnyTagOperation* pWriteOnAnyTagOperation = null;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError = W_SUCCESS;
   W_HANDLE hOperation = W_NULL_HANDLE;

   PDebugTrace("PNDEFWriteMessageOnAnyTag()");

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Create the NDEF structure */
   pWriteOnAnyTagOperation = (tNDEFWriteOnAnyTagOperation*)CMemoryAlloc( sizeof(tNDEFWriteOnAnyTagOperation) );

   if ( pWriteOnAnyTagOperation == null )
   {
      PDebugError("PNDEFWriteMessageOnAnyTag: cannot allocate the operation");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }
   CMemoryFill(pWriteOnAnyTagOperation, 0, sizeof(tNDEFWriteOnAnyTagOperation));

   hOperation = PBasicCreateOperation(
         pContext,
         static_PNDEFWriteOnAnyTagCancelOperation, pWriteOnAnyTagOperation);

   if(hOperation == W_NULL_HANDLE)
   {
      PDebugError("PNDEFWriteMessageOnAnyTag: Cannot allocate the operation");
      nError = W_ERROR_OUT_OF_RESOURCE;
      goto return_error;
   }

   /* Store the callback information */
   pWriteOnAnyTagOperation->sCallbackContext = sCallbackContext;

   /* Set the command state */
   pWriteOnAnyTagOperation->nCommandState = P_NDEF_WRITE_ANY_STATE_NOT_INIT;

   /* Check the parameters */
   if(( nPriority < W_PRIORITY_MINIMUM )
   || ( nPriority > W_PRIORITY_MAXIMUM ) )
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   if ((nActionMask & W_NDEF_ACTION_BITMASK) != nActionMask)
   {
      PDebugError("PNDEFWriteMessageOnAnyTag : invalid nActionMask value");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* save the operation parameters */
   pWriteOnAnyTagOperation->nActionMask = nActionMask;

   /* Duplicate the message handle */
   if(hMessage != W_NULL_HANDLE)
   {
      if ( ( nError = PHandleDuplicate(
                        pContext,
                        hMessage,
                        &pWriteOnAnyTagOperation->hMessage ) ) != W_SUCCESS )
      {
         goto return_error;
      }
   }

   if ((nActionMask & W_NDEF_ACTION_FORMAT_BITMASK) == 0)
   {
      /* no format is requested, only listen for tags */

      if ( ( nError = PReaderListenToCardDetection(
                     pContext,
                     static_PNDEFWriteOnAnyTagDetectionHandler, pWriteOnAnyTagOperation,
                     nPriority,
                     g_aTagTypeArray, sizeof(g_aTagTypeArray),
                     &pWriteOnAnyTagOperation->hRegistryHandle ) ) != W_SUCCESS )
      {
         goto return_error;
      }
   }
   else
   {
      /* a format acation is requested, listen on both tags and chips */

      if ( ( nError = PReaderListenToCardDetection(
                     pContext,
                     static_PNDEFWriteOnAnyTagDetectionHandler, pWriteOnAnyTagOperation,
                     nPriority,
                     g_aTagAndChipTypeArray, sizeof(g_aTagAndChipTypeArray),
                     &pWriteOnAnyTagOperation->hRegistryHandle ) ) != W_SUCCESS )
      {
         goto return_error;
      }
   }

   /* Get an operation handle if needed */
   if(phOperation != null)
   {
      *phOperation = hOperation;

      if((nError = PHandleDuplicate(
         pContext, hOperation, &pWriteOnAnyTagOperation->hWriteOnAnyTagOperation))
         != W_SUCCESS)
      {
         goto return_error;
      }
   }
   else
   {
      pWriteOnAnyTagOperation->hWriteOnAnyTagOperation = hOperation;
   }

   pWriteOnAnyTagOperation->nCommandState = P_NDEF_WRITE_ANY_STATE_DETECTION_PENDING;

   return;


return_error:

   PDebugError("PNDEFWriteMessageOnAnyTag: return %s", PUtilTraceError(nError));

   if(phOperation != null)
   {
      *phOperation = hOperation;
   }

   if(hOperation != W_NULL_HANDLE)
   {
      PBasicSetOperationCompleted(pContext, hOperation);
   }

   if(pWriteOnAnyTagOperation != null)
   {
      PHandleClose(pContext, pWriteOnAnyTagOperation->hMessage);
      PHandleClose(pContext, pWriteOnAnyTagOperation->hRegistryHandle);
      PHandleClose(pContext, pWriteOnAnyTagOperation->hWriteOnAnyTagOperation);
      CMemoryFree(pWriteOnAnyTagOperation);
   }

   PDFCPostContext2(
      &sCallbackContext,
      P_DFC_TYPE_NDEF,
      nError );

}

/*******************************************************************************

   PNDEFReadMessageOnAnyTag()

*******************************************************************************/

/* Declare a read NDEF on any tag structure */
typedef struct __tNDEFReadOnAnyTagRegistry
{
   /* Connection object registry */
   tHandleObjectHeader        sObjectHeader;

   /* NDEF operation handle */
   W_HANDLE                   hNDEFOperation;
   /* Connection handle */
   W_HANDLE                   hNDEFConnection;

   /* NDEF read information */
   uint8_t                    nTNF;
   tchar                      aTypeString[P_NDEF_MAX_STRING_LENGTH];

   /* Registry handle */
   W_HANDLE                   hRegistryHandle;

   /* Callback context */
   tDFCCallbackContext        sCallbackContext;

} tNDEFReadOnAnyTagRegistry;

/**
 * @brief   Assynchronous object destruction.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static void static_PNDEFReadOnAnyTagFreeRegistry(
            tContext* pContext,
            void* pObject )
{
   CMemoryFree(pObject);
}

/**
 * @brief   Destroyes a read on any tag registry object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pObject  The object to destroy.
 **/
static uint32_t static_PNDEFReadOnAnyTagDestroyRegistry(
            tContext* pContext,
            void* pObject )
{
   tNDEFReadOnAnyTagRegistry* pReadOnAnyTagRegistry
      = (tNDEFReadOnAnyTagRegistry*)pObject;

   PDebugTrace("static_PNDEFReadOnAnyTagDestroyRegistry");

   PHandleClose(pContext, pReadOnAnyTagRegistry->hRegistryHandle);
   pReadOnAnyTagRegistry->hRegistryHandle = W_NULL_HANDLE;

   CDebugAssert(pReadOnAnyTagRegistry->hNDEFOperation == W_NULL_HANDLE);
   CDebugAssert(pReadOnAnyTagRegistry->hNDEFConnection == W_NULL_HANDLE);

   /* Use a DFC to avoid a problem of concurency with static_PNDEFReadOnAnyTagDetectionHandler */
   PDFCPost1( pContext, P_DFC_TYPE_NDEF, static_PNDEFReadOnAnyTagFreeRegistry,
      pReadOnAnyTagRegistry );

   return P_HANDLE_DESTROY_DONE;
}

/* Read message on any tag registry type */
tHandleType g_sNDEFReadOnAnyTag = { static_PNDEFReadOnAnyTagDestroyRegistry,
                                    null, null, null, null };

#define P_HANDLE_TYPE_NDEF_READ_ON_ANY_TAG_REGISTRY (&g_sNDEFReadOnAnyTag)

/* See WNDEFReadMessage() */
static void static_PNDEFReadOnAnyTagReadMessageCompleted(
            tContext* pContext,
            void* pCallbackParameter,
            W_HANDLE hMessage,
            W_ERROR nError )
{
   tNDEFReadOnAnyTagRegistry* pReadOnAnyTagRegistry
      = (tNDEFReadOnAnyTagRegistry*)pCallbackParameter;

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFReadOnAnyTagReadMessageCompleted: receive %s",
         PUtilTraceError(nError));

      CDebugAssert(hMessage == W_NULL_HANDLE);
   }

   CDebugAssert(pReadOnAnyTagRegistry->hNDEFOperation != W_NULL_HANDLE);
   PHandleClose(pContext, pReadOnAnyTagRegistry->hNDEFOperation);
   pReadOnAnyTagRegistry->hNDEFOperation = W_NULL_HANDLE;

   /* Notify the reader registry */
   CDebugAssert(pReadOnAnyTagRegistry->hNDEFConnection != W_NULL_HANDLE);
   PReaderHandlerWorkPerformed(
      pContext,
      pReadOnAnyTagRegistry->hNDEFConnection,
      true, ( nError == W_SUCCESS )?true:false );
   pReadOnAnyTagRegistry->hNDEFConnection = W_NULL_HANDLE;

   if(nError != W_ERROR_CANCEL)
   {
      PDFCPostContext3(
         &pReadOnAnyTagRegistry->sCallbackContext, P_DFC_TYPE_NDEF, hMessage, nError );
   }

   /* Release the protection: may destroy the registry object */
   PHandleDecrementReferenceCount( pContext, pReadOnAnyTagRegistry );
}

/** See WReaderListenToCardDetection */
static void static_PNDEFReadOnAnyTagDetectionHandler(
            tContext* pContext,
            void* pHandlerParameter,
            W_HANDLE hConnection,
            W_ERROR nError )
{
   tNDEFReadOnAnyTagRegistry* pReadOnAnyTagRegistry;

   if (PHandleGetObject(pContext, (W_HANDLE) pHandlerParameter, P_HANDLE_TYPE_NDEF_READ_ON_ANY_TAG_REGISTRY, (void **) &pReadOnAnyTagRegistry) != W_SUCCESS)
   {
      PDebugWarning("static_PNDEFReadOnAnyTagDetectionHandler : hRegistry is no longer valid. Collision ?");
      PHandleClose(pContext, hConnection);
      return;
   }

   if(nError != W_SUCCESS)
   {
      PDebugError("static_PNDEFReadOnAnyTagDetectionHandler: receive %s",
         PUtilTraceError(nError));
   }

   if(pReadOnAnyTagRegistry->hRegistryHandle == W_NULL_HANDLE)
   {
      PDebugTrace("static_PNDEFReadOnAnyTagDetectionHandler: operation cancelled");
      PHandleClose(pContext, hConnection);
      return;
   }

   if(nError == W_SUCCESS)
   {
      CDebugAssert(pReadOnAnyTagRegistry->hNDEFConnection == W_NULL_HANDLE);
      CDebugAssert(pReadOnAnyTagRegistry->hNDEFOperation == W_NULL_HANDLE);

      pReadOnAnyTagRegistry->hNDEFConnection = hConnection;

      /* Read the message */
      PNDEFReadMessage(
         pContext,
         hConnection,
         static_PNDEFReadOnAnyTagReadMessageCompleted,
         pReadOnAnyTagRegistry,
         pReadOnAnyTagRegistry->nTNF,
         ( pReadOnAnyTagRegistry->nTNF != W_NDEF_TNF_ANY_TYPE )?pReadOnAnyTagRegistry->aTypeString:null,
         &pReadOnAnyTagRegistry->hNDEFOperation );

      /* Protect the registry against the destruction */
      PHandleIncrementReferenceCount(pReadOnAnyTagRegistry);
   }
}

/* See Client API Specifications */
void PNDEFReadMessageOnAnyTag(
            tContext* pContext,
            tPNDEFReadMessageCompleted* pCallback,
            void* pCallbackParameter,
            uint8_t nPriority,
            uint8_t nTNF,
            const tchar* pTypeString,
            W_HANDLE* phRegistry )
{
   tNDEFReadOnAnyTagRegistry* pReadOnAnyTagRegistry;
   tDFCCallbackContext sCallbackContext;
   W_ERROR nError = W_SUCCESS;
   W_HANDLE hRegistry = W_NULL_HANDLE;

   PDFCFillCallbackContext(
      pContext,
      (tDFCCallback*)pCallback,
      pCallbackParameter,
      &sCallbackContext );

   /* Create the NDEF structure */
   pReadOnAnyTagRegistry = (tNDEFReadOnAnyTagRegistry*)CMemoryAlloc( sizeof(tNDEFReadOnAnyTagRegistry) );
   if ( pReadOnAnyTagRegistry == null )
   {
      PDebugError("PNDEFReadMessageOnAnyTag: pNDEFOnAnyTagRegistry == null");
      /* Send the error */
      PDFCPostContext3(
         &sCallbackContext, P_DFC_TYPE_NDEF, W_NULL_HANDLE, W_ERROR_OUT_OF_RESOURCE );
      return;
   }
   CMemoryFill(pReadOnAnyTagRegistry, 0, sizeof(tNDEFReadOnAnyTagRegistry));

   if(phRegistry == null)
   {
      PDebugError("PNDEFReadMessageOnAnyTag: phRegistry is null");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* Store the callback information */
   pReadOnAnyTagRegistry->sCallbackContext = sCallbackContext;

   /* Check the parameters */
   if(( nPriority < W_PRIORITY_MINIMUM )
   || ( nPriority > W_PRIORITY_MAXIMUM ) )
   {
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* Check the read parameters */
   if ((nError = PNDEFCheckReadParameters(pContext, nTNF, pTypeString)) != W_SUCCESS)
   {
      PDebugError("PNDEFReadMessageOnAnyTag: PNDEFCheckReadParameters error");
      nError = W_ERROR_BAD_PARAMETER;
      goto return_error;
   }

   /* Copy the registry information */
   pReadOnAnyTagRegistry->nTNF = nTNF;
   if ( pTypeString != null )
   {
      CMemoryCopy(
         pReadOnAnyTagRegistry->aTypeString,
         pTypeString,
         PUtilStringLength(pTypeString) * 2 );
   }
   else
   {
      pReadOnAnyTagRegistry->aTypeString[0] = 0;
   }

   /* Create the registry handle */
   if ( ( nError = PHandleRegister(
                     pContext,
                     pReadOnAnyTagRegistry,
                     P_HANDLE_TYPE_NDEF_READ_ON_ANY_TAG_REGISTRY,
                     &hRegistry ) ) != W_SUCCESS )
   {
      PDebugError("PNDEFReadMessageOnAnyTag: could not create the registry handle");
      goto return_error;
   }

   /* Register the structure */
   if ( ( nError = PReaderListenToCardDetection(
               pContext,
               static_PNDEFReadOnAnyTagDetectionHandler, (void *) hRegistry,
               nPriority,
               g_aTagTypeArray, sizeof(g_aTagTypeArray),
               &pReadOnAnyTagRegistry->hRegistryHandle ) ) != W_SUCCESS )
   {
      PDebugError("PNDEFReadMessageOnAnyTag: PReaderListenToCardDetection() in error");
      goto return_error;
   }

   * phRegistry = hRegistry;

   return;

return_error:

   PDebugError("PNDEFReadMessageOnAnyTag: returning %s", PUtilTraceError(nError));

   if (hRegistry != W_NULL_HANDLE)
   {
      PHandleClose(pContext, hRegistry);
   }
   else
   {
      if(pReadOnAnyTagRegistry != null)
      {
         PHandleClose(pContext, pReadOnAnyTagRegistry->hRegistryHandle);
         CMemoryFree(pReadOnAnyTagRegistry);
      }
   }

   PDFCPostContext3(
      &sCallbackContext, P_DFC_TYPE_NDEF, W_NULL_HANDLE, nError );
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

