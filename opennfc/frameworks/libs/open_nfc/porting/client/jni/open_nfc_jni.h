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

#ifndef __OPEN_NFC_JNI_H
#define __OPEN_NFC_JNI_H

/**
 * Returns the first element on a an array of ints.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @return  The first element. zero if the array is null or if there is no element.
 **/
uint32_t WJavaGetIntArrayFirstElement(
            JNIEnv *env,
            jintArray arrayInstance);

/**
 * Sets the first element on a an array of ints.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @param[in]  value  The value to set.
 **/
void WJavaSetIntArrayFirstElement(
            JNIEnv *env,
            jintArray arrayInstance,
            uint32_t value);

/**
 * Returns the pointer on a an array of ints.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @return  A pointer on the array instance. null if the array is null.
 **/
jint* WJavaGetIntArrayElements(
            JNIEnv *env,
            jintArray arrayInstance);

/**
 * Returns the pointer on a an array of chars.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @return  A pointer on the array instance. null if the array is null.
 **/
jchar* WJavaGetCharArrayElements(
            JNIEnv *env,
            jcharArray arrayInstance);

/**
 * Returns the pointer on a an array of bytes.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @return  A pointer on the array instance. null if the array is null.
 **/
jbyte* WJavaGetByteArrayElements(
            JNIEnv *env,
            jbyteArray arrayInstance);

/**
 * Releases the pointer on a an array of ints.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @param[in]  pArrayContent  The array content. May be null.
 *
 * @param[in]  bCopy  true to copy back the content of the array.
 **/
void WJavaReleaseIntArrayElements(
            JNIEnv *env,
            jintArray arrayInstance,
            jint* pArrayContent,
            bool bCopy);

/**
 * Releases the pointer on a an array of chars.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @param[in]  pArrayContent  The array content. May be null.
 *
 * @param[in]  bCopy  true to copy back the content of the array.
 **/
void WJavaReleaseCharArrayElements(
            JNIEnv *env,
            jcharArray arrayInstance,
            jchar* pArrayContent,
            bool bCopy);

/**
 * Releases the pointer on a an array of bytes.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @param[in]  pArrayContent  The array content. May be null.
 *
 * @param[in]  bCopy  true to copy back the content of the array.
 **/
void WJavaReleaseByteArrayElements(
            JNIEnv *env,
            jbyteArray arrayInstance,
            jbyte* pArrayContent,
            bool bCopy);

/**
 * Returns the number of element in an array.
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  arrayInstance  The array instance. May be null.
 *
 * @return  The number of elements in the array or zero if the array is null.
 **/
uint32_t WJavaGetArrayLength(
            JNIEnv *env,
            jarray arrayInstance);

/**
 * Returns a pointer on the content of a string.
 *
 * The pointer returned by PJavaGetStringContent() should be freed with
 * WJavaReleaseStringContent().
 *
 * @param[in]  env  The global context.
 *
 * @param[in]  stringInstance  The string instance. May be null.
 *
 * @return  A pointer on a zero-ended Unicode string representing the content
 *          of the string instance. This value may be an empty string or null
 *          if the string instance is null.
 **/
const tchar* WJavaGetStringContent(
            JNIEnv *env,
            jstring stringInstance);

/**
 * Release a buffer allocated for the content of a string.
 *
 * The pointer shall be allocated with WJavaGetStringContent().
 *
 * @param[in]  pStringContent The pointer on the buffer to release, it may be null.
 **/
void WJavaReleaseStringContent(
            const tchar* pStringContent);

