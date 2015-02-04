/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*  This file contains definitions that belongs to the LAN 8720
*  PHY chip.
*
*
*END************************************************************************/
#ifndef _phy_lan8720_h_
#define _phy_lan8720_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

/*
** Phy Register Indexes - Common Across Device Types.
*/
#define LAN8720_REG_BMCR                        (0x0000) /* Basic Mode Control Register */
#define LAN8720_REG_BMSR                        (0x0001) /* Basic Mode Status Register */
#define LAN8720_REG_IDR1                        (0x0002) /* PHY Identification Register #1 */
#define LAN8720_REG_IDR2                        (0x0003) /* PHY Identification Register #2 */
#define LAN8720_REG_ANAR                        (0x0004) /* Auto-Negotiation Advertisement Register */
#define LAN8720_REG_ANLPAR                      (0x0005) /* Auto-Negotiation Link Partner Ability Register */
#define LAN8720_REG_ANER                        (0x0006) /* Auto-Negotiation Expansion Register */
#define LAN8720_REG_MCSR                        (0x0011) /* Mode Control/Status Register */
#define LAN8720_REG_SM                          (0x0012) /* Special modes */
#define LAN8720_REG_SECR                        (0x001A) /* Symbol Error Count Register */
#define LAN8720_REG_CSIR                        (0x001B) /* Control/Status IndicationRegister */
#define LAN8720_REG_ISR                         (0x001D) /* Interrupt Source Register */
#define LAN8720_REG_IMR                         (0x001E) /* Interrupt Mask Register */
#define LAN8720_REG_PHY_SCSR                    (0x001F) /* PHY Special Control/Status Register */


/*
** Common Register Bit Definitions
*/

/* PHY_BMCR Defs */
#define LAN8720_REG_BMCR_COLLISION_TEST         (0x0080)
#define LAN8720_REG_BMCR_FORCE_FULL_DUP         (0x0100)
#define LAN8720_REG_BMCR_RESTART_AUTONEG        (0x0200)
#define LAN8720_REG_BMCR_ISOLATE                (0x0400)
#define LAN8720_REG_BMCR_POWER_DOWN             (0x0800)
#define LAN8720_REG_BMCR_AUTO_NEG_ENABLE        (0x1000)
#define LAN8720_REG_BMCR_FORCE_SPEED_100        (0x2000)
#define LAN8720_REG_BMCR_FORCE_SPEED_10         (0x0000) // !
#define LAN8720_REG_BMCR_FORCE_SPEED_MASK       (0x2000)
#define LAN8720_REG_BMCR_LOOPBACK               (0x4000)
#define LAN8720_REG_BMCR_RESET                  (0x8000)

/* PHY_BMSR Defs */
#define LAN8720_REG_BMSR_EXTENDED_CAPABLE       (0x0001)
#define LAN8720_REG_BMSR_JABBER_DETECT          (0x0002)
#define LAN8720_REG_BMSR_LINK_STATUS            (0x0004)
#define LAN8720_REG_BMSR_AUTO_NEG_ABILITY       (0x0008)
#define LAN8720_REG_BMSR_REMOTE_FAULT           (0x0010)
#define LAN8720_REG_BMSR_AUTO_NEG_COMPLETE      (0x0020)
#define LAN8720_REG_BMSR_10T_HALF_DUP           (0x0800)
#define LAN8720_REG_BMSR_10T_FULL_DUP           (0x1000)
#define LAN8720_REG_BMSR_100X_HALF_DUP          (0x2000)
#define LAN8720_REG_BMSR_100X_FULL_DUP          (0x4000)
#define LAN8720_REG_BMSR_100T4_CAPABLE          (0x8000)


/* PHY_IDR2 Defs */
#define LAN8720_REG_IDR2_OUI_MASK               (0xFC00)
#define LAN8720_REG_IDR2_MODEL_NUMBER_MASK      (0x03F0)
#define LAN8720_REG_IDR2_REVISION_MASK          (0x000F)
#define LAN8720_REG_IDR2_MODEL_SHIFT            (4)


/* PHY_ANAR Defs */
#define LAN8720_REG_ANAR_PROTO_SEL_MASK         (0x001F)
#define LAN8720_REG_ANAR_PROTO_8023             (0x0001) // !
#define LAN8720_REG_ANAR_10T_HALF_DUP           (0x0020)
#define LAN8720_REG_ANAR_10T_FULL_DUP           (0x0040)
#define LAN8720_REG_ANAR_100T_HALF_DUP          (0x0080)
#define LAN8720_REG_ANAR_100T_FULL_DUP          (0x0100)
#define LAN8720_REG_ANAR_PAUSE_SUPPORT          (0x0400)
#define LAN8720_REG_ANAR_ASY_PAUSE_SUPPORT      (0x0800)
#define LAN8720_REG_ANAR_REMOTE_FAULT           (0x2000)
#define LAN8720_REG_ANAR_NEXT_PAGE              (0x8000)


/* PHY_ANLPAR Defs */
#define LAN8720_REG_ANLPAR_PROTO_SEL_MASK       (0x001F)
#define LAN8720_REG_ANLPAR_10T_HALF_DUP         (0x0020)
#define LAN8720_REG_ANLPAR_10T_FULL_DUP         (0x0040)
#define LAN8720_REG_ANLPAR_100T_HALF_DUP        (0x0080)
#define LAN8720_REG_ANLPAR_100T_FULL_DUP        (0x0100)
#define LAN8720_REG_ANLPAR_100T4_SUPPORT        (0x0200)
#define LAN8720_REG_ANLPAR_PAUSE_SUPPORT        (0x0400)
#define LAN8720_REG_ANLPAR_REMOTE_FAULT         (0x2000)
#define LAN8720_REG_ANLPAR_ACK                  (0x4000)
#define LAN8720_REG_ANLPAR_NEXT_PAGE_IND        (0x8000)

/* PHY_ANER Defs */
#define LAN8720_REG_ANER_AUTO_NEG_CAPABLE       (0x0001)
#define LAN8720_REG_ANER_PAGE_RX                (0x0002)
#define LAN8720_REG_ANER_NEXT_PAGE_ABLE         (0x0004)
#define LAN8720_REG_ANER_PRT_NEXT_PAGE_ABLE     (0x0008)
#define LAN8720_REG_ANER_PARALLEL_DET_FAULT     (0x0010)

/* PHY_SCSR Defs */
#define LAN8720_REG_PHY_SCSR_SPEED_MASK         (0x000C)
#define LAN8720_REG_PHY_SCSR_SPEED_100          (0x0008)
#define LAN8720_REG_PHY_SCSR_SPEED_10           (0x0004)

#define MII_TIMEOUT                             (0x10000)

extern const ENET_PHY_IF_STRUCT phy_lan8720_IF;


#ifdef __cplusplus
}
#endif

#endif

/* EOF */
