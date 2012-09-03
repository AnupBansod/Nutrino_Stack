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

  This header files contains the compilation configuration of the library.

*******************************************************************************/

#ifndef __PORTING_CONFIG_H
#define __PORTING_CONFIG_H

/* Export the Client API */
#define OPEN_NFC_CLIENT_API /* empty: see library header of llibopen_nfc.so */

/*******************************************************************************
   Porting Type (See the Porting Guide)
*******************************************************************************/

#define P_BUILD_CONFIG   P_CONFIG_USER
#define P_CONFIG_CLIENT_SERVER

/*******************************************************************************
   Traces and Debug (See the Porting Guide)
*******************************************************************************/

/* Define the Debug active flag and the trace active flag */

#ifdef _DEBUG
#	define P_TRACE_ACTIVE
#	define P_DEBUG_ACTIVE
#endif

#define P_TRACE_LEVEL_DEFAULT   P_TRACE_ERROR


/*******************************************************************************
   Test Engine Configuration (See the Porting Guide)
*******************************************************************************/

/* List of processor types used for the test bundles
 *    - 0x01 for ix86 processor family
 *    - 0x02 for any processor compatible with ARM7 Thumb little-endian
 *    - 0x03 for any processor compatible with ARM7 ARM little-endian
 *    - 0x04 for any processor compatible with MIPS32 little-endian
 */
#ifdef i386
#  define P_TEST_PROCESSOR       0x01
#endif
#ifdef __arm__
#  ifdef __thumb__
#     define P_TEST_PROCESSOR       0x02
#  else
#     define P_TEST_PROCESSOR       0x03
#  endif
#endif
#ifdef __mips__
#  define P_TEST_PROCESSOR       0x04
#endif

#define P_TEST_PRODUCT_NAME            "Linux Porting"


/*******************************************************************************
   Helper functions if the Open NFC J-Edition is installed on top of Open NFC Core Edition
*******************************************************************************/

/* Uncomment the line below to allow use of Open NFC J-Edition */
/* #define P_INCLUDE_J_EDITION_HELPER */

#define P_INCLUDE_JAVA_API

#endif /* __PORTING_CONFIG_H */