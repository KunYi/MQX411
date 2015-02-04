
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains MCC library common functions
*
*
*END************************************************************************/

#include "mcc_config.h"
#include "mcc_common.h"

#if (MCC_OS_USED == MCC_LINUX)
#include "linux/mcc_linux.h"
#include "linux/mcc_shm_linux.h"
#include <mach/hardware.h>
#include <linux/kernel.h>
#elif (MCC_OS_USED == MCC_MQX)
#include "mcc_mqx.h"
#endif


#if (MCC_OS_USED == MCC_MQX)
MCC_BOOKEEPING_STRUCT * bookeeping_data;
#endif

/*!
 * \brief This function registers an endpoint.
 *
 * Register an endpoint with specified structure / params (core, node and port).
 *
 * \param[in] endpoint Pointer to the endpoint structure.
 *
 * \return MCC_SUCCESS
 * \return MCC_ERR_NOMEM (maximum number of endpoints exceeded)
 * \return MCC_ERR_ENDPOINT (invalid value for port or endpoint already registered)
 */
int mcc_register_endpoint(MCC_ENDPOINT endpoint)
{
    int i;

    /* must be valid */
    if(endpoint.port == MCC_RESERVED_PORT_NUMBER)
        return MCC_ERR_ENDPOINT;

    /* check not already registered */
    if(mcc_get_endpoint_list(endpoint))
        return MCC_ERR_ENDPOINT;

    MCC_DCACHE_INVALIDATE_MLINES(&bookeeping_data->endpoint_table[0], MCC_ATTR_MAX_RECEIVE_ENDPOINTS * sizeof(MCC_ENDPOINT_MAP_ITEM));
    for(i = 0; i < MCC_ATTR_MAX_RECEIVE_ENDPOINTS; i++) {
        if(bookeeping_data->endpoint_table[i].endpoint.port == MCC_RESERVED_PORT_NUMBER) {
            bookeeping_data->endpoint_table[i].endpoint.core = endpoint.core;
            bookeeping_data->endpoint_table[i].endpoint.node = endpoint.node;
            bookeeping_data->endpoint_table[i].endpoint.port = endpoint.port;
            MCC_DCACHE_FLUSH_MLINES(&bookeeping_data->endpoint_table[i], sizeof(MCC_ENDPOINT_MAP_ITEM));
            return MCC_SUCCESS;
        }
    }
    return MCC_ERR_NOMEM;
}

/*!
 * \brief This function removes an endpoint.
 *
 * Removes an endpoint with specified structure / params (core, node and port).
 *
 * \param[in] endpoint Pointer to the endpoint structure.
 *
 * \return MCC_SUCCESS
 * \return MCC_ERR_ENDPOINT (invalid value for port or the endpoint doesn't exist)
 */
int mcc_remove_endpoint(MCC_ENDPOINT endpoint)
{
    int i=0;

    /* must be valid */
    if(endpoint.port == MCC_RESERVED_PORT_NUMBER)
        return MCC_ERR_ENDPOINT;

    MCC_DCACHE_INVALIDATE_MLINES(&bookeeping_data->endpoint_table[0], MCC_ATTR_MAX_RECEIVE_ENDPOINTS * sizeof(MCC_ENDPOINT_MAP_ITEM));
    for(i = 0; i < MCC_ATTR_MAX_RECEIVE_ENDPOINTS; i++) {

        if(MCC_ENDPOINTS_EQUAL(bookeeping_data->endpoint_table[i].endpoint, endpoint)) {
            /* clear the queue */
#if (MCC_OS_USED == MCC_LINUX)
            MCC_RECEIVE_BUFFER * buffer = mcc_dequeue_buffer((MCC_RECEIVE_LIST *)MQX_TO_VIRT(&bookeeping_data->endpoint_table[i].list));
            while(buffer) {
                mcc_queue_buffer((MCC_RECEIVE_LIST *)MQX_TO_VIRT(&bookeeping_data->free_list), buffer);
                buffer = mcc_dequeue_buffer((MCC_RECEIVE_LIST *)MQX_TO_VIRT(&bookeeping_data->endpoint_table[i].list));
            }
#elif (MCC_OS_USED == MCC_MQX)
            MCC_RECEIVE_BUFFER * buffer = mcc_dequeue_buffer((MCC_RECEIVE_LIST *)&bookeeping_data->endpoint_table[i].list);
            while(buffer) {
                mcc_queue_buffer(&bookeeping_data->free_list, buffer);
                buffer = mcc_dequeue_buffer((MCC_RECEIVE_LIST *)&bookeeping_data->endpoint_table[i].list);
            }
#endif
            /* indicate free */
            bookeeping_data->endpoint_table[i].endpoint.port = MCC_RESERVED_PORT_NUMBER;
            MCC_DCACHE_FLUSH_MLINES((void*)&bookeeping_data->endpoint_table[i].endpoint.port, sizeof(MCC_PORT));
            return MCC_SUCCESS;
        }
    }
    return MCC_ERR_ENDPOINT;
}

