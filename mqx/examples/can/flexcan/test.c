/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the source for the FlexCAN example program.
*
*   NOTE: This is a two node test. It requires the software to be loaded 
*   onto two boards, one board programmed as NODE 1 and the other programmed 
*   as NODE 2.  A properly terminated CAN cable (120 ohms on either end) is 
*   used to connect both boards together.  When running, the boards will 
*   exchange a CAN message once per second, and the following output
*   should be repetitively displayed (where <x> and <y> increment):
*
*       Data transmit: <x>
*       FLEXCAN tx update message. result: 0x0
*       Received data: <y>
*       ID is: 0x321
*       DLC is: 0x1
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include "test.h"

#ifndef PSP_MQX_CPU_IS_KINETIS
#include <flexcan.h>
#endif


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


LWEVENT_STRUCT event;

/* Task template list */
TASK_TEMPLATE_STRUCT MQX_template_list[] = {
   { MAIN_TASK, Main_Task, 1000L, 8L, "Main task", MQX_AUTO_START_TASK},
   { TX_TASK, Tx_Task, 1000L, 7L, "TX task", 0, 0, 0},
   { RX_TASK, Rx_Task, 1000L, 7L, "RX task", 0, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L }
};

#if PSP_MQX_CPU_IS_KINETIS
void MY_FLEXCAN_ISR
(
   /* [IN] FlexCAN base address */
   void   *can_ptr
)
{
   volatile CAN_MemMapPtr        can_reg_ptr;
   volatile uint32_t                               tmp_reg;
   volatile uint32_t                               temp;
      
   can_reg_ptr = (CAN_MemMapPtr)can_ptr;

   /* get the interrupt flag */
   tmp_reg = (can_reg_ptr->IFLAG1 & CAN_IMASK1_BUFLM_MASK);
   // check Tx/Rx interrupt flag and clear the interrupt
   if(tmp_reg){
      /* clear the interrupt and unlock message buffer */
      /* Start CR# 1751 */
      _lwevent_set(&event, tmp_reg);
      can_reg_ptr->IFLAG1 |= tmp_reg;
      /* End CR# 1751 */
      temp = can_reg_ptr->TIMER;
   }/* Endif */

   // Clear all other interrupts in ERRSTAT register (Error, Busoff, Wakeup)
   tmp_reg = can_reg_ptr->ESR1;
   if(tmp_reg & FLEXCAN_ALL_INT){
      /* Start CR# 1751 */
      can_reg_ptr->ESR1 |= (tmp_reg & FLEXCAN_ALL_INT);
      /* End CR# 1751 */
   } /* Endif */
      
   return;
}
#else
void MY_FLEXCAN_ISR
(
   /* [IN] FlexCAN base address */
   void   *can_ptr
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile uint32_t                               tmp_reg;
   volatile uint32_t                               temp;
      
   can_reg_ptr = (FLEXCAN_REG_STRUCT_PTR)can_ptr;

   /* get the interrupt flag */
   tmp_reg = (can_reg_ptr->IFLAG & FLEXCAN_IMASK_VALUE);
   // check Tx/Rx interrupt flag and clear the interrupt
   if(tmp_reg){
      /* clear the interrupt and unlock message buffer */
      /* Start CR# 1751 */
      _lwevent_set(&event, tmp_reg);
      can_reg_ptr->IFLAG |= tmp_reg;
      /* End CR# 1751 */
      temp = can_reg_ptr->TIMER;
   }/* Endif */

   // Clear all other interrupts in ERRSTAT register (Error, Busoff, Wakeup)
   tmp_reg = can_reg_ptr->ERRSTAT;
   if(tmp_reg & FLEXCAN_ALL_INT){
      /* Start CR# 1751 */
      can_reg_ptr->ERRSTAT |= (tmp_reg & FLEXCAN_ALL_INT);
      /* End CR# 1751 */
   } /* Endif */
      
   return;
}
#endif


