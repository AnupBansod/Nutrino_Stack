/*
 * Copyright (c) 2009-2010 Inside Secure, All Rights Reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>

#include "porting_config.h"
#include "open_nfc.h"
#include "ccclient.h"

#include <android/log.h>


static const char tag[] = "CCCLIENT";

typedef int SOCKET;

#define INVALID_SOCKET  (SOCKET)(-1)
#define max(a,b)  ((a) > (b) ? (a) : (b))

#define MSG_ROUTER_HELLO               'A'
#define MSG_PROVIDER_CONNECT           'B'
#define MSG_CLIENT_CONNECT             'C'
#define MSG_CLIENT_CONNECT_WAIT        'D'
#define MSG_CONNECT_OK                 'E'
#define MSG_CONNECT_ERR_NO_PROVIDER    'F'
#define MSG_CONNECT_ERR_PROVIDER_BUSY  'G'
#define MSG_DISCONNECT                 'H'
#define MSG_SPECIFIP_MESSAGE           'K'
#define MSG_ADMIN                      'M'

#define CCCLIENT_SEND_BUFFER_LENGTH  4096
#define CCCLIENT_RECV_BUFFER_LENGTH  4096

typedef struct __tCCClientInstance
{
   SOCKET socket;
   uint8_t aSendBuffer[CCCLIENT_SEND_BUFFER_LENGTH];
   uint8_t aRecvBuffer[CCCLIENT_RECV_BUFFER_LENGTH];
   pthread_mutex_t sSendCS;
} tCCClientInstance;

static sint32_t static_CCClientReceiveMessage(
            tCCClientInstance* pInstance,
            uint8_t* pBuffer,
            uint32_t nBufferLength,
            uint8_t* pnType,
            tchar*** pppParameterList,
            uint32_t* pnParameterNumber,
            uint8_t** pPayloadBuffer,
            uint32_t* pnPayloadLength,
            bool bWait);

static uint32_t static_CCClientSendMessage(
            tCCClientInstance* pInstance,
            uint8_t nType,
            const tchar** ppParameterList,
            uint32_t nParameterNumber,
            const uint8_t* pPayload1Buffer,
            uint32_t nPayload1Length,
            const uint8_t* pPayload2Buffer,
            uint32_t nPayload2Length);

sint32_t static_CCClientWaitRxEvent(
            tCCClientInstance* pInstance);

static SOCKET static_CCClientCreateSocket(
            uint32_t nInetAddr,
            uint32_t *pnError)
{
   SOCKET sConnectionSocket;
   struct sockaddr_in clientService;
   int bTCPNoDelay = true;
   sint32_t flags;

   sConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);

   if (sConnectionSocket < 0)
   {
		__android_log_print(ANDROID_LOG_ERROR, tag, "static_CCClientCreateSocket / socket() failed %d ", errno);

      *pnError = CP_ERROR_SOCKET_OPERATION;
      sConnectionSocket = INVALID_SOCKET;
      goto return_error;
   }

   if (setsockopt(sConnectionSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bTCPNoDelay, sizeof(bTCPNoDelay)) == -1)
   {
		__android_log_print(ANDROID_LOG_ERROR, tag, "static_CCClientCreateSocket / setsockopt() failed %d ", errno);

      *pnError = CP_ERROR_SOCKET_OPERATION;
      goto return_error;
   }

   clientService.sin_family = AF_INET;
   clientService.sin_addr.s_addr = nInetAddr;
   clientService.sin_port = htons( 14443 );

   if ( connect( sConnectionSocket, (struct sockaddr*) &clientService, sizeof(clientService) ) == -1)
   {
		__android_log_print(ANDROID_LOG_ERROR, tag, "static_CCClientCreateSocket / connect() failed %d ", errno);
      *pnError = CP_ERROR_CONNECTION_FAILURE;
      goto return_error;
   }

   flags = fcntl (sConnectionSocket, F_GETFL, 0);

   if (flags == -1) {
		__android_log_print(ANDROID_LOG_ERROR, tag, "static_CCClientCreateSocket / fcntl(F_GETFL) failed %d ", errno);
      *pnError = CP_ERROR_SOCKET_OPERATION;
      goto return_error;
   }

   if (fcntl(sConnectionSocket, F_SETFL, flags | O_NONBLOCK) < 0)
   {
		__android_log_print(ANDROID_LOG_ERROR, tag, "static_CCClientCreateSocket / fcntl(F_SETFL) failed %d ", errno);
      *pnError = CP_ERROR_SOCKET_OPERATION;
      goto return_error;
   }

   return sConnectionSocket;

return_error:

   if(sConnectionSocket != INVALID_SOCKET)
   {
      close(sConnectionSocket);
      sConnectionSocket = INVALID_SOCKET;
   }

   return INVALID_SOCKET;
}


static tCCClientInstance* static_CCClientCreateInstance(
            uint8_t       nType,
            uint32_t    nInetAddr,
            const tchar* pServiceType,
            const tchar* pProviderURI,
            const tchar* pServiceName,
            uint32_t* pnError)
{
   tCCClientInstance* pInstance;
   sint32_t res;
   tchar** ppParameterList;
   uint8_t nReceivedType;
   uint32_t nParameterNumber;
   uint8_t* pPayloadBuffer;
   uint32_t nPayloadLength;
   const tchar* apParameterList[13];
   tchar aApplicationaName[256];
   tchar aApplicationIdentifier[10];
   char    aApplicationIdentifierASCII[10];
   uint32_t pos;
   uint32_t i;

   pInstance = (tCCClientInstance*)malloc(sizeof(tCCClientInstance));

   if(pInstance == null)
   {
      *pnError = CP_ERROR_MEMORY;
      goto return_error;
   }

   pInstance->socket = INVALID_SOCKET;
   pthread_mutex_init(&pInstance->sSendCS, NULL);

   pInstance->socket = static_CCClientCreateSocket(nInetAddr, pnError);
   if(pInstance->socket == INVALID_SOCKET)
   {
      goto return_error;
   }

   if((res = static_CCClientReceiveMessage(
            pInstance,
            pInstance->aRecvBuffer,
            CCCLIENT_RECV_BUFFER_LENGTH,
            &nReceivedType,
            &ppParameterList,
            &nParameterNumber,
            &pPayloadBuffer,
            &nPayloadLength, true)) < 0)
   {
     *pnError = CP_ERROR_TX_RX_MESSAGE;
      goto return_error;
   }

   if((nReceivedType != MSG_ROUTER_HELLO) || (nParameterNumber < 1))
   {
      *pnError = CP_ERROR_PROTOCOL;
      goto return_error;
   }

   if((ppParameterList[0][0] != '0')
   || (ppParameterList[0][1] != '.')
   || (ppParameterList[0][2] != '0')
   || (ppParameterList[0][3] != '3')
   || (ppParameterList[0][4] != 0))
   {
      *pnError = CP_ERROR_CC_VERSION;
      goto return_error;
   };


#if 0 /* @todo */


   pos = GetModuleFileName(NULL, aApplicationaName, 256);
   while(pos != 0)
   {
      pos--;
      if((aApplicationaName[pos]==L'\\') || (aApplicationaName[pos]==L'/'))
      {
         pos++;
         break;
      }
   }

