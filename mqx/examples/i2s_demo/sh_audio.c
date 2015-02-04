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
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*
*END************************************************************************/

#include "sh_audio.h"
#include "wav.h"
#include <shell.h>
#include <string.h>
#include "sgtl5000.h"
#include "main.h"

/*FUNCTION****************************************************************
* 
* Function Name    : Shell_play
* Returned Value   : SHELL_EXIT_SUCCESS if everything is ok,
*                    SHELL_EXIT_ERROR otherwise
* Comments         :
*    Shell function for playing wave files 
*
*END*********************************************************************/

int32_t Shell_play(int32_t argc, char *argv[])
{
    bool print_usage, shorthelp = FALSE;
    int32_t return_code = SHELL_EXIT_SUCCESS;
    WAVE_FILE_HEADER header;
    AUDIO_DATA_FORMAT audio_format;
    FILE_PTR file_ptr, device_ptr;
    _mqx_int errcode = 0;
    uint16_t block_size = 0;
    char *data;
    uint32_t requested = 0;
    uint32_t  mclk_freq, fs_freq;
    I2S_STATISTICS_STRUCT stats;

    print_usage = Shell_check_help_request (argc, argv, &shorthelp);  
    
    /* Check if help should be printed out */
    if (print_usage)  
    {
        if (shorthelp)  
        {
            printf("%s <device> <filename>\n", argv[0]);
        } 
        else  
        {
            printf("Usage: %s <device> <filename>\n", argv[0]);
            printf("   device        = playback device (i.e. \"i2s:\")\n");
            printf("   filename      = wav file to play\n");
        }
        return(return_code);
    }
    /* Check if there is enough parameters */
    if (argc < 3)
    {
        printf("  Error: Not enough parameters.\n");
        return (SHELL_EXIT_ERROR);
    }
    /* Read header of file and fill header struct with data */
    if (GetWaveHeader(argv[2], &header) != 0)
    {
        printf("  Error: Unable to open a file: %s\n", argv[2]);
        return (SHELL_EXIT_ERROR);
    }
    /* Check if file is in WAVE format */
    if (strcmp(header.CHUNK_DESCRIPTOR.Format, "WAVE"))
    {
        printf("  Error: File is not a WAVE file.\n");
        return (SHELL_EXIT_ERROR);
    }
    /* Check if there is format subchunk */
    if (strcmp(header.FMT_SUBCHUNK.Subchunk1ID, "fmt "))
    {
        printf("  Error: File does not contain a format subchunk.\n");
    }
    /* Check if file is uncompressed */
    if (header.FMT_SUBCHUNK.AudioFormat != 1)
    {
        printf("  Error: File is compressed (not PCM).\n");
        return (SHELL_EXIT_ERROR);
    }
    /* Check if there is data subchunk */
    if (strcmp(header.DATA_SUBCHUNK.Subchunk2ID, "data"))
    {
        printf("  Error: File does not contain a data subchunk.\n");
        return (SHELL_EXIT_ERROR);
    }
    /* Open file so we can read from it*/
    file_ptr = fopen(argv[2], "r");
    if (file_ptr == NULL)
    {
        printf("  Unable to open a file: %s\n", argv[2]);
        return (SHELL_EXIT_ERROR);
    }
    /* Set file pointer to position behind header data */
    fseek(file_ptr, WAVE_HEADER_SIZE, IO_SEEK_SET);
    /* Set audio data format of file according to header (WAVE files are little endian) */
    audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
    audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
    audio_format.BITS = header.FMT_SUBCHUNK.BitsPerSample;
    audio_format.SIZE = header.FMT_SUBCHUNK.BitsPerSample/8;
    audio_format.CHANNELS = header.FMT_SUBCHUNK.NumChannels;
    /* Open audio device and set it up */
    if ((device_ptr = fopen(argv[1], NULL)) == NULL)
    {
        printf("  Error: Unable to open a device \"%s\".\n", argv[1]);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    /* Set master clock frequency so oversampling = 256 */
    mclk_freq = header.FMT_SUBCHUNK.SampleRate * CLK_MULT;
    /* Setup audio data format in device */
    if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_format) != I2S_OK)
    {
        printf("  Error: Input data format not supported.\n");
        fclose(device_ptr);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    /* Setup rest of parameters - master clock, valid data bits and sampling frequency */
    if (
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &audio_format.BITS) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &header.FMT_SUBCHUNK.SampleRate) != I2S_OK)
       )
    {
        printf("  Error: Unable to setup \"%s\" device driver.\n", argv[1]);
        fclose(device_ptr);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    /* Read back the sampling frequency to get real value */
    ioctl(device_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);
    /* Print some interesting information about playback */
    printf("  Playback information\n");
    printf("  Sampling frequency:     %d Hz\n", fs_freq);
    printf("  Bit depth:              %d bits\n", audio_format.BITS);
    printf("  Channels:               ");
    if (audio_format.CHANNELS == 1)
    {
        printf("mono\n");
    }
    else
    {
        printf("stereo\n");
    }
    printf("  Data rate:              %d kbyte/s\n", header.FMT_SUBCHUNK.ByteRate/1000);
    printf("  ----------------------------------------------\n"); 
    /* Set block size to 256 samples */
    block_size = audio_format.SIZE * 1024 * audio_format.CHANNELS;
    /* Allocate memory to store read data */
    data = (char *) _mem_alloc_zero(block_size);
    if (data == NULL)
    {
        printf("  Unable to allocate %u bytes.\n", block_size);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }

#if defined(BSP_HAS_AUDIO_CODEC)
    /* Setup audio codec */
    errcode = SetupCodec(device_ptr);
    if (errcode != 0)
    {
        printf("  Audio codec configuration failed. Error 0x%X.\n", errcode);
        fclose(device_ptr);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    /* Play a file */
    printf("  Playing a file...");
#else
    printf("  Audio codec is not present ");
#endif
    
    while (!feof(file_ptr))
    {
        
        requested = fread(data, 1, block_size, file_ptr);
        if (ferror(file_ptr) && !(feof(file_ptr)))
        {
            printf("\n  Error reading from a file %s.\n", argv[2]);
            break;
        }
        write(device_ptr, data, requested);
    }
    
    //ioctl(device_ptr, IO_IOCTL_I2S_TX_DUMMY_ON, NULL);
    //write(device_ptr, NULL, 4096*1024);
    /* Wait until transmit is done */
    fflush(device_ptr);
    printf("DONE\n");
    /* Print transfer statistics */
    if (ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats) != I2S_OK)
    {
        printf("  Error: Cannot read I2S statistics.\n");
    }
    else
    {
        printf("  ----------------------------------------------\n"); 
        printf("  Playback stats\n");
        printf("  Total interrupts:              %d\n", stats.INTERRUPTS);
        printf("  Bytes requested for transmit:  %d\n", stats.PACKETS_REQUESTED * audio_format.SIZE);
        printf("  Bytes transmitted:             %d\n", stats.TX_PACKETS * audio_format.SIZE);
        printf("  Underruns of hardware FIFO:    %d\n", stats.FIFO_ERROR);
        printf("  Software buffer empty:         %d\n", stats.BUFFER_ERROR);
        printf("  ----------------------------------------------\n");  
    }
    /* Clean up - close device and file, free allocated memory */
    if (fclose(device_ptr) != MQX_OK)
    {
        printf("  Error: Unable to close \"%s\" device driver.\n", argv[1]);
    }
    if (fclose(file_ptr) != MQX_OK)
    {
        printf("  Error: Unable to close file %s.\n", argv[2]);
    }
    _mem_free(data);
    
    return(return_code);
}
/*FUNCTION****************************************************************
* 
* Function Name    : Shell_record
* Returned Value   : SHELL_EXIT_SUCCESS if everything is ok,
*                    SHELL_EXIT_ERROR otherwise
* Comments         :
*    Shell function for recording wave files 
*
*END*********************************************************************/
int32_t Shell_record(int32_t argc, char *argv[])
{
    WAVE_FILE_HEADER header;
    AUDIO_DATA_FORMAT audio_format;
    FILE_PTR file_ptr, device_ptr;
    _mqx_int errcode = 0, result = 0;
    uint32_t  mclk_freq, fs_freq = 0;
    I2S_STATISTICS_STRUCT stats;
    uint32_t requests = 0, remainder = 0, i = 0;
    SD_WRITE_PARAM_STRUCT write_param;
    _task_id sdwrite_task_id;
    _pool_id msg_pool;
    REC_MESSAGE_PTR msg_ptr;
    bool print_usage, shorthelp = FALSE;
    int32_t return_code = SHELL_EXIT_SUCCESS;
    
    print_usage = Shell_check_help_request (argc, argv, &shorthelp);
    if (print_usage)  
    {
        if (shorthelp)  
        {
            printf("%s <device> <filename> <format>\n", argv[0]);
        } 
        else  
        {
            printf("Usage: %s <device> <filename> <format>\n", argv[0]);
            printf("   device        = recording device (i.e. \"i2s:\")\n");
            printf("   filename      = full filename for recording (i.e. \"a:\\test_record.vaw\")\n");
            printf("   format        = length_in_seconds:sampling_frequency:bit_depth:channels\n");
        }
        return(0);
    }
    
    if (argc < 3)
    {
        printf("  Not enough parameters.\n");
        return(SHELL_EXIT_ERROR);
    }
           
    /* Fill header struct according to input parameter and write header to file */ 
    errcode = SetWaveHeader(argv[3], &header, argv[2]);
    switch (errcode)
    {
        case -1:
        printf("  Error: Cannot create file %s.\n", argv[2]);
        return (SHELL_EXIT_ERROR);
        break;
        
        case -2:
        printf("  Error: Wrong format specified.\n");
        return (SHELL_EXIT_ERROR); 
        break;
        
        default:
        break;
    }
    
    /* Open file with header and make it ready for data writing */
    file_ptr = fopen(argv[2], "a");
    if (file_ptr == NULL)
    {
        printf("  Unable to open file: %s\n", argv[2]);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    fseek(file_ptr, WAVE_HEADER_SIZE, IO_SEEK_SET);
    
    /* Prepare audio format struct */
    audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
    audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
    audio_format.BITS = header.FMT_SUBCHUNK.BitsPerSample;
    audio_format.SIZE = header.FMT_SUBCHUNK.BitsPerSample/8;
    audio_format.CHANNELS = header.FMT_SUBCHUNK.NumChannels;
    
    /* Prepare device */
    if ((device_ptr = fopen(argv[1], "r")) == NULL)
    {
        printf("  Error: Unable to open device \"%s\".\n", argv[1]);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    /* Set master clock frequency so oversampling = 384 */
    mclk_freq = header.FMT_SUBCHUNK.SampleRate * CLK_MULT;
    
    /* Setup device */
    if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_format) != I2S_OK)
    {
        printf("  Error: Input data format not supported.\n");
        fclose(device_ptr);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    if (
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &audio_format.BITS) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &header.FMT_SUBCHUNK.SampleRate) != I2S_OK)
       )
    {
        printf("  Error: Unable to setup \"%s\" device driver.\n", argv[1]);
        fclose(device_ptr);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    ioctl(device_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);
    printf("  Recording information\n");
    printf("  Sampling frequency:     %d Hz\n", fs_freq);
    printf("  Bit depth:              %d bits\n", audio_format.BITS);
    printf("  Channels:               ");
    if (audio_format.CHANNELS == 1)
    {
        printf("mono\n");
    }
    else
    {
        printf("stereo\n");
    }
    printf("  Data rate:              %d kbyte/s\n", header.FMT_SUBCHUNK.ByteRate/1000);
    printf("  ----------------------------------------------\n");

#if defined(BSP_HAS_AUDIO_CODEC)
    /* Setup audio codec connected to device */
    errcode = SetupCodec(device_ptr);
    if (errcode != 0)
    {
        printf("  Audio codec configuration failed. Error 0x%X.\n", errcode);
        fclose(device_ptr);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
#else
    printf("  Audio codec is not present ");
#endif
    
    /* Compute requested number of data blocks */
    requests = header.DATA_SUBCHUNK.Subchunk2Size / REC_BLOCK_SIZE;
    remainder = header.DATA_SUBCHUNK.Subchunk2Size % REC_BLOCK_SIZE;
    if (remainder > 0)
    {
        requests++;
    } 
    /* Setup SD card write task parameter */
    write_param.file_ptr = file_ptr;
    result = _lwsem_create(&(write_param.sdwrite_sem), 0);
    if (result != MQX_OK)
    {
        printf("  Error: Cannot create event required for writing to SD card.\n");
        fclose(file_ptr);
        fclose(device_ptr);
        _task_set_error(result);
        return (SHELL_EXIT_ERROR);
    }

    /* Create SD card write task */
    sdwrite_task_id = _task_create(0, _task_get_template_index("sdwrite"), (uint32_t) &write_param);
    if (sdwrite_task_id == MQX_NULL_TASK_ID)
    {
        printf("  Error: Cannot create task required for writing to SD card.\n");
        fclose(file_ptr);
        fclose(device_ptr);
        _lwsem_destroy(&(write_param.sdwrite_sem));
        return (SHELL_EXIT_ERROR);
    }
    _time_delay (50);
    /* Create receive message pool */
    msg_pool = _msgpool_create(sizeof(REC_MESSAGE), 15, 1, 0);
    if (msg_pool == 0)
    {
        printf("  Error: Cannot create a message pool.\n");
        fclose(file_ptr);
        fclose(device_ptr);
        _lwsem_destroy(&(write_param.sdwrite_sem));
        return (SHELL_EXIT_ERROR);
    }

    printf("  Recording...");
    for(i = 0; i < requests; i++)
    {   
        /* Alloc data block */
        msg_ptr = (REC_MESSAGE_PTR) _msg_alloc(msg_pool);
        if (msg_ptr == NULL)
        {
            printf("\n  Error: Cannot create a message.");
            fclose(file_ptr);
            fclose(device_ptr);
            _lwsem_destroy(&(write_param.sdwrite_sem));
            result = _msgpool_destroy(msg_pool);
            if (result != MQX_OK)
            {
                printf("\n  Error: Cannot destroy a message pool.");
            }
            return (SHELL_EXIT_ERROR);
        }
        
        /* Fill message header, record data and send message to SD write task */
        msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, WRITE_QUEUE);
        msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0, WRITE_QUEUE);
        msg_ptr->HEADER.SIZE = sizeof(REC_MESSAGE); 
        if (i != (requests - 1) && (remainder != 0))
        {
            msg_ptr->LENGTH = read(device_ptr, msg_ptr->DATA, REC_BLOCK_SIZE);
        }
        else if (remainder != 0)
        {
            msg_ptr->LENGTH = read(device_ptr, msg_ptr->DATA, remainder);
        }
        if (!_msgq_send_queue(msg_ptr, _msgq_get_id(0, WRITE_QUEUE)))
        {
            printf("  Error: Failed to send a message to a queue.\n");
        }
    }
    /* Stop recording */
    ioctl(device_ptr, IO_IOCTL_I2S_DISABLE_DEVICE, NULL);
    ioctl(device_ptr, IO_IOCTL_I2S_ENABLE_DEVICE, NULL);
    /* Send empty message to signal end of recording */
    msg_ptr = (REC_MESSAGE_PTR) _msg_alloc(msg_pool);
    msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, WRITE_QUEUE);
    msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0, WRITE_QUEUE);
    msg_ptr->HEADER.SIZE = sizeof(REC_MESSAGE); 
    msg_ptr->LENGTH = 0;
    
    if (!_msgq_send_queue(msg_ptr, _msgq_get_id(0, WRITE_QUEUE)))
    {
        printf("  Error: Failed to send a message to a queue.\n");
    }
    /* Wait until writing of SD card data finishes */ 
    result = _lwsem_wait(&(write_param.sdwrite_sem));
    if (result != MQX_OK)
    {
        printf("  Error: Wait for an event dev_read_done failed. Code 0x%X\n", result);
        return (SHELL_EXIT_ERROR);
    }
    
    printf("DONE\n");
    /* Cleanup and print results */
    result = _lwsem_destroy(&(write_param.sdwrite_sem));
    if (result != MQX_OK)
    {
        printf("  Error: Unable to free a sdwrite_event. Code 0x%X\n", result);
    }
    
    _task_destroy(_task_get_id_from_name("sdwrite"));
    result = _msgpool_destroy(msg_pool);
    if (result != MQX_OK)
    {
        printf("  Error: Cannot destroy a message pool. Code 0x%X\n", result);
    }
    
    /* Print results */
    if (ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats) != I2S_OK)
    {
        printf("  Error: Cannot read I2S statistics.\n");
    }
    else
    {
        printf("  ----------------------------------------------\n"); 
        printf("  Recording stats\n");
        printf("  Total interrupts:     %u\n", stats.INTERRUPTS);
        printf("  Received bytes:       %u\n", stats.RX_PACKETS * audio_format.SIZE);
        printf("  Overruns of FIFO:     %u\n", stats.FIFO_ERROR);
        printf("  Full software buffer: %u\n", stats.BUFFER_ERROR);
        printf("  ----------------------------------------------\n");  
    }
    if (fclose(device_ptr) != MQX_OK)
    {
        printf("  Error: Unable to close I2S device driver.\n");
    }
    if (fclose(file_ptr) != MQX_OK)
    {
        printf("  Error: Unable to close file %s.\n", argv[2]);
    }
    
    return(return_code);
}

/* EOF */
