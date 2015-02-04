#ifndef _asrc_prv_h_
#define _asrc_prv_h_ 1
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
*   This file includes the private definitions for the asrc device drivers
*
*
*END************************************************************************/
#include <mutex.h>


#define PCMM_HANDLE_ASRC_VALID 0x41535243
#define PCMM_HANDLE_ASRC_SET_NODE_VALID 0x41534E4F

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

struct asrc_pcmm_funcs_struct;


/*---------------------------------------------------------------------
**
** ASRC DEVICE STRUCT
**
** This structure used to store information about an IO expander device
** for the IO device table
*/
typedef struct io_asrc_device_struct
{

   /* The I/O init function */
   _mqx_uint (_CODE_PTR_ DEV_INIT)(void *, void * *);

   /* The I/O open function */
   _mqx_uint (_CODE_PTR_ DEV_OPEN)(void *, void *, char *);

   /* The I/O open function */
   _mqx_uint (_CODE_PTR_ DEV_CLOSE)(void *, void *);

   /* The I/O deinit function */
   _mqx_uint (_CODE_PTR_ DEV_DEINIT)(void *);

   /* The ioctl function, (change bauds etc) */
   _mqx_int  (_CODE_PTR_ DEV_IOCTL)(void *, void *, _mqx_uint, _mqx_uint_ptr);

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

   /* mutex used to sychronize the access to ASRC device*/
   MUTEX_STRUCT          DEV_MUTEX;

   /* void * to asrc pcmmgr providing functions*/
   struct asrc_pcmm_funcs_struct * PCMM_DRV_FUNCS_PTR;

} IO_ASRC_DEVICE_STRUCT, * IO_ASRC_DEVICE_STRUCT_PTR;

/*
** The ASRC pcmmgr access handle structure
** Holds the handle for pcmmgr access and a void * to ASRC device structure
*/
typedef struct io_asrc_access_hdl_struct
{
    /* The void * to ASRC device structure*/
    IO_ASRC_DEVICE_STRUCT_PTR ASRC_DEV_PTR;

    /* The variable to store the pcmmgr access handle*/
    uint32_t                   PCMM_ACCESS_HANDLE;

}IO_ASRC_ACCESS_HDL_STRUCT, * IO_ASRC_ACCESS_HDL_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* I/O prototypes */
extern _mqx_int _io_asrc_open (FILE_DEVICE_STRUCT_PTR, char *, char *);
extern _mqx_int _io_asrc_close (FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_asrc_ioctl (FILE_DEVICE_STRUCT_PTR, _mqx_uint, void *);
extern _mqx_int _io_asrc_uninstall (IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