#else

   aApplicationaName[0] = 'T';
   aApplicationaName[1] = 'E';
   aApplicationaName[2] = 'S';
   aApplicationaName[3] = 'T';
   aApplicationaName[4] = 0;

   pos = 0;
#endif


   sprintf(aApplicationIdentifierASCII, "%08x", getpid());

   for(i = 0; i<9; i++)
   {
      aApplicationIdentifier[i] = aApplicationIdentifierASCII[i];
   }


   nParameterNumber = 0;
   apParameterList[nParameterNumber++] = null; /* UUID */

   apParameterList[nParameterNumber++] = null; /* host name */
   apParameterList[nParameterNumber++] = null; /* host address */
   apParameterList[nParameterNumber++] = null; /* host image */

   apParameterList[nParameterNumber++] = &aApplicationaName[pos]; /* application name */
   apParameterList[nParameterNumber++] = aApplicationIdentifier; /* application identifier */
   apParameterList[nParameterNumber++] = null; /* application image */

   apParameterList[nParameterNumber++] = pServiceName; /* Service name */
   apParameterList[nParameterNumber++] = null;  /* Service Image */
   apParameterList[nParameterNumber++] = pServiceType; /* Service Type */

   apParameterList[nParameterNumber++] = pProviderURI; /* Connection string */

   if(static_CCClientSendMessage(
      pInstance,
      nType,
      apParameterList,
      nParameterNumber,
      null, 0, null, 0) == 0)
   {
      *pnError = CP_ERROR_TX_RX_MESSAGE;
      goto return_error;
   };

   if((res = static_CCClientReceiveMessage(
            pInstance,
            pInstance->aRecvBuffer,
            CCCLIENT_RECV_BUFFER_LENGTH,
            &nReceivedType,
            &ppParameterList,
            &nParameterNumber,
            &pPayloadBuffer,
            &nPayloadLength, true)) < 0)
   {
      *pnError = CP_ERROR_TX_RX_MESSAGE;
      goto return_error;
   }

   if(nReceivedType == MSG_CONNECT_ERR_NO_PROVIDER)
   {
      *pnError = CP_ERROR_NO_PROVIDER;
      goto return_error;
   }
   else if(nReceivedType == MSG_CONNECT_ERR_PROVIDER_BUSY)
   {
      *pnError = CP_ERROR_PROVIDER_BUSY;
      goto return_error;
   }
   else if(nReceivedType != MSG_CONNECT_OK)
   {
      *pnError = CP_ERROR_PROTOCOL;
      goto return_error;
   }

   *pnError = CP_SUCCESS;

   return pInstance;

