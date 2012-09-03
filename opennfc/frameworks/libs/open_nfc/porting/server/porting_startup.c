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

  Implementation of the server porting procedures

*******************************************************************************/

#define P_MODULE  P_MODULE_DEC( STARTUP )

#include "open_nfc.h"
#include "porting_os.h"
#include "nfc_hal.h"
#include "porting_driver.h"

#include "porting_startup.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef struct __tOpenNFCServerInstance {

	tNALBinding  			 * pNALBinding;
	tDriverInstance		 * pDriverInstance;

	sem_t							hBootSemaphore;							/* Semaphore used to syncrhonize with the end of the NFCC boot sequence */

	/* event management */
	sem_t							hEventSemaphore;							/* semaphore used to trigger a call to PDriverProcessEvent */
	pthread_t					hProcessEventThreadId;					/* thread calling PDriverProcessEvent() */
	bool 							bProcessEventThreadStop;				/* flag to request the termination of the thread */

	/* client management */
	int 							nServerSocket;								/* socket waiting for client sonnections */
	pthread_t					hClientConnectionsThreadId;			/* thread processing the client connections */

	void *						pPortingConfig;

	int						   aWakeUpSockets[2];						/* socket pair use to force wake up of threads blocked in select() */

} tOpenNFCServerInstance;


struct __tUserIdentity {

  int   socket;
  uid_t uid;
  uid_t gid;
};

static tOpenNFCServerInstance g_Instance;

/*
 * Thread dealing with the call of PDriverProcessEvents
 */

void * PDriverProcessEventsThread(void * lpParameter)
{
	while (g_Instance.bProcessEventThreadStop == false)
	{
		if (sem_wait(&g_Instance.hEventSemaphore))
		{
			PDebugError("PDriverProcessEventsThread : sem_wait() failed");
			break;
		}

		PDriverProcessEvents(g_Instance.pDriverInstance);
	}

	return NULL;
}

/*
 * See Functional Specifications Document
 */
void CSyncTriggerEventPump ( void * pConfig )
{
	if (sem_post(&g_Instance.hEventSemaphore))
	{
		PDebugError("PSyncTriggerEventPump : sem_post failed %d", errno);
	}
}

/*
 * For the Linux porting, 2 different security authentications are implemented:
 *   One authentication is based on the authentication data provided by the user application.
 *   The other authentication relies on the UID-GID value for the user application.
 *
 * Data Based Authentication
 * -------------------------
 *  - The user application calls the function WSecurityAuthenticate()
 *    with a non-empty buffer containing the principal data.
 *  - The authentication data created by the HAL includes a copy of this buffer
 *  - The authentication function of the HAL is a comparison of the buffer
 *    content with the principal value.
 *
 * This authentication is used by a framwork on top of the C API, Android for example.
 * An application cannot perform the authentication directly. The framework performs
 * the authentication for the application with the hash of the certificate
 * used the sign the application code.
 *
 *
 * UID-GID Based Authentication
 * -------------------------
 *  - The user application calls the function WSecurityAuthenticate()
 *    with an empty buffer.
 *  - The authentication data created by the HAL includes the UID-GID specified
 *    in the User identity structure.
 *    The User identity structure is computed by the porting layer iin a way that cannot be
 *    conterfeated by the calling application.
 *  - The authentication function of the HAL is a comparison of the UID-GID values
 *    with the principal value.
 *
 * The format of the principal value is the following:
 * - A buffer of 20 bytes with the bytes [0 .. 11] set to 0xFF.
 * - If the bytes [12..15] are not set to 0xFF, they represent the GID value to compare.
 * - If the bytes [16..19] are not set to 0xFF, they represent the UID value to compare.
 *
 * At least one of the GID or UID value must be set in the buffer.
 */

#define P_PRINCIPAL_GID_POS     12
#define P_PRINCIPAL_UID_POS     16
#define P_PRINCIPAL_LENGTH      20

