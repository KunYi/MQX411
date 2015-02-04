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
*   This file contains initialisation code for the buffer manager.
*
*
*END************************************************************************/

#include "media_buffer_manager.h"
#include "sectordef.h"
#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <timer.h>
#include "media_buffer_manager_internal.h"
#include "ddi_media_errordefs.h"

/* 
** Constants 
*/

/* Number of sector buffers to pre-allocate. */
#define SECTOR_BUFFER_COUNT                 (0)

/* Number of auxiliary buffers to pre-allocate. */
#define AUX_BUFFER_COUNT                    (0)

/* Wait 20 times before force release all buffers */
#define MEDIA_BUFFER_MAX_WAIT_DEINIT        (20)

/* 
** Variables 
*/

#if SECTOR_BUFFER_COUNT > 0
#pragma alignvar(32)
/* Pre-allocated sector buffers. */
static SECTOR_BUFFER s_sectorBuffers[SECTOR_BUFFER_COUNT][NOMINAL_DATA_SECTOR_ALLOC_SIZE];
#endif /* SECTOR_BUFFER_COUNT > 0 */

#if AUX_BUFFER_COUNT > 0
#pragma alignvar(32)
/* Pre-allocated auxiliary buffers. */
static SECTOR_BUFFER s_auxBuffers[AUX_BUFFER_COUNT][NOMINAL_AUXILIARY_SECTOR_ALLOC_SIZE];
#endif /* AUX_BUFFER_COUNT > 0 */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_init
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS The buffer manager was initialised without error.
*   - ERROR_OS_KERNEL_TX_CALLER_ERROR This function was called from an
*   invalid execution context. It must only be called from a thread.
* Comments         :
*   Initialise the buffer manager.
*   Aside from preparing the buffer manager's global context, this function
*   will one auxiliary buffer to start with. This initial buffer is specified
*   as non-cacheable, non-bufferable (NCNB) and has the appropriate flag set.
*   It is not specifically placed in fast memory.
*
*END*--------------------------------------------------------------------*/
RtStatus_t media_buffer_init(void)
{ /* Body */
    _mqx_uint txStatus;
#if (SECTOR_BUFFER_COUNT > 0) || (AUX_BUFFER_COUNT > 0)
    int i;
#endif /* (SECTOR_BUFFER_COUNT > 0) || (AUX_BUFFER_COUNT > 0) */
    MUTEX_ATTR_STRUCT mutex_init;
    void * error_ptr;

    /* Only initialise once. */
    if (g_mediaBufferManagerContext.isInited)
    {
        return SUCCESS;
    } /* Endif */
        
    /* Initialize a mutex mutex attributes */
    assert(_mutatr_init(&mutex_init) == MQX_EOK);
    
    txStatus = _mutex_init(&g_mediaBufferManagerContext.mutex,&mutex_init);

    if (txStatus != MQX_OK)
    {
        return os_err_code_to_wl(txStatus, kNandDontCare );
    } /* Endif */
    
    /* Create the timeout timer. */
    assert(_timer_test(&error_ptr) == MQX_OK);
    g_mediaBufferManagerContext.timeoutTimer = 0;
    
    /* Init the rest. */
    g_mediaBufferManagerContext.nextTimeout = NO_NEXT_TIMEOUT;
    g_mediaBufferManagerContext.bufferToDispose = NO_NEXT_TIMEOUT;
    g_mediaBufferManagerContext.isInited = TRUE;
    g_mediaBufferManagerContext.slotCount = 0;
    g_mediaBufferManagerContext.bufferCount = 0;
    g_mediaBufferManagerContext.freeCount = 0;
    
#if SECTOR_BUFFER_COUNT > 0
    /* Add some buffers. */
    for (i=0; i < SECTOR_BUFFER_COUNT; ++i)
    {
        media_buffer_add(kMediaBufferType_Sector, kMediaBufferFlag_None, s_sectorBuffers[i]);
    } /* Endfor */
#endif /* SECTOR_BUFFER_COUNT > 0 */
    
#if AUX_BUFFER_COUNT > 0
    for (i=0; i < AUX_BUFFER_COUNT; ++i)
    {
        media_buffer_add(kMediaBufferType_Auxiliary, kMediaBufferFlag_None, s_auxBuffers[i]);
    } /* Endfor */
#endif /* AUX_BUFFER_COUNT > 0 */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_deinit
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function de-initializes media buffer
*
*END*--------------------------------------------------------------------*/
RtStatus_t media_buffer_deinit(void)
{
   // TODO: review need, 'case original do not have such deinit func 
   // TODO: switch code bellow to event & wait 
   uint32_t count = 0;
   
   /* We should wait till all mem buffer be freed */
   while ((media_buffer_count_all_temporary() != 0) 
         &&(count++ < MEDIA_BUFFER_MAX_WAIT_DEINIT))
   { 
   #ifdef MEDIA_BUFFER_DEBUG
      WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, "media buffer deinit: Waiting till all buf are freed \n");
   #endif      
      /* Delay 10ms in each waiting loop */
      _time_delay(TEMPORARY_BUFFER_TIMEOUT_MS*2);
   }
   
   /* Clean timer */
    _mutex_lock(&g_mediaBufferManagerContext.mutex);

    _timer_cancel(g_mediaBufferManagerContext.timeoutTimer);

    _mutex_unlock(&g_mediaBufferManagerContext.mutex);

    media_buffer_force_dispose_temporary();


    /* Close mutex */
    _mutex_destroy(&g_mediaBufferManagerContext.mutex);

    g_mediaBufferManagerContext.isInited = FALSE;  
    g_mediaBufferManagerContext.slotCount = 0;
    g_mediaBufferManagerContext.bufferCount = 0;
    g_mediaBufferManagerContext.freeCount = 0; 

    #ifdef MEDIA_BUFFER_DEBUG
        WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, "media buffer deinit: OK  \n");
    #endif  
    return SUCCESS;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_report_stats
