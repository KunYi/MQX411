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
*   This file contains low level functions for the MAX7310 device driver
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include <mutex.h>
#include <io_expander_prv.h>
#include <max7310_prv.h>



uint32_t _max7310_init (IOEXP_MAX7310_INIT_STRUCT_PTR,
    void **);
uint32_t _max7310_open (IO_EXPANDER_DEVICE_STRUCT_PTR,
    void **, char *);
uint32_t _max7310_close (IO_EXPANDER_DEVICE_STRUCT_PTR,
    void *);
uint32_t _max7310_deinit (IO_EXPANDER_DEVICE_STRUCT_PTR);
int32_t _max7310_read (IO_EXPANDER_DEVICE_STRUCT_PTR,
    void *, uint8_t *, uint32_t);
int32_t _max7310_write (IO_EXPANDER_DEVICE_STRUCT_PTR,
    void *, uint8_t *, uint32_t);
int32_t _max7310_ioctl (IO_EXPANDER_DEVICE_STRUCT_PTR,
    void *, uint32_t, uint32_t *);


#define INVALID_PIN_NO      0xFF
#define INVALID_VAL         0xFF
#define INVALID_DIR         0xFF

#define MAX7310_DIR_IN      1
#define MAX7310_DIR_OUT     0
#define MAX7310_VAL_HIGH    1
#define MAX7310_VAL_LOW     0

#define MAX7310_REG_INPUT_PORT          0x00
#define MAX7310_REG_OUTPUT_PORT         0x01
#define MAX7310_REG_POLARITY            0x02
#define MAX7310_REG_CONFIGURATION       0x03
#define MAX7310_REG_TIMEOUT             0x04

//#define MAX7310_ENABLE_DEBUG
#ifdef MAX7310_ENABLE_DEBUG
#define MAX7310_DEBUG  printf
#else
#define MAX7310_DEBUG(...)
#endif