/** The authentication data structure for Linux */
struct __tUserAuthenticationData
{
   uint8_t* pApplicationDataBuffer;
   uint32_t nApplicationDataBufferLength;
};

/**
 * Checks if a principal is encoding a GID-UID
 *
 * @param[in]   pBuffer  The principal buffer.
 *
 * @param[in]   nBufferLength  The principal buffer length.
 *
 * @return  1 if the principal is encodinf a GID-UID, 0 otherwise.
 **/
static uint32_t static_CSecurityIsGidUidPrincipal(
            const uint8_t* pBuffer,
            uint32_t nBufferLength)
{
   uint32_t i;

   if((nBufferLength != P_PRINCIPAL_LENGTH) || (pBuffer == null))
   {
      return 0;
   }

   for(i = 0; i < P_PRINCIPAL_GID_POS; i++)
   {
      if(pBuffer[i] != 0xFF)
      {
         return 0;
      }
   }

   return 1;
}

/* See HAL specification */
bool CSecurityCreateAuthenticationData(
            const tUserIdentity* pUserIdentity,
            const uint8_t* pApplicationDataBuffer,
            uint32_t nApplicationDataBufferLength,
            const tUserAuthenticationData** ppAuthenticationData)
{
   tUserAuthenticationData* pAuthenticationData;

   *ppAuthenticationData = (tUserAuthenticationData*)null;

   if(((nApplicationDataBufferLength == 0) && (pApplicationDataBuffer != null))
   || ((nApplicationDataBufferLength != 0) && (pApplicationDataBuffer == null)))
   {
      return false;
   }

   if(nApplicationDataBufferLength != 0)
   {
      uint32_t i;

      if(nApplicationDataBufferLength != P_PRINCIPAL_LENGTH)
      {
         return false;
      }

      /* Check that the value given by the application is not encoding a GID-UID principal */
      if(static_CSecurityIsGidUidPrincipal(pApplicationDataBuffer, nApplicationDataBufferLength) != 0)
      {
         return false;
      }
   }

   pAuthenticationData = (tUserAuthenticationData*)CMemoryAlloc(sizeof(tUserAuthenticationData));

   if(pAuthenticationData == null)
   {
      return false;
   }

   if(nApplicationDataBufferLength != 0)
   {
      pAuthenticationData->pApplicationDataBuffer = (uint8_t*)CMemoryAlloc(
         nApplicationDataBufferLength);

      if(pAuthenticationData->pApplicationDataBuffer == null)
      {
         CMemoryFree(pAuthenticationData);
         return false;
      }

      CMemoryCopy(pAuthenticationData->pApplicationDataBuffer,
         pApplicationDataBuffer, nApplicationDataBufferLength);
   }
   else
   {
      pAuthenticationData->pApplicationDataBuffer = null;
   }

   pAuthenticationData->nApplicationDataBufferLength = nApplicationDataBufferLength;

   *ppAuthenticationData = pAuthenticationData;

   return true;
}

/* See HAL specification */
void CSecurityDestroyAuthenticationData(
            const tUserAuthenticationData* pAuthenticationData)
{
   if(pAuthenticationData != null)
   {
      if(pAuthenticationData->pApplicationDataBuffer != null)
      {
         CMemoryFree(pAuthenticationData->pApplicationDataBuffer);
      }
      CMemoryFree((void*)pAuthenticationData);
   }
}