* Returned Value   : void
* Comments         :
*   This function shows report stats of media buffer
*
*END*--------------------------------------------------------------------*/
void media_buffer_report_stats(void)
{ /* Body */
    uint8_t i;
    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " Media Buffer statistics report \n");
    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " ============================== \n");

    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " Statistics for permanent buffers \n");
    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " totalAllocs: %d, concurrentAllocs: %d, maxConcurrentAllocs: %d, newAllocs:%d, realignedAllocs:%d\n",
    g_mediaBufferManagerContext.permStats.totalAllocs, 
    g_mediaBufferManagerContext.permStats.concurrentAllocs, 
    g_mediaBufferManagerContext.permStats.maxConcurrentAllocs, 
    g_mediaBufferManagerContext.permStats.newAllocs, 
    g_mediaBufferManagerContext.permStats.realignedAllocs);
    
    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " Statistics for temporary buffers \n");
    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " totalAllocs: %d, concurrentAllocs: %d, maxConcurrentAllocs: %d, newAllocs:%d, realignedAllocs:%d\n",
    g_mediaBufferManagerContext.tempStats.totalAllocs, 
    g_mediaBufferManagerContext.tempStats.concurrentAllocs, 
    g_mediaBufferManagerContext.tempStats.maxConcurrentAllocs, 
    g_mediaBufferManagerContext.tempStats.newAllocs, 
    g_mediaBufferManagerContext.tempStats.realignedAllocs);

    for (i = 0; i < kMediaBufferType_Count; i++)
    {
        WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " Statistics for permanent buffers for buffer type %d \n", i);
        WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " totalAllocs: %d, concurrentAllocs: %d, maxConcurrentAllocs: %d, newAllocs:%d, realignedAllocs:%d\n",
        g_mediaBufferManagerContext.permTypeStats[i].totalAllocs, 
        g_mediaBufferManagerContext.permTypeStats[i].concurrentAllocs, 
        g_mediaBufferManagerContext.permTypeStats[i].maxConcurrentAllocs, 
        g_mediaBufferManagerContext.permTypeStats[i].newAllocs, 
        g_mediaBufferManagerContext.permTypeStats[i].realignedAllocs);
        
        WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " Statistics for temporary buffers for buffer type %d \n", i);
        WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, " totalAllocs: %d, concurrentAllocs: %d, maxConcurrentAllocs: %d, newAllocs:%d, realignedAllocs:%d\n",
        g_mediaBufferManagerContext.tempTypeStats[i].totalAllocs, 
        g_mediaBufferManagerContext.tempTypeStats[i].concurrentAllocs, 
        g_mediaBufferManagerContext.tempTypeStats[i].maxConcurrentAllocs, 
        g_mediaBufferManagerContext.tempTypeStats[i].newAllocs, 
        g_mediaBufferManagerContext.tempTypeStats[i].realignedAllocs);
    } /* Endfor */
    
}

/* EOF */
