#if !defined(__metadata_h__)
#define __metadata_h__
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
*   This file contains class to wrap a metadata buffer.
*
*
*END************************************************************************/

/* 
** Macros 
*/

/*
** Tag macros
**
** These macros describe various tags we place in pages so we can recognize
** them as special to us.
*/

#define STM_TAG             (('S'<<16)|('T'<<8)|'M')
#define STMP_TAG            ((STM_TAG<<8)|'P')
#define BCB_TAG             (('B'<<16)|('C'<<8)|'B')
#define BCB_SPACE_TAG       ((BCB_TAG<<8)|' ')

namespace nand
{

    /* Utility class to manage a metadata buffer. */
    class Metadata
    {
    public:

    /*
    ** Field layout of the metadata.
    **
    ** There are two basic variants for the metadata fields, with several fields common to
    ** both. The most common has fields for the logical block address and logical sector
    ** index. This is used for all data drive blocks.
    **
    ** But system and boot blocks use a different set of fields that replaces of logical
    ** addresses with a four-byte tag or signature value. The tag value is written in
    ** big endian order, with the LSB appearing in #tag3. This is why the tag is broken into
    ** four byte-wide fields.
    **
    ** That value of the flags field is inverted from normal usage. That is, a flag is set
    ** if the bit is 0 and are cleared if the bit is 1. It is done this way because the
    ** default NAND bit value for an erased page is a 1.
    */
        struct Fields
        {
            /* Non-0xff value means the block is bad. */
            uint8_t blockStatus;
            
            /* Logical block number used for system drives. */
            uint8_t blockNumber;
            
            union {
                struct {
                    /* Halfword 0 of the logical block address. */
                    uint16_t lba0;
                    
                    /* The logical sector index. */
                    uint16_t lsi;
                } s_0;
                struct {
                    /* Byte 0 of the tag, MSB of the tag word. */
                    uint8_t tag0;
                    
                    /* Byte 1 of the tag. */
                    uint8_t tag1;
                    
                    /* Byte 2 of the tag. */
                    uint8_t tag2;
                    
                    /* Byte 3 of the tag, LSB of the tag word. */
                    uint8_t tag3;
                } s_1;
            } u_0;
            
            union {
                struct {
                    /* Halfword 1 of the logical block address. */
                    uint8_t lba1;
                    
                    /* Flags fields. */
                    uint8_t flags;
                } s_2;
                /* Trust number */
                struct {
                    uint8_t tag4;
                    uint8_t tag5;
                } s_3;
            } u_1;            
        };
        
        /* NAND metadata flag bitmasks. */
        enum
        {
            //! \brief When set, this flag indicates that the block belongs to a hidden drive.
            kIsHiddenBlockFlag = 1,
            
            /*
            ** Set to indicate that all pages in the block are sorted logically.
            **
            ** This flag is set on the last page in a block only when every page in that block is
            ** written in ascending logical order and there are no duplicate logical pages. So
            ** physical page 0 of the block contains logical page 0 (of the range of logical pages
            ** that fit into that block, not necessarily logical page 0 of the entire drive),
            ** physical page 1 contains logical page 1, and so on.
            */
            kIsInLogicalOrderFlag = 2    
        };
        
        /* Construction. */
        /* Default constructor. */
        Metadata() : m_fields(NULL) {}
        
        /* Constructor taking the metadata buffer pointer. */
        Metadata(SECTOR_BUFFER * buffer) : m_fields((Fields *)buffer) {}
        
        /* Assignment operator. */
        Metadata & operator = (const Metadata & other) { m_fields = other.m_fields; return *this; }
        
        /* Accessors */
        /* Returns the metadata buffer. */
        inline SECTOR_BUFFER * getBuffer() { return (SECTOR_BUFFER *)m_fields; }
        
        /* Changes the metadata buffer. */
        inline void setBuffer(SECTOR_BUFFER * buffer) { m_fields = (Fields *)buffer; }
        
        /* Field readers */
        /* Get the logical block address. */
        uint32_t getLba() const;
        
        /* Get the logical sector index. */
        uint16_t getLsi() const;
        
        /* Get the erase block number. */
        uint8_t getBlockNumber() const;
        
        /* Get the four byte signature. */
        uint32_t getSignature() const;
        
        /* Returns true if the flag is set. */
        bool isFlagSet(uint8_t flagMask) const;
        
        /* Returns true if the block status field is non-ff. */
        bool isMarkedBad() const;
        
        /* Returns true if the metadata is all ff. */
        bool isErased() const;
        
        /* Field writers */
        /* Sets the logical block address field. */
        void setLba(uint32_t lba);
        
        /* Sets the logical sector index field. */
        void setLsi(uint16_t lsi);
        
        /* Sets the erase block number. */
        void setBlockNumber(uint8_t blockNumber);
        
        /* Sets the signature field. */
        void setSignature(uint32_t signature);
        
        /* Sets a flag bit. */
        void setFlag(uint8_t flagMask);
        
        /* Clears a flag bit. */
        void clearFlag(uint8_t flagMask);
        
        /* Sets the block status byte to 0. */
        void markBad();
        
        /* Operations */
        /* Set all bytes to 0xff. */
        void erase();
        
        /* Fill in metadata with an LBA and LSI. */
        void prepare(uint32_t lba, uint32_t lsi);
        
        /* Fill in metadata with a signature. */
        void prepare(uint32_t signature);

        /* Set signature and trust number */
        void setSignatureAndTrustNumber(uint32_t signature, uint16_t trustNumber);

        /* Fill metadata with trustNumber */
        void prepareWithTrust(uint32_t signature, uint16_t trustNumber);

        uint32_t getTrustNumber();

        static uint32_t getMetadataStructSize();

    protected:
        /* Pointer to the metadata. */
        Fields * m_fields; 
    };

} /* namespace nand */

#endif /* __metadata_h__ */

/* EOF */
