/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This header file contain registers definition for MMU register
*   Shoul be removed when this definition will be included in IO map file!!
*
*
*
*END************************************************************************/
#ifndef __mmu_cortexa5_h__
#define __mmu_cortexa5_h__

#ifndef MMU_L2_SUPPORT
    #define MMU_L2_SUPPORT                              1
#endif

#define MMU_PAGE_TABLE_LEVEL_2_DESC_BASE_ADDR_SHIFT     (10)
#define MMU_PAGE_TABLE_LEVEL_2_DESC_BASE_ADDR_MASK      ((0x3fffffu) << (MMU_PAGE_TABLE_LEVEL_2_DESC_BASE_ADDR_SHIFT))

#define MMU_PAGE_TABLE_P_SHIFT                          (9)
#define MMU_PAGE_TABLE_P_MASK                           ((0x1u) << (MMU_PAGE_TABLE_P_SHIFT))

#define MMU_PAGE_TABLE_DOMAIN_SHIFT                     (5)
#define MMU_PAGE_TABLE_DOMAIN_MASK                      ((0xfu) << (MMU_PAGE_TABLE_DOMAIN_SHIFT))

#define MMU_PAGE_TABLE_ENTRY_TYPE_SHIFT                 (0)
#define MMU_PAGE_TABLE_ENTRY_TYPE_MASK                  ((0x3u) << (MMU_PAGE_TABLE_ENTRY_TYPE_SHIFT))

#define MMU_SECTION_SECTION_BASE_ADDR_SHIFT             (20)
#define MMU_SECTION_SECTION_BASE_ADDR_MASK              ((0xfffu) << (MMU_SECTION_SECTION_BASE_ADDR_SHIFT))

#define MMU_SECTION_TABLE_ENTRY_TYPE_A_SHIFT            (18)
#define MMU_SECTION_TABLE_ENTRY_TYPE_A_MASK             ((0x1u) << (MMU_SECTION_SECTION_BASE_ADDR_SHIFT))

#define MMU_SECTION_NG_SHIFT                            (17)
#define MMU_SECTION_NG_MASK                             ((0x1u) << (MMU_SECTION_NG_SHIFT))

#define MMU_SECTION_S_SHIFT                             (16)
#define MMU_SECTION_S_MASK                              ((0x1u) << (MMU_SECTION_S_SHIFT))

#define MMU_SECTION_APX_SHIFT                           (15)
#define MMU_SECTION_APX_MASK                            ((0x1u) << (MMU_SECTION_APX_SHIFT))

#define MMU_SECTION_TEX_SHIFT                           (12)
#define MMU_SECTION_TEX_MASK                            ((0x7u) << (MMU_SECTION_TEX_SHIFT))

#define MMU_SECTION_AP_SHIFT                            (10)
#define MMU_SECTION_AP_MASK                             ((0x3u) << (MMU_SECTION_AP_SHIFT))

#define MMU_SECTION_P_SHIFT                             (9)
#define MMU_SECTION_P_MASK                              ((0x1u) << (MMU_SECTION_P_SHIFT))

#define MMU_SECTION_DOMAIN_SHIFT                        (5)
#define MMU_SECTION_DOMAIN_MASK                         ((0xfu) << (MMU_SECTION_DOMAIN_SHIFT))

#define MMU_SECTION_XN_SHIFT                            (4)
#define MMU_SECTION_XN_MASK                             ((0x1u) << (MMU_SECTION_XN_SHIFT))

#define MMU_SECTION_C_SHIFT                             (3)
#define MMU_SECTION_C_MASK                              ((0x1u) << (MMU_SECTION_C_SHIFT))

#define MMU_SECTION_B_SHIFT                             (2)
#define MMU_SECTION_B_MASK                              ((0x1u) << (MMU_SECTION_B_SHIFT))

#define MMU_SECTION_ENTRY_TYPE_SHIFT                    (0)
#define MMU_SECTION_ENTRY_TYPE_MASK                     ((0x3u) << (MMU_SECTION_ENTRY_TYPE_SHIFT))

#define MMU_SUPER_SECTION_SUPER_SECTION_BASE_ADDR_SHIFT (24)
#define MMU_SUPER_SECTION_SUPER_SECTION_BASE_ADDR_MASK  ((0xffu) << (MMU_SUPER_SECTION_SUPER_SECTION_BASE_ADDR_SHIFT))