return_error:

   if(pInstance != null)
   {
      if(pInstance->socket != INVALID_SOCKET)
      {
         shutdown(pInstance->socket, SHUT_RDWR);
         close(pInstance->socket);
      }

      pthread_mutex_destroy(&pInstance->sSendCS);

      free(pInstance);
   }

   return null;
}

/**
 * Returns the value of the 32 bit integer encoded in a buffer.
 *
 * @param[in]  pBuffer The buffer where is encoded the integer.
 *
 * @param[in]  nOffset  The offset of the integer.
 *
 * @param[out] pnValue  A pointer on a variable valued with the integer value.
 *
 * @return  The offset following the integer value in the buffer.
 **/
static uint32_t static_CCClientParseIntegeter(
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t* pnValue)
{
   *pnValue = (pBuffer[nOffset + 3] << 24) | (pBuffer[nOffset + 2] << 16) | (pBuffer[nOffset + 1] << 8) | pBuffer[nOffset];

   return nOffset + 4;
}

/**
 * Writes a 32 bit integer in a buffer.
 *
 * @param[out] pBuffer  The buffer where to encode the value.
 *
 * @param[in]  nOffset  The offset of the integer.
 *
 * @param[in]  nValue  The value to encode.
 *
 * @return  The offset following the integer value in the buffer.
 **/
static uint32_t static_CCClientWriteInteger(
                  uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nValue)
{
   pBuffer[nOffset] = (uint8_t)(nValue & 0xFF);
   pBuffer[nOffset + 1] = (uint8_t)((nValue >> 8) & 0xFF);
   pBuffer[nOffset + 2] = (uint8_t)((nValue >> 16) & 0xFF);
   pBuffer[nOffset + 3] = (uint8_t)((nValue >> 24) & 0xFF);

   return nOffset + 4;
}

/**
 * Returns the size of a zero-ended Unicode string once encoded in Utf8.
 * The size does not include the terminating zero.
 *
 * @param[in]  pUnicodeString The Unicode string.
 *
 * @return  The size of a string encoded in Utf8.
 **/
static uint32_t static_CCClientGetUtf8Length(
               const tchar* pUnicodeString)
{
   tchar c;
   uint32_t nPos = 0;

   while((c = *pUnicodeString++) != 0)
   {
      if(c < 0x0080)
      {
         nPos++;
      }
      else if(c < 0x0800)
      {
         nPos +=2;
      }
      else
      {
         nPos +=3;
      }
   }

   return nPos;
}

