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

#ifndef _POUNCE_TYPES_H_
#define _POUNCE_TYPES_H_	
    
    /**--------------------------------------
    \brief Redefinition of types
    ---------------------------------------*/
    typedef unsigned char  U08;
    typedef unsigned short U16;
    typedef unsigned long  U32;
    typedef unsigned long long U64;
        
    typedef signed char  S08;
    typedef signed short S16;
    typedef signed long  S32;
    typedef signed long long S64;
        
    typedef unsigned char BOOL;
    
    typedef unsigned char * STRING;
    
    /**--------------------------------------
    \brief Bits handling
    ---------------------------------------*/
    typedef union
    {
        U08 u8Flags;
        struct
        {
            U08 _b0  :1;
            U08 _b1  :1;
            U08 _b2  :1;
            U08 _b3  :1;
            U08 _b4  :1;
            U08 _b5  :1;
            U08 _b6  :1;
            U08 _b7  :1;
        }_sBits;
    }_uFlags;
        
    /**--------------------------------------
    \brief Bytes handling
    ---------------------------------------*/
    typedef union
    {
        U32 u32Long;
        U16 u16Short[2];
        U08 u8Byte[4];
    }_uBytes;

    /**--------------------------------------
    \brief State  machine
    ---------------------------------------*/        
    typedef struct 
    {
    	  U08 u8PrevState;
    	  U08 u8NextState;
    	  U08 u8ActualState;
    	  U08 u8ErrorState;
    }_sSM;
            
    /**--------------------------------------
    \brief Manipulation of bits
    ---------------------------------------*/  
    #define SET_BIT(var, bit)                 (var) |= (1<<(bit))
    #define CLEAR_BIT(var, bit)               (var) &= ~(1<<(bit))
    #define QUERY_BIT(var, bit)               (var) & (1<<(bit))
    #define TOGGLE_BIT(var, bit)              (var) ^= (1<<(bit))
    
    /**--------------------------------------
    \brief Pin values
    ---------------------------------------*/       
    #define OUTPUT	(1)
    #define INTPUT	(0)
    
    /**--------------------------------------
    \brief Standard macros
    ---------------------------------------*/      
    #define WAIT()        _asm wait
    #define _NOP          asm(NOP)
    #define _BGND         asm(BGND)
    
    /**--------------------------------------
    \brief Error values
    ---------------------------------------*/      
    #define ERROR   (1)
    #define NoERROR (0)	
    
    /**--------------------------------------
    \brief Boolean values
    ---------------------------------------*/  
    #ifndef TRUE
        #define TRUE  (1)
    #endif

    #ifndef FALSE
        #define FALSE (0)
 	  #endif
    
    /**--------------------------------------
    \brief Function pointer
    ---------------------------------------*/        
    typedef void (* pFunc_t) (void);

#endif /** _POUNCE_TYPES_H_ */
