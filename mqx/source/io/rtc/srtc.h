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
*   This file contains the type definitions for the mcf52xx RTC module.
*
*
*END************************************************************************/

#ifndef __rtc_srtc_h__
#define __rtc_srtc_h__ 1

#ifdef __cplusplus
extern "C" {
#endif

#define GIC_INTERRUPT_ID_OFFSET   0x10u
#define NIC_INTERRUPT_ID_OFFSET   0x20u
/* RTC register masks  */

#define SRTC_RTCSC_RTIF                      (1<<7)
#define SRTC_RTCSC_RTCLKS(x)                 (((x)&0x03)<<5)
#define SRTC_RTCSC_RTIE                      (1<<4)
#define SRTC_RTCSC_RTCPS(x)                  ((x)&0x0F)

/* RTC status bits  */

#define SRTC_RTCISR_SW                       (1<<0)
#define SRTC_RTCISR_MIN                      (1<<1)
#define SRTC_RTCISR_ALM                      (1<<2)
#define SRTC_RTCISR_DAY                      (1<<3)
#define SRTC_RTCISR_1HZ                      (1<<4)
#define SRTC_RTCISR_HR                       (1<<5)
#define SRTC_RTCISR_EN                       (1<<31)
#define SRTC_RTCIENR_SW                      (1<<0)
#define SRTC_RTCIENR_MIN                     (1<<1)
#define SRTC_RTCIENR_ALM                     (1<<2)
#define SRTC_RTCIENR_DAY                     (1<<3)
#define SRTC_RTCIENR_1HZ                     (1<<4)
#define SRTC_RTCIENR_HR                      (1<<5)
#define SRTC_STPWCH_CNT(x)                   ((x)&0x1F)

PSP_INTERRUPT_TABLE_INDEX _bsp_get_rtc_vector(void);
void                     *_bsp_get_rtc_base_address(void); 

#ifdef __cplusplus
}
#endif

#endif /*__rtc_srtc_h__*/

/* EOF */
