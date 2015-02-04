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

#include "mqx.h"
#include "bsp.h"
#include "PounceTypes_v2.h"
#include "Fonts_LCD.h"  
#include "slcd_kwikstik-k40x256.h"


const U08 WaveFormShiftRegisterTable[] = 
{
  16,               // FrontPlane 2 
  24,               // FrontPlane 3 
  0,                // FrontPlane 4 
  8,                // FrontPlane 5 
  16,               // FrontPlane 6 
  24,               // FrontPlane 7 
  0,                // FrontPlane 8 
  8,                // FrontPlane 9 
  16,               // FrontPlane 10 
  24,               // FrontPlane 11 
  0,                // FrontPlane 12 
  8,                // FrontPlane 13 
  16,               // FrontPlane 14 
  24,               // FrontPlane 15 
  0,                // FrontPlane 16 
  8,                // FrontPlane 17 
  16,               // FrontPlane 18 
  24,               // FrontPlane 19 
  0,                // FrontPlane 20 
  8,                // FrontPlane 21 
  16,               // FrontPlane 22 
  24,               // FrontPlane 23 
  0,                // FrontPlane 24 
  8,                // FrontPlane 25 
  16,               // FrontPlane 26 
  24,               // FrontPlane 27 
  0,                // FrontPlane 28 
  8,                // FrontPlane 29 
  16,               // FrontPlane 30 
  24,               // FrontPlane 31 
  0,                // FrontPlane 32 
  8,                // FrontPlane 33 
  16,               // FrontPlane 34 
  24,               // FrontPlane 35 
  0,                // FrontPlane 36 
  8,                // FrontPlane 37 
  16,               // FrontPlane 38 
};

const U32 WaveFormRegisterClearTable[] = 
{
  0xFF00FFFF,               // FrontPlane 2 
  0x00FFFFFF,               // FrontPlane 3 
  0xFFFFFF00,               // FrontPlane 4 
  0xFFFF00FF,               // FrontPlane 5 
  0xFF00FFFF,               // FrontPlane 6 
  0x00FFFFFF,               // FrontPlane 7 
  0xFFFFFF00,               // FrontPlane 8 
  0xFFFF00FF,               // FrontPlane 9 
  0xFF00FFFF,               // FrontPlane 10 
  0x00FFFFFF,               // FrontPlane 11 
  0xFFFFFF00,               // FrontPlane 12 
  0xFFFF00FF,               // FrontPlane 13 
  0xFF00FFFF,               // FrontPlane 14 
  0x00FFFFFF,               // FrontPlane 15 
  0xFFFFFF00,               // FrontPlane 16 
  0xFFFF00FF,               // FrontPlane 17 
  0xFF00FFFF,               // FrontPlane 18 
  0x00FFFFFF,               // FrontPlane 19 
  0xFFFFFF00,               // FrontPlane 20 
  0xFFFF00FF,               // FrontPlane 21 
  0xFF00FFFF,               // FrontPlane 22 
  0x00FFFFFF,               // FrontPlane 23 
  0xFFFFFF00,               // FrontPlane 24 
  0xFFFF00FF,               // FrontPlane 25 
  0xFF00FFFF,               // FrontPlane 26 
  0x00FFFFFF,               // FrontPlane 27 
  0xFFFFFF00,               // FrontPlane 28 
  0xFFFF00FF,               // FrontPlane 29 
  0xFF00FFFF,               // FrontPlane 30 
  0x00FFFFFF,               // FrontPlane 31 
  0xFFFFFF00,               // FrontPlane 32 
  0xFFFF00FF,               // FrontPlane 33 
  0xFF00FFFF,               // FrontPlane 34 
  0x00FFFFFF,               // FrontPlane 35 
  0xFFFFFF00,               // FrontPlane 36 
  0xFFFF00FF,               // FrontPlane 37 
  0xFF00FFFF,               // FrontPlane 38 
};

BOOL isMarqueeRestart = FALSE;

STRING Error ;//= {"ERROR"};

/**
  Functions
*/
void _SLCDModule_StringWFRegisters(U08 FrontPlaneValues, U08 FrontPlanePosition); 
void _SLCDModule_MarqueeWFRegisters(U08 FrontPlaneValues, S16 FrontPlanePosition, U08 MaxPosition, U08 MinPosition);
void _SLCDModule_WriteWFRegister(U32 WFRegister, U08 Position);
U32 _SLCDModule_CopyWFRegister(U08 Position);
U08 _SLCDModule_SelectBackPlanePhase(void);
U16 _SLCDModule_StringLength(STRING SourceString);

