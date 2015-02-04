/*HEADER**********************************************************************
*
* Copyright 2008-2010 Freescale Semiconductor, Inc.
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

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_int_enable
* Returned Value   : uint32_t 
* Comments         :
*    This function enables interrupt for requested mailbox 
*
*END*********************************************************************/
uint32_t FLEXCAN_Int_enable
(
   /* [IN] FlexCAN device number */
   uint8_t dev_num,
   /* [IN] mailbox number */
   uint32_t mailbox_number
)
{
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }
   
   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) ) 
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }
   
   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BUF, mailbox_number);
   if (0 == index)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }
         
   if (_flexcan_int_init (index, FLEXCAN_MESSBUF_INT_LEVEL, FLEXCAN_MESSBUF_INT_SUBLEVEL, TRUE) != MQX_OK)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }
   
   /* IMASK, unmask the message buffer */
   (can_reg_ptr->IMASK1)  |= (0x1 << mailbox_number);

   return( FLEXCAN_OK );
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Error_int_enable
* Returned Value   : uint32_t 
* Comments         :
*    This function unmasks (enables) error, wake up & Bus off interrupts.
*
*
*END*********************************************************************/
uint32_t FLEXCAN_Error_int_enable
(
   /* [IN] FlexCAN device number */
   uint8_t dev_num
)
{
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_ERR, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_ERROR_INT_LEVEL, FLEXCAN_ERROR_INT_SUBLEVEL, TRUE) != MQX_OK)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_BUSOFF_INT_LEVEL, FLEXCAN_BUSOFF_INT_SUBLEVEL, TRUE) != MQX_OK)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }
                        
    /* BOFFMSK = 0x1, ERRMSK = 0x1 */
   can_reg_ptr->CTRL1 |= (CAN_CTRL1_BOFFMSK_MASK | CAN_CTRL1_ERRMSK_MASK);

   return ( FLEXCAN_OK );
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_int_disable
* Returned Value   : uint32_t 
* Comments         :
*    This function masks (disables) interrupt for requested mailbox 
*
*END*********************************************************************/
uint32_t FLEXCAN_Int_disable
(
   /* [IN] FlexCAN device number */
   uint8_t dev_num,
   /* [IN] mailbox number */
   uint32_t mailbox_number
)
{
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) ) 
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   /* Start CR# 1750 */
   can_reg_ptr->IMASK1  &= ~(0x1 << mailbox_number);
   /* End CR# 1750 */

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BUF, mailbox_number);
   if (0 == index)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   /* Disable the interrupt */
   if (_flexcan_int_init (index, FLEXCAN_MESSBUF_INT_LEVEL, FLEXCAN_MESSBUF_INT_SUBLEVEL, FALSE) != MQX_OK)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }
      
   return (FLEXCAN_OK);
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Error_int_disable
* Returned Value   : uint32_t 
* Comments         :
*    This function masks (disables) error, wake up & Bus off interrupts 
*
*END*********************************************************************/
uint32_t FLEXCAN_Error_int_disable
(
   /* [IN] FlexCAN device number */
   uint8_t dev_num
)
{
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

    /* BOFFMSK = 0x1, ERRMSK = 0x1 */
   can_reg_ptr->CTRL1 &= ~(CAN_CTRL1_BOFFMSK_MASK | CAN_CTRL1_ERRMSK_MASK);

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_ERR, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_ERROR_INT_LEVEL, FLEXCAN_ERROR_INT_SUBLEVEL, FALSE) != MQX_OK)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_BUSOFF_INT_LEVEL, FLEXCAN_BUSOFF_INT_SUBLEVEL, FALSE) != MQX_OK)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   return ( FLEXCAN_OK );
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr
* Returned Value   : uint32_t 
* Comments         :
*    This function installs interrupt handler for requested mailbox 
*
*END*********************************************************************/
uint32_t FLEXCAN_Install_isr
(
   /* [IN] FlexCAN device number */
   uint8_t       dev_num,
   /* [IN] mailbox number */
   uint32_t      mailbox_number,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   uint32_t   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) ) 
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }
   
   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BUF, mailbox_number);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   /* Install ISR */
   result = _int_install_isr (index, isr, (void *)can_reg_ptr);
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }

   return return_code;
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr_err_int
* Returned Value   : uint32_t 
* Comments         :
*    This function installs interrupt handler for a flexcan error
*
*END*********************************************************************/
uint32_t FLEXCAN_Install_isr_err_int
(
   /* [IN] FlexCAN device number */
   uint8_t       dev_num,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   uint32_t   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }
 
   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_ERR, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   result = _int_install_isr (index, isr, (void *)can_reg_ptr); 
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }

   return return_code;
}
 
