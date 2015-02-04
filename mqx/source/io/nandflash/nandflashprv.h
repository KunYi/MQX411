#ifndef _nandflashprv_h_
#define _nandflashprv_h_
/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   The file contains functions prototype, defines, structure 
*   definitions private to the NAND flash driver.
*
*
*END************************************************************************/


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/


/* Properties of device */
#define IO_NANDFLASH_ATTRIBS  (IO_DEV_ATTR_READ  | IO_DEV_ATTR_SEEK | \
                               IO_DEV_ATTR_WRITE | IO_DEV_ATTR_BLOCK_MODE)


/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/



/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* These are from nandflash.c */
extern _mqx_int  _io_nandflash_open(MQX_FILE_PTR, char *, char *);
extern _mqx_int  _io_nandflash_close(MQX_FILE_PTR);
extern _mqx_int  _io_nandflash_read (MQX_FILE_PTR, char *, _mqx_int);
extern _mqx_int  _io_nandflash_write(MQX_FILE_PTR, char *, _mqx_int);
extern _mqx_int  _io_nandflash_ioctl(MQX_FILE_PTR, _mqx_uint, void *);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
