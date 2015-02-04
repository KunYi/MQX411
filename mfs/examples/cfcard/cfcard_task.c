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
*   This file is the main file for filesystem demo. Note that this example
*   is a multi tasking example and needs an operating system to run. This 
*   means that customers who are not using MQX should change the operating system
*   dependent code. An attempt has been made to comment out the code
*   however, a programmer must review all lines of code to ensure that
*   it correctly compiles with their libraries of operating system and
*   targetcompiler. This program has been compiled and tested for ARC AA3
*   processor with MQX real time operating system.
*
*
*END************************************************************************/


#include "cfcard_task.h"
#include "cfcard_file.h"



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : CFCard_task
* Returned Value : None
* Comments       :
*     This rutine controls CompactFlash card insert and remove events
*     CompactFlash driver consists from two parts:
*             - lower layer (platform dependend) "pccarda:" driver can be open 
*               even if the CF card is not inserted
*             - upper layer (platform independend) "pcflasha:" driver should be 
*               opened only when card is present in the slot.Otherwise error is 
*               reported during fopen("pcflasha",..) function call.
*
*END*--------------------------------------------------------------------*/

void CFCard_task(uint32_t param)
{ 
   FS_STRUCT_PTR    CF_fs_handle = NULL;
   MQX_FILE_PTR     pccard_fp = NULL;
   uint32_t          card_inserted;
   uint32_t          device_state = CARD_DETACHED;

    /* open pccarda: driver*/
    pccard_fp = fopen("pccarda:", NULL);
    if ( pccard_fp == NULL ) 
    {
        printf("\nUnable to open pccard device \"pccarda:\"");
    }
    

    // there is no interrupt connected to pccard/pcflash driver, checking if the flash card was
    // removed has to be done in polling mode
    for ( ; ; ) 
    {
        // wait 200 ms
        _time_delay(200);
        
        if (device_state == CARD_DETACHED)
        {
            // check if card is inserted
            _io_ioctl(pccard_fp, APCCARD_IOCTL_IS_CARD_INSERTED, &card_inserted);
            if ((bool)card_inserted == TRUE)  
            {
                // wait till instertion of card is completed and check once more
                _time_delay(300);
                // check if card is inserted
                _io_ioctl(pccard_fp, APCCARD_IOCTL_IS_CARD_INSERTED, &card_inserted);
                if ((bool)card_inserted == TRUE) 
                {
                    // card is present in the slot - install filesystem
                    CF_fs_handle = CF_filesystem_install(pccard_fp, "PM_C2:", "d:" );
                    if(CF_fs_handle != NULL) 
                    {
                        device_state = CARD_INTERFACED;
                    }
                    else 
                    {
                        printf("Compact flash can not be initialized.\n");
                        device_state = CARD_ERROR;
                    }
                }
            }
        }
        
        else if (device_state == CARD_INTERFACED) 
        {
            // check if card is inserted - one of following IOCTL can be used here
            _io_ioctl(pccard_fp, APCCARD_IOCTL_IS_CARD_INSERTED, &card_inserted);
            if ((bool)card_inserted == FALSE)  
            {
                // card is not present in the slot
                device_state = CARD_DETACHED;
                CF_filesystem_uninstall(CF_fs_handle);
            }
        }
        
        else if (device_state == CARD_ERROR)
        {
            // check if card is inserted - one of following IOCTL can be used here
            _io_ioctl(pccard_fp, APCCARD_IOCTL_IS_CARD_INSERTED, &card_inserted);
            if ((bool)card_inserted == FALSE)  
            {
                // card is not present in the slot
                device_state = CARD_DETACHED;
            }
        }
    }
}

/* EOF */
