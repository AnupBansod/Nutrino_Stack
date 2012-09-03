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

/*******************************************************************************
   Contains the implementation of the security manager functions
*******************************************************************************/
#define P_MODULE  P_MODULE_DEC( SECMGT )

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* See C API Specification */
W_ERROR PSecurityAuthenticate(
         tContext * pContext,
         const uint8_t* pApplicationDataBuffer,
         uint32_t nApplicationDataBufferLength )
{
   PDebugTrace("PSecurityAuthenticate()");

   return PSecurityManagerDriverAuthenticate(
            pContext, pApplicationDataBuffer, nApplicationDataBufferLength );
}

#endif /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#if (P_BUILD_CONFIG == P_CONFIG_DRIVER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/** See WSecurityAuthenticate() */
W_ERROR PSecurityManagerDriverAuthenticate(
   tContext * pContext,
   const uint8_t* pApplicationDataBuffer,
   uint32_t nApplicationDataBufferLength )
{
   tUserInstance* pUserInstance;
   const tUserIdentity* pUserIdentity;

   PDebugTrace("PSecurityManagerDriverAuthenticate()");

   pUserInstance = PContextGetCurrentUserInstance(pContext);

   pUserIdentity = pUserInstance->pUserIdentity;

   if(pUserInstance->bAuthenticationDone != false)
   {
      PDebugError("PSecurityManagerDriverAuthenticate: The authentication is already done");
      return W_ERROR_BAD_STATE;
   }

   pUserInstance->bAuthenticationDone = true;
   pUserInstance->pUserAuthenticationData = null;

   if( CSecurityCreateAuthenticationData(
         pUserIdentity,
         pApplicationDataBuffer,
         nApplicationDataBufferLength,
         &pUserInstance->pUserAuthenticationData) == false)
   {
      PDebugError("PSecurityManagerDriverAuthenticate: Error returned by CSecurityCreateAuthenticationData()");
      pUserInstance->pUserAuthenticationData = null;
      return W_ERROR_SECURITY;
   }

   return W_SUCCESS;
}

/** See header file */
void PSecurityManagerDriverReleaseUserData(
   tContext * pContext,
   tUserInstance* pUserInstance)
{
   PDebugTrace("PSecurityManagerDriverReleaseUserData()");

   pUserInstance->bAuthenticationDone = false;
   if(pUserInstance->pUserAuthenticationData != null)
   {
      CSecurityDestroyAuthenticationData(pUserInstance->pUserAuthenticationData);
      pUserInstance->pUserAuthenticationData = null;
   }
}

/** See header file */
W_ERROR PSecurityManagerDriverCheckIdentity(
   tContext * pContext,
   const uint8_t* pPrincipalBuffer,
   uint32_t nPrincipalBufferLength)
{
   tUserInstance* pUserInstance;

   PDebugTrace("PSecurityManagerDriverCheckIdentity()");

   pUserInstance = PContextGetCurrentUserInstance(pContext);

   if(pUserInstance->bAuthenticationDone == false)
   {
      PDebugError("PSecurityManagerDriverCheckIdentity: The authentication is not performed");
      return W_ERROR_SECURITY;
   }

   if(CSecurityCheckIdentity(
            pUserInstance->pUserIdentity,
            pUserInstance->pUserAuthenticationData,
            pPrincipalBuffer, nPrincipalBufferLength) == false)
   {
      PDebugError("PSecurityManagerDriverReleaseUserData: Error returned by CSecurityCheckIdentity()");
      return W_ERROR_SECURITY;
   }

   return W_SUCCESS;
}

#endif /* P_CONFIG_DRIVER || P_CONFIG_MONOLITHIC */
