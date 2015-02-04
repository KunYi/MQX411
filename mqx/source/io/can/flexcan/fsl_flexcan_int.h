#ifndef FSL_FlexCAN_INT_h
#define FSL_FlexCAN_INT_h 1
/*HEADER**********************************************************************
*
* Copyright 2008-2013 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   This include file is used to provide constant and structure definitions
*   specific to the FlexCAN Serial Communications Controller
*   Revision History:
*   Apr 21, 2003   2.50          Initial version
*
*
*END************************************************************************/

#include <mqx.h>
#include <psp.h>


// CAN constants

#define CAN_MIN_TQ  8
#define CAN_MAX_TQ  25

#define CAN_MAX_PROPSEG 8
#define CAN_MAX_RJW 4
#define CAN_MIN_PSEG2 2


// concrete HW settings:
    
#define FLEXCAN_MESSBUF_INT_LEVEL       (3)
#define FLEXCAN_MESSBUF_INT_SUBLEVEL    (4)

#define FLEXCAN_ERROR_INT_LEVEL         (3)
#define FLEXCAN_ERROR_INT_SUBLEVEL      (2)

#define FLEXCAN_BUSOFF_INT_LEVEL        (3)
#define FLEXCAN_BUSOFF_INT_SUBLEVEL     (3)

#define FLEXCAN_WAKEUP_INT_LEVEL        (3)
#define FLEXCAN_WAKEUP_INT_SUBLEVEL     (1)

#define FLEXCAN_MSG_BUFADDR_OFFSET      (128)
#define FLEXCAN_MSG_BUFF_MAXDATA_SIZE   (8)

#ifndef FLEXCAN_IPBUS_CLK
#define FLEXCAN_IPBUS_CLK               (BSP_BUS_CLOCK) /* Indicates internal clock source */
#endif
#ifndef FLEXCAN_OSC_CLK
#define FLEXCAN_OSC_CLK                 (0) /* Must be set to external clock frequency, different to FLEXCAN_IPBUS_CLK! */ 
#endif

/*
** CAN interrupt types enum
*/
#define FLEXCAN_INT_BUF                 (0)
#if !defined(BSP_VYBRID_AUTOEVB_A5) || !defined(BSP_TWR_VF65GS10_A5)
#define FLEXCAN_INT_ERR                 (1)
#define FLEXCAN_INT_BOFF                (2)
#define FLEXCAN_INT_WAKEUP              (3)
#endif

/*
** CAN commands
*/
#define FLEXCAN_TX                      (0xFF)
#define FLEXCAN_RX                      (0x00)
#define FLEXCAN_EXTENDED                (0xFF)
#define FLEXCAN_STANDARD                (0x00)
#define FLEXCAN_ENABLE                  (0xFF)
#define FLEXCAN_DISABLE                 (0x00)

/*
** Module mode
*/
#define FLEXCAN_NORMAL_MODE             (0)
#define FLEXCAN_LISTEN_MODE             (1)
#define FLEXCAN_TIMESYNC_MODE           (2)
#define FLEXCAN_LOOPBK_MODE             (3)
#define FLEXCAN_BOFFREC_MODE            (4)
#define FLEXCAN_FREEZE_MODE             (5)
#define FLEXCAN_DISABLE_MODE            (6)

/*
** CAN wakeup interrupt mask
*/
#if defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_TWR_VF65GS10_A5)
#define CAN_MCR_WAKMSK_MASK             (0x0)
#define CAN_ESR1_WAKINT_MASK            (0x0)
#endif

