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
   Contains the declaration of some utility functions
*******************************************************************************/

#ifndef __WME_UTIL_H
#define __WME_UTIL_H

/**
 * @brief  Converts a pointer value into a 32-bit integer value.
 *
 * The pointer value should have been computed with PUtilConvertUint32ToPointer().
 *
 * @param[in]   pPointer  The pointer value to convert.
 *
 * @return  The 32-bit integer value.
 **/
#define PUtilConvertPointerToUint32( pPointer ) \
         ((uint32_t)(((uint8_t*)(pPointer)) - ((uint8_t*)0)))

/**
 * @brief  Converts a 32-bit integer value into a pointer value.
 *
 * @param[in]   nValue  The 32-bit integer value to convert.
 *
 * @return  The pointer value.
 **/
#define PUtilConvertUint32ToPointer( nValue ) \
         ((void*)(((uint8_t*)0) + (nValue)))

/**
 * @brief  Converts a pointer value into a handle value.
 *
 * The pointer value should have been computed with PUtilConvertHandleToPointer().
 *
 * @param[in]   pPointer  The pointer value to convert.
 *
 * @return  The handle value.
 **/
#define PUtilConvertPointerToHandle( pPointer ) \
         ((W_HANDLE)(((uint8_t*)(pPointer)) - ((uint8_t*)0)))

/**
 * @brief  Converts a handle value into a pointer value.
 *
 * @param[in]   hValue  The handle value to convert.
 *
 * @return  The pointer value.
 **/
#define PUtilConvertHandleToPointer( hValue ) \
         ((void*)(((uint8_t*)0) + (hValue)))

/**
 * @brief  Writes a value into a Big Endian buffer.
 *
 * @param[out]  pBuffer  The buffer of at least 2 bytes receiving the value.
 *
 * @param[in]   nValue  The value to store.
 **/
void PUtilWriteUint16ToBigEndianBuffer(
         uint16_t nValue,
         uint8_t* pBuffer);

/**
 * @brief  Reads a value from a Big Endian buffer.
 *
 * @param[out]  pBuffer  The buffer of at least 2 bytes containing the value.
 *
 * @return  The corresponding value.
 **/
uint16_t PUtilReadUint16FromBigEndianBuffer(
         const uint8_t* pBuffer);

/**
 * @brief  Writes a value into a Big Endian buffer.
 *
 * @param[out]  pBuffer  The buffer of at least 4 bytes receiving the value.
 *
 * @param[in]   nValue  The value to store.
 **/
void PUtilWriteUint32ToBigEndianBuffer(
         uint32_t nValue,
         uint8_t* pBuffer);

/**
 * @brief  Reads a value from a Big Endian buffer.
 *
 * @param[out]  pBuffer  The buffer of at least 4 bytes containing the value.
 *
 * @return  The corresponding value.
 **/
uint32_t PUtilReadUint32FromBigEndianBuffer(
         const uint8_t* pBuffer);


/**
 * @brief  Reads a value from a Little Endian buffer.
 *
 * @param[out]  pBuffer  The buffer of at least 2 bytes containing the value.
 *
 * @return  The corresponding value.
 **/
uint32_t PUtilReadUint32FromLittleEndianBuffer(
         const uint8_t* pBuffer);

/**
 * Converts a Utf-16 buffer (Little Endian) into a Utf-8 buffer.
 *
 * If pDestUtf8 = null, the length in bytes of the Utf-8 string is returned.
 *
 * @param[out] pDestUtf8  The buffer receiving the Utf-8 string.
 *
 * @param[in]  pSourceUtf16  The buffer containing the Utf-16 buffer.
 *
 * @param[in]  nSourceLength  The length in character of the Utf-16 buffer.
 *
 * @return  The length in bytes of the Utf-8 string.
 **/
uint32_t PUtilConvertUTF16ToUTF8(
                  uint8_t* pDestUtf8,
                  const tchar* pSourceUtf16,
                  uint32_t nSourceCharLength );

/**
 * Converts a Utf-8 buffer into a Utf-16 string (Little Endian).
 *

 *
 * @param[out] pDestUtf16  The buffer receiving the Utf-16 string.
 *
 * @param[in]  pSourceUtf8  The buffer containing the Utf-8 buffer.
 *
 * @param[in]  nSourceLength  The length in bytes of the Utf-8 buffer.
 *
 * @return  The length in characters of the Utf-16 string.
 *          0 if an error is detected in the Utf-8 format.
 **/
