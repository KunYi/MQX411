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

#include "wl_common.h"
#include "ddi_metadata.h"
#include <string.h>

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getLba
* Returned Value   : The logical block address.
* Comments         :
*   Get the logical block address.
*
*END*--------------------------------------------------------------------*/
uint32_t Metadata::getLba() const
{ /* Body */
    assert(m_fields);
    // return m_fields->lba0 | (m_fields->lba1 << 16);
    return m_fields->u_0.s_0.lba0 | (m_fields->u_1.s_2.lba1<< 16);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getLsi
* Returned Value   : The logical sector index.
* Comments         :
*   Get the logical sector index.
*
*END*--------------------------------------------------------------------*/
uint16_t Metadata::getLsi() const
{ /* Body */
    assert(m_fields);
    // return m_fields->lsi;
    return m_fields->u_0.s_0.lsi;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getBlockNumber
* Returned Value   : The erase block number.
* Comments         :
*   Get the erase block number.
*
*END*--------------------------------------------------------------------*/
uint8_t Metadata::getBlockNumber() const
{ /* Body */
    assert(m_fields);
    return m_fields->blockNumber;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getSignature
* Returned Value   : The four byte signature.
* Comments         :
*   Get the four byte signature.
*
*END*--------------------------------------------------------------------*/
uint32_t Metadata::getSignature() const
{ /* Body */
    assert(m_fields);
    // return ((m_fields->tag0 << 24)
    // | (m_fields->tag1 << 16)
    // | (m_fields->tag2 << 8)
    // | (m_fields->tag3));
    return ((m_fields->u_0.s_1.tag0 << 24)
    | (m_fields->u_0.s_1.tag1 << 16)
    | (m_fields->u_0.s_1.tag2 << 8)
    | (m_fields->u_0.s_1.tag3));
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isFlagSet
* Returned Value   : Returns true if the flag is set.
* Comments         :
*   Metadata flags are set when the bit is 0.
*
*END*--------------------------------------------------------------------*/
bool Metadata::isFlagSet
(
    /* [IN] Flag mask */
    uint8_t flagMask
) const
{ /* Body */
    assert(m_fields);
    // return ((m_fields->flags & flagMask) == 0);
    return ((m_fields->u_1.s_2.flags & flagMask) == 0);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : Returns true if the block status field is non-ff.
* Comments         :
*   Returns true if the block status field is non-ff.
*
*END*--------------------------------------------------------------------*/
bool Metadata::isMarkedBad() const
{ /* Body */
    assert(m_fields);
    return m_fields->blockStatus != 0xff;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isErased
* Returned Value   : Returns true if the metadata is all ff.
* Comments         :
*   Returns true if the metadata is all ff.
*
*END*--------------------------------------------------------------------*/
bool Metadata::isErased() const
{ /* Body */
    assert(m_fields);
    // return ((uint32_t *)m_fields)[0] == 0xffffffff
    // && ((uint32_t *)m_fields)[1] == 0xffffffff
    // && m_fields->flags == 0xff;
    return ((uint32_t *)m_fields)[0] == 0xffffffff
    && ((uint32_t *)m_fields)[1] == 0xffffffff
    && m_fields->u_1.s_2.flags == 0xff;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setLba
* Returned Value   : void
* Comments         :
*   Sets the logical block address field.
*
*END*--------------------------------------------------------------------*/
void Metadata::setLba
(
    /* [IN] Logical block address */
    uint32_t lba
)
{ /* Body */
    assert(m_fields);
    // m_fields->lba0 = lba & 0xffff;
    m_fields->u_0.s_0.lba0 = lba & 0xffff;
    // m_fields->lba1 = (lba >> 16) & 0xffff;    
    m_fields->u_1.s_2.lba1 = (lba >> 16) & 0xffff;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setLsi
* Returned Value   : void
* Comments         :
*   Sets the logical sector index field.
*
*END*--------------------------------------------------------------------*/
void Metadata::setLsi
(
    /* [IN] Logical sector index */
    uint16_t lsi
)
{ /* Body */
    assert(m_fields);
    // m_fields->lsi = lsi;
    m_fields->u_0.s_0.lsi = lsi;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setBlockNumber
* Returned Value   : void
* Comments         :
*   Sets the erase block number.
*
*END*--------------------------------------------------------------------*/  
void Metadata::setBlockNumber
(   
    /* [IN] Block number */
    uint8_t blockNumber
)
{ /* Body */
    assert(m_fields);
    m_fields->blockNumber = blockNumber;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setSignature
* Returned Value   : void
* Comments         :
*   Sets the signature field.
*
*END*--------------------------------------------------------------------*/
void Metadata::setSignature
(
    /* [IN] Signature */
    uint32_t signature
)
{ /* Body */
    assert(m_fields);
    // m_fields->tag3 = (signature & 0xff);
    // m_fields->tag2 = (signature >> 8) & 0xff;
    // m_fields->tag1 = (signature >> 16) & 0xff;
    // m_fields->tag0 = (signature >> 24) & 0xff;
    m_fields->u_0.s_1.tag3 = (signature & 0xff);
    m_fields->u_0.s_1.tag2 = (signature >> 8) & 0xff;
    m_fields->u_0.s_1.tag1 = (signature >> 16) & 0xff;
    m_fields->u_0.s_1.tag0 = (signature >> 24) & 0xff;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setFlag
* Returned Value   : void
* Comments         :
*   Set flag by setting the bit to 0.
*
*END*--------------------------------------------------------------------*/
void Metadata::setFlag
(
    /* [IN] Flag */
    uint8_t flagMask
)
{ /* Body */ 
    assert(m_fields);
    // m_fields->flags &= ~flagMask;
    m_fields->u_1.s_2.flags &= ~flagMask;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : clearFlag
* Returned Value   : void
* Comments         :
*   Clear flag by setting its bit to 1.
*
*END*--------------------------------------------------------------------*/
void Metadata::clearFlag
(
    /* [IN] Flag */
    uint8_t flagMask
)
{ /* Body */
    assert(m_fields);
    // m_fields->flags |= flagMask;
    m_fields->u_1.s_2.flags |= flagMask;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : markBad
* Returned Value   : void
* Comments         :
*   Sets the block status byte to 0.
*
*END*--------------------------------------------------------------------*/
void Metadata::markBad()
{ /* Body */
    assert(m_fields);
    m_fields->blockStatus = 0;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : erase
* Returned Value   : void
* Comments         :
*   Set all bytes to 0xff.
*
*END*--------------------------------------------------------------------*/
void Metadata::erase()
{ /* Body */
    assert(m_fields);
    _wl_mem_set(m_fields, 0xff, sizeof(*m_fields));
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : prepare
* Returned Value   : void
* Comments         :
*   Fill in metadata with an LBA and LSI.
*
*END*--------------------------------------------------------------------*/
void Metadata::prepare
(
    /* [IN] Logical block address */
    uint32_t lba, 
    
    /* [IN] Logical sector index */
    uint32_t lsi
)
{ /* Body */
    erase();
    setLba(lba);
    setLsi(lsi);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : prepare
* Returned Value   : void
* Comments         :
*   Fill in metadata with a signature.
*
*END*--------------------------------------------------------------------*/
void Metadata::prepare
(
    /* [IN] Signature */
    uint32_t signature
)
{ /* Body */
    erase();
    setSignature(signature);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setSignatureAndTrustNumber
* Returned Value   : void
* Comments         :
*   Set signature and trust number
*
*END*--------------------------------------------------------------------*/
void Metadata::setSignatureAndTrustNumber
(
    /* [IN] Signature */
    uint32_t signature, 
        
    /* [IN] Trust number */
    uint16_t trustNumber
) 
{ /* Body */ 
    assert(m_fields);
    /* Set signature */
    // m_fields->tag3 = (signature & 0xff);
    // m_fields->tag2 = (signature >> 8) & 0xff;
    // m_fields->tag1 = (signature >> 16) & 0xff;
    // m_fields->tag0 = (signature >> 24) & 0xff;
    m_fields->u_0.s_1.tag3 = (signature & 0xff);
    m_fields->u_0.s_1.tag2 = (signature >> 8) & 0xff;
    m_fields->u_0.s_1.tag1 = (signature >> 16) & 0xff;
    m_fields->u_0.s_1.tag0 = (signature >> 24) & 0xff;
    
    /* Set trust number */
    // m_fields->tag5 = (trustNumber & 0xFF);
    // m_fields->tag4 = (trustNumber >> 8) & 0xFF;
    m_fields->u_1.s_3.tag5 = (trustNumber & 0xFF);
    m_fields->u_1.s_3.tag4 = (trustNumber >> 8) & 0xFF;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : prepareWithTrust
* Returned Value   : void
* Comments         :
*   Fill metadata with trustNumber
*
*END*--------------------------------------------------------------------*/
void Metadata::prepareWithTrust
(
    /* [IN] Signature */
    uint32_t signature, 
            
    /* [IN] Trust number */
    uint16_t trustNumber
) 
{ /* Body */
    erase();
    /* Set signature and trustNumber */
    setSignatureAndTrustNumber(signature, trustNumber);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getTrustNumber
* Returned Value   : Trust number
* Comments         :
*   Get trust number from metadata
*
*END*--------------------------------------------------------------------*/
uint32_t Metadata::getTrustNumber() 
{ /* Body */
    assert(m_fields);
    // return ((m_fields->tag4 << 8)|(m_fields->tag5));
    return ((m_fields->u_1.s_3.tag4 << 8)|(m_fields->u_1.s_3.tag5));
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getMetadataStructSize
* Returned Value   : Size of metadata struct
* Comments         :
*   Get size of metadata struct
*
*END*--------------------------------------------------------------------*/
uint32_t Metadata::getMetadataStructSize() 
{ /* Body */
    return sizeof(Fields); 
} /* Endbody */

/* EOF */
