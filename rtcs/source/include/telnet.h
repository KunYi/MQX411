#ifndef __telnet_h__
#define __telnet_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the definitions for the Telnet
*   client and server.
*
*
*END************************************************************************/

/* telnet command codes */
#define TELCMD_IP       '\xf4'   /* Interrupt process */
#define TELCMD_AO       '\xf5'   /* Abort Output */
#define TELCMD_AYT      '\xf6'   /* Are You There */
#define TELCMD_EC       '\xf7'   /* Erase Char */
#define TELCMD_EL       '\xf8'   /* Erase Line */
#define TELCMD_GA       '\xf9'   /* Process ready for input */
#define TELCMD_WILL     '\xfb'
#define TELCMD_WONT     '\xfc'
#define TELCMD_DO       '\xfd'
#define TELCMD_DONT     '\xfe'
#define TELCMD_IAC      '\xff'   /* TELNET command sequence header */

/* telnet mandatory control codes */
#define TELCC_NULL      '\0'
#define TELCC_LF        '\n'
#define TELCC_CR        '\r'

/* telnet optional control codes */
#define TELCC_BELL      '\x07'   /* BELL */
#define TELCC_BS        '\x08'   /* BACKSPACE */
#define TELCC_HT        '\x09'   /* HORIZONTAL TAB */
#define TELCC_VT        '\x0b'   /* VERTICAL TAB */
#define TELCC_FF        '\x0c'   /* FORM FEED */


#define TELOPT_BINARY   0        /* Use 8 bit binary transmission */
#define TELOPT_ECHO     1        /* Echo data received */
#define TELOPT_SGA      3        /* Suppress Go-ahead signal */

#define TELNET_MAX_OPTS 4

/* Terminal options */
#define TEROPT_CRLF_MODE 1       /* New line endings style CR LF as opposed to CR NULL */
#define TEROPT_RAW_MODE  2       /* Indicates that raw mode without any char translation is used */


/* set the proper stream for the device */
#define IO_IOCTL_SET_STREAM             0x0050L
#define IO_IOCTL_SET_ECHO               0x0051L
#define IO_IOCTL_GET_ECHO               0x0052L
#define IO_IOCTL_SET_INBINARY           0x0053L
#define IO_IOCTL_GET_INBINARY           0x0054L
#define IO_IOCTL_SET_OUTBINARY          0x0055L
#define IO_IOCTL_GET_OUTBINARY          0x0056L
#define IO_IOCTL_SET_BINARY             0x0057L
#define IO_IOCTL_GET_BINARY             0x0058L
#define IO_IOCTL_SET_CRLF               0x0059L
#define IO_IOCTL_GET_CRLF               0x005aL
#define IO_IOCTL_SET_RAW                0x005bL
#define IO_IOCTL_GET_RAW                0x005cL

#endif
/* EOF */
