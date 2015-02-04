/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This file is imx53 ASRC driver source file
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#define ASRC_RATIO_DECIMAL_DEPTH 26

/* Sample rates are aligned with that defined in pcm.h file */
static const unsigned char asrc_process_table[][8][2] = {
    /* 32kHz 44.1kHz 48kHz   64kHz   88.2kHz 96kHz  176kHz   192kHz */
    /*5512Hz*/
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},},
    /*8kHz*/
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},},
    /*11025Hz*/
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},},
    /*16kHz*/
    {{0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},},
    /*22050Hz*/
    {{0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},},
    /*32kHz*/
    {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0}, {0, 0},},
    /*44.1kHz*/
    {{0, 2}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0},},
    /*48kHz*/
    {{0, 2}, {0, 2}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0},},
    /*64kHz*/
    {{1, 2}, {0, 2}, {0, 2}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 0},},
    /*88.2kHz*/
    {{1, 2}, {1, 2}, {1, 2}, {1, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1},},
    /*96kHz*/
    {{1, 2}, {1, 2}, {1, 2}, {1, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1},},
    /*176kHz*/
    {{2, 2}, {2, 2}, {2, 2}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},},
    /*192kHz*/
    {{2, 2}, {2, 2}, {2, 2}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},},
};


static int32_t _asrc_set_clock_ratio
(
    uint32_t asrc_set_id,
    int32_t input_sample_rate,
    int32_t output_sample_rate
)
{
    int32_t i;
    int32_t integ = 0;
    unsigned long reg_val = 0;
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;

    if (output_sample_rate == 0)
        return -1;
    while (input_sample_rate >= output_sample_rate) {
        input_sample_rate -= output_sample_rate;
        integ++;
    }
    reg_val |= (integ << 26);

    for (i = 1; i <= ASRC_RATIO_DECIMAL_DEPTH; i++) {
        if ((input_sample_rate * 2) >= output_sample_rate) {
            reg_val |= (1 << (ASRC_RATIO_DECIMAL_DEPTH - i));
            input_sample_rate =
                input_sample_rate * 2 - output_sample_rate;
        } else
            input_sample_rate = input_sample_rate << 1;

        if (input_sample_rate == 0)
            break;
    }

    /*only use pair A*/
    asrc_regs->ASRIDRLA = reg_val;
    asrc_regs->ASRIDRHA = (reg_val >> 24);

    return 0;
}

static int32_t _asrc_set_process_configuration
(
    uint32_t asrc_set_id,
    int32_t input_sample_rate,
    int32_t output_sample_rate
)
{
    int32_t i = 0, j = 0;
    unsigned long reg;
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;

    switch (input_sample_rate) {
        case 5512:
            i = 0;
        case 8000:
            i = 1;
            break;
        case 11025:
            i = 2;
            break;
        case 16000:
            i = 3;
            break;
        case 22050:
            i = 4;
            break;
        case 32000:
            i = 5;
            break;
        case 44100:
            i = 6;
            break;
        case 48000:
            i = 7;
            break;
        case 64000:
            i = 8;
            break;
        case 88200:
            i = 9;
            break;
        case 96000:
            i = 10;
            break;
        case 176400:
            i = 11;
            break;
        case 192000:
            i = 12;
            break;
        default:
            return -1;
    }

    switch (output_sample_rate) {
        case 32000:
            j = 0;
            break;
        case 44100:
            j = 1;
            break;
        case 48000:
            j = 2;
            break;
        case 64000:
            j = 3;
            break;
        case 88200:
            j = 4;
            break;
        case 96000:
            j = 5;
            break;
        case 176400:
            j = 6;
            break;
        case 192000:
            j = 7;
            break;
        default:
            return -1;
    }

    reg = asrc_regs->ASRCFG;
    reg &= ~(0x0f << 6);
    reg |=
        ((asrc_process_table[i][j][0] << 6) |
         (asrc_process_table[i][j][1] << 8));
    asrc_regs->ASRCFG = reg;

	return 0;
}



