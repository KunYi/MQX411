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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains board-specific Flextimer initialization functions.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "ftm_vybrid.h"

/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_ftm_base_address
* Returned Value   : address if successful, NULL otherwise
* Comments         :
*    This function returns the base register address of the corresponding device.
*
*END*********************************************************************/
void* _bsp_get_ftm_base_address
(
   uint8_t dev_num
)
{
   void* addr;

   switch (dev_num)
   {
      case 0:
         addr = (void *) FTM0_BASE_PTR;
         break;
      case 1:
         addr = (void *) FTM1_BASE_PTR;
         break;
      case 2:
         addr = (void *) FTM2_BASE_PTR;
         break;
      case 3:
         addr = (void *) FTM3_BASE_PTR;
         break;
      default:
         addr = NULL;
         break;
    }
    return addr;
}

/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_ftm_vector
* Returned Value   : vector number if successful, 0 otherwise
* Comments         :
*    This function returns desired interrupt vector number for specified device.
*
*END*********************************************************************/
uint32_t _bsp_get_ftm_vector
(
   uint8_t dev_num
)
{
   uint32_t vector;
   switch (dev_num)
   {
      case 0:
         vector = INT_FlexTimer0;
         break;
      case 1:
         vector = INT_FlexTimer1;
         break;
      case 2:
         vector = INT_FlexTimer2;
         break;
      case 3:
         vector = INT_FlexTimer3;
         break;
      default:
         vector = INT_FlexTimer0;
         break;
    }
    return vector;
}


/*FUNCTION****************************************************************
*
* Function Name    : _bsp_dump_ftm_register
* Returned Value   : NA
* Comments         :
*    dump ftm register
*
*END*********************************************************************/
void _bsp_dump_ftm_register(FTM_MemMapPtr ftm_reg)
{
    printf("SC      :   0x%x\n", ftm_reg->SC);
    printf("CNT     :   0x%x\n", ftm_reg->CNT);
    printf("MOD     :   0x%x\n", ftm_reg->MOD);
    printf("C0SC    :   0x%x\n", ftm_reg->CONTROLS[0].CnSC);
    printf("C0V     :   0x%x\n", ftm_reg->CONTROLS[0].CnV);
    printf("C1SC    :   0x%x\n", ftm_reg->CONTROLS[1].CnSC);
    printf("C1V     :   0x%x\n", ftm_reg->CONTROLS[1].CnV);
    printf("CNTIN   :   0x%x\n", ftm_reg->CNTIN);
    printf("STATUS  :   0x%x\n", ftm_reg->STATUS);
    printf("MODE    :   0x%x\n", ftm_reg->MODE);
    printf("SYNC    :   0x%x\n", ftm_reg->SYNC);
    printf("OUTINIT :   0x%x\n", ftm_reg->OUTINIT);
    printf("OUTMASK :   0x%x\n", ftm_reg->OUTMASK);
    printf("COMBINE :   0x%x\n", ftm_reg->COMBINE);
    printf("DEADTIME:   0x%x\n", ftm_reg->DEADTIME);
    printf("EXTTRIG :   0x%x\n", ftm_reg->EXTTRIG);
    printf("POL     :   0x%x\n", ftm_reg->POL);
    printf("FMS     :   0x%x\n", ftm_reg->FMS);
    printf("FILTER  :   0x%x\n", ftm_reg->FILTER);
    printf("FLTCTRL :   0x%x\n", ftm_reg->FLTCTRL);
    printf("QDCTRL  :   0x%x\n", ftm_reg->QDCTRL);
    printf("CONF    :   0x%x\n", ftm_reg->CONF);
    printf("FLTPOL  :   0x%x\n", ftm_reg->FLTPOL);
    printf("SYNCONF :   0x%x\n", ftm_reg->SYNCONF);
    printf("INVCTRL :   0x%x\n", ftm_reg->INVCTRL);
    printf("SWOCTRL :   0x%x\n", ftm_reg->SWOCTRL);
    printf("PWMLOAD :   0x%x\n", ftm_reg->PWMLOAD);
}

/* EOF */

