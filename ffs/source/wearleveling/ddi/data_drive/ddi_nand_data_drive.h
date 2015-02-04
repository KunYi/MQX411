#ifndef __ddi_nand_data_drive_h__
#define __ddi_nand_data_drive_h__
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
*   This file contains definitions of the NAND data drive class.
*
*
*END************************************************************************/

#include "ddi_nand_media.h"
#include "ddi_nand_ddi.h"
#include "nonsequential_sectors_map.h"

/* 
** Class definations 
*/

namespace nand {

/* NAND data drive. */
class DataDrive : public LogicalDrive
{
public:
    
    /* Default constructor. */
    DataDrive(Media * media, Region * region);
    
    /* Destructor. */
    virtual ~DataDrive();

    void addRegion(Region * region);
    
    /* Logical drive API */
    virtual RtStatus_t init();
    virtual RtStatus_t shutdown();
    virtual RtStatus_t getInfo(uint32_t infoSelector, void * value);
    virtual RtStatus_t setInfo(uint32_t infoSelector, const void * value);
    virtual RtStatus_t readSector(uint32_t sector, SECTOR_BUFFER * buffer);
    virtual RtStatus_t writeSector(uint32_t sector, const SECTOR_BUFFER * buffer);
    virtual RtStatus_t erase();
    virtual RtStatus_t flush();
    virtual RtStatus_t repair(bool bIsScanBad = false);

protected:
    /* The NAND media object that we belong to. */
    Media * m_media;
    uint32_t m_u32NumRegions;
    Region ** m_ppRegion;

    void processRegions(Region ** regionArray, unsigned * regionCount);
    void buildRegionsList();

    bool shouldRepairEraseBlock(uint32_t u32BlockFirstPage, NandPhysicalMedia *pNandDesc);

    RtStatus_t logicalBlockToVirtual(uint32_t logicalBlock, Region ** region, uint32_t * virtualBlock);
    RtStatus_t logicalSectorToVirtual(uint32_t logicalSector, Region ** region, uint32_t * virtualSector);

    RtStatus_t readSectorInternal(uint32_t u32LogicalSectorNumber, SECTOR_BUFFER * pSectorData);
    RtStatus_t writeSectorInternal(uint32_t u32LogicalSectorNumber, const SECTOR_BUFFER * pSectorData);

    bool isBlockHidden(uint32_t u32BlockPhysAddr);
};

} /* namespace nand */

/* 
** Function prototypes 
*/

void log_ecc_failures(nand::Region * pRegion, uint32_t wPhysicalBlockNumber, uint32_t wSectorOffset);

#endif /* __ddi_nand_data_drive_h__ */

/* EOF */
