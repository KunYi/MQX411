#ifndef __usb_host_msd_bo_h__
#define __usb_host_msd_bo_h__
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
*   This file is a header file which defines all necessary structure to
*   be used for sending commands to mass storage devices.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_snd.h"
#include "host_rcv.h"
#include "host_ch9.h"

/*******************************************************************
**
** Values specific to CLEAR FEATURE commands (must go to common.h later)
*/



/*******************************************************************
**
** Mass Storage definitions
*/

/* Error codes */
#define USB_TRANSFER_CANCELLED         0xF1
#define USB_MASS_INVALID_CBW           -1
#define USB_MASS_QUEUE_FULL            -2
#define USB_MASS_NO_MATCHING_REQUEST   -4
#define USB_MASS_INVALID_INTF_HANDLE   -5
#define USB_MASS_INVALID_BUFFER        -6
#define USB_MASS_FAILED_IN_COMMAND     -7
#define USB_MASS_FAILED_IN_DATA        -8
#define USB_MASS_FAILED_IN_STATUS      -9

/* Constants */
#define MAX_CBWCB_SIZE            16
#define CBW_DIRECTION_IN         0x80
#define CBW_DIRECTION_OUT        0x00
#define MAX_RETRIAL_ATTEMPTS     1
#define QUEUE_SIZE               4
#define DIRECTION_OUT            0x00
#define DIRECTION_IN             0x80
#define MASK_NON_DIRECTION_BITS  0x80

/* Command Status Wrapper Signature 'USBS'*/
#define CBW_SIGNATURE               0x43425355
#define CSW_SIGNATURE               0x53425355
#define CSW_STATUS_GOOD             0x00
#define CSW_STATUS_FAILED           0x01
#define CSW_STATUS_PHASE_ERROR      0x02

/* some useful defines */
#define ZERO_BIT0      0xFE
#define BE_HIGH_BYTE   0    /* LSB last in UFI specs (big endian)    */
#define BE_LOW_BYTE    1    /* LSB last in UFI specs (big endian)    */
#define LE_HIGH_BYTE   1    /* LSB last in UFI specs (little endian) */
#define LE_LOW_BYTE    0    /* LSB last in UFI specs (little endian) */

/*
** high nibble in x should be 1111
** transfer the low nibble of x to low nibble of a
*/
#define TRANSFER_LOW_NIBBLE(x,a)   (a) = (uint8_t)(((x) & 0x0F) | ((a) & 0xF0))

#define CSW_SIZE  0x0C
#define CBW_SIZE  0x1F
#define GET_MAX_LUN   0xFE
#define MASS_STORAGE_RESET 0xFF


#define USB_CLASS_MASS_IS_Q_EMPTY(intf_ptr)      (bool)(intf_ptr->QUEUE.COUNT==0)


/*********************************************************************
**
** Typedefs
*/

/* UFI Typical Command Block Wrapper for Most commands */
PACKED_STRUCT_BEGIN
struct _UFI_CBWCB
{
    uint8_t      BUFIOPCODE;                 /* 0 */
    uint8_t      BUFILUN;                    /* 1 */
    uint8_t      BUFILOGICALBLOCKADDRESS[4]; /* 2,3,4,5 */
    uint8_t      RESERVED6;                  /* 6 Reserved */
    uint8_t      BLENGTH[2];                 /* 7,8 length of the data block */
    uint8_t      RESERVED9;                  /* 9 Reserved */
    uint8_t      RESERVED10;                 /* 10 Reserved */
    uint8_t      RESERVED11;                 /* 11 Reserved */
} PACKED_STRUCT_END;

typedef struct _UFI_CBWCB UFI_CBWCB_STRUCT, * UFI_CBWCB_STRUCT_PTR;