/**
 * Returns the value of a string encoded in Utf8 in a buffer.
 *
 * @param[in]  pBuffer The buffer where is encoded the string.
 *
 * @param[in]  nOffset  The offset of the string in the buffer.
 *
 * @param[out] pStringBuffer  A pointer on the buffer receiving the string value.
 *             a zero is added at the end of the string.
 *
 * @return  The offset of the byte following the ending zero in the buffer.
 **/
static uint32_t static_CCClientParseString(
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  tchar* pStringBuffer)
{
   tchar c;
   uint32_t nIndex = 0;
   tchar nValue;

   for (;;)
   {
      c = pBuffer[nOffset++];

      if(c == 0x00)
      {
         pStringBuffer[nIndex] = 0x0000;
         return nOffset;
      }

      if ( (c & 0x80) == 0x00 )
      {
         /* [0xxx-xxxx] */
         nValue = c;      /* 0xxx-xxxx */
      }
      else if ( (c & 0xE0) == 0xE0 )
      {
         nValue =  ((((tchar)c) & 0x0f ) << 12);/* ....-xxxx */
         c = pBuffer[nOffset++];
         nValue |= ((((tchar)c) & 0x3f) << 6);   /* ..xx-xxxx */
         c = pBuffer[nOffset++];
         nValue |= (((tchar)c) & 0x3f);          /* ..xx-xxxx */
      }
      else
      {
         nValue =  (((tchar)c) & 0x1f) << 6;/* ...x-xxxx */
         c = pBuffer[nOffset++];
         nValue |= (((tchar)c) & 0x3f);       /* ..xx-xxxx */
      }

      pStringBuffer[ nIndex++ ] = nValue;
   }
}


/**
 * Returns the length in characters of a zero-ended Utf8 string.
 *
 * @param[in]  pBuffer  The buffer containing the string.
 *
 * @param[in]  nOffset  the offset where is located the string.
 *
 * @param[in]  nBufferLength  the buffer length in bytes.
 *
 * @return  The length in characters of the string, including a terminating zero.
 *          0 if an error is detected.
 **/
static uint32_t static_CCClientGetUnicodeLength(
                  const uint8_t* pBuffer,
                  uint32_t nOffset,
                  uint32_t nBufferLength)
{
   uint8_t c;
   uint32_t nStringLength = 0;

   for(;;)
   {
      c = pBuffer[nOffset++];

      nStringLength++;

      if(c == 0x00)
      {
         break;
      }

      if((c & 0x80) != 0x00)
      {
         if((c & 0xE0) == 0xC0)
         {
            nOffset++;
         }
         else
         {
            nOffset+=2;
         }
      }

      if(nOffset >= nBufferLength)
      {
         nStringLength = 0;
         break;
      }
   }

   return nStringLength;
}


/**
 * Writes a string in a Utf8 buffer.
 *
 * @param[out] pBuffer  The buffer where to encode the string in Utf8.
 *
 * @param[in]  nOffset  The offset in the buffer where to write the string.
 *
 * @param[in]  pString  The zero ended unicode string to encode in the buffer.
 *
 * @return  The offset following the string in the buffer.
 **/
static uint32_t static_CCClientWriteString(
                   uint8_t* pBuffer,
                   uint32_t nOffset,
                   const tchar* pString)
{
   tchar v;
   sint32_t i = 0;

   while ((v = pString[i++]) != 0)
   {
      if ( v < 128 )
      {
         /* [0xxx-xxxx] */
         pBuffer[nOffset++] = (uint8_t)v;          /* 00000000-0xxxxxx */
      }
      else if ( v < 2048 )
      {
         /* [110x-xxxx][10xx-xxxx] */
         pBuffer[nOffset++] = (uint8_t)(0xC0 | (v>>6));       /* 00000xxx-xx...... */
         pBuffer[nOffset++] = (uint8_t)(0x80 | (v&0x003F));   /* 00000...-..xxxxxx */
      }
      else
      {
         /* [1110-xxxx][10xx-xxxx][10xx-xxxx] */
         pBuffer[nOffset++] = (uint8_t)(0xE0 | ((v&0xF000)>>12)); /* xxxx....-........ */
         pBuffer[nOffset++] = (uint8_t)(0x80 | ((v&0x0FC0)>>6));  /* ....xxxx-xx...... */
         pBuffer[nOffset++] = (uint8_t)(0x80 | (v&0x003F));       /* ........-..xxxxxx */
      }
   }

   return nOffset;
}

