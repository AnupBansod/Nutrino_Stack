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

#define P_MODULE  P_MODULE_DEC( HAL )

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <arpa/inet.h>

#include "nal_porting_os.h"
#include "nal_porting_hal.h"
#include "porting_config_hal.h"
#include "driver/open_nfc_int.h"
#include "nfc_hal.h"

#define 	TIMER_SET		'S'
#define	TIMER_CANCEL	'C'
#define 	EVENT_QUIT		'Q'
#define	EVENT_KICK		'K'

extern tNALBinding * const g_pNALBinding;

/* The NFC controller instance */

struct __tNALInstance
{
   /* The timer instance  */

   struct __tNALTimerInstance
   {
      uint32_t nTimerValue;       /* The current timer expiration value, 0 if no timer is pending */
      bool       bIsExpired;        /* The expiration flag */
      bool       bIsInitialized;    /* The initalization flag */

   } sTimerInstance;

   /* The com instance */

   struct __tNALComInstance
   {
      int  fd;                        /* the fd associated with the driver */
      bool reset_pending;

   } sComInstance;

   int aWakeUpSockets[2];            /* sockets used to force select() exit */

   pthread_t hProcessDriverEventsThread;

} g_sNFCCInstance;


/* Retuns the pointer on the single instance */
static __inline tNALInstance* static_PNALGetInstance( void )
{
   return &g_sNFCCInstance;
}

/* Retuns the pointer on the single instance */
static __inline tNALComInstance* static_PComGetInstance( void )
{
   return &g_sNFCCInstance.sComInstance;
}

/* Retuns the pointer on the single instance */
static __inline tNALTimerInstance* static_PTimerGetInstance( void )
{
   return &g_sNFCCInstance.sTimerInstance;
}

/*******************************************************************************

  Timer Functions.

*******************************************************************************/

/* See Functional Specifications Document */
tNALTimerInstance * CNALTimerCreate( void* pPortingConfig )
{
   tNALTimerInstance * pTimer = static_PTimerGetInstance();

   PNALDebugTrace("PTimerCreate");

   if( pTimer != NULL )
   {
      pTimer->nTimerValue = 0L;
      pTimer->bIsExpired = false;
      pTimer->bIsInitialized = true;
   }

   return pTimer;
}

/* See Functional Specifications Document */
void CNALTimerDestroy(tNALTimerInstance * pTimer )
{
   PNALDebugTrace("PTimerDestroy");

   if( pTimer != NULL )
   {
      CNALDebugAssert( pTimer->bIsInitialized != false );
      pTimer->bIsInitialized = false;
   }
}

/* See Functional Specifications Document */
uint32_t CNALTimerGetCurrentTime( tNALTimerInstance* pTimer )
{
   uint32_t nCurrentTime = 0;

   CNALDebugAssert(pTimer != NULL);

   if( pTimer != NULL )
   {
      struct timespec tv;

      CNALDebugAssert( pTimer->bIsInitialized != false );

      if (clock_gettime(CLOCK_MONOTONIC, &tv) == 0)
      {
         nCurrentTime = tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
      }
      else
      {
         PNALDebugError("CNALTimerGetCurrentTime : clock_gettime() failed %d", errno);
      }
   }

   return nCurrentTime;
}

/* See Functional Specifications Document */
void CNALTimerSet( tNALTimerInstance* pTimer, uint32_t nTimeoutMs )
{

   CNALDebugAssert(pTimer != NULL);
	uint8_t buff[1];

   if( pTimer != NULL )
   {
      CNALDebugAssert( pTimer->bIsInitialized != false );

      pTimer->nTimerValue = CNALTimerGetCurrentTime(pTimer) + nTimeoutMs;
      pTimer->bIsExpired = false;

      /* write into the fd to force wakeup of the receive thread */
		buff[0] = TIMER_SET;
      write(g_sNFCCInstance.aWakeUpSockets[0], buff, 1);
   }
}

/* See Functional Specifications Document */
bool CNALTimerIsTimerElapsed( tNALTimerInstance* pTimer )
{
   bool bIsExpired = false;

   CNALDebugAssert(pTimer != NULL);

   if( pTimer != NULL )
   {
      CNALDebugAssert( pTimer->bIsInitialized != false );

      bIsExpired = pTimer->bIsExpired;
      pTimer->bIsExpired = false;
   }

   return bIsExpired;
}

/* See Functional Specifications Document */
void CNALTimerCancel( tNALTimerInstance* pTimer )
{
   CNALDebugAssert(pTimer != NULL);
	uint8_t buff[1];

   if( pTimer != NULL )
   {
      CNALDebugAssert( pTimer->bIsInitialized != false );
      pTimer->nTimerValue = 0L;
      pTimer->bIsExpired = false;

		buff[0] = TIMER_CANCEL;
      write(g_sNFCCInstance.aWakeUpSockets[0], buff, 1);
   }
}

/*
 * Marks the current timer as expired.
 */