uint32_t PUtilConvertUTF8ToUTF16(
                  tchar* pDestUtf16,
                  const uint8_t* pSourceUtf8,
                  uint32_t nSourceLength );


/**
 * Converts a Utf-16 buffer using host endianess into a Utf-16 Big endian string
 *
 * @param[out] pDestUtf16  The buffer receiving the Utf-16 BE string.
 *
 * @param[in]  pSourceUtf16  The buffer containing the Utf-16 native buffer.
 *
 * @param[in]  nSourceLength  The length in bytes of the Utf-16 native buffer.
 *
 **/
void PUtilConvertUTF16ToUTF16BE(
                  tchar* pDestUtf16,
                  const tchar* pSourceUtf16,
                  uint32_t nSourceLength );

/**
 * Converts a Utf-16 Big endian buffer using a Utf-16 using host endianess string
 *
 * @param[out] pDestUtf16  The buffer receiving the Utf-16 native string.
 *
 * @param[in]  pSourceUtf16  The buffer containing the Utf-16 BE buffer.
 *
 * @param[in]  nSourceLength  The length in bytes of the Utf-16 BE buffer.
 *
 **/

void PUtilConvertUTF16BEToUTF16(
                  tchar* pDestUtf16,
                  const tchar* pSourceUtf16,
                  uint32_t nSourceLength );


/**
 * Writes the hexadimal string representation of a byte.
 *
 * @param[out] pStringBuffer  The buffer where to write the string.
 *
 * @param[in]  nValue  The value to write.
 *
 * @return The length in characters of the string.
 **/
uint32_t PUtilWriteHexaUint8(
               tchar* pStringBuffer,
               uint8_t nValue);

/**
 * Writes the hexadimal string representation of a 32 bit integer.
 *
 * @param[out] pStringBuffer  The buffer where to write the string.
 *
 * @param[in]  nValue  The value to write.
 *
 * @return The length in characters of the string.
 **/
uint32_t PUtilWriteHexaUint32(
               tchar* pStringBuffer,
               uint32_t nValue);

/**
 * Writes the decimal string representation of a 32 bit integer.
 *
 * @param[out] pStringBuffer  The buffer where to write the string.
 *
 * @param[in]  nValue  The value to write.
 *
 * @return The length in characters of the string.
 **/
uint32_t PUtilWriteDecimalUint32(
               tchar* pStringBuffer,
               uint32_t nValue);

/**
 * @brief  Returns the length of a string.
 *
 * @pre  The string \a pString should be a valid zero-ended string.
 *
 * @param[in]  pString  The zero-ended string to check.
 *
 * @return  The length in characters of the string.
 **/
uint32_t PUtilStringLength(
               const tchar* pString );

/**
 * @brief  Compares two strings.
 *
 * @pre  The strings \a pString1 and \a pString2 should be a valid zero-ended string.
 *
 * @param[in]  pString1  The first zero-ended string to compare.
 *
 * @param[in]  pString2  The second zero-ended string to compare.
 *
 * @return  One of the following values:
 *            - 0 if \a pString1 is equal to \a pString2
 *            - < 0 if \a pString1 is inferior to \a pString2
 *            - > 0 if \a pString1 is superior to \a pString2
 **/
sint32_t PUtilStringCompare(
               const tchar* pString1,
               const tchar* pString2 );


/**
 * @brief  Compares the first elements of two strings.
 *
 * @pre  The strings \a pString1 and \a pString2 should be a valid zero-ended string.
 *
 * @param[in]  pString1  The first zero-ended string to compare.
 *
 * @param[in]  pString2  The second zero-ended string to compare.
 *
 * @param[in]  nLength   The number of characters to compare.

 * @return  One of the following values:
 *            - 0 if \a pString1 is equal to \a pString2
 *            - < 0 if \a pString1 is inferior to \a pString2
 *            - > 0 if \a pString1 is superior to \a pString2
 **/
sint32_t PUtilStringNCompare(
               const tchar * pString1,
               const tchar * pString2,
               uint32_t      nLength);


/**
 * @brief  Compares two strings, one in Unicode, the other in ASCII.
 *
 * @pre  The strings \a pASCIIString should be a valid zero-ended string.
 *
 * @param[in]  pUnicodeString  The Unicode string to compare.
 *
 * @param[in]  nUnicodeLength  The length in character of the Unicode string.
 *
 * @param[in]  pASCIIString  The zero-ended ASCII string to compare.
 *
 * @return  One of the following values:
 *            - 0 if \a pUnicodeString is equal to \a pASCIIString
 *            - < 0 if \a pUnicodeString is inferior to \a pASCIIString
 *            - > 0 if \a pUnicodeString is superior to \a pASCIIString
 **/
