#ifndef __faraday_asrc_h__
#define __faraday_asrc_h__
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
*   This file contains registers address definition for imx53 ASRC module
*
*
*END************************************************************************/

/* ----------------------------------------------------------------------------
   -- ASRC
   ---------------------------------------------------------------------------- */

/*! \addtogroup ASRC_Peripheral ASRC */
/*! \{ */

#ifndef __ASM__

#define ASRC_ASRCTR_ASRE(x)     (1<<(1+(x)))
#define ASRC_ASRCTR_IDR(x)      (1<<(13 + ((x)<<1)))
#define ASRC_ASRCTR_USR(x)      (1<<(14 + ((x)<<1)))
#define ASRC_ASRCTR_ATS(x)      (1<<(20 + (x)))

#define ASRC_ASRCFG_INIRQ(x)    (1<<(21+(x)))

#define ASRC_ASRCNCR_ANC_MASK(x)  (0xF<<((x)<<2))
#define ASRC_ASRCNCR_ANC_SHIFT(x) ((x)<<2)

#define ASRC_ASRCFG_PREMOD_MASK(x)   (0x3<<(6+((x)<<2)))
#define ASRC_ASRCFG_PREMOD_SHIFT(x)  (6+((x)<<2))
#define ASRC_ASRCFG_POSTMOD_MASK(x)  (0x3<<(8+((x)<<2)))
#define ASRC_ASRCFG_POSTMOD_SHIFT(x) (8+((x)<<2))

#define ASRC_ASRCSR_AICS_MASK(x)     (0xF<<((x)<<2))
#define ASRC_ASRCSR_AICS_SHIFT(x)    ((x)<<2)
#define ASRC_ASRCSR_AOCS_MASK(x)     (0xF<<(12 + ((x)<<2)))
#define ASRC_ASRCSR_AOCS_SHIFT(x)    (12 + ((x)<<2))



#define ASRC_PAIRS_NUM           (3)
#define ASRC_CHANNELS_NUM        (10)


#endif /* #ifndef __ASM__ */

/*! \} */ /* end of group ASRC */
#endif
