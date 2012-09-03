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

/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2005 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

#ifndef ZUTIL_H
#define ZUTIL_H


#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

#define ZLIB_INTERNAL
#include "wme_zlib_zlib.h"

#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

typedef uint8_t  uch;
typedef uch  uchf;
typedef uint16_t ush;
typedef ush  ushf;
typedef uint32_t  ulg;

extern const char * const WZLIB_z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) WZLIB_z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = (char*)ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

        /* common constants */

#ifndef DEF_WBITS
#  define DEF_WBITS MAX_WBITS
#endif
/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

        /* target dependencies */

/* functions */
/*extern void zmemcpy  (void* dest, const void* source, uInt len); */
#define zmemcpy CMemoryCopy

/* extern sint32_t  zmemcmp  (const void* s1, const void* s2, uInt len); */
#define zmemcmp CMemoryCompare

/* extern void zmemzero (void* dest, uInt len); */
#define zmemzero(dest, len) \
            CMemoryFill((dest), 0, (len))

void* WZLIB_zlib_alloc(void* pContext, uInt nSize);

void WZLIB_zlib_free(void* pContext, void *pBuffer);

#define zcalloc (alloc_func)WZLIB_zlib_alloc
#define zcfree (free_func)WZLIB_zlib_free

#define ZALLOC(strm, items, size) \
           (strm)->zalloc((strm)->opaque, (items)* (size))
#define ZFREE(strm, addr) \
           (strm)->zfree((strm)->opaque, addr)
#define TRY_FREE(s, p) ZFREE(s, p)


#if 0
#  include <stdio.h>
   extern sint32_t z_verbose;
   extern void z_error    OF((char *m));
#  define Assert(cond,msg) {if(!(cond)) z_error(msg);}
#  define Trace(x) {if (z_verbose>=0) fprintf x ;}
#  define Tracev(x) {if (z_verbose>0) fprintf x ;}
#  define Tracevv(x) {if (z_verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (z_verbose>0 && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (z_verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

#endif  /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* ZUTIL_H */
