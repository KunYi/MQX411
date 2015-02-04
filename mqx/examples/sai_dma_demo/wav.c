/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains code for operations with WAVE files 
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include "wav.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>

/*FUNCTION****************************************************************
* 
* Function Name    : GetWaveHeader
* Returned Value   : Zero if everything is ok, non zero otherwise
* Comments         :
*    This function reads wave file header and saves it into struct for
*    further usage.
*
*END*********************************************************************/

_mqx_int GetWaveHeader
   (
    /* [IN] file to read header from */
    char *filename, 
                       
    /* [OUT] header pointer to save header to */
    WAVE_FILE_HEADER_PTR header
   )
{  
   FILE_PTR file;
   unsigned char def[] = {sizeof(header->CHUNK_DESCRIPTOR.ChunkSize), 0};
   WAVE_CHUNK_DESCRIPTOR_PTR ch_des = &(header->CHUNK_DESCRIPTOR);
   WAVE_FMT_SUBCHUNK_PTR fmt_sch = &(header->FMT_SUBCHUNK);
   WAVE_DATA_SUBCHUNK_PTR data_sch = &(header->DATA_SUBCHUNK);
   uint32_t retval = 0;
   file = fopen(filename, "rb");
   if (file == NULL)
   {
      return(-1);
   }
   /* First chunk (descriptor) */
   retval = fread(&(ch_des->ChunkID), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   ch_des->ChunkID[4] = '\0'; 
   retval = fread(&(ch_des->ChunkSize), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      _mem_swap_endian(def, &(ch_des->ChunkSize));
   }
   retval = fread(&(ch_des->Format), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   ch_des->Format[4] = '\0';
   /* Second chunk (format) */
   retval = fread(&(fmt_sch->Subchunk1ID), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   fmt_sch->Subchunk1ID[4] = '\0';
   retval = fread(&(fmt_sch->Subchunk1Size), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(fmt_sch->Subchunk1Size);
      _mem_swap_endian(def, &(fmt_sch->Subchunk1Size));
   }
   retval = fread(&(fmt_sch->AudioFormat), 1, 2, file);
   if (retval < 2)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(fmt_sch->AudioFormat);
      _mem_swap_endian(def, &(fmt_sch->AudioFormat));
   }
   retval = fread(&(fmt_sch->NumChannels), 1, 2, file);
   if (retval < 2)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(fmt_sch->NumChannels);
      _mem_swap_endian(def, &(fmt_sch->NumChannels));
   }
   retval = fread(&(fmt_sch->SampleRate), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(fmt_sch->SampleRate);
      _mem_swap_endian(def, &(fmt_sch->SampleRate));
   }
   retval = fread(&(fmt_sch->ByteRate), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(fmt_sch->ByteRate);
      _mem_swap_endian(def, &(fmt_sch->ByteRate));
   }
   retval = fread(&(fmt_sch->BlockAlign), 1, 2, file);
   if (retval < 2)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(fmt_sch->BlockAlign);
      _mem_swap_endian(def, &(fmt_sch->BlockAlign));
   }
   retval = fread(&(fmt_sch->BitsPerSample), 1, 2, file);
   if (retval < 2)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(fmt_sch->BitsPerSample);
      _mem_swap_endian(def, &(fmt_sch->BitsPerSample)); 
   }
   retval = fread(&(data_sch->Subchunk2ID), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   /* Third chunk (data) */
   data_sch->Subchunk2ID[4] = '\0';
   retval = fread(&(data_sch->Subchunk2Size), 1, 4, file);
   if (retval < 4)
   {
       return(-1); 
   }
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(data_sch->Subchunk2Size);
      _mem_swap_endian(def, &(data_sch->Subchunk2Size));
   }
   fclose(file);
   return(0);
}


