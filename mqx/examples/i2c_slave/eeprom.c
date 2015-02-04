/*HEADER**********************************************************************
*
* Copyright 2008-2014 Freescale Semiconductor, Inc.
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
*   This file contains the source for a simple example of EEPROM emulation
*   using I2C slave driver.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "eeprom.h"


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if defined BSP_TWRPXS30
  #if I2C_POLLED_MODE
    #define I2C_DEVICE "i2c2:"
    #if ! BSPCFG_ENABLE_I2C2
      #error This application requires BSPCFG_ENABLE_I2C2 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
  #else
    #define I2C_DEVICE "ii2c2:"
    #if ! BSPCFG_ENABLE_II2C2
      #error This application requires BSPCFG_ENABLE_II2C2 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
  #endif
#elif defined BSP_VYBRID_AUTOEVB_A5 || defined BSP_VYBRID_AUTOEVB_M4
  #if I2C_POLLED_MODE
    #define I2C_DEVICE "i2c1:"
    #if ! BSPCFG_ENABLE_I2C1
      #error This application requires BSPCFG_ENABLE_I2C1 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
  #else 
    #define I2C_DEVICE "ii2c1:"
    #if ! BSPCFG_ENABLE_II2C1
      #error This application requires BSPCFG_ENABLE_II2C1 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
  #endif
#else
  #if I2C_POLLED_MODE
    #define I2C_DEVICE "i2c0:"
    #if ! BSPCFG_ENABLE_I2C0
      #error This application requires BSPCFG_ENABLE_I2C0 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
  #else
    #define I2C_DEVICE "ii2c0:"
    #if ! BSPCFG_ENABLE_II2C0
      #error This application requires BSPCFG_ENABLE_II2C0 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
  #endif
#endif


extern void dump_task(uint32_t);
extern void main_task(uint32_t);


static bool dump = TRUE;
static unsigned char memory[I2C_EEPROM_SIZE];


const TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{
   /* Task Index,   Function,  Stack,  Priority, Name,   Attributes,          Param, Time Slice */
   { 9,            dump_task, 2000L,    9L,     "Dump", MQX_AUTO_START_TASK, 0,     0 },
   { 10,           main_task, 2000L,    10L,    "Main", MQX_AUTO_START_TASK, 0,     0 },
   { 0 }
};


/*TASK*-------------------------------------------------------------------
* 
* Task Name : dump_task
* Comments  :
*
*END*----------------------------------------------------------------------*/

