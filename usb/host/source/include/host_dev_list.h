#ifndef __host_dev_list_h__
#define __host_dev_list_h__
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
*   This file defines the device list for USB class drivers.
*   +--------+                           
*   \  ROOT  \---+                       List of devices, one entry  
*   +--------+   \    +--------+           for each attached device
*                +--->\  NEXT  \---+
*                     \--------\   \    +--------+
*                  +--\ MEMLST \   +--->\  NEXT  \---+
*                  \  \--------\        \--------\   \    +--------+
*   Fixed-size     \  \ Device \     +--\ MEMLST \   +--->\  NULL  \
*     per-device   \  \ struct \     \  \--------\        \--------\
*     memory       \  +--------+     \  \ Device \     +--\ MEMLST \
*                  \                 \  \ struct \     \  \--------\
*                  \                 \  +--------+     \  \ Device \
*                  \                 \                 \  \ struct \
*                  +->+--------+     \                 \  +--------+
*   Lists of other    \  NEXT  \     \                 \
*     memory per   +--\--------\     +->+--------+     \
*     device --    \  \ LENGTH \        \  NULL  \     \
*     e.g. pipes,  \  \--------\        \--------\     +->+--------+
*     buffers,     \  \ Device \        \ LENGTH \        \  NULL  \
*     structs      \  \ malloc \        \--------\        \--------\
*                  \  \ memory \        \ Device \        \ LENGTH \
*                  \  +--------+        \ malloc \        \--------\
*                  \                    \ memory \        \ Device \
*                  \                    +--------+        \ malloc \
*                  +->+--------+                          \ memory \
*                     \  NULL  \                          +--------+
*                     \--------\
*                     \ LENGTH \  Length used to check
*                     \--------\    buffer overrun etc.
*                     \ Device \
*                     \ malloc \
*                     \ memory \
*                     +--------+
*   IMPLEMENTATION:
*   Linking list items depends on root pointers behaving like
*   a short entry containing a "next" pointer = NULL when
*   the list is empty.  So next MUST BE FIRST in the structs.
*
*
*END************************************************************************/
#include "host_common.h"

#if PSP_HAS_DATA_CACHE
#include "ehci_cache.h" //for USB_DMA_ALIGN
#endif

#define  MAX_DEV_INTFS  (32)

/* Types of malloc memory used with USB devices */
enum memory_type 
{
   USB_MEMTYPE_MIN_VALUE,  /* lowest value - 1     */
   USB_MEMTYPE_CONFIG,     /* device configuration */
   USB_MEMTYPE_INTERFACE,  /* Interface descr */
   USB_MEMTYPE_PIPES,      /* pipe bundle          */
   USB_MEMTYPE_BUFFER,     /* buffer               */
   USB_MEMTYPE_CLASS,      /* class driver struct  */
   USB_MEMTYPE_MAX_VALUE   /* highest value + 1    */
};
typedef enum memory_type memory_type;

typedef struct dev_memory
{
   struct dev_memory      *next;    /* next memory block or NULL */
   uint32_t                 blktype; /* type of data, block usage */
   uint32_t                 blksize; /* size of data area following */
   /* We store offset of payload to determine real payload location. Offset is needed for alignment.
   ** Since there is also the same byte written just before payload, the offset must be 1B wide.
   ** That determines maximum alignment of 256 Bytes.
   ** Note that this internal method to achieve alignment is not memory friendly, because it inserts
   ** dummy bytes and allocates more memory than needed
   */
   uint8_t                  filler[3]; /* since offset MUST be 1B long, we have to put 3 bytes */
   uint8_t                  offset;  /* number of bytes inserted after this item when payload starts */
    
   /* Note that normally here are bytes inserted to achieve requested alignement.
   ** Do not use address of payload directly, but always add offset bytes to get the address of payload.
   */
   union {   
      uint8_t         data[1];    /* actually [0 ... blksize-1] */
      unsigned char          bufr[1];    /* unsigned chars [0 ... blksize-1] */
      uint32_t        word[1];    /* general-purpose array */
      void          *bundle[1];  /* pipe bundle */
   } payload;                    /* payload of data */
} DEV_MEMORY, * DEV_MEMORY_PTR;
#define  MEM_HEADER_LEN     (16)

/********************************************************************
Note that device instance structure keeps data buffers inside it. 
These buffers are passed to DMA when host does enumeration. This
means that we must ensure that buffers inside this stucture
are aligned in PSP cache line size. 
********************************************************************/

/* Fixed-length fields applicable to all devices */
typedef struct dev_instance
{
    struct dev_instance       *next;             /* next device, or NULL */
    DEV_MEMORY_PTR             memlist;          /* list of malloc'd memory */
    _usb_host_handle           host;             /* host (several can exist) */
    uint8_t                     speed;            /* device speed */
    uint8_t                     hub_no;           /* hub # (root hub = 0) */
    uint8_t                     port_no;          /* hub's port # (1 - 8) */
    uint8_t                     address;          /* USB address, 1 - 127 */

    uint16_t                    cfg_value;
    uint8_t                     ctrl_retries;
    uint8_t                     new_config;       /* non-zero = new config */

    uint8_t                     num_of_interfaces;
    uint8_t                     RESERVED0[1];

    uint16_t                    state;            /* device state */
    _usb_pipe_handle           control_pipe;     /* control pipe handle */
    tr_callback                control_callback; /* control pipe callback */
    void                      *control_callback_param; /* control callback param */

#if USB_DMA_ALIGN
    /***********************************************************
    40 bytes have been used above. Device descriptor must be
    aligned on a cache boundary so we must ensure that there
    is enough padding before it.
    ***********************************************************/
    
    uint8_t                     RESERVED1[USB_DMA_ALIGN(40) - 40];
#endif // USB_DMA_ALIGN
   
    DEVICE_DESCRIPTOR          dev_descriptor;   /* device descriptor */

#if USB_DMA_ALIGN
    uint8_t                     RESERVED2[USB_DMA_ALIGN(sizeof(DEVICE_DESCRIPTOR)) - sizeof(DEVICE_DESCRIPTOR)];   // complete padding
#endif // USB_DMA_ALIGN

    unsigned char                      buffer[9];        /* enumeration buffer */
   
#if USB_DMA_ALIGN
    uint8_t                     RESERVED3[USB_DMA_ALIGN(9) - 9];
#endif // USB_DMA_ALIGN

   /* Interface/s which have been selected (pipes are open) */
   INTERFACE_DESCRIPTOR_PTR   intf_descriptor[MAX_DEV_INTFS];

} DEV_INSTANCE, * DEV_INSTANCE_PTR;

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_dev_list_free_memlist (_usb_device_instance_handle);
USB_STATUS usb_dev_list_free_mem (DEV_INSTANCE_PTR, DEV_MEMORY_PTR);
USB_STATUS usb_dev_list_get_mem (DEV_INSTANCE_PTR, uint32_t, memory_type, uint32_t align, DEV_MEMORY_PTR *);
USB_STATUS usb_dev_list_attach_device (_usb_host_handle, uint8_t, uint8_t, uint8_t);
void usb_dev_list_close_pipe_bundle (PIPE_BUNDLE_STRUCT_PTR);
void usb_dev_list_detach_device (_usb_host_handle, uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
