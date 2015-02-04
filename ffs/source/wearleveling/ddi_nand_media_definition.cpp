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
*   This file contains default media definition file for NAND.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_media.h"
#include "ddi_media_internal.h"
#include "ddi_nand.h"
#include "simple_mutex.h"
#include "ddi/media/ddi_nand_media.h"

/* 
** Constants 
*/

#define NUM_LOGICAL_MEDIA               1

#define MIN_DATA_DRIVE_SIZE             ( 8 )

/* 
** Global external declarations 
*/
/* NAND media */
/*
MediaAllocationTable_t g_mediaTablePreDefine[NUM_LOGICAL_MEDIA] = 
{
    
    {
        1,       
        { 0, kDriveTypeData, DRIVE_TAG_DATA, 13312000, true }
    } 
};
*/

nand::NandZipConfigBlockInfo_t g_nandZipConfigBlockInfo = 
{
    1,
    {
        kDriveTypeData,
        DRIVE_TAG_DATA,
        NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK, // 90
        0,
        NANDFLASH_1ST_DATA_DRIVE_START_BLOCK // 110
    }
    
    // TODO: multi fragment data drive example
    /*
    3,
    {
        {
            kDriveTypeData,
            DRIVE_TAG_DATA,
            10,
            0,
            0
        },
        {
            kDriveTypeUnknown,
            DRIVE_TAG_DATA,
            10,
            0,
            10
        },
        {
            kDriveTypeData,
            DRIVE_TAG_DATA,
            80,
            0,
            220
        },
    }
    */

};

const int g_MinDataDriveSize = MIN_DATA_DRIVE_SIZE;

WL_MUTEX g_NANDThreadSafeMutex;

/* EOF */
