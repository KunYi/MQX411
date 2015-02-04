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
*   This file contains the source for the RTC example program.
*
 *
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if ! BSPCFG_ENABLE_RTCDEV
#error This application requires BSPCFG_ENABLE_RTCDEV defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

/* 
 * RTC initialize time: Fri Sep 27 9:47:15 2013
 */
#define RTC_TIME_INIT_TM_YEAR   (2013)
#define RTC_TIME_INIT_TM_MON    (9)
#define RTC_TIME_INIT_TM_MDAY   (26)
#define RTC_TIME_INIT_TM_HOUR   (9)
#define RTC_TIME_INIT_TM_MIN    (47)
#define RTC_TIME_INIT_TM_SEC    (15)

#define ALARM_NEXT_TIME         (10)
#define ALARM_PERIOD            (4)
#define LWE_ALARM               (0x01)

extern void main_task(uint32_t);

/* Local function prototypes */
static void rtc_callback (void *rtc_registers_ptr);
static void print_rtc_time(uint32_t);

#if PSP_HAS_IRTC == 1
static void irtc_test(void);
#endif

#if BSPCFG_ENABLE_LCD
extern void lcd_task(uint32_t);
#endif

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { 1,           main_task,  2000,   8,          "Main", MQX_AUTO_START_TASK, 0,     0 },
#if BSPCFG_ENABLE_LCD
    { 2,           lcd_task,   2000,   9,          "LCD",  MQX_AUTO_START_TASK, 0,     0 },
#endif
    { 0 }
};

