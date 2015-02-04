
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
*   This file contains private information for the message component
*
*
*END************************************************************************/
#ifndef __msg_prv_h__
#define __msg_prv_h__

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */
/* Timeout modes */
#define MSG_TIMEOUT_NONE                0
#define MSG_TIMEOUT_ABSOLUTE            1
#define MSG_TIMEOUT_RELATIVE            2

/* Used to mark a message as belonging to the message pool */
#define MSG_VALID                       (0x6D67)  /* "mg" */

/* Message pool types */
#define MSG_POOL                        (5)
#define SYSTEM_MSG_POOL                 (10)

/* What bits control the priority/urgency of the message */
#define MSG_PRIORITY_MASK               (0x1F)

/* Values used when building a QUEUE ID */
#define MSG32_PROC_MASK                 (0xffff)
#define MSG32_PROC_SHIFT                (16)
#define MSG32_QUEUE_MASK                (0xffff)
#define MSG32_MAX_PROCESSORS            (0x10000)
#define MSG32_MAX_QNUMBERS              (0x10000)

#define MSG16_PROC_MASK                 (0xff)
#define MSG16_PROC_SHIFT                (8)
#define MSG16_QUEUE_MASK                (0xff)
#define MSG16_MAX_PROCESSORS            (256)
#define MSG16_MAX_QNUMBERS              (256)

#if MQX_USE_32BIT_MESSAGE_QIDS
#define PROC_MASK                       MSG32_PROC_MASK
#define PROC_SHIFT                      MSG32_PROC_SHIFT
#define QUEUE_MASK                      MSG32_QUEUE_MASK
#define MAX_PROCESSORS                  MSG32_MAX_PROCESSORS
#define MAX_QNUMBERS                    MSG32_MAX_QNUMBERS
#else
#define PROC_MASK                       MSG16_PROC_MASK
#define PROC_SHIFT                      MSG16_PROC_SHIFT
#define QUEUE_MASK                      MSG16_QUEUE_MASK
#define MAX_PROCESSORS                  MSG16_MAX_PROCESSORS
#define MAX_QNUMBERS                    MSG16_MAX_QNUMBERS
#endif /* MQX_USE_32BIT_MESSAGE_QIDS */

/* the message queue is owned by a task */
#define MSG_QUEUE                       (1)

/* the message queue belongs to the system, not owned by a task. */
#define SYSTEM_MSG_QUEUE                (2)

/* How to build a Queue ID from the processor number and queue number */
#define BUILD_QID(processor_number,queue) \
    (_queue_id)((((processor_number) & PROC_MASK) << PROC_SHIFT) | \
    ((queue) & QUEUE_MASK))

/* Extract the processor number from a queue id */
#define PROC_NUMBER_FROM_QID(qid) \
    ((qid) >> PROC_SHIFT)

/* Extract the queue number from a queue id */
#define QUEUE_FROM_QID(qid) \
    ((qid) & QUEUE_MASK)

/* Is this a valid queue number */
#define  VALID_QUEUE(qnum) \
    ((qnum) > 0)

/*
 * QUEUE MACROS
 */
/*
 * Given a message pointer from an application program, locate where
 * the address of the internal message structure begins.
 */
#define  GET_INTERNAL_MESSAGE_PTR(msg_ptr) \
    (INTERNAL_MESSAGE_STRUCT_PTR)(((unsigned char *)(msg_ptr)) - \
    FIELD_OFFSET(INTERNAL_MESSAGE_STRUCT,MESSAGE))

/* Return the address of the message component structure */
#define  _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data) \
    ((MSG_COMPONENT_STRUCT_PTR)(kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES]))

/*--------------------------------------------------------------------------*/
/*                        DATATYPE DEFINITIONS
 *
 */

/*-------------------------------------------------------------------------*/
/* MSGPOOL BLOCK STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 * 
 * \brief This structure is prepended to the beginning of each block of messages 
 * when they are allocated.
 *  
 * When the message pool grows, a new block of messages is created and is linked 
 * to the previous block of messages.
 */
