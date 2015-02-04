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
*   This file contains the code which implements a PCB driver
*   that sends MQX format packets over shared memory driver.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "io.h"
#include "io_prv.h"
#include "io_pcb.h"
#include "iopcbprv.h"
#include "pcb_shm.h"
#include "pcbshmv.h"
#include "bsp.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_shm_install
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*     This function is use to install the shared memory device.
*
*END*---------------------------------------------------------------------*/

_mqx_uint _io_pcb_shm_install
    (
        /* [IN] the name of this device */
        char    *device_name_ptr,

        /* [IN] the initialization record for this device */
        void       *init_ptr
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR info_ptr;
    uint32_t                    tmp;

    info_ptr = _mem_alloc_system_zero(sizeof(IO_PCB_SHM_INFO_STRUCT));
    if (info_ptr == NULL) {
        return(MQX_OUT_OF_MEMORY);
    }

    info_ptr->INIT = *((IO_PCB_SHM_INIT_STRUCT_PTR)init_ptr);

    /* Initialize the info structure */
    /*
    ** Set the Tx and Rx RINGPTR address base
    **
    */
    info_ptr->TX_RING_PTR = (IO_PCB_SHM_BUFFER_STRUCT_PTR)(
        ((IO_PCB_SHM_INIT_STRUCT_PTR)init_ptr)->TX_BD_ADDR);

    info_ptr->TX_RING_PTR = (IO_PCB_SHM_BUFFER_STRUCT_PTR)
        SHM_DESCR_ALIGN((uint32_t)info_ptr->TX_RING_PTR);
    tmp = (uint32_t)(IO_PCB_SHM_BUFFER_STRUCT_PTR)(((IO_PCB_SHM_INIT_STRUCT_PTR)
        init_ptr)->TX_LIMIT_ADDR);
    tmp -= (uint32_t)(info_ptr->TX_RING_PTR);
    tmp /= sizeof(IO_PCB_SHM_BUFFER_STRUCT);

    info_ptr->TXENTRIES = tmp;
    info_ptr->TX_LENGTH = tmp;
    info_ptr->TXNEXT = 0;
    info_ptr->TXLAST = 0;

    info_ptr->RX_RING_PTR = (IO_PCB_SHM_BUFFER_STRUCT_PTR)(
        ((IO_PCB_SHM_INIT_STRUCT_PTR)init_ptr)->RX_BD_ADDR);

    info_ptr->RX_RING_PTR = (IO_PCB_SHM_BUFFER_STRUCT_PTR)
        SHM_DESCR_ALIGN((uint32_t)info_ptr->RX_RING_PTR);

    tmp = (uint32_t)(IO_PCB_SHM_BUFFER_STRUCT_PTR)(((IO_PCB_SHM_INIT_STRUCT_PTR)
        init_ptr)->RX_LIMIT_ADDR);
    tmp -= (uint32_t)(info_ptr->RX_RING_PTR);
    tmp /= sizeof(IO_PCB_SHM_BUFFER_STRUCT);
    info_ptr->RXENTRIES = tmp;
    info_ptr->RX_LENGTH = tmp;
    info_ptr->RXNEXT = 0;
    info_ptr->RXLAST = 0;

    return (_io_pcb_dev_install(device_name_ptr,
        _io_pcb_shm_open,
        _io_pcb_shm_close,
        _io_pcb_shm_read,
        _io_pcb_shm_write,
        _io_pcb_shm_ioctl,
        _io_pcb_shm_uninstall,
        (void *)info_ptr));

}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_shm_open
* Returned Value  : _mqx_int result MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_shm_open
    (
        /* [IN] the file handle */
        FILE_DEVICE_STRUCT_PTR fd_ptr,

        /* [IN] the rest of the filename used to open the device */
        char              *open_name_ptr,

        /* [IN] the open flags for this device */
        char              *open_mode_flags
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR  info_ptr;
    IO_PCB_DEVICE_STRUCT_PTR    dev_ptr;

    dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    fd_ptr->DEV_DATA_PTR = dev_ptr->DRIVER_INIT_PTR;
    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;

    fd_ptr->FLAGS |= IO_FLAG_IS_PCB_DEVICE;
    info_ptr->READ_CALLBACK_FUNCTION = NULL;
    _lwsem_create(&info_ptr->READ_LWSEM, 0);
    _lwsem_create(&info_ptr->WRITE_LWSEM, 0);
    _queue_init(&info_ptr->READ_QUEUE, 0);
    _queue_init(&info_ptr->WRITE_QUEUE, 0);

    return(MQX_OK);

}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_shm_close
* Returned Value  : _mqx_int result   MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_shm_close
    (
        /* [IN] the file handle */
        FILE_DEVICE_STRUCT_PTR fd_ptr
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR  info_ptr;
    IO_PCB_STRUCT_PTR           pcb_ptr;
    IO_PCB_SHM_INIT_STRUCT_PTR  init_ptr;

#if MQX_CHECK_ERRORS
    if (!(fd_ptr->FLAGS & IO_FLAG_IS_PCB_DEVICE)) {
        fd_ptr->ERROR = IO_PCB_NOT_A_PCB_DEVICE;
        return(IO_ERROR);
    }
#endif
    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;
    init_ptr = &info_ptr->INIT;

    if (!_int_install_isr(init_ptr->RX_VECTOR,
        info_ptr->RX_OLDISR_PTR, info_ptr->RX_OLDISR_DATA))
    {
        return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
    }

    if (init_ptr->TX_VECTOR) {
        if (!_int_install_isr(init_ptr->TX_VECTOR, info_ptr->TX_OLDISR_PTR,
            info_ptr->TX_OLDISR_DATA))
        {
            return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
        }
    }

    _lwsem_destroy(&info_ptr->READ_LWSEM);
    _lwsem_destroy(&info_ptr->WRITE_LWSEM);

    while (_queue_get_size(&info_ptr->WRITE_QUEUE)) {
        pcb_ptr = (IO_PCB_STRUCT_PTR)
            ((void *)_queue_dequeue(&info_ptr->WRITE_QUEUE));
        _io_pcb_free(pcb_ptr);
    }

    while (_queue_get_size(&info_ptr->READ_QUEUE)) {
        pcb_ptr = (IO_PCB_STRUCT_PTR)
            ((void *)_queue_dequeue(&info_ptr->READ_QUEUE));
        _io_pcb_free(pcb_ptr);
    }

    return(MQX_OK);

}