/**
  void _LCDModule_Init(void)
  Parameters: None
  Description: This functions configures the registers of the SLCD Module
*/
void _SLCDModule_Init(void)
{
  MCG_C1 |= MCG_C1_IRCLKEN_MASK;
  //enable clock gate for Ports
  SIM_SCGC5 |=  ( !SIM_SCGC5_LPTIMER_MASK
                | !SIM_SCGC5_REGFILE_MASK
                | !SIM_SCGC5_TSI_MASK
                | SIM_SCGC5_PORTB_MASK
                | SIM_SCGC5_PORTC_MASK
                | SIM_SCGC5_PORTD_MASK
                );
  // Master General Purpose Control Register - Set mux to LCD analog operation.
  // After RESET these register are configured as 0 but indicated here for reference
  PORTB_PCR1  = PORT_PCR_MUX(0); //LCD_P1 
  PORTB_PCR2  = PORT_PCR_MUX(0); //LCD_P2
  PORTB_PCR3  = PORT_PCR_MUX(0); //LCD_P3
  PORTB_PCR4  = PORT_PCR_MUX(0); //LCD_P4
  PORTB_PCR5  = PORT_PCR_MUX(0); //LCD_P5
  PORTB_PCR6  = PORT_PCR_MUX(0); //LCD_P6
  PORTB_PCR7  = PORT_PCR_MUX(0); //LCD_P7
  PORTB_PCR8  = PORT_PCR_MUX(0); //LCD_P8
  PORTB_PCR9  = PORT_PCR_MUX(0); //LCD_P9
  PORTB_PCR10 = PORT_PCR_MUX(0); //LCD_P10
  PORTB_PCR11 = PORT_PCR_MUX(0); //LCD_P11
  PORTB_PCR16 = PORT_PCR_MUX(0); //LCD_P12
  PORTB_PCR17 = PORT_PCR_MUX(0); //LCD_P13
  PORTB_PCR18 = PORT_PCR_MUX(0); //LCD_P14
  PORTB_PCR19 = PORT_PCR_MUX(0); //LCD_P15
  PORTB_PCR20 = PORT_PCR_MUX(0); //LCD_P16
  PORTB_PCR21 = PORT_PCR_MUX(0); //LCD_P17
  PORTB_PCR22 = PORT_PCR_MUX(0); //LCD_P18
  PORTB_PCR23 = PORT_PCR_MUX(0); //LCD_P19
  PORTC_PCR0  = PORT_PCR_MUX(0); //LCD_P20
  PORTC_PCR1  = PORT_PCR_MUX(0); //LCD_P21
  PORTC_PCR2  = PORT_PCR_MUX(0); //LCD_P22
  PORTC_PCR3  = PORT_PCR_MUX(0); //LCD_P23
  PORTC_PCR4  = PORT_PCR_MUX(0); //LCD_P24
  PORTC_PCR5  = PORT_PCR_MUX(0); //LCD_P25
  PORTC_PCR6  = PORT_PCR_MUX(0); //LCD_P26
  PORTC_PCR7  = PORT_PCR_MUX(0); //LCD_P27
  PORTC_PCR8  = PORT_PCR_MUX(0); //LCD_P28
  PORTC_PCR9  = PORT_PCR_MUX(0); //LCD_P29
  PORTC_PCR10 = PORT_PCR_MUX(0); //LCD_P30
  PORTC_PCR11 = PORT_PCR_MUX(0); //LCD_P31
  PORTC_PCR12 = PORT_PCR_MUX(0); //LCD_P32
  PORTC_PCR13 = PORT_PCR_MUX(0); //LCD_P33
  PORTC_PCR14 = PORT_PCR_MUX(0); //LCD_P34
  PORTC_PCR15 = PORT_PCR_MUX(0); //LCD_P35
  PORTC_PCR16 = PORT_PCR_MUX(0); //LCD_P36
  PORTC_PCR17 = PORT_PCR_MUX(0); //LCD_P37
  PORTC_PCR18 = PORT_PCR_MUX(0); //LCD_P38
  PORTC_PCR19 = PORT_PCR_MUX(0); //LCD_P39  
  PORTD_PCR0  = PORT_PCR_MUX(0); //LCD_P40
  PORTD_PCR1  = PORT_PCR_MUX(0); //LCD_P41
  PORTD_PCR2  = PORT_PCR_MUX(0); //LCD_P42  
  PORTD_PCR3  = PORT_PCR_MUX(0); //LCD_P43  
  PORTD_PCR4  = PORT_PCR_MUX(0); //LCD_P44
  PORTD_PCR5  = PORT_PCR_MUX(0); //LCD_P45  
  PORTD_PCR6  = PORT_PCR_MUX(0); //LCD_P46  
  PORTD_PCR7  = PORT_PCR_MUX(0); //LCD_P47  
  
  // Configure NVIC for SLCD interrupt SLCD interrupt vector = 102
  // NVICICPR2 |= (1<<22); //Clear any pending interrupts on LCD
  // NVICISER2 |= (1<<22); //Enable interrupts from LCD interrupt*/
  // SLCD clock gate on
  SIM_SCGC3 |= SIM_SCGC3_SLCD_MASK;
  
  // Disable LCD
  LCD_GCR = 0;

  // Enable LCD pins 1-47
  LCD_PENH = 0;
  LCD_PENH = 0x0000FFFF;
  LCD_PENL = 0;
  LCD_PENL = 0xFFFFFFFE;
  // Enable LCD pins used as Backplanes 0-7
  LCD_BPENH = 0;
  LCD_BPENH = 0x0000FF00;
  LCD_BPENL = 0;
  // Configure backplane phase
  LCD_WF43TO40 = 0x08040201;
  LCD_WF47TO44 = 0x80402010;
  // Fill information on what segments are going to be turned on. Front Plane information
  LCD_WF3TO0   = 0;
  LCD_WF7TO4   = 0;
  LCD_WF11TO8  = 0;
  LCD_WF15TO12 = 0;  
  LCD_WF19TO16 = 0;
  LCD_WF23TO20 = 0;
  LCD_WF27TO24 = 0;
  LCD_WF31TO28 = 0;
  LCD_WF35TO32 = 0;
  LCD_WF39TO36 = 0;
  
  // Configure LCD Control Register
  LCD_GCR |=  ( LCD_GCR_CPSEL_MASK
              | LCD_GCR_RVEN_MASK
              | LCD_GCR_RVTRIM(15)
              | !LCD_GCR_HREFSEL_MASK // ( 0 - 3 Volts LCDs / 1 - 5 Volts LCDs)
              | LCD_GCR_LADJ(3) //0-3
              | LCD_GCR_VSUPPLY(3) //0-3
              | !LCD_GCR_LCDIEN_MASK
              | !LCD_GCR_FDCIEN_MASK
              | LCD_GCR_ALTDIV(1) //0-3
              | !LCD_GCR_LCDWAIT_MASK
              | !LCD_GCR_LCDSTP_MASK
              | !LCD_GCR_LCDEN_MASK
              | LCD_GCR_SOURCE_MASK
              | LCD_GCR_LCLK(0) //0-3
              | LCD_GCR_DUTY(7) //0-7
              );
  // Enable LCD module
  LCD_GCR |= LCD_GCR_LCDEN_MASK; 
}

/**
  void _LCDModule_PrintString(STRING SourceString, U08 StartPosition)
*/
void _SLCDModule_PrintString(STRING SourceString, U08 StartPosition)
{
  U08 StringCharCopy = 0;                            
  U08 Position = StartPosition;  
  
  do 
  {
    StringCharCopy = *SourceString;
    SourceString++;
    if (StringCharCopy > 0x80)
    {
      StringCharCopy = 0x80;
    } 
    else 
    {
      StringCharCopy -= 0x20;      
    }
    
    if (Fonts[StringCharCopy] != _NULL)
    {
      if (Position <= CHAR_MAX_POSITION_VALUE)
      {      
        _SLCDModule_StringWFRegisters(StringCharCopy,Position);        
        Position += CHAR_POSITION_INCREMENT;
      }
      else
      {
        //Do nothing
      }
    } 
    else 
    {
      //Do nothing
    }    
  }while (*SourceString != _NULL);
}

