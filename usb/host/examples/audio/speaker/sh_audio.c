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
*   This file contains the MQX shell commands for audio devices.
*
*
*END************************************************************************/
#include "sh_audio.h"
#include "sd_card.h"
#include "wav.h"
#include "shell.h"
#include <string.h>
#include <hwtimer.h>
#include "audio_speaker.h"

/******************************************************************************
* Global variables
*****************************************************************************/
FILE_PTR file_ptr;
extern volatile AUDIO_STREAM_DEVICE_STRUCT audio_stream;
extern char  device_direction;  /* audio speaker or audio microphone */
extern uint32_t                                 packet_size;
extern USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR   frm_type_desc;
extern uint8_t                                  audio_state;
extern uint8_t                                  sd_card_state;
extern uint8_t                                  file_open_count;
extern HWTIMER                                 audio_timer;
/******************************************************************************
* Local variables
*****************************************************************************/
extern AUDIO_COMMAND audio_com;
extern uint8_t wav_buff[MAX_ISO_PACKET_SIZE];
/******************************************************************************
*   @name        Shell_play
*
*   @brief       Servers the play command
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to play an audio wav file
*****************************************************************************/
int32_t Shell_play(int32_t argc, char *argv[])
{
    bool print_usage, shorthelp = FALSE;
    uint8_t bSamFreqType_index;
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

    if (!print_usage)
    {
        WAVE_FILE_HEADER header;
        if (argc > 1)
        {
            /* stop the current file playing */
            if(AUDIO_PLAYING == audio_state)
            {
                audio_state = AUDIO_IDLE;
                hwtimer_stop(&audio_timer);
            }
            /* check the device is connected */
            if ((USB_DEVICE_INUSE != audio_stream.DEV_STATE)||(device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
            {
                printf("  Error: Audio Speaker is not connected\n");
                return (SHELL_EXIT_ERROR);
            }
            if (FillWaveHeader(argv[1], &header) != 0)
            {
                printf("  Error: Unable to open file: %s\n", argv[1]);
                return (SHELL_EXIT_ERROR);
            }
            if (strcmp(header.CHUNK_DESCRIPTOR.Format, "WAVE"))
            {
                printf("  Error: File is not WAVE file.\n");
                return (SHELL_EXIT_ERROR);
            }
            if (strcmp(header.FMT_SUBCHUNK.Subchunk1ID, "fmt "))
            {
                printf("  Error: File does not contain format subchunk.\n");
            }
            if (BYTESWAP16(header.FMT_SUBCHUNK.AudioFormat) != 1)
            {
                printf("  Error: File is compressed (not PCM).\n");
                return (SHELL_EXIT_ERROR);
            }
            if (strcmp(header.DATA_SUBCHUNK.Subchunk2ID, "data"))
            {
                printf("  Error: File does not contain data subchunk.\n");
                return (SHELL_EXIT_ERROR);
            }
            file_ptr = fopen(argv[1], "r");
            if (file_ptr == NULL)
            {
                printf("  Unable to open file: %s\n", argv[1]);
                return (SHELL_EXIT_ERROR);
            }
            file_open_count ++;
            printf("Audio file properties:\n");
            printf("   - Sample rate      : %d Hz\n", BYTESWAP32(header.FMT_SUBCHUNK.SampleRate));
            printf("   - Sample size      : %d bits\n", BYTESWAP16(header.FMT_SUBCHUNK.BitsPerSample));
            printf("   - Number of channels : %d channels\n", BYTESWAP16(header.FMT_SUBCHUNK.NumChannels));
            /* Compare the sample rate */
            for (bSamFreqType_index =0; bSamFreqType_index < frm_type_desc->bSamFreqType; bSamFreqType_index++)
            {
                if (((frm_type_desc->tSamFreq[3*bSamFreqType_index + 2] << 16) |
                            (frm_type_desc->tSamFreq[3*bSamFreqType_index + 1] << 8)  |
                            (frm_type_desc->tSamFreq[3*bSamFreqType_index] << 0)) == BYTESWAP32(header.FMT_SUBCHUNK.SampleRate))
                {
                    packet_size = USB_Audio_Get_Packet_Size(frm_type_desc,bSamFreqType_index);
                    break;
                }
            }
            if (bSamFreqType_index == frm_type_desc->bSamFreqType)
            {
                printf("  The audio device doesn't support that audio sample rate \n");
                return (SHELL_EXIT_ERROR);
            }
            /* Compare the bits sample number */
            if (frm_type_desc->bBitResolution != BYTESWAP16(header.FMT_SUBCHUNK.BitsPerSample))
            {
                printf("  The audio device doesn't support that audio bit sample number \n");
                return (SHELL_EXIT_ERROR);
            }
            /* Compare the channel number */
            if (frm_type_desc->bNrChannels != BYTESWAP16(header.FMT_SUBCHUNK.NumChannels))
            {
                printf("  The audio device doesn't support that audio channel number \n");
                return (SHELL_EXIT_ERROR);
            }
            fseek(file_ptr, WAVE_HEADER_SIZE, IO_SEEK_SET);
            audio_state = AUDIO_PLAYING;
            printf("  Playing...\n");
            hwtimer_start(&audio_timer);
        }
        else
        {
            if (AUDIO_PLAYING == audio_state)
            {
                printf("  The file is playing...\n");
            }
            else if (AUDIO_PAUSE == audio_state)
            {
                audio_state = AUDIO_PLAYING;
                hwtimer_start(&audio_timer);
                printf("  Playing...\n");
            }
            else if (AUDIO_IDLE == audio_state)
            {
                printf("  Not enough parameters.\n");
            }
        }
    }
    else
    {
        if (shorthelp)
        {
            printf("%s <filename>\n", argv[0]);
        }
        else
        {
            printf("Usage: %s <filename>\n", argv[0]);
            printf("   filename      = wav file to play\n");
        }
    }
    return(SHELL_EXIT_SUCCESS);
}
/******************************************************************************
*   @name        Shell_pause
*
*   @brief       Servers the pause command
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to pause the current playing file
*****************************************************************************/
int32_t Shell_pause_audio(int32_t argc, char *argv[])
{
    bool print_usage, shorthelp = FALSE;
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

    if (!print_usage)
    {
        if (argc > 1)
        {
            printf("  Error: This command doesn't need parameters\n");
            return (SHELL_EXIT_ERROR);
        }
        else
        {
            if(AUDIO_PLAYING == audio_state)
            {
                audio_state = AUDIO_PAUSE;
                printf("  Paused...\n");
                hwtimer_stop(&audio_timer);
            }
            else if (AUDIO_IDLE == audio_state)
            printf("  No file is playing!\n");
        }
    }

    if (print_usage)
    {
        if (shorthelp)
        {
            printf("%s\n", argv[0]);
        }
        else
        {
            printf("Usage: %s\n", argv[0]);
        }
    }
    return(SHELL_EXIT_SUCCESS);
}

/******************************************************************************
*   @name        Shell_mute
*
*   @brief       Servers the mute command
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to mute the audio device
*****************************************************************************/
int32_t Shell_mute(int32_t argc, char *argv[])
{
    bool         print_usage, shorthelp = FALSE;
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);

    if (!print_usage)
    {
        if (argc > 1)
        {
            printf("  Error: This command doesn't need parameters\n");
            return (SHELL_EXIT_ERROR);
        }
        else
        {
            audio_mute_command();
        }
    }

    if (print_usage)
    {
        if (shorthelp)
        {
            printf("%s\n", argv[0]);
        }
        else
        {
            printf("Usage: %s\n", argv[0]);
        }
    }
    return(SHELL_EXIT_SUCCESS);
}

/******************************************************************************
*   @name        Shell_increase_volume
*
*   @brief       Servers the increase volume command
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to increase volume of the audio device
*****************************************************************************/
int32_t Shell_increase_volume(int32_t argc,char *argv[])
{
    bool         print_usage = FALSE, shorthelp = FALSE;
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);
    if (!print_usage)
    {
        if (argc > 1)
        {
            printf ("  Error: Channel parameter is too large\n");
            return (SHELL_EXIT_ERROR);
        }
        else
        {
            audio_increase_volume_command();
        }
    }
    else
    {
        if (shorthelp)
        {
            printf("%s\n", argv[0]);
        }
        else
        {
            printf("Usage: %s\n", argv[0]);
        }
    }

    return(SHELL_EXIT_SUCCESS);
}

/******************************************************************************
*   @name        Shell_decrease_volume
*
*   @brief       Servers the decrease volume command
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to decrease volume of the audio device
*****************************************************************************/
int32_t Shell_decrease_volume(int32_t argc,char *argv[])
{
    bool         print_usage = TRUE, shorthelp = FALSE;
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);
    if (!print_usage)
    {
        if (argc > 1)
        {
            printf ("  Error: Channel parameter is too large\n");
            return (SHELL_EXIT_ERROR);
        }
        else
        {
            audio_decrease_volume_command();
        }
    }
    else
    {
        if (shorthelp)
        {
            printf("%s\n", argv[0]);
        }
        else
        {
            printf("Usage: %s\n", argv[0]);
        }
    }

    return(SHELL_EXIT_SUCCESS);
}
/* EOF */