#define MMU_SUPER_SECTION_TABLE_ENTRY_TYPE_A_SHIFT      (18)
#define MMU_SUPER_SECTION_TABLE_ENTRY_TYPE_A_MASK       ((0x1u) << (MMU_SUPER_SECTION_SUPER_SECTION_BASE_ADDR_SHIFT))

#define MMU_SUPER_SECTION_NG_SHIFT                      (17)
#define MMU_SUPER_SECTION_NG_MASK                       ((0x1u) << (MMU_SUPER_SECTION_NG_SHIFT))

#define MMU_SUPER_SECTION_S_SHIFT                       (16)
#define MMU_SUPER_SECTION_S_MASK                        ((0x1u) << (MMU_SUPER_SECTION_S_SHIFT))

#define MMU_SUPER_SECTION_APX_SHIFT                     (15)
#define MMU_SUPER_SECTION_APX_MASK                      ((0x1u) << (MMU_SUPER_SECTION_APX_SHIFT))

#define MMU_SUPER_SECTION_TEX_SHIFT                     (12)
#define MMU_SUPER_SECTION_TEX_MASK                      ((0x7u) << (MMU_SUPER_SECTION_TEX_SHIFT))

#define MMU_SUPER_SECTION_AP_SHIFT                      (10)
#define MMU_SUPER_SECTION_AP_MASK                       ((0x3u) << (MMU_SUPER_SECTION_AP_SHIFT))

#define MMU_SUPER_SECTION_P_SHIFT                       (9)
#define MMU_SUPER_SECTION_P_MASK                        ((0x1u) << (MMU_SUPER_SECTION_P_SHIFT))

#define MMU_SUPER_SECTION_DOMAIN_SHIFT                  (5)
#define MMU_SUPER_SECTION_DOMAIN_MASK                   ((0xfu) << (MMU_SUPER_SECTION_DOMAIN_SHIFT))

#define MMU_SUPER_SECTION_XN_SHIFT                      (4)
#define MMU_SUPER_SECTION_XN_MASK                       ((0x1u) << (MMU_SUPER_SECTION_XN_SHIFT))

#define MMU_SUPER_SECTION_C_SHIFT                       (3)
#define MMU_SUPER_SECTION_C_MASK                        ((0x1u) << (MMU_SUPER_SECTION_C_SHIFT))

#define MMU_SUPER_SECTION_B_SHIFT                       (2)
#define MMU_SUPER_SECTION_B_MASK                        ((0x1u) << (MMU_SUPER_SECTION_B_SHIFT))

#define MMU_SUPER_SECTION_ENTRY_TYPE_SHIFT              (0)
#define MMU_SUPER_SECTION_ENTRY_TYPE_MASK               ((0x3u) << (MMU_SUPER_SECTION_ENTRY_TYPE_SHIFT))

#define MMU_LARGE_PAGE_LARGE_PAGE_BASE_ADDR_SHIFT       (16)
#define MMU_LARGE_PAGE_LARGE_PAGE_BASE_ADDR_MASK        ((0xffffu) << (MMU_LARGE_PAGE_LARGE_PAGE_BASE_ADDR_SHIFT))

#define MMU_LARGE_PAGE_XN_SHIFT                         (15)
#define MMU_LARGE_PAGE_XN_MASK                          ((0x1u) << (MMU_LARGE_PAGE_XN_SHIFT))

#define MMU_LARGE_PAGE_TEX_SHIFT                        (12)
#define MMU_LARGE_PAGE_TEX_MASK                         ((0x7u) << (MMU_LARGE_PAGE_TEX_SHIFT))

#define MMU_LARGE_PAGE_NG_SHIFT                         (11)
#define MMU_LARGE_PAGE_NG_MASK                          ((0x1u) << (MMU_LARGE_PAGE_NG_SHIFT))

#define MMU_LARGE_PAGE_S_SHIFT                          (10)
#define MMU_LARGE_PAGE_S_MASK                           ((0x1u) << (MMU_LARGE_PAGE_S_SHIFT))

#define MMU_LARGE_PAGE_APX_SHIFT                        (9)
#define MMU_LARGE_PAGE_APX_MASK                         ((0x1u) << (MMU_LARGE_PAGE_APX_SHIFT))

#define MMU_LARGE_PAGE_AP_SHIFT                         (4)
#define MMU_LARGE_PAGE_AP_MASK                          ((0x3u) << (MMU_LARGE_PAGE_AP_SHIFT))

#define MMU_LARGE_PAGE_C_SHIFT                          (3)
#define MMU_LARGE_PAGE_C_MASK                           ((0x1u) << (MMU_LARGE_PAGE_C_SHIFT))

