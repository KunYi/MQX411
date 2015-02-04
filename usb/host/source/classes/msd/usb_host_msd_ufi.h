#ifndef __usb_host_msd_ufi_h__
#define __usb_host_msd_ufi_h__
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
*   This file is a supporting header file for UFI.c which implements the UFI
*   command set for mass storage applications
*
*
*END************************************************************************/

#include "usb_host_msd_bo.h"

/* UFI specific error codes */
#define INVALID_UFI_REQUEST               (-1)

/* a should be two byte number */
#define HIGH_BYTE(a)                      ((a) >> 8)
#define LOW_BYTE(a)                       ((a) & 0xFF)

/*some useful macros for UFI command block*/
#define TRANSFER_LUN_TO_HIGH_3_BITS(x,l)   x &= ((l << 5)   | 0x1F)


/* Operation code for the UFI Commands (floppy drives) */
#define UFI_FORMAT_UNIT                   (0x04)
#define FORMAT_LUN_HEADER_BITS            (0x17)
#define UFI_READ_CAPACITY                 (0x25)
#define READ_CAPACITY_LUN_HEADER_BITS     (0x00)
#define UFI_READ_FORMAT_CAPACITY          (0x23)
#define UFI_INQUIRY                       (0x12)
#define INQUIRY_LUN_HEADER_BITS           (0x00)
#define UFI_MODE_SELECT                   (0x55)
#define MODE_SELECT_LUN_HEADER_BITS       (0x10)
#define UFI_MODE_SENSE                    (0x5A)
#define MODE_SENSE_LUN_HEADER_BITS        (0x00)
#define PAGE_CURRENT_VALUE                (0x00)
#define PAGE_CHANGEABLE_VALUE             (0x01)
#define PAGE_DEFAULT_VALUE                (0x10)
#define PAGE_SAVED_VALUE                  (0x11)

#define PAGE_CODE_ALL_PAGES               (0x3F)

#define UFI_PREVENT_ALLOW_MEDIUM_ROMVAL   (0x1E)
#define UFI_READ10                        (0x28)
#define READ10_LUN_HEADER_BITS            (0x00)
#define UFI_READ12                        (0xA8)
#define READ12_LUN_HEADER_BITS            (0x00)
#define UFI_REQUEST_SENSE                 (0x03)
#define UFI_REZERO_UNIT                   (0x01)
#define UFI_SEEK10                        (0x2B)
#define UFI_SEND_DIAGNOSTIC               (0x1D)
#define SEND_DIAG_LUN_HEADER_BITS         (0x00)
#define UFI_START_STOP                    (0x1B)
#define UFI_STOP_MEDIA                    (0x00)
#define UFI_START_MEDIA_AND_GET_FORMAT    (0x01)

#define UFI_TEST_UNIT_READY               (0x00)
#define UFI_VERIFY                        (0x2F)
#define VERIFY_LUN_HEADER_BITS            (0x00)

#define UFI_WRITE10                       (0x2A)
#define UFI_WRITE_LUN_HEADER_BITS         (0x00)
#define UFI_WRITE12                       (0xAA)
#define UFI_WRITE_AND_VERIFY              (0x2E)

#define DEFAULT_INTERLEAVE                (0x00)


/* USB Mass storage FORMAT UNIT Command information */

typedef struct _DEFECT_LIST_HEADER
{
   uint8_t   RESERVED1;
   uint8_t   BBIT_INFO_HEADER;
   #define DEFAULT_BIT_INFO               (0xA0)
   uint8_t   BLENGTH_MSB;
   uint8_t   BLENGTH_LSB;
   #define DEFAULT_LENGTH_MSB             (0x00)
   #define DEFAULT_LENGTH_LSB             (0x08)
} DEFECT_LIST_HEADER, * DEFECT_LIST_HEADER_PTR;


typedef struct _FORMAT_CAPACITY_DESCRIPTOR
{
   uint8_t   NNUM_BLOCKS[4];
   uint8_t   RESERVED;
   uint8_t   NBLOCK_LENGTH[3];
} FORMAT_CAPACITY_DESCRIPTOR, * FORMAT_CAPACITY_DESCRIPTOR_PTR;

