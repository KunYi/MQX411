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
*   This file contains the NAND HAL common functions.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "hal/ddi_nand_hal_internal.h"
#include "ddi_nand.h"
#include "ddi_nand_hal_nfcphymedia.h"

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getChipSelectCount
* Returned Value   : Numnber of active chips
* Comments         :
*   This function return the number of active chip selects.
*
*END*--------------------------------------------------------------------*/
unsigned NandHal::getChipSelectCount()
{ /* Body */
    return g_nandHalContext.chipSelectCount;
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getParameters
* Returned Value   : NandParameters
* Comments         :
*   This function accesses the shared parameters object.
*
*END*--------------------------------------------------------------------*/
NandParameters_t & NandHal::getParameters()
{/* Body */
    return g_nandHalContext.parameters;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getNand
* Returned Value   : NandPhysicalMedia
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
NandPhysicalMedia * NandHal::getNand
(   
    /* [IN] Chip number */
    unsigned chipSelect
)
{ /* Body */
    assert(chipSelect < MAX_NAND_DEVICES);
    return g_nandHalContext.nands[chipSelect];
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getChipSelectForAbsoluteBlock
* Returned Value   : Chip number
* Comments         :
*   This function returns the chip select number given an absolute block address.
*
*END*--------------------------------------------------------------------*/
unsigned NandHal::getChipSelectForAbsoluteBlock
(
    /* [IN] Absolute block  */
    uint32_t block
)
{ /* Body */
    unsigned chipSelect;
    
    for (chipSelect = 0; chipSelect < g_nandHalContext.chipSelectCount; ++chipSelect)
    {
        NandPhysicalMedia * nand = g_nandHalContext.nands[chipSelect];
        if (block < nand->wTotalBlocks)
        {
            return chipSelect;
        }/* Endif */
        
        /*  Subtract off this chip's blocks. */
        block -= nand->wTotalBlocks;
    }/* Endfor */
    
    /*  The block was out of range. */
    assert(0);
    // return 0;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getChipSelectForAbsolutePage
* Returned Value   : Chip number
* Comments         :
*   This function returns the chip select number given an absolute page address.
*
*END*--------------------------------------------------------------------*/
unsigned NandHal::getChipSelectForAbsolutePage
(
    /* [IN] Absolute page */
    uint32_t page
)
{ /* Body */
    uint32_t block = g_nandHalContext.nands[0]->pageToBlock(page);
    return getChipSelectForAbsoluteBlock(block);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getNandForAbsoluteBlock
* Returned Value   : Nand
* Comments         :
*   This function returns the NAND object for a given absolute block address.
*
*END*--------------------------------------------------------------------*/
NandPhysicalMedia * NandHal::getNandForAbsoluteBlock
(
    /* [IN] Absolute block */
    uint32_t block
)
{ /* Body */
    return getNand(getChipSelectForAbsoluteBlock(block));
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getNandForAbsolutePage
* Returned Value   : Nand
* Comments         :
*   This function returns the NAND object for a given absolute page address.
*
*END*--------------------------------------------------------------------*/
NandPhysicalMedia * NandHal::getNandForAbsolutePage
(   
    /* [IN] Absolute page */
    uint32_t page
)
{ /* Body */
    return getNand(getChipSelectForAbsolutePage(page));
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getTotalBlockCount
* Returned Value   : Total blocks
* Comments         :
*   This function returns the combined number of blocks of all chip selects.
*
*END*--------------------------------------------------------------------*/
uint32_t NandHal::getTotalBlockCount()
{ /* Body */
    return g_nandHalContext.totalBlockCount;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isAbsoluteBlockValid
* Returned Value   : TRUE or FALSE
* Comments         :
*   This function determine whether a block address is valid.
*
*END*--------------------------------------------------------------------*/
bool NandHal::isAbsoluteBlockValid
(
    /* [IN] Block address */
    uint32_t block
)
{ /* Body */
    unsigned chipSelect;
    
    for (chipSelect = 0; chipSelect < g_nandHalContext.chipSelectCount; ++chipSelect)
    {
        NandPhysicalMedia * nand = g_nandHalContext.nands[chipSelect];
        if (block < nand->wTotalBlocks)
        {
            return true;
        } /* Endif */
        
        /* Subtract off this chip's blocks. */
        block -= nand->wTotalBlocks;
    }/* Endfor */
    
    /* The block was out of range. */
    return false;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isAbsolutePageValid
* Returned Value   : TRUE or FALSE
* Comments         :
*   This function determine whether a page address is valid.
*
*END*--------------------------------------------------------------------*/
bool NandHal::isAbsolutePageValid
(
    /* [IN] Page address */
    uint32_t page
)
{ /* Body */
    uint32_t block = g_nandHalContext.nands[0]->pageToBlock(page);
    return isAbsoluteBlockValid(block);
} /* Endbody */

/* EOF */
