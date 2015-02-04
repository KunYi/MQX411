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
*   This file contains the source for the cplus example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>

/* Task IDs */
#define CPLUS_TASK 5

#if !defined(BSPCFG_ENABLE_CPP) || !(BSPCFG_ENABLE_CPP)
#error This application requires BSPCFG_ENABLE_CPP defined non-zero in user_config.h. Please recompile BSP and PSP with this option.
#endif

extern void cplus_task(uint32_t);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    {CPLUS_TASK, cplus_task, 2000, 8, "cplus", MQX_AUTO_START_TASK, 0, 0},
    {0,          0,          0,    0, 0,       0,                   0, 0}
};

class HelloWorld {
private:
   int check_init;
   const char *id;
public:
   HelloWorld() {
      check_init = 0x1234567;
   }
   ~HelloWorld() {
      _io_printf("%s: deallocation\n",id);
   }
   void print(const char *x) {
      id = x;
      if (check_init == 0x1234567) {
         _io_printf("%s: Constructed OK\n",id);
      } else {
         _io_printf("%s: Constructor not called\n",id);
      }
   }
};

HelloWorld global;

/*TASK*-----------------------------------------------------
* 
* Task Name    : cplus_task
* Comments     :
*    This task prints the state of each object
*
*END*-----------------------------------------------------*/

void cplus_task
   (
      uint32_t initial_data
   )
{
   { // Scope for local to destruct
      HelloWorld local;
      HelloWorld *heap;

      global.print("global");
      local.print("local");
      heap = new HelloWorld;
      if (heap != 0) {
         heap->print("heap");
         delete heap;
      } else {
         _io_printf("heap: new failed\n");
      } /* Endif */
   } // local should destruct
   _io_fflush(stdout);
   _task_block();
}

/* EOF */