/*READ*---------------------------------------------------------------------
*
* Function Name   : _io_pcb_shm_read
* Returned Value  : _mqx_int result MQX_OK or an error code.
* Comments        :
*     This function reads a packet directly
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_shm_read
    (
        /* [IN] the file descriptor */
        FILE_DEVICE_STRUCT_PTR  fd_ptr,

        /* [IN] the pcb address from which to write data */
        IO_PCB_STRUCT_PTR  *pcb_ptr
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR info_ptr;

    info_ptr = fd_ptr->DEV_DATA_PTR;

    _int_disable();
    if (fd_ptr->FLAGS & IO_O_NONBLOCK) {
        if (! _queue_get_size(&info_ptr->READ_QUEUE)) {
            *pcb_ptr = NULL;
            _int_enable();
            return(MQX_OK);
        }
    }
    _lwsem_wait(&info_ptr->READ_LWSEM);
    *pcb_ptr = (IO_PCB_STRUCT_PTR) ((void *)_queue_dequeue(&info_ptr->READ_QUEUE));
    _int_enable();

    return(MQX_OK);

}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_shm_write
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*     This function sends the pcb to the pcb output task.
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_shm_write
    (
        /* [IN] the file descriptor */
        FILE_DEVICE_STRUCT_PTR  fd_ptr,

        /* [IN] the pcb address from which to write data */
        IO_PCB_STRUCT_PTR       pcb_ptr
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR info_ptr;

    info_ptr = fd_ptr->DEV_DATA_PTR;


    _queue_enqueue((QUEUE_STRUCT_PTR)((void *)&info_ptr->WRITE_QUEUE),
        (QUEUE_ELEMENT_STRUCT_PTR)((void *)&pcb_ptr->QUEUE));
    _DCACHE_FLUSH_MLINES(pcb_ptr, sizeof(IO_PCB_STRUCT));
    _io_pcb_shm_tx((void *)info_ptr);

    return(MQX_OK);
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _io_pcb_shm_tx
*  Returned Value : void
*  Comments       :
*        Packet transmission ISR.
*
*END*-----------------------------------------------------------------*/

void _io_pcb_shm_tx
    (
        /* [IN] the device info */
        void   *handle
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR   info_ptr;
    IO_PCB_STRUCT_PTR            pcb_ptr;
    IO_PCB_SHM_BUFFER_STRUCT_PTR bd_ptr;
    IO_PCB_SHM_INIT_STRUCT_PTR   init_ptr;
    IO_PCB_FRAGMENT_STRUCT_PTR   frag_ptr;
    uint32_t                      work = FALSE;
    uint16_t                      num_frags;

    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)handle;
    init_ptr = &info_ptr->INIT;

    while (TRUE) {

        _int_disable();

        /* Check if queue empty */
        if (!_queue_get_size(&info_ptr->WRITE_QUEUE)) {
          _int_enable();
            break;
        }

        /* Check if queue empty */
        if (!(info_ptr->TXENTRIES)) {
          _int_enable();
          info_ptr->TX_BD_RUNOVER++;
          break;
        }

        /* Get the packet from output queue */
        pcb_ptr = (IO_PCB_STRUCT_PTR)((void *)
            _queue_dequeue(&info_ptr->WRITE_QUEUE));

        /* Get the next buffer descriptor */
        bd_ptr = &info_ptr->TX_RING_PTR[info_ptr->TXNEXT];

        /* Flush packet */
        num_frags = pcb_ptr->NUMBER_OF_FRAGMENTS;
        for(frag_ptr = (IO_PCB_FRAGMENT_STRUCT_PTR) &(pcb_ptr->FRAGMENTS[0]); num_frags; num_frags--, frag_ptr++)
        {
             _DCACHE_FLUSH_MLINES(frag_ptr->FRAGMENT,frag_ptr->LENGTH);
            frag_ptr->FRAGMENT = _bsp_vtop(frag_ptr->FRAGMENT);
        }


        /* Set the buffer descriptor */
        bd_ptr->PACKET_PTR = (IO_PCB_STRUCT_PTR) _bsp_vtop(pcb_ptr);
        bd_ptr->CONTROL = (IO_PCB_SHM_BUFFER_OWN|IO_PCB_SHM_BUFFER_ALOCATED);
        _DCACHE_FLUSH_LINE(bd_ptr);

        /* Update Info structure  */
        info_ptr->TXNEXT = NEXT_INDEX(info_ptr->TXNEXT, info_ptr->TX_LENGTH);
        info_ptr->TXENTRIES--;
        work = TRUE;
        _int_enable();

    }

    if (work) {
        /* Trigger remote ISR */
        (*init_ptr->INT_TRIGGER)(init_ptr->REMOTE_RX_VECTOR);
    }
}


