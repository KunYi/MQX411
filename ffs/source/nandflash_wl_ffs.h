#ifndef _nandflash_wl_ffs_h_
#define _nandflash_wl_ffs_h_
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
*   definitions specific for the NAND flash driver.
*
*
*END************************************************************************/

#include "nandflash_wl.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/
#define NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK 2000
#define NANDFLASH_1ST_DATA_DRIVE_START_BLOCK 10

/*
** NAND Flash WL IOCTL calls
*/

#define NANDFLASH_IOCTL_WL_NR_START_BASE            _IOC_NR(_IO(IO_TYPE_NANDFLASH,0x11))

#define NANDFLASH_IOCTL_GET_PAGE_METADATA_AREA_SIZE _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 1)

#define NANDFLASH_IOCTL_READ_METADATA               _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 2)

#define NANDFLASH_IOCTL_READ_WITH_METADATA          _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 3)

#define NANDFLASH_IOCTL_WRITE_WITH_METADATA         _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 4)

#define NANDFLASH_IOCTL_WRITE_RAW                   _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 5)

#define NANDFLASH_IOCTL_READ_RAW                    _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 6)

#define NANDFLASH_IOCTL_REPAIR                      _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 7)

#define NANDFLASH_IOCTL_REPAIR_WITH_BAD_SCAN        _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 8)

#define NANDFLASH_IOCTL_ERASE                       _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 9)

#define NANDFLASH_IOCTL_FLUSH_MEDIA                 _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 10)

#define NANDFLASH_IOCTL_WL_END_BASE                 _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_NR_START_BASE + 11)

/*----------------------------------------------------------------------*/
/*
**                          TYPE DEFINITIONS
*/
#define NANDWL_OPEN_NORMAL NULL
#define NANDWL_OPEN_IGNORE_INIT_MEDIA "I"
#define NANDWL_OPEN_FORCE_REPAIR_WHEN_FAILED "R"

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/


#ifdef __cplusplus
extern "C" {
#endif

    _mqx_uint _io_nandflash_wl_install(NANDFLASH_WL_INIT_STRUCT_CPTR, char *);
    _mqx_int  _io_nandflash_wl_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif /* _nandflash_wl_ffs_h_ */
/* EOF */
