/*HEADER**********************************************************************
*
* Copyright  Freescale Semiconductor, Inc.
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
*   Processor family specific file needed for flexcan module.
*   Revision History:
*   Date             Version  Changes
*   ---------        -------  -------
*   Sep 26/10        3.60     Initial version
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_flexcan_base_address  
* Returned Value   : Pointer to desired CAN device or NULL if not present
* Comments         :
*    This function returns pointer to base address of address space of the 
*    desired CAN device. Returns NULL otherwise.
*
*END*********************************************************************/
void *_bsp_get_flexcan_base_address
(
   /* [IN] FlexCAN device number */
   uint8_t dev_num
)
{ /* Body */
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
} /* EndBody */

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_flexcan_vector
* Returned Value   : MQX vector number for specified interrupt
* Comments         :
*    This function returns index into MQX interrupt vector table for
*    specified flexcan interrupt. If not known, returns 0.
*
*END*********************************************************************/
IRQInterruptIndex _bsp_get_flexcan_vector 
(
   /* [IN] FlexCAN device number */
    uint8_t dev_num,
   /* [IN] FlexCAN interrupt vector type */
    uint8_t vector_type,
   /* [IN] FlexCAN interrupt vector index */
    uint32_t vector_index
)
{ /* Body */
   IRQInterruptIndex index = (IRQInterruptIndex)0;
   
   switch (dev_num)
   {
      case 0: 
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
         break;
      case 1:    
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
         break;
      default: break;
   }
   
   return index;
} /* EndBody */