/*NOTIFIER*-------------------------------------------------------------
*
*  Function Name  : io_pcb_shm_tx_isr
*  Returned Value : void
*  Comments       :
*        Packet transmission ISR.
*
*END*-----------------------------------------------------------------*/

void _io_pcb_shm_tx_isr
    (
        /* [IN] the info structure */
        void       *handle
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR     info_ptr;
    IO_PCB_STRUCT_PTR              pcb_ptr;
    IO_PCB_SHM_BUFFER_STRUCT_PTR   bd_ptr;
    IO_PCB_FRAGMENT_STRUCT_PTR     frag_ptr;
    uint16_t                        num_frags;
    uint32_t                        cntrl;

    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)handle;
    _int_disable();

    while (info_ptr->TXENTRIES < info_ptr->TX_LENGTH) {

        /* Get the address of the Tx descriptor */
        bd_ptr = &info_ptr->TX_RING_PTR[info_ptr->TXLAST];
        _DCACHE_INVALIDATE_LINE(bd_ptr);
        pcb_ptr = (IO_PCB_STRUCT_PTR) _bsp_ptov(bd_ptr->PACKET_PTR);
        cntrl = bd_ptr->CONTROL;

        /* Make sure the buffer is released by remote CPU */
        if (cntrl != IO_PCB_SHM_BUFFER_ALOCATED) {
            break;
        }

       // Have to restore virtual addresses to free fragments
       num_frags = pcb_ptr->NUMBER_OF_FRAGMENTS;
       for(frag_ptr = (IO_PCB_FRAGMENT_STRUCT_PTR) &(pcb_ptr->FRAGMENTS[0]); num_frags; num_frags--, frag_ptr++)
       {
            frag_ptr->FRAGMENT = _bsp_ptov(frag_ptr->FRAGMENT);
       }

        /* Free PCB */
        _io_pcb_free(pcb_ptr);

        /* Update info */
        info_ptr->TX_PACKETS++;
        info_ptr->TXLAST = NEXT_INDEX(info_ptr->TXLAST, info_ptr->TX_LENGTH);
        info_ptr->TXENTRIES++;

    }

    /* Check if there is more to send */
    _io_pcb_shm_tx(handle);

    /* Enable Interrupts */
    _int_enable();

}


/*NOTIFIER*-------------------------------------------------------------
*
*  Function Name  : io_pcb_shm_rx_isr
*  Returned Value : void
*  Comments       :
*        Packet reception ISR.
*
*END*-----------------------------------------------------------------*/