/*TASK*-----------------------------------------------------------
*
* Task Name : Main_Task
* Comments :
* 
*
*END*-----------------------------------------------------------*/
void Main_Task(uint32_t parameter)
{ /* Body */
   _task_id     created_task;
   uint32_t result;

   _int_install_unexpected_isr();
   
   if (_bsp_flexcan_io_init(CAN_DEVICE) != 0)
   {
      printf ("\nError initializing pins for FlexCAN device %d!\n", CAN_DEVICE);
      _task_block();
   }

   printf("\n*********FLEXCAN TEST PROGRAM.*********");
   printf("\n   Message format: Standard (11 bit id)");
   printf("\n   Message buffer 0 used for Tx and Rx.");
   printf("\n   Interrupt Mode: Enabled");
   printf("\n   Operation Mode: TX and RX --> Normal");
   printf("\n***************************************\n");
  
   frequency = 125;
   printf("\nselected frequency (Kbps) is: %d", frequency);

   data_len_code = 1;
   printf("\nData length: %d", data_len_code);
  
   /* Select message format */
   format = FLEXCAN_STANDARD;
   
   /* Select mailbox number */
   RX_mailbox_num = 0;
   TX_mailbox_num = 1;
   RX_remote_mailbox_num = 2;
   TX_remote_mailbox_num = 3;

#if NODE==1
   RX_identifier = 0x123;
   TX_identifier = 0x321;
   RX_remote_identifier = 0x0F0;
   TX_remote_identifier = 0x00F;
#else
   RX_identifier = 0x321; 
   TX_identifier = 0x123;
   RX_remote_identifier = 0x00F;
   TX_remote_identifier = 0x0F0;
#endif   
   
   /* We use default settings */
   bit_timing0 = bit_timing1 = 0;
   
   /* Select operation mode */
   flexcan_mode = FLEXCAN_NORMAL_MODE;

   /* Enable interrupt */
   interrupt = FLEXCAN_ENABLE;
   
   /* Enable error interrupt */
   flexcan_error_interrupt = 1;
   
   /* Reset FLEXCAN device */
   result = FLEXCAN_Softreset ( CAN_DEVICE);
   printf("\nFLEXCAN reset. result: 0x%lx", result);

   /* Initialize FLEXCAN device */
   result = FLEXCAN_Initialize ( CAN_DEVICE, bit_timing0, bit_timing1, frequency, FLEXCAN_IPBUS_CLK);
   printf("\nFLEXCAN initilization. result: 0x%lx", result);

    /* Select mode */
   result = FLEXCAN_Select_mode( CAN_DEVICE, flexcan_mode);
   printf("\nFLEXCAN mode selected. result: 0x%lx", result);

   result = FLEXCAN_Set_global_stdmask (CAN_DEVICE, 0x222 );
   printf("\nFLEXCAN global mask. result: 0x%lx", result);

   /* Enable error interrupts */
   if(flexcan_error_interrupt == 1)
   {
      result = FLEXCAN_Install_isr_err_int( CAN_DEVICE, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN Error ISR install, result: 0x%lx", result);

      result = FLEXCAN_Install_isr_boff_int( CAN_DEVICE, MY_FLEXCAN_ISR  );
      printf("\nFLEXCAN Bus off ISR install, result: 0x%lx", result);

      result = FLEXCAN_Error_int_enable(CAN_DEVICE);
      printf("\nFLEXCAN error interrupt enable. result: 0x%lx", result);
   }

  /* Set up an event group */
    result = _lwevent_create(&event, LWEVENT_AUTO_CLEAR);
    if (result != MQX_OK) {
        printf("\nCannot create lwevent");
    }   

   created_task = _task_create(0, RX_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nRx task: task creation failed.");
   }

   created_task = _task_create(0, TX_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nTx task: task creation failed.");
   }

   /* Start FLEXCAN */
   result = FLEXCAN_Start(CAN_DEVICE);
   printf("\nFLEXCAN started. result: 0x%lx", result);

   // start the ping pong
   while(1){}

} /* EndBody */

/*TASK*-----------------------------------------------------------
*
* Task Name : Tx_Task
* Comments :
* 
*
*END*-----------------------------------------------------------*/
void Tx_Task(uint32_t parameter)
{/* Body */   
   unsigned char   data = 0;
   uint32_t result;
   unsigned char string[] = 
   {
     0xa, 0xb, 0xe, 0xc, 0xe, 0xd, 0xa, 0x0
   };


   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, TX_remote_mailbox_num, TX_remote_identifier,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx remote mailbox initialization. result: 0x%lx", result);

   
   /* Initialize mailbox */
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, TX_mailbox_num, TX_identifier,
                                        data_len_code, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx mailbox initialization. result: 0x%lx", result);
  
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, TX_mailbox_num, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx mailbox activation. result: 0x%lx", result);

   /* Install ISR */
   if(interrupt == FLEXCAN_ENABLE)
   {
      result = FLEXCAN_Install_isr( CAN_DEVICE, TX_mailbox_num, MY_FLEXCAN_ISR  );
      printf("\nFLEXCAN TX ISR install. result: 0x%lx", result);

      result = FLEXCAN_Install_isr( CAN_DEVICE, TX_remote_mailbox_num, MY_FLEXCAN_ISR  );
      printf("\nFLEXCAN TX remote ISR install. result: 0x%lx", result);
   }
   
   /* Let Rx Task start to initialize */
   _time_delay(1000);

   result = FLEXCAN_Tx_message(CAN_DEVICE, TX_mailbox_num, TX_identifier, 
                              format, data_len_code, &data);
   if(result != FLEXCAN_OK)
      printf("\nTransmit error. Error Code: 0x%lx", result);
   else
      printf("\nData transmit: %d", data);
      
   while(1)
   {    
      /* Let Rx Task receive message */
      _time_delay(1000);

      data++;

      result = FLEXCAN_Tx_mailbox(CAN_DEVICE, TX_mailbox_num, &data);
      if(result != FLEXCAN_OK)
         printf("\nTransmit error. Error Code: 0x%lx", result);
      else
         printf("\nData transmit: %d", data);
      
      string[7] = data;

      result = FLEXCAN_Update_message(CAN_DEVICE, string, 8, format, TX_remote_mailbox_num);
      printf("\nFLEXCAN tx update message. result: 0x%lx", result);

   }
} /* EndBody */

/*TASK*-----------------------------------------------------------
*
* Task Name : Rx_Task
* Comments :
* 
*
*END*-----------------------------------------------------------*/
void Rx_Task(uint32_t parameter)
{/* Body */   
   unsigned char   dptr[8];
   uint32_t result;
   uint32_t DLC = 0;
   uint32_t ID = 0;


   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, RX_remote_mailbox_num, RX_remote_identifier,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN rx remote mailbox initialization. result: 0x%lx", result);

   /* Initialize mailbox */
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, RX_mailbox_num, RX_identifier,
                                        data_len_code, FLEXCAN_RX, format,
                                        interrupt);
   printf("\nFLEXCAN rx mailbox initialization. result: 0x%lx", result);
   
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, RX_mailbox_num, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN mailbox activation. result: 0x%lx", result);


   /* Install ISR */
   if(interrupt == FLEXCAN_ENABLE)
   {
      result = FLEXCAN_Install_isr( CAN_DEVICE, RX_mailbox_num, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN RX ISR install. result: 0x%lx", result);

      result = FLEXCAN_Install_isr( CAN_DEVICE, RX_remote_mailbox_num, MY_FLEXCAN_ISR  );
      printf("\nFLEXCAN RX remote ISR install. result: 0x%lx", result);
   }


  if (_lwevent_wait_ticks(&event, 1 << RX_mailbox_num, FALSE, 0) != MQX_OK) {
     printf("\nEvent Wait failed");
  }

  result = FLEXCAN_Lock_mailbox (CAN_DEVICE, RX_mailbox_num);
  if(result != FLEXCAN_OK) 
  {
     printf("\nLock mailbox failed. Error Code: 0x%lx", result);
  }

  result = FLEXCAN_Rx_message(CAN_DEVICE, RX_mailbox_num, &ID, format, 
                             &DLC, &dptr, interrupt);
  if(result != FLEXCAN_OK)
     printf("\nReceived error. Error Code: 0x%lx", result);
  else
  {
     printf("\nReceived data: ");
     for (result = 0; result < DLC; result++) printf ("0x%x ", dptr[result]);
     printf("\nID is: 0x%x", ID);
     printf("\nDLC is: 0x%x\n", DLC);
  }

  result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
  if(result != FLEXCAN_OK) 
  {
     printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
  }

   
   while(1)
   {            
   
      if (_lwevent_wait_ticks(&event, 1 << RX_mailbox_num, FALSE, 0) != MQX_OK) {
         printf("\nEvent Wait failed");
      }
   
      result = FLEXCAN_Lock_mailbox (CAN_DEVICE, RX_mailbox_num);
      if(result != FLEXCAN_OK) 
      {
         printf("\nLock mailbox failed. Error Code: 0x%lx", result);
      }
     
      result = FLEXCAN_Rx_message(CAN_DEVICE, RX_mailbox_num, &ID, format, 
                             &DLC, &dptr, interrupt);
      if(result != FLEXCAN_OK)
         printf("\nReceived error. Error Code: 0x%lx", result);
      else
      {
         printf("\nReceived data: ");
         for (result = 0; result < DLC; result++) printf ("0x%x ", dptr[result]);
         printf("\nID is: 0x%x", ID);
         printf("\nDLC is: 0x%x\n", DLC);
      }

      result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
      if(result != FLEXCAN_OK) 
      {
         printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
      }
      
      FLEXCAN_Request_message (CAN_DEVICE, RX_remote_mailbox_num, format);
     
   }
} /* EndBody */


/* EOF */
