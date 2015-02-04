/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
* 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
*
*END************************************************************************/

#ifndef _Driver_SLCD_H_
#define _Driver_SLCD_H_

  #define MAX_COLUMNS_PER_CHAR                        4
  #define CHAR_RESET_POSITION                         5
  #define CHAR_MAX_POSITION_VALUE                     34
  #define CHAR_POSITION_INCREMENT                     6
  #define MARQUEE_MAX_POSITION_WITHOUT_ARROW          37
  #define MARQUEE_MAX_POSITION_WITH_ARROW             35
  #define MARQUEE_MIN_POSITION_WITHOUT_ARROW          0                        
  #define MARQUEE_MIN_POSITION_WITH_ARROW             2           
  #define MARQUEE_WITH_ARROWS                         1      
  #define MARQUEE_WITHOUT_ARROWS                      0      

  #define FRONT_PLANE_0_TO_3                          0
  #define FRONT_PLANE_4_TO_7                          4
  #define FRONT_PLANE_8_TO_11                         8
  #define FRONT_PLANE_12_TO_15                        12
  #define FRONT_PLANE_16_TO_19                        16
  #define FRONT_PLANE_20_TO_23                        20
  #define FRONT_PLANE_24_TO_27                        24
  #define FRONT_PLANE_28_TO_31                        28
  #define FRONT_PLANE_32_TO_35                        32
  #define FRONT_PLANE_36_TO_39                        36
  #define FRONT_PLANE_40_TO_43                        40
  #define FRONT_PLANE_44_TO_47                        44 

  #define UPPER_LEFT                                  0
  #define MIDDLE_LEFT                                 1
  #define LOWER_LEFT                                  2
  #define UPPER_RIGHT                                 3
  #define MIDDLE_RIGHT                                4                      
  #define LOWER_RIGHT                                 5
                      
  #define CHARGED                                     2                                            
  #define MID_LEVEL                                   1
  #define LOWER_LEVEL                                 0

  #define ARROWS                                      1
  #define NO_ARROWS                                   0

  typedef enum
  {
    PHASE_A = 0,
    PHASE_B,
    PHASE_C,
    PHASE_D,
    PHASE_E,
    PHASE_F,
    PHASE_G,
    PHASE_H  
  }eBackPlanePhases;

  
  extern BOOL isMarqueeRestart;

  extern void _SLCDModule_Init(void);

  //LCD 
  extern void _SLCDMoudle_TurnOnAllSegments(void);
  extern void _SLCDMoudle_TurnOffAllSegments(void);
  extern void _SLCDModule_SetSegment(U08 X, U08 Y);
  extern void _SLCDModule_ClearSegment(U08 X, U08 Y);
  extern void _SLCDModule_PrintString(STRING SourceString, U08 StartPosition);
  extern U08 _SLCDModule_PrintMarquee(STRING SourceString, S16 StartPosition, U08 MaxPosition, U08 MinPosition);
  extern void _SLCDModule_ClearLCD(U08 ArrowsOn);

 //Arrows  
  extern void _SLCDModule_PrintUpperArrow(void);
  extern void _SLCDModule_PrintLowerArrow(void);
  extern void _SLCDModule_PrintLeftArrow(void);
  extern void _SLCDModule_PrintRightArrow(void);
  extern void _SLCDModule_PrintUpperRCArrow(void);
  extern void _SLCDModule_PrintLowerRCArrow(void);

  extern void _SLCDModule_EraseUpperArrow(void);
  extern void _SLCDModule_EraseLowerArrow(void);
  extern void _SLCDModule_EraseLeftArrow(void);
  extern void _SLCDModule_EraseRightArrow(void);
  extern void _SLCDModule_EraseUpperRCArrow(void);
  extern void _SLCDModule_EraseLowerRCArrow(void);
 
 //Other  
  extern void _SLCDMOdule_PrintDotMarquee(S08 StartPosition);   
  extern void _SLCDModule_PrintRCBlocks(U08 Position);
  extern void _SLCDModule_EraseRCBlocks(U08 Position);
  extern void _SLCDModule_PrintNumber(U08 Number, U08 Position);

 //Signs
  extern void _SLCDModule_TurnOnUSBSign(void);
  extern void _SLCDModule_TurnOnWirelessSign(void);
  extern void _SLCDModule_TurnOnPounceSign(void);
  extern void _SLCDModule_TurnOnFreescaleSign(void);
  extern void _SLCDModule_TurnOnClockSign(void);
  extern void _SLCDModule_TurnOnJLinkSign(void);
  extern void _SLCDModule_TurnOnBatteryOutline(void);
  extern void _SLCDModule_TurnOnBattery(U08 BatteryLevel);

  extern void _SLCDModule_TurnOffUSBSign(void);
  extern void _SLCDModule_TurnOffWirelessSign(void);
  extern void _SLCDModule_TurnOffPounceSign(void);
  extern void _SLCDModule_TurnOffFreescaleSign(void);
  extern void _SLCDModule_TurnOffClockSign(void);
  extern void _SLCDModule_TurnOffJLinkSign(void);
  extern void _SLCDModule_TurnOffBattery(void);
  
#endif
