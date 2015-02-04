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
//#include "Cs48560.h"
#include "sh_audio.h"
#include <esai_vport.h>
#include <sem.h>
#include <mutex.h>
#include <lwevent.h>
#include <asrc_vybrid.h>
#include <asrc.h>

#define CHUNK_BUFFER_SIZE     (384*4*10)
#define MULTI_CH_BUFFER_SIZE     (10*1024*1024)
#define ESAI_READ_UNIT   (512 * 3 *10)

#define DEV_I2C1              "i2c1:"

//#define ESAI_REG_DEBUG 1

#define BSP_ESAI0_TX0_VPORT "esai_tx0:"
#define BSP_ESAI0_RX0_VPORT "esai_rx0:"
#define BSP_ASRC0_DEVICE_FILE "asrc0:"

uint8_t *chunk_buffer;


/*
** Input files strings
*/
#define AUDIO_FILE_PATH_16K_16WL  "\\wav\\ms_pcm_16khz_16bit_stereo.wav"
#define AUDIO_FILE_PATH_16K_24WL  "\\wav\\ms_pcm_16khz_24bit_stereo.wav"
#define AUDIO_FILE_PATH_22K05_16WL  "\\wav\\ms_pcm_22.05khz_16bit_stereo.wav"
#define AUDIO_FILE_PATH_22K05_24WL  "\\wav\\ms_pcm_22.05khz_24bit_stereo.wav"
#define AUDIO_FILE_PATH_32K_16WL  "\\wav\\ms_pcm_32khz_16bit_stereo.wav"
#define AUDIO_FILE_PATH_32K_24WL  "\\wav\\ms_pcm_32khz_24bit_stereo.wav"
#define AUDIO_FILE_PATH_48K_16WL   "\\wav\\ms_pcm_48khz_16bit_stereo.wav"
#define AUDIO_FILE_PATH_48K_24WL   "\\wav\\ms_pcm_48khz_24bit_stereo.wav"
#define AUDIO_FILE_PATH_64K_16WL   "\\wav\\Staying Alive_64khz_16bit.wav"
#define AUDIO_FILE_PATH_64K_24WL   "\\wav\\PCM_64khz_24bit_2ch.wav"
#define AUDIO_FILE_PATH_96K_16WL   "\\wav\\Staying Alive_96khz_16bit.wav"
#define AUDIO_FILE_PATH_96K_24WL   "\\wav\\PCM_96khz_24bit_2ch.wav"
#define AUDIO_FILE_PATH_192K_16WL   "\\wav\\Staying Alive_192khz_16bit.wav"
#define AUDIO_FILE_PATH_192K_24WL   "\\wav\\PCM_192khz_24bit_2ch.wav"
#define AUDIO_FILE_PATH_64K_16WL_6CH  "\\wav\\PCM_64khz_6.1kbps_6_16bit.wav"
#define AUDIO_FILE_PATH_48K_24WL_8CH  "\\wav\\8_Channel_ID.wav"
#define AUDIO_FILE_PATH_48K_LR_16WL  "\\wav\\pcm_L_R_48khz_16bit.wav"
#define AUDIO_FILE_PATH_48K_LR_24WL  "\\wav\\pcm_L_R_48khz_24bit.wav"

#define AUDIO_FILE_PATH_44K1_16WL  "\\wav\\ms_pcm_44.1khz_16bit_stereo.wav"
#define AUDIO_FILE_PATH_44K1_24WL  "\\wav\\ms_pcm_44.1khz_24bit_stereo.wav"

#define AUDIO_FILE_PATH_MONO_48K_16WL   "\\wav\\ms_pcm_48khz_16bit_mono.wav"
#define AUDIO_FILE_PATH_MONO_48K_24WL   "\\wav\\ms_pcm_48khz_24bit_mono.wav"


/*
** sample rates
*/
#define SAMPLE_RATE_16K           (16000)
#define SAMPLE_RATE_22K05         (22050)
#define SAMPLE_RATE_32K           (32000)
#define SAMPLE_RATE_44K1          (44100)
#define SAMPLE_RATE_48K           (48000)
#define SAMPLE_RATE_64K           (64000)
#define SAMPLE_RATE_96K           (96000)
#define SAMPLE_RATE_192K          (192000)

