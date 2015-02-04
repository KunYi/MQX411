/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   Processor and platform identifiers defined here
*
*
*END************************************************************************/

#ifndef __psp_cpudef_h__
    #define __psp_cpudef_h__

#include "mqx_cpudef.h"

/* CPU groups for I.MX devices */
#define PSP_CPU_GROUP_IMX21         (0x01)
#define PSP_CPU_GROUP_IMX23         (0x02)
#define PSP_CPU_GROUP_IMX25         (0x03)
#define PSP_CPU_GROUP_IMX27         (0x04)
#define PSP_CPU_GROUP_IMX28         (0x05)
#define PSP_CPU_GROUP_IMX31         (0x06)
#define PSP_CPU_GROUP_IMX35         (0x07)
#define PSP_CPU_GROUP_IMX37         (0x08)
#define PSP_CPU_GROUP_IMX50         (0x09)
#define PSP_CPU_GROUP_IMX51         (0x0A)
#define PSP_CPU_GROUP_IMX53         (0x0B)
#define PSP_CPU_GROUP_UCC           (0x0C)

/* TODO propose and check new numbers for Vybrid family a */
#define PSP_CPU_GROUP_VYBRID_M4      (0x41)
#define PSP_CPU_GROUP_VYBRID_A5      (0x51)



/* Specific CPU identification macros */
#define PSP_CPU_IMX21               (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX21, 0))
#define PSP_CPU_IMX21S              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX21, 1))

#define PSP_CPU_IMX233              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX23, 0))

#define PSP_CPU_IMX251              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX25, 0))
#define PSP_CPU_IMX253              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX25, 1))
#define PSP_CPU_IMX255              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX25, 2))
#define PSP_CPU_IMX257              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX25, 3))
#define PSP_CPU_IMX258              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX25, 4))

#define PSP_CPU_IMX27               (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX27, 0))
#define PSP_CPU_IMX27L              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX27, 1))

#define PSP_CPU_IMX281              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX28, 0))
#define PSP_CPU_IMX283              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX28, 1))
#define PSP_CPU_IMX285              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX28, 2))
#define PSP_CPU_IMX286              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX28, 3))
#define PSP_CPU_IMX287              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX28, 4))

#define PSP_CPU_IMX31               (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX31, 0))

#define PSP_CPU_IMX351              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX35, 0))
#define PSP_CPU_IMX353              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX35, 1))
#define PSP_CPU_IMX355              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX35, 2))
#define PSP_CPU_IMX356              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX35, 3))
#define PSP_CPU_IMX357              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX35, 4))

#define PSP_CPU_IMX37               (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_IMX37, 0))

#define PSP_CPU_IMX508              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX50, 0))

#define PSP_CPU_IMX512              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX51, 0))
#define PSP_CPU_IMX513              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX51, 1))
#define PSP_CPU_IMX514              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX51, 2))
#define PSP_CPU_IMX515              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX51, 3))
#define PSP_CPU_IMX516              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX51, 4))

#define PSP_CPU_IMX534              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX53, 0))
#define PSP_CPU_IMX535              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX53, 1))
#define PSP_CPU_IMX536              (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A8, PSP_CPU_GROUP_IMX53, 2))

#define PSP_CPU_MC13260             (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_OTHER, PSP_CPU_GROUP_UCC, 1))

#define PSP_CPU_VF65GS10_A5         (PSP_CPU_NUM(PSP_CPU_ARCH_ARM_CORTEX_A5, PSP_CPU_GROUP_VYBRID_A5, 2))

/* CPU id check support macros */
#define PSP_MQX_CPU_IS_IMX21        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX21))
#define PSP_MQX_CPU_IS_IMX23        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX23))
#define PSP_MQX_CPU_IS_IMX25        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX25))
#define PSP_MQX_CPU_IS_IMX27        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX27))
#define PSP_MQX_CPU_IS_IMX28        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX28))
#define PSP_MQX_CPU_IS_IMX31        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX31))
#define PSP_MQX_CPU_IS_IMX35        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX35))
#define PSP_MQX_CPU_IS_IMX37        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX37))
#define PSP_MQX_CPU_IS_IMX50        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX50))
#define PSP_MQX_CPU_IS_IMX51        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX51))
#define PSP_MQX_CPU_IS_IMX53        ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_IMX53))

/* I.MX processor family*/
#define PSP_MQX_CPU_IS_IMX          (PSP_MQX_CPU_IS_IMX21 | \
                                     PSP_MQX_CPU_IS_IMX23 | \
                                     PSP_MQX_CPU_IS_IMX25 | \
                                     PSP_MQX_CPU_IS_IMX27 | \
                                     PSP_MQX_CPU_IS_IMX28 | \
                                     PSP_MQX_CPU_IS_IMX31 | \
                                     PSP_MQX_CPU_IS_IMX35 | \
                                     PSP_MQX_CPU_IS_IMX37 | \
                                     PSP_MQX_CPU_IS_IMX50 | \
                                     PSP_MQX_CPU_IS_IMX51 | \
                                     PSP_MQX_CPU_IS_IMX53)

#define PSP_MQX_CPU_IS_UCC          ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_UCC))

#define PSP_MQX_CPU_IS_ARM9         (PSP_MQX_CPU_IS_UCC | PSP_MQX_CPU_IS_IMX)

#define PSP_MQX_CPU_IS_VYBRID_A5     ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_VYBRID_A5))
#define PSP_MQX_CPU_IS_VYBRID_M4     ((PSP_GET_CPU_GROUP(MQX_CPU) == PSP_CPU_GROUP_VYBRID_M4))

#define PSP_MQX_CPU_IS_VYBRID       (PSP_MQX_CPU_IS_VYBRID_A5 | PSP_MQX_CPU_IS_VYBRID_M4)

#endif