typedef  struct _FORMAT_UNIT_PARAMETER_BLOCK
{
   DEFECT_LIST_HEADER             DEF_LIST_HEADER;
   FORMAT_CAPACITY_DESCRIPTOR     FMT_CAPACITY_DESC;
}  FORMAT_UNIT_PARAMETER_BLOCK, * FORMAT_UNIT_PARAMETER_BLOCK_PTR;


/* USB Mass storage READ CAPACITY Command information */

/* USB Mass storage Read10 Command information */
typedef struct _MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO
{
   uint8_t  BLLBA[4];    /* Last Logical Block Address */
   uint8_t  BLENGTH[4];    /*Block length */
} MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO,
     *MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO_PTR;


/* USB Mass storage READ FORMAT CAPACITY Command information */

typedef struct _CAPACITY_LIST_HEADER {
   uint8_t  RESERVED[3];
   uint8_t  BLENGTH[1];    /* Capacity list length */
} CAPACITY_LIST_HEADER, * CAPACITY_LIST_HEADER_PTR;


typedef struct _CURRENT_CAPACITY_DESCRIPTOR {
   uint8_t   NNUM_BLOCKS[4];
   uint8_t   BDESC_CODE;    /* Only last two bits are used */
   #define  UNFORMATTED_MEDIA          (0x01)
   #define  FORMATTED_MEDIA            (0x02)
   #define  NO_MEDIA                   (0x03)
   uint8_t   NBLOCK_LENGTH[3];
} CURRENT_CAPACITY_DESCRIPTOR, * CURRENT_CAPACITY_DESCRIPTOR_PTR;


typedef struct _CAPACITY_LIST {
   CAPACITY_LIST_HEADER            LIST_HEADER;          /* 4 bytes */
   CURRENT_CAPACITY_DESCRIPTOR     CAPACITY_DESCRIPTOR;  /* 8 bytes */
   FORMAT_CAPACITY_DESCRIPTOR      FMT_CAPACITY_DESC;    /* 8 bytes */
} CAPACITY_LIST, * CAPACITY_LIST_PTR;


/* USB Mass storage INQUIRY Command information */

typedef struct _INQUIRY_DATA_FORMAT
{
   /*device type currently connected to Logical Unit */
   uint8_t   BDEVICE_TYPE;
   #define DIRECT_ACCESS_DEVICE     (0x00)
   #define   NONE                   (0x1F)

   /*removable media bit only bit 7 is used. rest reserved*/
   uint8_t   BRMB;

   /*ISO, ECMA, ANSI Version bits*/
   uint8_t   BVERSION_SPEC;
   #define DEFAULT_VERSION_SPEC     (0x00)

   /*Response data format */
   uint8_t   BRESP_DATA_FORMAt;
   #define DEFAULT_RESNPOSE_FORMAT  (0x01)

   /* length of parameters */
   uint8_t   BADITIONAL_LENGTH;
   #define DEFAULT_LENGTH           (0x1F)

   uint8_t   RESERVED1;
   uint8_t   RESERVED2;
   uint8_t   RESERVED3;

   uint8_t   BVID[8];         /* Vendor Identification String */
   uint8_t   BPID[16];        /* Product Identification String */
   uint8_t   BPRODUCT_REV[4]; /* Product revision level */

} INQUIRY_DATA_FORMAT, * INQUIRY_DATA_FORMAT_PTR;


/* USB Mass storage MODE SELECT Command information */
typedef union _READ_WRITE_RECOVERY_PAGE {
   uint8_t   FIELDS[12];
} READ_WRITE_RECOVERY_PAGE, * READ_WRITE_RECOVERY_PAGE_PTR;


typedef union _MODE_SELECT_PAGE {
   READ_WRITE_RECOVERY_PAGE  READ_WRITE_PAGE;
} MODE_SELECT_PAGE, * MODE_SELECT_PAGE_PTR;

typedef struct _MODE_PARAM_HEADER {
   uint8_t  BLENGTH[2];    /* Mode Data Length */
   uint8_t  BMEDIA_TYPE;   /* Media type code */
   #define DEFAULT_MEDIA_TYPE       (0x00)
   #define DD_720KB                 (0x1E)
   #define HD_125MB                 (0x93)
   #define DD_144MB                 (0x94)
   uint8_t  WP_DPOFUA;
   #define WP_DPOFUA_HEADER         (0xEF)
} MODE_PARAM_HEADER, * MODE_PARAM_HEADER_PTR;