/* See HAL specification */
bool CSecurityCheckIdentity(
            const tUserIdentity* pUserIdentity,
            const tUserAuthenticationData* pAuthenticationData,
            const uint8_t* pPrincipalBuffer,
            uint32_t nPrincipalBufferLength)
{
   if((pAuthenticationData == null)
   || (pPrincipalBuffer == null)
   || (nPrincipalBufferLength != P_PRINCIPAL_LENGTH))
   {
      return false;
   }

   if(static_CSecurityIsGidUidPrincipal(pPrincipalBuffer, nPrincipalBufferLength) != 0)
   {
      /* GID-UID case */
      uid_t uid;
      gid_t gid;
      uint8_t * pos = &pPrincipalBuffer[P_PRINCIPAL_GID_POS];

      gid = (gid_t)(
                         ((*pos) & 0xFF)
                    |    (((*(pos+1)) & 0xFF) << 8)
                    |    (((*(pos+2)) & 0xFF) << 16)
                    |    (((*(pos+3)) & 0xFF) << 24)
                  );
                  
                  pos+=4;

      uid = (uid_t)(
                      ((*pos) & 0xFF)
                  |    (((*(pos+1)) & 0xFF) << 8)
                  |    (((*(pos+2)) & 0xFF) << 16)
                  |    (((*(pos+3)) & 0xFF) << 24)
              );
              
              pos+=4;

      if(((gid != (gid_t)-1) && (gid != pUserIdentity->gid))
      || ((uid != (uid_t)-1) && (uid != pUserIdentity->uid)))
      {
         return false;
      }
   }
   else
   {
      /* User data case */

      if(pAuthenticationData->nApplicationDataBufferLength != nPrincipalBufferLength)
      {
         return false;
      }

      if(CMemoryCompare(pAuthenticationData->pApplicationDataBuffer, pPrincipalBuffer, nPrincipalBufferLength) != 0)
      {
         return false;
      }
   }

   return true;
}

/*
 *  PDriverCreate() callback function
 */

static void static_PDriverCreateCompletionCallback( void* pPortingConfig, uint32_t nMode)
{
	if (sem_post(&g_Instance.hBootSemaphore))
	{
		PDebugError("static_PDriverCreateCompletionCallback : sem_post failed %d", errno);
	}
}

/*
 * Initializes the NFCC controller
 */

void * StartNFCC(tNALBinding * pBinding, void * pPortingConfig)
{

	bool bSuccess = false;

	/* initializes the semaphore used for the boot sequence */

	if (sem_init(&g_Instance.hBootSemaphore, 0, 0))
	{
		PDebugError(" BootNFCC : sem_init(hBootSemaphore) failed : errno %d", errno);
		goto return_function;
	}

	/* initializes the semaphore used for the event processing */

	if (sem_init(&g_Instance.hEventSemaphore, 0, 0))
	{
		PDebugError(" BootNFCC : sem_init(hEventSemaphore) failed : errno %d", errno);
		goto return_function;
	}

	/* initialize the socket pair used for thread wake-up */

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, g_Instance.aWakeUpSockets))
	{
		PDebugError(" BootNFCC : socketpair() failed : errno %d", errno);
		goto return_function;
	}

	g_Instance.pNALBinding = pBinding;

	/* create the driver instance */

	g_Instance.pDriverInstance = PDriverCreate(static_PDriverCreateCompletionCallback , pPortingConfig, false, (void *) g_Instance.pNALBinding, sizeof(* g_Instance.pNALBinding));

	if( g_Instance.pDriverInstance == NULL )
	{
		PDebugError(" BootNFCC : PDriverCreate() failed");
		goto return_function;
	}

	/* starts the event processing thread */

	g_Instance.bProcessEventThreadStop = false;

	if (pthread_create(&g_Instance.hProcessEventThreadId, NULL, PDriverProcessEventsThread, NULL) != 0)
	{
		PDebugError(" BootNFCC : : pthread_create(PDriverProcessEventsThread) failed");
		goto return_function;
	}

	/* all is fine... */

	bSuccess = true;

return_function:

	if (bSuccess == false)
	{

		StopNFCC(&g_Instance);

		return NULL;
	}

	else
	{
		return &g_Instance;
	}
}


/*
 * Waits until the NFCC boot has been completed
 */

int WaitForNFCCBoot(void * unused)
{
	if (sem_wait(&g_Instance.hBootSemaphore))
	{
		PDebugError("WaitForNFCCBoot : : sem_wait(hBootSemaphore) failed %d", errno);

		return -1;
	}

	return 0;
}