#define ASRC_HARDCODE_VERSION 0

/*
** structure to record a test file information
*/
typedef struct{
  char     filename[255];
  int32_t    sample_rate;
  ESAI_FIFO_PCM_FORMAT pcm_word_length;
  ESAI_MODULE_SLOT_WIDTH esai_slot_length;
}test_wav_file_t;

typedef struct{
  char     filename[255];
  int32_t    source_sample_rate;
  int32_t    target_sample_rate;
  ESAI_FIFO_PCM_FORMAT pcm_word_length;
  ESAI_MODULE_SLOT_WIDTH esai_slot_length;
}asrc_test_wav_file_t;

MQX_FILE_PTR esai_vport_file = NULL;
MQX_FILE_PTR esai_rx_vport = NULL;


test_wav_file_t  esai_test_asrc_files =
{
    AUDIO_FILE_PATH_44K1_16WL,
    SAMPLE_RATE_48K,
    ESAI_FIFO_PCM_S16_LE,
    ESAI_MODULE_SW_32BIT
};


asrc_test_wav_file_t  asrc_wav_test_file =
{
    AUDIO_FILE_PATH_44K1_16WL,
    SAMPLE_RATE_44K1,
    SAMPLE_RATE_48K,
    ESAI_FIFO_PCM_S16_LE,
    ESAI_MODULE_SW_32BIT
};

typedef struct {
    uint32_t rate;
    ESAI_FIFO_PCM_FORMAT format;
    ESAI_MODULE_SLOT_WIDTH slot_wd;
} ESAI_TEST_ARGUS, * ESAI_TEST_ARGUS_PTR;

static ESAI_TEST_ARGUS esai_task_argus;
uint32_t esai_timeout_value = 0;
uint32_t esai_timeout_value_rx = 0;

/*FUNCTION****************************************************************
*
* Function Name    : Shell_play
* Returned Value   : SHELL_EXIT_SUCCESS if everything is ok,
*                    SHELL_EXIT_ERROR otherwise
* Comments         :
*    Shell function for playing wave files
*
*END*********************************************************************/

