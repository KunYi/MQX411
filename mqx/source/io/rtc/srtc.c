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
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

static uint32_t  alarm_time   = 0;
static uint32_t  alarm_period = 0;

static volatile  RTC_HANDLER   rtc_handler = {NULL, NULL};

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_init
* Returned Value   : MQX error code
* Comments         :
*    This function (re)initializes/clears/enables RTC module.
*
*END*********************************************************************/
int32_t _rtc_init
(
    /* [IN] param identifying init operations */
    void* param
)
{ /* Body */
    SNVS_MemMapPtr snvs = _bsp_get_rtc_base_address ();

    /* Enable SRTC, default setting: OSC clock 32768 */
    snvs->LPCR |= SNVS_LPCR_SRTC_ENV_MASK;
    return MQX_OK;
} /* End body */

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_isr
* Returned Value   : none
* Comments         :
*    This is ISR for RTC module, triggered each second.
*
*END*********************************************************************/
void _rtc_isr
(
    /* [IN] rtc module pointer passed to interrupt */
    void   *ptr
)
{ /* Body */
    SNVS_MemMapPtr snvs = _bsp_get_rtc_base_address ();
    uint32_t status = 0, result = 0;
    uint32_t tmp = snvs->LPCR;

    status = snvs->LPSR;
    if (status & SNVS_LPSR_LPTA_MASK)
    {
        snvs->LPSR |= SNVS_LPSR_LPTA_MASK;   /* clear interrupt flag  */

        if (alarm_period)
        {
            alarm_time += alarm_period;
        }
        else
        {
            alarm_time = 0;
        }

        snvs->LPCR  &= ~SNVS_LPCR_LPTA_EN_MASK;
        while (snvs->LPCR & SNVS_LPCR_LPTA_EN_MASK);
        /* Update SRTC alarm register */
        snvs->LPTAR = alarm_time;
        snvs->LPCR = tmp;

        /* call user call back */
        if (NULL != rtc_handler.isr_func)
        {
            rtc_handler.isr_func (rtc_handler.isr_dat);
        }
    } /* End if */
} /* End body */

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_callback_reg
* Returned Value   : MQX error code
* Comments         :
*    This function installs given user callback for RTC module.
*
*END*********************************************************************/
int32_t  _rtc_callback_reg
(
        /* [IN] pointer to user ISR code */
        INT_ISR_FPTR func,
        /* [IN] Type of callback (Alarm or stop watch) */
        void* data
)
{
    uint32_t       result = MQX_OK;
    SNVS_MemMapPtr snvs = _bsp_get_rtc_base_address ();
    uint32_t       srtc_vectors = _bsp_get_rtc_vector();
    if (func == NULL)
    {
        /* Disable SRTC alarm */
        snvs->LPCR &= ~SNVS_LPCR_LPTA_EN_MASK;
        /* Install default ISR for RTC interrupt */
        if (NULL == _int_install_isr (srtc_vectors, _int_get_default_isr(), NULL))
        {
            result = _task_get_error ();
        } /* Endif */
        _bsp_int_init(srtc_vectors, BSP_RTC_INT_LEVEL, 0, FALSE);

        rtc_handler.isr_func = NULL;
    }
    else
    {
        /* Install _rtc_isr() function for RTC interrupt */
        if (NULL == _int_install_isr (srtc_vectors, _rtc_isr, NULL))
        {
            result = _task_get_error ();
        } /* Endif */
        if (MQX_OK == result)
        {
            _bsp_int_init(srtc_vectors, BSP_RTC_INT_LEVEL, 0, TRUE);
            /* Enable SRTC alarm */
            snvs->LPCR |= SNVS_LPCR_LPTA_EN_MASK;
            /* Clear SRTC alarm flag*/
            snvs->LPSR |= SNVS_LPSR_LPTA_MASK;
            /* Clear isr fuction before changing its data */
            rtc_handler.isr_func = NULL;
            rtc_handler.isr_dat  = data;
            rtc_handler.isr_func = func;
        }
    }
    return result;
}


/*FUNCTION****************************************************************
*
* Function Name    : _rtc_set_time
* Returned Value   : MQX_OK
* Comments         :
*    This function sets the RTC time according to given time struct.
*
*END*********************************************************************/
int32_t _rtc_set_time
(
    /* [IN] given time to be set as RTC time */
    uint32_t time
)
{ /* Body */
    SNVS_MemMapPtr snvs = _bsp_get_rtc_base_address ();
    uint32_t tmp = snvs->LPCR;

    snvs->LPCR &= ~SNVS_LPCR_SRTC_ENV_MASK;   /* disable RTC */
    while (snvs->LPCR & SNVS_LPCR_SRTC_ENV_MASK);

    snvs->LPSRTCMR = (uint32_t)(time >> 17);
    snvs->LPSRTCLR = (uint32_t)(time << 15);

    snvs->LPCR = tmp;       /* restore RTC state */

    return MQX_OK;
} /* End body */

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_time
* Returned Value   : MQX_OK
* Comments         :
*    This function gets the RTC time and stores it in given time struct.
*
*END*********************************************************************/
int32_t _rtc_get_time
(
    /* [OUT] this parameter gets actual RTC time */
    uint32_t *time
)
{ /* Body */
    SNVS_MemMapPtr snvs = _bsp_get_rtc_base_address ();

    uint32_t tmp = 0;
    do {
        *time = tmp;
        tmp   = (snvs->LPSRTCMR << 17) | (snvs->LPSRTCLR >> 15) ;
    } while (tmp != *time);

    return MQX_OK;
} /* End body */

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_set_alarm
* Returned Value   : MQX_OK
* Comments         :
*    This function sets RTC alarm time according to given time struct.
*    Alarm happens immediately after match.
*
*END*********************************************************************/
int32_t _rtc_set_alarm
(
    /* [IN] time to be set as next RTC alarm time */
    uint32_t   time,
    /* [IN] time to be set as RTC alarm period time */
    uint32_t   period
)
{ /* Body */
    SNVS_MemMapPtr snvs = _bsp_get_rtc_base_address ();
    uint32_t tmp = snvs->LPCR;
    alarm_time   = time;
    alarm_period = period;
    snvs->LPCR  &= ~SNVS_LPCR_LPTA_EN_MASK;
    while (snvs->LPCR & SNVS_LPCR_LPTA_EN_MASK);
    snvs->LPTAR = alarm_time;
    snvs->LPCR = tmp;

    return MQX_OK;
} /* End body */

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_alarm
* Returned Value   : none
* Comments         :
*    This function gets the RTC alarm time and stores it in given time struct.
*
*END*********************************************************************/
int32_t _rtc_get_alarm
(
    /* [OUT] this parameter gets the RTC alarm time */
    uint32_t *time
)
{ /* Body */

    *time = alarm_time;

    return MQX_OK;
} /* End body */

/* EOF */