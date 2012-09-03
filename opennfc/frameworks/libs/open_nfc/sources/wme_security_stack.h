/*
 * Copyright (c) 2011 Inside Secure, All Rights Reserved.
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

#ifndef __WME_SECURITY_STACK_H
#define __WME_SECURITY_STACK_H
#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

typedef struct __tSecurityStackInstance tSecurityStackInstance;

/**
 * @brief   Creates a security stack instance.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  nIdentifier  The Secure Element identifier; This information
 *             is used for the debug traces.
 *
 * @param[in]  pDefaultPrincipal  A pointer on the default value for the principal.
 *             This value may be null if no default value is defined.
 *
 * @param[in]  nDefaultPrincipalLength  The length in bytes of the pDefaultPrincipal buffer.
 *             This value is zero if pDefaultPrincipal is null.
 *
 * @return   The security stack instance or null in case of error.
 **/
tSecurityStackInstance* PSecurityStackCreateInstance(
            tContext* pContext,
            uint32_t nIdentifier,
            const uint8_t* pDefaultPrincipal,
            uint32_t nDefaultPrincipalLength );

/**
 * @brief   Destroyes a security stack instance.
 *
 * @post  PSecurityStackDestroyInstance() does not return any error.
 *        The caller should always assume that the security stack instance
 *        is destroyed after this call.
 *
 * @post  The caller should never re-use the security stack instance value.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance to destroy.
 *             If nuul, the function does nothing and return.
 **/
void PSecurityStackDestroyInstance(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance );


/**
 * Filters an APDU value before sending it to the Secure Element.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
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
W_ERROR PSecurityStackFilterApdu(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            const uint8_t* pReaderToCardBuffer,
            uint32_t nReaderToCardBufferLength );

/**
 * Notifies the response APDU of the Secure Element.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
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
W_ERROR PSecurityStackNotifyResponseApdu(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            const uint8_t* pCardToReaderBuffer,
            uint32_t nCardToReaderBufferLength,
            W_ERROR nError );

/**
 * @brief Requests the Security Stack to load the Access Control File from the Secure Element.
 *
 * This function is called once during the initialization of the stack.
 *
 * The callback function is always called whether the operation is completed or if an error occurred.
 *
 * If the security stack does no find the PKCS#15 applet, the function returns W_SUCCESS.
 * Then the security control is deactivated.
 *
 * If any error is returned, the Security Stack will lock every access to the Secure Element,
 * except for the application(s) matching the default principal.
 *
 * PSecurityStackLoadAcf() does not return any error.
 * The following error codes are returned in the callback function:
 *   - W_SUCCESS  The operation completed successfully.
 *   - W_ERROR_BAD_STATE An operation is already pending on this connection.
 *   - W_ERROR_CONNECTION_COMPATIBILITY  The card is not compliant with ISO 7816 part 4.
 *   - W_ERROR_SECURITY  The version of the PKCS#15 applet is not compliant with the stack.
 *   - W_ERROR_RF_COMMUNICATION An error is detected in the protocol used by the card.
 *   - W_ERROR_TIMEOUT  A timeout occurred during the communication with the card.
 *   - other if any other error occurred.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
 *
 * @param[in]  pExchangeApduFunction  The exchange APDU function.
 *
 * @param[in]  hConnection  The secure element connection.
 *
 * @param[in]  pCallback  A pointer on the callback function.
 *
 * @param[in]  pCallbackParameter  A blind parameter transmitted to the callback function.
 **/
void PSecurityStackLoadAcf(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            tPBasicExchangeData* pExchangeApduFunction,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter );

/**
 * @brief Notifies the Security Stack that the connection with the Secure Element is closing.
 *
 * The callback function is always called whether the operation is completed or if an error occurred.
 *
 * PSecurityStackNotifyEndofConnection() does not return any error.
 * The following error codes are returned in the callback function:
 *   - W_SUCCESS  The operation completed successfully.
 *   - W_ERROR_BAD_STATE An operation is already pending on this connection.
 *   - W_ERROR_CONNECTION_COMPATIBILITY  The card is not compliant with ISO 7816 part 4,
 *     the PKCS#15 applet is not found or the version of the PKCS#15 applet is not compliant with the stack.
 *   - W_ERROR_RF_COMMUNICATION An error is detected in the protocol used by the card.
 *   - W_ERROR_TIMEOUT  A timeout occurred during the communication with the card.
 *   - other if any other error occurred.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pSecurityStackInstance  The security stack instance.
 *
 * @param[in]  pExchangeApduFunction  The exchange APDU function.
 *
 * @param[in]  hConnection  The secure element connection.
 *
 * @param[in]  pCallback  A pointer on the callback function.
 *
 * @param[in]  pCallbackParameter  A blind parameter transmitted to the callback function.
 **/
void PSecurityStackNotifyEndofConnection(
            tContext* pContext,
            tSecurityStackInstance* pSecurityStackInstance,
            tPBasicExchangeData* pExchangeApduFunction,
            W_HANDLE hConnection,
            tPBasicGenericCallbackFunction* pCallback,
            void* pCallbackParameter );

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */
#endif /* __WME_SECURITY_STACK_H */