int32_t Shell_play_asrc(int32_t argc, char *argv[])
{
    int32_t i = 0;
    int32_t result = 0;
    int32_t bytes_read;
    bool print_usage, shorthelp = FALSE;
    int32_t return_code = SHELL_EXIT_SUCCESS;
    WAVE_FILE_HEADER header;

    MQX_FILE_PTR asrc_pair_fd[3];
    MQX_FILE_PTR pcm_fd;
    MQX_FILE_PTR esai_fd;

    ESAI_VPORT_CONFIG_STRUCT  esai_vport_config;
    ESAI_ASRC_DMA_STRUCT     esai_dma_config;

    ASRC_SAMPLE_RATE_PAIR_STRUCT    asrc_sample_rate_config;
    ASRC_REF_CLK_PAIR_STRUCT        asrc_ref_clk_config;
    ASRC_CLK_DIV_PAIR_STRUCT        asrc_clk_div_config;
    ASRC_IO_FORMAT_PAIR_STRUCT      asrc_fifo_fmt_config;
    ASRC_INSTALL_SERVICE_STRUCT     asrc_service_in;
    ASRC_INSTALL_SERVICE_STRUCT     asrc_service_out;

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
            printf("Usage: %s <filename>\n", argv[0]);
            printf("   filename      = wav file to play via ASRC\n");
        }
        return(return_code);
    }

    /* Check if there is enough parameters */
    if (argc < 1)
    {
        printf("  Error: Not enough parameters.\n");
        return (SHELL_EXIT_ERROR);
    }

    /* Read header of file and fill header struct with data */
    if (GetWaveHeader(argv[1], &header) != 0)
    {
        printf("  Error: Unable to open a file: %s\n", argv[1]);
        return (SHELL_EXIT_ERROR);
    }

    if ((header.FMT_SUBCHUNK.BitsPerSample != 16) || (header.FMT_SUBCHUNK.NumChannels != 2))
    {
        printf("  Error: Unable to play a file with bit depth of %d. Only support 16 bit\n", header.FMT_SUBCHUNK.BitsPerSample);
        return (SHELL_EXIT_ERROR);
    }

    if (header.FMT_SUBCHUNK.SampleRate != 44100)
    {
        printf("  Error: Unable to playback audio file with sample rate of %d. Only 44k is supported\r\n", header.FMT_SUBCHUNK.SampleRate);
        return (SHELL_EXIT_ERROR);
    }

    chunk_buffer = _mem_alloc_zero(CHUNK_BUFFER_SIZE);

    strcpy(asrc_wav_test_file.filename, argv[1]);

    printf("===== Play music %s =====\n", asrc_wav_test_file.filename);
    printf("[source sample rate = %d]\n", asrc_wav_test_file.source_sample_rate);
    printf("[target sample rate = %d]\n", asrc_wav_test_file.target_sample_rate);
    printf("[slot length = %d]\n", asrc_wav_test_file.esai_slot_length);
    printf("=========================\n");

    /*tries to open music file handle*/
    pcm_fd = fopen(asrc_wav_test_file.filename, "r");
    if(!pcm_fd) {
        printf("Error Opening %s!\n", asrc_wav_test_file.filename);
        return -1;
    }

    /*tries to open 3 asrc device file handle*/
    for (i=0; i<1; i++) {
        asrc_pair_fd[i] = fopen(BSP_ASRC0_DEVICE_FILE, (const char *)0);
        if (asrc_pair_fd[i] == NULL)
        {
            printf("Error Opening asrc driver %d!\n", i);
            return -1;
        }

        asrc_sample_rate_config.ASRC_INPUT_SAMPLE_RATE = asrc_wav_test_file.source_sample_rate;
        asrc_sample_rate_config.ASRC_OUTPUT_SAMPLE_RATE = asrc_wav_test_file.target_sample_rate;
        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_SET_SAMPLE_RATE,
            (char *)&asrc_sample_rate_config);
        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_SET_SAMPLE_RATE %d!\n", result);
            return -2;
        }

        asrc_ref_clk_config.ASRC_INPUT_CLK = ASRC_CLK_NONE;
        asrc_ref_clk_config.ASRC_OUTPUT_CLK = ASRC_CLK_ESAI_TX;
        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_SET_REFCLK,
            &asrc_ref_clk_config);
        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_SET_REFCLK %d!\n", result);
            return -2;
        }

        asrc_clk_div_config.ASRC_INPUT_CLK_DIV = 0;
        switch (asrc_wav_test_file.esai_slot_length) {
            case ESAI_MODULE_SW_16BIT:
                asrc_clk_div_config.ASRC_OUTPUT_CLK_DIV = 16;
                break;
            case ESAI_MODULE_SW_24BIT:
                asrc_clk_div_config.ASRC_OUTPUT_CLK_DIV = 24;
                break;
            case ESAI_MODULE_SW_32BIT:
                asrc_clk_div_config.ASRC_OUTPUT_CLK_DIV = 32;
                break;
            default:
                printf("Error slot width %d!\n",
                    asrc_wav_test_file.esai_slot_length);
                return -2;
        }
        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_SET_SLOT_WIDTH,
            &asrc_clk_div_config);
        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_SET_SLOT_WIDTH %d!\n", result);
            return -2;
        }

        switch (asrc_wav_test_file.pcm_word_length) {
            case ESAI_FIFO_PCM_S16_LE:
                asrc_fifo_fmt_config.ASRC_INPUT_FORMAT = ASRC_FIFO_16BITS_LSB;
                asrc_fifo_fmt_config.ASRC_OUTPUT_FORMAT = ASRC_FIFO_16BITS_LSB;
                break;
            case ESAI_FIFO_PCM_S24_LE:
                asrc_fifo_fmt_config.ASRC_INPUT_FORMAT = ASRC_FIFO_24BITS_LSB;
                asrc_fifo_fmt_config.ASRC_OUTPUT_FORMAT = ASRC_FIFO_24BITS_LSB;
                break;
            default:
                printf("Error pcm word length %d!\n",
                    asrc_wav_test_file.pcm_word_length);
                return -3;
        }

        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_SET_IO_FORMAT,
            &asrc_fifo_fmt_config);
        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_SET_IO_FORMAT %d!\n", result);
            return -2;
        }
    }

    /*============ open ESAI and set it =============*/
    esai_fd = fopen(BSP_ESAI0_TX0_VPORT, NULL);
    if (esai_fd == NULL)
    {
        printf("Open esai fifo driver failed!\n");
        return -1;
    }

    switch (asrc_wav_test_file.pcm_word_length) {
        case ESAI_FIFO_PCM_S16_LE:
        case ESAI_FIFO_PCM_U16_LE:
            esai_vport_config.data_width = ESAI_VPORT_DW_16BITS;
            break;
        case ESAI_FIFO_PCM_S24_LE:
        case ESAI_FIFO_PCM_U24_LE:
            esai_vport_config.data_width = ESAI_VPORT_DW_24BITS;
            break;
        default:
            printf("pcm_word_length error!\n");
            return -3;
    }
    esai_vport_config.asrc_8bit_covert = 0;
    esai_vport_config.chnl_type = (ESAI_VPORT_CHNL_TYPE)2;

    ioctl(esai_fd, IO_IOCTL_ESAI_VPORT_CONFIG, (char *)&esai_vport_config);

    /*traverse these 1 asrc pairs for ESAI*/
    for (i=0; i<1; i++) {

        printf("INSTALL ASRC %d pair to ESAI\n", i);

        asrc_service_in.ASRC_SERVICE = ASRC_SERVICE_MEM;
        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_INSTALL_SERVICE_SRC,
            (char *)&asrc_service_in);

        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_INSTALL_SERVICE_SRC %d!\n", result);
            return -2;
        }

        asrc_service_out.ASRC_SERVICE = ASRC_SERVICE_ESAI;
        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_INSTALL_SERVICE_DEST,
            (char *)&asrc_service_out);

        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_INSTALL_SERVICE_DEST %d!\n", result);
            return -2;
        }

        esai_dma_config.input_dma_channel = asrc_service_in.ASRC_DMA_CHL;
        esai_dma_config.output_dma_channel = asrc_service_out.ASRC_DMA_CHL;

        result = ioctl(esai_fd, IO_IOCTL_ESAI_VPORT_SET_ASRC,
            (char *)&esai_dma_config);

        if (result != 0) {
            printf("Error IO_IOCTL_ESAI_FIFO_SET_ASRC %d!\n", result);
            return -2;
        }

        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_START,
            (char *)NULL);

        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_START %d!\n", result);
            return -2;
        }

        printf("ASRC START!\n");

        result = ioctl(esai_fd, IO_IOCTL_ESAI_VPORT_START, (char *)NULL);
        if (result != 0) {
            printf("Error IO_IOCTL_ESAI_FIFO_START %d!\n", result);
            return -2;
        }

        printf("ESAI START!\n");

        fseek(pcm_fd, 44, IO_SEEK_SET); //jump out the header

        while(!feof(pcm_fd))
        {
            bytes_read = fread(chunk_buffer, 1, CHUNK_BUFFER_SIZE, pcm_fd);
            if (bytes_read <= 0)
                continue;
            bytes_read = write(esai_fd, chunk_buffer, bytes_read);
        }

        _time_delay(1000);
        ioctl(esai_fd, IO_IOCTL_ESAI_VPORT_STOP, (char *)AUD_IO_FW_DIR_TX);

        printf("ESAI STOP!\n");

        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_STOP,
            (char *)NULL);

        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_STOP %d!\n", result);
            return -2;
        }

        printf("ASRC STOP!\n");

        /*clear asrc dma*/
        esai_dma_config.input_dma_channel = (uint32_t)NULL;
        esai_dma_config.output_dma_channel = (uint32_t)NULL;
        result = ioctl(esai_fd, IO_IOCTL_ESAI_VPORT_SET_ASRC,
            (char *)&esai_dma_config);

        if (result != 0) {
            printf("Error IO_IOCTL_ESAI_FIFO_SET_ASRC %d!\n", result);
            return -2;
        }

        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_UNINSTALL_SRC,
            (char *)NULL);

        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_UNINSTALL_SRC %d!\n", result);
            return -2;
        }

        result = ioctl(asrc_pair_fd[i], IO_IOCTL_ASRC_UNINSTALL_DEST,
            (char *)NULL);

        if (result != 0) {
            printf("Error IO_IOCTL_ASRC_UNINSTALL_DEST %d!\n", result);
            return -2;
        }

    }


    ioctl(esai_fd, IO_IOCTL_ESAI_VPORT_RESET, NULL);

    fclose(pcm_fd);

    printf("MUSIC DONE!\n");

    fclose(esai_fd);

    for (i=0; i<1; i++) {
        fclose(asrc_pair_fd[i]);
    }

    _mem_free(chunk_buffer);

    return 0;
}