typedef struct _MODE_SELECT_PARAMETER_LIST {
   MODE_PARAM_HEADER   MODE_PARAM_HEADER;
   #define      DEFAULT_NO_OF_PAGES (0x01)
   MODE_SELECT_PAGE   PAGES[DEFAULT_NO_OF_PAGES];
} MODE_SELECT_PARAMETER_LIST, * MODE_SELECT_PARAMETER_LIST_PTR;



/* USB Mass storage REQUEST SENSE Command information */

typedef struct _REQ_SENSE_DATA_FORMAT
{
   /* Valid and Error code*/
   uint8_t   BERROR_CODE;           /* 0 */
   #define REQUEST_SENSE_NOT_VALID  (0x00)
   #define REQUEST_SENSE_IS_VALID   (0x01)
   #define REQUEST_SENSE_ERROR_CODE (0x70)
   uint8_t   RESERVED1;             /* 1 */
   uint8_t   BSENSE_KEY;            /* 2  Only lower 4 bits are used */
   uint8_t   BINFO[4];              /* 3,4,5,6 Information  data format */
   uint8_t   BADITIONAL_LENGTH;     /* 7 additional sense length */
   uint8_t   RESERVED2[4];          /* 8,9,10,11*/
   uint8_t   BADITIONAL_CODE;       /* 12 Additional sense code */
   uint8_t   BADITIONAL_QUALIFIEr;  /* 13 Additional sense code qualifier */
   uint8_t   RESERVED3[4];          /* 14,15,16,17*/
} REQ_SENSE_DATA_FORMAT, * REQ_SENSE_DATA_FORMAT_PTR;


/* USB Mass storage WRITE10 Command information */
typedef struct _MASS_STORAGE_WRITE10_CMD_STRUCT_INFO {
   uint8_t  BLBA[4];
   uint8_t  BTRANSFER_LENGTH[2];
} MASS_STORAGE_WRITE10_CMD_STRUCT_INFO,
        *MASS_STORAGE_WRITE10_CMD_STRUCT_INFO_PTR;


/* USB Mass storage WRITE12 Command information */
typedef struct _MASS_STORAGE_WRITE12_CMD_STRUCT_INFO {
   uint8_t  BLBA[4];
   uint8_t  BTRANSFER_LENGTH;
} MASS_STORAGE_WRITE12_CMD_STRUCT_INFO,
     *MASS_STORAGE_WRITE12_CMD_STRUCT_INFO_PTR;

/* USB Mass storage SEEK10 Command information */
typedef struct MASS_STORAGE_SEEK10_STRUCT_INFO {
   uint8_t  BLBA[4];
} MASS_STORAGE_SEEK10_STRUCT_INFO, * MASS_STORAGE_SEEK10_STRUCT_INFO_PTR;

/* USB Mass storage SEND DIAGNOSTIC Command information */
typedef struct _MASS_STORAGE_SEND_DIAGNOSTIC_STRUCT_INFO {
   bool  SELF_TEST;
} MASS_STORAGE_SEND_DIAGNOSTIC_STRUCT_INFO,
     *MASS_STORAGE_SEND_DIAGNOSTIC_STRUCT_INFO_PTR;

/* USB Mass storage START-STOP UNIT Command information */
typedef struct _MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO {
   bool   START;
   bool   LOEJ;
} MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO,
     *MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO_PTR;

/* USB Mass storage VERIFY Command information */
typedef struct _MASS_STORAGE_VERIFY_CMD_STRUCT_INFO {
   uint8_t  BLBA[4];
   uint8_t  BTRANSFER_LENGTH[2];
} MASS_STORAGE_VERIFY_CMD_STRUCT_INFO,
     *MASS_STORAGE_VERIFY_CMD_STRUCT_INFO_PTR;


/* UFI commands */