/*FUNCTION****************************************************************
* 
* Function Name    : SetWaveHeader
* Returned Value   : Zero if everything is ok, non zero otherwise
* Comments         :
*    This function creates header according to format and writes it into
*    output file.
*
*END*********************************************************************/
_mqx_int SetWaveHeader
   (
    /* [IN] format (length_in_seconds:sampling_freq:bit_depth:channels) */
    char *format, 
    
    /* [OUT] header pointer to save header */
    WAVE_FILE_HEADER_PTR header, 
    
    /* [IN] file for writing */
    uint8_t *buffer
   )
{
   int32_t values[4];
   uint32_t tmp32 = 0;
   uint16_t tmp16 = 0;
   uint8_t i = 0;
   char *tmp;
   unsigned char def[] = {0, 0};
   uint32_t retval = 0;
   WAVE_CHUNK_DESCRIPTOR_PTR ch_des = &(header->CHUNK_DESCRIPTOR);
   WAVE_FMT_SUBCHUNK_PTR fmt_sch = &(header->FMT_SUBCHUNK);
   WAVE_DATA_SUBCHUNK_PTR data_sch = &(header->DATA_SUBCHUNK);
   tmp = strtok(format, ":");
   while ((tmp != NULL) || (i < 4))
   {
      values[i] = atoi(tmp);
      if ((values[i] <= 0) || (values[i] == INT_MAX))
      {
         return(-2);
      }  
      tmp = strtok(NULL, ":");
      i++;
   }
   /* Fill header */
   strcpy(ch_des->ChunkID, "RIFF");
   strcpy(ch_des->Format, "WAVE");
   strcpy(fmt_sch->Subchunk1ID, "fmt ");
   fmt_sch->Subchunk1Size = 16;
   fmt_sch->AudioFormat = 1;
   fmt_sch->NumChannels = values[3];
   fmt_sch->SampleRate = values[1];
   fmt_sch->BitsPerSample = values[2];
   fmt_sch->ByteRate = fmt_sch->SampleRate * fmt_sch->NumChannels * fmt_sch->BitsPerSample/8;
   fmt_sch->BlockAlign = fmt_sch->NumChannels * fmt_sch->BitsPerSample/8;
   strcpy(data_sch->Subchunk2ID, "data");
   data_sch->Subchunk2Size = values[0] * fmt_sch->SampleRate * fmt_sch->NumChannels * fmt_sch->BitsPerSample/8;
   ch_des->ChunkSize = 4 + (8 + fmt_sch->Subchunk1Size) + (8 + data_sch->Subchunk2Size);
   
   /* Write header to file */
   /* Chunk descriptor  ("RIFF" string)*/
   _mem_copy(&(ch_des->ChunkID), buffer ,4);
   buffer += 4;
   /* Chunk size */
   tmp32 = ch_des->ChunkSize;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp32);
      _mem_swap_endian(def, &(tmp32));
   }
   _mem_copy(&tmp32, buffer, 4);
   buffer += 4;
   /* Format ("WAVE" string) */
   _mem_copy(&(ch_des->Format), buffer, 4);  
   buffer += 4;
   /* Format subchunk */
   _mem_copy(&(fmt_sch->Subchunk1ID), buffer, 4);
   buffer += 4;
   /* Subchunk size */
   tmp32 = fmt_sch->Subchunk1Size;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp32);
      _mem_swap_endian(def, &(tmp32));
   }
   _mem_copy(&tmp32, buffer, 4);
   buffer += 4;
   /*  Audio format */
   tmp16 = fmt_sch->AudioFormat;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp16);
      _mem_swap_endian(def, &(tmp16));
   }
   _mem_copy(&tmp16, buffer, 2);
   buffer += 2;
   /* Number of channels */
   tmp16 = fmt_sch->NumChannels;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp16);
      _mem_swap_endian(def, &(tmp16));
   }
   _mem_copy(&tmp16, buffer, 2);
   buffer += 2;
   /* Sample rate */
   tmp32 = fmt_sch->SampleRate;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp32);
      _mem_swap_endian(def, &(tmp32));
   }
   _mem_copy(&tmp32, buffer, 4);
   buffer += 4;
   /* Byte rate */
   tmp32 = fmt_sch->ByteRate;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp32);
      _mem_swap_endian(def, &(tmp32));
   }
   _mem_copy(&tmp32, buffer, 4);
   buffer += 4;
   /* Block align */
   tmp16 = fmt_sch->BlockAlign;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp16);
      _mem_swap_endian(def, &(tmp16));
   }
   _mem_copy(&tmp16, buffer, 2);
   buffer += 2;
   /* Bit depth */
   tmp16 = fmt_sch->BitsPerSample;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp16);
      _mem_swap_endian(def, &(tmp16));
   }
   _mem_copy(&tmp16, buffer, 2);
   buffer += 2;
   /* Data subchunk */
   _mem_copy(&(data_sch->Subchunk2ID), buffer, 4);
   buffer += 4;
   /* Subchunk size */
   tmp32 = data_sch->Subchunk2Size;
   if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   {
      def[0] = sizeof(tmp32);
      _mem_swap_endian(def, &(tmp32));
   }
   _mem_copy(&tmp32, buffer, 4);
   buffer += 4;
   return(0); 
}

/* EOF */