typedef struct msgpool_block_struct
{
    /*! \brief The address of the next block of messages in this pool. */
    struct msgpool_block_struct      *NEXT_BLOCK_PTR;

    /*!
     * \brief The 'RAW' size of the messages in this block.
     *      
     * The message may be larger than requested in order to allow for memory 
     * alignment constraints.  Also this size includes the internal header for 
     * the message.
     */
    _mqx_uint RAW_MESSAGE_SIZE;

    /*! \brief The number of messages in this block. */
    _mqx_uint NUM_MESSAGES;

    /*! \brief The address of the first internal message in the pool. */
    void   *FIRST_IMSG_PTR;

}MSGPOOL_BLOCK_STRUCT, * MSGPOOL_BLOCK_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/* MSGPOOL STRUCT */
/*!
 * \cond DOXYGEN_PRIVATE
 * 
 * \brief This structure defines what a message pool looks like.
 *
 * The message pools are linked together via the NEXT_MSGPOOL_PTR field. The 
 * pools are sorted in size, with the smaller pools first.
 * \n The actual messages are allocated in blocks, and linked to the msgpool 
 * structure via the MSGPOOL_BLOCK_PTR.
 */
typedef struct msgpool_struct
{
    /*! \brief Pointer to the list of blocks of messages. */
    struct msgpool_block_struct      *MSGPOOL_BLOCK_PTR;

    /*! \brief Link to the next message pool, of larger size. */
    struct msgpool_struct      *NEXT_MSGPOOL_PTR;

    /*! \brief Current number of messages on the free list. */
    uint16_t SIZE;

    /*! \brief Maximum number of messages on the free list. */
    uint16_t MAX;

    /*! \brief Pointer to a list of FREE messages. */
    struct internal_message_struct      *MSG_FREE_LIST_PTR;

    /*!
     * \brief Size of the user message in bytes, as requested when the pool was 
     * allocated.
     */
    uint16_t MESSAGE_SIZE;

    /*! \brief Should the pool grow if empty. */
    uint16_t GROW_NUMBER;

    /*! \brief Validation stamp for message pools. */
    uint16_t VALID;

    /*! \brief The maximum number of messages allowed to be in the pool. */
    uint16_t GROW_LIMIT;

    /*! \brief The type of the message pool MSG_POOL or SYSTEM_MSG_POOL. */
    _mqx_uint MSGPOOL_TYPE;

}MSGPOOL_STRUCT, * MSGPOOL_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/* INTERNAL MESSAGE STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This structure is the internal representation of a message.
 *
 * When a message is provided to an application task from the message pool, the 
 * application is provided with the address of the MESSAGE_HEADER_STRUCT found 
 * at the end of this structure.
 */
typedef struct internal_message_struct
{
    /*! 
     * \brief Link to the next message in the list. NEXT is used to link the 
     * message onto a free list.
     */     
    struct internal_message_struct      *NEXT;
    
    /*! \brief Link to previous message in the list. */ 
    struct internal_message_struct      *PREV;

    /*! \brief Validation stamp. */
    uint16_t VALID;

    /*! \brief Is this a free message. */
    bool FREE;

    /*! \brief Is this a queued message. */
    bool QUEUED;

    /*! \brief Address of the msgpool that this message is from. */
    MSGPOOL_STRUCT_PTR MSGPOOL_PTR;

    /*! \brief What task owns this message. */
    TD_STRUCT_PTR TD_PTR;

    /*! \brief The actual message whose address is given to the application. */
    MESSAGE_HEADER_STRUCT MESSAGE;

}INTERNAL_MESSAGE_STRUCT, * INTERNAL_MESSAGE_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/* MSGQ STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This is the structure used to define a message queue.
 */