/**
  U08 _LCDModule_PrintMarquee(STRING SourceString, S16 StartPosition, U08 MaxPosition)
*/
U08 _SLCDModule_PrintMarquee(STRING SourceString, S16 StartPosition, U08 MaxPosition, U08 MinPosition)
{
  static U08 isFirstTime = TRUE;
  static U16 MarqueeLength = 0;
  U08 MarqueeEnd = FALSE;
  U08 StringCharCopy = 0;                            
  S08 Position = StartPosition; 
  STRING StringCopy = SourceString;
  STRING ReversedString;
  U16 StringLength = 0;
  
  StringLength = _SLCDModule_StringLength(StringCopy);  
  
  if ((isFirstTime) || (isMarqueeRestart))
  {
    MarqueeLength = StringLength * 7;
    isFirstTime = FALSE;
    isMarqueeRestart = FALSE;
  }
  
  if (StartPosition <= 0)
  {
    ReversedString = (STRING)(SourceString + (StringLength-1));   
  }

  if (MaxPosition == MARQUEE_WITH_ARROWS)
  {
    _SLCDModule_ClearLCD(ARROWS);
  }
  else if (MaxPosition == MARQUEE_WITHOUT_ARROWS)
  {
    _SLCDModule_ClearLCD(NO_ARROWS);
  }
  else
  {
    //nothing
  }
  
  do 
  {
    if (StartPosition > 0)
    {
      StringCharCopy = *SourceString;
      SourceString++;
    }
    else
    {
      StringCharCopy = *ReversedString;
      ReversedString--;      
    } 
    
    if (StringCharCopy > 0x80)
    {
      StringCharCopy = 0x80;
    } 
    else 
    {
      StringCharCopy -= 0x20;      
    }
    
    if (Fonts[StringCharCopy] != _NULL)
    {
      _SLCDModule_MarqueeWFRegisters(StringCharCopy,Position,MaxPosition,MinPosition);        
      if (StartPosition > 0)
      {
        Position += CHAR_POSITION_INCREMENT;
      }
      else
      {
        Position -= CHAR_POSITION_INCREMENT;
      }
    } 
    else 
    {
      //Do nothing
    }    
  }while (--StringLength);
  
  MarqueeLength--;
  if (MarqueeLength == 0)
  {
    MarqueeEnd = 1;
    isFirstTime = TRUE;
  }
  return MarqueeEnd;
}

/**
  void _LCDModule_StringWFRegisters(U08 FrontPlaneValues, U08 FrontPlanePosition)
*/
void _SLCDModule_StringWFRegisters(U08 FrontPlaneValues, U08 FrontPlanePosition)
{
  U08 ColumnCounter = 0;
  U08 CharArrayCounter = 0;
  U32 WaveFormRegister = 0;
  U08 BackPlane = 0;
  U08 SegmentOnOff = 0;  
  U32 Registro = 0xFFFFFFFF;
    
  do
  {
    if (ColumnCounter > MAX_COLUMNS_PER_CHAR)
    {
      BackPlane = _SLCDModule_SelectBackPlanePhase();
      ColumnCounter = 0; 
      FrontPlanePosition -= CHAR_RESET_POSITION;
    }
    
    WaveFormRegister = _SLCDModule_CopyWFRegister(FrontPlanePosition);
       
    SegmentOnOff = Fonts[FrontPlaneValues][CharArrayCounter];           
    if (SegmentOnOff == 0)
    {
      Registro = ~(1 << (BackPlane + WaveFormShiftRegisterTable[FrontPlanePosition]));   
      WaveFormRegister &= Registro;
    }
    else
    {
      WaveFormRegister |= (SegmentOnOff << (BackPlane + WaveFormShiftRegisterTable[FrontPlanePosition]));   
    }
    WaveFormRegister |= (SegmentOnOff << (BackPlane + WaveFormShiftRegisterTable[FrontPlanePosition]));
      
    _SLCDModule_WriteWFRegister(WaveFormRegister,FrontPlanePosition);         
      
    CharArrayCounter++;
    ColumnCounter++;
    FrontPlanePosition++;
  }while (CharArrayCounter < CHAR_SIZE);
}

/**
  void _LCDModule_MarqueeWFRegisters(U08 FrontPlaneValues, S16 FrontPlanePosition, U08 MaxPosition)
*/
void _SLCDModule_MarqueeWFRegisters(U08 FrontPlaneValues, S16 FrontPlanePosition, U08 MaxPosition, U08 MinPosition)
{
  U08 ColumnCounter = 0;
  U08 CharArrayCounter = 0;
  U32 WaveFormRegister = 0;
  U08 BackPlane = 0;
  U08 SegmentOnOff = 0;  
  U32 Registro = 0xFFFFFFFF;
  
  if (MaxPosition == MARQUEE_WITHOUT_ARROWS)
  {
    MaxPosition = MARQUEE_MAX_POSITION_WITHOUT_ARROW;
  } 
  else if (MaxPosition == MARQUEE_WITH_ARROWS)
  {
    MaxPosition = MARQUEE_MAX_POSITION_WITH_ARROW;
  } 
  else 
  {
    //
  }    
  
  if (MinPosition == MARQUEE_WITHOUT_ARROWS)
  {
    MinPosition = MARQUEE_MIN_POSITION_WITHOUT_ARROW;
  } 
  else if (MinPosition == MARQUEE_WITH_ARROWS)
  {
    MinPosition = MARQUEE_MIN_POSITION_WITH_ARROW;
  } 
  else 
  {
    //
  }   
    
  do
  {
    if (ColumnCounter > MAX_COLUMNS_PER_CHAR)
    {
      BackPlane = _SLCDModule_SelectBackPlanePhase();
      ColumnCounter = 0; 
      FrontPlanePosition -= CHAR_RESET_POSITION;
    }
    
    if ((FrontPlanePosition >= MinPosition) && (FrontPlanePosition < MaxPosition))
    {      
      WaveFormRegister = _SLCDModule_CopyWFRegister(FrontPlanePosition);       

      SegmentOnOff = Fonts[FrontPlaneValues][CharArrayCounter];         
      if (SegmentOnOff == 0)
      {
        Registro = ~(1 << (BackPlane + WaveFormShiftRegisterTable[FrontPlanePosition]));   
        WaveFormRegister &= Registro;
      }
      else
      { 
        WaveFormRegister |= (SegmentOnOff << (BackPlane + WaveFormShiftRegisterTable[FrontPlanePosition]));   
      }
      WaveFormRegister |= (SegmentOnOff << (BackPlane + WaveFormShiftRegisterTable[FrontPlanePosition]));   

      _SLCDModule_WriteWFRegister(WaveFormRegister,FrontPlanePosition);               
    }
    
    CharArrayCounter++;
    ColumnCounter++;
    FrontPlanePosition++;
  }while (CharArrayCounter < CHAR_SIZE);
}

/**
  void _LCDModule_PrintUpperArrow(void)
*/
void _SLCDModule_PrintUpperArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF35TO32;
  WFRegisterCopy2 = LCD_WF39TO36;

  WFRegisterCopy1 |= (U32)(1 << (PHASE_C + 16));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_B + 24));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_C + 24));
 
  WFRegisterCopy2 |= (U32)(1 << (PHASE_A));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_B));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_C));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_B + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_C + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_C + 16));
  
  LCD_WF35TO32 = WFRegisterCopy1;
  LCD_WF39TO36 = WFRegisterCopy2;
}

/**
  void _LCDModule_PrintLowerArrow(void)
*/
void _SLCDModule_PrintLowerArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF35TO32;
  WFRegisterCopy2 = LCD_WF39TO36;
 
  WFRegisterCopy1 |= (U32)(1 << (PHASE_F + 16));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_F + 24));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_G + 24));
 
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_G));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_H));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_G + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F + 16));
  
  LCD_WF35TO32 = WFRegisterCopy1;
  LCD_WF39TO36 = WFRegisterCopy2;
}