/* Used by host-side to send the READ CAPACITY command */
#define usb_mass_ufi_read_capacity(                                       \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ_CAPACITY,                                          \
      (uint8_t)(((cmd_ptr)->LUN << 5) | READ_CAPACITY_LUN_HEADER_BITS),    \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the FORMAT UNIT command */
#define usb_mass_ufi_format_unit(                                         \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR  */             cmd_ptr,                      \
                                                                          \
      /* [IN] Track number to be formatted (see UFI specs) */             \
      /* uint8_t */                          track_num,                    \
                                                                          \
      /* [IN] Interleave number (see UFI specs) */                        \
      /* uint8_t */                          interleave,                   \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* FORMAT_UNIT_PARAMETER_BLOCK_PTR */ format_ptr                    \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_FORMAT_UNIT,                                            \
      (uint8_t)(((cmd_ptr)->LUN << 5) | FORMAT_LUN_HEADER_BITS),           \
      (uint32_t)(((track_num) << 24) | (HIGH_BYTE((interleave)) << 16) |   \
         (LOW_BYTE((interleave)) << 8)),                                  \
      (uint32_t)((format_ptr) ? (sizeof(FORMAT_LUN_HEADER_BITS) << 8) : 0),\
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (unsigned char *)(format_ptr),                                            \
      (format_ptr) ? (sizeof(FORMAT_LUN_HEADER_BITS) << 8) : 0)


/* Used by host-side to send the FORMAT CAPACITY command */
#define usb_mass_ufi_format_capacity(                                     \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ_FORMAT_CAPACITY,                                   \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/*
** Used by host-side to send the INQUIRY command, to request information
** regarding parameters of the UFI device itself
*/
#define usb_mass_ufi_inquiry(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_INQUIRY,                                                \
      (uint8_t)(((cmd_ptr)->LUN << 5) | INQUIRY_LUN_HEADER_BITS),          \
      (uint32_t)(((buf_len) & 0xFF) << 8),                                 \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the MODE SELECT command */
#define usb_mass_ufi_mode_select(                                         \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */             cmd_ptr,                       \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* pointer */                        buf_ptr,                       \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                        buf_len                        \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_MODE_SELECT,                                            \
      (uint8_t)(((cmd_ptr)->LUN << 5) | MODE_SELECT_LUN_HEADER_BITS),      \
      (uint32_t)0,                                                         \
      (uint32_t)((buf_ptr) ? ((buf_len) << 8) : 0),                        \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (unsigned char *)(buf_ptr),                                               \
      (buf_ptr) ? (buf_len) : 0)



/* Used by host-side to send the MODE SENSE command */
#define usb_mass_ufi_mode_sense(                                                 \
      /* [IN] command object allocated by application*/                          \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                                  \
                                                                                 \
      /*[IN] Page control byte 2 bits   */                                       \
      /* uint8_t */                     bPC,                                     \
                                                                                 \
      /*[IN] Page code 6 bit          */                                         \
      /* uint8_t */                     bPage_code,                              \
                                                                                 \
      /* [IN] buffer to send or receive data*/                                   \
      /* pointer */                    buf_ptr,                                  \
                                                                                 \
      /* [IN] length of the expected data */                                     \
      /* uint32_t */                    buf_len                                  \
   )                                                                             \
                                                                                 \
   usb_mass_ufi_generic((cmd_ptr),                                               \
      (uint8_t)UFI_MODE_SENSE,                                                   \
      (uint8_t)((((uint32_t)(cmd_ptr)->LUN) << 5) | MODE_SENSE_LUN_HEADER_BITS), \
      (uint32_t)(((((uint32_t)(bPC)) << 6) | ((bPage_code) & 0x3F)) << 24),      \
      (uint32_t)((buf_ptr) ? ((buf_len) << 8) : 0),                              \
      (uint8_t)CBW_DIRECTION_IN,                                                 \
      (buf_ptr),                                                                 \
      (buf_ptr) ? (buf_len) : 0)


