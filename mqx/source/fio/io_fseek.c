
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the function for setting the current location
*   in a file.
*
*
*END************************************************************************/
#include "mqx_cnfg.h"
#if MQX_USE_IO_OLD

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief This function sets the current file position.
 * 
 * \param[in] file_ptr The stream to use.
 * \param[in] offset   The offset for the seek.
 * \param[in] mode     Mode to determine where to start the seek from.
 * 
 * \return MQX_OK
 * \return IO_ERROR (Failure.) 
 */ 
_mqx_int _io_fseek
   (
      MQX_FILE_PTR file_ptr,
      _file_offset  offset,
      _mqx_uint  mode
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR dev_ptr;
   _mqx_int             result;

#if MQX_CHECK_ERRORS
    if (file_ptr == NULL) {
        return (IO_ERROR);
    } /* Endif */
#endif

    switch (mode) {
    case IO_SEEK_SET:
        if (offset < 0) {
            return IO_ERROR;
        }
        file_ptr->LOCATION = offset;
        break;
    case IO_SEEK_CUR:
        /* Adjust possibly fake location before relative seek */
        if (file_ptr->HAVE_UNGOT_CHARACTER && (file_ptr->LOCATION > 0)) {
            offset--;
        }
        if (offset < 0 && (file_ptr->LOCATION < (_file_size) (-offset))) {
            return IO_ERROR;
        }
        file_ptr->LOCATION += offset;
        break;
    case IO_SEEK_END:
        if (offset < 0 && (file_ptr->SIZE < (_file_size) (-offset))) {
            return IO_ERROR;
        }
        file_ptr->LOCATION = file_ptr->SIZE + offset;
        break;
#if MQX_CHECK_ERRORS
    default:
        return (IO_ERROR);
#endif
    } /* Endswitch */

    /* Empty ungetc buffer */
    if (file_ptr->HAVE_UNGOT_CHARACTER) {
        file_ptr->HAVE_UNGOT_CHARACTER = FALSE;
    }

    /* Clear EOF flag */
    file_ptr->FLAGS &= ~((_mqx_uint) IO_FLAG_AT_EOF);

    dev_ptr = file_ptr->DEV_PTR;
    if (dev_ptr->IO_IOCTL != NULL) {
        result = (*dev_ptr->IO_IOCTL)(file_ptr, IO_IOCTL_SEEK, NULL);
        if (result != IO_ERROR_INVALID_IOCTL_CMD)
            return result;
    } /* Endif */

    return (MQX_OK);

} /* Endbody */

#endif // MQX_USE_IO_OLD