/*!
 * \brief This function dequeues the buffer.
 *
 * Dequeues the buffer from the list.
 *
 * \param[in] list Pointer to the MCC_RECEIVE_LIST structure.
 *  
 * \return Pointer to MCC_RECEIVE_BUFFER
 */
MCC_RECEIVE_BUFFER * mcc_dequeue_buffer(MCC_RECEIVE_LIST *list)
{
    MCC_DCACHE_INVALIDATE_MLINES((void*)list, sizeof(MCC_RECEIVE_LIST));
    MCC_RECEIVE_BUFFER * next_buf = list->head;

#if (MCC_OS_USED == MCC_LINUX)
    MCC_RECEIVE_BUFFER * next_buf_virt = (MCC_RECEIVE_BUFFER *)MQX_TO_VIRT(next_buf);
    if(next_buf) {
        MCC_DCACHE_INVALIDATE_MLINES((void*)&next_buf_virt->next, sizeof(MCC_RECEIVE_BUFFER*));
        list->head = next_buf_virt->next;
        if(list->tail == next_buf)
            list->tail = null;
    }
    MCC_DCACHE_FLUSH_MLINES(list, sizeof(MCC_RECEIVE_LIST));
    return next_buf_virt;
#elif (MCC_OS_USED == MCC_MQX)
    if(next_buf) {
        MCC_DCACHE_INVALIDATE_MLINES((void*)&next_buf->next, sizeof(MCC_RECEIVE_BUFFER*));
        list->head = next_buf->next;
        if(list->tail == next_buf)
            list->tail = null;
    }
    MCC_DCACHE_FLUSH_MLINES(list, sizeof(MCC_RECEIVE_LIST));
    return next_buf;
#endif
}

/*!
 * \brief This function queues the buffer.
 *
 * Queues the buffer in the list.
 *
 * \param[in] list Pointer to the MCC_RECEIVE_LIST structure.
 * \param[in] r_buffer Pointer to MCC_RECEIVE_BUFFER.
 *  
 * \return none
 */
void mcc_queue_buffer(MCC_RECEIVE_LIST *list, MCC_RECEIVE_BUFFER * r_buffer)
{
    MCC_DCACHE_INVALIDATE_MLINES((void*)list, sizeof(MCC_RECEIVE_LIST));

#if (MCC_OS_USED == MCC_LINUX)
    MCC_RECEIVE_BUFFER * last_buf = (MCC_RECEIVE_BUFFER *)MQX_TO_VIRT(list->tail);
    MCC_RECEIVE_BUFFER * r_buffer_mqx = (MCC_RECEIVE_BUFFER *)VIRT_TO_MQX(r_buffer);
    if(last_buf) {
        last_buf->next = r_buffer_mqx;
        MCC_DCACHE_FLUSH_MLINES((void*)&last_buf->next, sizeof(MCC_RECEIVE_BUFFER*));
    }
    else {
        list->head = r_buffer_mqx;
    }
    r_buffer->next = null;
    list->tail = r_buffer_mqx;
    MCC_DCACHE_FLUSH_MLINES(list, sizeof(MCC_RECEIVE_LIST));
    MCC_DCACHE_FLUSH_MLINES((void*)&r_buffer->next, sizeof(MCC_RECEIVE_BUFFER*));
#elif (MCC_OS_USED == MCC_MQX)
    MCC_RECEIVE_BUFFER * last_buf = list->tail;
    if(last_buf) {
        last_buf->next = r_buffer;
        MCC_DCACHE_FLUSH_MLINES((void*)&last_buf->next, sizeof(MCC_RECEIVE_BUFFER*));
    }
    else {
        list->head = r_buffer;
    }
    r_buffer->next = null;
    list->tail = r_buffer;
    MCC_DCACHE_FLUSH_MLINES(list, sizeof(MCC_RECEIVE_LIST));
    MCC_DCACHE_FLUSH_MLINES((void*)&r_buffer->next, sizeof(MCC_RECEIVE_BUFFER*));
#endif
}

/*!
 * \brief This function returns the endpoint list.
 *
 * Returns the MCC_RECEIVE_LIST respective to the endpoint structure provided.
 *
 * \param[in] endpoint Pointer to the MCC_ENDPOINT structure.
 *  
 * \return MCC_RECEIVE_LIST pointer 
 * \return null pointer
 */
