/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains board-specific HMI BTNLED settings.
*
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include "TSS_API.h"

/* Sensitivity value for each electrode */
static const uint8_t  sensitivity_values[] =
{
        0x40,0x40,0x40,0x40,    
        0x40,0x40,0x40,0x40,
        0x40,0x40,0x40,0x40,
        0x40,0x40,0x40,0x40,
        0x40,0x40,0x40,0x40,
        0x40,0x40,0x40,0x40,
        0x40    
};


/* HMI TSS System control */
static const HMI_TSS_SYSTEM_CONTROL_STRUCT hmi_system_control_kinetis=
{
        (TSS_SYSTEM_EN_MASK | TSS_DC_TRACKER_EN_MASK),
        TSS_TRIGGER_MODE_ALWAYS,     /* system trigger */
        0x08,                        /* number of samples */
        sensitivity_values           /* pointer to sensitivity values */
};

/* TSS ON BOARD keypad */
static const HMI_TSS_INIT_STRUCT hmi_init_table_tss_twrpi_void[]=    
{
        /* UID, FLAG */
        {HMI_BUTTON_1,0},    /* E1 - touch button 1 */
        {HMI_BUTTON_2,0},    /* E2 - touch button 2 */
        {HMI_BUTTON_3,0},    /* E3 - touch button 3 */
        {HMI_BUTTON_4,0},    /* E4 - touch button 4 */
        {0,0}
};

/* TSS TWRPI keypad */
static const HMI_TSS_INIT_STRUCT hmi_init_table_tss_twrpi_keypad[]=    
{
        /* UID, FLAG */
        {HMI_BUTTON_1,0},    /* 1 - keypad */
        {HMI_BUTTON_2,0},    /* 2 - keypad */
        {HMI_BUTTON_3,0},    /* 3 - keypad */
        {HMI_BUTTON_4,0},    /* 4 - keypad */
        {HMI_BUTTON_5,0},    /* 5 - keypad */
        {HMI_BUTTON_6,0},    /* 6 - keypad */
        {HMI_BUTTON_7,0},    /* 7 - keypad */
        {HMI_BUTTON_8,0},    /* 8 - keypad */
        {HMI_BUTTON_9,0},    /* 9 - keypad */
        {HMI_BUTTON_10,0},    /* * - keypad */
        {HMI_BUTTON_11,0},    /* 0 - keypad */
        {HMI_BUTTON_12,0},    /* # - keypad */
        {0,0}
};

/* TSS TWRPI button rotary */
static const HMI_TSS_INIT_STRUCT hmi_init_table_tss_twrpi_rotary_keypad[]=
{
        /* UID, FLAG */
        {HMI_BUTTON_1,0},    /* center button on TWRPI rotary */
        {0,0}
};

/* TSS TWRPI rotary */
static const HMI_TSS_INIT_STRUCT hmi_init_table_tss_twrpi_rotary[]=
{
    /* UID, FLAG */
    {HMI_ROTARY_1,0},    /* rotary */
    {0,0}
};

