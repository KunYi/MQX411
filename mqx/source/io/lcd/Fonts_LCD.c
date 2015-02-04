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

#include "Fonts_LCD.h"

const U08 _0[CHAR_SIZE] = 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,1,1,
  1,0,1,0,1,
  1,1,0,0,1,
  1,0,0,0, 1,
  0,1,1,1,0,  
};
const U08 _1[CHAR_SIZE]= 
{
  0,0,1,0,0,
  0,1,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,1,1,1,0
};
const U08 _2[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  0,0,0,0,1,
  0,0,0,1,0,
  0,0,1,0,0,
  0,1,0,0,0,
  1,1,1,1,1
};
const U08 _3[CHAR_SIZE]= 
{
  1,1,1,1,1,
  0,0,0,1,0,
  0,0,1,0,0,
  0,0,0,1,0,
  0,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,0  
};
const U08 _4[CHAR_SIZE]= 
{
  0,0,0,1,0,
  0,0,1,1,0,
  0,1,0,1,0,
  1,0,0,1,0,
  1,1,1,1,1,
  0,0,0,1,0,
  0,0,0,1,0  
};
const U08 _5[CHAR_SIZE]= 
{
  1,1,1,1,1,
  1,0,0,0,0,
  1,1,1,1,0,
  0,0,0,0,1,
  0,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,0    
};
const U08 _6[CHAR_SIZE]= 
{
  0,0,1,1,0,
  0,1,0,0,0,
  1,0,0,0,0,
  1,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,0  
};
const U08 _7[CHAR_SIZE]= 
{
  1,1,1,1,1,
  0,0,0,0,1,
  0,0,0,1,0,
  0,0,1,0,0,
  0,1,0,0,0,
  0,1,0,0,0,
  0,1,0,0,0
};
const U08 _8[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,0    
};
const U08 _9[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,1,
  0,0,0,0,1,
  0,0,0,1,0,
  0,1,1,0,0
};
const U08 _A[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,1,1,1,1,
  1,0,0,0,1,
  1,0,0,0,1  
};
const U08 _B[CHAR_SIZE]= 
{
  1,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,1,1,1,0
};
const U08 _C[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,1,
  0,1,1,1,0
};
const U08 _D[CHAR_SIZE]= 
{
  1,1,1,0,0,
  1,0,0,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,1,0,
  1,1,1,0,0  
};
const U08 _E[CHAR_SIZE]= 
{
  1,1,1,1,1,
  1,0,0,0,0,
  1,0,0,0,0,
  1,1,1,1,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,1,1,1,1
};
const U08 _F[CHAR_SIZE]= 
{
  1,1,1,1,1,
  1,0,0,0,0,
  1,0,0,0,0,
  1,1,1,1,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,0
};
const U08 _G[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,0,
  1,0,1,1,1,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,1
};
const U08 _H[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,1,1,1,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1
};
const U08 _I[CHAR_SIZE]= 
{
  0,1,1,1,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,1,1,1,0  
};
const U08 _J[CHAR_SIZE]= 
{
  0,0,1,1,1,
  0,0,0,1,0,
  0,0,0,1,0,
  0,0,0,1,0,
  0,0,0,1,0,
  1,0,0,1,0,
  0,1,1,0,0
};
const U08 _K[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,1,0,
  1,0,1,0,0,
  1,1,0,0,0,
  1,0,1,0,0,
  1,0,0,1,0,
  1,0,0,0,1    
};
const U08 _L[CHAR_SIZE]= 
{
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,1,1,1,1
};
const U08 _M[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,1,0,1,1,
  1,0,1,0,1,
  1,0,1,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1
};
const U08 _N[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,0,1,
  1,1,0,0,1,
  1,0,1,0,1,
  1,0,0,1,1,
  1,0,0,0,1,
  1,0,0,0,1
};
const U08 _O[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,0  
};
const U08 _P[CHAR_SIZE]= 
{
  1,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,1,1,1,0,
  1,0,0,0,0,
  1,0,0,0,0,
  1,0,0,0,0    
};
const U08 _Q[CHAR_SIZE]= 
{
  0,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,1,0,1,
  1,0,0,1,0,
  0,1,1,0,1
};
const U08 _R[CHAR_SIZE]= 
{
  1,1,1,1,0,
  1,0,0,0,1,
  1,0,0,0,1,
  1,1,1,1,0,
  1,0,1,0,0,
  1,0,0,1,0,
  1,0,0,0,1  
};
const U08 _S[CHAR_SIZE]= 
{
  0,1,1,1,1,
  1,0,0,0,0,
  1,0,0,0,0,
  0,1,1,1,0,
  0,0,0,0,1,
  0,0,0,0,1,
  1,1,1,1,0
};
const U08 _T[CHAR_SIZE]= 
{
  1,1,1,1,1,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0
};
const U08 _U[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,1,1,0
};
const U08 _V[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,0,1,0,
  0,0,1,0,0
};
const U08 _W[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,1,0,1,
  1,0,1,0,1,
  1,0,1,0,1,
  0,1,0,1,0
};
const U08 _X[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,0,1,0,
  0,0,1,0,0,
  0,1,0,1,0,
  1,0,0,0,1,
  1,0,0,0,1
};
const U08 _Y[CHAR_SIZE]= 
{
  1,0,0,0,1,
  1,0,0,0,1,
  1,0,0,0,1,
  0,1,0,1,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0  
};
const U08 _Z[CHAR_SIZE]= 
{
  1,1,1,1,1,
  0,0,0,0,1,
  0,0,0,1,0,
  0,0,1,0,0,
  0,1,0,0,0,
  1,0,0,0,0,
  1,1,1,1,1
};

const U08 _Exclamation[CHAR_SIZE] = 
{
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,1,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,1,0,0  
};

const U08 _Dot[CHAR_SIZE] = 
{
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,1,1,0,0,
  0,1,1,0,0  
};

const U08 _Colon[CHAR_SIZE] = 
{
  0,0,0,0,0,
  0,1,1,0,0,
  0,1,1,0,0,
  0,0,0,0,0,
  0,1,1,0,0,
  0,1,1,0,0,
  0,0,0,0,0  
};

const U08 _SPACE[CHAR_SIZE] = 
{
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0  
};

const U08 * Fonts[] = 
{
  _SPACE,
  _Exclamation,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _Dot,
  _NULL,
  (U08 *)_0,
  (U08 *)_1,
  (U08 *)_2,
  (U08 *)_3,
  (U08 *)_4,
  (U08 *)_5,
  (U08 *)_6,
  (U08 *)_7,
  (U08 *)_8,
  (U08 *)_9,
  (U08 *)_Colon,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL, 
  (U08 *)_A,
  (U08 *)_B,
  (U08 *)_C,
  (U08 *)_D,
  (U08 *)_E,
  (U08 *)_F,
  (U08 *)_G,
  (U08 *)_H,
  (U08 *)_I,
  (U08 *)_J,
  (U08 *)_K,
  (U08 *)_L,
  (U08 *)_M,
  (U08 *)_N,
  (U08 *)_O,
  (U08 *)_P,
  (U08 *)_Q,
  (U08 *)_R,
  (U08 *)_S,
  (U08 *)_T,
  (U08 *)_U,
  (U08 *)_V,
  (U08 *)_W,
  (U08 *)_X,
  (U08 *)_Y,
  (U08 *)_Z,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL,
  _NULL
};