void _SLCDModule_PrintLeftArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
 
  WFRegisterCopy1 = LCD_WF3TO0;
 
  WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 16));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_C + 24));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 24));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_E + 24));
  
  LCD_WF3TO0 = WFRegisterCopy1;
}

void _SLCDModule_PrintRightArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
 
  WFRegisterCopy1 = LCD_WF39TO36;
 
  WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 16));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_C + 8));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 8));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_E + 8));
  
  LCD_WF39TO36 = WFRegisterCopy1;
}
/**
  void _LCDModule_PrintUpperRCArrow(void)
*/
void _SLCDModule_PrintUpperRCArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF27TO24;
  WFRegisterCopy2 = LCD_WF31TO28;
 
  WFRegisterCopy1 |= (U32)(1 << (PHASE_E + 24));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_F + 24));
 
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_C + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 16));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 16));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F + 16));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 24));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F + 24));
  
  LCD_WF27TO24 = WFRegisterCopy1;
  LCD_WF31TO28 = WFRegisterCopy2;
}

/**
  void _LCDModule_PrintLowerRCArrow(void)
*/
void _SLCDModule_PrintLowerRCArrow(void)
{ 
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF27TO24;
  WFRegisterCopy2 = LCD_WF31TO28;
  
  WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 24));
  WFRegisterCopy1 |= (U32)(1 << (PHASE_E + 24));
 
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_G + 8));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 16));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 16));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_F + 16));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 24));
  WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 24));
  
  LCD_WF27TO24 = WFRegisterCopy1;
  LCD_WF31TO28 = WFRegisterCopy2;
}

/**
  void _LCDModule_EraseUpperArrow(void)
*/
void _SLCDModule_EraseUpperArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF35TO32;
  WFRegisterCopy2 = LCD_WF39TO36;

  WFRegisterCopy1 &= ~(1 << (PHASE_C + 16));
  WFRegisterCopy1 &= ~(1 << (PHASE_B + 24));
  WFRegisterCopy1 &= ~(1 << (PHASE_C + 24));
 
  WFRegisterCopy2 &= ~(1 << (PHASE_A));
  WFRegisterCopy2 &= ~(1 << (PHASE_B));
  WFRegisterCopy2 &= ~(1 << (PHASE_C));
  WFRegisterCopy2 &= ~(1 << (PHASE_B + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_C + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_C + 16));
  
  LCD_WF35TO32 = WFRegisterCopy1;
  LCD_WF39TO36 = WFRegisterCopy2;
}

/** 
  void _LCDModule_EraseLowerArrow(void)
*/
void _SLCDModule_EraseLowerArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF35TO32;
  WFRegisterCopy2 = LCD_WF39TO36;
 
  WFRegisterCopy1 &= ~(1 << (PHASE_F + 16));
  WFRegisterCopy1 &= ~(1 << (PHASE_F + 24));
  WFRegisterCopy1 &= ~(1 << (PHASE_G + 24));
 
  WFRegisterCopy2 &= ~(1 << (PHASE_F));
  WFRegisterCopy2 &= ~(1 << (PHASE_G));
  WFRegisterCopy2 &= ~(1 << (PHASE_H));
  WFRegisterCopy2 &= ~(1 << (PHASE_F + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_G + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_F + 16));
  
  LCD_WF35TO32 = WFRegisterCopy1;
  LCD_WF39TO36 = WFRegisterCopy2;
}

void _SLCDModule_EraseLeftArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
 
  WFRegisterCopy1 = LCD_WF3TO0;
  
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_D + 16));
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_C + 24));
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_D + 24));
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_E + 24));
 
  LCD_WF3TO0 = WFRegisterCopy1;
}


void _SLCDModule_EraseRightArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
 
  WFRegisterCopy1 = LCD_WF39TO36;
 
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_D + 16));
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_C + 8));
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_D + 8));
  WFRegisterCopy1 &= ~(U32)(1 << (PHASE_E + 8));
  
  LCD_WF39TO36 = WFRegisterCopy1;
}

/**
  void _LCDModule_EraseUpperRCArrow(void)
*/
void _SLCDModule_EraseUpperRCArrow(void)
{
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF27TO24;
  WFRegisterCopy2 = LCD_WF31TO28;
 
  WFRegisterCopy1 &= ~(1 << (PHASE_E + 24));
  WFRegisterCopy1 &= ~(1 << (PHASE_F + 24));
 
  WFRegisterCopy2 &= ~(1 << (PHASE_D));
  WFRegisterCopy2 &= ~(1 << (PHASE_E));
  WFRegisterCopy2 &= ~(1 << (PHASE_F));  
  WFRegisterCopy2 &= ~(1 << (PHASE_C + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_D + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_E + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_F + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_D + 16));
  WFRegisterCopy2 &= ~(1 << (PHASE_E + 16));
  WFRegisterCopy2 &= ~(1 << (PHASE_F + 16));
  WFRegisterCopy2 &= ~(1 << (PHASE_E + 24));
  WFRegisterCopy2 &= ~(1 << (PHASE_F + 24));
  
  LCD_WF27TO24 = WFRegisterCopy1;
  LCD_WF31TO28 = WFRegisterCopy2;
}

/**
  void _LCDModule_EraseLowerRCArrow(void)
*/
void _SLCDModule_EraseLowerRCArrow(void)
{ 
  U32 WFRegisterCopy1 = 0;   
  U32 WFRegisterCopy2 = 0;
 
  WFRegisterCopy1 = LCD_WF27TO24;
  WFRegisterCopy2 = LCD_WF31TO28;
  
  WFRegisterCopy1 &= ~(1 << (PHASE_D + 24));
  WFRegisterCopy1 &= ~(1 << (PHASE_E + 24));
 
  WFRegisterCopy2 &= ~(1 << (PHASE_D));
  WFRegisterCopy2 &= ~(1 << (PHASE_E));
  WFRegisterCopy2 &= ~(1 << (PHASE_F));
  WFRegisterCopy2 &= ~(1 << (PHASE_D + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_E + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_F + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_G + 8));
  WFRegisterCopy2 &= ~(1 << (PHASE_D + 16));
  WFRegisterCopy2 &= ~(1 << (PHASE_E + 16));
  WFRegisterCopy2 &= ~(1 << (PHASE_F + 16));
  WFRegisterCopy2 &= ~(1 << (PHASE_D + 24));
  WFRegisterCopy2 &= ~(1 << (PHASE_E + 24));
  
  LCD_WF27TO24 = WFRegisterCopy1;
  LCD_WF31TO28 = WFRegisterCopy2;
}                 