/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr_boff_int
* Returned Value   : uint32_t 
* Comments         :
*    This function installs interrupt handler for a flexcan bus off
*
*END*********************************************************************/
uint32_t FLEXCAN_Install_isr_boff_int
(
   /* [IN] FlexCAN device number */
   uint8_t       dev_num,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   uint32_t   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   result = _int_install_isr (index, isr, (void *)can_reg_ptr); 
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }

   return return_code;
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr_wake_int
* Returned Value   : uint32_t 
* Comments         :
*    This function installs interrupt handler for a flexcan wake-up
*
*END*********************************************************************/
uint32_t FLEXCAN_Install_isr_wake_int
(
   /* [IN] FlexCAN device number */
   uint8_t       dev_num,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   uint32_t   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile IRQInterruptIndex     index;
  
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_WAKEUP, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   result = _int_install_isr (index, isr, (void *)can_reg_ptr); 
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }
    
   return return_code;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _FLEXCAN_ISR
* Returned Value   : uint32_t 
* Comments         :
*    This function services FlexCAN interrupt routine. 
*
*END*********************************************************************/
void _FLEXCAN_ISR
(
   /* [IN] FlexCAN base address */
   void   *can_ptr
)
{
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile uint32_t                               tmp_reg;
      
   can_reg_ptr = (CAN_MemMapPtr)can_ptr;

   /* get the interrupt flag */
   tmp_reg = (can_reg_ptr->IFLAG1 & CAN_IMASK1_BUFLM_MASK);
   // check Tx/Rx interrupt flag and clear the interrupt
   if(tmp_reg)
   {
      /* clear the interrupt and unlock message buffer */
      can_reg_ptr->IFLAG1 |= tmp_reg;
      can_reg_ptr->TIMER;
   }

   // Clear all other interrupts in ERRSTAT register (Error, Busoff, Wakeup)
   tmp_reg = (can_reg_ptr->ESR1 & (CAN_ESR1_WAKINT_MASK | CAN_ESR1_ERRINT_MASK | CAN_ESR1_BOFFINT_MASK));
   if(tmp_reg)
   {
      can_reg_ptr->ESR1 |= tmp_reg;
   }

   return;
}
   
/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_int_status
* Returned Value   : uint32_t 
* Comments         :
*    This function gets the FlexCAN interrupt status. 
*
*END*********************************************************************/
uint32_t FLEXCAN_Int_status
(
   /* [IN] FlexCAN device number */
   uint8_t dev_num
)
{
   volatile CAN_MemMapPtr                 can_reg_ptr;
   volatile uint32_t                               tmp_reg;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   // check Tx/Rx interrupt flag
   tmp_reg = (can_reg_ptr->IFLAG1 & CAN_IMASK1_BUFLM_MASK);
   if(tmp_reg)
   {
      return (~(CAN_ESR1_WAKINT_MASK | CAN_ESR1_ERRINT_MASK | CAN_ESR1_BOFFINT_MASK));
   } /*Endif*/

   // check error interrupt
   tmp_reg = (can_reg_ptr->ESR1 & CAN_ESR1_ERRINT_MASK);
   if(tmp_reg)
   {
      return (CAN_ESR1_ERRINT_MASK);   
   } /*Endif*/

   // check busoff interrupt
   tmp_reg = (can_reg_ptr->ESR1 & CAN_ESR1_BOFFINT_MASK);
   if(tmp_reg)
   {
      return (CAN_ESR1_BOFFINT_MASK);   
   } /*Endif*/

   // check busoff interrupt
   tmp_reg = (can_reg_ptr->ESR1 & CAN_ESR1_WAKINT_MASK);
   if(tmp_reg)
   {
      return (CAN_ESR1_WAKINT_MASK);   
   } /*Endif*/
   
   return (FLEXCAN_OK);
}