int32_t Shell_play(int32_t argc, char *argv[])
{
    int32_t return_code = SHELL_EXIT_SUCCESS;
    bool print_usage, shorthelp = FALSE;
    WAVE_FILE_HEADER header;
    /*Variable for playback the esai file*/
    int i = 0;
    int bytes_read, bytes_write;
    ESAI_VPORT_CONFIG_STRUCT  esai_vport_config;
    MQX_FILE_PTR        pcm_fd;
    int32_t total_write = 0;
    TIME_STRUCT start_time, end_time, diff_time;
    int32_t sample_rate;
    int32_t ret;
    bool i2s = 1;

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
            printf("Usage: %s <filename>\n", argv[0]);
            printf("   filename      = wav file to play\n");
        }
        return(return_code);
    }

    /* Check if there is enough parameters */
    if (argc < 1)
    {
        printf("  Error: Not enough parameters.\n");
        return (SHELL_EXIT_ERROR);
    }

    /* Read header of file and fill header struct with data */
    if (GetWaveHeader(argv[1], &header) != 0)
    {
        printf("  Error: Unable to open a file: %s\n", argv[1]);
        return (SHELL_EXIT_ERROR);
    }

    if ((header.FMT_SUBCHUNK.BitsPerSample != 16) || (header.FMT_SUBCHUNK.NumChannels != 2))
    {
        printf("  Error: Unable to play a file with bit depth of %d. Only support 16 bit\n", header.FMT_SUBCHUNK.BitsPerSample);
        return (SHELL_EXIT_ERROR);
    }

    if (header.FMT_SUBCHUNK.SampleRate != 48000)
    {
        printf("  Error: Unable to playback audio file with sample rate of %d. Only 48k is supported\r\n", header.FMT_SUBCHUNK.SampleRate);
        return (SHELL_EXIT_ERROR);
    }

    chunk_buffer = _mem_alloc_zero(CHUNK_BUFFER_SIZE);

    strcpy(esai_test_asrc_files.filename, argv[1]);

    printf("===== Play music %s =====\n", esai_test_asrc_files.filename);

    pcm_fd = fopen(esai_test_asrc_files.filename, "r");
    if(!pcm_fd) {
        printf("Error Opening %s!\n", esai_test_asrc_files.filename);
        _mqx_exit (1);
    }

    sample_rate = esai_test_asrc_files.sample_rate;

    /*============ open ESAI and set it =============*/
    esai_vport_file = fopen(BSP_ESAI0_TX0_VPORT, NULL);
    if (esai_vport_file == NULL)
    {
        printf("Open esai fifo driver failed!\n");
        return return_code;
    }

    /*config to I2S format*/
    switch (esai_test_asrc_files.pcm_word_length) {
        case ESAI_FIFO_PCM_S16_LE:
        case ESAI_FIFO_PCM_U16_LE:
            esai_vport_config.data_width = ESAI_VPORT_DW_16BITS;
            break;
        case ESAI_FIFO_PCM_S24_LE:
        case ESAI_FIFO_PCM_U24_LE:
            esai_vport_config.data_width = ESAI_VPORT_DW_24BITS;
            break;
        default:
            printf("pcm_word_length error!\n");
            return return_code;
    }
    esai_vport_config.asrc_8bit_covert = 0;
        esai_vport_config.chnl_type = ESAI_VPORT_CHNL_STEREO;

    ret = ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_CONFIG,
        (char *)&esai_vport_config);
    if (ret != MQX_OK) {
        printf("Configure ESAI failed\n");
        return return_code;
    }

    if (i2s)
        ret = ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_SET_HW_INTERFACE,
            (char *)ESAI_VPORT_HW_INF_I2S);
    else
        ret = ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_SET_HW_INTERFACE,
            (char *)ESAI_VPORT_HW_INF_LEFTJ);
    if (ret != MQX_OK) {
        printf("Set hw interface to ESAI failed\n");
        return return_code;
    }

    ret = ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_SET_SAMPLE_RATE,
            (char *)sample_rate);
    if (ret != MQX_OK) {
        printf("Set sample rate %d to ESAI failed\n", sample_rate);
        return return_code;
    }

    ret = ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_SET_TX_TIMEOUT,
            (char *)esai_timeout_value);
    if (ret != MQX_OK) {
        printf("Set timeout %d to ESAI failed\n", esai_timeout_value);
        return return_code;
    }

    ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_START,
        (char *)NULL);
    printf("ESAI START!\n");