/* Used by host-side to send the PREVENT-ALLOW MEDIUM REMOVAL command */
#define usb_mass_ufi_prevent_allow_medium_removal(                        \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* uint8_t */                  bPrevent /*1 or 0 values */          \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_PREVENT_ALLOW_MEDIUM_ROMVAL,                           \
      (uint8_t)((cmd_ptr)->LUN << 5),                                     \
      (uint32_t)(((bPrevent) & 0x01) << 8),                               \
      (uint32_t)0,                                                        \
      (uint8_t)CBW_DIRECTION_OUT,                                         \
      NULL,                                                               \
      0)


/* Used by host-side to send the READ(10) command, to get data from device */
#define usb_mass_ufi_read_10(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to read */          \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ10,                                                 \
      (uint8_t)(((cmd_ptr)->LUN << 5) | READ10_LUN_HEADER_BITS),           \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((num_of_blocks) << 8),                                    \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the READ(12) command, to get data from device */
#define usb_mass_ufi_read_12(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to read */          \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ12,                                                 \
      (uint8_t)(((cmd_ptr)->LUN << 5) | READ12_LUN_HEADER_BITS),           \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)(num_of_blocks),                                           \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the REQUEST SENSE command */
#define usb_mass_ufi_request_sense(                                       \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len                               \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_REQUEST_SENSE,                                          \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)(((buf_len) & 0xFF) << 8),                                 \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (unsigned char *)(buf_ptr),                                               \
      (buf_len))


/* Used by host-side to send the REZERO UNIT command */
#define usb_mass_ufi_rezero_unit(                                         \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr                               \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_REZERO_UNIT,                                            \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the SEEK command */
#define usb_mass_ufi_seek_10(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] block address (see UFI specs) */                            \
      /* uint32_t */                 bBlock_address                        \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_SEEK10,                                                 \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the SEND DIAGNOSTIC command */
#define usb_mass_ufi_send_diagnostic(                                     \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* uint8_t */                  bSelf_test                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_SEND_DIAGNOSTIC,                                        \
      (uint8_t)((((cmd_ptr)->LUN << 5) | SEND_DIAG_LUN_HEADER_BITS) |      \
         (((bSelf_test) & 0xFF) << 2)),                                   \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the START-STOP command */
#define usb_mass_ufi_start_stop(                                          \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* Load Object Bit */                                               \
      /* uint8_t */                  bLoej,                                \
                                                                          \
      /* Start Stop bit  */                                               \
      /* uint8_t */                  bStart                                \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_START_STOP,                                             \
      (uint8_t)(((cmd_ptr)->LUN << 5) | SEND_DIAG_LUN_HEADER_BITS),        \
      (uint32_t)(((((bLoej) << 1) | (bStart)) & 0x3) << 8),                \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the TEST UNIT READY command */
#define usb_mass_ufi_test_unit_ready(                                     \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr                               \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_TEST_UNIT_READY,                                        \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the VERIFY command */
#define usb_mass_ufi_verify(                                              \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] address of the block to verify */                           \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* Length of the data to verify */                                  \
      /*uint16_t  */               dLength                                 \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_VERIFY,                                                 \
      (uint8_t)(((cmd_ptr)->LUN << 5) | VERIFY_LUN_HEADER_BITS),           \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((dLength) << 8),                                          \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the WRITE(10) command, to send data to device */
#define usb_mass_ufi_write_10(                                            \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to write */         \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_WRITE10,                                                \
      (uint8_t)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((num_of_blocks) << 8),                                    \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the WRITE(12) command, to send data to device */
#define usb_mass_ufi_write_12(                                            \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to write */         \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_WRITE12,                                                \
      (uint8_t)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)(num_of_blocks),                                          \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the WRITE AND VERIFY command */
#define usb_mass_ufi_write_and_verify(                                    \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks */                  \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_WRITE_AND_VERIFY,                                       \
      (uint8_t)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((num_of_blocks) << 8),                                    \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))


/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS usb_mass_ufi_generic
   (
      /* [IN] command object allocated by application*/
      COMMAND_OBJECT_PTR         cmd_ptr,
      uint8_t                     opcode,
      uint8_t                     lun,
      uint32_t                    lbaddr,
      uint32_t                    blen,

      uint8_t                     cbwflags,

      unsigned char                  *buf,
      uint32_t                    buf_len
   );

extern bool usb_mass_ufi_cancel
   (
      COMMAND_OBJECT_PTR         cmd_ptr   );
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