MCC_RECEIVE_LIST * mcc_get_endpoint_list(MCC_ENDPOINT endpoint)
{
    int i=0;

    /* must be valid */
    if(endpoint.port == MCC_RESERVED_PORT_NUMBER)
        return null;

    MCC_DCACHE_INVALIDATE_MLINES(&bookeeping_data->endpoint_table[0], MCC_ATTR_MAX_RECEIVE_ENDPOINTS * sizeof(MCC_ENDPOINT_MAP_ITEM));
    for(i = 0; i<MCC_ATTR_MAX_RECEIVE_ENDPOINTS; i++) {

        if(MCC_ENDPOINTS_EQUAL(bookeeping_data->endpoint_table[i].endpoint, endpoint))
#if (MCC_OS_USED == MCC_LINUX)
            return (MCC_RECEIVE_LIST *)MQX_TO_VIRT(&bookeeping_data->endpoint_table[i].list);
#elif (MCC_OS_USED == MCC_MQX)
            return (MCC_RECEIVE_LIST *)&bookeeping_data->endpoint_table[i].list;
#endif
    }
    return null;
}

/*!
 * \brief This function queues a signal
 *  
 * Signal circular queue rules:
 * 	tail points to next free slot
 * 	head points to first occupied slot
 * 	head == tail indicates empty
 * 	(tail + 1) % len = fill
 * This method costs 1 slot since you need to differentiate
 * between full and empty (if you fill the last slot it looks
 * like empty since h == t)
 *
 * \param[in] core Core number.
 * \param[in] signal Signal to be queued.
 *  
 * \return MCC_SUCCESS
 * \return MCC_ERR_SQ_FULL (signal queue is full - no more that MCC_MAX_OUTSTANDING_SIGNALS items allowed)
 */
int mcc_queue_signal(MCC_CORE core, MCC_SIGNAL signal)
{
    MCC_DCACHE_INVALIDATE_MLINES((void*)&bookeeping_data->signal_queue_head[core], sizeof(unsigned int));
    MCC_DCACHE_INVALIDATE_MLINES((void*)&bookeeping_data->signal_queue_tail[core], sizeof(unsigned int));
    int tail = bookeeping_data->signal_queue_tail[core];
    int new_tail = tail == (MCC_MAX_OUTSTANDING_SIGNALS-1) ? 0 : tail+1;

    if(MCC_SIGNAL_QUEUE_FULL(core))
        return MCC_ERR_SQ_FULL;

    MCC_DCACHE_INVALIDATE_MLINES((void*)&bookeeping_data->signals_received[core][tail], sizeof(MCC_SIGNAL));
    bookeeping_data->signals_received[core][tail].type = signal.type;
    bookeeping_data->signals_received[core][tail].destination.core = signal.destination.core;
    bookeeping_data->signals_received[core][tail].destination.node = signal.destination.node;
    bookeeping_data->signals_received[core][tail].destination.port = signal.destination.port;

    bookeeping_data->signal_queue_tail[core] = new_tail;
    MCC_DCACHE_FLUSH_MLINES((void*)&bookeeping_data->signal_queue_tail[core], sizeof(unsigned int));
    MCC_DCACHE_FLUSH_MLINES((void*)&bookeeping_data->signals_received[core][tail], sizeof(MCC_SIGNAL));

    return MCC_SUCCESS;
}

/*!
 * \brief This function dequeues a signal
 * 
 * It dequeues a signal from the signal queue for the particular core.
 *    
 * \param[in] core Core number.
 * \param[in] signal Signal to be dequeued.
 *  
 * \return MCC_SUCCESS
 * \return MCC_ERR_SQ_EMPTY (signal queue is empty, nothing to dequeue)
 */
int mcc_dequeue_signal(MCC_CORE core, MCC_SIGNAL *signal)
{
    MCC_DCACHE_INVALIDATE_MLINES((void*)&bookeeping_data->signal_queue_head[core], sizeof(unsigned int));
    MCC_DCACHE_INVALIDATE_MLINES((void*)&bookeeping_data->signal_queue_tail[core], sizeof(unsigned int));
    int head = bookeeping_data->signal_queue_head[core];

    if(MCC_SIGNAL_QUEUE_EMPTY(core))
        return MCC_ERR_SQ_EMPTY;

    MCC_DCACHE_INVALIDATE_MLINES((void*)&bookeeping_data->signals_received[core][head], sizeof(MCC_SIGNAL));
    signal->type = bookeeping_data->signals_received[core][head].type;
    signal->destination.core = bookeeping_data->signals_received[core][head].destination.core;
    signal->destination.node = bookeeping_data->signals_received[core][head].destination.node;
    signal->destination.port = bookeeping_data->signals_received[core][head].destination.port;

    bookeeping_data->signal_queue_head[core] = head == (MCC_MAX_OUTSTANDING_SIGNALS-1) ? 0 : head+1;
    MCC_DCACHE_FLUSH_MLINES((void*)&bookeeping_data->signal_queue_head[core], sizeof(unsigned int));

    return MCC_SUCCESS;
}