/**
  void _LCDMOdule_PrintDotMarquee(S08 StartPosition)
*/
void _SLCDMOdule_PrintDotMarquee(S08 StartPosition)
{
  U08 DotMarqueeCounter = 0;
  U08 BackPlane = PHASE_H;  
  S08 FrontPlanePosition = StartPosition;
  U32 WaveFormRegister = 0;
     
  do
  {
    if (FrontPlanePosition > 0)
    {      
      if ((WaveFormRegister = _SLCDModule_CopyWFRegister(FrontPlanePosition)) != _NULL)
      {                             
        WaveFormRegister &= WaveFormRegisterClearTable[FrontPlanePosition]; 
        WaveFormRegister |= (1 << (BackPlane + WaveFormShiftRegisterTable[FrontPlanePosition]));
      
        _SLCDModule_WriteWFRegister(WaveFormRegister,FrontPlanePosition);         
      }
    } 
    FrontPlanePosition++;
    DotMarqueeCounter++;
  }while (DotMarqueeCounter < DOT_MARQUEE_SIZE);
}

/**
  void _LCDModule_PrintRCBlocks(U08 Position)
*/
void _SLCDModule_PrintRCBlocks(U08 Position)
{
  U32 WFRegisterCopy1 = 0;
  U32 WFRegisterCopy2 = 0;
  
  if (Position == UPPER_LEFT) 
  {
    WFRegisterCopy1 = LCD_WF3TO0;
    WFRegisterCopy2 = LCD_WF7TO4;
      
    WFRegisterCopy1 |= (U32)(1 << (PHASE_A + 16));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_A + 24));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_B + 16));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_B + 24));
    
    WFRegisterCopy2 |= (U32)(1 << (PHASE_A));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_A + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_B));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_B + 8));
    
    LCD_WF3TO0 = WFRegisterCopy1;
    LCD_WF7TO4 = WFRegisterCopy2;
  } 
  else if (Position == MIDDLE_LEFT) 
  {
    WFRegisterCopy1 = LCD_WF3TO0;
    WFRegisterCopy2 = LCD_WF7TO4;

    WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 16));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 24));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_E + 16));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_E + 24));
    
    WFRegisterCopy2 |= (U32)(1 << (PHASE_D));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_E));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 8));  

    LCD_WF3TO0 = WFRegisterCopy1;
    LCD_WF7TO4 = WFRegisterCopy2;
  } 
  else if (Position == LOWER_LEFT) 
  {
    WFRegisterCopy1 = LCD_WF3TO0;
    WFRegisterCopy2 = LCD_WF7TO4;

    WFRegisterCopy1 |= (U32)(1 << (PHASE_G + 16));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_G + 24));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_H + 16));
    WFRegisterCopy1 |= (U32)(1U << (PHASE_H + 24));
    
    WFRegisterCopy2 |= (U32)(1 << (PHASE_G));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_G + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_H));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_H + 8));  

    LCD_WF3TO0 = WFRegisterCopy1;
    LCD_WF7TO4 = WFRegisterCopy2;
  } 
  else if (Position == UPPER_RIGHT) 
  {
    WFRegisterCopy1 = LCD_WF35TO32;
    WFRegisterCopy2 = LCD_WF39TO36;
    
    WFRegisterCopy1 |= (U32)(1 << (PHASE_A + 24));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_B + 24));
        
    WFRegisterCopy2 |= (U32)(1 << (PHASE_A));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_A + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_A + 16));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_B));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_B + 8));  
    WFRegisterCopy2 |= (U32)(1 << (PHASE_B + 16));
    
    LCD_WF35TO32 = WFRegisterCopy1;
    LCD_WF39TO36 = WFRegisterCopy2;
  } 
  else if (Position == MIDDLE_RIGHT) 
  {
    WFRegisterCopy1 = LCD_WF35TO32;
    WFRegisterCopy2 = LCD_WF39TO36;

    WFRegisterCopy1 |= (U32)(1 << (PHASE_D + 24));
    WFRegisterCopy1 |= (U32)(1 << (PHASE_E + 24));
        
    WFRegisterCopy2 |= (U32)(1 << (PHASE_D));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_D + 16));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_E));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_E + 16));

    LCD_WF35TO32 = WFRegisterCopy1;
    LCD_WF39TO36 = WFRegisterCopy2;
  } 
  else if (Position == LOWER_RIGHT) 
  {
    WFRegisterCopy1 = LCD_WF35TO32;
    WFRegisterCopy2 = LCD_WF39TO36;

    WFRegisterCopy1 |= (U32)(1U << (PHASE_G + 24));
    WFRegisterCopy1 |= (U32)(1U << (PHASE_H + 24));
    
    WFRegisterCopy2 |= (U32)(1 << (PHASE_G));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_G + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_G + 16));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_H));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_H + 8));
    WFRegisterCopy2 |= (U32)(1 << (PHASE_H + 16));
  
    LCD_WF35TO32 = WFRegisterCopy1;
    LCD_WF39TO36 = WFRegisterCopy2;  
  }
}

