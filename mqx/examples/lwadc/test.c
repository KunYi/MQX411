/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2011 Embedded Access Inc.
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
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_LWADC_MODULE
#error This application requires BSP_DEFAULT_LWADC_MODULE to be not defined in the BSP. Please recompile BSP with this option.
#endif

#ifndef BSP_ADC_POTENTIOMETER
#error This application requires BSP_ADC_POTENTIOMETER to be defined in the BSP. Please recompile BSP with this option.
#endif


extern void test_task(uint32_t);

extern const LWADC_INIT_STRUCT BSP_DEFAULT_LWADC_MODULE;

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
/* Task Index,   Function,   Stack,  Priority, Name,     Attributes,    Param,            Time Slice */
    { 1,   test_task, 2000,   8,        "test_task",  MQX_AUTO_START_TASK, 0, 0 },
    { 0 }
};


typedef struct adc_demo_struct {
    const char *    name;
    uint32_t         input;
} ADC_DEMO_STRUCT;

/* This structure defines the generic ADCs available on this board. The structure will be populated based on
** what generic ADCs are defined in the BSP. The generic ADCs currently supported are:
**  Potentiometer:                  BSP_ADC_POTENTIOMETER
**  Up to 8 generic ADC inputs:     BSP_ADC_INPUT_0..BSP_ADC_INPUT_7
**  One TWRPI ADC interface:        BSP_ADC_TWRPI_PINx
**  Tower primary elevator          BSP_ADC_TWR_AN0..BSP_ADC_TWR_AN7
**  Tower secondary elevator        BSP_ADC_TWR_AN8..BSP_ADC_TWR_AN13
**  Core voltage:                   BSP_ADC_VDD_CORE
**  Temperature:                    BSP_ADC_TEMPERATURE */

