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

/*
 * This source file is an altered version of a file provided in the
 * implementation of the ZLIB library under the copyright notice printed below.
 */

/* zconf.h -- configuration of the zlib compression library
 * Copyright (C) 1995-2005 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */
#ifndef ZCONF_H
#define ZCONF_H

#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)


#if !defined(STDC) && (defined(__STDC__) || defined(__cplusplus))
#  define STDC
#endif

/* Maximum value for memLevel in deflateInit2 */
#ifndef MAX_MEM_LEVEL
#  ifdef MAXSEG_64K
#    define MAX_MEM_LEVEL 5 /* default is 8*/
#  else
#    define MAX_MEM_LEVEL 5 /* default is 9*/
#  endif
#endif

/* Maximum value for windowBits in deflateInit2 and inflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */
#ifndef MAX_WBITS
/* due to heap size limitation in NFC Device use only 4k windows size, default is 32K (15 bits)*/
#  define MAX_WBITS   12/* 4K LZ77 window */
#endif


#ifndef ZEXTERN
#  define ZEXTERN extern
#endif



#if !defined(__MACTYPES__)
typedef uint8_t  Byte;  /* 8 bits */
#endif

typedef uint32_t   uInt;  /* 16 bits or more */
typedef uint32_t  uLong; /* 32 bits or more */


typedef char   charf;
typedef sint32_t    intf;
typedef uInt   uIntf;
typedef uLong  uLongf;

#ifdef STDC
   typedef void const *voidpc;
   typedef void    *voidpf;
   typedef void       *voidp;
#else
   typedef Byte const *voidpc;
   typedef Byte    *voidpf;
   typedef Byte       *voidp;
#endif

#ifndef z_off_t
#  define z_off_t sint32_t
#endif

#define NO_DUMMY_DECL

#endif  /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* ZCONF_H */
