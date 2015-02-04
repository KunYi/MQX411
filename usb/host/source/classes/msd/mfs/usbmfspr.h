#ifndef _usbmfspr_h_
#define _usbmfspr_h_
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   definitions private to the USB mass storage link driver
*   to MFS.
*
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* Error codes from lower layers */
#define USB_MFS_DEFAULT_SECTOR_SIZE  (512)

/*----------------------------------------------------------------------*/
/*
**                          ERROR CODES
*/

/* Error codes from lower layers */
//#define USB_MFS_NO_ERROR             MQX_OK
//#define USB_MFS_READ_ERROR           IO_ERROR_READ
//#define USB_MFS_WRITE_ERROR          IO_ERROR_WRITE
//#define USB_MFS_INVALID_SECTOR       IO_ERROR_SEEK

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/


/*
** USB_MFS_INFO_STRUCT
** Run time state information for each USB mass storage device
*/
typedef struct io_usb_mfs_info_struct
{
   COMMAND_OBJECT_STRUCT      COMMAND;    // MUST BE FIRST
#if PSP_HAS_DATA_CACHE
   unsigned char                      FILLER1[CACHE_LINE_FILLER(sizeof(COMMAND_OBJECT_STRUCT))];
#endif
   CBW_STRUCT                 CBW;
#if PSP_HAS_DATA_CACHE
   unsigned char                      FILLER2[CACHE_LINE_FILLER(sizeof(CBW_STRUCT))];
#endif
   CSW_STRUCT                 CSW;
#if PSP_HAS_DATA_CACHE
   unsigned char                      FILLER3[CACHE_LINE_FILLER(sizeof(CSW_STRUCT))];
#endif
   INQUIRY_DATA_FORMAT        INQUIRY_DATA;
#if PSP_HAS_DATA_CACHE
   unsigned char                      FILLER4[CACHE_LINE_FILLER(sizeof(INQUIRY_DATA_FORMAT))];
#endif
  MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO   CAPACITY;
#if PSP_HAS_DATA_CACHE
   unsigned char                      FILLER5[CACHE_LINE_FILLER(sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO))];
#endif
   REQ_SENSE_DATA_FORMAT      SENSE;
#if PSP_HAS_DATA_CACHE
   unsigned char                      FILLER6[CACHE_LINE_FILLER(sizeof(REQ_SENSE_DATA_FORMAT))];
#endif
   /* Handle for mass storage class  calls */
   void         *MSC_STREAM;
   
   /* 8 bytes of ASCI Data identifying the vendor of the product */
   
   /* 16 bytes of ASCI Data defined by the vendor */
  
   /* 4 bytes of ASCI Data defined by the vendor */
   
   /* CBW tag used for commands */
   uint32_t       CBW_TAG;         
   
   /* Drive number to associate with this slot */
   uint8_t        LUN;
   
   uint32_t       BLENGTH;   // logic block length
   uint32_t       BCOUNT;    // logic blocks count

   /* Total size of Drive in bytes */
   uint32_t       SIZE_BYTES; 

   /* The number of heads as reported  */
   uint32_t       NUMBER_OF_HEADS;          

   /* The number of tracks as reported  */
   uint32_t       NUMBER_OF_TRACKS;

   /* The number of sectos per cylinder as reported */
   uint32_t       SECTORS_PER_TRACK;

   /* Light weight semaphore struct */
   LWSEM_STRUCT  LWSEM;
   
   /* The address of temp buffer */

   /* The current error code for the device */
   uint32_t       ERROR_CODE;
   
   /* Start CR 812 */
   /* Indicates if the device is running in block mode or character mode */
   bool       BLOCK_MODE;
   /* End   CR 812 */

   uint32_t       COMMAND_STATUS;
   LWSEM_STRUCT  COMMAND_DONE;
} IO_USB_MFS_STRUCT, * IO_USB_MFS_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

#if MQX_USE_IO_OLD

int _io_usb_mfs_init(char *identifier, void *init_data, void **dev_context);

_mqx_int _io_usb_mfs_open(MQX_FILE_PTR, char *, char *);
_mqx_int _io_usb_mfs_close(MQX_FILE_PTR);
_mqx_int _io_usb_mfs_read (MQX_FILE_PTR, char *, int32_t);
_mqx_int _io_usb_mfs_write(MQX_FILE_PTR, char *, int32_t);
_mqx_int _io_usb_mfs_ioctl(MQX_FILE_PTR, int32_t, void *);
int32_t _io_usb_mfs_uninstall(IO_DEVICE_STRUCT_PTR  io_dev_ptr);

#else
int _io_usb_mfs_init(void *init_data, void **dev_context);
int _io_usb_mfs_open(void *dev_context, const char *dev_name, int flags, void **fp_context);
int _io_usb_mfs_close(void *dev_context, void *fp_context);
int _io_usb_mfs_read(void *dev_context, void *fp_context, void *data_ptr, size_t num);
int _io_usb_mfs_write(void *dev_context, void *fp_context, const void *data_ptr, size_t num);
int _io_usb_mfs_ioctl(void *dev_context, void *fp_context, unsigned long int command, va_list ap);
off_t _io_usb_mfs_seek(void *dev_context, void *fp_context, off_t offset, int mode);
int _io_usb_mfs_deinit(void *dev_context);

#endif //MQX_USE_IO_OLD

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