#if ESAI_REG_DEBUG
    printf("START the ESAI=====================>>>\n");
    printf("ECR=0x%x\n", ESAI_BASE_PTR->ECR);
    printf("ESR=0x%x\n", ESAI_BASE_PTR->ESR);
    printf("TFCR=0x%x\n", ESAI_BASE_PTR->TFCR);
    printf("TFSR=0x%x\n", ESAI_BASE_PTR->TFSR);
    printf("RFCR=0x%x\n", ESAI_BASE_PTR->RFCR);
    printf("RFSR=0x%x\n", ESAI_BASE_PTR->RFSR);
    printf("TSR=0x%x\n", ESAI_BASE_PTR->TSR);
    printf("SAISR=0x%x\n", ESAI_BASE_PTR->SAISR);
    printf("SAICR=0x%x\n", ESAI_BASE_PTR->SAICR);
    printf("TCR=0x%x\n", ESAI_BASE_PTR->TCR);
    printf("TCCR=0x%x\n", ESAI_BASE_PTR->TCCR);
    printf("RCR=0x%x\n", ESAI_BASE_PTR->RCR);
    printf("RCCR=0x%x\n", ESAI_BASE_PTR->RCCR);
    printf("TSMA=0x%x\n", ESAI_BASE_PTR->TSMA);
    printf("TSMB=0x%x\n", ESAI_BASE_PTR->TSMB);
    printf("PRRC=0x%x\n", ESAI_BASE_PTR->PRRC);
    printf("PCRC=0x%x\n", ESAI_BASE_PTR->PCRC);