W_ERROR CServerLoop(const tUserIdentity* pUserIdentity, tDriverInstance* pDriverInstance);

/*
 * Thread dealing with the reception of data from the client
 */

static void * static_ServerLoopThread( void * lpParameter )
{
   const tUserIdentity* pUserIdentity = (const tUserIdentity*)lpParameter;

   W_ERROR nError;

   PDebugTrace("static_ServerLoopThread: Calling PServerLoop(user:%d group:%d)",
      pUserIdentity->uid, pUserIdentity->gid);

   nError = CServerLoop(pUserIdentity, g_Instance.pDriverInstance);

   if(nError != W_SUCCESS)
   {
      PDebugError("static_ServerLoopThread: PServerLoop(user:%d group:%d) returned %d",
         pUserIdentity->uid, pUserIdentity->gid, nError);
   }
   else
   {
      PDebugTrace("static_ServerLoopThread: Returning from PServerLoop(user:%d group:%d)",
         pUserIdentity->uid, pUserIdentity->gid);
   }

   close(pUserIdentity->socket);
   free((void*)pUserIdentity);

   return (void *) nError;
}

/*
 * Thread dealing with processing of client connections
 */

#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

static void * static_AcceptClientConnectionsThread(void * lpParameter)
{
	PDebugTrace("static_AcceptClientConnectionsThread started");

	int nSocket;

	for(;;)
	{
		pthread_t thread_id;
		fd_set readfds;
      int nfds;
      tUserIdentity* pUserIdentity;

		FD_ZERO(&readfds);
		FD_SET(g_Instance.nServerSocket, &readfds);
		FD_SET(g_Instance.aWakeUpSockets[0], &readfds);

      nfds = max(g_Instance.nServerSocket,g_Instance.aWakeUpSockets[0]) +1;

      if (select(nfds, &readfds, NULL, NULL, NULL) < 0)
		{
			PDebugError("static_AcceptClientConnectionsThread : select() failed");
			goto end;
		}

		if (FD_ISSET(g_Instance.aWakeUpSockets[0], &readfds))
		{
			PDebugTrace("static_AcceptClientConnectionsThread : received terminate signal");
			goto end;
		}

		if ((nSocket = accept( g_Instance.nServerSocket, NULL, NULL )) == -1)
		{
			/* accept failed, this typically occurs during server shutdown (listen socket has been closed) */
			PDebugError("static_AcceptClientConnectionsThread : accept() failed");
			goto end;
		}

      {
        int on = 1;

        if (setsockopt(nSocket, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) == -1)
        {
            PDebugError("Error: Cannot request credential passing");
            goto end;
        }

      }

		PDebugTrace("Processing client connection");

		/* create the tread dealing with communication with the client */
      pUserIdentity = (tUserIdentity*)malloc(sizeof(tUserIdentity));

      memset(pUserIdentity, 0, sizeof(tUserIdentity));

      if(pUserIdentity == NULL)
      {
          PDebugError("Error: Cannot allocate client structure\n");
          close(nSocket);
      }
      else
      {
          char data[32], control[32];
          struct msghdr   msg;
          struct cmsghdr  *cmsg;
          struct iovec    iov;

          pUserIdentity->socket = nSocket;

          /* get the credential sent by the client */

          memset(&msg, 0, sizeof(msg));
          iov.iov_base   = data;
          iov.iov_len    = sizeof(data)-1;
          msg.msg_iov    = &iov;
          msg.msg_iovlen = 1;
          msg.msg_control = control;
          msg.msg_controllen = sizeof(control);

          if (recvmsg(pUserIdentity->socket, &msg, 0) < 0)
          {
              PDebugError("Error: Cannot receive credential message");
              close(nSocket);
              free(pUserIdentity);
              continue;
          }

          if (strcmp(data, "OPENNFC"))
          {
              PDebugError("Error : unexpected message format %s", data);
              close(nSocket);
              free(pUserIdentity);
              continue;
          }

          /* Loop over all control messages */
          cmsg = CMSG_FIRSTHDR(&msg);

          while (cmsg != NULL)
          {
              printf("got cmsg\n");
              if ((cmsg->cmsg_level == SOL_SOCKET)  && (cmsg->cmsg_type  == SCM_CREDENTIALS))
              {
                struct ucred * credptr = (struct ucred *) CMSG_DATA(cmsg);
                pUserIdentity->uid = credptr->uid;
                pUserIdentity->gid = credptr->gid;

                PDebugError("Client credentials UID %08X, GID %08X\n", pUserIdentity->uid, pUserIdentity->gid);
                break;
              }
          }

          if (pthread_create(&thread_id, NULL, static_ServerLoopThread, pUserIdentity) != 0)
          {
              PDebugError("Error: Cannot create client thread! \n");
              close(nSocket);
              free(pUserIdentity);
              continue;
          }
      }
   }

end:
	PDebugTrace("static_AcceptClientConnectionsThread stopped");

	return NULL;
}

