#if !defined(__ddi_nand_block_h__)
#define __ddi_nand_block_h__
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
*   This file contains a class to wrap a block of a NAND.
*
*
*END************************************************************************/

#include "ddi_nand_hal.h"

/* 
** Class definations 
*/

namespace nand
{

    /* Constant for the first page offset within a block. */
    const uint32_t kFirstPageInBlock = 0;

    /* Representation of one block of a NAND. */
    class Block : public BlockAddress
    {
    public:
        /* Init and cleanup */
        /* Default constructor, inits to block 0. */
        Block();
        
        explicit Block(const BlockAddress & addr);
        
        /* Assignment operator. */
        Block & operator = (const Block & other);
        
        /* Assignment operator to change block address. */
        Block & operator = (const BlockAddress & addr);

        /* Assignment operator to change block address. */
        Block & operator = (const PageAddress & page);

        /* Addresses */
        void set(const BlockAddress & addr);

        /* Prefix increment operator to advance the address to the next block. */
        Block & operator ++ ();
        
        /* Prefix decrement operator. */
        Block & operator -- ();
        
        /* Increment operator. */
        Block & operator += (uint32_t amount);
        
        /* Decrement operator. */
        Block & operator -= (uint32_t amount);
        
        /* Accessors */
        /* Returns the number of pages in this block. */
        inline unsigned getPageCount() const { return m_nand->pNANDParams->wPagesPerBlock; }
        
        /* Returns the NAND object owning this block. */
        inline NandPhysicalMedia * getNand() const { return m_nand; }

        /* Operations */
        RtStatus_t readPage(unsigned pageOffset, SECTOR_BUFFER * buffer, SECTOR_BUFFER * auxBuffer);

        RtStatus_t readMetadata(unsigned pageOffset, SECTOR_BUFFER * buffer);
        
        RtStatus_t writePage(unsigned pageOffset, const SECTOR_BUFFER * buffer, SECTOR_BUFFER * auxBuffer);
        
        /* Erase this block. */
        RtStatus_t erase();
        
        /* Test whether the block is marked bad. */
        bool isMarkedBad(SECTOR_BUFFER * auxBuffer=NULL, RtStatus_t * status=NULL);
        
        /* Erase this block and mark it bad. */
        RtStatus_t markBad();
        
        /* 
        ** Erase the block and mark it bad if the erase fails.
        **
        ** If the erase fails, then the erase error code will be returned even if marking the block
        ** bad succeeded. This lets the caller know not to use the block. 
        */
        RtStatus_t eraseAndMarkOnFailure();
        
        /* Tests whether the block is already erased. */
        bool isErased();

    protected:
        /* The physical NAND owning this block. */
        NandPhysicalMedia * m_nand;
    };

} /* namespace nand */

#endif /* __ddi_nand_block_h__ */

/* EOF */