const ADC_DEMO_STRUCT adc_inputs[] = {
        
#ifdef BSP_ADC_POTENTIOMETER 
   {"Potentiometer", BSP_ADC_POTENTIOMETER },
#endif    
   
#ifdef BSP_ADC_INPUT_0 
   {"Generic input #0", BSP_ADC_INPUT_0 },
#endif
   
#ifdef BSP_ADC_INPUT_1 
   {"Generic input #1", BSP_ADC_INPUT_1 },
#endif
   
#ifdef BSP_ADC_INPUT_2 
   {"Generic input #2", BSP_ADC_INPUT_2 },
#endif
   
#ifdef BSP_ADC_INPUT_3 
   {"Generic input #3", BSP_ADC_INPUT_3 },
#endif
   
#ifdef BSP_ADC_INPUT_4 
   {"Generic input #4", BSP_ADC_INPUT_4 },
#endif
   
#ifdef BSP_ADC_INPUT_5 
   {"Generic input #5", BSP_ADC_INPUT_5 },
#endif
   
#ifdef BSP_ADC_INPUT_6 
   {"Generic input #6", BSP_ADC_INPUT_6 },
#endif
   
#ifdef BSP_ADC_INPUT_7 
   {"Generic input #7", BSP_ADC_INPUT_7 },
#endif
   
#ifdef BSP_ADC_TWRPI_PIN8 
   {"TWRPI Pin#8", BSP_ADC_TWRPI_PIN8 },
#endif                
#ifdef BSP_ADC_TWRPI_PIN9 
   {"TWRPI Pin#9", BSP_ADC_TWRPI_PIN9 },
#endif                
#ifdef BSP_ADC_TWRPI_PIN12 
   {"TWRPI Pin#12", BSP_ADC_TWRPI_PIN12 },
#endif                
#ifdef BSP_ADC_TWRPI_PIN17 
   {"TWRPI Pin#17", BSP_ADC_TWRPI_PIN17 },
#endif                
#ifdef BSP_ADC_TWRPI_PIN18 
   {"TWRPI Pin#18", BSP_ADC_TWRPI_PIN18 },
#endif                

#ifdef BSP_ADC_TWR_AN0 
   {"Primary Elevator AN0", BSP_ADC_TWR_AN0 },
#endif                
#ifdef BSP_ADC_TWR_AN1 
   {"Primary Elevator AN1", BSP_ADC_TWR_AN1 },
#endif                
#ifdef BSP_ADC_TWR_AN2 
   {"Primary Elevator AN2", BSP_ADC_TWR_AN2 },
#endif                
#ifdef BSP_ADC_TWR_AN3 
   {"Primary Elevator AN3", BSP_ADC_TWR_AN3 },
#endif                
#ifdef BSP_ADC_TWR_AN4 
   {"Primary Elevator AN4", BSP_ADC_TWR_AN4 },
#endif                
#ifdef BSP_ADC_TWR_AN5 
   {"Primary Elevator AN5", BSP_ADC_TWR_AN5 },
#endif                
#ifdef BSP_ADC_TWR_AN6 
   {"Primary Elevator AN6", BSP_ADC_TWR_AN6 },
#endif                
#ifdef BSP_ADC_TWR_AN7 
   {"Primary Elevator AN7", BSP_ADC_TWR_AN7 },
#endif                
      
#ifdef BSP_ADC_TWR_AN8 
   {"Secondary Elevator AN8", BSP_ADC_TWR_AN8 },
#endif                
#ifdef BSP_ADC_TWR_AN9 
   {"Secondary Elevator AN9", BSP_ADC_TWR_AN9 },
#endif                
#ifdef BSP_ADC_TWR_AN10 
   {"Secondary Elevator AN10", BSP_ADC_TWR_AN10 },
#endif                

#ifdef BSP_ADC_TWR_AN11 
   {"Secondary Elevator AN11", BSP_ADC_TWR_AN11 },
#endif                
#ifdef BSP_ADC_TWR_AN12 
   {"Secondary Elevator AN12", BSP_ADC_TWR_AN12 },
#endif                
#ifdef BSP_ADC_TWR_AN13 
   {"Secondary Elevator AN13", BSP_ADC_TWR_AN13 },
#endif                

#ifdef BSP_ADC_VDD_CORE 
   {"Core VDD", BSP_ADC_VDD_CORE },
#endif                
#ifdef BSP_ADC_TEMPERATURE 
   {"Temperature", BSP_ADC_TEMPERATURE },
#endif                

};



static void print_lwadc_attribute( LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute, const char * name)
{
    uint32_t value;
    
    if (_lwadc_get_attribute(lwadc_ptr,attribute,&value)) {
        printf("%-30s = 0x%08x (%d)\n", name,value,value);
    } else {
        printf("%-30s not supported\n", name);
    }
}

#define PRINT_LWADC_ATTRIBUTE(p,x) print_lwadc_attribute(p,x,#x)

static void print_all_lwadc_attributes( LWADC_STRUCT_PTR lwadc_ptr)
{
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_RESOLUTION);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_REFERENCE);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_FREQUENCY);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_DIVIDER);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_DIFFERENTIAL);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_POWER_DOWN);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_NUMERATOR);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_DENOMINATOR);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_FORMAT);
    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_INPUT_CONVERSION_ENABLE);
}


