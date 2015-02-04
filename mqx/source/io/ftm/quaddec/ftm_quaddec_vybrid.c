/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This file provides I/O APIs of FTM Quadrature Decoder driver for vybrid.
*
*
*END************************************************************************/



#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>

#include "vybrid.h"
#include "ftm.h"
#include "ftm_vybrid.h"
#include "ftm_quaddec.h"

extern _mqx_uint _bsp_ftm_clocks_init(uint8_t channel);

extern _mqx_uint _frd_ftm_quaddec_init (IO_FTM_QUADDEC_STRUCT_PTR, char*);
extern _mqx_uint _frd_ftm_quaddec_deinit (IO_FTM_QUADDEC_STRUCT_PTR);
extern _mqx_uint _frd_ftm_quaddec_rx
    (IO_FTM_QUADDEC_STRUCT_PTR, uint8_t*, _mqx_uint);
extern _mqx_uint _frd_ftm_quaddec_tx
    (IO_FTM_QUADDEC_STRUCT_PTR, uint8_t*, _mqx_uint);
extern _mqx_uint _frd_ftm_quaddec_ioctl
    (IO_FTM_QUADDEC_STRUCT_PTR, _mqx_uint, _mqx_uint_ptr);


/*ISR*-------------------------------------------------------------------------
*
* ISR Name : ftm_quaddec_isr
* Comments : ftm quaddec isr
*
*END*-------------------------------------------------------------------------*/
static void ftm_quaddec_isr(void *arg)
{
    IO_FTM_QUADDEC_STRUCT_PTR dev_io_struct_ptr =
                                    (IO_FTM_QUADDEC_STRUCT_PTR)arg;
    QUADDEC_IO_INFO_STRUCT_PTR dev_io_info_ptr =
                   (QUADDEC_IO_INFO_STRUCT_PTR)dev_io_struct_ptr->DEV_INFO_PTR;
    FTM_QUADDEC_CALLBACK_STRUCT_PTR cb = dev_io_info_ptr->CB_PTR;
    FTM_MemMapPtr ftm_ptr = dev_io_info_ptr->FTM_PTR;
    _mqx_uint regval = 0;

    /* read SC register */
    regval = ftm_ptr->SC;

    /* clear TOF bits */
    ftm_ptr->SC &= ~FTM_SC_TOF_MASK;

    if (regval & FTM_SC_TOF_MASK) {
        /* read QDCTRL register */
        regval = ftm_ptr->QDCTRL;

        /* increasing or decreasing */
        if (regval & FTM_QDCTRL_TOFDIR_MASK) {
            if (cb)
                cb->cb_fn(cb->arg, (void *)QUADDEC_CNT_UP);
            else
                dev_io_info_ptr->QUADDEC_EVENT = QUADDEC_CNT_UP;
        } else {
            if (cb)
                cb->cb_fn(cb->arg, (void *)QUADDEC_CNT_DOWN);
            else
                dev_io_info_ptr->QUADDEC_EVENT = QUADDEC_CNT_DOWN;
        }
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _frd_ftm_quaddec_install
* Returned Value   : MQX error code
* Comments         :
*    Install an FTM quaddecder device.
*
*END*********************************************************************/
uint32_t _frd_ftm_quaddec_install
   (
      /* [IN] A string that identifies the device for fopen */
      char*                             identifier,

      /* [IN] The I/O init data pointer */
      QUADDEC_INIT_INFO_STRUCT_PTR      init_data_ptr
   )
{ /* Body */

	return _io_ftm_quaddec_install(identifier,
      (_mqx_uint (_CODE_PTR_)(void*, char*))_frd_ftm_quaddec_init,
      (_mqx_uint (_CODE_PTR_)(void*))_frd_ftm_quaddec_deinit,
      (_mqx_int (_CODE_PTR_)(void*, char*, _mqx_int))_frd_ftm_quaddec_rx,
      (_mqx_int (_CODE_PTR_)(void*, char*, _mqx_int))_frd_ftm_quaddec_tx,
      (_mqx_int (_CODE_PTR_)(void*, _mqx_uint, _mqx_uint_ptr))
                           _frd_ftm_quaddec_ioctl,
      (void *)init_data_ptr);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _frd_ftm_quaddec_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an device.
*
*END*********************************************************************/
_mqx_uint _frd_ftm_quaddec_init
   (
      /* [IN] Initialization information for the device being opened */
      IO_FTM_QUADDEC_STRUCT_PTR     io_ftm_quaddec_struct_ptr,

      /* [IN] The rest of the name of the device opened */
      char*                         open_name_ptr

   )
{ /* Body */
   _mqx_uint ret = MQX_OK;
   QUADDEC_IO_INFO_STRUCT_PTR dev_io_info_ptr = NULL;
   QUADDEC_INIT_INFO_STRUCT_PTR dev_init_data_ptr = NULL;
   FTM_MemMapPtr ftm_ptr = NULL;
   _mqx_uint int_vector = 0;

   dev_init_data_ptr = (QUADDEC_INIT_INFO_STRUCT_PTR)
                        (io_ftm_quaddec_struct_ptr->DEV_INIT_DATA_PTR);

   /* Configure clock controller for FTM */
   ret = _bsp_ftm_clocks_init(dev_init_data_ptr->CHANNEL);
   if (MQX_OK != ret)
      return ret;

   /* Enable FTM clock */
   _bsp_ftm_io_init(dev_init_data_ptr->CHANNEL);

   /* Allocates FTM IO info structure */
   dev_io_info_ptr = (QUADDEC_IO_INFO_STRUCT_PTR)_mem_alloc_system_zero(
                                        sizeof(QUADDEC_IO_INFO_STRUCT));
   if (NULL == dev_io_info_ptr)
      return MQX_OUT_OF_MEMORY;

   _mem_set_type((void *)dev_io_info_ptr, MEM_TYPE_IO_FTM_QUADDEC_STRUCT);

   io_ftm_quaddec_struct_ptr->DEV_INFO_PTR = dev_io_info_ptr;

   dev_io_info_ptr->FTM_PTR =
             (FTM_MemMapPtr) _bsp_get_ftm_base_address (dev_init_data_ptr->CHANNEL);

   ftm_ptr = dev_io_info_ptr->FTM_PTR;

   /*write protection is disabled*/
   if ((((ftm_ptr->FMS) & FTM_FMS_WPEN_MASK)>>FTM_FMS_WPEN_SHIFT) == 1)
       ftm_ptr->MODE |= ((1) << FTM_MODE_WPDIS_SHIFT);

   /*FTM features are enabled*/
   ftm_ptr->MODE |= FTM_MODE_FTMEN_MASK;

   /* Initialize CNTIN register */
   ftm_ptr->CNTIN = ((dev_init_data_ptr->CNT_INIT) << FTM_CNTIN_INIT_SHIFT)
                    & FTM_CNTIN_INIT_MASK;

   /* Initialize MODULO register */
   ftm_ptr->MOD = ((dev_init_data_ptr->MODULO) << FTM_MOD_MOD_SHIFT)
                    & FTM_MOD_MOD_MASK;

   /*To enable input capture mode*/
   ftm_ptr->COMBINE &= ~FTM_COMBINE_COMBINE0_MASK;
   ftm_ptr->COMBINE &= ~FTM_COMBINE_COMBINE1_MASK;
   ftm_ptr->COMBINE &= ~FTM_COMBINE_DECAPEN0_MASK;
   ftm_ptr->COMBINE &= ~FTM_COMBINE_DECAPEN1_MASK;
   ftm_ptr->CONTROLS[0].CnSC &= ~FTM_CnSC_MSB_MASK;
   ftm_ptr->CONTROLS[0].CnSC &= ~FTM_CnSC_MSA_MASK;
   ftm_ptr->CONTROLS[0].CnSC |= FTM_CnSC_ELSB_MASK;
   ftm_ptr->CONTROLS[0].CnSC |= FTM_CnSC_ELSA_MASK;
   ftm_ptr->CONTROLS[1].CnSC &= ~FTM_CnSC_MSB_MASK;
   ftm_ptr->CONTROLS[1].CnSC &= ~FTM_CnSC_MSA_MASK;
   ftm_ptr->CONTROLS[1].CnSC |= FTM_CnSC_ELSB_MASK;
   ftm_ptr->CONTROLS[1].CnSC |= FTM_CnSC_ELSA_MASK;

   if (TRUE == dev_init_data_ptr->FILTER_EN) {
       /*Set filter value of CH0*/
      ftm_ptr->FILTER |= FTM_FILTER_CH0FVAL(dev_init_data_ptr->FILTER_VAL);

      /*Set filter value of CH1*/
      ftm_ptr->FILTER |= FTM_FILTER_CH1FVAL(dev_init_data_ptr->FILTER_VAL);

      /*enable phase A filter*/
      ftm_ptr->QDCTRL |=  FTM_QDCTRL_PHAFLTREN_MASK;

      /*enable phase B filter*/
      ftm_ptr->QDCTRL |=  FTM_QDCTRL_PHBFLTREN_MASK;
   }

   /*configure phase A polarity*/
   if (INVERTED_POL == dev_init_data_ptr->PHA_POL)
      ftm_ptr->QDCTRL |= FTM_QDCTRL_PHAPOL_MASK;

   /*configure phase B polarity*/
   if (INVERTED_POL == dev_init_data_ptr->PHB_POL)
      ftm_ptr->QDCTRL |= FTM_QDCTRL_PHBPOL_MASK;

   /*select encoding mode for Quaddec*/
   if (CNT_DIR_MODE == dev_init_data_ptr->QUADDEC_MODE)
      ftm_ptr->QDCTRL |= FTM_QDCTRL_QUADMODE_MASK;

   /*quadmode enabled*/
   ftm_ptr->QDCTRL |=  FTM_QDCTRL_QUADEN_MASK;

   ftm_ptr->SC &= ~FTM_SC_PS_MASK;

   /* clock source selection */
   if (NO_CLK == dev_init_data_ptr->CLK_TYPE)
      ftm_ptr->SC &= ~FTM_SC_CLKS_MASK;
   else if (SYSTEM_CLK == dev_init_data_ptr->CLK_TYPE)
      ftm_ptr->SC |= FTM_SC_CLKS(1);
   else if (FIXED_FREQ_CLK == dev_init_data_ptr->CLK_TYPE)
      ftm_ptr->SC |= FTM_SC_CLKS(2);
   else if (EXTERNAL_CLK == dev_init_data_ptr->CLK_TYPE)
      ftm_ptr->SC |= FTM_SC_CLKS(3);

   /*clear CPWMS bit of SC register*/
   ftm_ptr->SC &= ~FTM_SC_CPWMS_MASK;

   /*clear TOF bit of SC register*/
   ftm_ptr->SC &= ~FTM_SC_TOF_MASK;

   io_ftm_quaddec_struct_ptr->OLD_ISR_DATA =
    _int_get_isr_data(_bsp_get_ftm_vector(dev_init_data_ptr->CHANNEL));

   io_ftm_quaddec_struct_ptr->OLD_ISR = (void *)
    _int_get_isr(_bsp_get_ftm_vector(dev_init_data_ptr->CHANNEL));

   /*Install FTM ISR and enable FTM interrupt*/
   if (TRUE == dev_init_data_ptr->INT_EN) {
      int_vector = _bsp_get_ftm_vector(dev_init_data_ptr->CHANNEL);

      if(_int_install_isr(int_vector,
                          ftm_quaddec_isr,
                          (void *)io_ftm_quaddec_struct_ptr) == NULL) {
         printf("install FTM quaddec isr failed! \n");
         return MQX_EFAULT;
      }

      _bsp_int_init(int_vector, BSP_FTM_INT_LEVEL, 0, TRUE);
      ftm_ptr->SC |= FTM_SC_TOIE_MASK;
   }

   return MQX_OK;
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _frd_ftm_quaddec_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an device.
*
*END*********************************************************************/
_mqx_uint _frd_ftm_quaddec_deinit
   (
      IO_FTM_QUADDEC_STRUCT_PTR   io_ftm_quaddec_struct_ptr
   )
{ /* Body */
    QUADDEC_IO_INFO_STRUCT_PTR dev_io_info_ptr = NULL;
    QUADDEC_INIT_INFO_STRUCT_PTR dev_init_data_ptr = NULL;
    FTM_MemMapPtr         ftm_ptr;

    dev_init_data_ptr = (QUADDEC_INIT_INFO_STRUCT_PTR)
                            (io_ftm_quaddec_struct_ptr->DEV_INIT_DATA_PTR);

    dev_io_info_ptr = (QUADDEC_IO_INFO_STRUCT_PTR)
                            (io_ftm_quaddec_struct_ptr->DEV_INFO_PTR);

    ftm_ptr = dev_io_info_ptr->FTM_PTR;

    if ((((ftm_ptr->FMS) & FTM_FMS_WPEN_MASK)>>FTM_FMS_WPEN_SHIFT) == 1)
        ftm_ptr->MODE |= ((1) << FTM_MODE_WPDIS_SHIFT);

    ftm_ptr->SC &= ~FTM_SC_TOIE_MASK;

    ftm_ptr->QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;

    ftm_ptr->MODE &= ~FTM_MODE_FTMEN_MASK;

    _int_install_isr(_bsp_get_ftm_vector(dev_init_data_ptr->CHANNEL),
                     (INT_ISR_FPTR) io_ftm_quaddec_struct_ptr->OLD_ISR,
                     io_ftm_quaddec_struct_ptr->OLD_ISR_DATA);

    _mem_free(io_ftm_quaddec_struct_ptr->DEV_INFO_PTR);

    io_ftm_quaddec_struct_ptr->DEV_INFO_PTR = NULL;

    return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _frd_ftm_quaddec_ioctl
* Returned Value   : MQX error code.
* Comments         :
*    This function performs miscellaneous services for
*    the I/O control of device.
*
*END*********************************************************************/
_mqx_uint _frd_ftm_quaddec_ioctl
   (
      /* [IN] the address of the device specific information */
      IO_FTM_QUADDEC_STRUCT_PTR   io_ftm_quaddec_struct_ptr,

      /* [IN] The command to perform */
      _mqx_uint                   cmd,

      /* [IN] Parameters for the command */
      _mqx_uint_ptr               param_ptr
   )
{ /* Body */
   QUADDEC_IO_INFO_STRUCT_PTR dev_io_info_ptr = NULL;
   QUADDEC_INIT_INFO_STRUCT_PTR dev_init_data_ptr = NULL;
   FTM_MemMapPtr         ftm_ptr;
   _mqx_uint regval = 0;

   dev_init_data_ptr = (QUADDEC_INIT_INFO_STRUCT_PTR)
                           (io_ftm_quaddec_struct_ptr->DEV_INIT_DATA_PTR);

   dev_io_info_ptr = (QUADDEC_IO_INFO_STRUCT_PTR)
                           (io_ftm_quaddec_struct_ptr->DEV_INFO_PTR);

   ftm_ptr = dev_io_info_ptr->FTM_PTR;

   if (NULL == param_ptr)
       return MQX_INVALID_POINTER;

   switch   (cmd) {

   case     IO_IOCTL_FTM_GET_CHANNEL:
            *param_ptr = (_mqx_uint)(dev_init_data_ptr->CHANNEL);
            break;

   case     IO_IOCTL_FTM_QUADDEC_SET_CB:
            dev_io_info_ptr->CB_PTR =
                                 (FTM_QUADDEC_CALLBACK_STRUCT_PTR)(param_ptr);
            break;

   case     IO_IOCTL_FTM_QUADDEC_GET_MODE:
            if (TRUE == dev_init_data_ptr->INT_EN)
                *param_ptr = (uint32_t)QUADDEC_ASYNC_MODE;
            else
                *param_ptr = (uint32_t)QUADDEC_SYNC_MODE;
            break;

    case    IO_IOCTL_FTM_QUADDEC_GET_EVENT:
            {
                if (TRUE == dev_init_data_ptr->INT_EN) {
                    *param_ptr = (_mqx_uint)(dev_io_info_ptr->QUADDEC_EVENT);
                    _int_disable();
                    dev_io_info_ptr->QUADDEC_EVENT = (uint8_t)QUADDEC_NO_EVENT;
                    _int_enable();
                } else {
                    /*read sc register*/
                    regval = ftm_ptr->SC;

                    /*clear TOF bit*/
                    ftm_ptr->SC &= ~FTM_SC_TOF_MASK;

                    /* should guarantee the TOF bit has been cleared */
                    while (ftm_ptr->SC & FTM_SC_TOF_MASK)
                        ftm_ptr->SC &= ~FTM_SC_TOF_MASK;

                    if (regval & FTM_SC_TOF_MASK) {
                        regval = ftm_ptr->QDCTRL;

                        if (regval & FTM_QDCTRL_TOFDIR_MASK) {
                            *param_ptr = QUADDEC_CNT_UP;
                        } else {
                            *param_ptr = QUADDEC_CNT_DOWN;
                        }
                    } else {
                        *param_ptr = QUADDEC_NO_EVENT;
                    }
                }
            }
            break;

    case    IO_IOCTL_FTM_REG_DUMP:
            _bsp_dump_ftm_register(ftm_ptr);
            break;

    default:
            break;
   }

   return MQX_OK;
} /* Endbody */



/*FUNCTION****************************************************************
*
* Function Name    : _frd_ftm_quaddec_rx
* Returned Value   : number of bytes read
* Comments         :
* This driver does not provide this interface. It should not be used.
*
*END*********************************************************************/
_mqx_uint _frd_ftm_quaddec_rx
   (
      /* [IN] the address of the device specific information */
      IO_FTM_QUADDEC_STRUCT_PTR   io_ftm_quaddec_struct_ptr,

      /* [IN] The array to copy data into */
      uint8_t*                    buffer,

      /* [IN] number of bytes to read */
      _mqx_uint                   length
   )
{ /* Body */

    return MQX_OK;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _frd_ftm_quaddec_tx
* Returned Value   : number of bytes transmitted
* Comments         :
* This driver does not provide this interface. It should not be used.
*
*END*********************************************************************/
_mqx_uint _frd_ftm_quaddec_tx
   (
      /* [IN] the address of the device specific information */
      IO_FTM_QUADDEC_STRUCT_PTR   io_ftm_quaddec_struct_ptr,

      /* [IN] The array characters are to be read from */
      uint8_t*                    buffer,

      /* [IN] number of bytes to output */
      _mqx_uint                   length
   )
{ /* Body */

    return MQX_OK;
} /* Endbody */

/* EOF */