#endif

    _time_get(&start_time);

    fseek(pcm_fd, 44, IO_SEEK_SET); //jump out the header

    while(!feof(pcm_fd))
    {

        bytes_read = fread(chunk_buffer, 1, CHUNK_BUFFER_SIZE, pcm_fd);

        if (bytes_read <= 0)
            continue;

        //for (i=0; i< 150; i++) {
            bytes_write = write(esai_vport_file, chunk_buffer, bytes_read);
            if (bytes_write != bytes_read) {
                ret = ferror(esai_vport_file);
                if (ret == IO_ERROR_TIMEOUT) {
                    printf("timeout, write data %d\n", bytes_write);
                } else if (ret == IO_ERROR_NOTALIGNED) {
                    printf("not aligned buffer, write data %d\n", bytes_write);
                } else {
                    printf("other error %d, bytes_write %d, bytes_read %d\n",
                        ret, bytes_write, bytes_read);
                }
            }

            if (bytes_write > 0)
                total_write += bytes_write;

            //printf("ASRFSTA = 0x%x\n", reg32_read((ASRC_BASE_ADDR + 0xA4)));
            //printf("TFSR = 0x%x, ESR = 0x%x\n", reg32_read((ESAI_BASE_ADDR + 0x14)),
            //    reg32_read((ESAI_BASE_ADDR + 0xC)));
        //}
    }

    _time_get(&end_time);
    _time_diff(&start_time, &end_time, &diff_time);
    printf("\ndata = %d, Time spends on SD reading is %ld sec, %ld millisec\n",
        total_write, diff_time.SECONDS, diff_time.MILLISECONDS);

