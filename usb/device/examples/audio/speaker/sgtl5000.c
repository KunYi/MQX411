/*HEADER**********************************************************************
*
* Copyright 2004-2010 Freescale Semiconductor, Inc.
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
*   This file contains functions for the SGTL5000
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <i2c.h>
#include "sgtl5000.h"
#include "audio_speaker.h"

MQX_FILE_PTR fd = NULL;
#define I2C_DEVICE_POLLED "i2c0:"

/*FUNCTION****************************************************************
*
* Function Name    : sgtl_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/
_mqx_int sgtl_Init(void)
{
    uint32_t param, result;
    I2C_STATISTICS_STRUCT stats;

    if (fd == NULL)
    {
       fd = fopen (I2C_DEVICE_POLLED, NULL);
    }
    if (fd == NULL)
    {
        printf ("ERROR: Unable to open I2C driver!\n");
        return(-9);
    }
    param = 200000;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_BAUD, &param))
    {
        return(-1);
    }
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL))
    {
        return(-2);
    }
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL))
    {
        return(-3);
    }
    param = SGTL5000_I2C_ADDR;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
    {
        return(-4);
    }

    /* Initiate start and send I2C bus address */
    result = fwrite (&param, 1, 0, fd);
    if (0 != result)
    {
        return(-5);
    }

    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats))
    {
        return(-6);
    }
   /* Stop I2C transfer */
     if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
     {
         return(-7);
     }
    /* Check ack (device exists) */
    if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param))
    {
        if ((param) || (stats.TX_NAKS))
        {
            return(-8);
        }
    }
    else
    {
        return(-9);
    }
    return(MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : sgtl_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/
_mqx_int sgtl_WriteReg(uint16_t reg, uint16_t reg_val)
{
    uint8_t buffer[4];
    uint32_t result;
    buffer[0] = (uint8_t)((reg >> 8) & 0xFF);
    buffer[1] =    (uint8_t)(reg & 0xFF);
    buffer[2] =    (uint8_t)((reg_val >> 8) & 0xFF);
    buffer[3] =    (uint8_t)(reg_val & 0xFF);
    result = write(fd, buffer, 4);
    if (4 != result)
    {
        #ifdef SGTL5000_DEBUG
         printf("sgtl_WriteReg: Error - write to address ");
         printf("0x%04X failed.\n", reg);
        #endif
        return(-1);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-3);
    }
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-2);
    }
    result = 0;
    return(MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : sgtl_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Reads value of register.
*
*END*********************************************************************/
_mqx_int sgtl_ReadReg(uint16_t reg, uint16_t *dest_ptr)
{
    uint8_t buffer[2];
    uint32_t result, param;
    buffer[0] = (uint8_t)((reg >> 8) & 0xFF);
    buffer[1] =    (uint8_t)(reg & 0xFF);
    result = write(fd, buffer, 2);
    if (2 != result)
    {
        #ifdef SGTL5000_DEBUG
         printf("sgtl_ReadReg: Error - SGTL not responding.\n");
        #endif
        return(-1);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-6);
    }
    /* Send repeated start */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL))
    {
        #ifdef SGTL5000_DEBUG
         printf("sgtl_ReadReg: Error - unable to send repeated start.\n");
        #endif
        return(-2);
    }
    /* Set read request */
    param = 2;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
    {
        #ifdef SGTL5000_DEBUG
         printf("sgtl_ReadReg: Error - unable to set number of bytes requested.\n");
        #endif
        return(-3);
    }
    result = 0;
    /* Read all data */
    result = read (fd, buffer, 2);
    if (2 != result)
    {
        #ifdef SGTL5000_DEBUG
         printf("sgtl_ReadReg: Error - SGTL not responding.\n");
        #endif
        return(-4);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-7);
    }
    *dest_ptr = (buffer[1] & 0xFFFF) | ((buffer[0] & 0xFFFF) << 8);
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-5);
    }
    return (MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : sgtl_ModifyReg
* Returned Value   : MQX error code
* Comments         :
*    Modifies value of register. Bits to set to zero are defined by first
*     mask, bits to be set to one are defined by second mask.
*
*END*********************************************************************/
_mqx_int sgtl_ModifyReg(uint16_t reg, uint16_t clr_mask, uint16_t set_mask)
{
    uint16_t reg_val = 0;
    if (MQX_OK != sgtl_ReadReg(reg, &reg_val))
    {
        #ifdef SGTL5000_DEBUG
         printf("sgtl_ModifyReg: Error - cannot read from SGTL.\n");
        #endif
        return(-1);
    }
    reg_val &= clr_mask;
    reg_val |= set_mask;
    if (MQX_OK != sgtl_WriteReg(reg, reg_val))
    {
        #ifdef SGTL5000_DEBUG
         printf("sgtl_ModifyReg: Error - cannot write to SGTL.\n");
        #endif
        return(-2);
    }
    return(MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : InitCodec
* Returned Value   : MQX error code
* Comments         :
*
*
*END*********************************************************************/
_mqx_int InitCodec (void)
{
    MQX_FILE_PTR i2s_ptr = NULL;
    _mqx_uint errcode = 0;
    uint32_t fs_freq = 44100;
    uint32_t mclk_freq = fs_freq * CLK_MULT;
#ifdef BSPCFG_ENABLE_SAI
    uint8_t mode = (I2S_TX_MASTER | I2S_RX_SLAVE);
#else
    uint8_t mode = I2S_MODE_MASTER;
#endif
    i2s_ptr = fopen(AUDIO_DEVICE, "w");

    if (i2s_ptr == NULL)
    {
        printf("\n  InitCodec error: Unable to open audio device.");
        return(0xDEAD);
    }

    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &mode);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq);
    _time_delay(50);

    errcode = sgtl_Init();
    if (errcode != MQX_OK)
    {
        return (errcode);
    }
    //--------------- Power Supply Configuration----------------
    // NOTE: This next 2 Write calls is needed ONLY if VDDD is
    // internally driven by the chip
    // Configure VDDD level to 1.2V (bits 3:0)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x0008);
    // Power up internal linear regulator (Set bit 9)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x7260);
    // NOTE: This next Write call is needed ONLY if VDDD is
    // externally driven
    // Turn off startup power supplies to save power (Clear bit 12 and 13)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4260);
    // NOTE: The next 2 Write calls is needed only if both VDDA and
    // VDDIO power supplies are less than 3.1V.
    // Enable the internal oscillator for the charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_CLK_TOP_CTRL, 0x0800);
    // Enable charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4A60);
    // NOTE: The next 2 modify calls is only needed if both VDDA and
    // VDDIO are greater than 3.1V
    // Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x006C);

    //------ Reference Voltage and Bias Current Configuration----------
    // NOTE: The value written in the next 2 Write calls is dependent
    // on the VDDA voltage value.
    // Set ground, ADC, DAC reference voltage (bits 8:4). The value should
    // be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
    // The bias current should be set to 50% of the nominal value (bits 3:1)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x004E);
    // Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and bias current
    // (bits 11:8) to the recommended value of 0.36mA for 10kOhm load with 1nF
    // capacitance
    sgtl_WriteReg(CHIP_LINE_OUT_CTRL, 0x0322);

    //----------------Other Analog Block Configurations------------------
    // Configure slow ramp up rate to minimize pop (bit 0)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x004F);
    // Enable short detect mode for headphone left/right
    // and center channel and set short detect current trip level
    // to 75mA
    sgtl_WriteReg(CHIP_SHORT_CTRL, 0x1106);
    // Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
    sgtl_WriteReg(CHIP_ANA_CTRL, 0x0133);

    //----------------Power up Inputs/Outputs/Digital Blocks-------------
    // Power up LINEOUT, HP, ADC, DAC
    sgtl_WriteReg(CHIP_ANA_POWER, 0x6AFF);
    // Power up desired digital blocks
    // I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5),
    // ADC (bit 6) are powered on
    sgtl_WriteReg(CHIP_DIG_POWER, 0x0073);

    //--------------------Set LINEOUT Volume Level-----------------------
    // Set the LINEOUT volume level based on voltage reference (VAG)
    // values using this formula
    // Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
    // Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the
    // left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set
    // to 5
    sgtl_WriteReg(CHIP_LINE_OUT_VOL, 0x0505);

    // Configure SYS_FS clock to 48kHz
    // Configure MCLK_FREQ to 256*Fs
    sgtl_ModifyReg(CHIP_CLK_CTRL, 0xFFC8, 0x0008); // bits 3:2
    // Configure the I2S clocks in master mode
    // NOTE: I2S LRCLK is same as the system sample clock
    // Data length = 16 bits
    sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x01B0); // bit 7

    // I2S_IN -> DAC -> HP_OUT
    // Route I2S_IN to DAC
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFDF, 0x0010);
    // Select DAC as the input to HP_OUT
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFBF, 0x0000);

    // TODO: Configure Microphone -> ADC -> I2S_OUT
    // Microphone -> ADC -> I2S_OUT
    // Set ADC input to Microphone
    //sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFB, 0x0000); // bit 2
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFF, 0x0004); // Set ADC input to LINE_IN
    // Route ADC to I2S_OUT
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFFC, 0x0000); // bits 1:0

    //---------------- Input Volume Control---------------------
    // Configure ADC left and right analog volume to desired default.
    // Example shows volume of 0dB
    sgtl_WriteReg(CHIP_ANA_ADC_CTRL, 0x0000);
    // Configure MIC gain if needed. Example shows gain of 20dB
    sgtl_ModifyReg(CHIP_MIC_CTRL, 0xFFFD, 0x0001); // bits 1:0

    //---------------- Volume and Mute Control---------------------
    // Configure HP_OUT left and right volume to minimum, unmute
    // HP_OUT and ramp the volume up to desired volume.
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, 0x7F7F);
    // Code assumes that left and right volumes are set to same value
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, 0x0000);
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFEF, 0x0000); // bit 5
    // LINEOUT and DAC volume control
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFEFF, 0x0000); // bit 8
    // Configure DAC left and right digital volume. Example shows
    // volume of 0dB
    sgtl_WriteReg(CHIP_DAC_VOL, 0x3C3C);
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFFB, 0x0000); // bit 2
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFF7, 0x0000); // bit 3
    // Unmute ADC
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFE ,0x0000); // bit 0
    if (fclose(i2s_ptr) != MQX_OK)
    {
        printf("\n  InitCodec error: Unable to close audio device.");
    }

    return (0);
}