/* LWGPIO buttons and LEDS 
*  interrupt not used - HMI_LWGPIO_FLAG_INT_ON is not set for buttons
*/
static const HMI_LWGPIO_INIT_STRUCT hmi_init_table_lwgpio[]={    
        /* UID, PIN ID, FUNCTIONALITY, FLAGS */
        {HMI_LED_1, BSP_LED1, BSP_LED1_MUX_GPIO, HMI_LWGPIO_FLAG_DIRECTION_OUT|HMI_LWGPIO_FLAG_ACTIVE_STATE_0},    /* LED 1 */
        {HMI_LED_2, BSP_LED2, BSP_LED2_MUX_GPIO, HMI_LWGPIO_FLAG_DIRECTION_OUT|HMI_LWGPIO_FLAG_ACTIVE_STATE_0},    /* LED 2 */
        {HMI_LED_3, BSP_LED3, BSP_LED3_MUX_GPIO, HMI_LWGPIO_FLAG_DIRECTION_OUT|HMI_LWGPIO_FLAG_ACTIVE_STATE_0},    /* LED 3 */
        {HMI_LED_4, BSP_LED4, BSP_LED4_MUX_GPIO, HMI_LWGPIO_FLAG_DIRECTION_OUT|HMI_LWGPIO_FLAG_ACTIVE_STATE_0},    /* LED 4 */
        {HMI_BUTTON_5, BSP_SW1, BSP_BUTTON1_MUX_GPIO, HMI_LWGPIO_FLAG_DIRECTION_IN|HMI_LWGPIO_FLAG_ACTIVE_STATE_0|HMI_LWGPIO_FLAG_PULL_UP_ON},    /* BUTTON 1 */
        {HMI_BUTTON_6, BSP_SW2, BSP_BUTTON2_MUX_GPIO, HMI_LWGPIO_FLAG_DIRECTION_IN|HMI_LWGPIO_FLAG_ACTIVE_STATE_0|HMI_LWGPIO_FLAG_PULL_UP_ON},    /* BUTTON 2 */
        {0,0,0},
};

static HMI_CLIENT_STRUCT_PTR hmi_btnled_handle_ptr;                 /* Client pointer */
static HMI_PROVIDER_STRUCT_PTR hmi_twrpi_provider_keypad_ptr;       /* On Board KEYPAD provider pointer*/
static HMI_PROVIDER_STRUCT_PTR hmi_twrpi_provider_keypad1_ptr;      /* TWRPI KEYPAD provider pointer */
static HMI_PROVIDER_STRUCT_PTR hmi_twrpi_provider_rotaryA_ptr;      /* TWRPI ROTARY provider pointer */
static HMI_PROVIDER_STRUCT_PTR hmi_twrpi_provider_rotaryB_ptr;      /* TWRPI ROTARY Button provider pointer */
static HMI_PROVIDER_STRUCT_PTR hmi_twrpi_provider_lwgpio_ptr;       /* LWGPIO provider pointer */ 
static _mqx_int actual_provider = 0;                                /* Present provider which is set, default = 0*/

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_btnled_init
* Returned Value   : HMI_CLIENT_STRUCT_PTR for success, NULL for failure
* Comments         :
*    This function performs BSP-specific initialization related to HMI.
*    It installs interrupts for TSS and initializes btnled client with
*    default providers.
*END*----------------------------------------------------------------------*/
HMI_CLIENT_STRUCT_PTR _bsp_btnled_init(void)
{
    
    _int_install_isr(INT_TSI0, TSS_TSI0Isr, NULL);    /* Install and then enable TSI0 Isr */
    _bsp_int_init(INT_TSI0, BSP_TSI_INT_LEVEL, 0, TRUE);
    _bsp_int_enable(INT_TSI0);

    _bsp_tss_io_init();                                 /* TSI init */
    hmi_tss_init(hmi_system_control_kinetis);           /* TSS init */

    /* BTNLED initialization */
    hmi_btnled_handle_ptr = btnled_init();
    if(hmi_btnled_handle_ptr == NULL)
    {
        return NULL;    /* BTNLED not initialized */
    }

    /* providers init - only providers available on board */
    hmi_twrpi_provider_lwgpio_ptr = hmi_lwgpio_provider_init(hmi_init_table_lwgpio);    /* LWGPIO */
    hmi_twrpi_provider_keypad_ptr = hmi_tss_keypad_provider_init(hmi_init_table_tss_twrpi_void, 0);  /* TSS Keypad - C0_TYPE */   

    /* add providers to the btnled table */
    if (!hmi_add_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_keypad_ptr) || !hmi_add_provider(hmi_btnled_handle_ptr,hmi_twrpi_provider_lwgpio_ptr))
    {
        return NULL;    /* provider's addition to the client's table failed */
    }
    return hmi_btnled_handle_ptr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_btnled_set_twrpi