#if EDMA_DEBUG

    for (i=0; i<isr_sdr_count; i++)
    {
        printf("sdr[%d][0] = 0x%x, sdr[1] = 0x%x\n, "
            "sdr[2] = 0x%x\n, sdr[3] = 0x%x\n",
            i, isr_sdr[i][0], isr_sdr[i][1], isr_sdr[i][2], isr_sdr[i][3]);
    }

    for (i=0; i< force_update_tcd_co; i++) {
        printf("force_update_tcd[%d][0] = 0x%x, [1] = 0x%x\n", i,
            force_update_tcd[i][0], force_update_tcd[i][1]);
    }
#endif

    _time_delay(10000);

#if ESAI_REG_DEBUG
    i = 1;
    while (i--)
    {
        printf("ECR=0x%x\n", ESAI_BASE_PTR->ECR);
        printf("ESR=0x%x\n", ESAI_BASE_PTR->ESR);
        printf("TFCR=0x%x\n", ESAI_BASE_PTR->TFCR);
        printf("TFSR=0x%x\n", ESAI_BASE_PTR->TFSR);
        printf("RFCR=0x%x\n", ESAI_BASE_PTR->RFCR);
        printf("RFSR=0x%x\n", ESAI_BASE_PTR->RFSR);
        printf("TSR=0x%x\n", ESAI_BASE_PTR->TSR);
        printf("SAISR=0x%x\n", ESAI_BASE_PTR->SAISR);
        printf("SAICR=0x%x\n", ESAI_BASE_PTR->SAICR);
        printf("TCR=0x%x\n", ESAI_BASE_PTR->TCR);
        printf("TCCR=0x%x\n", ESAI_BASE_PTR->TCCR);
        printf("RCR=0x%x\n", ESAI_BASE_PTR->RCR);
        printf("RCCR=0x%x\n", ESAI_BASE_PTR->RCCR);
        printf("TSMA=0x%x\n", ESAI_BASE_PTR->TSMA);
        printf("TSMB=0x%x\n", ESAI_BASE_PTR->TSMB);
        printf("PRRC=0x%x\n", ESAI_BASE_PTR->PRRC);
        printf("PCRC=0x%x\n", ESAI_BASE_PTR->PCRC);

        _time_delay(20);
    }
#endif

    ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_STOP, (char *)NULL);
    ioctl(esai_vport_file, IO_IOCTL_ESAI_VPORT_RESET, NULL);

    fclose(pcm_fd);

    printf("MUSIC DONE!\n");

    fclose(esai_vport_file);

    esai_vport_file = NULL;
    _mem_free(chunk_buffer);

    return 0;

}

