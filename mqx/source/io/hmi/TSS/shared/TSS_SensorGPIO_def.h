
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains GPIO definitions.
*
*
*END************************************************************************/
#ifndef TSS_SENSORTSI_DEF_H
#define TSS_SENSORTSI_DEF_H

#if TSS_DETECT_METHOD(GPIO)

  #if TSS_HCS08_MCU || TSS_CFV1_MCU

    #if TSS_USE_SIMPLE_LOW_LEVEL

      /***************** ROM & RAM Data definition ***********************/

      #if TSS_N_ELECTRODES > 0
        #if TSS_DETECT_EL_METHOD(0,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E0_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(0),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E0_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E0_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 1
        #if TSS_DETECT_EL_METHOD(1,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E1_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(1),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E1_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E1_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 2
        #if TSS_DETECT_EL_METHOD(2,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E2_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(2),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E2_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E2_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 3
        #if TSS_DETECT_EL_METHOD(3,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E3_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(3),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E3_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E3_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 4
        #if TSS_DETECT_EL_METHOD(4,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E4_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(4),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E4_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E4_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 5
        #if TSS_DETECT_EL_METHOD(5,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E5_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(5),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E5_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E5_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 6
        #if TSS_DETECT_EL_METHOD(6,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E6_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(6),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E6_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E6_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 7
        #if TSS_DETECT_EL_METHOD(7,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E7_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(7),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E7_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E7_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 8
        #if TSS_DETECT_EL_METHOD(8,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E8_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(8),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E8_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E8_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 9
        #if TSS_DETECT_EL_METHOD(9,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E9_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(9),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E9_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E9_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 10
        #if TSS_DETECT_EL_METHOD(10,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E10_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(10),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E10_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E10_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 11
        #if TSS_DETECT_EL_METHOD(11,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E11_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(11),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E11_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E11_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 12
        #if TSS_DETECT_EL_METHOD(12,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E12_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(12),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E12_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E12_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 13
        #if TSS_DETECT_EL_METHOD(13,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E13_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(13),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E13_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E13_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 14
        #if TSS_DETECT_EL_METHOD(14,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E14_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(14),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E14_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E14_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 15
        #if TSS_DETECT_EL_METHOD(15,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E15_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(15),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E15_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E15_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 16
        #if TSS_DETECT_EL_METHOD(16,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E16_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(16),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E16_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E16_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 17
        #if TSS_DETECT_EL_METHOD(17,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E17_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(17),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E17_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E17_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 18
        #if TSS_DETECT_EL_METHOD(18,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E18_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(18),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E18_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E18_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 19
        #if TSS_DETECT_EL_METHOD(19,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E19_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(19),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E19_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E19_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 20
        #if TSS_DETECT_EL_METHOD(20,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E20_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(20),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E20_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E20_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 21
        #if TSS_DETECT_EL_METHOD(21,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E21_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(21),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E21_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E21_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 22
        #if TSS_DETECT_EL_METHOD(22,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E22_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(22),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E22_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E22_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 23
        #if TSS_DETECT_EL_METHOD(23,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E23_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(23),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E23_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E23_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 24
        #if TSS_DETECT_EL_METHOD(24,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E24_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(24),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E24_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E24_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 25
        #if TSS_DETECT_EL_METHOD(25,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E25_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(25),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E25_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E25_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 26
        #if TSS_DETECT_EL_METHOD(26,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E26_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(26),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E26_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E26_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 27
        #if TSS_DETECT_EL_METHOD(27,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E27_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(27),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E27_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E27_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 28
        #if TSS_DETECT_EL_METHOD(28,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E28_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(28),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E28_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E28_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 29
        #if TSS_DETECT_EL_METHOD(29,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E29_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(29),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E29_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E29_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 30
        #if TSS_DETECT_EL_METHOD(30,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E30_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(30),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E30_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E30_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 31
        #if TSS_DETECT_EL_METHOD(31,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E31_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(31),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E31_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E31_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 32
        #if TSS_DETECT_EL_METHOD(32,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E32_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(32),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E32_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E32_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 33
        #if TSS_DETECT_EL_METHOD(33,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E33_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(33),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E33_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E33_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 34
        #if TSS_DETECT_EL_METHOD(34,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E34_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(34),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E34_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E34_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 35
        #if TSS_DETECT_EL_METHOD(35,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E35_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(35),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E35_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E35_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 36
        #if TSS_DETECT_EL_METHOD(36,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E36_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(36),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E36_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E36_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 37
        #if TSS_DETECT_EL_METHOD(37,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E37_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(37),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E37_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E37_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 38
        #if TSS_DETECT_EL_METHOD(38,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E38_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(38),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E38_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E38_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 39
        #if TSS_DETECT_EL_METHOD(39,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E39_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(39),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E39_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E39_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 40
        #if TSS_DETECT_EL_METHOD(40,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E40_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(40),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E40_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E40_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 41
        #if TSS_DETECT_EL_METHOD(41,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E41_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(41),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E41_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E41_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 42
        #if TSS_DETECT_EL_METHOD(42,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E42_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(42),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E42_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E42_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 43
        #if TSS_DETECT_EL_METHOD(43,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E43_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(43),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E43_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E43_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 44
        #if TSS_DETECT_EL_METHOD(44,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E44_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(44),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E44_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E44_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 45
        #if TSS_DETECT_EL_METHOD(45,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E45_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(45),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E45_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E45_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 46
        #if TSS_DETECT_EL_METHOD(46,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E46_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(46),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E46_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E46_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 47
        #if TSS_DETECT_EL_METHOD(47,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E47_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(47),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E47_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E47_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 48
        #if TSS_DETECT_EL_METHOD(48,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E48_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(48),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E48_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E48_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 49
        #if TSS_DETECT_EL_METHOD(49,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E49_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(49),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E49_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E49_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 50
        #if TSS_DETECT_EL_METHOD(50,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E50_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(50),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E50_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E50_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 51
        #if TSS_DETECT_EL_METHOD(51,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E51_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(51),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E51_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E51_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 52
        #if TSS_DETECT_EL_METHOD(52,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E52_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(52),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E52_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E52_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 53
        #if TSS_DETECT_EL_METHOD(53,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E53_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(53),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E53_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E53_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 54
        #if TSS_DETECT_EL_METHOD(54,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E54_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(54),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E54_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E54_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 55
        #if TSS_DETECT_EL_METHOD(55,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E55_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(55),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E55_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E55_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 56
        #if TSS_DETECT_EL_METHOD(56,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E56_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(56),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E56_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E56_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 57
        #if TSS_DETECT_EL_METHOD(57,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E57_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(57),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E57_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E57_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 58
        #if TSS_DETECT_EL_METHOD(58,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E58_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(58),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E58_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E58_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 59
        #if TSS_DETECT_EL_METHOD(59,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E59_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(59),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E59_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E59_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 60
        #if TSS_DETECT_EL_METHOD(60,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E60_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(60),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E60_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E60_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 61
        #if TSS_DETECT_EL_METHOD(61,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E61_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(61),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E61_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E61_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 62
        #if TSS_DETECT_EL_METHOD(62,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E62_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(62),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E62_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E62_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 63
        #if TSS_DETECT_EL_METHOD(63,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E63_ROMDATA_CONTEXT = { TSS_GET_EL_MODULE_ID(63),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E63_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E63_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

    #else /* TSS_USE_SIMPLE_LOW_LEVEL == 0 */

      /**************** Method Memory Data Decoding Macros ***************/

      #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_RESULT(memory,res)     TSS_GPIO_PORT##res##_METHOD_##memory##_CONTEXT
      #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_CONV(memory,text)      TSS_GET_GPIO_METHOD_DATA_CONTEXT_RESULT(memory,text)
      #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_TYPE(memory,elec)      TSS_GET_GPIO_METHOD_DATA_CONTEXT_CONV(memory,elec)

      #define TSS_GET_GPIO_METHOD_DATA_CONTEXT(memory,el)             TSS_GET_GPIO_METHOD_DATA_CONTEXT_TYPE(memory,TSS_E##el##_P)

      /***************** ROM & RAM Data definition ***********************/

      #if TSS_N_ELECTRODES > 0
        #if TSS_DETECT_EL_METHOD(0,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E0_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,0), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(0), TSS_ELECTRODE_BIT_NUM(0),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E0_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E0_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 1
        #if TSS_DETECT_EL_METHOD(1,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E1_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,1), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(1), TSS_ELECTRODE_BIT_NUM(1),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E1_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E1_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 2
        #if TSS_DETECT_EL_METHOD(2,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E2_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,2), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(2), TSS_ELECTRODE_BIT_NUM(2),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E2_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E2_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 3
        #if TSS_DETECT_EL_METHOD(3,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E3_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,3), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(3), TSS_ELECTRODE_BIT_NUM(3),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E3_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E3_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 4
        #if TSS_DETECT_EL_METHOD(4,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E4_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,4), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(4), TSS_ELECTRODE_BIT_NUM(4),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E4_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E4_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 5
        #if TSS_DETECT_EL_METHOD(5,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E5_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,5), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(5), TSS_ELECTRODE_BIT_NUM(5),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E5_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E5_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 6
        #if TSS_DETECT_EL_METHOD(6,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E6_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,6), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(6), TSS_ELECTRODE_BIT_NUM(6),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E6_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E6_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 7
        #if TSS_DETECT_EL_METHOD(7,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E7_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,7), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(7), TSS_ELECTRODE_BIT_NUM(7),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E7_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E7_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 8
        #if TSS_DETECT_EL_METHOD(8,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E8_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,8), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(8), TSS_ELECTRODE_BIT_NUM(8),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E8_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E8_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 9
        #if TSS_DETECT_EL_METHOD(9,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E9_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,9), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(9), TSS_ELECTRODE_BIT_NUM(9),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E9_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E9_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 10
        #if TSS_DETECT_EL_METHOD(10,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E10_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,10), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(10), TSS_ELECTRODE_BIT_NUM(10),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E10_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E10_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 11
        #if TSS_DETECT_EL_METHOD(11,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E11_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,11), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(11), TSS_ELECTRODE_BIT_NUM(11),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E11_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E11_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 12
        #if TSS_DETECT_EL_METHOD(12,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E12_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,12), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(12), TSS_ELECTRODE_BIT_NUM(12),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E12_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E12_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 13
        #if TSS_DETECT_EL_METHOD(13,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E13_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,13), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(13), TSS_ELECTRODE_BIT_NUM(13),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E13_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E13_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 14
        #if TSS_DETECT_EL_METHOD(14,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E14_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,14), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(14), TSS_ELECTRODE_BIT_NUM(14),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E14_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E14_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 15
        #if TSS_DETECT_EL_METHOD(15,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E15_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,15), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(15), TSS_ELECTRODE_BIT_NUM(15),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E15_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E15_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 16
        #if TSS_DETECT_EL_METHOD(16,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E16_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,16), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(16), TSS_ELECTRODE_BIT_NUM(16),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E16_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E16_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 17
        #if TSS_DETECT_EL_METHOD(17,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E17_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,17), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(17), TSS_ELECTRODE_BIT_NUM(17),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E17_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E17_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 18
        #if TSS_DETECT_EL_METHOD(18,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E18_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,18), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(18), TSS_ELECTRODE_BIT_NUM(18),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E18_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E18_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 19
        #if TSS_DETECT_EL_METHOD(19,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E19_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,19), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(19), TSS_ELECTRODE_BIT_NUM(19),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E19_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E19_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 20
        #if TSS_DETECT_EL_METHOD(20,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E20_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,20), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(20), TSS_ELECTRODE_BIT_NUM(20),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E20_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E20_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 21
        #if TSS_DETECT_EL_METHOD(21,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E21_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,21), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(21), TSS_ELECTRODE_BIT_NUM(21),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E21_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E21_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 22
        #if TSS_DETECT_EL_METHOD(22,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E22_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,22), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(22), TSS_ELECTRODE_BIT_NUM(22),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E22_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E22_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 23
        #if TSS_DETECT_EL_METHOD(23,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E23_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,23), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(23), TSS_ELECTRODE_BIT_NUM(23),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E23_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E23_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 24
        #if TSS_DETECT_EL_METHOD(24,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E24_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,24), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(24), TSS_ELECTRODE_BIT_NUM(24),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E24_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E24_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 25
        #if TSS_DETECT_EL_METHOD(25,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E25_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,25), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(25), TSS_ELECTRODE_BIT_NUM(25),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E25_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E25_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 26
        #if TSS_DETECT_EL_METHOD(26,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E26_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,26), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(26), TSS_ELECTRODE_BIT_NUM(26),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E26_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E26_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 27
        #if TSS_DETECT_EL_METHOD(27,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E27_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,27), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(27), TSS_ELECTRODE_BIT_NUM(27),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E27_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E27_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 28
        #if TSS_DETECT_EL_METHOD(28,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E28_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,28), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(28), TSS_ELECTRODE_BIT_NUM(28),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E28_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E28_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 29
        #if TSS_DETECT_EL_METHOD(29,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E29_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,29), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(29), TSS_ELECTRODE_BIT_NUM(29),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E29_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E29_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 30
        #if TSS_DETECT_EL_METHOD(30,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E30_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,30), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(30), TSS_ELECTRODE_BIT_NUM(30),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E30_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E30_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 31
        #if TSS_DETECT_EL_METHOD(31,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E31_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,31), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(31), TSS_ELECTRODE_BIT_NUM(31),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E31_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E31_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 32
        #if TSS_DETECT_EL_METHOD(32,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E32_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,32), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(32), TSS_ELECTRODE_BIT_NUM(32),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E32_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E32_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 33
        #if TSS_DETECT_EL_METHOD(33,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E33_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,33), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(33), TSS_ELECTRODE_BIT_NUM(33),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E33_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E33_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 34
        #if TSS_DETECT_EL_METHOD(34,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E34_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,34), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(34), TSS_ELECTRODE_BIT_NUM(34),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E34_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E34_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 35
        #if TSS_DETECT_EL_METHOD(35,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E35_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,35), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(35), TSS_ELECTRODE_BIT_NUM(35),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E35_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E35_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 36
        #if TSS_DETECT_EL_METHOD(36,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E36_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,36), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(36), TSS_ELECTRODE_BIT_NUM(36),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E36_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E36_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 37
        #if TSS_DETECT_EL_METHOD(37,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E37_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,37), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(37), TSS_ELECTRODE_BIT_NUM(37),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E37_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E37_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 38
        #if TSS_DETECT_EL_METHOD(38,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E38_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,38), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(38), TSS_ELECTRODE_BIT_NUM(38),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E38_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E38_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 39
        #if TSS_DETECT_EL_METHOD(39,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E39_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,39), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(39), TSS_ELECTRODE_BIT_NUM(39),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E39_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E39_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 40
        #if TSS_DETECT_EL_METHOD(40,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E40_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,40), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(40), TSS_ELECTRODE_BIT_NUM(40),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E40_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E40_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 41
        #if TSS_DETECT_EL_METHOD(41,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E41_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,41), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(41), TSS_ELECTRODE_BIT_NUM(41),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E41_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E41_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 42
        #if TSS_DETECT_EL_METHOD(42,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E42_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,42), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(42), TSS_ELECTRODE_BIT_NUM(42),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E42_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E42_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 43
        #if TSS_DETECT_EL_METHOD(43,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E43_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,43), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(43), TSS_ELECTRODE_BIT_NUM(43),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E43_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E43_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 44
        #if TSS_DETECT_EL_METHOD(44,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E44_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,44), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(44), TSS_ELECTRODE_BIT_NUM(44),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E44_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E44_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 45
        #if TSS_DETECT_EL_METHOD(45,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E45_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,45), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(45), TSS_ELECTRODE_BIT_NUM(45),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E45_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E45_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 46
        #if TSS_DETECT_EL_METHOD(46,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E46_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,46), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(46), TSS_ELECTRODE_BIT_NUM(46),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E46_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E46_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 47
        #if TSS_DETECT_EL_METHOD(47,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E47_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,47), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(47), TSS_ELECTRODE_BIT_NUM(47),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E47_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E47_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 48
        #if TSS_DETECT_EL_METHOD(48,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E48_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,48), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(48), TSS_ELECTRODE_BIT_NUM(48),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E48_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E48_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 49
        #if TSS_DETECT_EL_METHOD(49,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E49_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,49), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(49), TSS_ELECTRODE_BIT_NUM(49),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E49_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E49_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 50
        #if TSS_DETECT_EL_METHOD(50,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E50_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,50), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(50), TSS_ELECTRODE_BIT_NUM(50),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E50_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E50_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 51
        #if TSS_DETECT_EL_METHOD(51,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E51_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,51), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(51), TSS_ELECTRODE_BIT_NUM(51),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E51_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E51_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 52
        #if TSS_DETECT_EL_METHOD(52,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E52_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,52), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(52), TSS_ELECTRODE_BIT_NUM(52),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E52_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E52_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 53
        #if TSS_DETECT_EL_METHOD(53,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E53_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,53), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(53), TSS_ELECTRODE_BIT_NUM(53),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E53_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E53_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 54
        #if TSS_DETECT_EL_METHOD(54,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E54_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,54), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(54), TSS_ELECTRODE_BIT_NUM(54),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E54_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E54_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 55
        #if TSS_DETECT_EL_METHOD(55,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E55_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,55), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(55), TSS_ELECTRODE_BIT_NUM(55),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E55_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E55_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 56
        #if TSS_DETECT_EL_METHOD(56,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E56_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,56), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(56), TSS_ELECTRODE_BIT_NUM(56),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E56_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E56_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 57
        #if TSS_DETECT_EL_METHOD(57,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E57_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,57), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(57), TSS_ELECTRODE_BIT_NUM(57),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E57_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E57_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 58
        #if TSS_DETECT_EL_METHOD(58,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E58_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,58), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(58), TSS_ELECTRODE_BIT_NUM(58),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E58_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E58_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 59
        #if TSS_DETECT_EL_METHOD(59,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E59_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,59), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(59), TSS_ELECTRODE_BIT_NUM(59),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E59_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E59_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 60
        #if TSS_DETECT_EL_METHOD(60,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E60_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,60), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(60), TSS_ELECTRODE_BIT_NUM(60),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E60_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E60_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 61
        #if TSS_DETECT_EL_METHOD(61,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E61_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,61), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(61), TSS_ELECTRODE_BIT_NUM(61),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E61_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E61_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 62
        #if TSS_DETECT_EL_METHOD(62,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E62_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,62), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(62), TSS_ELECTRODE_BIT_NUM(62),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E62_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E62_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

      #if TSS_N_ELECTRODES > 63
        #if TSS_DETECT_EL_METHOD(63,GPIO)
          const TSS_GPIO_ELECTRODE_ROMDATA TSS_E63_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,63), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(63), TSS_ELECTRODE_BIT_NUM(63),
                                                                     #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                       TSS_E63_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                     #endif
                                                                     };
                TSS_GPIO_ELECTRODE_RAMDATA TSS_E63_RAMDATA_CONTEXT = { 0u };
        #endif
      #endif

    #endif /* End of TSS_USE_SIMPLE_LOW_LEVEL == 0 */

  #elif TSS_KINETIS_MCU

    /********************* Method Memory Data Decoding Macros **********/

    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_RESULT(memory,res)     TSS_GPIO_PORT##res##_METHOD_##memory##_CONTEXT
    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_CONV(memory,text)      TSS_GET_GPIO_METHOD_DATA_CONTEXT_RESULT(memory,text)
    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_TYPE(memory,elec)      TSS_GET_GPIO_METHOD_DATA_CONTEXT_CONV(memory,elec)

    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT(memory,el)             TSS_GET_GPIO_METHOD_DATA_CONTEXT_TYPE(memory,TSS_E##el##_P)

    /***************** ROM & RAM Data definition ***********************/

    #if TSS_N_ELECTRODES > 0
      #if TSS_DETECT_EL_METHOD(0,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E0_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,0), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(0), TSS_ELECTRODE_BIT_NUM(0),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E0_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E0_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 1
      #if TSS_DETECT_EL_METHOD(1,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E1_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,1), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(1), TSS_ELECTRODE_BIT_NUM(1),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E1_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E1_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 2
      #if TSS_DETECT_EL_METHOD(2,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E2_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,2), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(2), TSS_ELECTRODE_BIT_NUM(2),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E2_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E2_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 3
      #if TSS_DETECT_EL_METHOD(3,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E3_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,3), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(3), TSS_ELECTRODE_BIT_NUM(3),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E3_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E3_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 4
      #if TSS_DETECT_EL_METHOD(4,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E4_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,4), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(4), TSS_ELECTRODE_BIT_NUM(4),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E4_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E4_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 5
      #if TSS_DETECT_EL_METHOD(5,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E5_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,5), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(5), TSS_ELECTRODE_BIT_NUM(5),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E5_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E5_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 6
      #if TSS_DETECT_EL_METHOD(6,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E6_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,6), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(6), TSS_ELECTRODE_BIT_NUM(6),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E6_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E6_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 7
      #if TSS_DETECT_EL_METHOD(7,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E7_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,7), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(7), TSS_ELECTRODE_BIT_NUM(7),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E7_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E7_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 8
      #if TSS_DETECT_EL_METHOD(8,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E8_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,8), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(8), TSS_ELECTRODE_BIT_NUM(8),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E8_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E8_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 9
      #if TSS_DETECT_EL_METHOD(9,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E9_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,9), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(9), TSS_ELECTRODE_BIT_NUM(9),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E9_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E9_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 10
      #if TSS_DETECT_EL_METHOD(10,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E10_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,10), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(10), TSS_ELECTRODE_BIT_NUM(10),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E10_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E10_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 11
      #if TSS_DETECT_EL_METHOD(11,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E11_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,11), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(11), TSS_ELECTRODE_BIT_NUM(11),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E11_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E11_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 12
      #if TSS_DETECT_EL_METHOD(12,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E12_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,12), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(12), TSS_ELECTRODE_BIT_NUM(12),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E12_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E12_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 13
      #if TSS_DETECT_EL_METHOD(13,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E13_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,13), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(13), TSS_ELECTRODE_BIT_NUM(13),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E13_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E13_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 14
      #if TSS_DETECT_EL_METHOD(14,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E14_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,14), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(14), TSS_ELECTRODE_BIT_NUM(14),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E14_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E14_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 15
      #if TSS_DETECT_EL_METHOD(15,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E15_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,15), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(15), TSS_ELECTRODE_BIT_NUM(15),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E15_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E15_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 16
      #if TSS_DETECT_EL_METHOD(16,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E16_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,16), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(16), TSS_ELECTRODE_BIT_NUM(16),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E16_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E16_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 17
      #if TSS_DETECT_EL_METHOD(17,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E17_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,17), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(17), TSS_ELECTRODE_BIT_NUM(17),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E17_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E17_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 18
      #if TSS_DETECT_EL_METHOD(18,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E18_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,18), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(18), TSS_ELECTRODE_BIT_NUM(18),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E18_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E18_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 19
      #if TSS_DETECT_EL_METHOD(19,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E19_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,19), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(19), TSS_ELECTRODE_BIT_NUM(19),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E19_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E19_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 20
      #if TSS_DETECT_EL_METHOD(20,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E20_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,20), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(20), TSS_ELECTRODE_BIT_NUM(20),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E20_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E20_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 21
      #if TSS_DETECT_EL_METHOD(21,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E21_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,21), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(21), TSS_ELECTRODE_BIT_NUM(21),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E21_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E21_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 22
      #if TSS_DETECT_EL_METHOD(22,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E22_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,22), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(22), TSS_ELECTRODE_BIT_NUM(22),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E22_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E22_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 23
      #if TSS_DETECT_EL_METHOD(23,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E23_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,23), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(23), TSS_ELECTRODE_BIT_NUM(23),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E23_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E23_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 24
      #if TSS_DETECT_EL_METHOD(24,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E24_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,24), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(24), TSS_ELECTRODE_BIT_NUM(24),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E24_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E24_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 25
      #if TSS_DETECT_EL_METHOD(25,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E25_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,25), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(25), TSS_ELECTRODE_BIT_NUM(25),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E25_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E25_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 26
      #if TSS_DETECT_EL_METHOD(26,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E26_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,26), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(26), TSS_ELECTRODE_BIT_NUM(26),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E26_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E26_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 27
      #if TSS_DETECT_EL_METHOD(27,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E27_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,27), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(27), TSS_ELECTRODE_BIT_NUM(27),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E27_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E27_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 28
      #if TSS_DETECT_EL_METHOD(28,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E28_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,28), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(28), TSS_ELECTRODE_BIT_NUM(28),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E28_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E28_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 29
      #if TSS_DETECT_EL_METHOD(29,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E29_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,29), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(29), TSS_ELECTRODE_BIT_NUM(29),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E29_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E29_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 30
      #if TSS_DETECT_EL_METHOD(30,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E30_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,30), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(30), TSS_ELECTRODE_BIT_NUM(30),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E30_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E30_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 31
      #if TSS_DETECT_EL_METHOD(31,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E31_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,31), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(31), TSS_ELECTRODE_BIT_NUM(31),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E31_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E31_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 32
      #if TSS_DETECT_EL_METHOD(32,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E32_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,32), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(32), TSS_ELECTRODE_BIT_NUM(32),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E32_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E32_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 33
      #if TSS_DETECT_EL_METHOD(33,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E33_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,33), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(33), TSS_ELECTRODE_BIT_NUM(33),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E33_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E33_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 34
      #if TSS_DETECT_EL_METHOD(34,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E34_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,34), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(34), TSS_ELECTRODE_BIT_NUM(34),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E34_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E34_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 35
      #if TSS_DETECT_EL_METHOD(35,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E35_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,35), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(35), TSS_ELECTRODE_BIT_NUM(35),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E35_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E35_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 36
      #if TSS_DETECT_EL_METHOD(36,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E36_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,36), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(36), TSS_ELECTRODE_BIT_NUM(36),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E36_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E36_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 37
      #if TSS_DETECT_EL_METHOD(37,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E37_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,37), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(37), TSS_ELECTRODE_BIT_NUM(37),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E37_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E37_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 38
      #if TSS_DETECT_EL_METHOD(38,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E38_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,38), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(38), TSS_ELECTRODE_BIT_NUM(38),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E38_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E38_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 39
      #if TSS_DETECT_EL_METHOD(39,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E39_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,39), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(39), TSS_ELECTRODE_BIT_NUM(39),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E39_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E39_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 40
      #if TSS_DETECT_EL_METHOD(40,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E40_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,40), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(40), TSS_ELECTRODE_BIT_NUM(40),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E40_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E40_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 41
      #if TSS_DETECT_EL_METHOD(41,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E41_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,41), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(41), TSS_ELECTRODE_BIT_NUM(41),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E41_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E41_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 42
      #if TSS_DETECT_EL_METHOD(42,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E42_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,42), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(42), TSS_ELECTRODE_BIT_NUM(42),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E42_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E42_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 43
      #if TSS_DETECT_EL_METHOD(43,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E43_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,43), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(43), TSS_ELECTRODE_BIT_NUM(43),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E43_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E43_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 44
      #if TSS_DETECT_EL_METHOD(44,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E44_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,44), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(44), TSS_ELECTRODE_BIT_NUM(44),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E44_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E44_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 45
      #if TSS_DETECT_EL_METHOD(45,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E45_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,45), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(45), TSS_ELECTRODE_BIT_NUM(45),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E45_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E45_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 46
      #if TSS_DETECT_EL_METHOD(46,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E46_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,46), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(46), TSS_ELECTRODE_BIT_NUM(46),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E46_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E46_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 47
      #if TSS_DETECT_EL_METHOD(47,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E47_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,47), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(47), TSS_ELECTRODE_BIT_NUM(47),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E47_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E47_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 48
      #if TSS_DETECT_EL_METHOD(48,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E48_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,48), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(48), TSS_ELECTRODE_BIT_NUM(48),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E48_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E48_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 49
      #if TSS_DETECT_EL_METHOD(49,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E49_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,49), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(49), TSS_ELECTRODE_BIT_NUM(49),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E49_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E49_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 50
      #if TSS_DETECT_EL_METHOD(50,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E50_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,50), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(50), TSS_ELECTRODE_BIT_NUM(50),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E50_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E50_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 51
      #if TSS_DETECT_EL_METHOD(51,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E51_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,51), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(51), TSS_ELECTRODE_BIT_NUM(51),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E51_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E51_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 52
      #if TSS_DETECT_EL_METHOD(52,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E52_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,52), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(52), TSS_ELECTRODE_BIT_NUM(52),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E52_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E52_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 53
      #if TSS_DETECT_EL_METHOD(53,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E53_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,53), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(53), TSS_ELECTRODE_BIT_NUM(53),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E53_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E53_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 54
      #if TSS_DETECT_EL_METHOD(54,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E54_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,54), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(54), TSS_ELECTRODE_BIT_NUM(54),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E54_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E54_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 55
      #if TSS_DETECT_EL_METHOD(55,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E55_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,55), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(55), TSS_ELECTRODE_BIT_NUM(55),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E55_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E55_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 56
      #if TSS_DETECT_EL_METHOD(56,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E56_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,56), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(56), TSS_ELECTRODE_BIT_NUM(56),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E56_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E56_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 57
      #if TSS_DETECT_EL_METHOD(57,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E57_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,57), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(57), TSS_ELECTRODE_BIT_NUM(57),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E57_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E57_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 58
      #if TSS_DETECT_EL_METHOD(58,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E58_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,58), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(58), TSS_ELECTRODE_BIT_NUM(58),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E58_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E58_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 59
      #if TSS_DETECT_EL_METHOD(59,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E59_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,59), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(59), TSS_ELECTRODE_BIT_NUM(59),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E59_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E59_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 60
      #if TSS_DETECT_EL_METHOD(60,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E60_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,60), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(60), TSS_ELECTRODE_BIT_NUM(60),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E60_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E60_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 61
      #if TSS_DETECT_EL_METHOD(61,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E61_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,61), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(61), TSS_ELECTRODE_BIT_NUM(61),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E61_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E61_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 62
      #if TSS_DETECT_EL_METHOD(62,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E62_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,62), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(62), TSS_ELECTRODE_BIT_NUM(62),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E62_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E62_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 63
      #if TSS_DETECT_EL_METHOD(63,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E63_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,63), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(63), TSS_ELECTRODE_BIT_NUM(63),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E63_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E63_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

  #elif TSS_CFM_MCU

    /********************* Method Memory Data Decoding Macros **********/

    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_RESULT(memory,res)     TSS_GPIO_PORT##res##_METHOD_##memory##_CONTEXT
    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_CONV(memory,text)      TSS_GET_GPIO_METHOD_DATA_CONTEXT_RESULT(memory,text)
    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT_TYPE(memory,elec)      TSS_GET_GPIO_METHOD_DATA_CONTEXT_CONV(memory,elec)

    #define TSS_GET_GPIO_METHOD_DATA_CONTEXT(memory,el)             TSS_GET_GPIO_METHOD_DATA_CONTEXT_TYPE(memory,TSS_E##el##_P)

    /***************** ROM & RAM Data definition ***********************/

    #if TSS_N_ELECTRODES > 0
      #if TSS_DETECT_EL_METHOD(0,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E0_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,0), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(0), TSS_ELECTRODE_BIT_NUM(0),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E0_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E0_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 1
      #if TSS_DETECT_EL_METHOD(1,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E1_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,1), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(1), TSS_ELECTRODE_BIT_NUM(1),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E1_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E1_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 2
      #if TSS_DETECT_EL_METHOD(2,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E2_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,2), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(2), TSS_ELECTRODE_BIT_NUM(2),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E2_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E2_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 3
      #if TSS_DETECT_EL_METHOD(3,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E3_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,3), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(3), TSS_ELECTRODE_BIT_NUM(3),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E3_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E3_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 4
      #if TSS_DETECT_EL_METHOD(4,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E4_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,4), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(4), TSS_ELECTRODE_BIT_NUM(4),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E4_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E4_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 5
      #if TSS_DETECT_EL_METHOD(5,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E5_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,5), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(5), TSS_ELECTRODE_BIT_NUM(5),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E5_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E5_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 6
      #if TSS_DETECT_EL_METHOD(6,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E6_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,6), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(6), TSS_ELECTRODE_BIT_NUM(6),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E6_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E6_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 7
      #if TSS_DETECT_EL_METHOD(7,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E7_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,7), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(7), TSS_ELECTRODE_BIT_NUM(7),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E7_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E7_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 8
      #if TSS_DETECT_EL_METHOD(8,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E8_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,8), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(8), TSS_ELECTRODE_BIT_NUM(8),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E8_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E8_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 9
      #if TSS_DETECT_EL_METHOD(9,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E9_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,9), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(9), TSS_ELECTRODE_BIT_NUM(9),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E9_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E9_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 10
      #if TSS_DETECT_EL_METHOD(10,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E10_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,10), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(10), TSS_ELECTRODE_BIT_NUM(10),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E10_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E10_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 11
      #if TSS_DETECT_EL_METHOD(11,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E11_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,11), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(11), TSS_ELECTRODE_BIT_NUM(11),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E11_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E11_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 12
      #if TSS_DETECT_EL_METHOD(12,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E12_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,12), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(12), TSS_ELECTRODE_BIT_NUM(12),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E12_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E12_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 13
      #if TSS_DETECT_EL_METHOD(13,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E13_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,13), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(13), TSS_ELECTRODE_BIT_NUM(13),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E13_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E13_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 14
      #if TSS_DETECT_EL_METHOD(14,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E14_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,14), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(14), TSS_ELECTRODE_BIT_NUM(14),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E14_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E14_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 15
      #if TSS_DETECT_EL_METHOD(15,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E15_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,15), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(15), TSS_ELECTRODE_BIT_NUM(15),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E15_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E15_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 16
      #if TSS_DETECT_EL_METHOD(16,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E16_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,16), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(16), TSS_ELECTRODE_BIT_NUM(16),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E16_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E16_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 17
      #if TSS_DETECT_EL_METHOD(17,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E17_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,17), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(17), TSS_ELECTRODE_BIT_NUM(17),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E17_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E17_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 18
      #if TSS_DETECT_EL_METHOD(18,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E18_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,18), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(18), TSS_ELECTRODE_BIT_NUM(18),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E18_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E18_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 19
      #if TSS_DETECT_EL_METHOD(19,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E19_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,19), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(19), TSS_ELECTRODE_BIT_NUM(19),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E19_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E19_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 20
      #if TSS_DETECT_EL_METHOD(20,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E20_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,20), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(20), TSS_ELECTRODE_BIT_NUM(20),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E20_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E20_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 21
      #if TSS_DETECT_EL_METHOD(21,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E21_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,21), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(21), TSS_ELECTRODE_BIT_NUM(21),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E21_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E21_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 22
      #if TSS_DETECT_EL_METHOD(22,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E22_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,22), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(22), TSS_ELECTRODE_BIT_NUM(22),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E22_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E22_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 23
      #if TSS_DETECT_EL_METHOD(23,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E23_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,23), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(23), TSS_ELECTRODE_BIT_NUM(23),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E23_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E23_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 24
      #if TSS_DETECT_EL_METHOD(24,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E24_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,24), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(24), TSS_ELECTRODE_BIT_NUM(24),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E24_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E24_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 25
      #if TSS_DETECT_EL_METHOD(25,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E25_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,25), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(25), TSS_ELECTRODE_BIT_NUM(25),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E25_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E25_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 26
      #if TSS_DETECT_EL_METHOD(26,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E26_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,26), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(26), TSS_ELECTRODE_BIT_NUM(26),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E26_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E26_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 27
      #if TSS_DETECT_EL_METHOD(27,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E27_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,27), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(27), TSS_ELECTRODE_BIT_NUM(27),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E27_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E27_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 28
      #if TSS_DETECT_EL_METHOD(28,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E28_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,28), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(28), TSS_ELECTRODE_BIT_NUM(28),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E28_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E28_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 29
      #if TSS_DETECT_EL_METHOD(29,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E29_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,29), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(29), TSS_ELECTRODE_BIT_NUM(29),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E29_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E29_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 30
      #if TSS_DETECT_EL_METHOD(30,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E30_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,30), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(30), TSS_ELECTRODE_BIT_NUM(30),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E30_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E30_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 31
      #if TSS_DETECT_EL_METHOD(31,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E31_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,31), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(31), TSS_ELECTRODE_BIT_NUM(31),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E31_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E31_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 32
      #if TSS_DETECT_EL_METHOD(32,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E32_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,32), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(32), TSS_ELECTRODE_BIT_NUM(32),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E32_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E32_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 33
      #if TSS_DETECT_EL_METHOD(33,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E33_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,33), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(33), TSS_ELECTRODE_BIT_NUM(33),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E33_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E33_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 34
      #if TSS_DETECT_EL_METHOD(34,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E34_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,34), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(34), TSS_ELECTRODE_BIT_NUM(34),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E34_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E34_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 35
      #if TSS_DETECT_EL_METHOD(35,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E35_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,35), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(35), TSS_ELECTRODE_BIT_NUM(35),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E35_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E35_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 36
      #if TSS_DETECT_EL_METHOD(36,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E36_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,36), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(36), TSS_ELECTRODE_BIT_NUM(36),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E36_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E36_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 37
      #if TSS_DETECT_EL_METHOD(37,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E37_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,37), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(37), TSS_ELECTRODE_BIT_NUM(37),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E37_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E37_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 38
      #if TSS_DETECT_EL_METHOD(38,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E38_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,38), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(38), TSS_ELECTRODE_BIT_NUM(38),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E38_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E38_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 39
      #if TSS_DETECT_EL_METHOD(39,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E39_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,39), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(39), TSS_ELECTRODE_BIT_NUM(39),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E39_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E39_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 40
      #if TSS_DETECT_EL_METHOD(40,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E40_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,40), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(40), TSS_ELECTRODE_BIT_NUM(40),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E40_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E40_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 41
      #if TSS_DETECT_EL_METHOD(41,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E41_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,41), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(41), TSS_ELECTRODE_BIT_NUM(41),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E41_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E41_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 42
      #if TSS_DETECT_EL_METHOD(42,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E42_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,42), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(42), TSS_ELECTRODE_BIT_NUM(42),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E42_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E42_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 43
      #if TSS_DETECT_EL_METHOD(43,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E43_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,43), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(43), TSS_ELECTRODE_BIT_NUM(43),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E43_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E43_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 44
      #if TSS_DETECT_EL_METHOD(44,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E44_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,44), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(44), TSS_ELECTRODE_BIT_NUM(44),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E44_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E44_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 45
      #if TSS_DETECT_EL_METHOD(45,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E45_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,45), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(45), TSS_ELECTRODE_BIT_NUM(45),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E45_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E45_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 46
      #if TSS_DETECT_EL_METHOD(46,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E46_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,46), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(46), TSS_ELECTRODE_BIT_NUM(46),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E46_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E46_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 47
      #if TSS_DETECT_EL_METHOD(47,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E47_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,47), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(47), TSS_ELECTRODE_BIT_NUM(47),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E47_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E47_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 48
      #if TSS_DETECT_EL_METHOD(48,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E48_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,48), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(48), TSS_ELECTRODE_BIT_NUM(48),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E48_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E48_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 49
      #if TSS_DETECT_EL_METHOD(49,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E49_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,49), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(49), TSS_ELECTRODE_BIT_NUM(49),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E49_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E49_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 50
      #if TSS_DETECT_EL_METHOD(50,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E50_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,50), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(50), TSS_ELECTRODE_BIT_NUM(50),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E50_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E50_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 51
      #if TSS_DETECT_EL_METHOD(51,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E51_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,51), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(51), TSS_ELECTRODE_BIT_NUM(51),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E51_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E51_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 52
      #if TSS_DETECT_EL_METHOD(52,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E52_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,52), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(52), TSS_ELECTRODE_BIT_NUM(52),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E52_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E52_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 53
      #if TSS_DETECT_EL_METHOD(53,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E53_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,53), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(53), TSS_ELECTRODE_BIT_NUM(53),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E53_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E53_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 54
      #if TSS_DETECT_EL_METHOD(54,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E54_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,54), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(54), TSS_ELECTRODE_BIT_NUM(54),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E54_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E54_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 55
      #if TSS_DETECT_EL_METHOD(55,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E55_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,55), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(55), TSS_ELECTRODE_BIT_NUM(55),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E55_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E55_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 56
      #if TSS_DETECT_EL_METHOD(56,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E56_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,56), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(56), TSS_ELECTRODE_BIT_NUM(56),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E56_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E56_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 57
      #if TSS_DETECT_EL_METHOD(57,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E57_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,57), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(57), TSS_ELECTRODE_BIT_NUM(57),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E57_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E57_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 58
      #if TSS_DETECT_EL_METHOD(58,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E58_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,58), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(58), TSS_ELECTRODE_BIT_NUM(58),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E58_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E58_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 59
      #if TSS_DETECT_EL_METHOD(59,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E59_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,59), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(59), TSS_ELECTRODE_BIT_NUM(59),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E59_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E59_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 60
      #if TSS_DETECT_EL_METHOD(60,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E60_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,60), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(60), TSS_ELECTRODE_BIT_NUM(60),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E60_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E60_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 61
      #if TSS_DETECT_EL_METHOD(61,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E61_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,61), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(61), TSS_ELECTRODE_BIT_NUM(61),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E61_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E61_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 62
      #if TSS_DETECT_EL_METHOD(62,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E62_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,62), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(62), TSS_ELECTRODE_BIT_NUM(62),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E62_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E62_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif

    #if TSS_N_ELECTRODES > 63
      #if TSS_DETECT_EL_METHOD(63,GPIO)
        const TSS_GPIO_ELECTRODE_ROMDATA TSS_E63_ROMDATA_CONTEXT = { &TSS_GET_GPIO_METHOD_DATA_CONTEXT(ROMDATA,63), TSS_GET_SAMPLE_ELECTRODE_LOW_ROUTINE(63), TSS_ELECTRODE_BIT_NUM(63),
                                                                   #if TSS_USE_NOISE_AMPLITUDE_FILTER
                                                                     TSS_E63_NOISE_AMPLITUDE_FILTER_HALF_SIZE,
                                                                   #endif
                                                                   };
              TSS_GPIO_ELECTRODE_RAMDATA TSS_E63_RAMDATA_CONTEXT = { 0u };
      #endif
    #endif
  #endif

#endif

#endif
