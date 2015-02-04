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
*   Memory management and profiling for WL Module.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "mem_management.h"
#include "mem_management_internal.h"
#include "wl_common.h"

#if NANDWL_USE_MM_LEAK_DETECTION

#define MM_MEM_HEAD_SIGNATURE       (('H' << 24) | ('T' << 16) | ('D' << 8) | ('S'))
#define MM_MEM_TAIL_SIGNATURE       (('E' << 24) | ('D' << 16) | ('T' << 8) | ('H'))


MEM_ALLOCATION_TABLE g_mem_allocation_table = { 0 };

static _mqx_uint mm_check_buf_valid(MEM_ENTRY_HEAD_PTR buf_head);
static _mqx_uint mm_dump_buf_invalid(MEM_ENTRY_HEAD_PTR buf_head);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mm_alloc
* Returned Value   : a malloc-ed buffer
* Comments         :
*   Allocate a buffer with a given size
*
*END*--------------------------------------------------------------------*/
void * mm_alloc
(
    /* [IN] size of requested buffer  */
    _mem_size request_size, 
    
    /* [IN] file name which requests buffer */
    const char* file_name, 
    
    /* [IN] Location in file_name */
    int file_loc,
        
    /* [IN] Should zero buffer after allocated */
    bool isZero, 
        
    /* [IN] Is this buffer requested from new operator */
    bool is_from_operator
) 
{ /* Body */
    uint32_t real_alloc_size = request_size + sizeof(MEM_ENTRY_HEAD)
            + sizeof(MEM_ENTRY_TAIL);
    MEM_ENTRY_HEAD_PTR buf_head;
    MEM_ENTRY_TAIL_PTR buf_tail;
    uint32_t idx;

    buf_head = (MEM_ENTRY_HEAD_PTR) _mm_internal_alloc(real_alloc_size);
    if (NULL == buf_head)
    {
        return NULL;
    } /* Endif */

    buf_tail = (MEM_ENTRY_TAIL_PTR)((uint8_t *) buf_head + request_size
            + sizeof(MEM_ENTRY_HEAD));

    buf_head->file_loc = file_loc;

    buf_head->file_name = file_name;

    buf_head->requested_size = request_size;
    buf_head->is_from_operator = is_from_operator;
    buf_head->buf_head_sign = MM_MEM_HEAD_SIGNATURE;
    buf_tail->buf_tail_sign = MM_MEM_TAIL_SIGNATURE;


    if (isZero)
    {
        _mm_internal_zero(((uint8_t *) buf_head) + sizeof(MEM_ENTRY_HEAD),
                request_size);
    }/* Endif */

    idx = g_mem_allocation_table.nextSaveIdx;
    /*
    if (NULL == g_mem_allocation_table.buf_ptr[idx])
    {
        while (1);

    }
    */
    

    g_mem_allocation_table.buf_ptr[idx] = buf_head;
    if (is_from_operator)
    {
        g_mem_allocation_table.totalNewSize += request_size;
    }
    else
    {
        g_mem_allocation_table.totalAllocSize += request_size;
    }/* Endif */

    while ((idx < MEM_MAX_SAVE_BUF_PTR) && (NULL
            != g_mem_allocation_table.buf_ptr[idx]))
    {
        /* search toward end of buf */
        idx++;
    } /* Endwhile */

    if (idx >= MEM_MAX_SAVE_BUF_PTR)
    {
        /* search from beginning */
        idx = 0;
        while ((idx < g_mem_allocation_table.nextSaveIdx) && (NULL
                != g_mem_allocation_table.buf_ptr[idx]))
        {   idx++;}
    }/* Endif */
    if (NULL != g_mem_allocation_table.buf_ptr[idx])
    {
        printf("memory allocation table is exceed (%d buf(s))",
                MEM_MAX_SAVE_BUF_PTR);
        while (1)
        {
        
        }
    }/* Endif */

    g_mem_allocation_table.nextSaveIdx = idx;

    return (((uint8_t *) buf_head) + sizeof(MEM_ENTRY_HEAD));
} /* Enbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mm_check_buf_valid
* Returned Value   : MQX_OK or MQX_INVALID_POINTER
* Comments         :
*       Check the given buffer whether it is valid or not 
*
*END*--------------------------------------------------------------------*/
_mqx_uint mm_check_buf_valid
(
    /* [IN] pointer to head of allocated buffer */
    MEM_ENTRY_HEAD_PTR buf_head
) 
{ /* Body */
    MEM_ENTRY_TAIL_PTR buf_tail = NULL;
    if (buf_head->buf_head_sign == MM_MEM_HEAD_SIGNATURE)
    {
        buf_tail = (MEM_ENTRY_TAIL_PTR)(((uint8_t *) buf_head)
                + sizeof(MEM_ENTRY_HEAD) + buf_head->requested_size);
        if (buf_tail->buf_tail_sign == MM_MEM_TAIL_SIGNATURE)
        {
            return (MQX_OK);
        }/* Endif */
    }/* Endif */

    return MQX_INVALID_POINTER;
}/* Enbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mm_dump_buf_invalid
* Returned Value   : MQX_OK or MQX_INVALID_POINTER
* Comments         :
*       Check the given buffer whether it is valid or not 
*
*END*--------------------------------------------------------------------*/
_mqx_uint mm_dump_buf_invalid
(
    /* [IN] pointer to head of allocated buffer */
    MEM_ENTRY_HEAD_PTR buf_head
) 
{ /* Body */
    uint32_t i;
    void *buf = ((uint8_t *) buf_head) + sizeof(MEM_ENTRY_HEAD);

    /* Memory overwritten */
    printf("\nBUFFER (0x%0x) with head(0x%0x) DUMP ", buf, buf_head);
    printf("\n");
    printf(
            "\n ******************* ERROR when free a buffer(0x%0x) with head(0x%0x) which is allocated at file %s:%d *******************",
            buf, buf_head, buf_head->file_name, buf_head->file_loc);
    _ASM_NOP();
    _ASM_NOP();
    for (i = 0; i < buf_head->requested_size; i++)
    {
        printf("%02x ", *( (uint8_t *)buf +i) );
    }/* Endfor */

    _ASM_NOP();
    _ASM_NOP();
    _ASM_NOP();
    _ASM_NOP();
    _ASM_NOP();
    _ASM_NOP();
    
    return MQX_OK;
}/* Enbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mm_free
* Returned Value   : error or MQX_OK
* Comments         : free the alocated buffer
*
*END*--------------------------------------------------------------------*/
_mqx_uint mm_free
(
    /* [IN] pointer to head of allocated buffer */
    void * buf
) 
{ /* Body */
    uint32_t ret = MQX_INVALID_POINTER;
    MEM_ENTRY_HEAD_PTR buf_head = (MEM_ENTRY_HEAD_PTR)((uint8_t *) buf
            - sizeof(MEM_ENTRY_HEAD));

    uint32_t idx;

    ret = mm_check_buf_valid(buf_head);

    if (ret != MQX_OK)
    {
        mm_dump_buf_invalid(buf_head);

    }
    else
    {

        if (buf_head->is_from_operator)
        {
            g_mem_allocation_table.totalDeleteSize += buf_head->requested_size;
        }
        else
        {
            g_mem_allocation_table.totalFreedSize += buf_head->requested_size;
        } /* Endif */

        for (idx = 0; idx < MEM_MAX_SAVE_BUF_PTR; idx++)
        {
            if (g_mem_allocation_table.buf_ptr[idx] == buf_head)
            {
                g_mem_allocation_table.buf_ptr[idx] = NULL;
                break;
            }/* Endif */
        }/* Endfor */

        if (idx == MEM_MAX_SAVE_BUF_PTR)
        {

            printf(
                    "\n ****Trying to free Buffer, which not resist in Allocation Table *** ");
            mm_dump_buf_invalid(buf_head);

        }/* Endif */

        _mm_internal_free(buf_head);

    }/* Endif */
    /*
     if (ret != MQX_OK) {
     while (1) {};
     }
     */

    return (ret);
}/* Enbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mm_display
* Returned Value   : N/A
* Comments         : Display the memory statistics
*
*END*--------------------------------------------------------------------*/
void mm_display() 
{ /* Body */

    printf("\nTotal memory allocated: %lu byte(s)",
            g_mem_allocation_table.totalAllocSize);
    printf("\nTotal memory free: %lu byte(s)",
            g_mem_allocation_table.totalFreedSize);

    printf("\nTotal memory allocated from *New* operator: %lu byte(s)",
            g_mem_allocation_table.totalNewSize);
    printf("\nTotal memory free from *Delete* operator: %lu byte(s)",
            g_mem_allocation_table.totalDeleteSize);    

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mm_display_unfree
* Returned Value   : N/A
* Comments         : Display the un-free memory 
*
*END*--------------------------------------------------------------------*/
void mm_display_unfree() 
{/* Body */
    uint32_t i;
    MEM_ENTRY_HEAD_PTR buf_head;

    for (i = 0; i < MEM_MAX_SAVE_BUF_PTR; i++)
    {
        buf_head = g_mem_allocation_table.buf_ptr[i];
        if (NULL != buf_head )
        {
            if (MQX_OK != mm_check_buf_valid(buf_head))
            {
                mm_dump_buf_invalid(buf_head);
            }/* Endif */

            /*          uint32_t j;           */
            printf("\n Buffer which is allocated at file %s:%d with request size %d",
                    buf_head->file_name,
                    buf_head->file_loc,
                    buf_head->requested_size
            );
        }/* Endif */
    }
    /* Endfor */
}/* Endbody */



void * operator new (size_t size, const char *fname, int floc)
{
    return mm_alloc(size, fname, floc, FALSE, TRUE);
}

void * operator new[] (size_t size, const char *fname, int floc)
{
    // return  ::operator new[] (size);
    return mm_alloc(size, fname, floc, FALSE, TRUE);
}

void operator delete (void *p)
{
    mm_free(p);    
}

void operator delete[] (void *p)
{
    mm_free(p);    
}

#endif
/* EOF */