/*
 * creates a server socket waiting for client connection and starts
 * the thread dealing with client connections
 */

int StartAcceptingClientConnections(void * unused)
{
	int nRetVal = -1;

	struct sockaddr_un sSockAddr;

	g_Instance.nServerSocket = socket(AF_UNIX, SOCK_STREAM, 0);

	if (g_Instance.nServerSocket == -1)
	{
		PDebugError("Error: Cannot create the listen socket");
		goto return_function;
	}

	CMemoryFill(&sSockAddr, 0, sizeof(sSockAddr));

	sSockAddr.sun_family = AF_UNIX;
	strcpy(&sSockAddr.sun_path[1], "opennfc");

	if (bind( g_Instance.nServerSocket, (struct sockaddr*) &sSockAddr, sizeof(sSockAddr)) == -1)
	{
		PDebugError("Error: Cannot bind the listen socket");
		goto return_function;
	}

	if (listen( g_Instance.nServerSocket, 5 ) == -1)
	{
		PDebugError("Error: Error returned by listen()");
		goto return_function;
	}

	g_Instance.hClientConnectionsThreadId = 0;

	if (pthread_create(&g_Instance.hClientConnectionsThreadId, NULL, static_AcceptClientConnectionsThread, NULL) != 0)
	{
		PDebugError("AcceptClientConnections : pthread_create(AcceptClientConnectionsThread) failed");
		goto return_function;
	}

	nRetVal = 0;

return_function:

	if (nRetVal)
	{
		if (g_Instance.nServerSocket != -1)
		{
			close(g_Instance.nServerSocket);
			g_Instance.nServerSocket = -1;
		}
	}

	return nRetVal;
}


int StopAcceptingClientConnections(void * unused)
{
   ssize_t nResult;

	PDebugTrace("RefuseClientConnections");

	if (g_Instance.nServerSocket != -1)
	{
		close(g_Instance.nServerSocket);
		g_Instance.nServerSocket = -1;
	}

	/* write into the aWakeUpSockets to force select() exit */

   nResult = write(g_Instance.aWakeUpSockets[1], "K", 1);
	if (nResult <= 0)
	{
		PDebugError("%s: write error %d", __FUNCTION__, (int) nResult);
	}

	PDebugTrace("waiting for the termination of the client connection manager");
	pthread_join(g_Instance.hClientConnectionsThreadId, NULL);

	return 0;
}


int CloseClientConnections(void * unused)
{
	return 0;
}


int StopNFCC(void * unused)
{

   if (g_Instance.pDriverInstance != NULL)
   {
      PDriverDestroy(g_Instance.pDriverInstance);
      g_Instance.pDriverInstance = NULL;
   }

   close(g_Instance.aWakeUpSockets[0]);
   close(g_Instance.aWakeUpSockets[1]);

	return 0;
}