void dump_task
   (
      uint32_t dummy
   )
{
   uint32_t    i, j;
   
   while (1)
   {
      if ((status ()) && (dump))
      {
         getchar ();
      
         printf ("\nMemory dump:\n");
         for (i = 0; i < 16; i++)
         {
            printf ("0x%02x: ", i * 16);
            for (j = 0; j < 16; j++)
            {
               printf ("%02x ", memory[i * 16 + j]);
            }
            for (j = 0; j < 16; j++)
            {
               if ((32 <= memory[i * 16 + j]) && (memory[i * 16 + j] <= 128))
               {
                  printf ("%c", memory[i * 16 + j]);
               }
               else
               {
                  printf (".");
               }
            }
            printf ("\n");
         }
         printf ("\nSend character over terminal to dump memory contents.\n");
      }
      _time_delay (100);
   }
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name : main_task
* Comments  :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t            dummy
   )
{
   MQX_FILE_PTR          fd;
   _mqx_int            param, result, nack;
   unsigned char                 address;

   printf ("\n%s I2C slave example - 256 byte EEPROM emulation\n\n", I2C_POLLED_MODE ? "Polled" : "Interrupt");
   printf ("Please attach I2C master to appropriate I2C bus.\n");
   printf ("I2C EEPROM communication protocol should be used (e.g. 'i2c' example).\n\n");

   /* Open the I2C driver */         
   fd = fopen (I2C_DEVICE, NULL);
   if (fd == NULL) 
   {
      printf ("ERROR opening the I2C driver!\n");
      _task_block ();
   }

   /* Test ioctl commands */
   param = 100000;
   printf ("Set current baud rate to %d ... ", param);
   if (I2C_OK == ioctl (fd, IO_IOCTL_I2C_SET_BAUD, &param))
   {
      printf ("OK\n");
   } else {
      printf ("ERROR\n");
   }

   printf ("Get current baud rate ... ");
   if (I2C_OK == ioctl (fd, IO_IOCTL_I2C_GET_BAUD, &param))
   {
      printf ("%d\n", param);
   } else {
      printf ("ERROR\n");
   }

   printf ("Set slave mode ... ");
   if (I2C_OK == ioctl (fd, IO_IOCTL_I2C_SET_SLAVE_MODE, NULL))
   {
      printf ("OK\n");
   } else {
      printf ("ERROR\n");
   }

   printf ("Get current mode ... ");
   if (I2C_OK == ioctl (fd, IO_IOCTL_I2C_GET_MODE, &param))
   {
      printf ("0x%02x\n", param);
   } else {
      printf ("ERROR\n");
   }
   
   param = I2C_EEPROM_ADDRESS;
   printf ("Set station address to 0x%02x ... ", param);
   if (I2C_OK == ioctl (fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param))
   {
      printf ("OK\n");
   } else {
      printf ("ERROR\n");
   }
   
   param = 0x00;
   printf ("Get station address ... ");
   if (I2C_OK == ioctl (fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param))
   {
      printf ("0x%02x\n", param);
   } else {
      printf ("ERROR\n");
   }

   printf ("\nSend character over terminal to dump memory contents.\n");
   
   

#if I2C_POLLED_MODE
   
   /* I2C slave polled mode EEPROM emulation */
   result = 0;
   address = 0;
   while (1)
   {
      /* EEPROM always ACKs incoming bytes */
      param = 0xFFFFFFFF;
      if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
      {
         printf ("ERROR: set rx request\n");
      }
      
      /* Wait to be addressed */
      if (0 == result)
      {
         /* Enable memory dumping when idle */
         dump = TRUE;
         
         do 
         {
            /* Get memory address for read/write operation */
            result = fread (&address, 1, 1, fd);
            
            /* Handle special states */
            if (0 == result)
            {
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
               {
                  printf ("ERROR: get state read address\n");
               }
               if (I2C_STATE_FINISHED == param)
               {
                  /* Stop condition detected on the bus, return to ready state */
                  if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
                  {
                     printf ("ERROR: stop read address\n");
                  }
                  
                  /* EEPROM always ACKs incoming bytes */
                  param = 0xFFFFFFFF;
                  if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
                  {
                     printf ("ERROR: set rx request\n");
                  }
               }
               else if ((I2C_STATE_RECEIVE != param) && (I2C_STATE_ADDRESSED_AS_SLAVE_RX != param))
               {
                  /* Memory address was omitted or unknown event */
                  result = 1;
               }
            }
         } while (1 != result);
         
         /* Temporarily disable memory dump when changing/sending contents */
         dump = FALSE;
      }
      
      /* Check for next byte and required operation */
      result = fread (&(memory[address]), 1, 1, fd);
      
      /* Get required operation */
      if (result)
      {
         param = I2C_STATE_ADDRESSED_AS_SLAVE_RX;
      }
      else
      {
         if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
         {
            printf ("ERROR: get required operation\n");
         }
      }
      
      switch (param)
      {
         case I2C_STATE_RECEIVE:
         case I2C_STATE_ADDRESSED_AS_SLAVE_RX:
        
            if (result)
            {
               /* Receive data, store bytes from given address, loop within page size */
               while (result)
               {
                  address++;
                  if (0 == (address % I2C_EEPROM_PAGE_SIZE))
                  {
                     address -= I2C_EEPROM_PAGE_SIZE;
                  }
                  result = fread (&(memory[address]), 1, 1, fd);
               }

               /* Signal internal programming by NACK on I2C bus */
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_DISABLE_DEVICE, NULL))
               {
                  printf ("ERROR: disable device\n");
               }
            
               /* Simulate programming process */
               _time_delay (5);

               /* Programming finished, enable device on the bus again */
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_ENABLE_DEVICE, NULL))
               {
                  printf ("ERROR: enable device\n");
               }
               
               /* Wait to be addressed, send ACK, synchronize with master */
               result = fread (&address, 1, 1, fd);
            }
            break;

         case I2C_STATE_TRANSMIT:
         case I2C_STATE_ADDRESSED_AS_SLAVE_TX:
         
            /* Try to send first byte */
            fwrite (&(memory[address]), 1, 1, fd);
            
            /* Wait for transfer complete */
            if (I2C_OK != ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &nack))
            {
               printf ("ERROR: flush first byte\n");
            }
            
            if (nack)
            {
               /* 1 byte sent */
               address = (address + 1) % I2C_EEPROM_SIZE;
            }
            else
            {
               /* Check if stop condition occurred */
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
               {
                  printf ("ERROR: get state read address\n");
               }
                
               /* Write data until NACK or STOP */
               while ((0 == nack) && (I2C_STATE_FINISHED != param))
               {
                  /* Send next data */
                  address = (address + 1) % I2C_EEPROM_SIZE;
                  fwrite (&(memory[address]), 1, 1, fd);
                  
                  /* Wait for transfer complete */
                  if (I2C_OK != ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &nack))
                  {
                     printf ("ERROR: flush data\n");
                  }
                  
                  /* Check if stop condition occurred */
                  if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
                  {
                     printf ("ERROR: get state read address\n");
                  }
               }
            }
            
            /* Return to ready state, wait for bus idle */
            if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
            {
               printf ("ERROR: stop tx\n");
            }
            result = 0;
            break;
            
         case I2C_STATE_FINISHED:
         case I2C_STATE_LOST_ARBITRATION:
            
            /* Return to ready state, wait for bus idle */
            if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
            {
               printf ("ERROR: stop lost arbitration\n");
            }
            result = 0;
            break;
            
         default: 
            break;
      }
   }
   
   
   
