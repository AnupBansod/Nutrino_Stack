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

#ifndef __WME_SECURITY_STACK_ACL_MANAGER_H
#define __WME_SECURITY_STACK_ACL_MANAGER_H
#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* AID minimum and maximum lengths */
#define P_SECSTACK_AID_MIN_LENGTH 5
#define P_SECSTACK_AID_MAX_LENGTH 16

typedef struct __tSecurityStackACL tSecurityStackACL;

/**
 * @brief Parses the ACL stored in the EF(SE-ACF) file.
 *
 * @param[in]  pBuffer  A pointer to the buffer storing the contents EF(SE-ACF) file
 *
 * @param[in]  nBuffer  The length in bytes of the EF(SE-ACF) file
 *
 * @param[out] ppACL    A pointer to the returned pointer to the parsed ACL
 *
 * @return One of the following error codes:
 *          - W_SUCCESS  The ACL has been successfully parsed
 *          - W_ERROR_OUT_OF_RESOURCE  Out of memory
 *          - W_ERROR_BAD_PARAMETER  The EF(SE-ACF) file is malformed.
 */
W_ERROR PSecurityStackParseACL
(
   const uint8_t* pBuffer, /* EF(SE-ACF) file */
   uint32_t nBuffer,
   tSecurityStackACL** ppACL
);

/**
 * @brief Checks an AID and an APDU against the ACL
 *
 * @param[in]  pContext  The current context
 *
 * @param[in]  pACL  The ACL
 *
 * @param[in]  pAID  The AID of the current or being selected application in the SE
 *
 * @param[in]  nAID  The length in bytes of the AID
 *
 * @param[in]  pAPDUHeader  A pointer to the first four bytes of the ADPU to be checked.
 *             May be null in case only the AID needs to be checked.
 *
 * @return One of the following error codes:
 *          - W_SUCCESS  The access if granted
 *          - W_ERROR_BAD_PARAMETER  A passed parameter is invalid
 *          - W_ERROR_SECURITY  The access is denied
 */
W_ERROR PSecurityStackCheckACL(
   tContext* pContext,
   tSecurityStackACL* pACL,
   const uint8_t* pAID,
   uint32_t nAID,
   const uint8_t* pAPDUHeader);

/**
 * @brief Frees an ACL
 *
 * @param[in]  pACL  The ACL
 *
 * @return One of the following error codes:
 *          - W_SUCCESS  The ACL has been freed
 *          - W_ERROR_BAD_PARAMETER  The pACL parameter is null
 */
W_ERROR PSecurityStackFreeACL
(
   tSecurityStackACL* pACL
);

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */
#endif /* __WME_SECURITY_STACK_ACL_MANAGER_H */
