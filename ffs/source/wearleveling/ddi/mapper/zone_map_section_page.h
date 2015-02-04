#if !defined(__zonemapsectionpage_h__)
#define __zonemapsectionpage_h__
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
*   This file contains a class to wrap a section of the zone map
*
*
*END************************************************************************/

#include "ddi_page.h"

/* 
** Class definations 
*/

namespace nand
{

/* STMP codes */
/* Metadata STMP code value for zone map pages. */
#define LBA_STRING_PAGE1            (('L'<<24)|('B'<<16)|('A'<<8)|'M')

/* Metadata STMP code value for phys map pages. */
#define PHYS_STRING_PAGE1           (('E'<<24)|('X'<<16)|('M'<<8)|'A')

/* Map section header constants */
/* Signature shared by all map types, used to identify a valid map header. */
const uint32_t kNandMapHeaderSignature = (('x'<<24)|('m'<<16)|('a'<<8)|('p')); /* 'xmap' */

/* Unique signature used for the zone map. */
const uint32_t kNandZoneMapSignature = (('z'<<24)|('o'<<16)|('n'<<8)|('e')); /* 'zone' */

/* Unique signature used for the phy map. */
const uint32_t kNandPhysMapSignature = (('p'<<24)|('h'<<16)|('y'<<8)|('s')); /* 'phys' */

/* 
** Current version of the map header.
**
** The low byte is the minor version, all higher bytes form the major version.
**
** Version history:
** - Version 1.0 was the original map section format that had a very basic two-word "header"
** with no signature.
** - Version 2.0 is the first version with a real header. 
*/
const uint32_t kNandMapSectionHeaderVersion = 0x00000200;

/* Header for zone and phy maps when stored on the NAND. */
struct NandMapSectionHeader
{
    /* Common signature for all map types. */
    uint32_t signature;  
    
    /* 'zone' or 'phys' */
    uint32_t mapType;  
    
    /* Version of this header structure, see #kNandMapSectionHeaderVersion. */
    uint32_t version;    
    
    /* Size in bytes of each entry. */
    uint32_t entrySize;  
    
    /* Total number of entries in this section. */
    uint32_t entryCount; 
    
    /* LBA for the first entry in this section. */
    uint32_t startLba;   
};

/* Type for the map section header. */
typedef struct NandMapSectionHeader NandMapSectionHeader_t;

/*
** Represents one section of a zone map.
**
** This class works for both the virtual to physical map (zone map) as well as the physical
** allocation map (phy map). Be sure to set the map type with the #setMapType() method after
** you create and instance of this class.
**
** You can use a ZoneMapSectionPage to either read or write pages from a map block. When reading,
** this class is useful to help parse and validate the section page header. For writing,
** the writeSection() method can fill in the header and help compute sizes and offsets. Before you
** can do any reading or writing, remember to specify the buffers to use either explicitly or
** by calling #allocateBuffers().
**
** To use the class for reading a section page, you only need to set the map type with a call
** to setMapType(). If you wish to use getSectionNumber(), then you will also need to set
** the map entry size in bytes by calling setEntrySize(). Once the object is configured, use
** the superclass's #read() method to actually read the page. After the read completes, you can
** access the section header with the #getHeader() and related getX methods. You should call
** #validateHeader() to ensure that the section that was just read is valid.
**
** To write a section page by way of the writeSection() method, call setEntrySize(),
** setMetadataSignature(), and setMapType() after instatiating the object. writeSection() is
** intended to be used in a loop, though it can just as easily be used for a single write.
*/
class ZoneMapSectionPage : public Page
{
public:
    /* Construction and destruction */
    /* Default constructor. */
    ZoneMapSectionPage();

    /* Constructor taking a page address. */
    ZoneMapSectionPage(const PageAddress & addr);
    
    /* Assignment operator. */
    ZoneMapSectionPage & operator = (const PageAddress & addr);
    
    /* Configuration */
    /* Specify the entry  size in bytes. */
    void setEntrySize(unsigned entrySize) { m_entrySize = entrySize; }

    /* Set the signature value for the section page metadata. */
    void setMetadataSignature(uint32_t sig) { m_metadataSignature = sig; }

    /* Set the map type signature. */
    void setMapType(uint32_t theType) { m_mapType = theType; }

    /* Computes the number of entries that will fit in a section page. */
    unsigned getMaxEntriesPerPage() { return (getDataSize() - sizeof(NandMapSectionHeader_t)) / m_entrySize; }
    
    /* Section information */
    /* Get the header structure for the page. */
    NandMapSectionHeader_t * getHeader() { return m_header; }
    
    /* Get a pointer to the section entry data. */
    uint8_t * getEntries() { return m_sectionData; }
    
    /* Returns the section number for this section. */
    uint32_t getSectionNumber();
    
    /* Returns the starting LBA. */
    uint32_t getStartLba() { return m_header->startLba; }
    
    /* Returns the entry count. */
    uint32_t getEntryCount() { return m_header->entryCount; }
    
    /* Validates the header contents. */
    bool validateHeader();
    
    /* 
    ** Write one page of the map block. 
    ** 
    ** This method intended to be used in a loop, though it can just as easily be used for a
    ** single write of a map section. 
    */
    RtStatus_t writeSection(
    uint32_t startingEntryNum,
    uint32_t remainingEntries,
    uint8_t * startingEntry,
    uint32_t * actualNumEntriesWritten,
    bool trustFlag = false,
    uint16_t trustNumber = 0);

protected:
    /* The header of the section. */
    NandMapSectionHeader_t * m_header;  
    
    /* Pointer to the start of the section data in the page buffer. */
    uint8_t * m_sectionData;    
    
    /* Size in bytes of each entry. */
    unsigned m_entrySize;   
    
    /* The signature set in the metadata of section pages. */
    uint32_t m_metadataSignature;   
    
    /* Map type signature as used in the section page header. */
    uint32_t m_mapType;     

    /* Specify the buffers to use for reading and writing. */
    virtual void buffersDidChange();
};

} /* namespace nand */

#endif /* __zonemapsectionpage_h__ */

/* EOF */
