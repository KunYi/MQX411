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
*   This file contains utilities used by the logical drive layer.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_media_internal.h"
#include "ddi_media.h"
#include "ddi_media_errordefs.h"
#include <stdlib.h>

/* 
** Type definations 
*/


/* Internal drive iterator structure. */
struct OpaqueDriveIterator
{
    /* Index of the next drive to return the tag of. */
    unsigned nextIndex;
};

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveCreateIterator
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_GENERAL
* Comments         :
*   This function creates drive iterator 
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveCreateIterator
(
    /* [IN/OUT] Drive iterator pointer */
    DriveIterator_t * iter
)
{ /* Body */
    /* Allocate an iterator. */
    // TODO: use _wl_mem_alloc
    DriveIterator_t newIter = (DriveIterator_t)_wl_mem_alloc(sizeof(OpaqueDriveIterator));
    // DriveIterator_t newIter = (DriveIterator_t)malloc(sizeof(OpaqueDriveIterator));
    if (newIter == NULL)
    {
        return ERROR_DDI_LDL_GENERAL;
    } /* Endif */
    
    /* Set up iterator. */
    newIter->nextIndex = 0;
    
    /* Return the new iterator to the caller. */
    *iter = newIter;
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveIteratorNext
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_ITERATOR_DONE
* Comments         :
*   This function returns the next drive
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveIteratorNext
(
    /* [IN/OUT] Drive iterator */
    DriveIterator_t iter, 

    /* [IN] Drive tag */
    DriveTag_t * tag
)
{ /* Body */
    assert(iter);
    
    /* 
    ** Return the tag for this index to the caller 
    ** and increment the iterator's index for the next time through. 
    */
    while (iter->nextIndex < MAX_LOGICAL_DRIVES)
    {
        LogicalDrive * drive = g_ldlInfo.m_drives[iter->nextIndex++];
        
        /* Skip drives that do not have a valid API table set. */
        if (drive)
        {
            *tag = drive->getTag();
            break;
        }
    } /* Endwhile */
    
    /* Check if all drives have been returned through this iterator. */
    if (iter->nextIndex >= MAX_LOGICAL_DRIVES)
    {
        return ERROR_DDI_LDL_ITERATOR_DONE;
    } /* Endif */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveIteratorDispose
* Returned Value   : void
* Comments         :
*   This function disposes drive iterator
*
*END*--------------------------------------------------------------------*/
void DriveIteratorDispose
(
/* [IN/OUT] Drive iterator pointer */
DriveIterator_t iter
)
{ /* Body */
    if (iter)
    {
        // TODO: use _wl_mem_free
        _wl_mem_free(iter);
        // free(iter);
    } /* Endif */
} /* Endbody */

/* EOF */
