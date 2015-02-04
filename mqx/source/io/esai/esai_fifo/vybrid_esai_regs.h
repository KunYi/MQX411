#ifndef __faraday_esai_h__
#define __faraday_esai_h__
/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This file contains registers address definition for faraday ESAI module
*
*
*END************************************************************************/

/* ----------------------------------------------------------------------------
   -- ESAI
   ---------------------------------------------------------------------------- */

/*! \addtogroup ESAI_Peripheral ESAI */
/*! \{ */

#ifndef __ASM__
/* ----------------------------------------------------------------------------
   -- ESAI - Register accessor macros
   ---------------------------------------------------------------------------- */

/*! \addtogroup ESAI_Register_Accessor_Macros ESAI - Register accessor macros */
/*! \{ */

#define ESAI_TFCR_TWA_WL_32      (0x0 << 16)
#define ESAI_TFCR_TWA_WL_28      (0x1 << 16)
#define ESAI_TFCR_TWA_WL_24      (0x2 << 16)
#define ESAI_TFCR_TWA_WL_20      (0x3 << 16)
#define ESAI_TFCR_TWA_WL_16      (0x4 << 16)
#define ESAI_TFCR_TWA_WL_12      (0x5 << 16)
#define ESAI_TFCR_TWA_WL_8       (0x6 << 16)
#define ESAI_TFCR_TWA_WL_4       (0x7 << 16)

#define ESAI_RFCR_RWA_WL_32      (0x0 << 16)
#define ESAI_RFCR_RWA_WL_28      (0x1 << 16)
#define ESAI_RFCR_RWA_WL_24      (0x2 << 16)
#define ESAI_RFCR_RWA_WL_20      (0x3 << 16)
#define ESAI_RFCR_RWA_WL_16      (0x4 << 16)
#define ESAI_RFCR_RWA_WL_12      (0x5 << 16)
#define ESAI_RFCR_RWA_WL_8       (0x6 << 16)
#define ESAI_RFCR_RWA_WL_4       (0x7 << 16)

#define ESAI_TCR_TMOD_NORMAL	 (0x00 << 8)
#define ESAI_TCR_TMOD_ONDEMAND	 (0x01 << 8)
#define ESAI_TCR_TMOD_NETWORK	 (0x01 << 8)
#define ESAI_TCR_TMOD_RESERVED   (0x02 << 8)
#define ESAI_TCR_TMOD_AC97	     (0x03 << 8)

#define ESAI_TCR_TSWS_STL8_WDL8	    (0x00 << 10)
#define ESAI_TCR_TSWS_STL12_WDL8	(0x04 << 10)
#define ESAI_TCR_TSWS_STL12_WDL12	(0x01 << 10)
#define ESAI_TCR_TSWS_STL16_WDL8	(0x08 << 10)
#define ESAI_TCR_TSWS_STL16_WDL12	(0x05 << 10)
#define ESAI_TCR_TSWS_STL16_WDL16	(0x02 << 10)
#define ESAI_TCR_TSWS_STL20_WDL8	(0x0c << 10)
#define ESAI_TCR_TSWS_STL20_WDL12	(0x09 << 10)
#define ESAI_TCR_TSWS_STL20_WDL16	(0x06 << 10)
#define ESAI_TCR_TSWS_STL20_WDL20	(0x03 << 10)
#define ESAI_TCR_TSWS_STL24_WDL8	(0x10 << 10)
#define ESAI_TCR_TSWS_STL24_WDL12	(0x0d << 10)
#define ESAI_TCR_TSWS_STL24_WDL16	(0x0a << 10)
#define ESAI_TCR_TSWS_STL24_WDL20	(0x07 << 10)
#define ESAI_TCR_TSWS_STL24_WDL24	(0x1e << 10)
#define ESAI_TCR_TSWS_STL32_WDL8	(0x18 << 10)
#define ESAI_TCR_TSWS_STL32_WDL12	(0x15 << 10)
#define ESAI_TCR_TSWS_STL32_WDL16	(0x12 << 10)
#define ESAI_TCR_TSWS_STL32_WDL20	(0x0f << 10)
#define ESAI_TCR_TSWS_STL32_WDL24	(0x1f << 10)

#define ESAI_RCR_RMOD_NORMAL     (0x00 << 8)
#define ESAI_RCR_RMOD_ONDEMAND   (0x01 << 8)
#define ESAI_RCR_RMOD_NETWORK    (0x01 << 8)
#define ESAI_RCR_RMOD_RESERVED   (0x02 << 8)
#define ESAI_TCR_TMOD_AC97       (0x03 << 8)


/*! \} */ /* end of group ESAI_Register_Masks */

#endif /* #ifndef __ASM__ */

/*! \} */ /* end of group ESAI */
#endif