#else
   
   
   
   /* I2C slave interrupt mode EEPROM emulation */ 
   
   result = 0;
   address = 0;
   while (1)
   {
      /* EEPROM always ACKs incoming bytes */
      param = 0xFFFFFFFF;
      if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
      {
         printf ("ERROR: set rx request\n");
      }
      
      /* Enable memory dumping when idle */
      dump = TRUE;
      
      do {
         /* Get memory address for read/write operation */
         result = fread (&address, 1, 1, fd);
      
         /* Handle special states */
         if (0 == result)
         {
            if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
            {
               printf ("ERROR: get state read address\n");
            }
            if (I2C_STATE_FINISHED == param)
            {
               /* Stop condition detected on the bus, return to ready state */
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
               {
                  printf ("ERROR: stop read address\n");
               }
           
               /* EEPROM always ACKs incoming bytes */
               param = 0xFFFFFFFF;
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
               {
                  printf ("ERROR: set rx request read address\n");
               }
            }
            else if ((I2C_STATE_RECEIVE != param) && (I2C_STATE_ADDRESSED_AS_SLAVE_RX != param))
            {
               /* Memory address was omitted, using the last one */
               result = 1;
            }
         }
      } while (0 == result);
    
      /* Temporarily disable memory dump when changing/sending contents */
      dump = FALSE;

      do 
      {
         /* Check next byte */
         result = fread (&(memory[address]), 1, 1, fd);
         
         /* Get required operation */
         if (result)
         {
            param = I2C_STATE_ADDRESSED_AS_SLAVE_RX;
         }
         else
         {
            if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
            {
               printf ("ERROR: get required operation\n");
            }
         }
      } while ((0 == result) && (I2C_STATE_ADDRESSED_AS_SLAVE_RX == param));
      
      switch (param)
      {
         case I2C_STATE_RECEIVE:
         case I2C_STATE_ADDRESSED_AS_SLAVE_RX:
         
            if (result)
            {
               /* Receive data, store bytes from given address, loop within page size */
               while (result)
               {
                  address++;
                  if (0 == (address % I2C_EEPROM_PAGE_SIZE))
                  {
                     address -= I2C_EEPROM_PAGE_SIZE;
                  }
                  do {
                     /* Check if there's still anything to read */
                     result = fread (&(memory[address]), 1, 1, fd);
                    
                     /* Check state change */
                     if (result)
                     {
                        param = I2C_STATE_ADDRESSED_AS_SLAVE_RX;
                     }
                     else
                     {
                        if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
                        {
                           printf ("ERROR: get required operation\n");
                        }
                     }
                  } while ((0 == result) && (I2C_STATE_ADDRESSED_AS_SLAVE_RX == param));
               }

               /* Signal internal programming by NACK on I2C bus */
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_DISABLE_DEVICE, NULL))
               {
                  printf ("ERROR: disable device\n");
               }
            
               /* Simulate programming process */
               _time_delay (5);

               /* Programming finished, enable device on the bus again */
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_ENABLE_DEVICE, NULL))
               {
                  printf ("ERROR: enable device\n");
               }
            }
            break;

         case I2C_STATE_TRANSMIT:
         case I2C_STATE_ADDRESSED_AS_SLAVE_TX:

            /* Try to send first byte */
            fwrite (&(memory[address]), 1, 1, fd);
             
            /* Wait for transfer complete */
            if (I2C_OK != ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &nack))
            {
               printf ("ERROR: flush first byte\n");
            }
            
            if (nack)
            {
               /* 1 byte sent */
               address = (address + 1) % I2C_EEPROM_SIZE;
            }
            else
            {
               /* Check if stop condition occurred */
               if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
               {
                  printf ("ERROR: get state read address\n");
               }
                
               /* Write data until NACK or STOP */
               while ((0 == nack) && (I2C_STATE_FINISHED != param))
               {
                  /* Send next byte */
                  address = (address + 1) % I2C_EEPROM_SIZE;
                  fwrite (&(memory[address]), 1, 1, fd);
                  
                  /* Wait for transfer complete */
                  if (I2C_OK != ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &nack))
                  {
                     printf ("ERROR: flush data\n");
                  }
                  
                  /* Check if stop condition occurred */
                  if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATE, &param))
                  {
                     printf ("ERROR: get state read address\n");
                  }
               }
            }
            
            /* Return to ready state, wait for bus idle */
            if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
            {
               printf ("ERROR: stop tx\n");
            }
            break;

         case I2C_STATE_FINISHED:
         case I2C_STATE_LOST_ARBITRATION:

            /* Return to ready state, wait for bus idle */
            if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
            {
               printf ("ERROR: stop finished/lost arbitration\n");
            }
            break;
            
         default: 
            break;
      }
   }
   
#endif



} /* Endbody */

/* EOF */
