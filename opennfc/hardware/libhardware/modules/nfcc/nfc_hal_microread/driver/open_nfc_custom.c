/* -------------------------------------------------------------------------
 * Copyright (c) 2010 Inside Secure, All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ------------------------------------------------------------------------- */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/inet.h>

#include <asm/uaccess.h>

#include "open_nfc_main.h"
#include "open_nfc_ccclient.h"

#define P_MAX_RX_BUFFER_SIZE  128
#define P_COM_RESET_FLAG  0x00
#define P_COM_DATA_FLAG   0x01

struct open_nfc_custom_dev
{
   /* configuration stuff */

   bool        configured;    /* set to true if OPEN_NFC_IOC_CONFIGURE has been done */
   uint32_t    cc_address;    /* connection center address */
   tchar       cc_uri[OPEN_NFC_IOC_MAX_CONFIG_LENGTH-4];

   /* connection stuff */
   void      * cc_instance;   /* connection context */

   /* lock */

   struct mutex   mutex;             /* mutex for concurrency */

   uint8_t     rx_buffer[P_MAX_RX_BUFFER_SIZE];
   uint8_t *   p_rx_data;
   int         nb_available_bytes;
   bool        initial_reset_done;
   int         reset_pending;

   struct semaphore 	receive_sem;
   wait_queue_head_t read_queue;
   wait_queue_head_t	write_queue;

   struct task_struct * receive_thread;
};


/**
  * Function called when the user opens /dev/nfcc
  *
  * @return 0 on success, a negative value on failure.
  */
int open_nfc_custom_open(struct inode *inode, struct file *filp)
{
   struct open_nfc_custom_dev * custom_dev;

   /* allocate a custom device structure and initialize it */

   custom_dev = kmalloc(sizeof(struct open_nfc_custom_dev), GFP_KERNEL);

   if (custom_dev == NULL) {
      return -ENOMEM;
   }

   memset(custom_dev, 0, sizeof(struct open_nfc_custom_dev));

   sema_init(&custom_dev->receive_sem, 0);

   init_waitqueue_head(&custom_dev->read_queue);
   init_waitqueue_head(&custom_dev->write_queue);

   mutex_init (&custom_dev->mutex);

   filp->private_data = custom_dev;

   return 0;
}

/**
  * Function called when the user closes file
  *
  * @return 0 on success, a negative value on failure.
  */
int open_nfc_custom_release(struct inode *inode, struct file *filp)
{
   struct open_nfc_custom_dev * custom_dev = filp->private_data;

   mutex_lock(&custom_dev->mutex);

   if (custom_dev->cc_instance)
   {
      printk(KERN_DEBUG "open_nfc_custom_release : CCClientDisconnect\n");

      CCClientDisconnect(custom_dev->cc_instance);
   }

   printk(KERN_DEBUG "open_nfc_custom_release : up(receive sem)\n");

   up(&custom_dev->receive_sem);

   if (custom_dev->receive_thread)
   {
      printk(KERN_DEBUG "open_nfc_custom_release : kthread_stop\n");

		mutex_unlock(&custom_dev->mutex);

      kthread_stop(custom_dev->receive_thread);

		mutex_lock(&custom_dev->mutex);
   }

   printk(KERN_DEBUG "open_nfc_custom_release : kfree(custom_dev)\n");

   CCClientClose(custom_dev->cc_instance);
   custom_dev->cc_instance = NULL;

   mutex_unlock(&custom_dev->mutex);

   /* free the custom device context */
   kfree (custom_dev);

   return 0;
}

/**
  * Function called when the user reads data
  *
  * @return 0 on success, a negative value on failure.
  */
ssize_t open_nfc_custom_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
   struct open_nfc_custom_dev * custom_dev =filp->private_data;
   ssize_t retval;

   /* we allow read only if the connection with the connection center has been established */
   mutex_lock(&custom_dev->mutex);

   if (custom_dev->cc_instance == NULL)
   {
      printk(KERN_ERR "open_nfc_custom_read : not connected\n");

      retval =  -ENOTCONN;
      goto end;
   }

   retval = custom_dev->nb_available_bytes;

   if (retval == 0)
   {
      /* no data available */
      retval =  -EAGAIN;
      goto end;
   }

   if (count >= retval)
   {
      if (copy_to_user(buf, custom_dev->p_rx_data, custom_dev->nb_available_bytes))
      {
         printk(KERN_ERR "open_nfc_custom_read : unable to access to user buffer. data lost\n");
         retval = -EFAULT;
      }
   }
   else
   {
      printk(KERN_ERR "open_nfc_custom_read : provided buffer too short. data lost\n");
      retval = 0;
   }

   custom_dev->nb_available_bytes = 0;

	/* increase the semaphore to wake up receive thread for next data reception */
   up(&custom_dev->receive_sem);


end:
   mutex_unlock(&custom_dev->mutex);

   return retval;
}

