/*HEADER**********************************************************************
*
* Copyright 2008-2013 Freescale Semiconductor, Inc.
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
*   Revision History:
*   Date             Version  Changes
*   ---------        -------  -------
*   Jan.20/04        2.50     Initial version
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "fsl_flexcan_int.h"

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Returns pointer to base address of the desired CAN device.
 *
 * @param   dev_num      FlexCAN device number.
 * @return  Pointer to desired CAN device or NULL if not present.
 */
void *_bsp_get_flexcan_base_address
(
    uint8_t dev_num
)
{
    void   *addr;

    switch(dev_num) {
    case 0:
        addr = (void *)CAN0_BASE_PTR;
        break;
    case 1:
        addr = (void *)CAN1_BASE_PTR;
        break;
    default:
        addr = NULL;
    }

    return addr;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_flexcan_vector
* Returned Value   : MQX vector number for specified interrupt
* Comments         :
*    This function returns index into MQX interrupt vector table for
*    specified flexcan interrupt. If not known, returns 0.
*
*END*********************************************************************/
/*!
 * @brief Returns index into MQX interrupt vector table for specified flexcan interrupt.
 *
 * @param   dev_num             FlexCAN device number.
 * @param   vector_type         FlexCAN interrupt vector type.
 * @param   vector_index        FlexCAN interrupt vector index.
 * @return  MQX vector number for specified interrupt. If not known, returns 0.
 */
IRQInterruptIndex _bsp_get_flexcan_vector 
(
    uint8_t dev_num,
    uint8_t vector_type,
    uint32_t vector_index
)
{
    IRQInterruptIndex index = (IRQInterruptIndex)0;

    switch (dev_num)
    {
        case 0:
#if defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_TWR_VF65GS10_A5)
            index = INT_FlexCAN0;
#else
            switch (vector_type)
            {
                case FLEXCAN_INT_BUF:
                     index = INT_CAN0_ORed_Message_buffer;
                     break;
                case FLEXCAN_INT_ERR:
                     index = INT_CAN0_Error;
                     break;
                case FLEXCAN_INT_BOFF:
                     index = INT_CAN0_Bus_Off;
                     break;
                default:
                     break;
            }
#endif
            break;
      case 1:
#if defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_TWR_VF65GS10_A5)
            index = INT_FlexCAN1;
#else
         switch (vector_type)
         {
            case FLEXCAN_INT_BUF:
                 index = INT_CAN1_ORed_Message_buffer;
                 break;
            case FLEXCAN_INT_ERR:
                 index = INT_CAN1_Error;
                 break;
            case FLEXCAN_INT_BOFF:
                 index = INT_CAN1_Bus_Off;
                 break;
            default:
                 break;
         }
#endif
         break;
      default: break;
    }

    return index;
}

/*!
 * @brief Enables interrupt for requested mailbox.
 *
 * @param   dev_num                FlexCAN device number.
 * @param   mailbox_number         Mailbox number.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Int_enable
(
    uint8_t dev_num,
    uint32_t mailbox_number
)
{
    volatile IRQInterruptIndex     index;

    if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) ) 
    {
        return (FLEXCAN_INVALID_MAILBOX);
    }

    index = _bsp_get_flexcan_vector(dev_num, FLEXCAN_INT_BUF, mailbox_number);
    if (0 == index)
    {
        return (FLEXCAN_INT_ENABLE_FAILED);
    }
         
    if (_bsp_int_init(index, FLEXCAN_MESSBUF_INT_LEVEL, FLEXCAN_MESSBUF_INT_SUBLEVEL, TRUE) !=
        MQX_OK)
    {
        return (FLEXCAN_INT_ENABLE_FAILED);
    }

    return( FLEXCAN_OK );
}

/*!
 * @brief Masks (disables) interrupt for requested mailbox.
 *
 * @param   dev_num                FlexCAN device number.
 * @param   mailbox_number         Mailbox number.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Int_disable
(
    uint8_t dev_num,
    uint32_t mailbox_number
)
{
    volatile IRQInterruptIndex     index;

    if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
    {
        return (FLEXCAN_INVALID_MAILBOX);
    }

    index = _bsp_get_flexcan_vector(dev_num, FLEXCAN_INT_BUF, mailbox_number);
    if (0 == index)
    {
        return (FLEXCAN_INT_DISABLE_FAILED);
    }

    // Disable the interrupt
    if (_bsp_int_init(index, FLEXCAN_MESSBUF_INT_LEVEL, FLEXCAN_MESSBUF_INT_SUBLEVEL, FALSE) !=
        MQX_OK)
    {
        return (FLEXCAN_INT_DISABLE_FAILED);
    }

    return (FLEXCAN_OK);
}

/*!
 * @brief Installs interrupt handler for requested mailbox.
 *
 * @param   dev_num                FlexCAN device number.
 * @param   mailbox_number         Mailbox number.
 * @param   isr                    Interrupt service routine.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Install_isr
(
    uint8_t       dev_num,
    uint32_t      mailbox_number,
    INT_ISR_FPTR isr
)
{
    uint32_t   return_code = FLEXCAN_OK;
    INT_ISR_FPTR result;
    volatile CAN_MemMapPtr                 can_reg_ptr;
    volatile IRQInterruptIndex     index;

    can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
    if (NULL == can_reg_ptr)  
    {
        return (FLEXCAN_INVALID_ADDRESS);
    }

    if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) ) 
    {
        return (FLEXCAN_INVALID_MAILBOX);
    }

    index = _bsp_get_flexcan_vector(dev_num, FLEXCAN_INT_BUF, mailbox_number);
    if (0 == index)
    {
        return (FLEXCAN_INT_INSTALL_FAILED);
    }

    // Install ISR
    result = _int_install_isr(index, isr, (void *)can_reg_ptr);
    if(result == (INT_ISR_FPTR)NULL)
    {
        return_code = _task_get_error();
    }

    return return_code;
}

#if !(defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_TWR_VF65GS10_A5))
/*!
 * @brief Unmasks (enables) error, wake up & Bus off interrupts.
 *
 * @param   dev_num          FlexCAN device number.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Error_int_enable
(
    uint8_t dev_num
)
{
    volatile IRQInterruptIndex     index;

    index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_ERR, 0);
    if (0 == index)
    {
        return (FLEXCAN_INT_ENABLE_FAILED);
    }

    if (_bsp_int_init(index, FLEXCAN_ERROR_INT_LEVEL, FLEXCAN_ERROR_INT_SUBLEVEL, TRUE) !=
        MQX_OK)
    {
        return (FLEXCAN_INT_ENABLE_FAILED);
    }

    index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
    if (0 == index)
    {
        return (FLEXCAN_INT_ENABLE_FAILED);
    }

    if (_bsp_int_init(index, FLEXCAN_BUSOFF_INT_LEVEL, FLEXCAN_BUSOFF_INT_SUBLEVEL, TRUE) !=
        MQX_OK)
    {
        return (FLEXCAN_INT_ENABLE_FAILED);
    }

    return ( FLEXCAN_OK );
}

/*!
 * @brief Masks (disables) error, wake up & Bus off interrupts.
 *
 * @param   dev_num           FlexCAN device number.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Error_int_disable
(
    uint8_t dev_num
)
{
    volatile CAN_MemMapPtr                 can_reg_ptr;
    volatile IRQInterruptIndex     index;

    can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
    if (NULL == can_reg_ptr)  
    {
        return (FLEXCAN_INVALID_ADDRESS);
    }

    // BOFFMSK = 0x1, ERRMSK = 0x1
    can_reg_ptr->CTRL1 &= ~(CAN_CTRL1_BOFFREC_MASK | CAN_CTRL1_ERRMSK_MASK);

    index = _bsp_get_flexcan_vector(dev_num, FLEXCAN_INT_ERR, 0);
    if (0 == index)
    {
        return (FLEXCAN_INT_DISABLE_FAILED);
    }

    if (_bsp_int_init(index, FLEXCAN_ERROR_INT_LEVEL, FLEXCAN_ERROR_INT_SUBLEVEL, FALSE) !=
        MQX_OK)
    {
        return (FLEXCAN_INT_DISABLE_FAILED);
    }

    index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
    if (0 == index)
    {
        return (FLEXCAN_INT_DISABLE_FAILED);
    }

    if (_bsp_int_init(index, FLEXCAN_BUSOFF_INT_LEVEL, FLEXCAN_BUSOFF_INT_SUBLEVEL, FALSE) !=
        MQX_OK)
    {
        return (FLEXCAN_INT_DISABLE_FAILED);
    }

    return ( FLEXCAN_OK );
}

/*!
 * @brief Installs interrupt handler for a flexcan error.
 *
 * @param   dev_num                FlexCAN device number.
 * @param   isr                    Interrupt service routine.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Install_isr_err_int
(
    uint8_t       dev_num,
    INT_ISR_FPTR isr
)
{
    uint32_t   return_code = FLEXCAN_OK;
    INT_ISR_FPTR result;
    volatile CAN_MemMapPtr                 can_reg_ptr;
    volatile IRQInterruptIndex     index;

    can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
    if (NULL == can_reg_ptr)  
    {
        return (FLEXCAN_INVALID_ADDRESS);
    }

    index = _bsp_get_flexcan_vector(dev_num, FLEXCAN_INT_ERR, 0);
    if (0 == index)
    {
        return (FLEXCAN_INT_INSTALL_FAILED);
    }

    result = _int_install_isr(index, isr, (void *)can_reg_ptr); 
    if(result == (INT_ISR_FPTR)NULL)
    {
        return_code = _task_get_error();
    }

    return return_code;
}
 
/*!
 * @brief Installs interrupt handler for a flexcan bus off.
 *
 * @param   dev_num                FlexCAN device number.
 * @param   isr                    Interrupt service routine.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Install_isr_boff_int
(
    uint8_t       dev_num,
    INT_ISR_FPTR isr
)
{
    uint32_t   return_code = FLEXCAN_OK;
    INT_ISR_FPTR result;
    volatile CAN_MemMapPtr                 can_reg_ptr;
    volatile IRQInterruptIndex     index;

    can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
    if (NULL == can_reg_ptr)  
    {
        return (FLEXCAN_INVALID_ADDRESS);
    }

    index = _bsp_get_flexcan_vector(dev_num, FLEXCAN_INT_BOFF, 0);
    if (0 == index)
    {
        return (FLEXCAN_INT_INSTALL_FAILED);
    }

    result = _int_install_isr(index, isr, (void *)can_reg_ptr); 
    if(result == (INT_ISR_FPTR)NULL)
    {
        return_code = _task_get_error();
    }

    return return_code;
}

/*!
 * @brief Installs interrupt handler for a flexcan wake-up.
 *
 * @param   dev_num                FlexCAN device number.
 * @param   isr                    Interrupt service routine.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Install_isr_wake_int
(
    uint8_t       dev_num,
    INT_ISR_FPTR isr
)
{
    uint32_t   return_code = FLEXCAN_OK;
    INT_ISR_FPTR result;
    volatile CAN_MemMapPtr                 can_reg_ptr;
    volatile IRQInterruptIndex     index;

    can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
    if (NULL == can_reg_ptr)  
    {
        return (FLEXCAN_INVALID_ADDRESS);
    }

    index = _bsp_get_flexcan_vector(dev_num, FLEXCAN_INT_WAKEUP, 0);
    if (0 == index)
    {
        return (FLEXCAN_INT_INSTALL_FAILED);
    }

    result = _int_install_isr(index, isr, (void *)can_reg_ptr); 
    if(result == (INT_ISR_FPTR)NULL)
    {
        return_code = _task_get_error();
    }

    return return_code;
}
#endif

/*!
 * @brief Services FlexCAN interrupt routine.
 *
 * @param   can_ptr                FlexCAN base address.
 * @return  0 if successful; non-zero failed.
 */
void _FLEXCAN_ISR
(
    void   *can_ptr
)
{
    volatile CAN_MemMapPtr          can_reg_ptr;
    volatile uint32_t               tmp_reg;

    can_reg_ptr = (CAN_MemMapPtr)can_ptr;

    // Get the interrupt flag
    tmp_reg = (can_reg_ptr->IFLAG1 & CAN_IMASK1_BUFLM_MASK);
    // Check Tx/Rx interrupt flag and clear the interrupt
    if(tmp_reg)
    {
    // Clear the interrupt and unlock message buffer
        can_reg_ptr->IFLAG1 |= tmp_reg;
        can_reg_ptr->TIMER;
    }

    // Clear all other interrupts in ERRSTAT register (Error, Busoff, Wakeup)
    tmp_reg = (can_reg_ptr->ESR1 & (CAN_ESR1_WAKINT_MASK | CAN_ESR1_ERRINT_MASK |
                                    CAN_ESR1_BOFFINT_MASK));
    if(tmp_reg)
    {
        can_reg_ptr->ESR1 |= tmp_reg;
    }

    return;
}

/*!
 * @brief Gets the FlexCAN interrupt status.
 *
 * @param   dev_num                FlexCAN device number.
 * @return  0 if successful; non-zero failed.
 */
uint32_t FLEXCAN_Int_status
(
    uint8_t dev_num
)
{
    volatile CAN_MemMapPtr                 can_reg_ptr;
    volatile uint32_t                      tmp_reg;

    can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
    if (NULL == can_reg_ptr)  
    {
        return (FLEXCAN_INVALID_ADDRESS);
    }

    // check Tx/Rx interrupt flag
    tmp_reg = (can_reg_ptr->IFLAG1 & CAN_IMASK1_BUFLM_MASK);
    if(tmp_reg)
    {
        return (~(CAN_ESR1_WAKINT_MASK | CAN_ESR1_ERRINT_MASK | CAN_ESR1_BOFFINT_MASK));
    }

    // check error interrupt
    tmp_reg = (can_reg_ptr->ESR1 & CAN_ESR1_ERRINT_MASK);
    if(tmp_reg)
    {
        return (CAN_ESR1_ERRINT_MASK);   
    }

    // check busoff interrupt
    tmp_reg = (can_reg_ptr->ESR1 & CAN_ESR1_BOFFINT_MASK);
    if(tmp_reg)
    {
        return (CAN_ESR1_BOFFINT_MASK);   
    }

#if !(defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_TWR_VF65GS10_A5))
    // check busoff interrupt
    tmp_reg = (can_reg_ptr->ESR1 & CAN_ESR1_WAKINT_MASK);
    if(tmp_reg)
    {
        return (CAN_ESR1_WAKINT_MASK);   
    }
#endif

    return (FLEXCAN_OK);
}
