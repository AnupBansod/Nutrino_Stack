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

/* uncompr.c -- decompress a memory buffer
 * Copyright (C) 1995-2003 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */
#include "wme_context.h"

#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)


#define ZLIB_INTERNAL
#include "wme_zlib_zlib.h"

void* WZLIB_zlib_alloc(void* pContext, uInt nSize);

void WZLIB_zlib_free(void* pContext, void *pBuffer);

/* ===========================================================================
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted.
*/
sint32_t  WZLIB_uncompress (void* pContext, uint8_t *dest, uint32_t *destLen, const uint8_t *source, uint32_t sourceLen)
{
    z_stream stream;
    sint32_t err;

    stream.next_in = (Byte*)source;
    stream.avail_in = (uInt)sourceLen;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.opaque = pContext;
    if(pContext == null)
    {
      stream.zalloc = (alloc_func)&WZLIB_zlib_alloc;
      stream.zfree = (free_func)&WZLIB_zlib_free;
    }
    else
    {
      stream.zalloc = (alloc_func)&PTestAlloc;
      stream.zfree = (free_func)&PTestFree;
    }

    err = inflateInit(&stream);
    if (err != Z_OK) return err;

    err = WZLIB_inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        WZLIB_inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    err = WZLIB_inflateEnd(&stream);
    return err;
}

#endif  /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */
