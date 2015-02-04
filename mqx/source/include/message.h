
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   message component.
*
*
*END************************************************************************/
#ifndef __message_h__
#define __message_h__

#include <mqx_cnfg.h>
#if (!MQX_USE_MESSAGES) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error MESSAGE component is currently disabled in MQX kernel. Please set MQX_USE_MESSAGES to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* Error codes */

#define MSGPOOL_OUT_OF_MESSAGES                  (MSG_ERROR_BASE|0x01)
#define MSGPOOL_OUT_OF_POOLS                     (MSG_ERROR_BASE|0x02)
#define MSGPOOL_INVALID_POOL_ID                  (MSG_ERROR_BASE|0x03)
#define MSGPOOL_ALL_MESSAGES_NOT_FREE            (MSG_ERROR_BASE|0x04)
#define MSGPOOL_MESSAGE_SIZE_TOO_SMALL           (MSG_ERROR_BASE|0x05)
#define MSGPOOL_POOL_NOT_CREATED                 (MSG_ERROR_BASE|0x06)

#define MSGQ_INVALID_QUEUE_ID                    (MSG_ERROR_BASE|0x10)
#define MSGQ_QUEUE_IN_USE                        (MSG_ERROR_BASE|0x11)
#define MSGQ_NOT_QUEUE_OWNER                     (MSG_ERROR_BASE|0x12)
#define MSGQ_QUEUE_IS_NOT_OPEN                   (MSG_ERROR_BASE|0x13)
#define MSGQ_MESSAGE_NOT_AVAILABLE               (MSG_ERROR_BASE|0x14)
#define MSGQ_INVALID_MESSAGE_PRIORITY            (MSG_ERROR_BASE|0x15)
#define MSGQ_INVALID_MESSAGE                     (MSG_ERROR_BASE|0x16)
#define MSGQ_QUEUE_FULL                          (MSG_ERROR_BASE|0x17)
#define MSGQ_RECEIVE_TIMEOUT                     (MSG_ERROR_BASE|0x18)
#define MSGQ_TOO_MANY_QUEUES                     (MSG_ERROR_BASE|0x19)
#define MSGQ_MESSAGE_IS_QUEUED                   (MSG_ERROR_BASE|0x1A)
#define MSGQ_INVALID_QUEUE_NUMBER                (MSG_ERROR_BASE|0x1B)

#define MSG_CANNOT_CREATE_COMPONENT              (MSG_ERROR_BASE|0x20)



/* What the msg component valid field should be */
#define MESSAGE_VALID                 (_mqx_uint)(0x6d657373)   /* "mess" */

/* What a NULL pool id looks like.  Used for error returns from functions */
#define MSGPOOL_NULL_POOL_ID     ((_pool_id)0)

/* Maximum number of data bytes in a message. */
#define MSGPOOL_MESSAGE_DATA_SIZE \
   (MAX_UINT_16 - sizeof(MESSAGE_HEADER_STRUCT))

/* What a NULL queue id looks like. Used for error returns from functions */
#define MSGQ_NULL_QUEUE_ID    ((_queue_id)0)

/* For _msgq_receive, receives from any queue */
#define MSGQ_ANY_QUEUE        ((_queue_id)0)

/* For open queue, this queue number will return a free queue_id */
#define MSGQ_FREE_QUEUE       (0)

/* The maximum priority level for the message */
#define MSG_MAX_PRIORITY      (0xF)

/* The lowest queue provided by msgq_open with an MSGQ_MSGQ_ANY_QUEUE request. */
#define MSGQ_FIRST_USER_QUEUE (8)

/*--------------------------------------------------------------------------*/
/*
 *             MESSAGE STRUCTURE CONTROL FIELD BIT DEFINITIONS
 */

#define MSG_HDR_ENDIAN_MASK             (0x40)
#define MSG_DATA_ENDIAN_MASK            (0x20)

#define MSG_HDR_LITTLE_ENDIAN           (0x40)
#define MSG_DATA_LITTLE_ENDIAN          (0x20)

#define MSG_HDR_BIG_ENDIAN              (0x00)
#define MSG_DATA_BIG_ENDIAN             (0x00)

/* Is the message an urgent message */
#define MSG_HDR_URGENT                  (0x10)
#define MSG_HDR_PRIORITY_MASK           (0x0F)

/* NOTE that the lower nibble of the control field is the message priority */

#define MSG_IS_HDR_LITTLE_ENDIAN(ctrl) \
   (((ctrl) & MSG_HDR_ENDIAN_MASK) == MSG_HDR_LITTLE_ENDIAN)
#define MSG_IS_DATA_LITTLE_ENDIAN(ctrl) \
   (((ctrl) & MSG_DATA_ENDIAN_MASK) == MSG_DATA_LITTLE_ENDIAN)

#define MSG_IS_HDR_BIG_ENDIAN(ctrl) \
   (((ctrl) & MSG_HDR_ENDIAN_MASK) == MSG_HDR_BIG_ENDIAN)
#define MSG_IS_DATA_BIG_ENDIAN(ctrl) \
   (((ctrl) & MSG_DATA_ENDIAN_MASK) == MSG_DATA_BIG_ENDIAN)

/*
 * ENDIAN
 * Indicate in messages the endianness of the message
 */
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
#define MSG_HDR_ENDIAN   MSG_HDR_LITTLE_ENDIAN
#define MSG_DATA_ENDIAN  MSG_DATA_LITTLE_ENDIAN
#else
#define MSG_HDR_ENDIAN   MSG_HDR_BIG_ENDIAN
#define MSG_DATA_ENDIAN  MSG_DATA_BIG_ENDIAN
#endif

#define MSG_MUST_CONVERT_HDR_ENDIAN(ctrl) \
   (((ctrl) & MSG_HDR_ENDIAN_MASK) != MSG_HDR_ENDIAN)

#define MSG_MUST_CONVERT_DATA_ENDIAN(ctrl) \
   (((ctrl) & MSG_DATA_ENDIAN_MASK) != MSG_DATA_ENDIAN)

#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
#define MSG_SET_DATA_ENDIAN(ctrl)  (ctrl |= MSG_DATA_LITTLE_ENDIAN)
#else
#define MSG_SET_DATA_ENDIAN(ctrl)  (ctrl &= ~MSG_DATA_LITTLE_ENDIAN)
#endif

/*--------------------------------------------------------------------------*/
/*
 *                    TYPEDEFS FOR _CODE_PTR_ FUNCTIONS
 */ 
 
typedef void (_CODE_PTR_ MSGQ_NOTIFICATION_FPTR)( void *);

/*--------------------------------------------------------------------------*/
/*                        DATATYPE DEFINITIONS                          */

typedef void   *_pool_id;           /* what a pool_id looks like */
typedef uint16_t _msg_size;          /* what a message size looks like */
#if MQX_USE_32BIT_MESSAGE_QIDS
typedef uint32_t _queue_number;      /* what a queue number is         */
typedef uint32_t _queue_id;          /* What a queue_id looks like */
#else
typedef uint16_t _queue_number;      /* what a queue number is         */
typedef uint16_t _queue_id;          /* What a queue_id looks like */
#endif

/*--------------------------------------------------------------------------*/

/* MESSAGE HEADER STRUCT */
/*!
 * \brief This structure defines the first field (header) of any message.
 *  
 * Any pool of messsages must have at least this size and must start with a 
 * message header.
 * 
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _msg_free
 * \see _msgq_poll
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_send
 */
typedef struct message_header_struct
{
   /*! 
    * \brief The size of the DATA field in bytes, including the header. The 
    * maximum value is MAX_MESSAGE_SIZE. This field is set by the application. 
    */
   _msg_size       SIZE;

#if MQX_USE_32BIT_MESSAGE_QIDS
   uint16_t         PAD;
#endif

   /*! 
    * \brief Queue ID of the queue to which MQX is to send the message. This 
    * field is set by the application. 
    */
   _queue_id       TARGET_QID;

   /*!
    * \brief Queue ID of a message queue that is associated with the sending
    * task. 
    * 
    * When messages are allocated, this field is initialized to MSGQ_NULL_QUEUE_ID. 
    * If the sending task does not have a message queue associated with it, MQX 
    * does not use this field.
    */
   _queue_id       SOURCE_QID;

   /*! 
    * \brief Indicates the following for the message: endian format, priority 
    * and urgency.
    */
   unsigned char           CONTROL;

#if MQX_USE_32BIT_MESSAGE_QIDS
   /*! \brief Not used. */
   unsigned char           RESERVED[3];
#else
   /*! \brief Not used. */
   unsigned char           RESERVED;
#endif

} MESSAGE_HEADER_STRUCT, * MESSAGE_HEADER_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern void            *_msg_alloc(_pool_id);
extern void            *_msg_alloc_system(_msg_size);
extern _mqx_uint        _msg_available(_pool_id);
extern _mqx_uint        _msg_create_component(void);
extern void             _msg_free(void *);
extern void             _msg_swap_endian_data(unsigned char *,
   MESSAGE_HEADER_STRUCT_PTR);
extern void             _msg_swap_endian_header(MESSAGE_HEADER_STRUCT_PTR);

extern _pool_id         _msgpool_create(uint16_t, uint16_t, uint16_t, uint16_t);
extern bool          _msgpool_create_system(uint16_t, uint16_t, uint16_t, 
   uint16_t);
extern _mqx_uint        _msgpool_destroy(_pool_id);
extern _mqx_uint        _msgpool_test(void **, void **);

extern bool          _msgq_close(_queue_id);
extern _mqx_uint        _msgq_get_count(_queue_id);
extern _queue_id        _msgq_get_id(_processor_number, _queue_number);
extern _task_id         _msgq_get_owner(_queue_id);
extern _mqx_uint        _msgq_get_notification_function(_queue_id,
   MSGQ_NOTIFICATION_FPTR *, void **);
extern _queue_id        _msgq_open(_queue_number, uint16_t);
extern _queue_id        _msgq_open_system(_queue_number, uint16_t, 
   MSGQ_NOTIFICATION_FPTR, void *);
extern void            *_msgq_poll(_queue_id);
extern void            *_msgq_peek(_queue_id);
extern void            *_msgq_receive(_queue_id, uint32_t);
extern void            *_msgq_receive_for(_queue_id, MQX_TICK_STRUCT_PTR);
extern void            *_msgq_receive_ticks(_queue_id, _mqx_uint);
extern void            *_msgq_receive_until(_queue_id, MQX_TICK_STRUCT_PTR);
extern bool          _msgq_send(void *);
extern _mqx_uint        _msgq_send_broadcast(void *, _queue_id *, _pool_id);
extern bool          _msgq_send_queue(void *, _queue_id);
extern bool          _msgq_send_urgent(void *);
extern bool          _msgq_send_priority(void *, _mqx_uint);
extern MSGQ_NOTIFICATION_FPTR _msgq_set_notification_function(_queue_id,
   MSGQ_NOTIFICATION_FPTR, void *);
extern _mqx_uint        _msgq_test(void **, void **);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