sint32_t PUtilMixedStringCompare(
               const tchar* pUnicodeString,
               uint32_t nUnicodeLength,
               const char* pASCIIString );

/**
 * @brief  Copies a string at the end of another string.
 *
 * @param[in]  pBuffer  The buffer receiving the string.
 *
 * @param[inout] nPos  The initial zero based position in bytes where to copy the string.
 *               This value is updated with the new position of the zero at the end of the string.
 *
 * @param[in]  pString  The zero-ended string to copy.
 *
 * @return  The pointer on the new position of the zero at the end of the string.
 */
tchar* PUtilStringCopy(
               tchar* pBuffer,
               uint32_t* pPos,
               const tchar* pString);

#ifdef P_TRACE_ACTIVE

/**
 * Writes the hexadecimal value of a byte.
 *
 * @param[in]  pTraceBuffer  A pointer on the buffer where to write the log.
 *
 * @param[in]  nValue  The byte value.
 *
 * @return The number of characters added to the buffer.
 **/
uint32_t PUtilLogUint8(
         char* pTraceBuffer,
         uint32_t nValue);

/**
 * Writes the hexadecimal value of a uint 16.
 *
 * @param[in]  pTraceBuffer  A pointer on the buffer where to write the log.
 *
 * @param[in]  nValue  The value.
 *
 * @return The number of characters added to the buffer.
 **/
uint32_t PUtilLogUint16(
                  char* pTraceBuffer,
                  uint32_t nValue);

/**
 * Writes the hexadecimal value of a uint 32.
 *
 * @param[in]  pTraceBuffer  A pointer on the buffer where to write the log.
 *
 * @param[in]  nValue  The value.
 *
 * @return The number of characters added to the buffer.
 **/
uint32_t PUtilLogUint32(
                  char* pTraceBuffer,
                  uint32_t nValue);

/**
 * Writes the content of a byte array.
 *
 * @param[in]  pTraceBuffer  A pointer on the buffer where to write the log.
 *
 * @param[in]  pBuffer  The pointer on the buffer to dump.
 *
 * @param[in]  nLength  The length in bytes of the buffer to dump.
 *
 * @return The number of characters added to the buffer.
 **/
uint32_t PUtilLogArray(
         char* pTraceBuffer,
         uint8_t* pBuffer,
         uint32_t nLength);

/**
 * Copies a string at the end of another string.
 *
 * @param[in]  pBuffer  The buffer receiving the string.
 *
 * @param[inout] nPos  The initial zero based position in bytes where to copy the string.
 *               This value is updated with the new position of the end of the string.
 *
 * @param[in]  pString  The zero-ended ASCII string to copy.
 */
void PUtilTraceASCIIStringCopy(
         char* pBuffer,
         uint32_t* pPos,
         const char* pString);

#endif /* #ifdef P_TRACE_ACTIVE */

/**
 * Returns the string corresponding to a boolean value.
 *
 * @param[in]  bValue  The boolean value.
 *
 * @return The string "true" or "false".
 **/
const char* PUtilTraceBool(
         bool bValue);

/**
 * Returns the error string corresponding to an error code.
 *
 * @param[in]  nError  The error code.
 *
 * @return The error string.
 **/
const char* PUtilTraceError(
         W_ERROR nError);

/**
 * Returns the error string corresponding to a priortiy code.
 *
 * @param[in]  nPriority  The priority code.
 *
 * @return The priority string.
 **/
const char* PUtilTracePriority(
         W_ERROR nPriority);

/**
 * Returns the string corresponding to a card protocol.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  nProtocol  The protocol code.
 *
 * @return The protocol string.
 **/
const char* PUtilTraceCardProtocol(
         tContext* pContext,
         uint32_t nProtocol);

/**
 * Returns the string corresponding to a reader protocol.
 *
 * @param[in]  pContext  The context.
 *
 * @param[in]  nProtocol  The protocol code.
 *
 * @return The protocol string.
 **/
const char* PUtilTraceReaderProtocol(
         tContext* pContext,
         uint32_t nProtocol);

/**
 * Returns the protocol string corresponding to a protocol code.
 *
 * @param[in]  nProtocol  The protocol code.
 *
 * @return The protocol string.
 **/
const char* PUtilTraceConnectionProperty(
         uint8_t nProtocol);

#endif /* #ifdef __WME_UTIL_H */