/**
 * Builds a message.
 *
 * @param[out]  pBuffer the buffer receiving the message content.
 *
 * @param[in]   nBufferLength  The maximum length in byte of the message data stored in \a pBuffer.
 *
 * @param[in]   nType  The type of the message.
 *
 * @param[in]   ppParameterList  The array of message parameters.
 *
 * @param[in]   nParameterNumber  The number of message parameters.
 *
 * @param[in]   nPayloadLength  The length in bytes of the binary payload or zero if no payload is provided.
 *
 * @return  The length in bytes of the message or zero if the buffer is too short.
 **/
static uint32_t static_CCClientBuildMessage(
            uint8_t* pBuffer,
            uint32_t nBufferLength,
            uint8_t nType,
            const tchar** ppParameterList,
            uint32_t nParameterNumber,
            uint32_t nPayloadLength)
{
   uint32_t nLength;
   uint32_t nIndex, nOffset;

   nLength = 1;
   for(nIndex = 0; nIndex < nParameterNumber ;nIndex++)
   {
       if (ppParameterList[nIndex] != null)
       {
          nLength += static_CCClientGetUtf8Length(ppParameterList[nIndex]);
       }
       nLength++;
   }

   if(nPayloadLength != 0)
   {
      nLength += 1 + nPayloadLength;
   }

   if((nLength + 4) > nBufferLength)
   {
      return 0;
   }

   nOffset = 0;

   nOffset = static_CCClientWriteInteger(pBuffer, nOffset, nLength);
   pBuffer[nOffset++] = nType;

   for(nIndex = 0; nIndex < nParameterNumber ;nIndex++)
   {
       if (ppParameterList[nIndex] != null)
       {
          nOffset = static_CCClientWriteString(pBuffer, nOffset, ppParameterList[nIndex]);
       }
       pBuffer[nOffset++] = 0x00;
   }

   if(nPayloadLength != 0)
   {
      pBuffer[nOffset++] = 0x01;
   }

   return nLength + 4 - nPayloadLength;
}

/**
 * Parses the content of a message buffer.
 *
 * @param[in]  pBuffer  The message buffer.
 *
 * @param[in]  nBufferLength  The length in bytes of the message buffer.
 *
 * @param[in]  nMessageLength  The message length in bytes.
 *
 * @param[out]  pnType  A pointer on a variable valued with the type of the message.
 *
 * @param[out]  pppParameterList  A pointer on an array receiving the pointers on the string parameters.
 *
 * @param[out]  pnParameterNumber  Set with the maximum number of parameter in the`\a ppParameterList array.
 *              Receive the actual number of parameters.
 *
 * @param[out]  pPayloadBuffer  A pointer on a variable valued with the address of the binary payload buffer, if any.
 *
 * @param[out]  pnPayloadLength  A pointer on a variable receiving the length in bytes of the binary payload buffer.
 *              This value is set to zero if no payload is present or if an error occurs.
 *
 * @return  The actual length in bytes of the message, or zero if an error occurs.
 **/