/**
  void _LCDModule_EraseRCBlocks(U08 Position)
*/
void _SLCDModule_EraseRCBlocks(U08 Position)
{
  U32 WFRegisterCopy1 = 0;
  U32 WFRegisterCopy2 = 0;
  
  if (Position == UPPER_LEFT) 
  {
    WFRegisterCopy1 = LCD_WF3TO0;
    WFRegisterCopy2 = LCD_WF7TO4;
      
    WFRegisterCopy1 &= ~(1 << (PHASE_A + 16));
    WFRegisterCopy1 &= ~(1 << (PHASE_A + 24));
    WFRegisterCopy1 &= ~(1 << (PHASE_B + 16));
    WFRegisterCopy1 &= ~(1 << (PHASE_B + 24));
    
    WFRegisterCopy2 &= ~(1 << (PHASE_A));
    WFRegisterCopy2 &= ~(1 << (PHASE_A + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_B));
    WFRegisterCopy2 &= ~(1 << (PHASE_B + 8));
    
    LCD_WF3TO0 = WFRegisterCopy1;
    LCD_WF7TO4 = WFRegisterCopy2;
  } 
  else if (Position == MIDDLE_LEFT) 
  {
    WFRegisterCopy1 = LCD_WF3TO0;
    WFRegisterCopy2 = LCD_WF7TO4;

    WFRegisterCopy1 &= ~(1 << (PHASE_D + 16));
    WFRegisterCopy1 &= ~(1 << (PHASE_D + 24));
    WFRegisterCopy1 &= ~(1 << (PHASE_E + 16));
    WFRegisterCopy1 &= ~(1 << (PHASE_E + 24));
    
    WFRegisterCopy2 &= ~(1 << (PHASE_D));
    WFRegisterCopy2 &= ~(1 << (PHASE_D + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_E));
    WFRegisterCopy2 &= ~(1 << (PHASE_E + 8));  

    LCD_WF3TO0 = WFRegisterCopy1;
    LCD_WF7TO4 = WFRegisterCopy2;
  } 
  else if (Position == LOWER_LEFT) 
  {
    WFRegisterCopy1 = LCD_WF3TO0;
    WFRegisterCopy2 = LCD_WF7TO4;

    WFRegisterCopy1 &= ~(1U << (PHASE_G + 16));
    WFRegisterCopy1 &= ~(1U << (PHASE_G + 24));
    WFRegisterCopy1 &= ~(1U << (PHASE_H + 16));
    WFRegisterCopy1 &= ~(1U << (PHASE_H + 24));
    
    WFRegisterCopy2 &= ~(1 << (PHASE_G));
    WFRegisterCopy2 &= ~(1 << (PHASE_G + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_H));
    WFRegisterCopy2 &= ~(1 << (PHASE_H + 8));  

    LCD_WF3TO0 = WFRegisterCopy1;
    LCD_WF7TO4 = WFRegisterCopy2;
  } 
  else if (Position == UPPER_RIGHT) 
  {
    WFRegisterCopy1 = LCD_WF35TO32;
    WFRegisterCopy2 = LCD_WF39TO36;
    
    WFRegisterCopy1 &= ~(1 << (PHASE_A + 24));
    WFRegisterCopy1 &= ~(1 << (PHASE_B + 24));
    
    WFRegisterCopy2 &= ~(1 << (PHASE_A));
    WFRegisterCopy2 &= ~(1 << (PHASE_A + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_A + 16));
    WFRegisterCopy2 &= ~(1 << (PHASE_B));
    WFRegisterCopy2 &= ~(1 << (PHASE_B + 8));  
    WFRegisterCopy2 &= ~(1 << (PHASE_B + 16));
    
    LCD_WF35TO32 = WFRegisterCopy1;
    LCD_WF39TO36 = WFRegisterCopy2;
  } 
  else if (Position == MIDDLE_RIGHT) 
  {
    WFRegisterCopy1 = LCD_WF35TO32;
    WFRegisterCopy2 = LCD_WF39TO36;

    WFRegisterCopy1 &= ~(1 << (PHASE_D + 24));
    WFRegisterCopy1 &= ~(1 << (PHASE_E + 24));
    
    WFRegisterCopy2 &= ~(1 << (PHASE_D));
    WFRegisterCopy2 &= ~(1 << (PHASE_D + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_D + 16));    
    WFRegisterCopy2 &= ~(1 << (PHASE_E));
    WFRegisterCopy2 &= ~(1 << (PHASE_E + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_E + 16));

    LCD_WF35TO32 = WFRegisterCopy1;
    LCD_WF39TO36 = WFRegisterCopy2;
  } 
  else if (Position == LOWER_RIGHT) 
  {
    WFRegisterCopy1 = LCD_WF35TO32;
    WFRegisterCopy2 = LCD_WF39TO36;

    WFRegisterCopy1 &= ~(1U << (PHASE_G + 24));
    WFRegisterCopy1 &= ~(1U << (PHASE_H + 24));
     
    WFRegisterCopy2 &= ~(1 << (PHASE_G));
    WFRegisterCopy2 &= ~(1 << (PHASE_G + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_G + 16));
    WFRegisterCopy2 &= ~(1 << (PHASE_H));
    WFRegisterCopy2 &= ~(1 << (PHASE_H + 8));
    WFRegisterCopy2 &= ~(1 << (PHASE_H + 16));
  
    LCD_WF35TO32 = WFRegisterCopy1;
    LCD_WF39TO36 = WFRegisterCopy2;  
  }
}