static void monitor_potentiometer(uint32_t input, uint32_t max_range)
{
    LWADC_STRUCT    potentiometer;
    LWADC_VALUE     percent, last = MAX_UINT_32, min_percent = 100, max_percent = 0;
    uint32_t         resolution, reference;
    
    printf("Monitoring potentiometer\n");

    /* Initialize an LWADC_STRUCT for the potentiometer  */
    _lwadc_init_input(&potentiometer,input);
    
    /* Print out all the attributes associated with it. Some will be device-wide attributes */
    print_all_lwadc_attributes(&potentiometer);
    
    /* We want to change the default scaling of the potentiometer to return a percent instead of milli-volts.
    ** We change the numerator to reflect the range, and the denominator to reflect the max potentiometer value. */
    if (!_lwadc_set_attribute(&potentiometer,LWADC_NUMERATOR,100)) {
        printf("Error, unable to set attribute.\n");
        return;
    }
    if (!_lwadc_get_attribute(&potentiometer,LWADC_RESOLUTION,&resolution)) {
        printf("Error, unable to get attribute.\n");
        return;
    }
    if (!_lwadc_get_attribute(&potentiometer,LWADC_REFERENCE,&reference)) {
        printf("Error, unable to get attribute.\n");
        return;
    }
    if (!_lwadc_set_attribute(&potentiometer,LWADC_DENOMINATOR,resolution*max_range/reference)) {
        printf("Error, unable to set attribute.\n");
        return;
    }

    /* Print out what we changed.*/
    PRINT_LWADC_ATTRIBUTE(&potentiometer, LWADC_NUMERATOR);
    PRINT_LWADC_ATTRIBUTE(&potentiometer, LWADC_DENOMINATOR);

    printf("Rotate potentiometer. Test will run until readings range from  10%% to 90%% \n");
   
    while ((min_percent>10) || (max_percent<90)) {
        /* Request 10 consecutive readings, averaged and scaled. */
        _lwadc_read_average(&potentiometer, 10, &percent);
        
        /* If it is different, print it. */
        if (percent != last) {
            printf("potentiometer = %3d\r",percent);
            last=percent;
            if (percent<min_percent) min_percent=percent;
            if (percent>max_percent) max_percent=percent;
        }
    }
    printf("\n");
}

static void monitor_all_inputs(void)
{
    LWADC_STRUCT_PTR    lwadc_inputs;
    LWADC_VALUE_PTR     last;
    LWADC_VALUE         i,scaled, raw, delta, max_delta = 160;

    printf("Monitoring all inputs\n");

    lwadc_inputs = (LWADC_STRUCT_PTR) _mem_alloc_zero(ELEMENTS_OF(adc_inputs)*sizeof(LWADC_STRUCT));
    last         = (LWADC_VALUE_PTR)  _mem_alloc_zero(ELEMENTS_OF(adc_inputs)*sizeof(LWADC_VALUE));
    
    if ((lwadc_inputs == NULL) || (last==NULL)) {
        printf("Error, Insufficient memory to run full test\n.");
        _task_block();
    }
    
    for (i=0;i<ELEMENTS_OF(adc_inputs);i++) {
        /* Set last value to a value out of range of the ADC. */
        last[i] = MAX_UINT_32;
        if ( !_lwadc_init_input(&lwadc_inputs[i],adc_inputs[i].input) ) {
            /* Failed to initialize this input. We will end up failing the reads below as well. */
            printf("Failed to initialize ADC input %s\n",adc_inputs[i].name);
        }
    }
    
    printf("Monitoring ADC Inputs\n");
    while (1) {
        for (i=0;i<ELEMENTS_OF(adc_inputs);i++) {
            /* This waits until a new conversion is read on the channel */
            if (_lwadc_wait_next(&lwadc_inputs[i])) {
                if (_lwadc_read(&lwadc_inputs[i], &scaled) &&
                    _lwadc_read_raw(&lwadc_inputs[i], &raw)) {
                    
                    /* Obtained data, is the change significant enough to display? */
                    delta = (raw>last[i])?raw-last[i]:last[i]-raw;
                    if (delta > max_delta) {
                        printf("%-30s = %04x (%d mv)\n",adc_inputs[i].name, raw,scaled);
                        last[i]=raw;
                    }
                }
            }
        }
    }
}

void test_task(uint32_t initial_data)
{
    printf("\n\n\nADC Test for %s\n",BSP_NAME);

    _lwadc_init(&BSP_DEFAULT_LWADC_MODULE);

    #ifdef BSP_ADC_POTENTIOMETER
        monitor_potentiometer(BSP_ADC_POTENTIOMETER,BSP_ADC_POTENTIOMETER_MAX);
    #endif
    monitor_all_inputs();
}

/* EOF */