int32_t asrc_plugin_init
(
)
{
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;
    /*enable asrc root clock and ipg clock*/


    /*reset asrc hardware*/
    asrc_regs->ASRCTR |= (1<<4);

    while (asrc_regs->ASRCTR & (1<<4))
    {}
    asrc_regs->ASRCTR = 1;

    /*default 6:2:2 channels assignment, pair A use 2 channels*/
    asrc_regs->ASRCNCR = (0x6<<8 | 0x2<<4 | 0x2);

    /*set recommend parameters value*/
    asrc_regs->ASRPMn[0] = 0x7fffff;
    asrc_regs->ASRPMn[1] = 0x255555;
    asrc_regs->ASRPMn[2] = 0xff7280;
    asrc_regs->ASRPMn[3] = 0xff7280;
    asrc_regs->ASRPMn[4] = 0xff7280;

    /*set mandatory value to task queue FIFO*/
    asrc_regs->ASRTFR1 = 0x001f00;

    /*set processing clock for 76Khz, FsASRC 119Mhz or ipg clock????*/
    //asrc_regs->ASR76K = 0x61D;
    asrc_regs->ASR76K = 0x36B;

    /*set processing clock for 56Khz, FsASRC 119Mhz or ipg clock????*/
    //asrc_regs->ASR56K = 0x84D;
    asrc_regs->ASR56K = 0x4A3;

    return 0;

}

int32_t asrc_plugin_config_pair
(
    uint32_t asrc_set_id,
    uint32_t word_width,
    uint32_t slot_width,
    int32_t source_sample_rate
)
{
    uint32_t regVal = 0;
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;
    /*config asrc pair A*/

    /*input source clock is not physical*/
    /*output clock is ESAI tx*/
    asrc_regs->ASRCSR = 0x700F;

    /*force to use ideal ratio for pair A*/
    regVal = asrc_regs->ASRCTR;
    regVal &= ~(0x1 << 20);
    regVal |= (0x3 << 13);
    asrc_regs->ASRCTR = regVal;

    /*set input clock divder and output clock divider, 16 bits sample length*/
    regVal = asrc_regs->ASRCDR1;
    regVal &= 0xfc0fc0;
    if (16 == slot_width)
        regVal |= 0x5000;
    else if (32 == slot_width)
        regVal |= 0x6000;
    asrc_regs->ASRCDR1 = regVal;

    /*output sample rate is 48khz*/
    /*input sample rate is 44.1khz*/

    /*set ideal ratio*/
    _asrc_set_clock_ratio(asrc_set_id, source_sample_rate, 48000);

    /*set processes configuration*/
    _asrc_set_process_configuration(asrc_set_id, source_sample_rate, 48000);

    /*set input and output threshold*/
    regVal = asrc_regs->ASRMCRA;
    regVal &= ~(0x3f<<12);
    regVal &= ~(0x3f<<0);
    regVal |= 1<<22;
    regVal |= 1<<21;
    regVal |= (0x1<<12);
    regVal |= (0x20<<0);
    asrc_regs->ASRMCRA = regVal;

    /*set input and output fifo word format, 16bits LSB aligned*/
    regVal = asrc_regs->ASRMCR1[0];
    if (16 == word_width)
        regVal = 0x201;
    else if (24 == word_width)
        regVal = 0;
    asrc_regs->ASRMCR1[0] = regVal;

    return 0;
}

