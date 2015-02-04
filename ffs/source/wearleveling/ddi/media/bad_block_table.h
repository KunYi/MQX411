#if !defined(__badblocktable_h__)
#define __badblocktable_h__
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
*   This file contains BadBlockTable class defination
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_nand_hal.h"

/* 
** Class definations 
*/

namespace nand 
{
    class BadBlockTable 
    {
    public:
        /* Default constructor. */
        BadBlockTable();
        
        /* Destructor. */
        ~BadBlockTable();
        
        /* Allocate memory for input number of blocks. */
        RtStatus_t allocate(uint32_t numberBlocks);

        RtStatus_t scanAndBuildBBT();
        
        /* Free all memory owned by the object. */
        void release();
        
        bool isBlockBad(uint32_t blockAddress);    

        uint8_t * getEntries() { return m_entries; }

        uint8_t getEntry(uint32_t index);

        bool isAllocated() { return m_isAllocated; }

        void markBad(uint32_t blockAddress);

        uint32_t getNumberBadBlocks() { return m_numberBadBlocks; }

    protected:
        /* Bad blocks bitmap */
        uint8_t * m_entries;
        /* Size of bitmap */
        uint32_t m_size;
        /* Number of bad blocks */
        uint32_t m_numberBadBlocks;
        /* Flag is used to maked global BadBlockTable variable as allocated */
        bool m_isAllocated;
    };
} /* namespace nand */

#endif /* __badblocktable_h__ */

/* EOF */