void _io_pcb_shm_rx_isr
    (
        /* [IN] the info structure */
        void       *handle
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR      info_ptr;
    IO_PCB_SHM_INIT_STRUCT_PTR      init_ptr;
    IO_PCB_STRUCT_PTR               local_pcb_ptr;
    IO_PCB_STRUCT_PTR               remote_pcb_ptr;
    IO_PCB_FRAGMENT_STRUCT_PTR      frag_ptr;
    IO_PCB_SHM_BUFFER_STRUCT_PTR    bd_ptr;
    unsigned char                   *data_addr;
    uint32_t                        data_length;
    uint32_t                        cntrl;
    uint16_t                        num_frags;
    uint32_t                        max_size;
    bool                            discard = FALSE;

    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)handle;
    init_ptr = &info_ptr->INIT;

    /* Get the next RX buffer descriptor */
    bd_ptr = &info_ptr->RX_RING_PTR[info_ptr->RXNEXT];
    _DCACHE_INVALIDATE_LINE(bd_ptr);
    cntrl = bd_ptr->CONTROL;
    remote_pcb_ptr = (IO_PCB_STRUCT_PTR) _bsp_ptov(bd_ptr->PACKET_PTR);
    _DCACHE_INVALIDATE_MBYTES(remote_pcb_ptr, sizeof(*remote_pcb_ptr));
    num_frags = remote_pcb_ptr->NUMBER_OF_FRAGMENTS;

    /* Disable interrupts */
    _int_disable();

    while(cntrl == (IO_PCB_SHM_BUFFER_OWN|IO_PCB_SHM_BUFFER_ALOCATED)){

        discard = FALSE;
        /* Get a PCB */
        local_pcb_ptr = _io_pcb_alloc(info_ptr->READ_PCB_POOL, FALSE);
        if ((local_pcb_ptr == NULL)) {
            break;
        }
        data_addr = local_pcb_ptr->FRAGMENTS[0].FRAGMENT;
        data_length = ((IO_PCB_SHM_INIT_STRUCT_PTR) &info_ptr->INIT)->INPUT_MAX_LENGTH;
        max_size = ((IO_PCB_SHM_INIT_STRUCT_PTR) &info_ptr->INIT)->INPUT_MAX_LENGTH;

        /* Copy packet */
        for(frag_ptr = (IO_PCB_FRAGMENT_STRUCT_PTR) &(remote_pcb_ptr->FRAGMENTS[0]); num_frags; num_frags--, frag_ptr++)
        {
            if(frag_ptr->LENGTH > max_size){
                discard = TRUE;
                break;
            }

            _DCACHE_INVALIDATE_MBYTES(_bsp_ptov((void *)frag_ptr->FRAGMENT), frag_ptr->LENGTH);
            _mem_copy(_bsp_ptov((void *)frag_ptr->FRAGMENT), (void *)data_addr, frag_ptr->LENGTH);
             data_addr += frag_ptr->LENGTH;
             data_length -= frag_ptr->LENGTH;
        }

        local_pcb_ptr->FRAGMENTS[0].LENGTH = max_size - data_length;
        if (info_ptr->READ_CALLBACK_FUNCTION) {
            (*info_ptr->READ_CALLBACK_FUNCTION)(info_ptr->FD,
                local_pcb_ptr);
        } else {
            _queue_enqueue((QUEUE_STRUCT_PTR)&info_ptr->READ_QUEUE,
                (QUEUE_ELEMENT_STRUCT_PTR)&local_pcb_ptr->QUEUE);
            _lwsem_post(&info_ptr->READ_LWSEM);
        }

        /* Set the buffer pointer and control bits */
        bd_ptr->CONTROL &= IO_PCB_SHM_BUFFER_ALOCATED;
        _DCACHE_FLUSH_LINE(bd_ptr);
        /* Update Info structure  */
        info_ptr->RXNEXT = NEXT_INDEX(info_ptr->RXNEXT, info_ptr->RX_LENGTH);
        info_ptr->RXENTRIES--;
        /* Get the next RX buffer descriptor */
        bd_ptr = &info_ptr->RX_RING_PTR[info_ptr->RXNEXT];
        _DCACHE_INVALIDATE_LINE(bd_ptr);
        cntrl = bd_ptr->CONTROL;
        remote_pcb_ptr = (IO_PCB_STRUCT_PTR) _bsp_ptov(bd_ptr->PACKET_PTR);
        _DCACHE_INVALIDATE_MBYTES(remote_pcb_ptr, sizeof(*remote_pcb_ptr));
        num_frags = remote_pcb_ptr->NUMBER_OF_FRAGMENTS;
    }

    if (init_ptr->TX_VECTOR == 0) {
        _io_pcb_shm_tx_isr(handle);
    }

    /* Reception successful  */
    if (!discard) {
        /* Trigger remote side */
        (*init_ptr->INT_TRIGGER)(init_ptr->REMOTE_TX_VECTOR);
    }

    _int_enable();

}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_shm_ioctl
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_shm_ioctl
    (
        /* [IN] the file handle for the device */
        FILE_DEVICE_STRUCT_PTR fd_ptr,

        /* [IN] the ioctl command */
        _mqx_uint              cmd,

        /* [IN] the ioctl parameters */
        void                  *param_ptr
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR  info_ptr;
    IO_PCB_STRUCT_PTR           pcb_ptr;
    _mqx_uint                   result = MQX_OK;
    _psp_code_addr              old_value;
    _psp_code_addr_ptr          pc_ptr = (_psp_code_addr_ptr)param_ptr;
    _psp_data_addr_ptr          pd_ptr = (_psp_data_addr_ptr)param_ptr;
    bool                    *bool_param_ptr;
    IO_PCB_SHM_INIT_STRUCT_PTR  init_ptr;

    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;
    init_ptr = &info_ptr->INIT;

    switch (cmd) {

        case IO_PCB_IOCTL_ENQUEUE_READQ:
            pcb_ptr = (IO_PCB_STRUCT_PTR)*pd_ptr;
            _queue_enqueue((QUEUE_STRUCT_PTR)&info_ptr->READ_QUEUE,
                (QUEUE_ELEMENT_STRUCT_PTR)&pcb_ptr->QUEUE);
            _lwsem_post(&info_ptr->READ_LWSEM);
            break;

        case IO_PCB_IOCTL_READ_CALLBACK_SET:
            old_value = (_psp_code_addr)info_ptr->READ_CALLBACK_FUNCTION;
            info_ptr->READ_CALLBACK_FUNCTION = (void (_CODE_PTR_)(
                FILE_DEVICE_STRUCT_PTR, IO_PCB_STRUCT_PTR))*pc_ptr;
            *pc_ptr = old_value;
            break;

        case IO_PCB_IOCTL_SET_INPUT_POOL:
            old_value = (_psp_code_addr)info_ptr->READ_PCB_POOL;
            info_ptr->READ_PCB_POOL = (_io_pcb_pool_id)*pc_ptr;
            *pc_ptr = old_value;
            info_ptr->FD = fd_ptr;
            break;

        case IO_PCB_IOCTL_START:
             info_ptr->RX_OLDISR_PTR  = _int_get_isr( init_ptr->RX_VECTOR);
             info_ptr->RX_OLDISR_DATA = _int_get_isr_data(init_ptr->RX_VECTOR);
             info_ptr->TX_OLDISR_PTR  = _int_get_isr( init_ptr->TX_VECTOR);
             info_ptr->TX_OLDISR_DATA = _int_get_isr_data(init_ptr->TX_VECTOR);

#if BSPCFG_IO_PCB_SHM_SUPPORT
             if (_bsp_io_pcb_shm_int_install(init_ptr,info_ptr)!=MQX_OK) {
                  _mem_free(info_ptr);
                  return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
              }
#else
            /* Install rx ISR */
            if (!_int_install_isr(init_ptr->RX_VECTOR, _io_pcb_shm_rx_isr, info_ptr))
            {
                _mem_free(info_ptr);
                return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
            }

            /* Install the tx finished ISR */
            if (!_int_install_isr(init_ptr->TX_VECTOR, _io_pcb_shm_tx_isr, info_ptr))
            {
                _mem_free(info_ptr);
                return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
            }
#endif
            break;

        case IO_PCB_IOCTL_UNPACKED_ONLY:
            bool_param_ptr = (bool *)param_ptr;
            *bool_param_ptr = TRUE;
            break;

        default:
            /* result = _io_ioctl(info_ptr->FD, cmd, param_ptr); */
            break;

    }
    return result;

}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_shm_uninstall
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_shm_uninstall
    (
        IO_PCB_DEVICE_STRUCT_PTR dev_ptr
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR  info_ptr;
    _mqx_int                    result = MQX_OK;
    _mqx_uint                   valid;

    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)dev_ptr->DRIVER_INIT_PTR;
    valid = info_ptr->WRITE_LWSEM.VALID;
    if (valid) {
         return IO_ERROR_DEVICE_BUSY;
    }

    _mem_free(info_ptr);

     return result;

}

/* EOF */
