#ifndef __drive_tags_h__
#define __drive_tags_h__
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
*   This file contains drive tags
*
*
*END************************************************************************/

/* 
** Constants 
*/

/* Drive tag definitions */
/* Player drive tag. */
#define DRIVE_TAG_STMPSYS_S                     0x00
#define DRIVE_TAG_STMPSYS_S_INFO                ("Player Firmware Drive")

/* UsbMscMtp drive tag, old name was DRIVE_TAG_USBMSC_S.  */
#define DRIVE_TAG_HOSTLINK_S                    0x01
#define DRIVE_TAG_HOSTLINK_S_INFO               ("Hostlink Firmware Drive")

/* Primary player resource drive tag. */
#define DRIVE_TAG_RESOURCE_BIN                  0x02
#define DRIVE_TAG_RESOURCE_BIN_INFO             ("Primary Resource Drive")

/* Backup player resource drive tag. */
#define DRIVE_TAG_RESOURCE2_BIN                 0x12
#define DRIVE_TAG_RESOURCE2_BIN_INFO            ("Backup Resource Drive")

/* Master player resource drive tag. */
#define DRIVE_TAG_RESOURCE_MASTER_BIN           0x22
#define DRIVE_TAG_RESOURCE_MASTER_BIN_INFO      ("Master Resource Drive")

/* The host has 0x05 reserved for OTG drive. */
#define DRIVE_TAG_OTGHOST_S                     0x05
#define DRIVE_TAG_OTGHOST_S_INFO                ("USB OTG Drive")

/* Internal data drive tag. */
#define DRIVE_TAG_DATA                          0x00
#define DRIVE_TAG_DATA_INFO                     ("Public Data Drive")

/* External data drive tag. */
#define DRIVE_TAG_DATA_EXTERNAL                 0x01
#define DRIVE_TAG_DATA_EXTERNAL_INFO            ("External Media Data Drive")

/* Hidden data drive tag for DRM information. */
#define DRIVE_TAG_DATA_HIDDEN                   0x02
#define DRIVE_TAG_DATA_HIDDEN_INFO              ("Hidden Data Drive")

/* Hidden data drive 2 tag for persistent settings. */
#define DRIVE_TAG_DATA_HIDDEN_2                 0x03
#define DRIVE_TAG_DATA_HIDDEN_2_INFO            ("Second Hidden Data Drive")

/* Primary player firmware drive tag. */
#define DRIVE_TAG_BOOTMANAGER_S                 0x50
#define DRIVE_TAG_BOOTMANAGER_S_INFO            ("Primary Firmware Drive")

/* Backup player firmware drive tag. */
#define DRIVE_TAG_BOOTMANAGER2_S                0x60
#define DRIVE_TAG_BOOTMANAGER2_S_INFO           ("Backup Firmware Drive")

/* Master player firmware drive tag. */
#define DRIVE_TAG_BOOTMANAGER_MASTER_S          0x70
#define DRIVE_TAG_BOOTMANAGER_MASTER_S_INFO     ("Master Firmware Drive")

/* Bootlet firmware drive tag. */
#define DRIVE_TAG_BOOTLET_S                     0xa0
#define DRIVE_TAG_BOOTLET_S_INFO                ("Bootlet Firmware Drive")

/* The host has 0xFF reserved for usbmsc.sb file used in recovery mode operation only. */
#define DRIVE_TAG_UPDATER_S                     0xFF
#define DRIVE_TAG_UPDATER_S_INFO                ("USB Recovery")

#endif /* __drive_tags_h__ */

/* EOF */
