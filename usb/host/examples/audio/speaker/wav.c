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
*   This file contains code for operations with WAVE files
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include "wav.h"

extern uint8_t                           file_open_count;
_mqx_int FillWaveHeader(char *filename, WAVE_FILE_HEADER_PTR header)
{
    FILE_PTR file;
    unsigned char def[] = {sizeof(header->CHUNK_DESCRIPTOR.ChunkSize), 0};
    /* CHUNK DESCRIPTOR */
    WAVE_CHUNK_DESCRIPTOR_PTR ch_des = &(header->CHUNK_DESCRIPTOR);
    /* FMT SUBCHUNK */
    WAVE_FMT_SUBCHUNK_PTR fmt_sch = &(header->FMT_SUBCHUNK);
    /* DATA SUBCHUNK */
    WAVE_DATA_SUBCHUNK_PTR data_sch = &(header->DATA_SUBCHUNK);
    file = fopen(filename, "r");
    if (file == NULL)
    {
        return(-1);
    }
    file_open_count ++;
    /* read Chunk ID */
    if (4 != fread(&(ch_des->ChunkID), 1, 4, file))
        return(-1);
    ch_des->ChunkID[4] = '\0';

    /* read  Chunk Size */
    if (4 != fread(&(ch_des->ChunkSize), 1, 4, file))
        return(-1);
    _mem_swap_endian(def, &(ch_des->ChunkSize));

    /* read  Chunk Format */
    if (4 != fread(&(ch_des->Format), 1, 4, file))
        return(-1);
    ch_des->Format[4] = '\0';

    /* read  Sub chunk1 ID */
    if (4 != fread(&(fmt_sch->Subchunk1ID), 1, 4, file))
        return(-1);
    fmt_sch->Subchunk1ID[4] = '\0';

    /* read  Sub chunk1 Size */
    if (4 != fread(&(fmt_sch->Subchunk1Size), 1, 4, file))
        return(-1);
    def[0] = sizeof(fmt_sch->Subchunk1Size);
    _mem_swap_endian(def, &(fmt_sch->Subchunk1Size));

    /* read  Audio Format */
    if (2 != fread(&(fmt_sch->AudioFormat), 1, 2, file))
        return(-1);
    def[0] = sizeof(fmt_sch->AudioFormat);
    _mem_swap_endian(def, &(fmt_sch->AudioFormat));

    /* read  Num Channels */
    if (2 != fread(&(fmt_sch->NumChannels), 1, 2, file))
        return(-1);
    def[0] = sizeof(fmt_sch->NumChannels);
    _mem_swap_endian(def, &(fmt_sch->NumChannels));

    /* read  Sample Rate */
    if (4 != fread(&(fmt_sch->SampleRate), 1, 4, file))
        return(-1);
    def[0] = sizeof(fmt_sch->SampleRate);
    _mem_swap_endian(def, &(fmt_sch->SampleRate));

    /* read  Byte Rate */
    if (4 != fread(&(fmt_sch->ByteRate), 1, 4, file))
        return(-1);
    def[0] = sizeof(fmt_sch->ByteRate);
    _mem_swap_endian(def, &(fmt_sch->ByteRate));

    /* read  Block Align */
    if (2 != fread(&(fmt_sch->BlockAlign), 1, 2, file))
        return(-1);
    def[0] = sizeof(fmt_sch->BlockAlign);
    _mem_swap_endian(def, &(fmt_sch->BlockAlign));

    /* read  Bits Per Sample*/
    if (2 != fread(&(fmt_sch->BitsPerSample), 1, 2, file))
        return(-1);
    def[0] = sizeof(fmt_sch->BitsPerSample);
    _mem_swap_endian(def, &(fmt_sch->BitsPerSample));

    /* read  Sub chunk2 ID*/
    if (4 != fread(&(data_sch->Subchunk2ID), 1, 4, file))
        return(-1);
    data_sch->Subchunk2ID[4] = '\0';

    /* read  Sub chunk2 Size*/
    if (4 != fread(&(data_sch->Subchunk2Size), 1, 4, file))
        return(-1);
    def[0] = sizeof(data_sch->Subchunk2Size);
    _mem_swap_endian(def, &(data_sch->Subchunk2Size));

    fclose(file);
    file_open_count --;
    return(0);
}
/* EOF */