/**
  * Function called when the user writes data
  *
  * @return 0 on success, a negative value on failure.
  */

ssize_t open_nfc_custom_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
   struct open_nfc_custom_dev * custom_dev = filp->private_data;
   void * temp;
   int retval;

   mutex_lock(&custom_dev->mutex);

   if (custom_dev->cc_instance == NULL)
   {
      printk(KERN_ERR "open_nfc_custom_write  : not connected\n");

      retval = -ENOTCONN;
      goto end;
   }

   /* allocate a temporary kernel buffer to store user data */
   temp = kmalloc(count, GFP_KERNEL);

   if (temp == NULL)
   {
      printk(KERN_ERR "open_nfc_custom_write  : kmalloc failed\n");

      /* no memory available... */
      retval = -ENOMEM;
      goto end;
   }

   if (copy_from_user(temp, buf, count) == 0)
   {
      if (CCClientSendDataEx(custom_dev->cc_instance, P_COM_DATA_FLAG, buf, count) != 0)
      {
         retval = count;
      }
      else
      {
			printk(KERN_ERR "open_nfc_custom_write : CCClientSendDataEx() failed");
         retval = 0;
      }
   }
   else
   {
      printk(KERN_ERR "open_nfc_custom_write  : copy_from_user failed\n");

      retval = -EFAULT;
   }

   /* free the allocated buffer */
   kfree(temp);

end :

   mutex_unlock(&custom_dev->mutex);

   return retval;
}

/**
  * Processes the OPEN_NFC_IOC_CONFIGURE ioctl
  *
  * @return 0 on success, a negative value on failure.
  */

const char uri[] = "cc:nfcc_i2c";

int open_nfc_custom_ioctl_configure(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
   struct open_nfc_custom_dev * custom_dev = filp->private_data;
   struct open_nfc_ioc_configure config;
   int retval;
   int i;

	mutex_lock(&custom_dev->mutex);

	if (custom_dev->cc_instance != NULL)
	{
		printk(KERN_ERR "OPEN_NFC_IOC_CONFIGURE is not allowed after OPEN_NFC_IOC_CONNECT");
		retval = -EISCONN;
		goto end;
	}

   if (copy_from_user(&config, (void *) arg, sizeof(config)))
   {
      printk(KERN_ERR "open_nfc_custom_ioctl_configure : copy_from_user failed\n");

      retval = -EFAULT;
      goto end;
   }

   /* config.length contains the number of byte used if the config.buffer array : here should be >= 4
    * config.buffer[0] - config.buffer[3] contain the IP address of the connection center
    */

   if ((config.length < 4) || (config.length > OPEN_NFC_IOC_MAX_CONFIG_LENGTH))
   {
      printk(KERN_ERR "open_nfc_custom_ioctl_configure : invalid IOCTL format\n");

      retval = -EINVAL;
      goto end;
   }

   custom_dev->cc_address = config.buffer[0];
   custom_dev->cc_address = (uint32_t) (custom_dev->cc_address << 8) | config.buffer[1];
   custom_dev->cc_address = (uint32_t) (custom_dev->cc_address << 8) | config.buffer[2];
   custom_dev->cc_address = (uint32_t) (custom_dev->cc_address << 8) | config.buffer[3];

	printk(KERN_DEBUG "open_nfc_custom_ioctl_configure : cc_address %08x\n", custom_dev->cc_address);

#if 0 /* @todo */
   for (i=0; i<config.length - 4; i++)
   {
      custom_dev->cc_uri[i] = config.buffer[4+i];
	}
#endif


	for (i=0; i<strlen(uri); i++)
	{
		custom_dev->cc_uri[i] = uri[i];
	}

	custom_dev->cc_uri[i] = 0;

	custom_dev->configured = true;

   mutex_unlock(&custom_dev->mutex);

	retval = 0;

end:

   printk(KERN_DEBUG "open_nfc_custom_ioctl_configure : return %d\n", retval);

   return retval;
}


