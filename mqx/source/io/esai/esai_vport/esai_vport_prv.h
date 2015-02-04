#ifndef _esai_vport_prv_h_
#define _esai_vport_prv_h_ 1
/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This file includes the private definitions for the ESAI
*   VPORT I/O drivers.
*
*
*END************************************************************************/
#include "esai_fifo_prv.h"

#define PCMM_HANDLE_ESAI_VALID 0x45534149

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/
struct esai_vport_pcmm_funcs_struct;

/*---------------------------------------------------------------------
**
** IO ESAI DEVICE STRUCT
**
** This structure used to store information about an ESAI io device
** for the IO device table
*/
typedef struct io_esai_vport_device_struct
{

   /* The I/O init function */
   _mqx_uint (_CODE_PTR_ DEV_INIT)(void *, void * *, char *, void *);

   /* The I/O deinit function */
   _mqx_uint (_CODE_PTR_ DEV_DEINIT)(void *, void *);

   /* The read function */
   _mqx_int  (_CODE_PTR_ DEV_READ)(void *, char *, _mqx_int);

   /* The write function */
   _mqx_int  (_CODE_PTR_ DEV_WRITE)(void *, char *, _mqx_int);

   /* The ioctl function, (change bauds etc) */
   _mqx_int  (_CODE_PTR_ DEV_IOCTL)(void *, _mqx_uint, _mqx_uint_ptr);

   /* The I/O channel initialization data */
   void *               DEV_INIT_DATA_PTR;

   /* Device specific information */
   void *               DEV_INFO_PTR;

   /* Open count for number of accessing file descriptors */
   _mqx_uint             COUNT;

   /* Open flags for this channel */
   _mqx_uint             FLAGS;

   /* handle for pcm manger using */
   uint32_t               PCMM_DRV_HANDLE;

   /* context pointer for ESAI FIFO node*/
   ESAI_FIFO_CONTEXT_STRUCT_PTR FIFO_CONTEXT_PTR;

   /* functions for pcm manger using */
   struct esai_vport_pcmm_funcs_struct  *PCMM_DRV_FUNCS;

} IO_ESAI_VPORT_DEVICE_STRUCT, * IO_ESAI_VPORT_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* I/O prototypes */
extern _mqx_int _io_esai_vport_open (FILE_DEVICE_STRUCT_PTR, char *, char *);
extern _mqx_int _io_esai_vport_close (FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_esai_vport_read (FILE_DEVICE_STRUCT_PTR, char *, _mqx_int);
extern _mqx_int _io_esai_vport_write (FILE_DEVICE_STRUCT_PTR, char *, _mqx_int);
extern _mqx_int _io_esai_vport_ioctl (FILE_DEVICE_STRUCT_PTR, _mqx_uint, void *);
extern _mqx_int _io_esai_vport_uninstall (IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
