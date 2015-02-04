/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   The MD5 Lookup Table
*
*
*END************************************************************************/

static uint32_t mdtab[] = {
   0xD76AA478l, 0xE8C7B756l, 0x242070DBl, 0xC1BDCEEEl,
   0xF57C0FAFl, 0x4787C62Al, 0xA8304613l, 0xFD469501l,
   0x698098D8l, 0x8B44F7AFl, 0xFFFF5BB1l, 0x895CD7BEl,
   0x6B901122l, 0xFD987193l, 0xA679438El, 0x49B40821l,
   0xF61E2562l, 0xC040B340l, 0x265E5A51l, 0xE9B6C7AAl,
   0xD62F105Dl, 0x02441453l, 0xD8A1E681l, 0xE7D3FBC8l,
   0x21E1CDE6l, 0xC33707D6l, 0xF4D50D87l, 0x455A14EDl,
   0xA9E3E905l, 0xFCEFA3F8l, 0x676F02D9l, 0x8D2A4C8Al,
   0xFFFA3942l, 0x8771F681l, 0x6D9D6122l, 0xFDE5380Cl,
   0xA4BEEA44l, 0x4BDECFA9l, 0xF6BB4B60l, 0xBEBFBC70l,
   0x289B7EC6l, 0xEAA127FAl, 0xD4EF3085l, 0x04881D05l,
   0xD9D4D039l, 0xE6DB99E5l, 0x1FA27CF8l, 0xC4AC5665l,
   0xF4292244l, 0x432AFF97l, 0xAB9423A7l, 0xFC93A039l,
   0x655B59C3l, 0x8F0CCC92l, 0xFFEFF47Dl, 0x85845DD1l,
   0x6FA87E4Fl, 0xFE2CE6E0l, 0xA3014314l, 0x4E0811A1l,
   0xF7537E82l, 0xBD3AF235l, 0x2AD7D2BBl, 0xEB86D391l
};