static uint32_t static_CCClientParseMessage(
            const uint8_t* pBuffer,
            uint32_t nBufferLength,
            uint32_t nMessageLength,
            uint8_t* pnType,
            tchar*** pppParameterList,
            uint32_t* pnParameterNumber,
            uint8_t** pPayloadBuffer,
            uint32_t* pnPayloadLength)
{
   uint32_t nOffset = 0;
   uint32_t nLowBoundary;
   uint32_t nParameterNumber = 0;

   if(nMessageLength == 0)
   {
      return 0;
   }
   *pnType = pBuffer[nOffset++];

   nLowBoundary = nMessageLength;
   *pppParameterList = (tchar**)&pBuffer[nLowBoundary];

   while(nOffset < nMessageLength)
   {
      if(pBuffer[nOffset] == 0x01)
      {
         nOffset++;
         if(nOffset > nMessageLength)
         {
            return 0;
         }

         *pnPayloadLength = nMessageLength - nOffset;

         *pPayloadBuffer = (uint8_t *) &pBuffer[nOffset];
         break;
      }
      else
      {
         uint32_t nUnicodeLength = static_CCClientGetUnicodeLength( pBuffer, nOffset, nBufferLength);
         tchar* pString;
         if(nUnicodeLength == 0)
         {
            return 0;
         }
         if(nLowBoundary + sizeof(tchar*) + (nUnicodeLength * sizeof(tchar)) > nBufferLength)
         {
            return 0;
         }
         nBufferLength -= (nUnicodeLength * sizeof(tchar));
         nLowBoundary += sizeof(tchar*);
         if(nUnicodeLength != 1)
         {
            pString = (tchar*)&pBuffer[nBufferLength];
            nOffset = static_CCClientParseString( pBuffer, nOffset, pString);
         }
         else
         {
            pString = null;
            nOffset++;
         }
         (*pppParameterList)[nParameterNumber++] = pString;
      }
   }

   *pnParameterNumber = nParameterNumber;

   return nMessageLength;
}

static sint32_t static_CCClientReceive(
            tCCClientInstance* pInstance,
            uint8_t* pBuffer,
            uint32_t nLength,
            bool bWait)
{
   sint32_t nOffset = 0;
   sint32_t res;

   while(nLength != 0)
   {
      res = recv(pInstance->socket, &pBuffer[nOffset], nLength, 0);
      if(res < 0)
      {
         if(errno == EWOULDBLOCK)
         {
            res = 0;
         }
         else
         {
            return res;
         }
      }
      else if(res == 0)
      {
         /* Socket gracefully shutdown */
         return -1;
      }

      if(res == 0)
      {
         if((nOffset == 0) && (bWait == false))
         {
            return 0;
         }
         static_CCClientWaitRxEvent(pInstance);
      }
      else
      {
         nOffset += res;
         nLength -= res;
      }
   }

   return 1;
}

sint32_t static_CCClientWaitRxEvent(
            tCCClientInstance* pInstance)
{
   int result, nFds = 0;
   fd_set readFds, writeFds, errFds;
   int nResult;

   FD_ZERO(&readFds);
   FD_ZERO(&writeFds);
   FD_ZERO(&errFds);

   FD_SET(pInstance->socket, &readFds);
/*   FD_SET(pInstance->socket, &writeFds); */

   nFds = max(nFds, pInstance->socket);

   result = select(nFds + 1, &readFds, &writeFds, &errFds, NULL);

   if ((result < 0) && (errno == EINTR))
   {
      nResult = 0;
   }
   else if (result < 0) {
      perror("select()");
      nResult = -1;
   }
   else if (result == 0)
   {
      nResult = 0; /* TimeOut */
   }
   else
   {
      nResult = 0;
   }

   return nResult;
}

sint32_t CCClientWaitRxEvent(
            void* pConnection)
{
   tCCClientInstance* pInstance = (tCCClientInstance*)pConnection;

   return static_CCClientWaitRxEvent( pInstance );
}

static sint32_t static_CCClientReceiveMessage(
            tCCClientInstance* pInstance,
            uint8_t* pBuffer,
            uint32_t nBufferLength,
            uint8_t* pnType,
            tchar*** pppParameterList,
            uint32_t* pnParameterNumber,
            uint8_t** pPayloadBuffer,
            uint32_t* pnPayloadLength,
            bool bWait)
{
   uint32_t nLength;
   sint32_t res = static_CCClientReceive(
      pInstance,
      pBuffer, 4, bWait);

   if(res <= 0)
   {
      return res;
   }

   static_CCClientParseIntegeter( pBuffer, 0, &nLength);

   if(nLength == 0)
   {
      return 0;
   }

   if(nLength > nBufferLength)
   {
      return -1;
   }

   res = static_CCClientReceive(
      pInstance,
      pBuffer, nLength, true);

   if(res < 0)
   {
      return res;
   }

   if(static_CCClientParseMessage(
            pBuffer, nBufferLength,
            nLength,
            pnType,
            pppParameterList, pnParameterNumber,
            pPayloadBuffer, pnPayloadLength) == 0)
   {
      return -1;
   }

   return 1;
}

