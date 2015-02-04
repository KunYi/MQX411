#ifndef _io_expander_prv_h_
#define _io_expander_prv_h_ 1
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This file includes the private definitions for the IO expander device drivers
*
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/


/*---------------------------------------------------------------------
**
** IO EXPANDER DEVICE STRUCT
**
** This structure used to store information about an IO expander device
** for the IO device table
*/
typedef struct io_expander_device_struct
{

   /* The I/O init function */
   uint32_t (*DEV_INIT)(void *, void **);

   /* The I/O open function */
   uint32_t (*DEV_OPEN)(void *, void **, char *);

   /* The I/O open function */
   uint32_t (*DEV_CLOSE)(void *, void *);

   /* The I/O deinit function */
   uint32_t (*DEV_DEINIT)(void *);

   /* The read function */
   int32_t  (*DEV_READ)(void *, void *, char *, int32_t);

   /* The write function */
   int32_t  (*DEV_WRITE)(void *, void *, char *, int32_t);

   /* The ioctl function, (change bauds etc) */
   int32_t  (*DEV_IOCTL)(void *, void *, uint32_t, uint32_t *);

   /* The I/O channel initialization data */
   void *               DEV_INIT_DATA_PTR;

   /* Device specific information */
   void *               DEV_INFO_PTR;

   /* Open count for number of accessing file descriptors */
   uint32_t             COUNT;

   /* Open flags for this channel */
   uint32_t             FLAGS;

} IO_EXPANDER_DEVICE_STRUCT, * IO_EXPANDER_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* I/O prototypes */
extern int32_t _io_expander_open (FILE_DEVICE_STRUCT_PTR, char *, char *);
extern int32_t _io_expander_close (FILE_DEVICE_STRUCT_PTR);
extern int32_t _io_expander_read (FILE_DEVICE_STRUCT_PTR, char *, int32_t);
extern int32_t _io_expander_write (FILE_DEVICE_STRUCT_PTR, char *, int32_t);
extern int32_t _io_expander_ioctl (FILE_DEVICE_STRUCT_PTR, uint32_t, void *);
extern int32_t _io_expander_uninstall (IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