/**
  void _LCDModule_TurnOnUSBSign(void)
*/
void _SLCDModule_TurnOnUSBSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;   
  WFRegisterCopy |= (1 << (PHASE_B + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOnWirelessSign(void)
*/
void _SLCDModule_TurnOnWirelessSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;   
  WFRegisterCopy |= (1 << (PHASE_A + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOnPounceSign(void)
*/
void _SLCDModule_TurnOnPounceSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF39TO36;   
  WFRegisterCopy |= (1 << (PHASE_F + 24)); 
  LCD_WF39TO36 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOnFreescaleSign(void)
*/
void _SLCDModule_TurnOnFreescaleSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;  
  WFRegisterCopy |= (1 << (PHASE_E + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOnClockSign(void)
*/
void _SLCDModule_TurnOnClockSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;   
  WFRegisterCopy |= (1 << (PHASE_D + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOnJLinkSign(void)
*/
void _SLCDModule_TurnOnJLinkSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;  
  WFRegisterCopy |= (1 << (PHASE_F + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOffUSBSign(void)
*/
void _SLCDModule_TurnOffUSBSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;  
  WFRegisterCopy &= ~(1 << (PHASE_B + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOffWirelessSign(void) 
*/
void _SLCDModule_TurnOffWirelessSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;   
  WFRegisterCopy &= ~(1 << (PHASE_A + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOffPounceSign(void)
*/
void _SLCDModule_TurnOffPounceSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF39TO36;   
  WFRegisterCopy &= ~(1 << (PHASE_F + 24)); 
  LCD_WF39TO36 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOffFreescaleSign(void)
*/
void _SLCDModule_TurnOffFreescaleSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;  
  WFRegisterCopy &= ~(1 << (PHASE_E + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOffClockSign(void)
*/
void _SLCDModule_TurnOffClockSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;
  WFRegisterCopy &= ~(1 << (PHASE_D + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _LCDModule_TurnOffJLinkSign(void)
*/
void _SLCDModule_TurnOffJLinkSign(void)
{
  U32 WFRegisterCopy = 0;
  
  WFRegisterCopy = LCD_WF3TO0;   
  WFRegisterCopy &= ~(1 << (PHASE_F + 8));
  LCD_WF3TO0 = WFRegisterCopy;
}

/**
  void _SLCDModule_TurnOnBatteryOutline(void)
*/
void _SLCDModule_TurnOnBatteryOutline(void)
{
  U32 WFRegisterCopy1 = 0;
  WFRegisterCopy1 = LCD_WF3TO0;   
  
  WFRegisterCopy1 |= (U32)(1 << (PHASE_G + 8)); //Battery Outline 

  LCD_WF3TO0 = WFRegisterCopy1;
}

/**
  void _LCDModule_TurnOnBattery(U08 BatteryLevel)
*/
void _SLCDModule_TurnOnBattery(U08 BatteryLevel)
{
  U32 WFRegisterCopy1 = 0;
  U32 WFRegisterCopy2 = 0;
  
  WFRegisterCopy1 = LCD_WF3TO0;   
  WFRegisterCopy2 = LCD_WF39TO36;   
  
  if (BatteryLevel == CHARGED) 
  {    
    WFRegisterCopy1 |= (U32)(1U << (PHASE_H +  8)); //Battery Upper Block
    WFRegisterCopy2 |= (U32)(1U << (PHASE_G + 24)); //Battery Middle Block
    WFRegisterCopy2 |= (U32)(1U << (PHASE_H + 24)); //Battery Lower Block
  }
    
  if (BatteryLevel == MID_LEVEL) 
  {      
    WFRegisterCopy2 |= (U32)(1U << (PHASE_G + 24)); //Battery Middle Block
    WFRegisterCopy2 |= (U32)(1U << (PHASE_H + 24)); //Battery Lower Block
  }
  
  if (BatteryLevel == LOWER_LEVEL)
  {
    WFRegisterCopy2 |= (U32)(1U << (PHASE_H + 24)); //Battery Lower Block
  }
  
  LCD_WF3TO0 = WFRegisterCopy1;
  LCD_WF39TO36 = WFRegisterCopy2;  
}

/**
  void _LCDModule_TurnOffBattery(void)
*/
void _SLCDModule_TurnOffBattery(void)
{
  U32 WFRegisterCopy1 = 0;
  U32 WFRegisterCopy2 = 0;
  
  WFRegisterCopy1 = LCD_WF3TO0;  
  WFRegisterCopy2 = LCD_WF39TO36;
  
  WFRegisterCopy1 &= ~(1U << (PHASE_G + 8)); //Battery Outline 
  WFRegisterCopy1 &= ~(1U << (PHASE_H + 8)); //Battery Upper Block
  WFRegisterCopy2 &= ~(1U << (PHASE_G + 24)); //Battery Middle Block
  WFRegisterCopy2 &= ~(1U << (PHASE_H + 24)); //Battery Lower Block
  
  LCD_WF3TO0 = WFRegisterCopy1;
  LCD_WF39TO36 = WFRegisterCopy2;  
}

/**
  void _LCDModule_SetSegment(U08 X, U08 Y)
*/
void _SLCDModule_SetSegment(U08 X, U08 Y)
{
  U32 WFRegisterCopy = 0;
  
  if (X <= CHAR_MAX_POSITION_VALUE)
  {
    WFRegisterCopy = _SLCDModule_CopyWFRegister(X);
    
    WFRegisterCopy &= WaveFormRegisterClearTable[X];  

    if (Y <= 7)
    {
      //do nothing
    } 
    else 
    {
      Y = 0;
    }

    WFRegisterCopy |= (1 << (Y + WaveFormShiftRegisterTable[X]));   

    _SLCDModule_WriteWFRegister(WFRegisterCopy,X); 
  }
}

/**
  void _LCDModule_ClearSegment(U08 X, U08 Y)
*/
void _SLCDModule_ClearSegment(U08 X, U08 Y)
{
  U32 WFRegisterCopy = 0;
  
  if (X <= CHAR_MAX_POSITION_VALUE)
  {
    WFRegisterCopy = _SLCDModule_CopyWFRegister(X);
  
    WFRegisterCopy &= WaveFormRegisterClearTable[X]; 
    WFRegisterCopy |= (0 << (Y + WaveFormShiftRegisterTable[X]));            
  
    _SLCDModule_WriteWFRegister(WFRegisterCopy,X); 
  }
}

/**
  void _LCDMoudle_TurnOffAllSegments(void)
*/
void _SLCDMoudle_TurnOffAllSegments(void)
{
  LCD_WF3TO0   = 0x00000000;
  LCD_WF7TO4   = 0x00000000;
  LCD_WF11TO8  = 0x00000000;
  LCD_WF15TO12 = 0x00000000;
  LCD_WF19TO16 = 0x00000000;
  LCD_WF23TO20 = 0x00000000;
  LCD_WF27TO24 = 0x00000000;
  LCD_WF31TO28 = 0x00000000;
  LCD_WF35TO32 = 0x00000000;
  LCD_WF39TO36 = 0x00000000;
}

/**
  void _LCDMoudle_TurnOnAllSegments(void) 
*/
void _SLCDMoudle_TurnOnAllSegments(void) 
{
  LCD_WF3TO0   = 0xFFFFFF00;
  LCD_WF7TO4   = 0xFFFFFFFF;
  LCD_WF11TO8  = 0xFFFFFFFF;
  LCD_WF15TO12 = 0xFFFFFFFF;
  LCD_WF19TO16 = 0xFFFFFFFF;
  LCD_WF23TO20 = 0xFFFFFFFF;
  LCD_WF27TO24 = 0xFFFFFFFF;
  LCD_WF31TO28 = 0xFFFFFFFF;
  LCD_WF35TO32 = 0xFFFFFFFF;
  LCD_WF39TO36 = 0xFFFFFFFF;
}

/**
  void _LCDModule_ClearLCD(U08 ArrowsOn) 
*/
void _SLCDModule_ClearLCD(U08 ArrowsOn) 
{
  if (ArrowsOn == ARROWS)
  {
    LCD_WF3TO0   &= 0xFFFFFF00;
    LCD_WF7TO4   &= 0x00000000;
    LCD_WF11TO8  &= 0x00000000;
    LCD_WF15TO12 &= 0x00000000;
    LCD_WF19TO16 &= 0x00000000;
    LCD_WF23TO20 &= 0x00000000;
    LCD_WF27TO24 &= 0x00000000;
    LCD_WF31TO28 &= 0x00000000;
    LCD_WF35TO32 &= 0x00000000;     
    LCD_WF39TO36 &= 0xFFFFFF00;  
  }
  else if (ArrowsOn == NO_ARROWS)
  {
    LCD_WF3TO0   &= 0x0000FF00;
    LCD_WF7TO4   &= 0x00000000;
    LCD_WF11TO8  &= 0x00000000;
    LCD_WF15TO12 &= 0x00000000;
    LCD_WF19TO16 &= 0x00000000;
    LCD_WF23TO20 &= 0x00000000;
    LCD_WF27TO24 &= 0x00000000;
    LCD_WF31TO28 &= 0x00000000;
    LCD_WF35TO32 &= 0x00000000;
    LCD_WF39TO36 &= 0xFF000000;  
  }
  else
  {
  }
}

/**
  void _LCDModule_WriteWFRegister(U32 WFRegister, U08 Position)
*/
void _SLCDModule_WriteWFRegister(U32 WFRegister, U08 Position)
{
  if (((Position + 2) >= FRONT_PLANE_0_TO_3) && ((Position + 2) < FRONT_PLANE_4_TO_7))
  {
    LCD_WF3TO0 = WFRegister;
  } 
  else if (((Position + 2) >= FRONT_PLANE_4_TO_7) && ((Position + 2) < FRONT_PLANE_8_TO_11)) 
  {
    LCD_WF7TO4 = WFRegister;
  }
  else if (((Position + 2) >= FRONT_PLANE_8_TO_11) && ((Position + 2) < FRONT_PLANE_12_TO_15)) 
  {
    LCD_WF11TO8 = WFRegister;  
  }
  else if (((Position + 2) >= FRONT_PLANE_12_TO_15) && ((Position + 2) < FRONT_PLANE_16_TO_19)) 
  {
    LCD_WF15TO12 = WFRegister;
  }
  else if (((Position + 2) >= FRONT_PLANE_16_TO_19) && ((Position + 2) < FRONT_PLANE_20_TO_23)) 
  {
    LCD_WF19TO16 = WFRegister;
  }
  else if (((Position + 2) >= FRONT_PLANE_20_TO_23) && ((Position + 2) < FRONT_PLANE_24_TO_27)) 
  {
    LCD_WF23TO20 = WFRegister;
  }
  else if (((Position + 2) >= FRONT_PLANE_24_TO_27) && ((Position + 2) < FRONT_PLANE_28_TO_31)) 
  {
    LCD_WF27TO24 = WFRegister;
  }
  else if (((Position + 2) >= FRONT_PLANE_28_TO_31) && ((Position + 2) < FRONT_PLANE_32_TO_35)) 
  {
    LCD_WF31TO28 = WFRegister;
  }
  else if (((Position + 2) >= FRONT_PLANE_32_TO_35) && ((Position + 2) < FRONT_PLANE_36_TO_39)) 
  {
    LCD_WF35TO32 = WFRegister;
  }
  else if (((Position + 2) >= FRONT_PLANE_36_TO_39) && ((Position + 2) < FRONT_PLANE_40_TO_43)) 
  {
    LCD_WF39TO36 = WFRegister;
  } 
  else 
  {
    //do nothing
  }
}

/**
  U32 _LCDModule_CopyWFRegister(U08 Position)
*/
U32 _SLCDModule_CopyWFRegister(U08 Position)
{
  U32 WFRegister = 0;
  if (((Position + 2) >= FRONT_PLANE_0_TO_3) && ((Position + 2) < FRONT_PLANE_4_TO_7))
  {
    WFRegister = LCD_WF3TO0;
  } 
  else if (((Position + 2) >= FRONT_PLANE_4_TO_7) && ((Position + 2) < FRONT_PLANE_8_TO_11)) 
  {
    WFRegister = LCD_WF7TO4;
  }
  else if (((Position + 2) >= FRONT_PLANE_8_TO_11) && ((Position + 2) < FRONT_PLANE_12_TO_15)) 
  {
    WFRegister = LCD_WF11TO8;
  }
  else if (((Position + 2) >= FRONT_PLANE_12_TO_15) && ((Position + 2) < FRONT_PLANE_16_TO_19)) 
  {
    WFRegister = LCD_WF15TO12;
  }
  else if (((Position + 2) >= FRONT_PLANE_16_TO_19) && ((Position + 2) < FRONT_PLANE_20_TO_23)) 
  {
    WFRegister = LCD_WF19TO16;
  }
  else if (((Position + 2) >= FRONT_PLANE_20_TO_23) && ((Position + 2) < FRONT_PLANE_24_TO_27)) 
  {
    WFRegister = LCD_WF23TO20;
  }
  else if (((Position + 2) >= FRONT_PLANE_24_TO_27) && ((Position + 2) < FRONT_PLANE_28_TO_31)) 
  {
    WFRegister = LCD_WF27TO24;
  }
  else if (((Position + 2) >= FRONT_PLANE_28_TO_31) && ((Position + 2) < FRONT_PLANE_32_TO_35)) 
  {
    WFRegister = LCD_WF31TO28;
  }
  else if (((Position + 2) >= FRONT_PLANE_32_TO_35) && ((Position + 2) < FRONT_PLANE_36_TO_39)) 
  {
    WFRegister = LCD_WF35TO32;
  }
  else   
  {
    if (((Position + 2) >= FRONT_PLANE_36_TO_39) && ((Position + 2) < FRONT_PLANE_40_TO_43)) 
    {
      WFRegister = LCD_WF39TO36;
    } 
    else 
    {
      WFRegister = _NULL;
    }
  }
  return  WFRegister;  
}

/**
  U08 _LCDModule_SelectBackPlanePhase(void)
*/
U08 _SLCDModule_SelectBackPlanePhase(void)
{
  static U08 CurrentBackPlanePhase = 1;
  U08 BackPlane = 0;
  
  if (CurrentBackPlanePhase == 1)
  {
    BackPlane = PHASE_B;  
    CurrentBackPlanePhase++;
  }
  else if (CurrentBackPlanePhase == 2)
  {
    BackPlane = PHASE_C;  
    CurrentBackPlanePhase++;
  }
  else if (CurrentBackPlanePhase == 3)
  {
    BackPlane = PHASE_D;  
    CurrentBackPlanePhase++;
  }
  else if (CurrentBackPlanePhase == 4)
  {
    BackPlane = PHASE_E;  
    CurrentBackPlanePhase++;
  }
  else if (CurrentBackPlanePhase == 5)
  {
    BackPlane = PHASE_F;  
    CurrentBackPlanePhase++;
  }
  else if (CurrentBackPlanePhase == 6)
  {
    BackPlane = PHASE_G;          
    CurrentBackPlanePhase = 1;
    //CurrentBackPlane++;
  }
  
  /**
    Not used for printing strings
  
  else if (CurrentBackPlanePhase == 7)
  {
    BackPlane = PHASE_H;  
    CurrentBackPlanePhase = 0;
  }*/ 
  else
  {
    BackPlane = PHASE_A;
    CurrentBackPlanePhase = 0;
  }
     
  return BackPlane;
}

/**
  U16 _LCDModule_StringLength(STRING SourceString)
*/
U16 _SLCDModule_StringLength(STRING SourceString)
{
  U16 StrLen = 0;
  while (*SourceString++ != _NULL)
  {
    StrLen++;
  }
  return StrLen;
}

void _SLCDModule_PrintNumber(U08 Number, U08 Position)
{
  if (Number < 10)
  {
    U08 ColumnCounter = 0;
    U08 CharArrayCounter = 0;
    U32 WaveFormRegister = 0;
    U08 BackPlane = 0;
    U08 SegmentOnOff = 0;  
    U32 Registro = 0xFFFFFFFF;
    
    //_SLCDModule_ClearLCD(NO_ARROWS);
    
    do
    {
      if (ColumnCounter > MAX_COLUMNS_PER_CHAR)
      {
        BackPlane = _SLCDModule_SelectBackPlanePhase();
        ColumnCounter = 0; 
        Position -= CHAR_RESET_POSITION;
      }
      
      if (Position < CHAR_MAX_POSITION_VALUE)
      {      
        WaveFormRegister = _SLCDModule_CopyWFRegister(Position);       
  
        SegmentOnOff = Fonts[Number + 0x10][CharArrayCounter];         
        //WaveFormRegister &= WaveFormRegisterClearTable[Position]; 
        if (SegmentOnOff == 0)
        {
          Registro = ~(1 << (BackPlane + WaveFormShiftRegisterTable[Position]));   
          WaveFormRegister &= Registro;
        }
        else
        {
          WaveFormRegister |= (SegmentOnOff << (BackPlane + WaveFormShiftRegisterTable[Position]));   
        }
        _SLCDModule_WriteWFRegister(WaveFormRegister,Position);               
      }
      
      CharArrayCounter++;
      ColumnCounter++;
      Position++;
    }while (CharArrayCounter < CHAR_SIZE);      
  }
  else
  {
    _SLCDModule_ClearLCD(NO_ARROWS);
    _SLCDModule_PrintString(Error,4);
  }

}