static uint32_t static_CCClientSend(
            SOCKET socket,
            const uint8_t* pBuffer,
            uint32_t nLength)
{
   sint32_t res;
   sint32_t nOffset = 0;

   while(nLength != 0)
   {
     res = send(socket, &pBuffer[nOffset], nLength, 0);
     if(res < 0)
     {
        return 0;
    }
    nOffset += res;
    nLength -= res;
   }

   return 1;
}

static uint32_t static_CCClientSendMessage(
            tCCClientInstance* pInstance,
            uint8_t nType,
            const tchar** ppParameterList,
            uint32_t nParameterNumber,
            const uint8_t* pPayload1Buffer,
            uint32_t nPayload1Length,
            const uint8_t* pPayload2Buffer,
            uint32_t nPayload2Length)
{
   uint32_t nMessageHeadLength;
   uint32_t nResult = 0;


   pthread_mutex_lock(&pInstance->sSendCS);

   nMessageHeadLength = static_CCClientBuildMessage(
      pInstance->aSendBuffer, CCCLIENT_SEND_BUFFER_LENGTH,
      nType,
      ppParameterList, nParameterNumber,
      nPayload1Length + nPayload2Length);

   if(nMessageHeadLength != 0)
   {
      if(static_CCClientSend(
         pInstance->socket, pInstance->aSendBuffer, nMessageHeadLength) != 0)
      {
         nResult = 1;

         if(nPayload1Length != 0)
         {
            if(static_CCClientSend(
               pInstance->socket, pPayload1Buffer, nPayload1Length) == 0)
            {
               nResult = 0;
            }
         }

         if((nResult > 0) && (nPayload2Length != 0))
         {
            if(static_CCClientSend(
               pInstance->socket, pPayload2Buffer, nPayload2Length) == 0)
            {
               nResult = 0;
            }
         }
      }
   }

   pthread_mutex_unlock(&pInstance->sSendCS);

   return nResult;
}

/* See header file */
bool CCClientGetProtocol(
            const tchar* pProviderURI,
            tchar* pProtocolBuffer,
            uint32_t nProtocolBufferLength)
{
   uint32_t nPos = 0;
   uint32_t i = 0;
   tchar c;

   if((pProviderURI[0] != L'c')
   || (pProviderURI[1] != L'c')
   || (pProviderURI[2] != L':'))
   {
      return false;
   }

   nPos = 3;

   /* Skip the host name */
   if(pProviderURI[nPos] == L'/')
   {
      if(pProviderURI[++nPos] != L'/')
      {
         return false;
      }
      do
      {
         c = pProviderURI[++nPos];
         if(c == 0x0000)
         {
            return false;
         }
      } while(c != L'/');

      nPos++;
   }

   while((pProviderURI[nPos] != 0x0000) && (pProviderURI[nPos] != L'?'))
   {
      pProtocolBuffer[i++] = pProviderURI[nPos];
      if(i == nProtocolBufferLength)
      {
         break;
      }
      nPos++;
   }
   pProtocolBuffer[i] = 0;

   return true;
}

/* See header file */
void* CCClientOpen(
            uint32_t     nInetAddr,
            const tchar* pProviderURI,
            bool bWait,
            uint32_t* pnError)
{
   tchar aServiceName[51];
   uint8_t nType;

   if(CCClientGetProtocol(pProviderURI, aServiceName, 51) == false)
   {
      *pnError = CP_ERROR_URI_SYNTAX;
      return null;
   }

   nType = (bWait == true) ? MSG_CLIENT_CONNECT_WAIT : MSG_CLIENT_CONNECT;

   return static_CCClientCreateInstance(
      nType, nInetAddr, aServiceName, pProviderURI, null, pnError);
}