/*FUNCTION****************************************************************
*
* Function Name    : max7310_WriteRegister
* Returned Value   : MQX error code.
* Comments         :
*    This function writes MAX7310 register's value via I2C
*
*END*********************************************************************/
static uint32_t max7310_WriteRegister
(
    /*[IN] The address of MAX7310 private info structure*/
    MAX7310_INFO_STRUCT_PTR io_info_ptr,

    /*[IN] The register's address that written to*/
    uint8_t regNumber,

    /*[IN] The new register value*/
    uint8_t regValue
)
{

    uint8_t i2cData[2];
    int32_t i2c_device_address = (int32_t)io_info_ptr->INIT.I2C_address;
    uint32_t ret_val =0;
    uint32_t length = 2;

    /*just write one register, not using the auto-increment*/
    i2cData[0] = regNumber;
    i2cData[1] = regValue;

    /*open i2c controller*/
    io_info_ptr->I2C_FILE = fopen (io_info_ptr->INIT.I2C_control, NULL);

    if (io_info_ptr->I2C_FILE == NULL) {
        MAX7310_DEBUG("MAX7310 init I2C fail, %s\n",
            io_info_ptr->INIT.I2C_control);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    /* Set I2C into Master mode */
    if (0 != ioctl (io_info_ptr->I2C_FILE, IO_IOCTL_I2C_SET_MASTER_MODE, NULL)){
        fclose(io_info_ptr->I2C_FILE);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    _time_delay(1);

    /* Set the destination address */
    if(0 != ioctl (io_info_ptr->I2C_FILE,
        IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &i2c_device_address))
    {
        ret_val = 1;
    }

    /* Write 2 bytes of data: the desired register and then the data */
    if(fwrite (&i2cData, 1, length, io_info_ptr->I2C_FILE) != length)
    {
        if(!ret_val) ret_val = 2;
    }

    fflush (io_info_ptr->I2C_FILE);

    /* Send out stop */
    if(0 != ioctl (io_info_ptr->I2C_FILE, IO_IOCTL_I2C_STOP, NULL))
    {
        if(!ret_val) ret_val = 3;
    }

    fclose(io_info_ptr->I2C_FILE);

    return ret_val;
}

/*FUNCTION****************************************************************
*
* Function Name    : max7310_ReadRegister
* Returned Value   : MQX error code.
* Comments         :
*    This function read a MAX7310 register's value via I2C
*
*END*********************************************************************/
static uint32_t max7310_ReadRegister
(
    /*[IN] The address of MAX7310 private info structure*/
    MAX7310_INFO_STRUCT_PTR io_info_ptr,

    /*[IN] The register's address that written to*/
    uint8_t regNumber,

    /*[OUT] Return the register's value*/
    uint8_t *pRegValue
)
{

    uint8_t i2cData[1] = {0};
    int32_t i2c_device_address = (int32_t)io_info_ptr->INIT.I2C_address;
    uint32_t ret_val =0;
    uint32_t length = 1;

    //   Start condition
    //   Device address with the R/W bit cleared to indicate write
    //   Send one bytes for the 8 bit register address
    //   Stop Condition followed by start condition (or a single restart condition)
    //   Device address with the R/W bit set to indicate read
    //   Read one bytes from the addressed register
    //   Stop condition

    /*just read one register, not using the auto-increment*/
    i2cData[0] = regNumber;

    /*open i2c controller*/
    io_info_ptr->I2C_FILE = fopen (io_info_ptr->INIT.I2C_control, NULL);

    if (io_info_ptr->I2C_FILE == NULL) {
        MAX7310_DEBUG("MAX7310 init I2C fail, %s\n",
            io_info_ptr->INIT.I2C_control);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    /* Set I2C into Master mode */
    if (0 != ioctl (io_info_ptr->I2C_FILE, IO_IOCTL_I2C_SET_MASTER_MODE, NULL)){
        fclose(io_info_ptr->I2C_FILE);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    _time_delay(1);

    /* Set the destination address */
    if(0 != ioctl (io_info_ptr->I2C_FILE,
        IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &i2c_device_address))
    {
        ret_val = 1;
        goto err_cache;
    }

    /* Write 1 byte of data: the desired register and then the data */
    if(fwrite (&i2cData, 1, 1, io_info_ptr->I2C_FILE) != length)
    {
        if(!ret_val) ret_val = 2;
        goto err_cache;
    }

    fflush (io_info_ptr->I2C_FILE);

    if (I2C_OK != ioctl (io_info_ptr->I2C_FILE, IO_IOCTL_I2C_STOP, NULL))
    {
        MAX7310_DEBUG("MAX7310 Read Reg ERROR during I2C stop\n");
    }

    /* Pioneer CD S10.5 and MAX7310 use same I2C bus.For compatible
       with CD S10.5, I2C stop and I2C start time differece should > 1ms */
    _time_delay(1);

    /* Set the destination address */
    if(0 != ioctl (io_info_ptr->I2C_FILE,
        IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &i2c_device_address))
    {
        ret_val = 1;
        goto err_cache;
    }

    ioctl (io_info_ptr->I2C_FILE, IO_IOCTL_I2C_SET_RX_REQUEST,&length);

    if(fread (i2cData, 1, length, io_info_ptr->I2C_FILE) != length){
        MAX7310_DEBUG("MAX7310 Read Reg I2C fread failed :%x,%d\n",
            *pRegValue, ret_val);
        ret_val = 4;
        goto err_cache;
    }
    *pRegValue = i2cData[0];

    fflush (io_info_ptr->I2C_FILE);

    if (I2C_OK != ioctl (io_info_ptr->I2C_FILE, IO_IOCTL_I2C_STOP, NULL))
    {
        MAX7310_DEBUG("MAX7310 Read Reg ERROR during I2C stop\n");
    }

err_cache:

    fclose(io_info_ptr->I2C_FILE);

    return ret_val;
}

/*FUNCTION****************************************************************
*
* Function Name    : max7310_set_dir
* Returned Value   : MQX error code.
* Comments         :
*    This function sets the direction of one specified pin.
*
*END*********************************************************************/
static uint32_t max7310_set_dir
(
    /*[IN] The address of MAX7310 private info structure*/
    MAX7310_INFO_STRUCT_PTR io_info_ptr,

    /*[IN] The pin number that we apply direction to*/
    uint8_t pin,

    /*[IN] The direction, MAX7310_DIR_OUT or MAX7310_DIR_IN*/
    uint8_t dir
)
{
    uint8_t val = 0;
    uint32_t ret = 0;
    _mutex_lock(&io_info_ptr->DEV_MUTEX);
    ret = max7310_ReadRegister(io_info_ptr, MAX7310_REG_CONFIGURATION, &val);
    if (ret != 0) {
        MAX7310_DEBUG("MAX7310 set dir, read register fails\n");
		_mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }

    if (dir == MAX7310_DIR_OUT) {

        val &= ~(1 << pin);

    } else if (dir == MAX7310_DIR_IN) {

        val |= (1 << pin);;

    } else {

        MAX7310_DEBUG("MAX7310 set dir, invalid dir %d\n", dir);
        _mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }

    ret = max7310_WriteRegister(io_info_ptr, MAX7310_REG_CONFIGURATION, val);
    if (ret != 0) {
        MAX7310_DEBUG("MAX7310 set dir, write register fails\n");
        _mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }
    _mutex_unlock(&io_info_ptr->DEV_MUTEX);
    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : max7310_get_dir
* Returned Value   : MQX error code.
* Comments         :
*    This function gets the direction of one specified pin.
*
*END*********************************************************************/
static uint32_t max7310_get_dir
(
    /*[IN] The address of MAX7310 private info structure*/
    MAX7310_INFO_STRUCT_PTR io_info_ptr,

    /*[IN] The pin number that we read direction for*/
    uint8_t pin,

    /*[OUT] The direction, MAX7310_DIR_OUT or MAX7310_DIR_IN*/
    uint8_t *dir
)
{
    uint8_t val = 0;
    uint32_t ret = 0;
    _mutex_lock(&io_info_ptr->DEV_MUTEX);
    ret = max7310_ReadRegister(io_info_ptr, MAX7310_REG_CONFIGURATION, &val);
    if (ret != 0) {
        MAX7310_DEBUG("MAX7310 set dir, read register fails\n");
		_mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }

    val = val >> pin;
    val = val & 1;

    if (val == 1)
        *dir = MAX7310_DIR_IN;
    else
        *dir = MAX7310_DIR_OUT;
    _mutex_unlock(&io_info_ptr->DEV_MUTEX);
    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : max7310_get_val
* Returned Value   : MQX error code.
* Comments         :
*    This function gets the logic level value of one specified pin.
*
*END*********************************************************************/
static uint32_t max7310_get_val
(
    /*[IN] The address of MAX7310 private info structure*/
    MAX7310_INFO_STRUCT_PTR io_info_ptr,

    /*[IN] The pin number that we get logic level for*/
    uint8_t pin,

    /*[OUT] The input logic level, MAX7310_VAL_HIGH or MAX7310_VAL_LOW*/
    uint8_t *input_val
)
{
    uint8_t val = 0;
    uint32_t ret = 0;
    _mutex_lock(&io_info_ptr->DEV_MUTEX);
    ret = max7310_ReadRegister(io_info_ptr, MAX7310_REG_INPUT_PORT, &val);
    if (ret != 0) {
        MAX7310_DEBUG("MAX7310 get input val, read register fails\n");
        _mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }

    val = val >> pin;
    val = val & 1;

    if (val == 1)
        *input_val = MAX7310_VAL_HIGH;
    else
        *input_val = MAX7310_VAL_LOW;
    _mutex_unlock(&io_info_ptr->DEV_MUTEX);
    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : max7310_set_val
* Returned Value   : MQX error code.
* Comments         :
*    This function sets the logic level value of one specified pin.
*
*END*********************************************************************/
static uint32_t max7310_set_val
(
    /*[IN] The address of MAX7310 private info structure*/
    MAX7310_INFO_STRUCT_PTR io_info_ptr,

    /*[IN] The pin number that we set logic level to*/
    uint8_t pin,

    /*[IN] The output logic level*/
    uint8_t output_val
)
{
    uint8_t val = 0;
    uint32_t ret = 0;
    _mutex_lock(&io_info_ptr->DEV_MUTEX);
    ret = max7310_ReadRegister(io_info_ptr, MAX7310_REG_OUTPUT_PORT, &val);
    if (ret != 0) {
        MAX7310_DEBUG("MAX7310 set output val, read register fails\n");
        _mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }

    if (output_val == MAX7310_VAL_LOW) {

        val &= ~(1 << pin);

    } else if (output_val == MAX7310_VAL_HIGH) {

        val |= (1 << pin);;

    } else {

        MAX7310_DEBUG("MAX7310 set output val, invalid val %d\n", output_val);
        _mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }

    ret = max7310_WriteRegister(io_info_ptr, MAX7310_REG_OUTPUT_PORT, val);
    if (ret != 0) {
        MAX7310_DEBUG("MAX7310 set output val, write register fails\n");
        _mutex_unlock(&io_info_ptr->DEV_MUTEX);
        return MQX_EFAULT;
    }
    _mutex_unlock(&io_info_ptr->DEV_MUTEX);
    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_install
* Returned Value   : MQX error code
* Comments         :
*    Install an max7310 device.
*
*END*********************************************************************/
uint32_t _max7310_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *           identifier,

      /* [IN] The I/O init data void * */
      IOEXP_MAX7310_INIT_STRUCT_CPTR            init_data_ptr
   )
{ /* Body */

	return _io_expander_install(identifier,
      (uint32_t (*)(void *, void **))_max7310_init,
      (uint32_t (*)(void *, void **, char *))_max7310_open,
      (uint32_t (*)(void *, void *))_max7310_close,
      (uint32_t (*)(void *))_max7310_deinit,
      (int32_t (*)(void *, void *, char *, int32_t))_max7310_read,
      (int32_t (*)(void *, void *, char *, int32_t))_max7310_write,
      (int32_t (*)(void *, void *, uint32_t, uint32_t *))_max7310_ioctl,
      (void *)init_data_ptr);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an max7310 device.
*
*END*********************************************************************/
uint32_t _max7310_init
   (
      /* [IN] Initialization information for the device being opened */
      IOEXP_MAX7310_INIT_STRUCT_PTR   io_init_ptr,

      /* [OUT] Address to store device specific information */
      void **                   io_info_ptr_ptr

   )
{ /* Body */
    MAX7310_INFO_STRUCT_PTR           io_info_ptr;

    MAX7310_DEBUG("MAX7310 INIT\n");

    if (NULL == io_init_ptr || NULL == io_info_ptr_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    io_info_ptr = (MAX7310_INFO_STRUCT_PTR)_mem_alloc_system_zero (
        (uint32_t)sizeof (MAX7310_INFO_STRUCT));
    if (NULL == io_info_ptr)
    {
        MAX7310_DEBUG("MAX7310 init alloc mem fail\n");
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (io_info_ptr, MEM_TYPE_IO_EXP_MAX7310_INFO_STRUCT);

    _mutex_init(&io_info_ptr->DEV_MUTEX, NULL);

    *io_info_ptr_ptr            = io_info_ptr;
    io_info_ptr->INIT           = *io_init_ptr;
    io_info_ptr->PIN_BITMAP     = 0;

    /* Set MAX7310 Input not Invert */
    _mutex_lock(&io_info_ptr->DEV_MUTEX);
    max7310_WriteRegister(io_info_ptr,
                          MAX7310_REG_POLARITY,
                          0x00);
    _mutex_unlock(&io_info_ptr->DEV_MUTEX);

    MAX7310_DEBUG("MAX7310 END\n");
    return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_open
* Returned Value   : MQX error code
* Comments         :
*    This function opens an max7310 device.
*
*END*********************************************************************/
uint32_t _max7310_open
   (
      /* [IN] the initialization information for the device being opened */
      IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr,

      /* [IN] The access handler for this operation */
      void **                     access_data_ptr,

      /* [IN] The rest of the name of the device opened */
      char *                      open_name_ptr

   )
{ /* Body */
    MAX7310_INFO_STRUCT_PTR         io_info_ptr;
    MAX7310_ACCESS_DESC_STRUCT_PTR  access_desc_ptr;

    MAX7310_DEBUG("MAX7310 OPEN\n");

    if (NULL == access_data_ptr || NULL == expander_io_dev_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    io_info_ptr = (MAX7310_INFO_STRUCT_PTR)expander_io_dev_ptr->DEV_INFO_PTR;

    if (NULL == io_info_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    /*allocate a access descriptor for this file descriptor*/
    access_desc_ptr = (MAX7310_ACCESS_DESC_STRUCT_PTR)_mem_alloc_system_zero(
        (uint32_t)sizeof (MAX7310_ACCESS_DESC_STRUCT));
    if (access_desc_ptr == NULL) {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (access_desc_ptr, MEM_TYPE_IO_EXPANDER_DEVICE_STRUCT);

    access_desc_ptr->identity   = MAX7310_MAGIC_WORD;
    access_desc_ptr->direction  = INVALID_DIR;
    access_desc_ptr->pin        = INVALID_PIN_NO;
    access_desc_ptr->val        = INVALID_VAL;

    _mutex_init(&access_desc_ptr->access_mutex, NULL);

    *access_data_ptr = access_desc_ptr;

    MAX7310_DEBUG("MAX7310 OPEN END\n");

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_close
* Returned Value   : MQX error code
* Comments         :
*    This function closes an max7310 device.
*
*END*********************************************************************/
uint32_t _max7310_close
   (
      /* [IN] the initialization information for the device being opened */
      IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr,

      /* [IN] The access handler for this operation */
      void *                       access_data

   )
{
    MAX7310_INFO_STRUCT_PTR         io_info_ptr;
    MAX7310_ACCESS_DESC_STRUCT_PTR  access_ptr;

    MAX7310_DEBUG("MAX7310 CLOSE\n");

    if (NULL == access_data || NULL == expander_io_dev_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    io_info_ptr = (MAX7310_INFO_STRUCT_PTR)expander_io_dev_ptr->DEV_INFO_PTR;
    if (NULL == io_info_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    access_ptr = (MAX7310_ACCESS_DESC_STRUCT_PTR)access_data;
    if (NULL == access_ptr || MAX7310_MAGIC_WORD != access_ptr->identity)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    _mutex_lock(&access_ptr->access_mutex);
    _mutex_lock(&io_info_ptr->DEV_MUTEX);

    if (access_ptr->pin < MAX7310_PINS_NUM) {
        io_info_ptr->PIN_BITMAP &= ~(1 << (access_ptr->pin));
        access_ptr->pin = INVALID_PIN_NO;
    }

    _mutex_unlock(&io_info_ptr->DEV_MUTEX);
    _mutex_unlock(&access_ptr->access_mutex);

    _mutex_destroy(&access_ptr->access_mutex);
    _mem_free(access_ptr);

    MAX7310_DEBUG("MAX7310 CLOSE END\n");

    return MQX_OK;

}


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an max7310 device.
*
*END*********************************************************************/

uint32_t _max7310_deinit
   (
      /* [IN] the initialization information for the device being opened */
      IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr
   )
{ /* Body */
    MAX7310_INFO_STRUCT_PTR         io_info_ptr;

    MAX7310_DEBUG("MAX7310 DEINIT\n");

    if (NULL == expander_io_dev_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    io_info_ptr = (MAX7310_INFO_STRUCT_PTR)expander_io_dev_ptr->DEV_INFO_PTR;

    _mutex_destroy(&io_info_ptr->DEV_MUTEX);

    /* Free info struct */
    _mem_free (expander_io_dev_ptr->DEV_INFO_PTR);
    expander_io_dev_ptr->DEV_INFO_PTR = NULL;

    MAX7310_DEBUG("MAX7310 DEINIT END\n");
    return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_ioctl
* Returned Value   : MQX error code.
* Comments         :
*    This function performs miscellaneous services for
*    the max7310 I/O device.
*
*END*********************************************************************/

int32_t _max7310_ioctl
   (
      /* [IN] the initialization information for the device being opened */
      IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr,

      /* [IN] The access handler for this operation */
      void *                     access_data,

      /* [IN] The command to perform */
      uint32_t                     cmd,

      /* [IN] Parameters for the command */
      uint32_t *                 param_ptr
   )
{ /* Body */
    int32_t                          result = MQX_OK;

    MAX7310_INFO_STRUCT_PTR         io_info_ptr;
    MAX7310_ACCESS_DESC_STRUCT_PTR  access_ptr;
    uint32_t val;
    uint8_t dev_val;

    MAX7310_DEBUG("MAX7310 IOCTL, cmd %u\n", cmd);

    if (NULL == access_data || NULL == expander_io_dev_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    io_info_ptr = (MAX7310_INFO_STRUCT_PTR)expander_io_dev_ptr->DEV_INFO_PTR;
    if (NULL == io_info_ptr) {
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    access_ptr = (MAX7310_ACCESS_DESC_STRUCT_PTR)access_data;
    if (NULL == access_ptr || MAX7310_MAGIC_WORD != access_ptr->identity)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    _mutex_lock(&access_ptr->access_mutex);

    switch (cmd)
    {
        case IO_IOCTL_IOEXP_GET_INPUT_REG:
            _mutex_lock(&io_info_ptr->DEV_MUTEX);
            dev_val = 0;
            result = max7310_ReadRegister(io_info_ptr,
                MAX7310_REG_INPUT_PORT, &dev_val);
            if (result == MQX_OK) {
                *param_ptr = dev_val;
                _mutex_unlock(&io_info_ptr->DEV_MUTEX);
            } else {
                _mutex_unlock(&io_info_ptr->DEV_MUTEX);
                _mutex_unlock(&access_ptr->access_mutex);
                return MQX_IO_OPERATION_NOT_AVAILABLE;
            }
            break;
        case IO_IOCTL_IOEXP_SET_PIN_NO:
            val = (uint32_t)param_ptr; //pin number from 1 to 8
            _mutex_lock(&io_info_ptr->DEV_MUTEX);

            if (val < MAX7310_PINS_NUM &&
                0 == (io_info_ptr->PIN_BITMAP & (1<<val))){

                /*set the new pin number*/
                if (INVALID_PIN_NO != access_ptr->pin) {
                    io_info_ptr->PIN_BITMAP &= ~(1 << access_ptr->pin);
                }
                access_ptr->pin = val;
                io_info_ptr->PIN_BITMAP |= (1 << access_ptr->pin);
                _mutex_unlock(&io_info_ptr->DEV_MUTEX);

            } else if (val < MAX7310_PINS_NUM && val == access_ptr->pin) {
                _mutex_unlock(&io_info_ptr->DEV_MUTEX);
                _mutex_unlock(&access_ptr->access_mutex);
                return MQX_OK;
            } else {
                _mutex_unlock(&io_info_ptr->DEV_MUTEX);
                _mutex_unlock(&access_ptr->access_mutex);
                return MQX_IO_OPERATION_NOT_AVAILABLE;
            }
            break;
        case IO_IOCTL_IOEXP_CLEAR_PIN_NO:

            _mutex_lock(&io_info_ptr->DEV_MUTEX);

            /*clear current pin set*/
            if (INVALID_PIN_NO != access_ptr->pin) {
                io_info_ptr->PIN_BITMAP &= ~(1 << access_ptr->pin);
            }
            access_ptr->pin = INVALID_PIN_NO;
            _mutex_unlock(&io_info_ptr->DEV_MUTEX);

            break;
        case IO_IOCTL_IOEXP_SET_PIN_DIR_IN:
            if (access_ptr->pin == INVALID_PIN_NO) {
                _mutex_unlock(&access_ptr->access_mutex);
                return MQX_IO_OPERATION_NOT_AVAILABLE;
            }
            /*call the dir setting func*/
            result = max7310_set_dir(io_info_ptr, access_ptr->pin,
                                                        MAX7310_DIR_IN);
            if (result == MQX_OK)
                access_ptr->direction = MAX7310_DIR_IN;

#ifdef MAX7310_ENABLE_DEBUG
            dev_val = 0xFF;
            result = max7310_get_dir(io_info_ptr, access_ptr->pin, &dev_val);
            MAX7310_DEBUG("SET PIN IN, check val = %u\n", dev_val);
#endif
            break;

        case IO_IOCTL_IOEXP_SET_PIN_DIR_OUT:
            if (access_ptr->pin == INVALID_PIN_NO) {
                _mutex_unlock(&access_ptr->access_mutex);
                return MQX_IO_OPERATION_NOT_AVAILABLE;
            }

            /*call the dir setting func*/
            result = max7310_set_dir(io_info_ptr, access_ptr->pin,
                                                        MAX7310_DIR_OUT);
            if (result == MQX_OK)
                access_ptr->direction = MAX7310_DIR_OUT;

#ifdef MAX7310_ENABLE_DEBUG
            dev_val = 0xFF;
            result = max7310_get_dir(io_info_ptr, access_ptr->pin, &dev_val);
            MAX7310_DEBUG("SET PIN OUT, check val = %u\n", dev_val);
#endif
            break;

        case IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH:
            if (access_ptr->pin == INVALID_PIN_NO) {
                _mutex_unlock(&access_ptr->access_mutex);
                return MQX_IO_OPERATION_NOT_AVAILABLE;
            }
            /*check the direction*/
            result = max7310_get_dir(io_info_ptr, access_ptr->pin, &dev_val);
            if (result != MQX_OK)
                break;

            if (dev_val != MAX7310_DIR_OUT) {
                MAX7310_DEBUG("MAX7310 IOCTL IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH, "
                    "err dir = %u\n", dev_val);
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }

            /*call the val setting func*/
            result = max7310_set_val(io_info_ptr, access_ptr->pin,
                                                        MAX7310_VAL_HIGH);
            if (result == MQX_OK)
                access_ptr->val = MAX7310_VAL_HIGH;

#ifdef MAX7310_ENABLE_DEBUG
            val = 0xFF;
            result = max7310_get_val(io_info_ptr, access_ptr->pin, &dev_val);
            MAX7310_DEBUG("SET PIN HIGH, check val = %u\n", dev_val);
#endif
            break;
        case IO_IOCTL_IOEXP_SET_PIN_VAL_LOW:
            if (access_ptr->pin == INVALID_PIN_NO) {
                _mutex_unlock(&access_ptr->access_mutex);
                return MQX_IO_OPERATION_NOT_AVAILABLE;
            }

            /*check the direction*/
            result = max7310_get_dir(io_info_ptr, access_ptr->pin, &dev_val);
            if (result != MQX_OK)
                break;

            if (dev_val != MAX7310_DIR_OUT) {
                MAX7310_DEBUG("MAX7310 IOCTL IO_IOCTL_IOEXP_SET_PIN_VAL_LOW, "
                    "err dir = %u\n", dev_val);
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }

            /*call the val setting func*/
            result = max7310_set_val(io_info_ptr, access_ptr->pin,
                                                        MAX7310_VAL_LOW);
            if (result == MQX_OK)
                access_ptr->val = MAX7310_VAL_LOW;

#ifdef MAX7310_ENABLE_DEBUG
            val = 0xFF;
            result = max7310_get_val(io_info_ptr, access_ptr->pin, &dev_val);
            MAX7310_DEBUG("SET PIN LOW, check val = %u\n", dev_val);
#endif
            break;
        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }
    _mutex_unlock(&access_ptr->access_mutex);
    return result;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_read
* Returned Value   : number of bytes read
* Comments         :
*   Read the logic level of current pin to the input user's buffer. The logic level value is
*   one byte size. If some error happens in the reading, return value is less than 0.
*
*END*********************************************************************/

int32_t _max7310_read
   (
      /* [IN] the address of the device specific information */
      IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr,

      /* [IN] The access handler for this operation */
      void *                       access_data,

      /* [IN] The array to copy data into */
      uint8_t *                       buffer,

      /* [IN] number of bytes to read */
      uint32_t                         length
   )
{ /* Body */
    uint32_t                         result = MQX_OK;
    MAX7310_INFO_STRUCT_PTR         io_info_ptr;
    MAX7310_ACCESS_DESC_STRUCT_PTR  access_ptr;
    uint8_t val;

    if (NULL == access_data || NULL == expander_io_dev_ptr
         || length < 1 || NULL == buffer)
        return -1;

    io_info_ptr = (MAX7310_INFO_STRUCT_PTR)expander_io_dev_ptr->DEV_INFO_PTR;
    if (NULL == io_info_ptr)
        return -1;

    access_ptr = (MAX7310_ACCESS_DESC_STRUCT_PTR)access_data;
    if (NULL == access_ptr || MAX7310_MAGIC_WORD != access_ptr->identity)
        return -1;

    _mutex_lock(&access_ptr->access_mutex);
    if (access_ptr->pin == INVALID_PIN_NO) {
        _mutex_unlock(&access_ptr->access_mutex);
        return -1;
    }

    /*check the direction*/
    result = max7310_get_dir(io_info_ptr, access_ptr->pin, &val);
    if (result != MQX_OK) {
        _mutex_unlock(&access_ptr->access_mutex);
        return -1;
    }

    if (val != MAX7310_DIR_IN) {
        result = MQX_IO_OPERATION_NOT_AVAILABLE;
        _mutex_unlock(&access_ptr->access_mutex);
        return -1;
    }
    /*call the read func*/
    result = max7310_get_val(io_info_ptr, access_ptr->pin, &val);
    if (result != MQX_OK) {
        _mutex_unlock(&access_ptr->access_mutex);
        return -2;
    }

    buffer[0] = val;
    access_ptr->val = val;

    _mutex_unlock(&access_ptr->access_mutex);
    return 1;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _max7310_write
* Returned Value   : number of bytes transmitted
* Comments         :
*   Write the logic level value in the first byte of the user's buffer to the current pin.
*   If some error happens during the call, return value is less than 0
*
*END*********************************************************************/

int32_t _max7310_write
   (
      /* [IN] the address of the device specific information */
      IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr,

      /* [IN] The access handler for this operation */
      void *                       access_data,

      /* [IN] The array characters are to be read from */
      uint8_t *                       buffer,

      /* [IN] number of bytes to output */
      uint32_t                         length
   )
{ /* Body */
    uint32_t                         result = MQX_OK;
    MAX7310_INFO_STRUCT_PTR         io_info_ptr;
    MAX7310_ACCESS_DESC_STRUCT_PTR  access_ptr;
    uint8_t val;

    if (NULL == access_data || NULL == expander_io_dev_ptr
         || length < 1 || NULL == buffer)
        return -1;

    io_info_ptr = (MAX7310_INFO_STRUCT_PTR)expander_io_dev_ptr->DEV_INFO_PTR;
    if (NULL == io_info_ptr)
        return -1;

    access_ptr = (MAX7310_ACCESS_DESC_STRUCT_PTR)access_data;
    if (NULL == access_ptr || MAX7310_MAGIC_WORD != access_ptr->identity)
        return -1;

    _mutex_lock(&access_ptr->access_mutex);

    if (access_ptr->pin == INVALID_PIN_NO) {
        _mutex_unlock(&access_ptr->access_mutex);
        return -1;
    }

    /*check the direction*/
    result = max7310_get_dir(io_info_ptr, access_ptr->pin, &val);
    if (result != MQX_OK) {
        _mutex_unlock(&access_ptr->access_mutex);
        return -1;
    }

    if (val != MAX7310_DIR_OUT) {
        result = MQX_IO_OPERATION_NOT_AVAILABLE;
        _mutex_unlock(&access_ptr->access_mutex);
        return -1;
    }

    /*call the val setting func*/
    val = buffer[0];
    result = max7310_set_val(io_info_ptr, access_ptr->pin, val);
    if (result != MQX_OK) {
        _mutex_unlock(&access_ptr->access_mutex);
        return -2;
    }
    access_ptr->val = val;

    _mutex_unlock(&access_ptr->access_mutex);
    return 1;

} /* Endbody */

/* EOF */