void CNALTimerSetExpired( tNALTimerInstance* pTimer)
{
   CNALDebugAssert(pTimer != NULL);

   if (pTimer != NULL)
   {
      CNALDebugAssert( pTimer->bIsInitialized != false );

      pTimer->nTimerValue = 0L;
      pTimer->bIsExpired = true;
   }
}


/*******************************************************************************

  Communication Port Functions

*******************************************************************************/

/* See Functional Specifications Document */
tNALComInstance* CNALComCreate(
               void* pPortingConfig,
               uint32_t* pnType )
{
   tNALComInstance* pComPort = static_PComGetInstance();
	struct open_nfc_ioc_configure config;
	uint32_t addr;

   int idx = 0;
   int ret;

   /*
    * open the device
    */

   pComPort->fd = open("/dev/nfcc", O_RDWR | O_NONBLOCK);

   if (pComPort->fd == -1)
   {
      PNALDebugError("Unable to open the device");
      goto return_error;
   }

   /*
    * configure the driver
    */



	addr = inet_addr((char *) pPortingConfig);

   /* first 4 bytes contain the IPV4 address of the Connection center */

	config.buffer[idx++] = (addr) >> 24;
   config.buffer[idx++] = (addr) >> 16;
   config.buffer[idx++] = (addr) >> 8;
   config.buffer[idx++] = (addr);

   /* the rest of the buffer contains the service name to connect */

   idx += sprintf((char *) (&config.buffer[idx]), "cc:nfcc_i2c") + 1;

   config.length = idx;

   ret = ioctl(pComPort->fd, OPEN_NFC_IOC_CONFIGURE, &config);

   if (ret < 0)
   {
      PNALDebugError("Unable to perform  OPEN_NFC_IOC_CONFIGURE");
      goto return_error;
   }

   /*
    * Establish the connection with the Connection Center
    */

   ret = ioctl(pComPort->fd, OPEN_NFC_IOC_CONNECT, NULL);

   if (ret < 0)
   {
      PNALDebugError("Unable to perform  OPEN_NFC_IOC_CONNECT");
      goto return_error;
   }


   /*
    * All is fine
    */

   *pnType = P_COM_TYPE_NFCC_SHDLC_I2C;

   return pComPort;

return_error:


   if (pComPort->fd != -1)
   {
      close(pComPort->fd);
      pComPort->fd = -1;
   }

   *pnType = 0;

   return null;
}

/* See Functional Specifications Document */
void CNALComDestroy(
         tNALComInstance* pComPort )
{

   PNALDebugTrace("CNALComDestroy");

   if(pComPort != NULL)
   {
      close (pComPort->fd);
      pComPort->fd = -1;
   }
}

/* See Functional Specifications Document */
uint32_t CNALComReadBytes(
         tNALComInstance* pComPort,
         uint8_t* pReadBuffer,
         uint32_t nBufferLength)
{
   ssize_t nResult = 0;

   if(pComPort != null)
   {
      nResult = read(pComPort->fd, pReadBuffer, nBufferLength);

      if (nResult < 0)
      {
         nResult = 0;
      }
   }

   return nResult;
}

/* See Functional Specifications Document */
uint32_t CNALComWriteBytes(
         tNALComInstance* pComPort,
         uint8_t* pBuffer,
         uint32_t nBufferLength )
{
   ssize_t nResult = 0;

   if(pComPort != null)
   {
      nResult = write(pComPort->fd, pBuffer, nBufferLength);

      if ((nResult < 0) && (errno != EAGAIN))
      {
         PNALDebugError("PComWriteBytes : unable to send some data %d", errno);
         nResult = 0;
      }
   }

   return nResult;
}

/*******************************************************************************

  Reset Functions

*******************************************************************************/

/* See Functional Specifications Document */
void CNALResetNFCController(
         void* pPortingConfig,
         uint32_t nResetType )
{
   tNALComInstance* pComPort = static_PComGetInstance();
   int ret;

   PNALDebugTrace("*****************  Reseting NFC Controller *********************\n");

   if (pComPort->fd != -1)
   {
      ret = ioctl(pComPort->fd, OPEN_NFC_IOC_RESET, NULL);

      if (ret == 0)
      {
         pComPort->reset_pending = true;
      }
      else
      {
         PNALDebugError("Unable to perform  OPEN_NFC_IOC_RESET");
      }
   }

   return;

}

/* See Functional Specifications Document */
bool CNALResetIsPending( void* pPortingConfig )
{
   tNALComInstance* pComPort = static_PComGetInstance();

   if (pComPort->reset_pending)
   {
      struct timeval tv;
      fd_set fds;

      /* the reset is completed once we execptfd is set */
      FD_ZERO(&fds);
      FD_SET(pComPort->fd, &fds);

      tv.tv_sec = 0;
      tv.tv_usec = 1;

      if (select(pComPort->fd + 1, NULL, &fds, NULL, &tv) > 0)
      {
         pComPort->reset_pending = false;
      }
   }


   return pComPort->reset_pending;
}


/*
 * Thread dealing with the call of poll function of the NAL binding
 */

