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

#ifndef __WME_SECURITY_MANAGER_H
#define __WME_SECURITY_MANAGER_H
#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/**
 * @brief Releases the security data linked to a user session.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pUserInstance  The user instance.
 **/
void PSecurityManagerDriverReleaseUserData(
   tContext * pContext,
   tUserInstance* pUserInstance);

/**
 * @brief  Checks the identity of user based on a principal value.
 *
 * @param[in]  pContext  The current context.
 *
 * @param[in]  pPrincipalBuffer  A pointer on the default value for the principal.
 *             This value is never null.
 *
 * @param[in]  nPrincipalBufferLength  The length in bytes of the \a pPrincipalBuffer buffer.
 *             This value is always positive.
 *
 * @return  W_SUCCESS if the application matches the principal value,
 *          W_ERROR_SECURITY otherwise.
 **/
W_ERROR PSecurityManagerDriverCheckIdentity(
   tContext * pContext,
   const uint8_t* pPrincipalBuffer,
   uint32_t nPrincipalBufferLength);

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */
#endif /* __WME_SECURITY_MANAGER_H */
