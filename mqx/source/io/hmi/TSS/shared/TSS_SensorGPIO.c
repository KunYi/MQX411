/***********************************************************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
*********************************************************************************************************************//*!
*
* @file   TSS_SensorGPIO.c
*
* @brief  Contains functions to Perform the sensing of the electrodes and set the status for each electrode
*
* @version 1.0.31.0
*
* @date Nov-6-2012
*
*
***********************************************************************************************************************/

#include "TSS_SensorGPIO.h"

#if TSS_DETECT_METHOD(GPIO)

  #include "TSS_Timer.h"
  #include "TSS_GPIO.h"

  #if TSS_HCS08_MCU || TSS_CFV1_MCU

    #if TSS_USE_SIMPLE_LOW_LEVEL

      #include "TSS_SensorGPIO_def.h" /* ROMDATA and RAMDATA GPIO definitions */

      /************************ Prototypes *******************************/

      #if TSS_USE_DEFAULT_ELECTRODE_LEVEL_LOW
        void GPIO_ElectrodesSetStateLow(void);
      #else
        void GPIO_ElectrodesSetStateHigh(void);
      #endif
      #if TSS_USE_GPIO_STRENGTH
        uint8_t GPIO_SetPinStrength(void);
      #endif
      #if TSS_USE_GPIO_SLEW_RATE
        uint8_t GPIO_SetPinSlewRate(void);
      #endif
      #if TSS_USE_NOISE_AMPLITUDE_FILTER
        void GPIO_NoiseAmplitudeFilterInit(void);
      #endif
      #ifdef TSS_ONPROXIMITY_CALLBACK
        void GPIO_ProximityInit(void);
      #endif

      #if TSS_USE_DEFAULT_ELECTRODE_LEVEL_LOW
        /***************************************************************************//*!
        *
        * @brief  Sets the electrode state as output low
        *
        * @param  void
        *
        * @return void
        *
        * @remarks
        *
        ****************************************************************************/
        void GPIO_ElectrodesSetStateLow(void)
        {
          #if TSS_N_ELECTRODES > 0
            #if TSS_DETECT_EL_METHOD(0,GPIO)
              TSS_ELECTRODE_LOW(0);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 1
            #if TSS_DETECT_EL_METHOD(1,GPIO)
              TSS_ELECTRODE_LOW(1);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 2
            #if TSS_DETECT_EL_METHOD(2,GPIO)
              TSS_ELECTRODE_LOW(2);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 3
            #if TSS_DETECT_EL_METHOD(3,GPIO)
              TSS_ELECTRODE_LOW(3);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 4
            #if TSS_DETECT_EL_METHOD(4,GPIO)
              TSS_ELECTRODE_LOW(4);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 5
            #if TSS_DETECT_EL_METHOD(5,GPIO)
              TSS_ELECTRODE_LOW(5);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 6
            #if TSS_DETECT_EL_METHOD(6,GPIO)
              TSS_ELECTRODE_LOW(6);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 7
            #if TSS_DETECT_EL_METHOD(7,GPIO)
              TSS_ELECTRODE_LOW(7);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 8
            #if TSS_DETECT_EL_METHOD(8,GPIO)
              TSS_ELECTRODE_LOW(8);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 9
            #if TSS_DETECT_EL_METHOD(9,GPIO)
              TSS_ELECTRODE_LOW(9);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 10
            #if TSS_DETECT_EL_METHOD(10,GPIO)
              TSS_ELECTRODE_LOW(10);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 11
            #if TSS_DETECT_EL_METHOD(11,GPIO)
              TSS_ELECTRODE_LOW(11);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 12
            #if TSS_DETECT_EL_METHOD(12,GPIO)
              TSS_ELECTRODE_LOW(12);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 13
            #if TSS_DETECT_EL_METHOD(13,GPIO)
              TSS_ELECTRODE_LOW(13);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 14
            #if TSS_DETECT_EL_METHOD(14,GPIO)
              TSS_ELECTRODE_LOW(14);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 15
            #if TSS_DETECT_EL_METHOD(15,GPIO)
              TSS_ELECTRODE_LOW(15);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 16
            #if TSS_DETECT_EL_METHOD(16,GPIO)
              TSS_ELECTRODE_LOW(16);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 17
            #if TSS_DETECT_EL_METHOD(17,GPIO)
              TSS_ELECTRODE_LOW(17);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 18
            #if TSS_DETECT_EL_METHOD(18,GPIO)
              TSS_ELECTRODE_LOW(18);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 19
            #if TSS_DETECT_EL_METHOD(19,GPIO)
              TSS_ELECTRODE_LOW(19);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 20
            #if TSS_DETECT_EL_METHOD(20,GPIO)
              TSS_ELECTRODE_LOW(20);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 21
            #if TSS_DETECT_EL_METHOD(21,GPIO)
              TSS_ELECTRODE_LOW(21);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 22
            #if TSS_DETECT_EL_METHOD(22,GPIO)
              TSS_ELECTRODE_LOW(22);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 23
            #if TSS_DETECT_EL_METHOD(23,GPIO)
              TSS_ELECTRODE_LOW(23);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 24
            #if TSS_DETECT_EL_METHOD(24,GPIO)
              TSS_ELECTRODE_LOW(24);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 25
            #if TSS_DETECT_EL_METHOD(25,GPIO)
              TSS_ELECTRODE_LOW(25);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 26
            #if TSS_DETECT_EL_METHOD(26,GPIO)
              TSS_ELECTRODE_LOW(26);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 27
            #if TSS_DETECT_EL_METHOD(27,GPIO)
              TSS_ELECTRODE_LOW(27);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 28
            #if TSS_DETECT_EL_METHOD(28,GPIO)
              TSS_ELECTRODE_LOW(28);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 29
            #if TSS_DETECT_EL_METHOD(29,GPIO)
              TSS_ELECTRODE_LOW(29);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 30
            #if TSS_DETECT_EL_METHOD(30,GPIO)
              TSS_ELECTRODE_LOW(30);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 31
            #if TSS_DETECT_EL_METHOD(31,GPIO)
              TSS_ELECTRODE_LOW(31);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 32
            #if TSS_DETECT_EL_METHOD(32,GPIO)
              TSS_ELECTRODE_LOW(32);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 33
            #if TSS_DETECT_EL_METHOD(33,GPIO)
              TSS_ELECTRODE_LOW(33);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 34
            #if TSS_DETECT_EL_METHOD(34,GPIO)
              TSS_ELECTRODE_LOW(34);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 35
            #if TSS_DETECT_EL_METHOD(35,GPIO)
              TSS_ELECTRODE_LOW(35);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 36
            #if TSS_DETECT_EL_METHOD(36,GPIO)
              TSS_ELECTRODE_LOW(36);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 37
            #if TSS_DETECT_EL_METHOD(37,GPIO)
              TSS_ELECTRODE_LOW(37);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 38
            #if TSS_DETECT_EL_METHOD(38,GPIO)
              TSS_ELECTRODE_LOW(38);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 39
            #if TSS_DETECT_EL_METHOD(39,GPIO)
              TSS_ELECTRODE_LOW(39);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 40
            #if TSS_DETECT_EL_METHOD(40,GPIO)
              TSS_ELECTRODE_LOW(40);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 41
            #if TSS_DETECT_EL_METHOD(41,GPIO)
              TSS_ELECTRODE_LOW(41);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 42
            #if TSS_DETECT_EL_METHOD(42,GPIO)
              TSS_ELECTRODE_LOW(42);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 43
            #if TSS_DETECT_EL_METHOD(43,GPIO)
              TSS_ELECTRODE_LOW(43);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 44
            #if TSS_DETECT_EL_METHOD(44,GPIO)
              TSS_ELECTRODE_LOW(44);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 45
            #if TSS_DETECT_EL_METHOD(45,GPIO)
              TSS_ELECTRODE_LOW(45);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 46
            #if TSS_DETECT_EL_METHOD(46,GPIO)
              TSS_ELECTRODE_LOW(46);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 47
            #if TSS_DETECT_EL_METHOD(47,GPIO)
              TSS_ELECTRODE_LOW(47);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 48
            #if TSS_DETECT_EL_METHOD(48,GPIO)
              TSS_ELECTRODE_LOW(48);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 49
            #if TSS_DETECT_EL_METHOD(49,GPIO)
              TSS_ELECTRODE_LOW(49);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 50
            #if TSS_DETECT_EL_METHOD(50,GPIO)
              TSS_ELECTRODE_LOW(50);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 51
            #if TSS_DETECT_EL_METHOD(51,GPIO)
              TSS_ELECTRODE_LOW(51);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 52
            #if TSS_DETECT_EL_METHOD(52,GPIO)
              TSS_ELECTRODE_LOW(52);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 53
            #if TSS_DETECT_EL_METHOD(53,GPIO)
              TSS_ELECTRODE_LOW(53);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 54
            #if TSS_DETECT_EL_METHOD(54,GPIO)
              TSS_ELECTRODE_LOW(54);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 55
            #if TSS_DETECT_EL_METHOD(55,GPIO)
              TSS_ELECTRODE_LOW(55);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 56
            #if TSS_DETECT_EL_METHOD(56,GPIO)
              TSS_ELECTRODE_LOW(56);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 57
            #if TSS_DETECT_EL_METHOD(57,GPIO)
              TSS_ELECTRODE_LOW(57);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 58
            #if TSS_DETECT_EL_METHOD(58,GPIO)
              TSS_ELECTRODE_LOW(58);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 59
            #if TSS_DETECT_EL_METHOD(59,GPIO)
              TSS_ELECTRODE_LOW(59);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 60
            #if TSS_DETECT_EL_METHOD(60,GPIO)
              TSS_ELECTRODE_LOW(60);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 61
            #if TSS_DETECT_EL_METHOD(61,GPIO)
              TSS_ELECTRODE_LOW(61);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 62
            #if TSS_DETECT_EL_METHOD(62,GPIO)
              TSS_ELECTRODE_LOW(62);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 63
            #if TSS_DETECT_EL_METHOD(63,GPIO)
              TSS_ELECTRODE_LOW(63);
            #endif
          #endif
        }
      #else
        /***************************************************************************//*!
        *
        * @brief  Sets the electrode state as output high
        *
        * @param  void
        *
        * @return void
        *
        * @remarks
        *
        ****************************************************************************/
        void GPIO_ElectrodesSetStateHigh(void)
        {
          #if TSS_N_ELECTRODES > 0
            #if TSS_DETECT_EL_METHOD(0,GPIO)
              TSS_ELECTRODE_HIGH(0);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 1
            #if TSS_DETECT_EL_METHOD(1,GPIO)
              TSS_ELECTRODE_HIGH(1);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 2
            #if TSS_DETECT_EL_METHOD(2,GPIO)
              TSS_ELECTRODE_HIGH(2);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 3
            #if TSS_DETECT_EL_METHOD(3,GPIO)
              TSS_ELECTRODE_HIGH(3);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 4
            #if TSS_DETECT_EL_METHOD(4,GPIO)
              TSS_ELECTRODE_HIGH(4);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 5
            #if TSS_DETECT_EL_METHOD(5,GPIO)
              TSS_ELECTRODE_HIGH(5);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 6
            #if TSS_DETECT_EL_METHOD(6,GPIO)
              TSS_ELECTRODE_HIGH(6);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 7
            #if TSS_DETECT_EL_METHOD(7,GPIO)
              TSS_ELECTRODE_HIGH(7);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 8
            #if TSS_DETECT_EL_METHOD(8,GPIO)
              TSS_ELECTRODE_HIGH(8);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 9
            #if TSS_DETECT_EL_METHOD(9,GPIO)
              TSS_ELECTRODE_HIGH(9);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 10
            #if TSS_DETECT_EL_METHOD(10,GPIO)
              TSS_ELECTRODE_HIGH(10);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 11
            #if TSS_DETECT_EL_METHOD(11,GPIO)
              TSS_ELECTRODE_HIGH(11);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 12
            #if TSS_DETECT_EL_METHOD(12,GPIO)
              TSS_ELECTRODE_HIGH(12);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 13
            #if TSS_DETECT_EL_METHOD(13,GPIO)
              TSS_ELECTRODE_HIGH(13);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 14
            #if TSS_DETECT_EL_METHOD(14,GPIO)
              TSS_ELECTRODE_HIGH(14);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 15
            #if TSS_DETECT_EL_METHOD(15,GPIO)
              TSS_ELECTRODE_HIGH(15);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 16
            #if TSS_DETECT_EL_METHOD(16,GPIO)
              TSS_ELECTRODE_HIGH(16);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 17
            #if TSS_DETECT_EL_METHOD(17,GPIO)
              TSS_ELECTRODE_HIGH(17);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 18
            #if TSS_DETECT_EL_METHOD(18,GPIO)
              TSS_ELECTRODE_HIGH(18);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 19
            #if TSS_DETECT_EL_METHOD(19,GPIO)
              TSS_ELECTRODE_HIGH(19);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 20
            #if TSS_DETECT_EL_METHOD(20,GPIO)
              TSS_ELECTRODE_HIGH(20);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 21
            #if TSS_DETECT_EL_METHOD(21,GPIO)
              TSS_ELECTRODE_HIGH(21);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 22
            #if TSS_DETECT_EL_METHOD(22,GPIO)
              TSS_ELECTRODE_HIGH(22);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 23
            #if TSS_DETECT_EL_METHOD(23,GPIO)
              TSS_ELECTRODE_HIGH(23);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 24
            #if TSS_DETECT_EL_METHOD(24,GPIO)
              TSS_ELECTRODE_HIGH(24);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 25
            #if TSS_DETECT_EL_METHOD(25,GPIO)
              TSS_ELECTRODE_HIGH(25);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 26
            #if TSS_DETECT_EL_METHOD(26,GPIO)
              TSS_ELECTRODE_HIGH(26);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 27
            #if TSS_DETECT_EL_METHOD(27,GPIO)
              TSS_ELECTRODE_HIGH(27);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 28
            #if TSS_DETECT_EL_METHOD(28,GPIO)
              TSS_ELECTRODE_HIGH(28);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 29
            #if TSS_DETECT_EL_METHOD(29,GPIO)
              TSS_ELECTRODE_HIGH(29);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 30
            #if TSS_DETECT_EL_METHOD(30,GPIO)
              TSS_ELECTRODE_HIGH(30);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 31
            #if TSS_DETECT_EL_METHOD(31,GPIO)
              TSS_ELECTRODE_HIGH(31);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 32
            #if TSS_DETECT_EL_METHOD(32,GPIO)
              TSS_ELECTRODE_HIGH(32);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 33
            #if TSS_DETECT_EL_METHOD(33,GPIO)
              TSS_ELECTRODE_HIGH(33);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 34
            #if TSS_DETECT_EL_METHOD(34,GPIO)
              TSS_ELECTRODE_HIGH(34);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 35
            #if TSS_DETECT_EL_METHOD(35,GPIO)
              TSS_ELECTRODE_HIGH(35);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 36
            #if TSS_DETECT_EL_METHOD(36,GPIO)
              TSS_ELECTRODE_HIGH(36);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 37
            #if TSS_DETECT_EL_METHOD(37,GPIO)
              TSS_ELECTRODE_HIGH(37);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 38
            #if TSS_DETECT_EL_METHOD(38,GPIO)
              TSS_ELECTRODE_HIGH(38);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 39
            #if TSS_DETECT_EL_METHOD(39,GPIO)
              TSS_ELECTRODE_HIGH(39);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 40
            #if TSS_DETECT_EL_METHOD(40,GPIO)
              TSS_ELECTRODE_HIGH(40);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 41
            #if TSS_DETECT_EL_METHOD(41,GPIO)
              TSS_ELECTRODE_HIGH(41);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 42
            #if TSS_DETECT_EL_METHOD(42,GPIO)
              TSS_ELECTRODE_HIGH(42);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 43
            #if TSS_DETECT_EL_METHOD(43,GPIO)
              TSS_ELECTRODE_HIGH(43);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 44
            #if TSS_DETECT_EL_METHOD(44,GPIO)
              TSS_ELECTRODE_HIGH(44);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 45
            #if TSS_DETECT_EL_METHOD(45,GPIO)
              TSS_ELECTRODE_HIGH(45);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 46
            #if TSS_DETECT_EL_METHOD(46,GPIO)
              TSS_ELECTRODE_HIGH(46);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 47
            #if TSS_DETECT_EL_METHOD(47,GPIO)
              TSS_ELECTRODE_HIGH(47);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 48
            #if TSS_DETECT_EL_METHOD(48,GPIO)
              TSS_ELECTRODE_HIGH(48);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 49
            #if TSS_DETECT_EL_METHOD(49,GPIO)
              TSS_ELECTRODE_HIGH(49);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 50
            #if TSS_DETECT_EL_METHOD(50,GPIO)
              TSS_ELECTRODE_HIGH(50);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 51
            #if TSS_DETECT_EL_METHOD(51,GPIO)
              TSS_ELECTRODE_HIGH(51);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 52
            #if TSS_DETECT_EL_METHOD(52,GPIO)
              TSS_ELECTRODE_HIGH(52);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 53
            #if TSS_DETECT_EL_METHOD(53,GPIO)
              TSS_ELECTRODE_HIGH(53);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 54
            #if TSS_DETECT_EL_METHOD(54,GPIO)
              TSS_ELECTRODE_HIGH(54);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 55
            #if TSS_DETECT_EL_METHOD(55,GPIO)
              TSS_ELECTRODE_HIGH(55);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 56
            #if TSS_DETECT_EL_METHOD(56,GPIO)
              TSS_ELECTRODE_HIGH(56);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 57
            #if TSS_DETECT_EL_METHOD(57,GPIO)
              TSS_ELECTRODE_HIGH(57);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 58
            #if TSS_DETECT_EL_METHOD(58,GPIO)
              TSS_ELECTRODE_HIGH(58);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 59
            #if TSS_DETECT_EL_METHOD(59,GPIO)
              TSS_ELECTRODE_HIGH(59);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 60
            #if TSS_DETECT_EL_METHOD(60,GPIO)
              TSS_ELECTRODE_HIGH(60);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 61
            #if TSS_DETECT_EL_METHOD(61,GPIO)
              TSS_ELECTRODE_HIGH(61);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 62
            #if TSS_DETECT_EL_METHOD(62,GPIO)
              TSS_ELECTRODE_HIGH(62);
            #endif
          #endif
          #if TSS_N_ELECTRODES > 63
            #if TSS_DETECT_EL_METHOD(63,GPIO)
              TSS_ELECTRODE_HIGH(63);
            #endif
          #endif
        }
      #endif

      #if TSS_USE_GPIO_STRENGTH
        /***************************************************************************//*!
        *
        * @brief  Performs setting of Strength on each defined GPIO pin
        *
        * @param  Nothing
        *
        * @return Status Code
        *
        * @remarks If the setting is not possible the function shows a Warning
        *
        ****************************************************************************/
        uint8_t GPIO_SetPinStrength(void)
        {
          uint8_t u8result = TSS_STATUS_OK;                /* Sets return status code */

          #if TSS_N_ELECTRODES > 0
            #if TSS_DETECT_EL_METHOD(0,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(0)
                TSS_ELECTRODE_STRENGTH_SET(0);
              #else
                #warning "TSS - The electrode E0 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 1
            #if TSS_DETECT_EL_METHOD(1,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(1)
                TSS_ELECTRODE_STRENGTH_SET(1);
              #else
                #warning "TSS - The electrode E1 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 2
            #if TSS_DETECT_EL_METHOD(2,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(2)
                TSS_ELECTRODE_STRENGTH_SET(2);
              #else
                #warning "TSS - The electrode E2 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 3
            #if TSS_DETECT_EL_METHOD(3,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(3)
                TSS_ELECTRODE_STRENGTH_SET(3);
              #else
                #warning "TSS - The electrode E3 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 4
            #if TSS_DETECT_EL_METHOD(4,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(4)
                TSS_ELECTRODE_STRENGTH_SET(4);
              #else
                #warning "TSS - The electrode E4 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 5
            #if TSS_DETECT_EL_METHOD(5,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(5)
                TSS_ELECTRODE_STRENGTH_SET(5);
              #else
                #warning "TSS - The electrode E5 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 6
            #if TSS_DETECT_EL_METHOD(6,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(6)
                TSS_ELECTRODE_STRENGTH_SET(6);
              #else
                #warning "TSS - The electrode E6 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 7
            #if TSS_DETECT_EL_METHOD(7,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(7)
                TSS_ELECTRODE_STRENGTH_SET(7);
              #else
                #warning "TSS - The electrode E7 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 8
            #if TSS_DETECT_EL_METHOD(8,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(8)
                TSS_ELECTRODE_STRENGTH_SET(8);
              #else
                #warning "TSS - The electrode E8 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 9
            #if TSS_DETECT_EL_METHOD(9,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(9)
                TSS_ELECTRODE_STRENGTH_SET(9);
              #else
                #warning "TSS - The electrode E9 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 10
            #if TSS_DETECT_EL_METHOD(10,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(10)
                TSS_ELECTRODE_STRENGTH_SET(10);
              #else
                #warning "TSS - The electrode E10 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 11
            #if TSS_DETECT_EL_METHOD(11,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(11)
                TSS_ELECTRODE_STRENGTH_SET(11);
              #else
                #warning "TSS - The electrode E11 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 12
            #if TSS_DETECT_EL_METHOD(12,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(12)
                TSS_ELECTRODE_STRENGTH_SET(12);
              #else
                #warning "TSS - The electrode E12 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 13
            #if TSS_DETECT_EL_METHOD(13,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(13)
                TSS_ELECTRODE_STRENGTH_SET(13);
              #else
                #warning "TSS - The electrode E13 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 14
            #if TSS_DETECT_EL_METHOD(14,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(14)
                TSS_ELECTRODE_STRENGTH_SET(14);
              #else
                #warning "TSS - The electrode E14 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 15
            #if TSS_DETECT_EL_METHOD(15,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(15)
                TSS_ELECTRODE_STRENGTH_SET(15);
              #else
                #warning "TSS - The electrode E15 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 16
            #if TSS_DETECT_EL_METHOD(16,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(16)
                TSS_ELECTRODE_STRENGTH_SET(16);
              #else
                #warning "TSS - The electrode E16 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 17
            #if TSS_DETECT_EL_METHOD(17,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(17)
                TSS_ELECTRODE_STRENGTH_SET(17);
              #else
                #warning "TSS - The electrode E17 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 18
            #if TSS_DETECT_EL_METHOD(18,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(18)
                TSS_ELECTRODE_STRENGTH_SET(18);
              #else
                #warning "TSS - The electrode E18 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 19
            #if TSS_DETECT_EL_METHOD(19,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(19)
                TSS_ELECTRODE_STRENGTH_SET(19);
              #else
                #warning "TSS - The electrode E19 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 20
            #if TSS_DETECT_EL_METHOD(20,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(20)
                TSS_ELECTRODE_STRENGTH_SET(20);
              #else
                #warning "TSS - The electrode E20 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 21
            #if TSS_DETECT_EL_METHOD(21,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(21)
                TSS_ELECTRODE_STRENGTH_SET(21);
              #else
                #warning "TSS - The electrode E21 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 22
            #if TSS_DETECT_EL_METHOD(22,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(22)
                TSS_ELECTRODE_STRENGTH_SET(22);
              #else
                #warning "TSS - The electrode E22 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 23
            #if TSS_DETECT_EL_METHOD(23,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(23)
                TSS_ELECTRODE_STRENGTH_SET(23);
              #else
                #warning "TSS - The electrode E23 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 24
            #if TSS_DETECT_EL_METHOD(24,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(24)
                TSS_ELECTRODE_STRENGTH_SET(24);
              #else
                #warning "TSS - The electrode E24 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 25
            #if TSS_DETECT_EL_METHOD(25,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(25)
                TSS_ELECTRODE_STRENGTH_SET(25);
              #else
                #warning "TSS - The electrode E25 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 26
            #if TSS_DETECT_EL_METHOD(26,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(26)
                TSS_ELECTRODE_STRENGTH_SET(26);
              #else
                #warning "TSS - The electrode E26 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 27
            #if TSS_DETECT_EL_METHOD(27,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(27)
                TSS_ELECTRODE_STRENGTH_SET(27);
              #else
                #warning "TSS - The electrode E27 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 28
            #if TSS_DETECT_EL_METHOD(28,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(28)
                TSS_ELECTRODE_STRENGTH_SET(28);
              #else
                #warning "TSS - The electrode E28 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 29
            #if TSS_DETECT_EL_METHOD(29,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(29)
                TSS_ELECTRODE_STRENGTH_SET(29);
              #else
                #warning "TSS - The electrode E29 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 30
            #if TSS_DETECT_EL_METHOD(30,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(30)
                TSS_ELECTRODE_STRENGTH_SET(30);
              #else
                #warning "TSS - The electrode E30 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 31
            #if TSS_DETECT_EL_METHOD(31,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(31)
                TSS_ELECTRODE_STRENGTH_SET(31);
              #else
                #warning "TSS - The electrode E31 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 32
            #if TSS_DETECT_EL_METHOD(32,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(32)
                TSS_ELECTRODE_STRENGTH_SET(32);
              #else
                #warning "TSS - The electrode E32 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 33
            #if TSS_DETECT_EL_METHOD(33,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(33)
                TSS_ELECTRODE_STRENGTH_SET(33);
              #else
                #warning "TSS - The electrode E33 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 34
            #if TSS_DETECT_EL_METHOD(34,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(34)
                TSS_ELECTRODE_STRENGTH_SET(34);
              #else
                #warning "TSS - The electrode E34 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 35
            #if TSS_DETECT_EL_METHOD(35,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(35)
                TSS_ELECTRODE_STRENGTH_SET(35);
              #else
                #warning "TSS - The electrode E35 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 36
            #if TSS_DETECT_EL_METHOD(36,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(36)
                TSS_ELECTRODE_STRENGTH_SET(36);
              #else
                #warning "TSS - The electrode E36 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 37
            #if TSS_DETECT_EL_METHOD(37,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(37)
                TSS_ELECTRODE_STRENGTH_SET(37);
              #else
                #warning "TSS - The electrode E37 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 38
            #if TSS_DETECT_EL_METHOD(38,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(38)
                TSS_ELECTRODE_STRENGTH_SET(38);
              #else
                #warning "TSS - The electrode E38 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 39
            #if TSS_DETECT_EL_METHOD(39,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(39)
                TSS_ELECTRODE_STRENGTH_SET(39);
              #else
                #warning "TSS - The electrode E39 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 40
            #if TSS_DETECT_EL_METHOD(40,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(40)
                TSS_ELECTRODE_STRENGTH_SET(40);
              #else
                #warning "TSS - The electrode E40 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 41
            #if TSS_DETECT_EL_METHOD(41,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(41)
                TSS_ELECTRODE_STRENGTH_SET(41);
              #else
                #warning "TSS - The electrode E41 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 42
            #if TSS_DETECT_EL_METHOD(42,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(42)
                TSS_ELECTRODE_STRENGTH_SET(42);
              #else
                #warning "TSS - The electrode E42 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 43
            #if TSS_DETECT_EL_METHOD(43,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(43)
                TSS_ELECTRODE_STRENGTH_SET(43);
              #else
                #warning "TSS - The electrode E43 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 44
            #if TSS_DETECT_EL_METHOD(44,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(44)
                TSS_ELECTRODE_STRENGTH_SET(44);
              #else
                #warning "TSS - The electrode E44 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 45
            #if TSS_DETECT_EL_METHOD(45,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(45)
                TSS_ELECTRODE_STRENGTH_SET(45);
              #else
                #warning "TSS - The electrode E45 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 46
            #if TSS_DETECT_EL_METHOD(46,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(46)
                TSS_ELECTRODE_STRENGTH_SET(46);
              #else
                #warning "TSS - The electrode E46 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 47
            #if TSS_DETECT_EL_METHOD(47,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(47)
                TSS_ELECTRODE_STRENGTH_SET(47);
              #else
                #warning "TSS - The electrode E47 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 48
            #if TSS_DETECT_EL_METHOD(48,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(48)
                TSS_ELECTRODE_STRENGTH_SET(48);
              #else
                #warning "TSS - The electrode E48 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 49
            #if TSS_DETECT_EL_METHOD(49,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(49)
                TSS_ELECTRODE_STRENGTH_SET(49);
              #else
                #warning "TSS - The electrode E49 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 50
            #if TSS_DETECT_EL_METHOD(50,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(50)
                TSS_ELECTRODE_STRENGTH_SET(50);
              #else
                #warning "TSS - The electrode E50 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 51
            #if TSS_DETECT_EL_METHOD(51,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(51)
                TSS_ELECTRODE_STRENGTH_SET(51);
              #else
                #warning "TSS - The electrode E51 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 52
            #if TSS_DETECT_EL_METHOD(52,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(52)
                TSS_ELECTRODE_STRENGTH_SET(52);
              #else
                #warning "TSS - The electrode E52 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 53
            #if TSS_DETECT_EL_METHOD(53,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(53)
                TSS_ELECTRODE_STRENGTH_SET(53);
              #else
                #warning "TSS - The electrode E53 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 54
            #if TSS_DETECT_EL_METHOD(54,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(54)
                TSS_ELECTRODE_STRENGTH_SET(54);
              #else
                #warning "TSS - The electrode E54 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 55
            #if TSS_DETECT_EL_METHOD(55,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(55)
                TSS_ELECTRODE_STRENGTH_SET(55);
              #else
                #warning "TSS - The electrode E55 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 56
            #if TSS_DETECT_EL_METHOD(56,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(56)
                TSS_ELECTRODE_STRENGTH_SET(56);
              #else
                #warning "TSS - The electrode E56 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 57
            #if TSS_DETECT_EL_METHOD(57,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(57)
                TSS_ELECTRODE_STRENGTH_SET(57);
              #else
                #warning "TSS - The electrode E57 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 58
            #if TSS_DETECT_EL_METHOD(58,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(58)
                TSS_ELECTRODE_STRENGTH_SET(58);
              #else
                #warning "TSS - The electrode E58 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 59
            #if TSS_DETECT_EL_METHOD(59,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(59)
                TSS_ELECTRODE_STRENGTH_SET(59);
              #else
                #warning "TSS - The electrode E59 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 60
            #if TSS_DETECT_EL_METHOD(60,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(60)
                TSS_ELECTRODE_STRENGTH_SET(60);
              #else
                #warning "TSS - The electrode E60 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 61
            #if TSS_DETECT_EL_METHOD(61,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(61)
                TSS_ELECTRODE_STRENGTH_SET(61);
              #else
                #warning "TSS - The electrode E61 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 62
            #if TSS_DETECT_EL_METHOD(62,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(62)
                TSS_ELECTRODE_STRENGTH_SET(62);
              #else
                #warning "TSS - The electrode E62 does not provide STRENGTH feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 63
            #if TSS_DETECT_EL_METHOD(63,GPIO)
              #if TSS_ELECTRODE_STRENGTH_CHECK(63)
                TSS_ELECTRODE_STRENGTH_SET(63);
              #else
                #warning "TSS - The electrode E63 does not provide STRENGTH feature"
              #endif
            #endif
          #endif

          return u8result;
        }
      #endif

      #if TSS_USE_GPIO_SLEW_RATE
        /***************************************************************************//*!
        *
        * @brief  Performs setting of Slew Rate on each defined GPIO pin
        *
        * @param  Nothing
        *
        * @return Status Code
        *
        * @remarks If the setting is not possible the function shows a Warning
        *
        ****************************************************************************/
        uint8_t GPIO_SetPinSlewRate(void)
        {
          uint8_t u8result = TSS_STATUS_OK;                /* Sets return status code */

          #if TSS_N_ELECTRODES > 0
            #if TSS_DETECT_EL_METHOD(0,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(0)
                TSS_ELECTRODE_SLEWRATE_SET(0);
              #else
                #warning "TSS - The electrode E0 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 1
            #if TSS_DETECT_EL_METHOD(1,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(1)
                TSS_ELECTRODE_SLEWRATE_SET(1);
              #else
                #warning "TSS - The electrode E1 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 2
            #if TSS_DETECT_EL_METHOD(2,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(2)
                TSS_ELECTRODE_SLEWRATE_SET(2);
              #else
                #warning "TSS - The electrode E2 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 3
            #if TSS_DETECT_EL_METHOD(3,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(3)
                TSS_ELECTRODE_SLEWRATE_SET(3);
              #else
                #warning "TSS - The electrode E3 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 4
            #if TSS_DETECT_EL_METHOD(4,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(4)
                TSS_ELECTRODE_SLEWRATE_SET(4);
              #else
                #warning "TSS - The electrode E4 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 5
            #if TSS_DETECT_EL_METHOD(5,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(5)
                TSS_ELECTRODE_SLEWRATE_SET(5);
              #else
                #warning "TSS - The electrode E5 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 6
            #if TSS_DETECT_EL_METHOD(6,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(6)
                TSS_ELECTRODE_SLEWRATE_SET(6);
              #else
                #warning "TSS - The electrode E6 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 7
            #if TSS_DETECT_EL_METHOD(7,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(7)
                TSS_ELECTRODE_SLEWRATE_SET(7);
              #else
                #warning "TSS - The electrode E7 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 8
            #if TSS_DETECT_EL_METHOD(8,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(8)
                TSS_ELECTRODE_SLEWRATE_SET(8);
              #else
                #warning "TSS - The electrode E8 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 9
            #if TSS_DETECT_EL_METHOD(9,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(9)
                TSS_ELECTRODE_SLEWRATE_SET(9);
              #else
                #warning "TSS - The electrode E9 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 10
            #if TSS_DETECT_EL_METHOD(10,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(10)
                TSS_ELECTRODE_SLEWRATE_SET(10);
              #else
                #warning "TSS - The electrode E10 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 11
            #if TSS_DETECT_EL_METHOD(11,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(11)
                TSS_ELECTRODE_SLEWRATE_SET(11);
              #else
                #warning "TSS - The electrode E11 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 12
            #if TSS_DETECT_EL_METHOD(12,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(12)
                TSS_ELECTRODE_SLEWRATE_SET(12);
              #else
                #warning "TSS - The electrode E12 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 13
            #if TSS_DETECT_EL_METHOD(13,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(13)
                TSS_ELECTRODE_SLEWRATE_SET(13);
              #else
                #warning "TSS - The electrode E13 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 14
            #if TSS_DETECT_EL_METHOD(14,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(14)
                TSS_ELECTRODE_SLEWRATE_SET(14);
              #else
                #warning "TSS - The electrode E14 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 15
            #if TSS_DETECT_EL_METHOD(15,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(15)
                TSS_ELECTRODE_SLEWRATE_SET(15);
              #else
                #warning "TSS - The electrode E15 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 16
            #if TSS_DETECT_EL_METHOD(16,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(16)
                TSS_ELECTRODE_SLEWRATE_SET(16);
              #else
                #warning "TSS - The electrode E16 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 17
            #if TSS_DETECT_EL_METHOD(17,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(17)
                TSS_ELECTRODE_SLEWRATE_SET(17);
              #else
                #warning "TSS - The electrode E17 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 18
            #if TSS_DETECT_EL_METHOD(18,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(18)
                TSS_ELECTRODE_SLEWRATE_SET(18);
              #else
                #warning "TSS - The electrode E18 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 19
            #if TSS_DETECT_EL_METHOD(19,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(19)
                TSS_ELECTRODE_SLEWRATE_SET(19);
              #else
                #warning "TSS - The electrode E19 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 20
            #if TSS_DETECT_EL_METHOD(20,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(20)
                TSS_ELECTRODE_SLEWRATE_SET(20);
              #else
                #warning "TSS - The electrode E20 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 21
            #if TSS_DETECT_EL_METHOD(21,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(21)
                TSS_ELECTRODE_SLEWRATE_SET(21);
              #else
                #warning "TSS - The electrode E21 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 22
            #if TSS_DETECT_EL_METHOD(22,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(22)
                TSS_ELECTRODE_SLEWRATE_SET(22);
              #else
                #warning "TSS - The electrode E22 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 23
            #if TSS_DETECT_EL_METHOD(23,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(23)
                TSS_ELECTRODE_SLEWRATE_SET(23);
              #else
                #warning "TSS - The electrode E23 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 24
            #if TSS_DETECT_EL_METHOD(24,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(24)
                TSS_ELECTRODE_SLEWRATE_SET(24);
              #else
                #warning "TSS - The electrode E24 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 25
            #if TSS_DETECT_EL_METHOD(25,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(25)
                TSS_ELECTRODE_SLEWRATE_SET(25);
              #else
                #warning "TSS - The electrode E25 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 26
            #if TSS_DETECT_EL_METHOD(26,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(26)
                TSS_ELECTRODE_SLEWRATE_SET(26);
              #else
                #warning "TSS - The electrode E26 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 27
            #if TSS_DETECT_EL_METHOD(27,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(27)
                TSS_ELECTRODE_SLEWRATE_SET(27);
              #else
                #warning "TSS - The electrode E27 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 28
            #if TSS_DETECT_EL_METHOD(28,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(28)
                TSS_ELECTRODE_SLEWRATE_SET(28);
              #else
                #warning "TSS - The electrode E28 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 29
            #if TSS_DETECT_EL_METHOD(29,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(29)
                TSS_ELECTRODE_SLEWRATE_SET(29);
              #else
                #warning "TSS - The electrode E29 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 30
            #if TSS_DETECT_EL_METHOD(30,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(30)
                TSS_ELECTRODE_SLEWRATE_SET(30);
              #else
                #warning "TSS - The electrode E30 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 31
            #if TSS_DETECT_EL_METHOD(31,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(31)
                TSS_ELECTRODE_SLEWRATE_SET(31);
              #else
                #warning "TSS - The electrode E31 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 32
            #if TSS_DETECT_EL_METHOD(32,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(32)
                TSS_ELECTRODE_SLEWRATE_SET(32);
              #else
                #warning "TSS - The electrode E32 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 33
            #if TSS_DETECT_EL_METHOD(33,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(33)
                TSS_ELECTRODE_SLEWRATE_SET(33);
              #else
                #warning "TSS - The electrode E33 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 34
            #if TSS_DETECT_EL_METHOD(34,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(34)
                TSS_ELECTRODE_SLEWRATE_SET(34);
              #else
                #warning "TSS - The electrode E34 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 35
            #if TSS_DETECT_EL_METHOD(35,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(35)
                TSS_ELECTRODE_SLEWRATE_SET(35);
              #else
                #warning "TSS - The electrode E35 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 36
            #if TSS_DETECT_EL_METHOD(36,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(36)
                TSS_ELECTRODE_SLEWRATE_SET(36);
              #else
                #warning "TSS - The electrode E36 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 37
            #if TSS_DETECT_EL_METHOD(37,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(37)
                TSS_ELECTRODE_SLEWRATE_SET(37);
              #else
                #warning "TSS - The electrode E37 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 38
            #if TSS_DETECT_EL_METHOD(38,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(38)
                TSS_ELECTRODE_SLEWRATE_SET(38);
              #else
                #warning "TSS - The electrode E38 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 39
            #if TSS_DETECT_EL_METHOD(39,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(39)
                TSS_ELECTRODE_SLEWRATE_SET(39);
              #else
                #warning "TSS - The electrode E39 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 40
            #if TSS_DETECT_EL_METHOD(40,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(40)
                TSS_ELECTRODE_SLEWRATE_SET(40);
              #else
                #warning "TSS - The electrode E40 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 41
            #if TSS_DETECT_EL_METHOD(41,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(41)
                TSS_ELECTRODE_SLEWRATE_SET(41);
              #else
                #warning "TSS - The electrode E41 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 42
            #if TSS_DETECT_EL_METHOD(42,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(42)
                TSS_ELECTRODE_SLEWRATE_SET(42);
              #else
                #warning "TSS - The electrode E42 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 43
            #if TSS_DETECT_EL_METHOD(43,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(43)
                TSS_ELECTRODE_SLEWRATE_SET(43);
              #else
                #warning "TSS - The electrode E43 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 44
            #if TSS_DETECT_EL_METHOD(44,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(44)
                TSS_ELECTRODE_SLEWRATE_SET(44);
              #else
                #warning "TSS - The electrode E44 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 45
            #if TSS_DETECT_EL_METHOD(45,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(45)
                TSS_ELECTRODE_SLEWRATE_SET(45);
              #else
                #warning "TSS - The electrode E45 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 46
            #if TSS_DETECT_EL_METHOD(46,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(46)
                TSS_ELECTRODE_SLEWRATE_SET(46);
              #else
                #warning "TSS - The electrode E46 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 47
            #if TSS_DETECT_EL_METHOD(47,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(47)
                TSS_ELECTRODE_SLEWRATE_SET(47);
              #else
                #warning "TSS - The electrode E47 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 48
            #if TSS_DETECT_EL_METHOD(48,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(48)
                TSS_ELECTRODE_SLEWRATE_SET(48);
              #else
                #warning "TSS - The electrode E48 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 49
            #if TSS_DETECT_EL_METHOD(49,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(49)
                TSS_ELECTRODE_SLEWRATE_SET(49);
              #else
                #warning "TSS - The electrode E49 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 50
            #if TSS_DETECT_EL_METHOD(50,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(50)
                TSS_ELECTRODE_SLEWRATE_SET(50);
              #else
                #warning "TSS - The electrode E50 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 51
            #if TSS_DETECT_EL_METHOD(51,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(51)
                TSS_ELECTRODE_SLEWRATE_SET(51);
              #else
                #warning "TSS - The electrode E51 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 52
            #if TSS_DETECT_EL_METHOD(52,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(52)
                TSS_ELECTRODE_SLEWRATE_SET(52);
              #else
                #warning "TSS - The electrode E52 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 53
            #if TSS_DETECT_EL_METHOD(53,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(53)
                TSS_ELECTRODE_SLEWRATE_SET(53);
              #else
                #warning "TSS - The electrode E53 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 54
            #if TSS_DETECT_EL_METHOD(54,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(54)
                TSS_ELECTRODE_SLEWRATE_SET(54);
              #else
                #warning "TSS - The electrode E54 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 55
            #if TSS_DETECT_EL_METHOD(55,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(55)
                TSS_ELECTRODE_SLEWRATE_SET(55);
              #else
                #warning "TSS - The electrode E55 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 56
            #if TSS_DETECT_EL_METHOD(56,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(56)
                TSS_ELECTRODE_SLEWRATE_SET(56);
              #else
                #warning "TSS - The electrode E56 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 57
            #if TSS_DETECT_EL_METHOD(57,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(57)
                TSS_ELECTRODE_SLEWRATE_SET(57);
              #else
                #warning "TSS - The electrode E57 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 58
            #if TSS_DETECT_EL_METHOD(58,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(58)
                TSS_ELECTRODE_SLEWRATE_SET(58);
              #else
                #warning "TSS - The electrode E58 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 59
            #if TSS_DETECT_EL_METHOD(59,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(59)
                TSS_ELECTRODE_SLEWRATE_SET(59);
              #else
                #warning "TSS - The electrode E59 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 60
            #if TSS_DETECT_EL_METHOD(60,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(60)
                TSS_ELECTRODE_SLEWRATE_SET(60);
              #else
                #warning "TSS - The electrode E60 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 61
            #if TSS_DETECT_EL_METHOD(61,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(61)
                TSS_ELECTRODE_SLEWRATE_SET(61);
              #else
                #warning "TSS - The electrode E61 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 62
            #if TSS_DETECT_EL_METHOD(62,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(62)
                TSS_ELECTRODE_SLEWRATE_SET(62);
              #else
                #warning "TSS - The electrode E62 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif
          #if TSS_N_ELECTRODES > 63
            #if TSS_DETECT_EL_METHOD(63,GPIO)
              #if TSS_ELECTRODE_SLEWRATE_CHECK(63)
                TSS_ELECTRODE_SLEWRATE_SET(63);
              #else
                #warning "TSS - The electrode E63 does not provide SLEWRATE feature"
              #endif
            #endif
          #endif

          return u8result;
        }
      #endif

      #if TSS_USE_NOISE_AMPLITUDE_FILTER
        /***************************************************************************//*!
        *
        * @brief  Initializes the Noise Amplitude Filter variables
        *
        * @param  Nothing
        *
        * @return Status Code
        *
        * @remarks
        *
        ****************************************************************************/
        void GPIO_NoiseAmplitudeFilterInit(void)
        {
          uint8_t u8counter;

          for (u8counter = 0; u8counter < TSS_N_ELECTRODES; u8counter++)
          {
            /* Do except TSI module since it does not use Amplitude filter */
            if (TSS_CONVERT_MODULE_ID_TO_METHOD_ID(((TSS_GENERIC_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8counter]))->gen_cu8ModuleID) == TSS_GET_METHOD_ID(GPIO))
            {
              /* Set default value */
              ((TSS_GENERIC_ELECTRODE_RAMDATA *) (tss_acp8ElectrodeRAMData[u8counter]))->gen_u16AmplitudeFilterBase = 0u;
            }
          }
        }
      #endif

      #ifdef TSS_ONPROXIMITY_CALLBACK
        /***************************************************************************//*!
        *
        * @brief  Initializes the Proximity function
        *
        * @param  Nothing
        *
        * @return Status Code
        *
        * @remarks
        *
        ****************************************************************************/
        void GPIO_ProximityInit(void)
        {
          uint8_t u8counter;

          if (tss_CSSys.SystemConfig.ProximityEn)
          {
            for (u8counter = 0; u8counter < TSS_N_ELECTRODES; u8counter++)
            {
              /* Do except TSI module since it does not use Amplitude filter */
              if (TSS_CONVERT_MODULE_ID_TO_METHOD_ID(((TSS_GENERIC_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8counter]))->gen_cu8ModuleID) == TSS_GET_METHOD_ID(GPIO))
              {
                /* Measure signal on Proximity Electrode */
                if (u8counter == tss_CSSys.LowPowerElectrode)
                {
                  (void) UNI_SampleElectrode(u8counter, TSS_SAMPLE_COMMAND_PROCESS);
                }
              }
            }
          }
        }
      #endif

      /***************************************************************************//*!
      *
      * @brief  Initializes the modules for the sensing of the electrodes
      *
      * @param  Nothing
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t GPIO_SensorInit(void)
      {
        uint8_t u8result = TSS_INIT_STATUS_OK;

        #if TSS_USE_DEFAULT_ELECTRODE_LEVEL_LOW
          GPIO_ElectrodesSetStateLow();
        #else
          GPIO_ElectrodesSetStateHigh();
        #endif
        #if TSS_USE_GPIO_STRENGTH
          (void) GPIO_SetPinStrength();
        #endif
        #if TSS_USE_GPIO_SLEW_RATE
          (void) GPIO_SetPinSlewRate();
        #endif
        #if TSS_USE_NOISE_AMPLITUDE_FILTER
          (void) GPIO_NoiseAmplitudeFilterInit();
        #endif
        #ifdef TSS_ONPROXIMITY_CALLBACK
          (void) GPIO_ProximityInit();
        #endif

        return u8result;
      }

    #else /* TSS_USE_SIMPLE_LOW_LEVEL == 0 */

      /************************** Prototypes ***************************/

      uint8_t GPIO_MethodControl(uint8_t u8ElNum, uint8_t u8Command);

      /**********************  Modules definition **********************/
      #if (TSS_GPIO_VERSION == 2)
        #if TSS_DETECT_PORT_METHOD(A,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTA_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTAD, (uint8_t*) &PORT_PTAOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(B,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTB_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTBD, (uint8_t*) &PORT_PTBOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(C,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTC_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTCD, (uint8_t*) &PORT_PTCOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(D,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTD_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTDD, (uint8_t*) &PORT_PTDOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(E,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTE_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTED, (uint8_t*) &PORT_PTEOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(F,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTF_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTFD, (uint8_t*) &PORT_PTFOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(G,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTG_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTGD, (uint8_t*) &PORT_PTGOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(H,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTH_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTHD, (uint8_t*) &PORT_PTHOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(I,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTI_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTID, (uint8_t*) &PORT_PTIOE};
        #endif
        #if TSS_DETECT_PORT_METHOD(J,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTJ_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PORT_PTJD, (uint8_t*) &PORT_PTJOE};
        #endif
      #else /* (TSS_GPIO_VERSION == 1) */
        #if TSS_DETECT_PORT_METHOD(A,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTA_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTAD};
        #endif
        #if TSS_DETECT_PORT_METHOD(B,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTB_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTBD};
        #endif
        #if TSS_DETECT_PORT_METHOD(C,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTC_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTCD};
        #endif
        #if TSS_DETECT_PORT_METHOD(D,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTD_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTDD};
        #endif
        #if TSS_DETECT_PORT_METHOD(E,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTE_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTED};
        #endif
        #if TSS_DETECT_PORT_METHOD(F,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTF_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTFD};
        #endif
        #if TSS_DETECT_PORT_METHOD(G,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTG_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTGD};
        #endif
        #if TSS_DETECT_PORT_METHOD(H,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTH_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTHD};
        #endif
        #if TSS_DETECT_PORT_METHOD(I,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTI_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTID};
        #endif
        #if TSS_DETECT_PORT_METHOD(J,GPIO)
          const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTJ_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) &PTJD};
        #endif
      #endif /* End of (TSS_GPIO_VERSION == 1) */

      #include "TSS_SensorGPIO_def.h" /* ROMDATA and RAMDATA GPIO definitions */

      /***************************************************************************//*!
      *
      * @brief  Initializes the modules for the sensing of the electrodes
      *
      * @param  Nothing
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t GPIO_MethodControl(uint8_t u8ElNum, uint8_t u8Command)
      {
        uint8_t u8result = TSS_INIT_STATUS_OK;
        uint8_t u8ElCounter;

        TSS_GPIO_METHOD_ROMDATA *psMethodROMDataStruct;
        TSS_GPIO_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;

        TSS_TPMFTM_MemMapPtr psTimerStruct;
        #if (TSS_GPIO_VERSION == 2)
          uint8_t *psPort;
          uint8_t *psPortOE;
          uint8_t *psPortIE;
        #else /* (TSS_GPIO_VERSION == 1) */
          TSS_GPIO_MemMapPtr psPort;
        #endif

        /* Pointers Decoding */
        psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElNum]);
        psMethodROMDataStruct = (TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu8MethodROMData);

        psTimerStruct = (TSS_TPMFTM_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu8Timer);
        #if (TSS_GPIO_VERSION == 2)
          psPort = (uint8_t *) (psMethodROMDataStruct->gpio_cpsu8Port);
          psPortOE = (uint8_t *) (psMethodROMDataStruct->gpio_cpsu8PortControl);
          psPortIE = (uint8_t *) (psPortOE + 8u);
        #else /* (TSS_GPIO_VERSION == 1) */
          psPort = (TSS_GPIO_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu8Port);
        #endif

        /************* Do GPIO Init *******************/
        if ((u8Command == TSS_INIT_COMMAND_INIT_MODULES) || (u8Command == TSS_INIT_COMMAND_ENABLE_ELECTRODES) || (u8Command == TSS_INIT_COMMAND_RECALIBRATE))
        {
          /* HW Timer Init */
          psTimerStruct->SC = 0u; /* Reset Timer */
          psTimerStruct->CNT.Word = 0u; /* Reset Counter */
          psTimerStruct->SC |= TSS_HW_TPMFTM_IE; /* Enable Interrupt */
          if (tss_CSSys.SystemConfig.ProximityEn)
          {
            psTimerStruct->SC |= (TSS_SENSOR_PROX_PRESCALER & TSS_HW_TPMFTM_PRESCALE_MASK); /* Set Prescaler */
            psTimerStruct->MOD.Word = TSS_SENSOR_PROX_TIMEOUT; /* Set MOD Register */
          }
          else
          {
            psTimerStruct->SC |= (TSS_SENSOR_PRESCALER & TSS_HW_TPMFTM_PRESCALE_MASK); /* Set Prescaler */
            psTimerStruct->MOD.Word = TSS_SENSOR_TIMEOUT; /* Set MOD Register */
          }
          /* Find all with the same module */
          for(u8ElCounter = 0; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++)
          {
            if ((TSS_GENERIC_METHOD_ROMDATA *) psMethodROMDataStruct == (TSS_GENERIC_METHOD_ROMDATA *) (((TSS_GENERIC_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->gen_cpsMethodROMData))
            {
              /* Handle only enabled electrode */
              if ((tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u))) || ((u8ElCounter == tss_CSSys.LowPowerElectrode) && (tss_CSSys.SystemConfig.ProximityEn)))
              {
                /* Noise Amplitude Filter Initialization */
                #if TSS_USE_NOISE_AMPLITUDE_FILTER
                  ((TSS_GPIO_ELECTRODE_RAMDATA *) (tss_acp8ElectrodeRAMData[u8ElCounter]))->gpio_u16AmplitudeFilterBase = 0u;
                #endif

                /* Calculate Pointer to the Module */
                psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]);
                #if (TSS_GPIO_VERSION == 2)
                  /* Set PIN Strength if enabled */
                  #if TSS_USE_GPIO_STRENGTH
                    #warning "TSS - GPIO pin Strength function is not available on the MCU"
                  #endif
                  /* Set PIN SlewRate if enabled */
                  #if TSS_USE_GPIO_SLEW_RATE
                    #warning "TSS - GPIO pin SlewRate function is not available on the MCU"
                  #endif
                  /* Set Default PIN State */
                  *psPortIE &= ~(1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set PIN to OUTPUT */
                  *psPortOE |= (1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set PIN to OUTPUT */
                  #if TSS_USE_DEFAULT_ELECTRODE_LEVEL_LOW
                    *psPort &= ~(1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set LOG0 to OUTPUT pin */
                  #else
                    *psPort |= (1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set LOG1 to OUTPUT pin */
                  #endif
                #else /*(TSS_GPIO_VERSION == 1)*/
                  /* Set PIN Strength if enabled */
                  #if TSS_USE_GPIO_STRENGTH
                    #warning "TSS - GPIO pin Strength function is not available on the MCU"
                  #endif
                  /* Set PIN SlewRate if enabled */
                  #if TSS_USE_GPIO_SLEW_RATE
                    #warning "TSS - GPIO pin SlewRate function is not available on the MCU"
                  #endif
                  /* Only for enabled electrodes */
                  if (tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter  & 0x07u)))
                  {
                    /* Set Default PIN State */
                    psPort->DD |= (1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set PIN to OUTPUT */
                    #if TSS_USE_DEFAULT_ELECTRODE_LEVEL_LOW
                      psPort->D &= ~(1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set LOG0 to OUTPUT pin */
                    #else
                      psPort->D |= (1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set LOG1 to OUTPUT pin */
                    #endif
                  }
                #endif /*End of (TSS_GPIO_VERSION == 1)*/
                /* Measure signal on Proximity Electrode */
                if ((u8ElCounter == tss_CSSys.LowPowerElectrode) && (tss_CSSys.SystemConfig.ProximityEn))
                {
                  (void) GPIO_SampleElectrode(u8ElCounter, TSS_SAMPLE_COMMAND_PROCESS);
                }
              }
            }
          }
        }

        /* Exit */
        return u8result;
      }

      /***************************************************************************//*!
      *
      * @brief  Performs a valid reading of one electrode stores the timer values
      *         and returns a status code
      *
      * @param  u8ElecNum Electrode number to be scanned
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t GPIO_SampleElectrode(uint8_t u8ElecNum, uint8_t u8Command)
      {
        uint8_t u8NSamp;
        uint8_t u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;   /* Sets default status */
        uint16_t u16CapSubSample;
        uint8_t u8FaultCnt;
        TSS_GPIO_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
        TSS_GPIO_METHOD_ROMDATA *psMethodROMDataStruct;
        TSS_TPMFTM_MemMapPtr psTimerStruct;
        #if TSS_USE_NOISE_AMPLITUDE_FILTER
          int8_t i8AmplitudeFilterDeltaLimitTemp;
          TSS_GPIO_ELECTRODE_RAMDATA *psElectrodeRAMDataStruct;
        #endif

        switch (u8Command)
        {
          case TSS_SAMPLE_COMMAND_RESTART:
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
          break;
          case TSS_SAMPLE_COMMAND_PROCESS:
            /* Pointers Decoding */
            psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElecNum]);
            psMethodROMDataStruct = (TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu8MethodROMData);
            #if TSS_USE_NOISE_AMPLITUDE_FILTER
              psElectrodeRAMDataStruct = (TSS_GPIO_ELECTRODE_RAMDATA *) (tss_acp8ElectrodeRAMData[u8ElecNum]);
            #endif

            psTimerStruct = (TSS_TPMFTM_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu8Timer);
            tss_psElectrodeTimertAdr = (uint8_t *) psTimerStruct;
            tss_pu8ElectrodePortAdr = (uint8_t *) (psMethodROMDataStruct->gpio_cpsu8Port);
            #if (TSS_GPIO_VERSION == 2)
              tss_pu8ElectrodePortControlAdr = (uint8_t *) (psMethodROMDataStruct->gpio_cpsu8PortControl);
            #endif
            tss_u8ElectrodePortMask = (uint8_t) (1u << psElectrodeROMDataStruct->gpio_cu8PortBit);

            /* Initialization of variables */
            tss_u8HWTimerFlag = 0u;
            tss_u16CapSample = 0u;
            u8FaultCnt = 0u;
            u8NSamp = tss_CSSys.NSamples;

            #if TSS_USE_NOISE_AMPLITUDE_FILTER
              if (psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase == 0u)
              {
                i8AmplitudeFilterDeltaLimitTemp = 127;
              }
              else
              {
                i8AmplitudeFilterDeltaLimitTemp = psElectrodeROMDataStruct->gpio_ci8AmplitudeFilterDeltaLimit;
              }
            #endif

            /* Main oversampling measurement loop */
            while(u8NSamp)
            {
              psTimerStruct->SC &= TSS_HW_TPMFTM_OFF; /* Stop Timer */
              psTimerStruct->CNT.Word = 0u; /* Reset Counter i.e. Reset Timer*/

              TSS_CLEAR_SAMPLE_INTERRUPTED_FLAG();        /* Clears the interrupt sample flag */

              u16CapSubSample = (psElectrodeROMDataStruct->gpio_fSampleElectrodeLow)(); /* Samples one electrode */

              if(!TSS_FAULT_DETECTED)        /* Verifies that no fault has occur */
              {
                if(!TSS_SAMPLE_INTERRUPTED)  /* Verifies that the sample has not been inturrupted*/
                {
                  #if TSS_USE_NOISE_AMPLITUDE_FILTER
                    if (TSS_u16NoiseAmplitudeFilter(u16CapSubSample, &(psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase), i8AmplitudeFilterDeltaLimitTemp))
                    {
                      u8FaultCnt++;
                    }
                    else
                    {
                      tss_u16CapSample += u16CapSubSample;
                      u8NSamp--;
                      u8FaultCnt = 0u;  /* Restart Fault counter due long cyclic interrupts */
                    }
                  #else
                    tss_u16CapSample += u16CapSubSample;
                    u8NSamp--;
                    u8FaultCnt = 0u;  /* Restart Fault counter due long cyclic interrupts */
                  #endif
                }
                else
                {
                  /* Do nothing if interrupt occured and do sample again in next loop*/
                  u8FaultCnt++;
                }
              }
              else
              {
                /* Set PIN to OUTPUT LOW State */
                TSS_SET_ELECTRODE_OUTPUT_LOW();  /* Sets the electrode to OUTPUT LOW default value bacause short was detected there */
                u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT;  /* If a fault has occurred sets the return status code error */
                u8NSamp = 0u;
              }

              if(u8FaultCnt >= TSS_FAULT_TIMEOUT)
              {
                u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT; /* If a fault has occurred sets the return status code error */
                u8NSamp = 0u;
              }
            }

            if ((tss_u16CapSample < TSS_KEYDETECT_CAP_LOWER_LIMIT) && (u8ElectrodeStatus != TSS_SAMPLE_ERROR_CHARGE_TIMEOUT))
            {
              u8ElectrodeStatus = TSS_SAMPLE_ERROR_SMALL_CAP;
            }

            #if TSS_USE_NOISE_AMPLITUDE_FILTER
              if (u8ElectrodeStatus == TSS_SAMPLE_STATUS_OK)
              {
                if (psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase == 0u)
                {
                  psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase = (uint16_t) (tss_u16CapSample / tss_CSSys.NSamples);
                }
              }
              else
              {
                psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase = 0u; /* Initiate Amplitude Filter Base refresh */
              }
            #endif
          break;
          case TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE:
            /* Return the same electrode number because there is no more electrodes in the module */
            u8ElectrodeStatus = u8ElecNum;
          break;
          case TSS_SAMPLE_COMMAND_RECALIB:
            /* Do Nothing because the Command is no relevant for GPIO Method */
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
          break;
          case TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG:
            u8ElectrodeStatus = TSS_SAMPLE_ERROR_RESULT_NA;
          break;
          case TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG:
            u8ElectrodeStatus = TSS_SAMPLE_ERROR_RESULT_NA;
          break;
        }

        return u8ElectrodeStatus;  /* Return status code */
      }

    #endif /* End of TSS_USE_SIMPLE_LOW_LEVEL == 0 */

  #elif TSS_KINETIS_MCU

    /************************** Prototypes ***************************/

    uint8_t GPIO_MethodControl(uint8_t u8ElNum, uint8_t u8Command);

    /**********************  Modules definition **********************/
    #if (TSS_GPIO_VERSION == 3)
      #if TSS_DETECT_PORT_METHOD(A,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTA_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTA_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(B,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTB_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTB_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(C,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTC_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTC_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(D,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTD_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTD_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(E,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTE_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTE_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(F,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTF_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTF_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(G,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTG_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTG_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(H,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTH_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTH_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(I,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTI_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTI_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(J,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTJ_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PTJ_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
    #else /* (TSS_GPIO_VERSION == 1) */
      #if TSS_DETECT_PORT_METHOD(A,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTA_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTA_BASE_PTR, (uint32_t*) PTA_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(B,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTB_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTB_BASE_PTR, (uint32_t*) PTB_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(C,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTC_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTC_BASE_PTR, (uint32_t*) PTC_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(D,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTD_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTD_BASE_PTR, (uint32_t*) PTD_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(E,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTE_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTE_BASE_PTR, (uint32_t*) PTE_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(F,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTF_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTF_BASE_PTR, (uint32_t*) PTF_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(G,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTG_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTG_BASE_PTR, (uint32_t*) PTG_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(H,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTH_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTH_BASE_PTR, (uint32_t*) PTH_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(I,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTI_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTI_BASE_PTR, (uint32_t*) PTI_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
      #if TSS_DETECT_PORT_METHOD(J,GPIO)
        const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTJ_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint32_t*) TSS_HW_TIMER_PTR, (uint32_t*) PORTJ_BASE_PTR, (uint32_t*) PTJ_BASE_PTR, TSS_HW_TIMER_VECTOR-16};
      #endif
    #endif    

    #include "TSS_SensorGPIO_def.h" /* ROMDATA and RAMDATA GPIO definitions */

    /***************************************************************************//*!
    *
    * @brief  Initializes the modules for the sensing of the electrodes
    *
    * @param  Nothing
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t GPIO_MethodControl(uint8_t u8ElNum, uint8_t u8Command)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      uint8_t u8ElCounter;

      TSS_GPIO_METHOD_ROMDATA *psMethodROMDataStruct;
      TSS_GPIO_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;

      TSS_TPMFTM_MemMapPtr psTimerStruct;
      #if (TSS_GPIO_VERSION == 1)
        PORT_MemMapPtr psPortStruct;
      #endif  
      RGPIO_MemMapPtr psRGPIOStruct;
      NVIC_MemMapPtr psNVICStruct = NVIC_BASE_PTR;

      /* Pointers Decoding */
      psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElNum]);
      psMethodROMDataStruct = (TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu32MethodROMData);

      psTimerStruct = (TSS_TPMFTM_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu32Timer);
      #if (TSS_GPIO_VERSION == 1)
        psPortStruct = (PORT_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu32Port);
      #endif  
      psRGPIOStruct = (RGPIO_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu32RGPIO);

      /************* Do GPIO Init *******************/
      if ((u8Command == TSS_INIT_COMMAND_INIT_MODULES) || (u8Command == TSS_INIT_COMMAND_ENABLE_ELECTRODES) || (u8Command == TSS_INIT_COMMAND_RECALIBRATE))
      {
        /* HW Timer Init */
        psTimerStruct->SC = 0u; /* Reset Timer */
        psTimerStruct->CNT = 0u; /* Reset Counter */
        psTimerStruct->SC |= TSS_HW_TPMFTM_IE; /* Enable Interrupt */
        if (tss_CSSys.SystemConfig.ProximityEn)
        {
          psTimerStruct->SC |= (TSS_SENSOR_PROX_PRESCALER & TSS_HW_TPMFTM_PRESCALE_MASK); /* Set Prescaler */
          psTimerStruct->MOD = TSS_SENSOR_PROX_TIMEOUT; /* Set MOD Register */
        }
        else
        {
          psTimerStruct->SC |= (TSS_SENSOR_PRESCALER & TSS_HW_TPMFTM_PRESCALE_MASK); /* Set Prescaler */
          psTimerStruct->MOD = TSS_SENSOR_TIMEOUT; /* Set MOD Register */
        }
        /* Enable HW Timer Interrupt */
        #if defined(NVICISER1)
          psNVICStruct->ISER[psMethodROMDataStruct->gpio_u8TimerIRQNum >> 5u] = 1 << (psMethodROMDataStruct->gpio_u8TimerIRQNum  & 0x1Fu);
        #else
          psNVICStruct->ICPR |= (1u << (psMethodROMDataStruct->gpio_u8TimerIRQNum & 0x1Fu));
          psNVICStruct->ISER |= (1u << (psMethodROMDataStruct->gpio_u8TimerIRQNum & 0x1Fu));
        #endif

        for(u8ElCounter = 0; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++) /* Find all with the same module */
        {
          if ((uint32_t**) psMethodROMDataStruct == (uint32_t **) ((uint32_t *)tss_acp8ElectrodeROMData[u8ElCounter])[0])
          {
            /* Handle only enabled electrode */
            if ((tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u))) || ((u8ElCounter == tss_CSSys.LowPowerElectrode) && (tss_CSSys.SystemConfig.ProximityEn)))
            {
              /* Noise Amplitude Filter Initialization */
              #if TSS_USE_NOISE_AMPLITUDE_FILTER
                ((TSS_GPIO_ELECTRODE_RAMDATA *) (tss_acp8ElectrodeRAMData[u8ElCounter]))->gpio_u16AmplitudeFilterBase = 0u;
              #endif

              /* Calculate Pointer to the Module */
              psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]);
              
              #if (TSS_GPIO_VERSION == 3)
                #if TSS_USE_GPIO_STRENGTH
                  #error "TSS - GPIO Strength function is not supported on the MCU"
                #endif
                #if TSS_USE_GPIO_SLEW_RATE
                  #error "TSS - GPIO Slew Rate function is not supported on the MCU"
                #endif
              #else
                /* Init GPIO settings */
                psPortStruct->PCR[psElectrodeROMDataStruct->gpio_cu32PortBit] = 0u; /* Erase PCR */
                /* Set PIN as GPIO */
                psPortStruct->PCR[psElectrodeROMDataStruct->gpio_cu32PortBit] |= PORT_PCR_MUX(0x01u); /* Set GPIO function for PIN */
                /* Set PIN Strength if enabled */
                #if TSS_USE_GPIO_STRENGTH
                  psPortStruct->PCR[psElectrodeROMDataStruct->gpio_cu32PortBit] |= PORT_PCR_DSE_MASK; /* Set PIN Strength*/
                #endif
                /* Set PIN SlewRate if enabled */
                #if TSS_USE_GPIO_SLEW_RATE
                  psPortStruct->PCR[psElectrodeROMDataStruct->gpio_cu32PortBit] |= PORT_PCR_SRE_MASK; /* Set PIN SlewRate*/
                #endif
              #endif  
              /* Set Default PIN State */
              psRGPIOStruct->PDDR |= (1 << psElectrodeROMDataStruct->gpio_cu32PortBit); /* Set PIN to OUTPUT */
              #if TSS_USE_DEFAULT_ELECTRODE_LEVEL_LOW
                psRGPIOStruct->PCOR = (1 << psElectrodeROMDataStruct->gpio_cu32PortBit);  /* Set LOG0 to OUTPUT pin */
              #else
                psRGPIOStruct->PSOR = (1 << psElectrodeROMDataStruct->gpio_cu32PortBit);  /* Set LOG1 to OUTPUT pin */
              #endif
              /* Measure signal on Proximity Electrode */
              if ((u8ElCounter == tss_CSSys.LowPowerElectrode) && (tss_CSSys.SystemConfig.ProximityEn))
              {
                (void) GPIO_SampleElectrode(u8ElCounter, TSS_SAMPLE_COMMAND_PROCESS);
              }
            }
          }
        }
      }
      #if (TSS_USE_NOISE_MODE == 1)
        else if (u8Command == TSS_INIT_COMMAND_MODE_GET)
        {
          /* Return GPIO as active mode */
          u8result = TSS_MODE_GPIO;
        }
      #endif

      /* Exit */
      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Performs a valid reading of one electrode stores the timer values
    *         and returns a status code
    *
    * @param  u8ElecNum Electrode number to be scanned
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t GPIO_SampleElectrode(uint8_t u8ElecNum, uint8_t u8Command)
    {
      uint8_t u8NSamp;
      uint8_t u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;   /* Sets default status */
      uint16_t u16CapSubSample;
      uint8_t u8FaultCnt;
      TSS_GPIO_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
      TSS_TPMFTM_MemMapPtr psTimerStruct;
      RGPIO_MemMapPtr psRGPIOStruct;
      #if TSS_USE_NOISE_AMPLITUDE_FILTER
        int8_t i8AmplitudeFilterDeltaLimitTemp;
        TSS_GPIO_ELECTRODE_RAMDATA *psElectrodeRAMDataStruct;
      #endif

      switch(u8Command)
      {
        case TSS_SAMPLE_COMMAND_RESTART:
          u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
        break;
        case TSS_SAMPLE_COMMAND_PROCESS:
          /* Pointers Decoding */
          psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElecNum]);
          psTimerStruct = (TSS_TPMFTM_MemMapPtr) (((TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu32MethodROMData))->gpio_cpsu32Timer);
          psRGPIOStruct = (RGPIO_MemMapPtr) (((TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu32MethodROMData))->gpio_cpsu32RGPIO);
          #if TSS_USE_NOISE_AMPLITUDE_FILTER
            psElectrodeRAMDataStruct = (TSS_GPIO_ELECTRODE_RAMDATA *) (tss_acp8ElectrodeRAMData[u8ElecNum]);
          #endif

          tss_psElectrodeTimertAdr = (uint32_t *) psTimerStruct;
          tss_psElectrodeRGPIOAdr = (uint32_t *) psRGPIOStruct;
          tss_u8ElectrodeBitNum = psElectrodeROMDataStruct->gpio_cu32PortBit;

          /* Initialization of variables */
          tss_u8HWTimerFlag = 0u;
          tss_u16CapSample = 0u;
          u8FaultCnt = 0u;
          u8NSamp = tss_CSSys.NSamples;

          #if TSS_USE_NOISE_AMPLITUDE_FILTER
            if (psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase == 0u)
            {
              i8AmplitudeFilterDeltaLimitTemp = 127;
            }
            else
            {
              i8AmplitudeFilterDeltaLimitTemp = psElectrodeROMDataStruct->gpio_ci8AmplitudeFilterDeltaLimit;
            }
          #endif

          /* Main oversampling measurement loop */
          while(u8NSamp)
          {
            psTimerStruct->SC &= TSS_HW_TPMFTM_OFF; /* Stop Timer */
            psTimerStruct->CNT = 0u; /* Reset Counter i.e. Reset Timer*/

            TSS_CLEAR_SAMPLE_INTERRUPTED_FLAG();  /* Clears the interrupt sample flag */

            u16CapSubSample = (psElectrodeROMDataStruct->gpio_fSampleElectrodeLow)();  /* Samples one electrode */

            if(!TSS_FAULT_DETECTED)          /* Verifies that no fault has occur */
            {
              if(!TSS_SAMPLE_INTERRUPTED)  /* Verifies that the sample has not been inturrupted*/
              {
                #if TSS_USE_NOISE_AMPLITUDE_FILTER
                  if (TSS_u16NoiseAmplitudeFilter(u16CapSubSample, &(psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase), i8AmplitudeFilterDeltaLimitTemp))
                  {
                    u8FaultCnt++;
                  }
                  else
                  {
                    tss_u16CapSample += u16CapSubSample;
                    u8NSamp--;
                    u8FaultCnt = 0u;  /* Restart Fault counter due long cyclic interrupts */
                  }

                #else
                  tss_u16CapSample += u16CapSubSample;
                  u8NSamp--;
                  u8FaultCnt = 0u;  /* Restart Fault counter due long cyclic interrupts */
                #endif
              }
              else
              {
                /* Do nothing if interrupt occured and do sample again in next loop*/
                u8FaultCnt++;
              }
            }
            else
            {
              /* Set PIN to OUTPUT LOW State */
              psRGPIOStruct->PDDR |= (1 << psElectrodeROMDataStruct->gpio_cu32PortBit); /* Set PIN to OUTPUT */
              psRGPIOStruct->PCOR = (1 << psElectrodeROMDataStruct->gpio_cu32PortBit);  /* Set LOG0 to OUTPUT pin */

              u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT; /* If a fault has occurred sets the return status code error */
              u8NSamp = 0u;
            }

            if(u8FaultCnt >= TSS_FAULT_TIMEOUT)
            {
              u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT; /* If a fault has occurred sets the return status code error */
              u8NSamp = 0u;
            }
          }

          if ((tss_u16CapSample < TSS_KEYDETECT_CAP_LOWER_LIMIT) && (u8ElectrodeStatus != TSS_SAMPLE_ERROR_CHARGE_TIMEOUT))
          {
            u8ElectrodeStatus = TSS_SAMPLE_ERROR_SMALL_CAP;
          }

          #if TSS_USE_NOISE_AMPLITUDE_FILTER
            if (u8ElectrodeStatus == TSS_SAMPLE_STATUS_OK)
            {
              if (psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase == 0u)
              {
                psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase = (uint16_t) (tss_u16CapSample / tss_CSSys.NSamples);
              }
            }
            else
            {
              psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase = 0u; /* Initiate Amplitude Filter Base refresh */
            }
          #endif
        break;
        case TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE:
          /* Return the same electrode number because there is no more electrodes in the module */
          u8ElectrodeStatus = u8ElecNum;
        break;
        case TSS_SAMPLE_COMMAND_RECALIB:
          /* Do Nothing because the Command is no relevant for GPIO Method */
          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
        case TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG:
          u8ElectrodeStatus = TSS_SAMPLE_ERROR_RESULT_NA;
        break;
        case TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG:
          u8ElectrodeStatus = TSS_SAMPLE_ERROR_RESULT_NA;
        break;
      #if (TSS_USE_NOISE_MODE == 1)
        case TSS_SAMPLE_COMMAND_GET_NOISE_VALUE:
          /* Noise value not available */
          u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
        break;
      #endif
      }

      return u8ElectrodeStatus;   /* Return status code */
    }

  #elif TSS_CFM_MCU

    /************************** Prototypes ***************************/

    uint8_t GPIO_MethodControl(uint8_t u8ElNum, uint8_t u8Command);

    /**********************  Modules definition **********************/

    #if TSS_DETECT_PORT_METHOD(A,GPIO)
      const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTA_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) PCTLA_BASE_PTR, (uint8_t*) PTA_BASE_PTR, (uint8_t*) &MXC_PTAPF1};
    #endif
    #if TSS_DETECT_PORT_METHOD(B,GPIO)
      const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTB_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) PCTLB_BASE_PTR, (uint8_t*) PTB_BASE_PTR, (uint8_t*) &MXC_PTBPF1};
    #endif
    #if TSS_DETECT_PORT_METHOD(C,GPIO)
      const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTC_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) PCTLC_BASE_PTR, (uint8_t*) PTC_BASE_PTR, (uint8_t*) &MXC_PTCPF1};
    #endif
    #if TSS_DETECT_PORT_METHOD(D,GPIO)
      const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTD_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) PCTLD_BASE_PTR, (uint8_t*) PTD_BASE_PTR, (uint8_t*) &MXC_PTDPF1};
    #endif
    #if TSS_DETECT_PORT_METHOD(E,GPIO)
      const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTE_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) PCTLE_BASE_PTR, (uint8_t*) PTE_BASE_PTR, (uint8_t*) &MXC_PTEPF1};
    #endif
    #if TSS_DETECT_PORT_METHOD(F,GPIO)
      const    TSS_GPIO_METHOD_ROMDATA TSS_GPIO_PORTF_METHOD_ROMDATA_CONTEXT = {GPIO_MethodControl, (uint8_t*) TSS_HW_TIMER_PTR, (uint8_t*) PCTLF_BASE_PTR, (uint8_t*) PTF_BASE_PTR, (uint8_t*) &MXC_PTFPF1};
    #endif

    #include "TSS_SensorGPIO_def.h" /* ROMDATA and RAMDATA GPIO definitions */

    /***************************************************************************//*!
    *
    * @brief  Initializes the modules for the sensing of the electrodes
    *
    * @param  Nothing
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t GPIO_MethodControl(uint8_t u8ElNum, uint8_t u8Command)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      uint8_t u8ElCounter;
      typedef uint8_t * TSS_MXC_MemMapPtr;

      TSS_GPIO_METHOD_ROMDATA *psMethodROMDataStruct;
      TSS_GPIO_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;

      TSS_TPMFTM_MemMapPtr psTimerStruct;
      PCTL_MemMapPtr psPCTLStruct;
      PT_MemMapPtr psPortStruct;
      TSS_MXC_MemMapPtr psMXCStruct;

      /* Pointers Decoding */
      psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElNum]);
      psMethodROMDataStruct = (TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu8MethodROMData);

      psTimerStruct = (TSS_TPMFTM_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu8Timer);
      psPCTLStruct = (PCTL_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu8PCTL);
      psPortStruct = (PT_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu8Port);
      psMXCStruct = (TSS_MXC_MemMapPtr) (psMethodROMDataStruct->gpio_cpsu8MXC);

      /************* Do GPIO Init *******************/
      if ((u8Command == TSS_INIT_COMMAND_INIT_MODULES) || (u8Command == TSS_INIT_COMMAND_ENABLE_ELECTRODES) || (u8Command == TSS_INIT_COMMAND_RECALIBRATE))
      {
        /* HW Timer Init */
        psTimerStruct->SC = 0u; /* Reset Timer */
        psTimerStruct->CNT.Word = 0u; /* Reset Counter */
        psTimerStruct->SC |= TSS_HW_TPMFTM_IE; /* Enable Interrupt */
        if (tss_CSSys.SystemConfig.ProximityEn)
        {
          psTimerStruct->SC |= (TSS_SENSOR_PROX_PRESCALER & TSS_HW_TPMFTM_PRESCALE_MASK); /* Set Prescaler */
          psTimerStruct->MOD.Word = TSS_SENSOR_PROX_TIMEOUT; /* Set MOD Register */
        }
        else
        {
          psTimerStruct->SC |= (TSS_SENSOR_PRESCALER & TSS_HW_TPMFTM_PRESCALE_MASK); /* Set Prescaler */
          psTimerStruct->MOD.Word = TSS_SENSOR_TIMEOUT; /* Set MOD Register */
        }
        /* Find all with the same module */
        for(u8ElCounter = 0; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++)
        {
          if ((uint32_t**) psMethodROMDataStruct == (uint32_t **) ((uint32_t *)tss_acp8ElectrodeROMData[u8ElCounter])[0])
          {
            /* Handle only enabled electrode */
            if ((tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter  & 0x07u))) || ((u8ElCounter == tss_CSSys.LowPowerElectrode) && (tss_CSSys.SystemConfig.ProximityEn)))
            {
              /* Noise Amplitude Filter Initialization */
              #if TSS_USE_NOISE_AMPLITUDE_FILTER
                ((TSS_GPIO_ELECTRODE_RAMDATA *) (tss_acp8ElectrodeRAMData[u8ElCounter]))->gpio_u16AmplitudeFilterBase = 0u;
              #endif

              /* Calculate Pointer to the Module */
              psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]);
              /* Init MXC settings */
              if (psElectrodeROMDataStruct->gpio_cu8PortBit & 0x01u)
              {
                /* Erase PCR */
                psMXCStruct[3u - (psElectrodeROMDataStruct->gpio_cu8PortBit >> 1u)] &= 0x0Fu;
                /* Set PIN as GPIO */
                psMXCStruct[3u - (psElectrodeROMDataStruct->gpio_cu8PortBit >> 1u)] |= 0x10u; /* Set GPIO function for PIN */
              }
              else
              {
                /* Erase PCR */
                psMXCStruct[3u - (psElectrodeROMDataStruct->gpio_cu8PortBit >> 1u)] &= 0xF0u; /* Erase PCR */
                /* Set PIN as GPIO */
                psMXCStruct[3u - (psElectrodeROMDataStruct->gpio_cu8PortBit >> 1u)] |= 0x01u; /* Set GPIO function for PIN */
              }
              /* Set PIN Strength if enabled */
              #if TSS_USE_GPIO_STRENGTH
                psPCTLStruct->DS |= (1u << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set PIN Strength*/
              #endif
              /* Set PIN SlewRate if enabled */
              #if TSS_USE_GPIO_SLEW_RATE
                psPCTLStruct->SRE |= (1u << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set PIN SlewRate*/
              #endif
              /* Set Default PIN State */
              psPortStruct->DD |= (1u << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set PIN to OUTPUT */
              #if TSS_USE_DEFAULT_ELECTRODE_LEVEL_LOW
                psPortStruct->D &= ~(1u << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set LOG0 to OUTPUT pin */
              #else
                psPortStruct->D |= (1u << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set LOG1 to OUTPUT pin */
              #endif
              /* Measure signal on Proximity Electrode */
              if ((u8ElCounter == tss_CSSys.LowPowerElectrode) && (tss_CSSys.SystemConfig.ProximityEn))
              {
                (void) GPIO_SampleElectrode(u8ElCounter, TSS_SAMPLE_COMMAND_PROCESS);
              }
            }
          }
        }
      }

      /* Exit */
      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Performs a valid reading of one electrode stores the timer values
    *         and returns a status code
    *
    * @param  u8ElecNum Electrode number to be scanned
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t GPIO_SampleElectrode(uint8_t u8ElecNum, uint8_t u8Command)
    {
      uint8_t u8NSamp;
      uint8_t u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;   /* Sets default status */
      uint16_t u16CapSubSample;
      uint8_t u8FaultCnt;
      TSS_GPIO_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
      TSS_TPMFTM_MemMapPtr psTimerStruct;
      PT_MemMapPtr psPortStruct;
      #if TSS_USE_NOISE_AMPLITUDE_FILTER
        int8_t i8AmplitudeFilterDeltaLimitTemp;
        TSS_GPIO_ELECTRODE_RAMDATA *psElectrodeRAMDataStruct;
      #endif

      switch (u8Command)
      {
        case TSS_SAMPLE_COMMAND_RESTART:
          u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
        break;
        case TSS_SAMPLE_COMMAND_PROCESS:
          /* Pointers Decoding */
          psElectrodeROMDataStruct = (TSS_GPIO_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElecNum]);
          psTimerStruct = (TSS_TPMFTM_MemMapPtr) (((TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu8MethodROMData))->gpio_cpsu8Timer);
          psPortStruct = (PT_MemMapPtr) (((TSS_GPIO_METHOD_ROMDATA *) (psElectrodeROMDataStruct->gpio_cpsu8MethodROMData))->gpio_cpsu8Port);
          #if TSS_USE_NOISE_AMPLITUDE_FILTER
            psElectrodeRAMDataStruct = (TSS_GPIO_ELECTRODE_RAMDATA *) (tss_acp8ElectrodeRAMData[u8ElecNum]);
          #endif

          tss_psElectrodeTimertAdr = (uint32_t *) psTimerStruct;
          tss_psElectrodePortAdr = (uint32_t *) psPortStruct;
          tss_u8ElectrodeBitNum = psElectrodeROMDataStruct->gpio_cu8PortBit;

          /* Initialization of variables */
          tss_u8HWTimerFlag = 0u;
          tss_u16CapSample = 0u;
          u8FaultCnt = 0u;
          u8NSamp = tss_CSSys.NSamples;

          #if TSS_USE_NOISE_AMPLITUDE_FILTER
            if (psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase == 0u) {
              i8AmplitudeFilterDeltaLimitTemp = 127;
            }
            else
            {
              i8AmplitudeFilterDeltaLimitTemp = psElectrodeROMDataStruct->gpio_ci8AmplitudeFilterDeltaLimit;
            }
          #endif

          /* Main oversampling measurement loop */
          while(u8NSamp)
          {
            psTimerStruct->SC &= TSS_HW_TPMFTM_OFF; /* Stop Timer */
            psTimerStruct->CNT.Word = 0u; /* Reset Counter i.e. Reset Timer*/

            TSS_CLEAR_SAMPLE_INTERRUPTED_FLAG();  /* Clears the interrupt sample flag */

            u16CapSubSample = (psElectrodeROMDataStruct->gpio_fSampleElectrodeLow)();    /* Samples one electrode */

            if(!TSS_FAULT_DETECTED)  /* Verifies that no fault has occur */
            {
              if(!TSS_SAMPLE_INTERRUPTED)  /* Verifies that the sample has not been inturrupted*/
              {
                #if TSS_USE_NOISE_AMPLITUDE_FILTER
                  if (TSS_u16NoiseAmplitudeFilter(u16CapSubSample, &(psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase), i8AmplitudeFilterDeltaLimitTemp))
                  {
                    u8FaultCnt++;
                  }
                  else
                  {
                    tss_u16CapSample += u16CapSubSample;
                    u8NSamp--;
                    u8FaultCnt = 0u;                 /* Restart Fault counter due long cyclic interrupts */
                  }
                #else
                  tss_u16CapSample += u16CapSubSample;
                  u8NSamp--;
                  u8FaultCnt = 0u;                   /* Restart Fault counter due long cyclic interrupts */
                #endif
              }
              else
              {
                /* Do nothing if interrupt occured and do sample again in next loop*/
                u8FaultCnt++;
              }
            }
            else
            {
              /* Set PIN to OUTPUT LOW State */
              psPortStruct->DD |= (1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set PIN to OUTPUT */
              psPortStruct->D &= ~(1 << psElectrodeROMDataStruct->gpio_cu8PortBit); /* Set LOG0 to OUTPUT pin */

              u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT; /* If a fault has occurred sets the return status code error */
              u8NSamp = 0u;
            }

            if(u8FaultCnt >= TSS_FAULT_TIMEOUT)
            {
              u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT; /* If a fault has occurred sets the return status code error */
              u8NSamp = 0u;
            }
          }

          if ((tss_u16CapSample < TSS_KEYDETECT_CAP_LOWER_LIMIT) && (u8ElectrodeStatus != TSS_SAMPLE_ERROR_CHARGE_TIMEOUT))
          {
            u8ElectrodeStatus = TSS_SAMPLE_ERROR_SMALL_CAP;
          }

          #if TSS_USE_NOISE_AMPLITUDE_FILTER
            if (u8ElectrodeStatus == TSS_SAMPLE_STATUS_OK)
            {
              if (psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase == 0u)
              {
                psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase = (uint16_t) (tss_u16CapSample / tss_CSSys.NSamples);
              }
            }
            else
            {
              psElectrodeRAMDataStruct->gpio_u16AmplitudeFilterBase = 0u; /* Initiate Amplitude Filter Base refresh */
            }
          #endif
        break;
        case TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE:
          /* Return the same electrode number because there is no more electrodes in the module */
          u8ElectrodeStatus = u8ElecNum;
        break;
        case TSS_SAMPLE_COMMAND_RECALIB:
          /* Do Nothing because the Command is no relevant for GPIO Method */
          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
        case TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG:
          u8ElectrodeStatus = TSS_SAMPLE_ERROR_RESULT_NA;
        break;
        case TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG:
          u8ElectrodeStatus = TSS_SAMPLE_ERROR_RESULT_NA;
        break;
      }

      return u8ElectrodeStatus;   /* Return status code */
    }
  #else /* End of TSS_CFM_MCU */
    #error "TSS - Not supported MCU used"
  #endif

#endif /* End of TSS_DETECT_METHOD(GPIO) */
