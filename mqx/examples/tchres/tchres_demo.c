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
*   This file contains the source for the tchres_demo example program.
*
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>

#if (BSP_TWR_K70F120M || BSP_TWR_K60F120M)
        #error This application can not be supported on twrk70f120m and twrk60f120m boards (Used ADC have not gpio alternative).
#elif (BSP_TWR_K60N512 || BSP_TWR_K60D100M || BSP_TWR_K64F120M) 
    #if !BSPCFG_ENABLE_ADC1
        #error This application requires BSPCFG_ENABLE_ADC1 to be enabled. Please enable BSPCFG_ENABLE_ADC1 in user_config.h.
    #endif
#elif BSP_TWR_K40X256 || BSP_TWR_K40D100M
    #if !BSPCFG_ENABLE_ADC0
        #error This application requires BSPCFG_ENABLE_ADC0 to be enabled. Please enable BSPCFG_ENABLE_ADC0 in user_config.h.
    #endif
#elif BSP_TWR_K53N512
    #if !BSPCFG_ENABLE_ADC0 || !BSPCFG_ENABLE_ADC1 
        #error This application requires BSPCFG_ENABLE_ADC0 and BSPCFG_ENABLE_ADC1 to be enabled.
    #endif
#elif !BSPCFG_ENABLE_ADC 
    #error This demo application reguires BSPCFG_ENABLE_ADC to be enabled. Please enable BSPCFG_ENABLE_ADC in user_config.h. 
#endif

#define MAIN_TASK 1

void Main_task(uint32_t);

const ADC_INIT_STRUCT adc_init = {
    ADC_RESOLUTION_12BIT,     /* resolution */
};

/* Initial structure for TCHRES taken from init_tchres.c */
extern TCHRES_INIT_STRUCT _bsp_tchscr_resisitve_init;

const TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{ 
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   Main_task,   1000,  8,   "main", MQX_AUTO_START_TASK},
   {0}
};


/*TASK*-----------------------------------------------------
* 
* Task Name    : Main_task
* Comments     :
*    This task prints " Hello World "
*
*END*-----------------------------------------------------*/

void Main_task(uint32_t initial_data) {
    TCHRES_INSTALL_PARAM_STRUCT install_params;
    FILE_PTR tchscr_dev;
    TCHRES_POSITION_STRUCT position;
    _mqx_int result;
    
    printf("\nTCHRES demo application\n");
    printf("This demo application requires TWR-LCD board.\n");

#if (defined BSP_TCHRES_ADC_XPLUS_DEVICE) && (defined BSP_TCHRES_ADC_YPLUS_DEVICE)
    printf("Opening ADC device for XPLUS electrode ... " BSP_TCHRES_ADC_XPLUS_DEVICE " ");
    if (fopen(BSP_TCHRES_ADC_XPLUS_DEVICE, (const char*)&adc_init) == NULL) {
        printf("failed\n");
        _task_block();
    } else {
        install_params.ADC_XPLUS_DEVICE = BSP_TCHRES_ADC_XPLUS_DEVICE;
        printf("done\n");
    }

    printf("Opening ADC device for YPLUS electrode ... " BSP_TCHRES_ADC_YPLUS_DEVICE " ");
    if (fopen(BSP_TCHRES_ADC_YPLUS_DEVICE, (const char*)&adc_init) == NULL) {
        printf("failed\n");
        _task_block();
    } else {
        install_params.ADC_YPLUS_DEVICE = BSP_TCHRES_ADC_YPLUS_DEVICE;
        printf("done\n");
    }
#elif (defined BSP_TCHRES_ADC_DEVICE)
    printf("Opening ADC device for XPLUS and YPLUS electrodes ... " BSP_TCHRES_ADC_DEVICE " ");
    if (fopen(BSP_TCHRES_ADC_DEVICE, (const char*)&adc_init) == NULL) {
        printf("failed\n");
        _task_block();
    } else {
        install_params.ADC_XPLUS_DEVICE = install_params.ADC_YPLUS_DEVICE = BSP_TCHRES_ADC_DEVICE;
        printf("done\n");
    }
#else
    printf("This demo application requires ADC devices for TCHRES to be defined!\n");
    _task_block();
#endif

    printf("Installing TCHRES device ... ");
    _io_tchres_install("tchscr:", &_bsp_tchscr_resisitve_init, &install_params);
    printf("done\n");
    
    printf("Opening TCHRES device ...");
    if ((tchscr_dev = fopen("tchscr:", NULL)) == NULL) {
        printf("failed\n");
        _task_block();
    } else {
        printf("done\n");
    }
    
    for (;;) {
        if ((result = _io_ioctl(tchscr_dev, IO_IOCTL_TCHSCR_GET_POSITION_RAW, &position)) == TCHRES_OK) {
            printf("Touch detected in raw ADC values (X,Y) = (%d, %d)\n", position.X, position.Y);
        } else {
            printf("No touch detected ...\n");
        }
        _time_delay(100);
    }
}

/* EOF */