int32_t asrc_plugin_start_pair
(
    uint32_t asrc_set_id
)
{
    uint32_t regVal = 0;
    int32_t i = 0;
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;

    /*enable asrc pair A*/
    asrc_regs->ASRCTR |= 0x2;

    regVal = asrc_regs->ASRCFG;
    while (!(regVal & (1 << 21)))
        regVal= asrc_regs->ASRCFG;

    regVal = 0;
    for (i = 0; i < 20; i++) {
        asrc_regs->ASRDI[0].ASRDI = regVal;
        asrc_regs->ASRDI[0].ASRDI = regVal;
        asrc_regs->ASRDI[0].ASRDI = regVal;
        asrc_regs->ASRDI[0].ASRDI = regVal;
        asrc_regs->ASRDI[0].ASRDI = regVal;
        asrc_regs->ASRDI[0].ASRDI = regVal;
        asrc_regs->ASRDI[0].ASRDI = regVal;
        asrc_regs->ASRDI[0].ASRDI = regVal;
    }

    return 0;
}

int32_t asrc_plugin_stop_pair
(
    uint32_t asrc_set_id
)
{
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;

    /*disable asrc pair A*/
    asrc_regs->ASRCTR &= ~(0x2);

    return 0;
}

void asrc_plugin_dump_regs()
{
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;

    printf("=====================>>>DUMP ASRC\n");
    printf("ASRCTR=0x%x\n", asrc_regs->ASRCTR);
    printf("ASRIER=0x%x\n", asrc_regs->ASRIER);
    printf("ASRCNCR=0x%x\n", asrc_regs->ASRCNCR);
    printf("ASRCFG=0x%x\n", asrc_regs->ASRCFG);
    printf("ASRCSR=0x%x\n", asrc_regs->ASRCSR);
    printf("ASRCDR1=0x%x\n", asrc_regs->ASRCDR1);
    printf("ASRCDR2=0x%x\n", asrc_regs->ASRCDR2);
    printf("ASRSTR=0x%x\n", asrc_regs->ASRSTR);
    printf("ASRPMN1=0x%x\n", asrc_regs->ASRPMn[0]);
    printf("ASRPMN2=0x%x\n", asrc_regs->ASRPMn[1]);
    printf("ASRPMN3=0x%x\n", asrc_regs->ASRPMn[2]);
    printf("ASRPMN4=0x%x\n", asrc_regs->ASRPMn[3]);
    printf("ASRPMN5=0x%x\n", asrc_regs->ASRPMn[4]);
    printf("ASRTFR1=0x%x\n", asrc_regs->ASRTFR1);
    printf("ASRCCR=0x%x\n", asrc_regs->ASRCCR);
    printf("ASRDIDRHA=0x%x\n", asrc_regs->ASRIDRHA);
    printf("ASRDIDRLA=0x%x\n", asrc_regs->ASRIDRLA);
    printf("ASRDIDRHB=0x%x\n", asrc_regs->ASRIDRHB);
    printf("ASRDIDRLB=0x%x\n", asrc_regs->ASRIDRLB);
    printf("ASRDIDRHC=0x%x\n", asrc_regs->ASRIDRHC);
    printf("ASRDIDRLC=0x%x\n", asrc_regs->ASRIDRLC);
    printf("ASR76K=0x%x\n", asrc_regs->ASR76K);
    printf("ASR56K=0x%x\n", asrc_regs->ASR56K);
    printf("ASRMCRA=0x%x\n", asrc_regs->ASRMCRA);
    printf("ASRFSTA=0x%x\n", asrc_regs->ASRFSTA);
    printf("ASRMCRB=0x%x\n", asrc_regs->ASRMCRB);
    printf("ASRFSTB=0x%x\n", asrc_regs->ASRFSTB);
    printf("ASRMCRC=0x%x\n", asrc_regs->ASRMCRC);
    printf("ASRFSTC=0x%x\n", asrc_regs->ASRFSTC);
    printf("ASRMCR1A=0x%x\n", asrc_regs->ASRMCR1[0]);
    printf("ASRMCR1B=0x%x\n", asrc_regs->ASRMCR1[1]);
    printf("ASRMCR1C=0x%x\n", asrc_regs->ASRMCR1[2]);
    printf("=====================>>>DUMP ASRC END\n");

}

