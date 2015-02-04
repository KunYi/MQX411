#if !defined(__ddi_nand_h__)
#define __ddi_nand_h__
/**HEADER********************************************************************
* Copyright (c) SigmaTel, Inc. All rights reserved.
* 
* SigmaTel, Inc.
* Proprietary & Confidential
* 
* This source code and the algorithms implemented therein constitute
* confidential information and may comprise trade secrets of SigmaTel, Inc.
* or its associates, and any use thereof is subject to the terms and
* conditions of the Confidential Disclosure Agreement pursual to which this
* source code was originally received.
*****************************************************************************
* $FileName: ddi_nand.h$
* $Version : 3.8.0.1$
* $Date    : Aug-9-2012$
*
* Comments:
*
*   This file contains declarations of public interfaces to
*   the NAND driver.
*   
**END*************************************************************************/

/*  
** Constants 
*/

enum
{
    /* DriveSetInfo() key for control over system drive recovery. [bool] 
    **
    ** Use DriveSetInfo() to modify this property of system drives. Setting it 
    ** to true will enable automatic recovery of system drives when an error is 
    ** encountered during a page read. The drive will be completely erased and 
    ** rewritten from the master copy. Setting this property to false will disable 
    ** the recovery functionality. 
    */
    kDriveInfoNandSystemDriveRecoveryEnabled = (('n'<<24)|('s'<<16)|('r'<<8)|('e')) /* 'nsre' */
};

/*  
** Prototypes 
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
*******************************************************************************
** Shutdown the NAND HAL and GPMI. 
*******************************************************************************
*/
void ddi_nand_hal_shutdown(void);

#ifdef __cplusplus
} /*  extern "C" */
#endif /*  __cplusplus */

#endif /* __ddi_nand_h__ */

/* EOF */
