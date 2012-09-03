/*
 * Open NFC driver module.
 *
 */

#include <linux/ioctl.h>

 /* Set Open NFC major number.
  *
  * If set to 0, a free major number will be allocated for the driver.
  *	If set to a value different from 0, the specified value will be used as a major value
  */

#define OPEN_NFC_MAJOR			 0

/*
 * Use 'o' as magic number
 *
 * May be customized if conflict occurs
 */

#define OPEN_NFC_IOC_MAGIC  	'o'

/**
 * OPEN_NFC_IOC_CONFIGURE configuration structure
 *
 * Customer implementation may increase the OPEN_NFC_IOC_MAX_CONFIG_LENGTH if needed
 */

#define OPEN_NFC_IOC_MAX_CONFIG_LENGTH		16

struct open_nfc_ioc_configure
{
	int length;															/* actual configuration length */
	unsigned char buffer[OPEN_NFC_IOC_MAX_CONFIG_LENGTH];	/* buffer containing the config value */
};

/**
 * OPEN_NFC_IOC_CONFIGURE is used to pass configuration data to the Open NFC driver.
 *
 * In our driver implementation, this is used to pass the IP address of the connection center
 *
 * Customer implementation can use this IOCTL for any purpose
 */

#define OPEN_NFC_IOC_CONFIGURE	_IOW(OPEN_NFC_IOC_MAGIC, 0, struct open_nfc_ioc_configure)


/**
  * OPEN_NFC_IOC_CONNECT is used to request Open NFC driver to prepare to communicate with the NFC Controller.
  *
  * In our driver implementation, this is used to establish TCP connection with the connection center
  *
  * Customer implementation can use this IOCTL for any purpose
  */

#define OPEN_NFC_IOC_CONNECT	_IO(OPEN_NFC_IOC_MAGIC, 1)


/**
  * OPEN_NFC_IOC_RESET is used to request Open NFC driver to reset the NFC Controller.
  *
  * Note : The reset IOCTL is asynchronous. The Open NFC stack checks the completion of
  * the reset by selecting the fd for writing (using select()).
  */

#define OPEN_NFC_IOC_RESET    	_IO(OPEN_NFC_IOC_MAGIC, 2)


#define OPEN_NFC_MAX_IOCTL_VALUE	2

/* EOF */

