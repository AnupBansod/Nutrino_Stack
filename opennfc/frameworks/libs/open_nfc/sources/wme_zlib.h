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

#ifndef WME_ZLIB_H
#define WME_ZLIB_H



#if (P_BUILD_CONFIG == P_CONFIG_USER) || (P_BUILD_CONFIG == P_CONFIG_MONOLITHIC)

/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */
#define ZLIB_OK            0
#define ZLIB_STREAM_END    1
#define ZLIB_NEED_DICT     2
#define ZLIB_ERRNO        (-1)
#define ZLIB_STREAM_ERROR (-2)
#define ZLIB_DATA_ERROR   (-3)
#define ZLIB_MEM_ERROR    (-4)
#define ZLIB_BUF_ERROR    (-5)
#define ZLIB_VERSION_ERROR (-6)

/* compression levels */
#define ZLIB_NO_COMPRESSION         0
#define ZLIB_BEST_SPEED             1
#define ZLIB_BEST_COMPRESSION       9
#define ZLIB_DEFAULT_COMPRESSION  (-1)


/*
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least the value returned by
   compressBound(sourceLen). Upon exit, destLen is the actual size of the
   compressed buffer.

     compress2 returns ZLIB_OK if success, ZLIB_MEM_ERROR if there was not enough
   memory, ZLIB_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
sint32_t WZLIB_compress2(
         uint8_t* pDest,
         uint32_t* pnDestLen,
         const uint8_t* pSource,
         uint32_t nSourceLen,
         sint32_t nLevel);


sint32_t WZLIB_uncompress(
         void* pContext,
         uint8_t* pDest,
         uint32_t* pnDestLen,
         const uint8_t* pSource,
         uint32_t nSourceLen);
/*
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     uncompress returns ZLIB_OK if success, ZLIB_MEM_ERROR if there was not
   enough memory, ZLIB_BUF_ERROR if there was not enough room in the output
   buffer, or ZLIB_DATA_ERROR if the input data was corrupted or incomplete.
*/

#endif  /* P_CONFIG_USER || P_CONFIG_MONOLITHIC */

#endif /* WME_ZLIB_H */
