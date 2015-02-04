#ifndef __ddi_nand_hal_internal_h__
#define __ddi_nand_hal_internal_h__
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   Declarations internal to the NAND HAL.
*
*
*
*END************************************************************************/

#include "wl_common.h"
#include "sectordef.h"
#include "ddi_nand_hal.h"
#include "simple_mutex.h"

#define MAX_NAND_DEVICES  (1)

/* 
** Definations 
*/

/* Type 2 status byte masks. */
enum _nand_hal_type_2_status
{
    /* 0=Page N Program Pass, 1=Fail */
    kType2StatusPassMask = 0x01,        
    
    /* 0=Page N-1 Program Pass, 1=Fail */
    kType2StatusCachePassMask = 0x02,   
    
    /* 1=Ready, 0=Busy */
    kType2StatusReadyMask = 0x20,       
    
    /* 1=Cache Ready, 0=Cache Busy */
    kType2StatusCacheReadyMask = 0x40,  
    
    /* 0=Protected, 1=Unprotected */
    kType2StatusWriteProtectMask = 0x80 
};

/* Type 6 status byte masks. */
enum _nand_hal_type_6_status
{
    /* 0=Program Pass, 1=Fail */
    kType6StatusPassMask = 0x01,

    /* 1=Ready, 0=Busy */
    kType6StatusReadyMask = 0x40,       
    
    /* 0=Protected, 1=Unprotected */
    kType6StatusWriteProtectMask = 0x80 
};

/* Toshiba PBA-NAND status byte masks. */
enum _nand_hal_type_16_status
{
    /* 0=Page N Program Pass, 1=Fail */
    kType16StatusPassMask = 0x01,      

    /* 0=Page N-1 Program Pass, 1=Fail */
    kType16StatusCachePassMask = 0x02,      
    
    /* 0=District 0 Pass, 1=Fail */
    kType16StatusDistrict0PassMask = 0x02,  
    
    /* 0=District 1 Pass, 1=Fail */
    kType16StatusDistrict1PassMask = 0x04,  
    
    /* 1=Need Reclaim, 0=No Reclaim Needed */
    kType16StatusReadReclaimMask = 0x10,    
    
    /* 1=Ready, 0=Busy */
    kType16StatusReadyMask = 0x20,   
    
    /* 1=Cache Ready, 0=Cache Busy */
    kType16StatusCacheReadyMask = 0x40      
};

/* The standard maximum percentage of blocks that may go bad. */
const unsigned kDefaultMaxBadBlockPercentage = 5;

/*
** \brief Global context information for the HAL.
**
** This structure contains all of the important global information for the HAL,
** such as the number of active chip selects. It also includes the all-important pointers to
** the NAND object for each chip select. In addition, there is information about the
** NANDs that is common to all chip selects, such as the Read ID command results and the
** NAND parameters struct.
*/
typedef struct NandHalContext {
    /* The mutex that serializes all access to the HAL. */
    WL_MUTEX serializationMutex;
    
    /* Number of active chip selects. */    
    unsigned chipSelectCount;   
    
    /* Combined number of blocks from all chip selects. */
    uint32_t totalBlockCount;   
    
    /* Shared description of NAND properties. */
    NandParameters_t parameters;    
    
    /* Pointers to the individual NAND objects. */
    NandPhysicalMedia * nands[MAX_NAND_DEVICES];
} NandHalContext_t;

/*  Forward declaration of the context global for use in NandHalMutex. */
extern NandHalContext_t g_nandHalContext;
extern MQX_FILE_PTR     g_nandHalFDPTR[MAX_NAND_DEVICES];

#ifdef __cplusplus
/*
** Automatic mutex locker for the NAND HAL serialization mutex.
*/
class NandHalMutex : public SimpleMutex
{
public:
    /* Constructor. */
    NandHalMutex() 
    : SimpleMutex(NULL) {}
    
    /* Destructor. */
    ~NandHalMutex() {}
};

#endif

#endif /* __ddi_nand_hal_internal_h__ */

/* EOF */