* Returned Value   : MQX_OK for success, MQX_OUT_OF_MEMORY for a failure
* Comments         :
*    This function selects TWRPI module.
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_btnled_set_twrpi(_mqx_int twrpi_index)
{    
    _mqx_int result = MQX_OUT_OF_MEMORY;
    
    hmi_tss_init(hmi_system_control_kinetis);            /* TSS init */
    
    if (hmi_btnled_handle_ptr == NULL) /* client not initialized */
    {
        return result;
    }
    if (actual_provider != twrpi_index)    /* changing provider */
    {
        switch(actual_provider)        /* actual providers - deinit */
        {
            case BSP_TWRPI_VOID:
                /* remove provider from client table */
                hmi_remove_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_keypad_ptr);
                /* provider deinit */
                if (hmi_tss_provider_deinit(hmi_twrpi_provider_keypad_ptr))
                {
                    return MQX_MEM_POOL_INVALID;  /* deinit failed */
                }
                break;
            case BSP_TWRPI_KEYPAD:
                hmi_remove_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_keypad1_ptr);
                if (hmi_tss_provider_deinit(hmi_twrpi_provider_keypad1_ptr))
                {
                    return MQX_MEM_POOL_INVALID;  /* deinit failed */
                }
                break;
            case BSP_TWRPI_ROTARY:
                hmi_remove_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_rotaryA_ptr);
                hmi_remove_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_rotaryB_ptr);
                if (hmi_tss_provider_deinit(hmi_twrpi_provider_rotaryA_ptr) && hmi_tss_provider_deinit(hmi_twrpi_provider_rotaryB_ptr))
                {
                    return MQX_MEM_POOL_INVALID;  /* deinit failed */
                }
                break;
            default:
                break;
        }
        switch(twrpi_index)        /* init new providers */
        {
            case BSP_TWRPI_VOID:
                /* init provider */
                hmi_twrpi_provider_keypad_ptr = hmi_tss_keypad_provider_init(hmi_init_table_tss_twrpi_void, 0); /* TSS Keypad - C0_TYPE */
                /* add provider to client table, true if client and provider were not null and were added. */
                if (hmi_add_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_keypad_ptr))
                {
                    result = MQX_OK;
                }
                break;
            case BSP_TWRPI_KEYPAD:
                hmi_twrpi_provider_keypad1_ptr = hmi_tss_keypad_provider_init(hmi_init_table_tss_twrpi_keypad, 1);  /* TSS Keypad1 - C1_TYPE */
                if (hmi_add_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_keypad1_ptr))
                {
                    result = MQX_OK;
                }
                break;
            case BSP_TWRPI_ROTARY:
                hmi_twrpi_provider_rotaryA_ptr = hmi_tss_rotary_provider_init(hmi_init_table_tss_twrpi_rotary, 3);  /* TSS Rotary - C3_TYPE */
                hmi_twrpi_provider_rotaryB_ptr = hmi_tss_keypad_provider_init(hmi_init_table_tss_twrpi_rotary_keypad, 2);   /* TSS Rotary - C2_TYPE */
                if (hmi_add_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_rotaryA_ptr) && hmi_add_provider(hmi_btnled_handle_ptr, hmi_twrpi_provider_rotaryB_ptr))
                {
                    result = MQX_OK;
                }
                break;
            default:
                break;
        }
        if (result == MQX_OK)
        {
            actual_provider = twrpi_index;    /* store index */
        }
        else
        {
            actual_provider = -1;    /* something went wrong and provider was not set */
        }
    }
        
    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_btnled_get_twrpi
* Returned Value   : _mqx_int
* Comments         :
*    This function returns present TWRPI module.
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_btnled_get_twrpi()
{    
    return actual_provider;    /* return actual provider */
}
/* EOF */