void * PDriverReceiveThread(void * arg)
{
   tNALInstance   * pNALInstance = static_PNALGetInstance();
   tNALComInstance    * pComPort = static_PComGetInstance();
   tNALTimerInstance  * pTimer = static_PTimerGetInstance();

   fd_set             readfds, writefds;
   int               data_fd, ctrl_fd, max_fd;

   uint32_t nTimeoutMs;
   struct timeval   tv;
   int res;
   uint8_t buffer[256];


   data_fd = pComPort->fd;
   ctrl_fd = pNALInstance->aWakeUpSockets[1];

   if (data_fd > ctrl_fd)
   {
      max_fd = data_fd + 1;
   }
   else
   {
      max_fd = ctrl_fd + 1;
   }

   for (;;)
   {
		bool bCallPoll = false;
      FD_ZERO(&readfds);
      FD_ZERO(&writefds);

      FD_SET(data_fd, &readfds);
      FD_SET(ctrl_fd, &readfds);

      if (pComPort->reset_pending)
      {
         FD_SET(data_fd, &writefds);
      }

      if (pTimer->nTimerValue != 0)
      {
         uint32_t nCurrentTime = CNALTimerGetCurrentTime(pTimer);

         if (pTimer->nTimerValue > nCurrentTime)
         {
            nTimeoutMs = pTimer->nTimerValue - nCurrentTime;

            tv.tv_sec = nTimeoutMs / 1000;
            tv.tv_usec = (nTimeoutMs - tv.tv_sec * 1000) * 1000;

            res = select(max_fd, &readfds, &writefds, NULL, &tv);
         }
         else
         {
            res = 0;
         }
      }
      else
      {
         res = select(max_fd, &readfds, &writefds, NULL, null);
      }

      if (res < 0)
      {
         if (errno != EINTR)
         {
            PNALDebugError("PDriverReceiveThread : select failed : errno %d\n", errno);
            pthread_exit((void *) -1);
         }
      }
      else if (res > 0)
      {
			if ((FD_ISSET(data_fd, &readfds)) || ((FD_ISSET(data_fd, &writefds))))
         {
            /* some data has been received */
				bCallPoll = true;
         }
			else
			{
				/* process data received on the control socket */
				int ret, i;

				ret = recv(ctrl_fd, buffer, sizeof(buffer), 0);

            if (((ret < 0) && (errno != EINTR)) || (ret == 0))
            {
               /* something goes wrong with the control socket, this typically occurs when exitin the service */

               pthread_exit((void *) -1);
            }


				for (i=0; i<ret; i++)
				{
					switch (buffer[i])
					{
						case EVENT_QUIT :
							pthread_exit((void *) -1);
							break;

						case EVENT_KICK :
							bCallPoll= true;
							break;
					}
				}
			}
      }
      else
      {
         /* select returned due to a timeout */
         CNALTimerSetExpired(pTimer);
			bCallPoll = true;
      }

		if (bCallPoll == true)
		{
			g_pNALBinding->pPollFunction(arg);
		}
   }
}


void CNALSyncTriggerEventPump(
            void* pPortingConfig )
{
	tNALInstance * pNALInstance = static_PNALGetInstance();
	uint8_t buff[1];

	buff[0] = EVENT_KICK;
	write(pNALInstance->aWakeUpSockets[0], buff, 1);
}

/* See Functional Specification Document */
tNALInstance * CNALPreCreate(void * pPortingConfig)
{
   tNALInstance * pNALInstance = static_PNALGetInstance();

	CNALMemoryFill(pNALInstance, 0, sizeof(* pNALInstance));

   if (socketpair(AF_UNIX, SOCK_STREAM, 0, pNALInstance->aWakeUpSockets))
   {
      PNALDebugError("CNALPreCreate : socketpair() failed %d", errno);

		return null;
   }

	return pNALInstance;
}


/* See Functional Specification Document */
bool CNALPostCreate(tNALInstance * pNALInstance, void * pNALVoidInstance)
{

	if (pthread_create(&pNALInstance->hProcessDriverEventsThread, NULL, PDriverReceiveThread, pNALVoidInstance))
	{
		printf("Can not create the event processing thread\n");
		return false;
	}

   return true;
}

/* See Functional Specification Document */
void CNALPreDestroy(tNALInstance * pNALInstance)
{
	if (pNALInstance->hProcessDriverEventsThread)
	{
		uint8_t buff[1];

		buff[0] = EVENT_QUIT;
		write(pNALInstance->aWakeUpSockets[0], buff, 1);
		pthread_join(pNALInstance->hProcessDriverEventsThread, NULL);
	}
}


/* See Functional Specification Document */
void CNALPostDestroy(tNALInstance * pNALInstance)
{
   if (pNALInstance)
   {
		close(pNALInstance->aWakeUpSockets[0]);
		close(pNALInstance->aWakeUpSockets[1]);
   }
}


tNALBinding  * GetNALBinding(void)
{
   return g_pNALBinding;
}

/* EOF */
