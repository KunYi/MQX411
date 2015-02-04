#ifndef __tftp_h__
#define __tftp_h__
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
*   This file contains the definitions needed by TFTP.
*
*
*END************************************************************************/

/* Constants */

#define TFTPOP_RRQ     1
#define TFTPOP_WRQ     2
#define TFTPOP_DATA    3
#define TFTPOP_ACK     4
#define TFTPOP_ERROR   5

#define TFTPERR_UNKNOWN            0
#define TFTPERR_FILE_NOT_FOUND     1
#define TFTPERR_ACCESS_VIOLATION   2
#define TFTPERR_DISK_FULL          3
#define TFTPERR_ILLEGAL_OP         4
#define TFTPERR_ILLEGAL_TID        5
#define TFTPERR_FILE_EXISTS        6
#define TFTPERR_ILLEGAL_USER       7

#define TFTP_DATA_SIZE            512
#define TFTPSRV_MAX_TRANSACTIONS  20

#define TFTP_TIMEOUT_RETRIES    1        /* retransmit forever */
#define TFTP_TIMEOUT_MIN        5000     /* 5 seconds */
#define TFTP_TIMEOUT_MAX        60000    /* 1 minute */
#define TFTP_MAX_MESSAGE_SIZE   576
/*
** TFTP_MAX_MODE_SIZE is set to 9 because "netascii" is eight characters, plus
** one for the NUL, if TFTP ever allows other mode types with more characters,
** this value will have to be increased accordingly.
*/
#define TFTP_MAX_MODE_SIZE      9
#define TFTPSRV_INFINITE_RETRIES -1
/*
** This value can be set to TFTP_INFINITE_RETRIES if the end user wishes to
** never cancel a transaction. The default value when shipped is 20.
*/
#define TFTPSRV_TIMEOUT_RETRIES 20

/* TFTP Structure definitions */
typedef struct tftp_header {
   unsigned char       OP[2];
   unsigned char       BLOCK[2];
} TFTP_HEADER, * TFTP_HEADER_PTR;

/* TFTP data packet */
typedef struct tftp_packet {
   TFTP_HEADER HEAD;
   unsigned char       DATA[TFTP_DATA_SIZE];
} TFTP_PACKET, * TFTP_PACKET_PTR;

/* The Illegal TID error packet */
typedef struct tftp_error_packet {
   unsigned char       OP[2];
   unsigned char       CODE[2];
   unsigned char       MSG[25];
} TFTP_ERROR_PACKET, * TFTP_ERROR_PACKET_PTR;

typedef struct tftp_to_struct {
   uint32_t  TS;
   bool  UPDATE;
   uint32_t  TO;
   uint32_t  M;
   uint32_t  D;
} TFTP_TO_STRUCT, * TFTP_TO_STRUCT_PTR;

typedef struct tftp_trans_struct {
   struct tftp_trans_struct            *NEXT;
   struct tftp_trans_struct           **PREV;
   uint32_t                       TIME;
   uint32_t                       SOCK;
   MQX_FILE_PTR                  TRANS_FILE_PTR;
   sockaddr_in                   ADDR;
   uint32_t                       RECV_OP;
   uint32_t                       SEND_OP;
   uint32_t                       BLOCK;
   uint32_t                       SEND_SIZE;
   TFTP_TO_STRUCT                XMIT_TIMER;
   uint32_t                       NUM_RETRIES;
   bool                       EXIT;
   TFTP_PACKET                   SND;
} TFTP_TRANS_STRUCT, * TFTP_TRANS_STRUCT_PTR;

/* The state structure for the TFTP Server */
typedef struct tftpsrv_state_struct {
   /* DO NOT INSERT ANYTHING BETWEEN SRV_SOCK and SOCKETS! */
   uint32_t                 SRV_SOCK;
   uint32_t                 SOCKETS[TFTPSRV_MAX_TRANSACTIONS];
   TFTP_TRANS_STRUCT      *TRANS_PTRS[TFTPSRV_MAX_TRANSACTIONS];
   TFTP_TRANS_STRUCT      *EVENT_HEAD;
   uint32_t                 NUM_TRANSACTIONS;
   unsigned char                   BUFFER[TFTP_MAX_MESSAGE_SIZE];
} TFTPSRV_STATE_STRUCT, * TFTPSRV_STATE_STRUCT_PTR;

/* TFTP Macros */
#define TFTP_WAIT(t) RTCS_selectset(&TFTP_config.SOCK, 1, t)

#define TFTP_RECV(p) recvfrom(TFTP_config.SOCK, &(p), sizeof(p), 0, \
                              (sockaddr *)&remote_addr, &remote_size)

#define TFTP_SEND(s,p,a) sendto(s, &(p), sizeof(p), 0, (sockaddr *)&(a), sizeof(a))

#define TFTP_SENDRRQ() sendto(TFTP_config.SOCK, TFTP_config.RRQ_PTR, TFTP_config.RRQ_LEN, 0, \
                              (sockaddr *)(&TFTP_config.SADDR), sizeof(TFTP_config.SADDR))

#define TFTP_RESEND() TFTP_config.RRQ_PTR ? TFTP_SENDRRQ() : TFTP_SEND(TFTP_config.SOCK, TFTP_config.ACK, TFTP_config.SADDR)

extern TFTP_ERROR_PACKET _tftp_error_tid;
extern TFTP_ERROR_PACKET _tftp_error_op;
extern TFTP_ERROR_PACKET _tftp_error_accvio;
extern TFTP_ERROR_PACKET _tftp_error_srv;
extern TFTP_ERROR_PACKET _tftp_error_busy;
extern TFTP_ERROR_PACKET _tftp_error_to;
extern TFTP_ERROR_PACKET _tftp_error_nofile;
extern TFTP_ERROR_PACKET _tftp_error_exists;

extern void    TFTPSRV_service_request     (TFTPSRV_STATE_STRUCT_PTR);
extern void    TFTPSRV_service_transaction (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR);
extern void    TFTPSRV_close_transaction   (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR);
extern uint32_t TFTPSRV_service_timer       (TFTPSRV_STATE_STRUCT_PTR);

extern void TFTPSRV_build_ACK  (TFTP_TRANS_STRUCT_PTR);
extern void TFTPSRV_build_DATA (TFTP_TRANS_STRUCT_PTR);
extern void TFTPSRV_send       (TFTP_TRANS_STRUCT_PTR);

extern void TFTPSRV_timer_start  (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR, uint32_t);
extern void TFTPSRV_timer_cancel (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR);

extern uint32_t TFTPSRV_open_device (uint32_t, char *, char *, MQX_FILE_PTR *);

extern uint32_t TFTP_timeout_init    (TFTP_TO_STRUCT_PTR);
extern uint32_t TFTP_timeout_restart (TFTP_TO_STRUCT_PTR);
extern uint32_t TFTP_timeout_update  (TFTP_TO_STRUCT_PTR);
extern uint32_t TFTP_timeout_left    (TFTP_TO_STRUCT_PTR, bool *);

#endif
/* EOF */