const char *_days_abbrev[] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char *_months_abbrev[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

#if PSP_HAS_IRTC == 1
static const uint8_t   backup_data[] = "Backup Data";
static uint8_t         read_data[sizeof(backup_data)];
#endif

LWEVENT_STRUCT lwevent;

/*TASK**************************************************************************
 *
 * Function Name    : main_task
 * Returned Value   :
 * Comments         : Playing aroung with the RTC module.
 *
 *END**************************************************************************/

void main_task
(
    uint32_t initial_data
)
{ /* Body */
    uint32_t        rtc_time;
    uint32_t        rtc_time_default;
    TIME_STRUCT     mqx_time;
    DATE_STRUCT     date_time;

    if (_lwevent_create(&lwevent,0) != MQX_OK)
    {
        printf("\nMake event failed");
        _task_block();
    }
    printf ("\f RTC Demo :\n\n");
    _rtc_get_time(&rtc_time);
    print_rtc_time(rtc_time);

    /* initialize time */
    date_time.YEAR     = RTC_TIME_INIT_TM_YEAR;
    date_time.MONTH    = RTC_TIME_INIT_TM_MON;
    date_time.DAY      = RTC_TIME_INIT_TM_MDAY;
    date_time.HOUR     = RTC_TIME_INIT_TM_HOUR;
    date_time.MINUTE   = RTC_TIME_INIT_TM_MIN;
    date_time.SECOND   = RTC_TIME_INIT_TM_SEC;
    date_time.MILLISEC = 0;

    /* Convert date time to time struct */
    if ( _time_from_date(&date_time, &mqx_time) == FALSE)
    {
        printf("\n Cannot convert date_time ");
        _task_block();
    }

    /* Convert rtc time to TIME_STRUCT */
    rtc_time = mqx_time.SECONDS;

    printf(" Set RTC time is: %s %s %3d %.2d:%.2d:%.2d %d\n",
            _days_abbrev[date_time.WDAY],
            _months_abbrev[date_time.MONTH - 1],
            date_time.DAY,
            date_time.HOUR,
            date_time.MINUTE,
            date_time.SECOND,
            date_time.YEAR);
    /* Set MQX time*/
    _time_set(&mqx_time);
    /* Set RTC time*/
    _rtc_set_time(rtc_time);
    printf("\n MQX time: %d SECONDS, %d MILISECOND ", mqx_time.SECONDS, mqx_time.MILLISECONDS);

    /*
     * set-up alarm
     */

    /* install callback */
    _rtc_callback_reg((INT_ISR_FPTR)rtc_callback, (void*)NULL);

    /* Get current time */
    _rtc_get_time(&rtc_time);

    /* setup alarm in next 10 seconds & period 4 seconds*/
    rtc_time += (uint32_t)ALARM_NEXT_TIME; /* Alarm occurs in next 10 seconds */
    printf("\n Setup to occur alarm in next %d seconds & with period: %d seconds",ALARM_NEXT_TIME, ALARM_PERIOD);
    _rtc_set_alarm(rtc_time,(uint32_t)ALARM_PERIOD);
    printf("\n Wait %d seconds ....",ALARM_NEXT_TIME);
    /* Wait to clear LWE_ALARM event */
    _lwevent_wait_ticks(&lwevent,LWE_ALARM,FALSE,0);
    _lwevent_clear(&lwevent,LWE_ALARM);
    printf ("\nALARM! ALARM! ALARM!\n");
    /* Print current time */
    _rtc_get_time(&rtc_time);
    print_rtc_time(rtc_time);

    printf("\n Wait next alarm in %d seconds....",ALARM_PERIOD);
    _lwevent_wait_ticks(&lwevent,LWE_ALARM,FALSE,0);
    _lwevent_clear(&lwevent,LWE_ALARM);
    printf ("\nALARM! ALARM! ALARM!\n");
    _rtc_get_time(&rtc_time);
    print_rtc_time(rtc_time);

    printf ("\nClearing RTC:\n");
    _rtc_set_time(0);
    _rtc_get_time(&rtc_time_default);
    print_rtc_time(rtc_time_default);
    /* Wait 2 seconds after resynchronize rtc time with mqx time*/
    do{
       _rtc_get_time(&rtc_time);
    } while ((rtc_time - rtc_time_default) < 2);

    /* Get current time & display on terminal */
    _rtc_get_time(&rtc_time);
    print_rtc_time(rtc_time);

    printf ("Synchronize RTC to MQX time again:\n");
    _time_get(&mqx_time);
    rtc_time = mqx_time.SECONDS;
    _rtc_set_time(rtc_time);
    
    _rtc_get_time(&rtc_time);
    print_rtc_time(rtc_time);
    
#if PSP_HAS_IRTC == 1
    irtc_test();
#endif /* PSP_HAS_IRTC == 1 */

    printf ("Finish, press/hold reset to repeat.\n");
    _task_block() ;
} /* Endbody */

/*FUNCTION**********************************************************************
 *
 * Function Name    : rtc_callback
 * Returned Value   :
 * Comments         :
 *    RTC interrupt callback
 *
 *END**************************************************************************/

static void rtc_callback (void *rtc_registers_ptr)
{
   _lwevent_set(&lwevent,LWE_ALARM);
}

/*FUNCTION**********************************************************************
 *
 * Function Name    : print_current_time
 * Returned Value   : none
 * Comments         :
 *    Get current time & printf it in "date" format to stdout
 *
 *END**************************************************************************/
static void print_rtc_time
(
    uint32_t rtc_time
)
{ /* Body*/
    TIME_STRUCT  ts;
    DATE_STRUCT  tm;

    ts.SECONDS = rtc_time;
    ts.MILLISECONDS = 0;
    /* Convert rtc_time to date format */
    if (_time_to_date(&ts, &tm) == FALSE ) {
        printf("\n Cannot convert rtc_time to date format");
        _task_block();
    }

    printf(" (RTC) Current time is: %s %s %3d %.2d:%.2d:%.2d %d\n",
            _days_abbrev[tm.WDAY],
            _months_abbrev[tm.MONTH - 1],
            tm.DAY,
            tm.HOUR,
            tm.MINUTE,
            tm.SECOND,
            (tm.YEAR));

} /* Endboy*/

/*FUNCTION**********************************************************************
 *
 * Function Name    : irtc_test
 * Returned Value   :
 * Comments         :
 *    RTC interrupt callback
 *
 *END**************************************************************************/

#if PSP_HAS_IRTC == 1
static void irtc_test
(
    void
)
{ /* Body */
    uint16_t i;

    /* Testing RTC Up counter */
    printf ("Testing RTC Up-Value Counter\n");
    printf ("Current   Up-Counter Value = %d\n", _rtc_get_upcounter());
    printf ("Increment Up-Counter Value by 12 ");

    /* Increment upcounter by 12 */
    for (i = 0; i < 12; i++) {
        _rtc_inc_upcounter();
        puts(".");
    }
    printf("\nNew  up-counter value = %d\n", _rtc_get_upcounter());

    /* Testing RTC Stand By RAM */
    printf("Testing RTC Stand By RAM\n");

    if (MQX_OK !=  _rtc_write_to_standby_ram(0, (uint8_t *)backup_data, sizeof(backup_data)))
    {
        /* Writing data to stand-by RAM failed */
        puts("Stand By RAM test failed \n");
    }
    else
    {
        /* Writing data to stand-by RAM successful */
        if (MQX_OK != _rtc_read_from_standby_ram(0, (uint8_t *)read_data, sizeof(read_data)))
        {
            puts("Stand-By RAM test failed \n");
        }
        else {
            printf ((char *)read_data);
            puts("\nStand-By RAM test succesfull \n");
        }
    }
} /* Endbody */
#endif /* PSP_HAS_IRTC == 1 */

#if BSPCFG_ENABLE_LCD
/*TASK**************************************************************************
 *
 * Function Name    : lcd_task
 * Returned Value   :
 * Comments         : Show rtc time on lcd.
 *
 *END**************************************************************************/

void lcd_task
(
        uint32_t initial_data
)
{
    DATE_STRUCT      time_rtc;
    uint32_t         time;
    TIME_STRUCT      ts;
    eLCD_Symbols     spec_sym;
    char             time[5];
    char             state = 0;

    puts("\n\n");
    if (_lcd_init() == IO_ERROR)
    {
        puts("_lcd_init() function returned IO_ERROR\n");
        puts("lcd_task blocked\n\n");
        _task_block();
    }

    puts("TWRPI-SLCD display is connected and lcd_task is running\n");

    _lcd_segments( FALSE );
    while(1)
    {
        _rtc_get_time(&time);
        ts.SECONDS = time;
        ts.MILLISECONDS = 0;
        _time_to_date(&ts, &time_rtc);

        /* post meridiem */
        if( time_rtc.HOUR > 12 )
        {
            time_rtc.HOUR -= 12;
            spec_sym = LCD_AM;
            _lcd_symbol( spec_sym, FALSE );
            spec_sym = LCD_PM;
            _lcd_symbol( spec_sym, TRUE);
        }
        else
        {
            spec_sym = LCD_PM;
            _lcd_symbol( spec_sym, FALSE);
            spec_sym = LCD_AM;
            _lcd_symbol( spec_sym, TRUE);
        }
        sprintf( time, "%2d%2d", time_rtc.HOUR, time_rtc.MINUTE);
        if( time_rtc.MINUTE < 10 )
        {
            time[2] = '0';
        }
        _lcd_puts( time );
        spec_sym = LCD_COL1;
        state = (state + 1) & 1;
        _lcd_symbol( spec_sym, (bool)state );
        _time_delay(1000);
    } /* Endwhile */
} /* Endbody */
#endif /* BSPCFG_ENABLE_LCD */

/* EOF */