/* UFI Typical Command Block Wrapper for Extended commands */
PACKED_STRUCT_BEGIN
struct _UFI_CBWCB_EXTENDED
{
    uint8_t      BUFIOPCODE;                 /* 0 */
    uint8_t      BUFILUN;                    /* 1 */
    uint8_t      BUFILOGICALBLOCKADDRESS[4]; /* 2,3,4,5 */
    uint8_t      BLENGTH[4];                 /* 6,7,8,9 length of the data block */
    uint8_t      RESERVED10;                 /* 10 Reserved */
    uint8_t      RESERVED11;                 /* 11 Reserved */
} PACKED_STRUCT_END;

typedef struct _UFI_CBWCB_EXTENDED UFI_CBWCB_EXTENDED, * UFI_CBWCB_EXTENDED_STRUCT_PTR;


/* define a union that covers all supported protocols.  */
#if defined(__IAR_SYSTEMS_ICC__)
/* check whether packed attribute is necessary */
#pragma diag_suppress=Pe021
#endif
PACKED_UNION_BEGIN
union _cbwcb {
   UFI_CBWCB_STRUCT     UFI_CBWCB;
   UFI_CBWCB_EXTENDED   UFI_CBWCB_EXT;
   uint8_t               MAX_COMMAND[16];
} PACKED_UNION_END;

typedef union _cbwcb CBWCB, * CBWCB_PTR;

/* State machine constants of Class driver */
enum USB_CLASS_MASS_COMMAND_STATUS {
   STATUS_COMPLETED = USB_OK,
   STATUS_CANCELLED,
   STATUS_QUEUED_IN_DRIVER,
   STATUS_QUEUED_CBW_ON_USB,
   STATUS_FINISHED_CBW_ON_USB,
   STATUS_QUEUED_DPHASE_ON_USB,
   STATUS_FINISHED_DPHASE_ON_USB,
   STATUS_QUEUED_CSW_ON_USB,
   STATUS_FINISHED_CSW_ON_USB,
   STATUS_FAILED_IN_CSW,
   STATUS_RESET_BULK_IN,
   STATUS_RESET_BULK_OUT,
   STATUS_RESET_INTERFACE,
   STATUS_RESET_DEVICE,
   STATUS_CLEAR_BULK_PIPE
};

typedef enum USB_CLASS_MASS_COMMAND_STATUS USB_CLASS_MASS_COMMAND_STATUS;


/* Define the representation of a circular queue */
typedef struct _MASS_QUEUE_STRUCT
{
   void       *ELEMENTS[QUEUE_SIZE]; /* storage for the queue*/
   // CR TBD
   uint32_t     COUNT;
   uint8_t      FIRST;         /* index of the first element in the queue*/
   uint8_t      LAST;          /* index of the last element in the queue*/
} MASS_QUEUE_STRUCT, * MASS_QUEUE_STRUCT_PTR;

/* Command Block Wrapper (see USB Mass Storage specs) */
PACKED_STRUCT_BEGIN
struct _cbw_struct
{
   uint8_t      DCBWSIGNATURE[4];            /* 0-3   */
   uint8_t      DCBWTAG[4];                  /* 4-7   */
   uint8_t      DCBWDATATRANSFERLENGTH[4];   /* 8-11  */
   uint8_t      BMCBWFLAGS;                  /* 12    */
   uint8_t      BCBWLUN;                     /* 13    */
   uint8_t      BCBWCBLENGTH;                /* 14    */
   CBWCB       CMD_BLOCK;                    /* 15-31 */
}
PACKED_STRUCT_END;

typedef struct _cbw_struct CBW_STRUCT, * CBW_STRUCT_PTR;

/* Command Status Wrapper   (see USB Mass Storage specs) */
typedef struct _csw_struct
{
    uint8_t     DCSWSIGNATURE[4];         /* 0-3    */
    uint8_t     DCSWTAG[4];               /* 4-7    */
    uint8_t     DCSWDATARESIDUE[4];       /* 8-11   */
    unsigned char      BCSWSTATUS;                /* 12     */
} CSW_STRUCT, * CSW_STRUCT_PTR;