#define MMU_LARGE_PAGE_B_SHIFT                          (2)
#define MMU_LARGE_PAGE_B_MASK                           ((0x1u) << (MMU_LARGE_PAGE_B_SHIFT))

#define MMU_LARGE_PAGE_ENTRY_TYPE_SHIFT                 (0)
#define MMU_LARGE_PAGE_ENTRY_TYPE_MASK                  ((0x3u) << (MMU_LARGE_PAGE_ENTRY_TYPE_SHIFT))

#define MMU_SMALL_PAGE_SMALL_PAGE_BASE_ADDR_SHIFT       (12)
#define MMU_SMALL_PAGE_SMALL_PAGE_BASE_ADDR_MASK        ((0xfffffu) << (MMU_SMALL_PAGE_SMALL_PAGE_BASE_ADDR_SHIFT))

#define MMU_SMALL_PAGE_NG_SHIFT                         (11)
#define MMU_SMALL_PAGE_NG_MASK                          ((0x1u) << (MMU_SMALL_PAGE_NG_SHIFT))

#define MMU_SMALL_PAGE_S_SHIFT                          (10)
#define MMU_SMALL_PAGE_S_MASK                           ((0x1u) << (MMU_SMALL_PAGE_S_SHIFT))

#define MMU_SMALL_PAGE_APX_SHIFT                        (9)
#define MMU_SMALL_PAGE_APX_MASK                         ((0x1u) << (MMU_SMALL_PAGE_APX_SHIFT))

#define MMU_SMALL_PAGE_TEX_SHIFT                        (6)
#define MMU_SMALL_PAGE_TEX_MASK                         ((0x7u) << (MMU_SMALL_PAGE_TEX_SHIFT))

#define MMU_SMALL_PAGE_AP1_SHIFT                        (5)
#define MMU_SMALL_PAGE_AP1_MASK                         ((0x1u) << (MMU_SMALL_PAGE_AP1_SHIFT))

#define MMU_SMALL_PAGE_AP0_SHIFT                        (4)
#define MMU_SMALL_PAGE_AP0_MASK                         ((0x1u) << (MMU_SMALL_PAGE_AP0_SHIFT))

#define MMU_SMALL_PAGE_AP_SHIFT                         (4)
#define MMU_SMALL_PAGE_AP_MASK                          ((0x3u) << (MMU_SMALL_PAGE_AP_SHIFT))


#define MMU_SMALL_PAGE_C_SHIFT                          (3)
#define MMU_SMALL_PAGE_C_MASK                           ((0x1u) << (MMU_SMALL_PAGE_C_SHIFT))

#define MMU_SMALL_PAGE_B_SHIFT                          (2)
#define MMU_SMALL_PAGE_B_MASK                           ((0x1u) << (MMU_SMALL_PAGE_B_SHIFT))

#define MMU_SMALL_PAGE_ENTRY_TYPE_SHIFT                 (1)
#define MMU_SMALL_PAGE_ENTRY_TYPE_MASK                  ((0x1u) << (MMU_SMALL_PAGE_ENTRY_TYPE_SHIFT))

#define MMU_SMALL_PAGE_XN_SHIFT                         (0)
#define MMU_SMALL_PAGE_XN_MASK                          ((0x1u) << (MMU_SMALL_PAGE_XN_SHIFT))

#define MMU_L1_INDEX_SHIFT                              (20)
#define MMU_L1_INDEX_MASK                               ((0xfffu) << MMU_L1_INDEX_SHIFT)

#define MMU_L2_INDEX_SHIFT                              (12)
#define MMU_L2_INDEX_MASK                               ((0xfffffu) << MMU_L2_INDEX_SHIFT)


#define MMU_L2_TABLE_SIZE                               (0x400)  //256 entries * 4bytes = 1024
#define MMU_L2_TABLE_ENTRIES                            (0x100) //256
#define MMU_L2_TABLE_ALIGN                              (0x400) //1024

/* define for APX/AP bit setting*/
#define MMU_AP_PERMISSION_FAULT        0
#define MMU_AP_PRIVILEGED_ACCESS_ONLY  1
#define MMU_AP_NO_USER_MODE_WRITE      2
#define MMU_AP_FULL_ACCESS             3
#define MMU_AP_RESERVED_1              4
#define MMU_AP_PRIVILEGED_READ_ONLY    5
#define MMU_AP_READ_ONLY               6
#define MMU_AP_RESERVED_2              7

#endif