/* See header file */
void* CCClientOpenAsProvider(
            uint32_t       nInetAddr,
            const tchar* pServiceType,
            const tchar* pServiceName,
            uint32_t* pnError)
{
   return static_CCClientCreateInstance(
      MSG_PROVIDER_CONNECT, nInetAddr, pServiceType, null, pServiceName, pnError);
}

/* See header file */
int CCClientGetReceptionEvent(
            void* pConnection)
{
   tCCClientInstance* pInstance = (tCCClientInstance*)pConnection;

   if(pInstance != null)
   {
      return pInstance->socket;
   }

   return -1;
}

/* See header file */
void CCClientClose(
            void* pConnection)
{
   tCCClientInstance* pInstance = (tCCClientInstance*)pConnection;

   if(pInstance != null)
   {
      if(pInstance->socket != INVALID_SOCKET)
      {
         shutdown(pInstance->socket, SHUT_RDWR);
         close(pInstance->socket);
      }

      free(pInstance);
   }
}

/* See header file */
uint32_t CCClientSendData(
            void* pConnection,
            const uint8_t* pPayloadBuffer,
            uint32_t nPayloadLength)
{
   tCCClientInstance* pInstance = (tCCClientInstance*)pConnection;

   if(pInstance == null)
   {
      return 0;
   }

   return static_CCClientSendMessage(
      pInstance,
      MSG_SPECIFIP_MESSAGE,
      null, 0,
      pPayloadBuffer, nPayloadLength,
      null, 0);
}

/* See header file */
uint32_t CCClientSendDataEx(
            void* pConnection,
            const uint8_t nPayload1,
            const uint8_t* pPayload2Buffer,
            uint32_t nPayload2Length)
{
   tCCClientInstance* pInstance = (tCCClientInstance*)pConnection;
   uint32_t nMessageHeadLength;
   uint32_t nResult = 0;
   uint8_t* pBuffer = pInstance->aSendBuffer;
   uint32_t nLength = 3 + nPayload2Length;

   if(pInstance == null)
   {
      return 0;
   }

   if((nLength + 4) > CCCLIENT_SEND_BUFFER_LENGTH)
   {
      return 0;
   }

   pthread_mutex_lock(&pInstance->sSendCS);

   nMessageHeadLength = static_CCClientWriteInteger(pBuffer, 0, nLength);
   pBuffer[nMessageHeadLength++] = MSG_SPECIFIP_MESSAGE;
   pBuffer[nMessageHeadLength++] = 0x01;
   pBuffer[nMessageHeadLength++] = nPayload1;
   if(nPayload2Length < 64)
   {
      memcpy(&pBuffer[nMessageHeadLength], pPayload2Buffer, nPayload2Length);
      nMessageHeadLength += nPayload2Length;
      nPayload2Length = 0;
   }

   if(static_CCClientSend(
      pInstance->socket, pBuffer, nMessageHeadLength) != 0)
   {
      nResult = 1;

      if(nPayload2Length != 0)
      {
         if(static_CCClientSend(
            pInstance->socket, pPayload2Buffer, nPayload2Length) == 0)
         {
            nResult = 0;
         }
      }
   }

   pthread_mutex_unlock(&pInstance->sSendCS);

   return nResult;
}

/* See header file */
sint32_t CCClientReceiveData(
            void* pConnection,
            uint8_t* pBuffer,
            uint32_t nBufferLength,
            uint8_t** ppPayload)
{
   tCCClientInstance* pInstance = (tCCClientInstance*)pConnection;
   uint8_t nType;
   uint32_t nParameterNumber;
   uint32_t nPayloadLength;
   tchar** ppParameterList;
   sint32_t nResult;

   if(pInstance == null)
   {
      return -1;
   }

   nResult = static_CCClientReceiveMessage(
            pInstance,
            pBuffer, nBufferLength,
            &nType,
            &ppParameterList,
            &nParameterNumber,
            ppPayload,
            &nPayloadLength,
            false);

   if(nResult <= 0)
   {
      return nResult;
   }

   if(nType != MSG_SPECIFIP_MESSAGE)
   {
      return 0;
   }

   if(nParameterNumber != 0)
   {
      return 0;
   }

   return (sint32_t)nPayloadLength;
}



