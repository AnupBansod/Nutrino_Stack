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

#ifndef __WME_READER_REGISTRY_H
#define __WME_READER_REGISTRY_H

/*******************************************************************************
  Contains the declaration of the reader registry implementation
*******************************************************************************/

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/**
 * Returns the pointer on the connection object corresponding to a handle value.
 *
 * This function should be called to check the validity of a handle value and
 * to obtain the pointer on the object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  hUserConnection  The user connection handle.
 *
 * @param[in]  pExpectedType  The expected object type.
 *
 * @param[out] ppObject  A pointer on a variable valued with the pointer on the
 *             corresponding object is the handle is valid and the object is of
 *             the expected type. Otherwise, the value is set to null.
 *
 * @return     One of the following value:
 *              - W_SUCCESS The handle is valid and of the specified type.
 *              - W_ERROR_BAD_HANDLE  The handle is not valid, the handle is
 *                W_NULL_HANDLE or the object is not of the specified type and
 *                is not a connection.
 *              - W_ERROR_CONNECTION_COMPATIBILITY The handle is not of the
 *                specified type but is a connection.
 **/
W_ERROR PReaderUserGetConnectionObject(
            tContext* pContext,
            W_HANDLE hUserConnection,
            tHandleType* pExpectedType,
            void** ppObject);

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
         W_HANDLE *phEventRegistry );

/**
 * @brief Converts a pointer into a connection object.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  pPointer  The pointer to convert. This value should be created
 *             by PUtilConvertHandleToPointer() with a connection handle.
 *
 * @param[in]  pExpectedType  The expected type.
 *
 * @param[in]  phUserConnection  A pointer on a variable valued with
 *             the connection handle.
 *
 * @return  The connection structure.
 **/
void* PReaderUserConvertPointerToConnection(
                  tContext* pContext,
                  void* pPointer,
                  tHandleType* pExpectedType,
                  W_HANDLE* phUserConnection);

/**
 * @brief Returns a boolean that indicates if a property is visible
 *        from API (WBasicXXX functions) or if it is only internal.
 *
 * @param[in]   nProperty  The identifier of the property to check.
 *
 * @return  true if nProperty is visible or false if nProperty is only internal.
 **/
bool PReaderUserIsPropertyVisible(uint8_t nProperty);

/**
 * @brief Returns a the number of cards detected in the field
 *
 * @param[in]  pContext  The context.
 *
 * @return  [0-255[ the number of cards detected
 **/
uint8_t PReaderUserGetNbCardDetected(tContext * pContext);

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* __WME_READER_REGISTRY_H */