/**
 * Returns a pointer on the native buffer of a buffer reference.
 *
 * @param[in]  hBufferReference  The buffer reference.
 *
 * @return  The pointer on the buffer or null if the reference is null.
 *
 * @keyword  P_FUNCTION
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
uint8_t* WJavaGetByteBufferPointer(
            W_HANDLE hBufferReference);

/**
 * Returns the length of a buffer reference.
 *
 * @param[in]  hBufferReference  The buffer reference.
 *
 * @return  The length in bytes of the buffer or zero if the reference is null.
 *
 * @keyword  P_FUNCTION
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
uint32_t WJavaGetByteBufferLength(
            W_HANDLE hBufferReference);

/**
 * Returns the offset of a buffer reference.
 *
 * @param[in]  hBufferReference  The buffer reference.
 *
 * @return  The offset in bytes of the buffer or zero if the reference is null.
 *
 * @keyword  P_FUNCTION
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
uint32_t WJavaGetByteBufferOffset(
            W_HANDLE hBufferReference);

/**
 * Creates an new buffer reference for the native functions.
 *
 * @param[in]  pJavaBuffer  The java byte buffer to share.
 *             This may be null or empty.
 *             If the java buffer is non-null, the content is copied in the native buffer.
 *
 * @param[in]  nOffset  The zero-based offset in bytes of the first byte to share.
 *
 * @param[in]  nLength  The number of bytes to share.
 *
 * @return  The reference of the native buffer or W_NULL_HANDLE if an error occured.
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_HANDLE WJavaCreateByteBuffer(
            OPEN_NFC_BUF1_I uint8_t* pJavaBuffer,
            OPEN_NFC_BUF1_OFFSET uint32_t nOffset,
            OPEN_NFC_BUF1_LENGTH uint32_t nLength);

/**
 * Releases a buffer reference.
 *
 * @param[in]  hBufferReference  The buffer reference.
 *
 * @param[out]  pJavaBuffer  If non-null, the content of the native buffer
 *         is copied to the java buffer before being release.
 *
 * @param[in]   nJavaBufferLength  The total length in bytes of the java buffer.
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
void WJavaReleaseByteBuffer(
            W_HANDLE hBufferReference,
            OPEN_NFC_BUF1_O uint8_t* pJavaBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nJavaBufferLength);

/**
 * See W14Part3GetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR W14Part3GetConnectionInfoBuffer(
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See W14Part4GetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR W14Part4GetConnectionInfoBuffer(
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See W15GetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR W15GetConnectionInfoBuffer (
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See WPicoGetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR WPicoGetConnectionInfoBuffer (
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See WMifareGetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR WMifareGetConnectionInfoBuffer (
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See WType1ChipGetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR WType1ChipGetConnectionInfoBuffer (
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See WFeliCaGetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR WFeliCaGetConnectionInfoBuffer (
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See WBPrimeGetConnectionInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR WBPrimeGetConnectionInfoBuffer(
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See WMyDGetConnectionInfoBuffer()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR WMyDGetConnectionInfoBuffer(
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);

/**
 * See WFeliCaRead()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
void WFeliCaReadSimple (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_OA uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  OPEN_NFC_BUF2_LENGTH uint32_t  nNumberOfService,
                  OPEN_NFC_BUF2_I uint32_t* pServiceCodeList,
                  OPEN_NFC_BUF3_LENGTH uint32_t nNumberOfBlocks,
                  OPEN_NFC_BUF3_I const uint8_t* pBlockList);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
void WFeliCaWriteSimple (
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA const uint8_t* pBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nLength,
                  OPEN_NFC_BUF2_LENGTH uint32_t  nNumberOfService,
                  OPEN_NFC_BUF2_I uint32_t* pServiceCodeList,
                  OPEN_NFC_BUF3_LENGTH uint32_t nNumberOfBlocks,
                  OPEN_NFC_BUF3_I const uint8_t* pBlockList);

/**
 * See WNDEFGetTagInfo()
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
W_ERROR WNDEFGetTagInfoBuffer(
            W_HANDLE hConnection,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);


/**
 * Retreives the next callback for the upper layer.
 *
 * @param[out]  pArgs  The array receiving the arguments. The content is left
 *              unchanged if no callback is available.
 *
 * @param[out]  nArgsSize  The size in elements of the array of arguments.
 *
 * @return  true if a callback is available, false otherwise.
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
bool WDFCPumpJNICallback(
               OPEN_NFC_BUF1_O uint32_t* pArgs,
               OPEN_NFC_BUF1_LENGTH uint32_t nArgsSize);

/**
 * See WEmulOpenConnection()
 *
 * @keyword  JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
void WEmulOpenConnectionBuffer(
            OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pOpenCallback,
            void* pOpenCallbackParameter,
            OPEN_NFC_USER_EVENT_HANDLER tWBasicGenericEventHandler* pEventCallback,
            void* pEventCallbackParameter,
            OPEN_NFC_USER_EVENT_HANDLER tWEmulCommandReceived* pCommandCallback,
            void* pCommandCallbackParameter,
            uint32_t nCardType,
            OPEN_NFC_BUF1_I uint8_t* pUID,
            OPEN_NFC_BUF1_LENGTH uint32_t nUIDLength,
            uint32_t nRandomIdentifierLength,
            OPEN_NFC_BUF2_OW W_HANDLE* phHandle);

/**
 * Checks a NFC Controller property of type boolean.
 *
 * @param  nPropertyIdentifier  The property identifier.
 *
 * @return  true if the property is true,
 *          false if the property is false or if an error is detected.
 *
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
bool WJavaNFCControllerGetBooleanProperty(
            uint8_t nPropertyIdentifier );

/**
 * @keyword P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 *
 * @see WP2PSetConfiguration
 **/
  
W_ERROR WJavaP2PSetConfigurationBuffer(
            OPEN_NFC_BUF1_I uint8_t* pConfigurationBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nConfigurationBufferLength);
  

/**
 * @keyword P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 *
 * @see WP2PGetConfiguration
 **/
 
 W_ERROR WJavaP2PGetConfigurationBuffer(
            OPEN_NFC_BUF1_O uint8_t* pConfigurationBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nConfigurationBufferLength);
 
/**
 * @keyword P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 *
 * @see WP2PGetLinkProperties
 **/
W_ERROR WJavaP2PGetLinkPropertiesBuffer(
            W_HANDLE hLink,
            OPEN_NFC_BUF1_O uint8_t* pInfoBuffer,
            OPEN_NFC_BUF1_LENGTH uint32_t nInfoBufferLength);


/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.nfctag.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
uint32_t WJavaNDEFGetMessageContent(
                  W_HANDLE hMessage,
                  OPEN_NFC_BUF1_O uint8_t* pMessageBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nMessageBufferLength);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
void WJavaNDEFWriteMessage(
                  W_HANDLE hConnection,
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  OPEN_NFC_BUF1_IA uint8_t* pMessageBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nMessageBufferLength,
                  uint32_t nActionMask,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

/**
 * @keyword  P_FUNCTION, JNI(org.opennfc.MethodAutogen)
 *
 * @ifdef P_INCLUDE_JAVA_API
 **/
void WJavaNDEFWriteMessageOnAnyTag(
                  OPEN_NFC_USER_CALLBACK tWBasicGenericCallbackFunction* pCallback,
                  void* pCallbackParameter,
                  uint8_t nPriority,
                  OPEN_NFC_BUF1_IA uint8_t* pMessageBuffer,
                  OPEN_NFC_BUF1_LENGTH uint32_t nMessageBufferLength,
                  uint32_t nActionMask,
                  OPEN_NFC_BUF2_OW W_HANDLE* phOperation);

#endif /* __OPEN_NFC_JNI_H */

