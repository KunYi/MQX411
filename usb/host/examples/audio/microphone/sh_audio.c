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
#include "wav.h"
#include "stdlib.h"
#include <shell.h>
#include <string.h>
#include <lwevent.h>
#include "audio_microphone.h"
#include <hwtimer.h>
/***************************************
**
** Global variables
****************************************/
extern uint8_t                                  wav_sdcard_buff0[REC_BLOCK_SIZE];
extern uint8_t                                  wav_sdcard_buff1[REC_BLOCK_SIZE];
/* audio speaker or audio microphone */
extern char  device_direction;
extern ENDPOINT_DESCRIPTOR_PTR                 endp;
extern volatile AUDIO_CONTROL_DEVICE_STRUCT    audio_control;
extern volatile AUDIO_STREAM_DEVICE_STRUCT     audio_stream;
extern volatile uint8_t *g_cur_mute;
extern volatile uint8_t *g_cur_vol;
extern volatile uint8_t *g_cur_samp;
extern HWTIMER audio_timer;
uint32_t                                       packet_remainder = 0;
bool                                           final_packet = FALSE;
LWEVENT_STRUCT_PTR                             block_rec;

/***************************************
**
** Local variables
****************************************/
extern AUDIO_COMMAND audio_com;
/*FUNCTION****************************************************************
*
* Function Name    : Shell_play
* Returned Value   : SHELL_EXIT_SUCCESS if everything is ok,
*                    SHELL_EXIT_ERROR otherwise
* Comments         :
*    Shell function for recording wave files
*
*END*********************************************************************/
int32_t Shell_record(int32_t argc, char *argv[])
{
   uint32_t ev;
   bool print_usage, shorthelp = FALSE;
   int32_t return_code = SHELL_EXIT_SUCCESS;
   uint32_t requests = 0;
   print_usage = Shell_check_help_request (argc, argv, &shorthelp);
   if (!print_usage)
   {
      if (argc > 2)
      {
         WAVE_FILE_HEADER header;
         FILE_PTR file_ptr;
         _mqx_int errcode = 0, result = 0;
         uint32_t i = 0;

         if ((USB_DEVICE_INUSE != audio_stream.DEV_STATE)||\
             (device_direction != USB_AUDIO_DEVICE_DIRECTION_OUT))
         {
             printf("  Error: Audio Microphone is not connected\n");
             return (SHELL_EXIT_ERROR);
         }
         if (((endp->bmAttributes>>2)&EP_TYPE_MASK)!=ISOCH_NOSYNC)
         {
             printf("  Error: This sync type of the device is not supported\n");
             return (SHELL_EXIT_ERROR);
         }
         /* Set sampling frequency for microphone device */
         if (USB_OK != usb_class_audio_endpoint_command(&audio_com,(void *)g_cur_samp,USB_AUDIO_SET_CUR_SAMPLING_FREQ))
                printf("\n Set sampling frequency error");

         /* Fill header struct according to input parameters */
         /* write header to file */
         errcode = SetWaveHeader(&header, argv[1],argv[2]);
         switch (errcode)
         {
            case -1:
               printf("  Error: Cannot create file %s.\n", argv[1]);
               return (SHELL_EXIT_ERROR);
            case -2:
               printf("  Error: Wrong format specified.\n");
               return (SHELL_EXIT_ERROR);
            default:
               break;
         }
         printf("Audio file properties:\n");
         printf("   - Sample rate      : %d Hz\n", \
                  header.FMT_SUBCHUNK.SampleRate);
         printf("   - Sample size      : %d bits\n", \
                  header.FMT_SUBCHUNK.BitsPerSample);
         printf("   - Number of channels : %d channels\n", \
                  header.FMT_SUBCHUNK.NumChannels);
         /* Open file with header and make it ready for data writing */
         file_ptr = fopen(argv[1], "ab");
         if (file_ptr == NULL)
         {
            printf("  Unable to open file: %s\n", argv[1]);
            fclose(file_ptr);
            return (SHELL_EXIT_ERROR);
         }
         fseek(file_ptr, WAVE_HEADER_SIZE, IO_SEEK_SET);
           /* Compute requested number of data blocks */
         requests = header.DATA_SUBCHUNK.Subchunk2Size / REC_BLOCK_SIZE;
         packet_remainder = header.DATA_SUBCHUNK.Subchunk2Size % REC_BLOCK_SIZE;

         block_rec = _mem_alloc_zero(sizeof(LWEVENT_STRUCT));
         if(block_rec == NULL) {
            printf("  Out of memory error.");
            fclose(file_ptr);
            return (SHELL_EXIT_ERROR);
         }

         result = _lwevent_create(block_rec, LWEVENT_AUTO_CLEAR);
         if (result != MQX_OK) {
            printf("  Error: Cannot create event required for waiting block "
                    "record ready.\n");
            fclose(file_ptr);
            _task_set_error(result);
            return (SHELL_EXIT_ERROR);
         }
         printf("  Recording...");
         hwtimer_start(&audio_timer);
         _time_delay (50);
         for(i = 0; i < requests; i++)
         {
            result = _lwevent_wait_ticks(block_rec, EV_BLOCK0_W_READY | EV_BLOCK1_W_READY, FALSE, 0);
            if (result != MQX_OK)
            {
                printf("  Error: Wait for event block record ready failed.\n");
                fclose(file_ptr);
                _task_set_error(result);
                return (SHELL_EXIT_ERROR);
            }
            ev = _lwevent_get_signalled();
            if (ev & EV_BLOCK0_W_READY) {
               result = fwrite(wav_sdcard_buff0, 1, sizeof(wav_sdcard_buff0), file_ptr);
            }
            else if (ev & EV_BLOCK1_W_READY) {
               result = fwrite(wav_sdcard_buff1, 1, sizeof(wav_sdcard_buff1), file_ptr);
            }
            if (result != REC_BLOCK_SIZE)
            {
                    printf("  Error: Write failed. Wrote %d Bytes instead of %d.\n",\
                                    result, REC_BLOCK_SIZE);
            }
         }

         final_packet = TRUE;
         /* Alloc rest of data */
         if(packet_remainder != 0)
         {
            result = _lwevent_wait_ticks(block_rec, EV_BLOCK0_W_READY | EV_BLOCK1_W_READY, FALSE, 0);
            if (result != MQX_OK)
            {
                printf("  Error: Wait for event block record ready failed.\n");
                fclose(file_ptr);
                _task_set_error(result);
                return (SHELL_EXIT_ERROR);
            }
            ev = _lwevent_get_signalled();
            if (ev & EV_BLOCK0_W_READY) {
               result = fwrite(wav_sdcard_buff0, 1, packet_remainder, file_ptr);
            }
            else if (ev & EV_BLOCK1_W_READY) {
               result = fwrite(wav_sdcard_buff1, 1, packet_remainder, file_ptr);
            }
            if (result != packet_remainder)
            {
                    printf("  Error: Write failed. Wrote %d Bytes instead of %d.\n",\
                                    result, packet_remainder);
            }
         }
         fclose(file_ptr);
         hwtimer_stop(&audio_timer);
         result = _lwevent_destroy(block_rec);
         if (MQX_OK != result)
         {
            printf("Error: Unable to free sdwrite_event.\n");
         }
         if(NULL != block_rec)
             _mem_free(block_rec);
         printf("done\n");
         final_packet = FALSE;

      }
      else
      {
         printf("  Not enough parameters.\n");
      }
   }

   if (print_usage)
   {
      if (shorthelp)
      {
         printf("%s <filename> <length>\n", argv[0]);
      }
      else
      {
         printf("Usage: %s <filename> <length>\n", argv[0]);
         printf("   filename      = filename for recording\n");
         printf("   length        = length in seconds\n");
      }
   }
   return(return_code);
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
            if ((USB_DEVICE_INUSE != audio_stream.DEV_STATE)||\
                (device_direction != USB_AUDIO_DEVICE_DIRECTION_OUT))
            {
                printf("  Error: Audio Microphone is not connected\n");
                return (SHELL_EXIT_ERROR);
            }
            g_cur_mute[0] = !g_cur_mute[0];
            if (g_cur_mute[0])
            {
                printf("Mute ...\n");
            }
            else
            {
                printf("UnMute ...\n");
            }
            /* Send set mute request */
            audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
            audio_com.CALLBACK_FN = usb_host_audio_mute_ctrl_callback;
            audio_com.CALLBACK_PARAM = 0;
            usb_class_audio_feature_command(&audio_com,0,(void *)g_cur_mute,USB_AUDIO_SET_CUR_MUTE);
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
            printf("Usage: %s", argv[0]);
        }
    }
    return(SHELL_EXIT_SUCCESS);
}
/* EOF */