typedef struct msgq_struct
{
    /*! \brief The first message on the message queue. */
    INTERNAL_MESSAGE_STRUCT_PTR FIRST_MSG_PTR;

    /*! \brief The last message on the message queue. */
    INTERNAL_MESSAGE_STRUCT_PTR LAST_MSG_PTR;

    /*! \brief The current number of messages on the queue. */
    uint16_t NO_OF_ENTRIES;

    /*! \brief The maximum number of messages that can be placed on this queue. */
    uint16_t MAX_ENTRIES;

    /*! \brief Owner task (task which OPENED) the queue. */
    TD_STRUCT_PTR TD_PTR;

    /*! \brief Link to next message queue owned by the task. */
    struct msgq_struct      *NEXT_MSGQ_PTR;

    /*! \brief The queue number of this message queue. */
    uint16_t QUEUE;

    /*! \brief The type of this queue: MSG_QUEUE or SYSTEM_MSG_QUEUE. */
    uint16_t TYPE;

    /*!
     * \brief A function can be called asynchronously when a message is put onto
     * the message queue. This is called the notification function.
     */
    MSGQ_NOTIFICATION_FPTR NOTIFICATION_FUNCTION;
    /*! \brief Parameter passed to the notification function. */
    void   *NOTIFICATION_FUNCTION_PARAMETER;

}MSGQ_STRUCT, * MSGQ_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/* MSG COMPONENT STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This is the base structure used by the message component to store all
 * component specific information.
 *  
 * It currently overlays the MSGQ_RESERVED fields of the kernel data structure.
 */
typedef struct msg_component_struct
{
    /*! The maximum number of message pools allowed in the system. */
    _mqx_uint MAX_MSGPOOLS;

    /*!
     * \brief The address of an array of msgpool structures allocated at 
     * component creation time. There is MAX_MSGPOOLS number of these structures 
     * in the array.
     */
    MSGPOOL_STRUCT_PTR MSGPOOLS_PTR;

    /*! 
     * \brief Pointer to smallest message pool. The message pools with smallest
     * messages are found first. 
     */ 
    MSGPOOL_STRUCT_PTR SMALLEST_MSGPOOL_PTR;
    /*! \brief Pointer to largest message pool. */
    MSGPOOL_STRUCT_PTR LARGEST_MSGPOOL_PTR;

    /*! \brief The maximum number of message pools created at any one time. */
    _mqx_uint MAX_MSGPOOLS_EVER;

    /*! \brief The maximum number of message queues allowed. */
    _mqx_uint MAX_MSGQS;

    /*!
     * \brief The address of an array of message queue structures allocated at 
     * component creation time. There are MAX_MSGQS number of these structures
     * in the array.
     */
    MSGQ_STRUCT_PTR MSGQS_PTR;

    /*! \brief A validation stamp for messages. */
    _mqx_uint VALID;

}MSG_COMPONENT_STRUCT, * MSG_COMPONENT_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__

    extern void _msg_cleanup
    (
        TD_STRUCT_PTR
    );

    extern void _msgq_insert_message_internal
    (
        MSGQ_STRUCT_PTR,
        INTERNAL_MESSAGE_STRUCT_PTR,
        bool
    );

    extern _queue_id _msgq_open_internal
    (
        _queue_number,
        uint16_t,
        _mqx_uint,
        MSGQ_NOTIFICATION_FPTR,
        void *
    );

    extern MESSAGE_HEADER_STRUCT_PTR _msgq_receive_internal
    (
        _queue_id,
        MQX_TICK_STRUCT_PTR,
        _mqx_uint,
        _mqx_uint_ptr
    );

    extern void _msgq_release_internal
    (
        uint16_t
    );

    extern bool _msgq_send_blocked_internal
    (
    		void *
    );

    extern bool _msgq_send_internal
    (
        MESSAGE_HEADER_STRUCT_PTR,
        bool,
        _queue_id
    );

    extern void _msgpool_add_internal
    (
        MSGPOOL_STRUCT_PTR,
        uint16_t
    );

    extern _pool_id _msgpool_create_internal
    (
        uint16_t,
        uint16_t,
        uint16_t,
        uint16_t,
        _mqx_uint
    );

#endif /* __TAD_COMPILE__ */

#ifdef __cplusplus
}
#endif

#endif /* __msg_prv_h__ */
/* EOF */
