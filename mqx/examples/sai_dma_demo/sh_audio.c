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
    uint32_t  mclk_freq, fs_freq;
    I2S_STATISTICS_STRUCT stats;
    uint32_t requests, remainder, i;
    
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
            printf("   device        = playback device (i.e. \"sai:\")\n");
            printf("   filename      = wav file to play (i.e. \"a:\\test.wav\" )\n");
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
    if(header.FMT_SUBCHUNK.SampleRate == 96000)
    {
      mclk_freq = header.FMT_SUBCHUNK.SampleRate * 256;
    } 
    else
    {
      mclk_freq = header.FMT_SUBCHUNK.SampleRate * CLK_MULT;
    }  
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

    /*Get the stats of the sai, this is the interface between application and 
    driver which provides the status of sai.*/
    ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
    requests = header.DATA_SUBCHUNK.Subchunk2Size / stats.SIZE;
    remainder = header.DATA_SUBCHUNK.Subchunk2Size % stats.SIZE;
    if(remainder != 0)
    {
      requests += 1;
    }
    uint32_t num = 0;
    for(i = 0; i < requests; i ++)
    {
        /*Waiting for there are empty blocks*/
        ioctl(device_ptr, IO_IOCTL_I2S_WAIT_TX_EVENT,NULL);
        /* Get the info where to fill the data */
        ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
        if((i != requests - 1) || (remainder == 0))
        {
            num = fread(stats.IN_BUFFER, 1, stats.SIZE, file_ptr);
            if(!num)
            {
                return -1;
            }
        }
        else
        {
            num = fread(stats.IN_BUFFER, 1, remainder, file_ptr); 
            if(!num)
            {
                return -1;
            }
            /*Fill zero to buffer*/
            uint32_t j = 0;
            for(j = remainder; j < stats.SIZE; j ++)
            {
                stats.IN_BUFFER[j] = 0;
            }
        }
        if (i == 0)
        {
            ioctl(device_ptr, IO_IOCTL_I2S_START_TX, NULL);
        }
        ioctl(device_ptr, IO_IOCTL_I2S_UPDATE_TX_STATUS, &stats.SIZE);
    }
    _time_delay(4);
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
        printf("  Bytes requested for transmit:  %d\n", stats.PACKETS_REQUESTED);
        printf("  Bytes transmitted:             %d\n", stats.PACKETS_REQUESTED);
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
    _mqx_int errcode = 0;
    uint32_t  mclk_freq, fs_freq = 0;
    I2S_STATISTICS_STRUCT stats;
    uint32_t requests = 0, remainder = 0, i = 0;
    bool print_usage, shorthelp = FALSE;
    int32_t return_code = SHELL_EXIT_SUCCESS;
    SD_WRITE_PARAM_STRUCT write_param;
    _task_id sdwrite_task_id;
    /*Allocate RAM buffer */
    uint8_t rate = 8;
    uint32_t buffer_size = 32768;
    uint8_t *ram_buffer = (uint8_t *)_mem_alloc_zero(buffer_size);
    uint8_t *des_address = ram_buffer;
    uint8_t *buffer_end = ram_buffer + buffer_size;

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
            printf("   device        = recording device (i.e. \"sai:\")\n");
            printf("   filename      = full filename for recording (i.e. \"a:\\test_record.wav\")\n");
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
    errcode = SetWaveHeader(argv[3], &header, ram_buffer);
    des_address += 44;
    switch (errcode)
    {
        case -1:
        printf("  Error: Cannot create file %s.\n", argv[2]);
        return (SHELL_EXIT_ERROR);

        case -2:
        printf("  Error: Wrong format specified.\n");
        return (SHELL_EXIT_ERROR); 

        default:
        break;
    }

    /* Open file with header and make it ready for data writing */
    file_ptr = fopen(argv[2], "w");
    if (file_ptr == NULL)
    {
        printf("  Unable to open file: %s\n", argv[2]);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }

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

    /* Setup audio codec connected to device */
    errcode = SetupCodec(device_ptr);
    if (errcode != 0)
    {
        printf("  Audio codec configuration failed. Error 0x%X.\n", errcode);
        fclose(device_ptr);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }

    /*Get the status of the I2S*/
    ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);


    /* Compute requested number of data blocks */
    requests = header.DATA_SUBCHUNK.Subchunk2Size / stats.SIZE;
    remainder = header.DATA_SUBCHUNK.Subchunk2Size % stats.SIZE;
    if (remainder > 0)
    {
        requests++;
    }
    uint32_t process_size = 0;

    /* Prepare the SDwrite task parameter */
    uint32_t write_length = 44 + header.DATA_SUBCHUNK.Subchunk2Size;
    write_param.file_ptr = file_ptr;
    write_param.address = ram_buffer;
    write_param.start_address = ram_buffer;
    write_param.size = rate * stats.SIZE;
    if(write_length % (rate * stats.SIZE))
    {
        write_param.requests = write_length / (rate * stats.SIZE) + 1;
    }
    else
    {
        write_param.requests = write_length / (rate * stats.SIZE);
    }
    _lwsem_create(&(write_param.sdwrite_sem), 0);
    _lwsem_create(&(write_param.buffer_full), 0);
    /* Create SD card write task */
    sdwrite_task_id = _task_create(0, _task_get_template_index("sdwrite"), (uint32_t) &write_param);
    if (sdwrite_task_id == MQX_NULL_TASK_ID)
    {
        printf("  Error: Cannot create task required for writing to SD card.\n");
        fclose(file_ptr);
        fclose(device_ptr);
        return (SHELL_EXIT_ERROR);
    }
    _time_delay (50);
    printf("  Recording...");
    for(i = 0; i < requests; i++)
    {
        ioctl(device_ptr, IO_IOCTL_I2S_START_RX, NULL);
        if (i != (requests - 1) || (remainder == 0))
        {
            process_size = stats.SIZE;
        }
        else
        {
            process_size = remainder;
        }
        ioctl(device_ptr, IO_IOCTL_I2S_WAIT_RX_EVENT, NULL);
        ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);

        //Copy data
        if(des_address + process_size > buffer_end)
        {
            uint32_t up_data = buffer_end - des_address;
            uint32_t left_data = process_size - up_data;
            _mem_copy(stats.OUT_BUFFER, des_address, up_data);
            _mem_copy(stats.OUT_BUFFER + up_data , ram_buffer, left_data);
            des_address = ram_buffer + left_data;
        }
        else
        {
            _mem_copy(stats.OUT_BUFFER, des_address, process_size);
            des_address += process_size;
        }
        ioctl(device_ptr, IO_IOCTL_I2S_UPDATE_RX_STATUS, &process_size);
        /*Ram buffer management */
        if(i % rate == rate -1)
        {
            _lwsem_post(&write_param.buffer_full);
        }
        if((i == requests -1) && (write_length%(rate * stats.SIZE)))
        {
            write_param.last_size = write_length%(rate * stats.SIZE);
            _lwsem_post(&write_param.buffer_full);
        }
    }
    /* Stop recording */
    ioctl(device_ptr, IO_IOCTL_I2S_DISABLE_DEVICE, NULL);
    _lwsem_wait(&write_param.sdwrite_sem);
    _lwsem_destroy(&(write_param.sdwrite_sem));
    _lwsem_destroy(&(write_param.buffer_full));
    _task_destroy(_task_get_id_from_name("sdwrite"));
    printf("DONE\n");

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
        printf("  Received bytes:       %u\n", stats.PACKETS_PROCESSED);
        printf("  Overruns of FIFO:     %u\n", stats.FIFO_ERROR);
        printf("  Full software buffer: %u\n", stats.BUFFER_ERROR);
        printf("  ----------------------------------------------\n");  
    }
    _mem_free(ram_buffer);
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