int32_t Shell_record(int32_t argc, char *argv[])
{
    WAVE_FILE_HEADER header;
    FILE_PTR file_ptr;
    _mqx_int errcode = 0;
    bool print_usage, shorthelp = FALSE;
    int32_t return_code = SHELL_EXIT_SUCCESS;

    //This test would only do with the 48K, 5s recording
    int32_t read_len = 0;
    ESAI_VPORT_CONFIG_STRUCT  esai_rx_vport_config;
    ESAI_TEST_ARGUS_PTR argus_ptr = &esai_task_argus;
    char filetype[] = {"5:48000:16:2"};
    int32_t ret = 0;
    uint8_t *unit_buf = 0;
    uint32_t rx_unit = ESAI_READ_UNIT;
    uint32_t read_len_total;

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
            printf("Usage: %s <filename>\n", argv[0]);
            printf("   filename      = wav file to record\n");
        }
        return(return_code);
    }

    if (argc < 2)
    {
        printf("  Not enough parameters.\n");
        return(SHELL_EXIT_ERROR);
    }
    unit_buf = _mem_alloc_zero(ESAI_READ_UNIT);

    read_len_total = 48000 * 2 * 4 * 5;

    /* Fill header struct according to input parameter and write header to file */
    errcode = SetWaveHeader(filetype, &header, argv[1]);
    switch (errcode)
    {
        case -1:
        printf("  Error: Cannot create file %s.\n", argv[1]);
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
    file_ptr = fopen(argv[1], "a");
    if (file_ptr == NULL)
    {
        printf("  Unable to open file: %s\n", argv[1]);
        fclose(file_ptr);
        return (SHELL_EXIT_ERROR);
    }
    fseek(file_ptr, WAVE_HEADER_SIZE, IO_SEEK_SET);


    //Set the argu to the fixed parameters.
    esai_task_argus.rate = SAMPLE_RATE_48K;
    esai_task_argus.slot_wd = ESAI_MODULE_SW_32BIT;
    esai_task_argus.format = ESAI_FIFO_PCM_S16_LE;

    /*Open the RX vport */
    if (NULL == esai_rx_vport) {
        esai_rx_vport = fopen(BSP_ESAI0_RX0_VPORT, NULL);
        if (esai_rx_vport == NULL)
        {
            printf("Open esai fifo driver failed!\n");
            return 0;
        }
    }

    esai_rx_vport_config.asrc_8bit_covert = 0;
    esai_rx_vport_config.chnl_type = ESAI_VPORT_CHNL_STEREO;

    if (ESAI_FIFO_PCM_S16_LE == argus_ptr->format)
        esai_rx_vport_config.data_width = ESAI_VPORT_DW_16BITS;
    else
        esai_rx_vport_config.data_width = ESAI_VPORT_DW_24BITS;

    ioctl(esai_rx_vport, IO_IOCTL_ESAI_VPORT_CONFIG,
            (char *)&esai_rx_vport_config);

    ret = ioctl(esai_rx_vport, IO_IOCTL_ESAI_VPORT_SET_HW_INTERFACE,
        (char *)ESAI_VPORT_HW_INF_I2S);
    if (ret != MQX_OK) {
        printf("Set hw interface to ESAI failed\n");
        return 0;
    }

    ret = ioctl(esai_rx_vport, IO_IOCTL_ESAI_VPORT_SET_SAMPLE_RATE,
            (char *)argus_ptr->rate);
    if (ret != MQX_OK) {
        printf("Set sample rate %d to ESAI failed\n", argus_ptr->rate);
        return 0;
    }

    ret = ioctl(esai_rx_vport, IO_IOCTL_ESAI_VPORT_START,
                (void *)NULL);
    if (MQX_OK != ret) {
        printf("failed to call IO_IOCTL_ESAI_FIFO_START cmd, %d\n", ret);
        return 0;
    }

    while (read_len_total!= 0)
    {
        _mem_zero(unit_buf, sizeof(unit_buf));
        read_len = read(esai_rx_vport, unit_buf, rx_unit);
        if (read_len != rx_unit) {
            printf("failed to read, %d\n", read_len);
            return 0;
        }
        if (read_len_total > read_len)
        {
            fwrite(unit_buf, read_len, 1, file_ptr);
            read_len_total -= read_len;
        }
        else
        {
            fwrite(unit_buf, read_len_total, 1, file_ptr);
            read_len_total = 0;
        }
    }

    printf("rx test over\n");
    fclose(file_ptr);

    ret = fclose(esai_rx_vport);
    if (MQX_OK != ret) {
        printf("failed to close ESAI rx vport, %d\n", ret);
        return 0;
    }
    esai_rx_vport = NULL;

    _mem_free(unit_buf);

    return 0;
}

/* EOF */
