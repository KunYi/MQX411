/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "io_rev.h"
#include "bsp_rev.h"


/* This function sits in a tight loop (interrupts will be disabled) for
 * exceptions that we are not currently handling. */
void unhandled_exception(void) {
    while(1);
}

/* Initialize the interrupt controller and turn ISRs on. */
void init_gic(void) {
    int i;
    uint16_t *ptr;
    extern void _gic_init(void);

    _gic_init();

    // set non-secure all interrupts
    GICD_IGROUPR(0) = 0xffffffff;
    GICD_IGROUPR(1) = 0xffffffff;
    GICD_IGROUPR(2) = 0xffffffff;
    GICD_IGROUPR(3) = 0xffffffff;
    GICD_IGROUPR(4) = 0xffffffff;

    // clear pending flags all interrupts
    GICD_ICPENDR(0) = 0xffffffff;
    GICD_ICPENDR(1) = 0xffffffff;
    GICD_ICPENDR(2) = 0xffffffff;
    GICD_ICPENDR(3) = 0xffffffff;
    GICD_ICPENDR(4) = 0xffffffff;

    // CPU interface
    //GICC_PMR = 0xff;    // set priority
    GICC_CTLR = 7;      // enable signaling of interrupts

    // distributor
    GICD_CTLR = 3;      // interrupts forwarded

    // route all interrupts to core 1
    for (i = 0, ptr = (uint16_t*)MSCM_IRSPRC0; i < 112; i++, ptr++) {
        *ptr = MSCM_IRSPRCn_CP1_MASK;
    }
}