static int open_nfc_custom_cc_receive_thread (void * arg)
{
   struct open_nfc_custom_dev * custom_dev = (struct open_nfc_custom_dev *) arg;
   int result;
   bool wait = false;
	bool failure = false;

   printk(KERN_DEBUG "open_nfc_custom_cc_receive_thread : started\n");

   while (kthread_should_stop() == false)
   {
		result = CCClientReceiveData(custom_dev->cc_instance, custom_dev->rx_buffer,  P_MAX_RX_BUFFER_SIZE, & custom_dev->p_rx_data, true);

		mutex_lock(&custom_dev->mutex);

		if(result > 0)
		{
			if(custom_dev->initial_reset_done)
			{
				if(custom_dev->p_rx_data[0] == 0) /* Answer to reset? */
				{
					printk(KERN_DEBUG "open_nfc_custom_cc_receive_thread : Reset confirmation received\n");

					if(custom_dev->reset_pending > 0)
					{
						custom_dev->reset_pending--;

						if (custom_dev->reset_pending == 0)
						{
							wake_up(&custom_dev->write_queue);
						}
					}
					else
					{
						printk(KERN_ERR "open_nfc_custom_cc_receive_thread : An error occured in the protocol with the NFCC\n");
					}
				}
				else
				{
					if(custom_dev->reset_pending == 0)
					{
						custom_dev->nb_available_bytes = result - 1;
						custom_dev->p_rx_data++;

						/* wake up poll() */

						wake_up(&custom_dev->read_queue);

						/* must wait for the consumption of the data */
						wait = true;
					}
				}
			}
			else
			{
				printk(KERN_DEBUG "discarding data received prior reset acknowledge\n");
				result = 0;
			}
		}
		else
		{
			printk(KERN_ERR "open_nfc_custom_cc_receive_thread : CCClientReceiveData failure\n");

			failure = true;
			/* wake up poll() */
			wake_up(&custom_dev->read_queue);
		}

		mutex_unlock(&custom_dev->mutex);

		if (wait || failure)
		{
			/* wait until the received data has been read or the termination of the thread has been requested */
			if (down_interruptible(&custom_dev->receive_sem) == -EINTR)
			{
				printk(KERN_ERR "down_interruptible failed (due to reception of a signal)");
			}

			wait = false;
		}
   }

   return 0;
}


/**
  * Processes the OPEN_NFC_IOC_CONNECT ioctl
  *
  * @return 0 on success, a negative value on failure.
  */

int open_nfc_custom_ioctl_connect(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
   struct open_nfc_custom_dev * custom_dev = filp->private_data;
   uint32_t error;
   int retval;

   mutex_lock(&custom_dev->mutex);

	if (custom_dev->cc_instance != NULL)
	{
		printk("open_nfc_custom_ioctl_connect : already connected");

		retval = -EISCONN;
		goto end;
	}

   custom_dev->cc_instance = CCClientOpen(custom_dev->cc_address, custom_dev->cc_uri, false, & error);

   if (custom_dev->cc_instance == NULL)
   {
      printk(KERN_ERR "open_nfc_custom_ioctl_connect : CCClientOpen failed\n");

      /* the connection failed, return a negative value containing the error code */
      retval = -error;
      goto end;
   }

   /* the connection with the Connection Center has been established,
      start the receive thread */

   custom_dev->receive_thread = kthread_run(open_nfc_custom_cc_receive_thread,  custom_dev, "OpenNFCRx");

   if (custom_dev->receive_thread == ERR_PTR(-ENOMEM))
   {
      printk(KERN_ERR "open_nfc_custom_ioctl_connect : kthread_run() failed\n");

      custom_dev->receive_thread = NULL;

      retval = -ENOMEM;
      goto end;
   }

   retval = 0;

end:
   mutex_unlock(&custom_dev->mutex);

   return retval;;
}


/**
  * Processes the OPEN_NFC_IOC_RESET ioctl
  *
  * @return 0 on success, a negative value on failure.
  */

int open_nfc_custom_ioctl_reset(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
   struct open_nfc_custom_dev * custom_dev = filp->private_data;
   unsigned char reset_type = arg;
   int retval;

   mutex_lock(&custom_dev->mutex);

   if (custom_dev->cc_instance == NULL)
   {
      printk(KERN_ERR "open_nfc_custom_ioctl_reset : not connected\n");

      retval = -ENOTCONN;
      goto end;
   }

   if (CCClientSendDataEx(custom_dev->cc_instance, P_COM_RESET_FLAG, &reset_type, 1) == 0)
   {
      printk(KERN_ERR "open_nfc_custom_ioctl_reset : CCClientSendDataEx failed\n");

      retval = -ENOTCONN;
      goto end;
   }

   custom_dev->initial_reset_done = true;
   custom_dev->reset_pending++;

	retval = 0;

end:

   mutex_unlock(&custom_dev->mutex);

   return retval;
}

/**
  * Process the poll()
  *
  * @return the poll status
  */

unsigned int open_nfc_custom_poll(struct file *filp, poll_table *wait)
{
   struct open_nfc_custom_dev * custom_dev = filp->private_data;
   unsigned int mask = 0;

   mutex_lock(&custom_dev->mutex);

   poll_wait(filp, &custom_dev->read_queue, wait);
   poll_wait(filp, &custom_dev->write_queue, wait);

   if (custom_dev->reset_pending == 0)
   {
      mask = POLLOUT | POLLWRNORM;
   }

   if (custom_dev->nb_available_bytes)
   {
      mask |= POLLIN | POLLRDNORM;
   }

   mutex_unlock(&custom_dev->mutex);

   return mask;
}

/**
  * Specific initialization, when driver module is inserted.
  *
  * @return 0 if successfull, or error code
  */
int open_nfc_custom_init(void)
{
   return 0;
}

/**
  * Specific cleanup, when driver module is removed.
  *
  * @return void
  */
void open_nfc_custom_exit(void)
{
}

/* EOF */
