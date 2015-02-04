/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
*   This file contains the Vybrid_m4 core specific code for the
*   shared memory driver.
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
* Function Name   : _bsp_io_pcb_shm_int_trigger
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*     This function triggers an interrupt to the destination processor.
*
*END*---------------------------------------------------------------------*/

void _bsp_io_pcb_shm_int_trigger(uint32_t vector)
{
    MSCM_IRCPGIR = MSCM_IRCPGIR_TLF(1) | MSCM_IRCPGIR_INTID(vector - INT_CPU_to_CPU_int0);
}


/*NOTIFIER*-------------------------------------------------------------
*
*  Function Name  : _bsp_io_pcb_shm_rx_isr
*  Returned Value : void
*  Comments       :
*        Packet reception ISR.
*
*END*-----------------------------------------------------------------*/

static void _bsp_io_pcb_shm_rx_isr
    (
        /* [IN] the info structure */
        void   *handle
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)handle;

    /* clear the inter-core interrupt */
#if PSP_MQX_CPU_IS_VYBRID_A5
    MSCM_IRCP0IR = (1 << (info_ptr->INIT.RX_VECTOR - INT_CPU_to_CPU_int0));
#else
    MSCM_IRCP1IR = (1 << (info_ptr->INIT.RX_VECTOR - NVIC_CPU_to_CPU_int0));
#endif
    _io_pcb_shm_rx_isr(handle);
}

/*NOTIFIER*-------------------------------------------------------------
*
*  Function Name  : _bsp_io_pcb_shm_tx_isr
*  Returned Value : void
*  Comments       :
*        Packet transmission ISR.
*
*END*-----------------------------------------------------------------*/

static void _bsp_io_pcb_shm_tx_isr
    (
        /* [IN] the info structure */
        void   *handle
    )
{
    IO_PCB_SHM_INFO_STRUCT_PTR    info_ptr = (IO_PCB_SHM_INFO_STRUCT_PTR)handle;

    /* clear the inter-core interrupt */
#if PSP_MQX_CPU_IS_VYBRID_A5
    MSCM_IRCP0IR = (1 << (info_ptr->INIT.TX_VECTOR - INT_CPU_to_CPU_int0));
#else
    MSCM_IRCP1IR = (1 << (info_ptr->INIT.TX_VECTOR - NVIC_CPU_to_CPU_int0));
#endif
    _io_pcb_shm_tx_isr(handle);
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _bsp_io_pcb_shm_int_install
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*     This function is use to install the shared memory device ISRs.
*
*END*---------------------------------------------------------------------*/
#if PSP_MQX_CPU_IS_VYBRID_A5
_mqx_uint _bsp_io_pcb_shm_int_install
    (
        /* [IN] the initialization record for this device */
        IO_PCB_SHM_INIT_STRUCT_PTR init_ptr,

        /* [IN] the context record for this device */
        IO_PCB_SHM_INFO_STRUCT_PTR info_ptr
    )
{
    if (!_int_install_isr(init_ptr->RX_VECTOR, _bsp_io_pcb_shm_rx_isr, info_ptr))
    {
        return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
    }

    /* Install the tx finished ISR */
    if (!_int_install_isr(init_ptr->TX_VECTOR, _bsp_io_pcb_shm_tx_isr, info_ptr))
    {
        return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
    }

    _bsp_int_init(init_ptr->RX_VECTOR, BSPCFG_IO_PCB_SHM_RX_PRIO, 0, TRUE);
    _bsp_int_init(init_ptr->TX_VECTOR, BSPCFG_IO_PCB_SHM_TX_PRIO, 0 , TRUE);

    return MQX_OK;
}
#else
_mqx_uint _bsp_io_pcb_shm_int_install
    (
        /* [IN] the initialization record for this device */
        IO_PCB_SHM_INIT_STRUCT_PTR init_ptr,

        /* [IN] the context record for this device */
        IO_PCB_SHM_INFO_STRUCT_PTR info_ptr
    )
{
    if (!_int_install_isr(init_ptr->RX_VECTOR, _bsp_io_pcb_shm_rx_isr, info_ptr))
    {
        return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
    }

    /* Install the tx finished ISR */
    if (!_int_install_isr(init_ptr->TX_VECTOR, _bsp_io_pcb_shm_tx_isr, info_ptr))
    {
        return MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED;
    }

    _bsp_int_init(init_ptr->RX_VECTOR, BSPCFG_IO_PCB_SHM_RX_PRIO, 0, TRUE);
    _bsp_int_init(init_ptr->TX_VECTOR, BSPCFG_IO_PCB_SHM_TX_PRIO, 0 , TRUE);

    return MQX_OK;
}
#endif