/*
** FLEXCAN error codes
*/
#define FLEXCAN_OK                       (0x00)
#define FLEXCAN_UNDEF_ERROR              (FLEXCAN_ERROR_BASE | 0x01)
#define FLEXCAN_MESSAGE14_TX             (FLEXCAN_ERROR_BASE | 0x02)
#define FLEXCAN_MESSAGE15_TX             (FLEXCAN_ERROR_BASE | 0x03)
#define FLEXCAN_MESSAGE_OVERWRITTEN      (FLEXCAN_ERROR_BASE | 0x04)
#define FLEXCAN_NO_MESSAGE               (FLEXCAN_ERROR_BASE | 0x05)
#define FLEXCAN_MESSAGE_LOST             (FLEXCAN_ERROR_BASE | 0x06)
#define FLEXCAN_MESSAGE_BUSY             (FLEXCAN_ERROR_BASE | 0x07)
#define FLEXCAN_MESSAGE_ID_MISSMATCH     (FLEXCAN_ERROR_BASE | 0x08)
#define FLEXCAN_MESSAGE14_START          (FLEXCAN_ERROR_BASE | 0x09)
#define FLEXCAN_MESSAGE15_START          (FLEXCAN_ERROR_BASE | 0x0A)
#define FLEXCAN_INVALID_ADDRESS          (FLEXCAN_ERROR_BASE | 0x0B)
#define FLEXCAN_INVALID_MAILBOX          (FLEXCAN_ERROR_BASE | 0x0C)
#define FLEXCAN_TIMEOUT                  (FLEXCAN_ERROR_BASE | 0x0D)
#define FLEXCAN_INVALID_FREQUENCY        (FLEXCAN_ERROR_BASE | 0x0E)
#define FLEXCAN_INT_ENABLE_FAILED        (FLEXCAN_ERROR_BASE | 0x0F)
#define FLEXCAN_INT_DISABLE_FAILED       (FLEXCAN_ERROR_BASE | 0x10)
#define FLEXCAN_INT_INSTALL_FAILED       (FLEXCAN_ERROR_BASE | 0x11)
#define FLEXCAN_REQ_MAILBOX_FAILED       (FLEXCAN_ERROR_BASE | 0x12)
#define FLEXCAN_DATA_SIZE_ERROR          (FLEXCAN_ERROR_BASE | 0x13)
#define FLEXCAN_MESSAGE_FORMAT_UNKNOWN   (FLEXCAN_ERROR_BASE | 0x14)
#define FLEXCAN_INVALID_DIRECTION        (FLEXCAN_ERROR_BASE | 0x15)
#define FLEXCAN_RTR_NOT_SET              (FLEXCAN_ERROR_BASE | 0x16)
#define FLEXCAN_SOFTRESET_FAILED         (FLEXCAN_ERROR_BASE | 0x17)
#define FLEXCAN_INVALID_MODE             (FLEXCAN_ERROR_BASE | 0x18)
#define FLEXCAN_START_FAILED             (FLEXCAN_ERROR_BASE | 0x19)
#define FLEXCAN_CLOCK_SOURCE_INVALID     (FLEXCAN_ERROR_BASE | 0x1A)
#define FLEXCAN_INIT_FAILED              (FLEXCAN_ERROR_BASE | 0x1B)
#define FLEXCAN_ERROR_INT_ENABLE_FAILED  (FLEXCAN_ERROR_BASE | 0x1C)
#define FLEXCAN_ERROR_INT_DISABLE_FAILED (FLEXCAN_ERROR_BASE | 0x1D)
#define FLEXCAN_FREEZE_FAILED            (FLEXCAN_ERROR_BASE | 0x1E)



#ifdef __cplusplus
extern "C" {
#endif

extern void   *_bsp_get_flexcan_base_address(uint8_t);
extern IRQInterruptIndex _bsp_get_flexcan_vector (uint8_t,uint8_t,uint32_t);

extern uint32_t FLEXCAN_Int_enable(uint8_t,uint32_t);
extern uint32_t FLEXCAN_Int_disable(uint8_t,uint32_t);
extern uint32_t FLEXCAN_Install_isr(uint8_t,uint32_t,INT_ISR_FPTR);
#if !defined(BSP_VYBRID_AUTOEVB_A5)
extern uint32_t FLEXCAN_Error_int_enable(uint8_t);
extern uint32_t FLEXCAN_Error_int_disable(uint8_t);
extern uint32_t FLEXCAN_Install_isr_err_int(uint8_t,INT_ISR_FPTR);
extern uint32_t FLEXCAN_Install_isr_boff_int(uint8_t,INT_ISR_FPTR);
extern uint32_t FLEXCAN_Install_isr_wake_int(uint8_t,INT_ISR_FPTR);
#endif
extern  void   _FLEXCAN_ISR(void *);
extern uint32_t FLEXCAN_Int_status(uint8_t);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