/*FUNCTION****************************************************************
*
* Function Name    : SetupCodec
* Returned Value   : MQX error code
* Comments         :
*
*
*END*********************************************************************/
_mqx_int SetupCodec(MQX_FILE_PTR device)
{
    uint32_t mclk_freq;
    uint32_t fs_freq;
    uint32_t multiple;
    _mqx_int errcode = 0;
    uint16_t mask_ones, mask_zeros;
    uint8_t mode = 0;

    ioctl(device, IO_IOCTL_I2S_GET_MODE, &mode);
    ioctl(device, IO_IOCTL_I2S_GET_MCLK_FREQ, &mclk_freq);
    ioctl(device, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);

    multiple = mclk_freq/fs_freq;
    /* Convert "multiple" to value required for SGTL5000 CHIP_CLK_CTRL register (bits 1:0)*/
    mask_ones = (multiple/128) - 2;
    if (mask_ones > 0x2)
    {
        return (-1);
    }

    switch (fs_freq)
    {
    case 8000:
        mask_zeros = 0xFFE0;
        mask_ones |= 0x0020;
        break;

    case 11025:
        mask_zeros = 0xFFE4;
        mask_ones |= 0x0024;
        break;

    case 12000:
        mask_zeros = 0xFFE8;
        mask_ones |= 0x0028;
        break;

    case 16000:
        mask_zeros = 0xFFD0;
        mask_ones |= 0x0010;
        break;

    case 22050:
        mask_zeros = 0xFFD4;
        mask_ones |= 0x0014;
        break;

    case 24000:
        mask_zeros = 0xFFD8;
        mask_ones |= 0x0018;
        break;

    case 32000:
        mask_zeros = 0xFFC0;
        mask_ones |= 0x0000;
        break;

    case 44100:
        mask_zeros = 0xFFC4;
        mask_ones |= 0x0004;
        break;

    case 48000:
        mask_zeros = 0xFFC8;
        mask_ones |= 0x0008;
        break;

    case 96000:
        mask_zeros = 0xFFCC;
        mask_ones |= 0x000C;
        break;

    default:
        return(-2);
    }
    errcode = sgtl_ModifyReg(CHIP_CLK_CTRL, mask_zeros, mask_ones);

    if (errcode != MQX_OK)
    {
        return(-3);
    }
    /*
    * Setup I2S mode in codec
    */
#ifdef BSPCFG_ENABLE_SAI
    if (((mode & I2S_RX_MASTER) && (mode & I2S_IO_READ)) || ((mode & I2S_TX_MASTER) && (mode & I2S_IO_WRITE)))
#else
    if (mode & I2S_MODE_MASTER)
#endif
    {
        mask_ones = 0x0000;
    }
    else
    {
        mask_ones = 0x0800;
    }
    mask_zeros = 0xFF7F;

    errcode = sgtl_ModifyReg(CHIP_I2S_CTRL, mask_zeros, mask_ones);
    if (errcode != MQX_OK)
    {
        return(-4);
    }
    /* rise volume */
    sgtl_WriteReg(CHIP_DAC_VOL, 0x3C3C);
    return(0);
}

/* EOF */