/* USB Mass Class  One Single Command Object for all protocols */
typedef struct _COMMAND_OBJECT {
   CLASS_CALL_STRUCT_PTR             CALL_PTR;      /* Class intf data pointer and key    */
   uint32_t                           LUN;           /* Logical unit number on device      */
   CBW_STRUCT_PTR                    CBW_PTR;       /* current CBW being constructed      */
   CSW_STRUCT_PTR                    CSW_PTR;       /* CSW for this command               */
   void (_CODE_PTR_                  CALLBACK)
      (USB_STATUS,                            /* status of this command                       */
       void *,                               /* pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT */
       void *,                               /* pointer to the command object                */
       uint32_t                                /* length of the data transfered if any         */
      );

   void                             *DATA_BUFFER;   /* buffer for IN/OUT for the command  */
   uint32_t                           BUFFER_LEN;    /* length of data buffer              */
   USB_CLASS_MASS_COMMAND_STATUS     STATUS;        /* current status of this command     */
   USB_CLASS_MASS_COMMAND_STATUS     PREV_STATUS;   /* previous status of this command    */
   uint32_t                           TR_BUF_LEN;    /* length of the buffer received in
                                                       currently executed TR              */
   uint8_t                            RETRY_COUNT;   /* Number of times this commad tried  */
   uint8_t                            TR_INDEX;      /* TR_INDEX of the TR used for search */
} COMMAND_OBJECT_STRUCT, * COMMAND_OBJECT_PTR;


/*
** USB Mass Class Interface structure. This structure will be passed to all
** commands to this class driver. The structure holds all information
** pertaining to an interface on storage device. This allows the class driver
** to know which interface the command is directed for.
*/

typedef struct _Usb_Mass_Intf_Struct {
   GENERAL_CLASS                       G;                 // This is general class containing following
   _usb_pipe_handle                    CONTROL_PIPE;      /* control pipe handle*/
   _usb_pipe_handle                    BULK_IN_PIPE;      /* Bulk in pipe handle*/
   _usb_pipe_handle                    BULK_OUT_PIPE;     /* Bulk out pipe handle*/
   MASS_QUEUE_STRUCT                   QUEUE;             /* structure that queues requests*/
   uint8_t                              INTERFACE_NUM;     /* interface number*/
   uint8_t                              ALTERNATE_SETTING; /* Alternate setting*/
   CLASS_CALL_STRUCT_PTR               APP;               /* Store app handle, can help finding if lost */
} USB_MASS_CLASS_INTF_STRUCT, * USB_MASS_CLASS_INTF_STRUCT_PTR;


/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_class_mass_get_app(_usb_device_instance_handle, _usb_interface_descriptor_handle, CLASS_CALL_STRUCT_PTR *);
int32_t usb_class_mass_q_insert(USB_MASS_CLASS_INTF_STRUCT_PTR, COMMAND_OBJECT_PTR);
void usb_class_mass_deleteq(USB_MASS_CLASS_INTF_STRUCT_PTR);
void usb_class_mass_get_pending_request(USB_MASS_CLASS_INTF_STRUCT_PTR, COMMAND_OBJECT_PTR *);
void usb_class_mass_q_init(USB_MASS_CLASS_INTF_STRUCT_PTR);
void usb_class_mass_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
void usb_class_mass_exit(void);
USB_STATUS usb_class_mass_storage_device_command(CLASS_CALL_STRUCT_PTR, COMMAND_OBJECT_PTR);
bool usb_class_mass_storage_device_command_cancel(CLASS_CALL_STRUCT_PTR, COMMAND_OBJECT_PTR);
USB_STATUS usb_class_mass_getmaxlun_bulkonly(CLASS_CALL_STRUCT_PTR, uint8_t *, tr_callback, void *);
USB_STATUS usb_class_mass_reset_recovery_on_usb(USB_MASS_CLASS_INTF_STRUCT_PTR);
bool usb_class_mass_cancelq(USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr, COMMAND_OBJECT_PTR pCmd);

#ifdef __cplusplus
}
#endif

#endif
