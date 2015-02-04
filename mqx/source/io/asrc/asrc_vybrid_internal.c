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
*   This file is vybrid ASRC driver source file for low level implementations
*
*
*END************************************************************************/
//#define __ASRC_USE_EDMA_DRIVER


#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include <asrc_prv.h>
#include <asrc.h>
#include <asrc_vybrid.h>
#include <asrc_vybrid_prv.h>

#include "dma.h"

//#ifdef __ASRC_USE_EDMA_DRIVER
//#include "edma_channel.h"
//#endif




//#define ASRC_VYBRID_INTERNAL_ENABLE_DEBUG
#ifdef ASRC_VYBRID_INTERNAL_ENABLE_DEBUG
#define ASRC_VYBRID_INTERNAL_DEBUG  printf
#else
#define ASRC_VYBRID_INTERNAL_DEBUG(...)
#endif

#define ASRC_VYBRID_INTERNAL_ENABLE_ERROR
#ifdef ASRC_VYBRID_INTERNAL_ENABLE_ERROR
#define ASRC_VYBRID_INTERNAL_ERROR  printf
#else
#define ASRC_VYBRID_INTERNAL_ERROR(...)
#endif

#define MAX_ASRC_LIBRARY_PROVIDER_MEMSIZE  1048576 /*1Mbytes*/

#define ASRC_RATIO_DECIMAL_DEPTH 26

#define ASRC_FIFO_MAX_SIZE 64

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

static const uint32_t asrc_input_sample_rates[] = {
    5512, 8000, 11025, 16000, 22050, 32000, 44100, 48000, 64000,
    88200, 96000, 176400, 192000,
};

static const uint32_t asrc_output_sample_rates[] = {
    32000, 44100, 48000, 64000,
    88200, 96000, 176400, 192000,
};

extern uint32_t _bsp_asrc_get_audio_extal(void);
extern uint32_t _bsp_asrc_get_custom_clock(void);


/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_asrc_calc_clock_divider
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function returns the clock divider for this division fact
*    Not support the ASRC SERIAL CLK
*
*END*********************************************************************/
static int32_t _vybrid_asrc_calc_clock_divider
(
    /* [IN] target_fact, it is calculated by "main_clock / sample_rate"*/
    uint32_t target_fact,

    /* [OUT] returns divider value*/
    uint32_t *div,

    /* [OUT] returns prescale value*/
    uint32_t *prescale
)
{

    int32_t i = 0;
    uint32_t val = 1;
    uint32_t div_val = 0;

    /*The max fact is 1024= 128(prescale) * 8 (divider)*/
    if (target_fact > 1024)
        return -1;

    /*try to find the prescale which is power of 2 from 1 to 128*/
    for (i=0; i<8;i++){
        val = 1<<i;

        if (val & target_fact) {
            break;
        }
    }

    if (i == 8)
        i = 7;

    /*get the part for divider*/
    div_val = target_fact >> i;

    if (div_val > 8)
        return -2;

    /*The divide ratio may range from 1 to 8 (register val = 0 to 7)*/
    *div = div_val-1;
    *prescale = i;

    return 0;
}


/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_asrc_set_clock_ratio
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function selects ASRC clock ratio for this sample rate pair
*
*END*********************************************************************/
static int32_t _vybrid_asrc_set_clock_ratio
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node,

    /* [IN] input sample rate for this ASRC pair*/
    int32_t input_sample_rate,

    /* [IN] output sample rate for this ASRC pair*/
    int32_t output_sample_rate
)
{
    int32_t i;
    int32_t integ = 0;
    unsigned long reg_val = 0;
    ASRC_MemMapPtr asrc_regs = asrc_set_node->asrc_info_ptr->asrc_regs;

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

    if (0 == asrc_set_node->asrc_pair_id) {
        asrc_regs->ASRIDRLA = reg_val;
        asrc_regs->ASRIDRHA = (reg_val >> 24);
    } else if (1 == asrc_set_node->asrc_pair_id) {
        asrc_regs->ASRIDRLB = reg_val;
        asrc_regs->ASRIDRHB = (reg_val >> 24);
    } else if (2 == asrc_set_node->asrc_pair_id) {
        asrc_regs->ASRIDRLC = reg_val;
        asrc_regs->ASRIDRHC = (reg_val >> 24);
    }

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_asrc_set_process_configuration
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function selects ASRC processes for this sample rate pair
*
*END*********************************************************************/
static int32_t _vybrid_asrc_set_process_configuration
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node,

    /* [IN] input sample rate for this ASRC pair*/
    int32_t input_sample_rate,

    /* [IN] output sample rate for this ASRC pair*/
    int32_t output_sample_rate
)
{
    int32_t i = 0, j = 0;
    unsigned long reg;
    ASRC_MemMapPtr asrc_regs = asrc_set_node->asrc_info_ptr->asrc_regs;

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
    reg &= ~(ASRC_ASRCFG_PREMOD_MASK(asrc_set_node->asrc_pair_id));
    reg &= ~(ASRC_ASRCFG_POSTMOD_MASK(asrc_set_node->asrc_pair_id));
    reg |=
        ((asrc_process_table[i][j][0] <<
            ASRC_ASRCFG_PREMOD_SHIFT(asrc_set_node->asrc_pair_id)) |
         (asrc_process_table[i][j][1] <<
            ASRC_ASRCFG_POSTMOD_SHIFT(asrc_set_node->asrc_pair_id)));
    asrc_regs->ASRCFG = reg;

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_asrc_ccm_clocks_enable
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function enables the CCM clocks for ASRC
*
*END*********************************************************************/
static int32_t _vybrid_asrc_ccm_clocks_enable()
{
    /*enable asrc root clock and ipg clock*/

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_check_config_state
* Returned Value   : void
* Comments         :
*    This function checks if the ASRC set node in init or stop state
*
*END*********************************************************************/
static inline bool vybrid_asrc_check_config_state
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    if (ASRC_STOP == asrc_set_node->state ||
        ASRC_INIT == asrc_set_node->state)
        return TRUE;

    return FALSE;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_check_running_state
* Returned Value   : void
* Comments         :
*    This function checks if the ASRC set node in running state
*
*END*********************************************************************/
static inline bool vybrid_asrc_check_running_state
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    if (ASRC_RUNNING == asrc_set_node->state)
        return TRUE;

    return FALSE;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_check_stop_state
* Returned Value   : void
* Comments         :
*    This function checks if the ASRC set node in stop state
*
*END*********************************************************************/
static inline bool vybrid_asrc_check_stop_state
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    if (ASRC_STOP == asrc_set_node->state)
        return TRUE;

    return FALSE;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_hw_reset
* Returned Value   : void
* Comments         :
*    This function resets the ASRC HW
*
*END*********************************************************************/
static void vybrid_asrc_hw_reset
(
    /* [IN] registers base pointer for thie ASRC device*/
    ASRC_MemMapPtr asrc_regs
)
{
    uint32_t regVal = 0;

    /*reset asrc hardware*/
    regVal = asrc_regs->ASRCTR;
    regVal |= ASRC_ASRCTR_SRST_MASK;
    asrc_regs->ASRCTR = regVal;

    /*out of reset*/
    while (asrc_regs->ASRCTR & ASRC_ASRCTR_SRST_MASK)
    {}

    regVal = ASRC_ASRCTR_ASRCEN_MASK;
    asrc_regs->ASRCTR = regVal;

    /*set recommend parameters value*/
    asrc_regs->ASRPMn[0] = 0x7fffff;
    asrc_regs->ASRPMn[1] = 0x255555;
    asrc_regs->ASRPMn[2] = 0xff7280;
    asrc_regs->ASRPMn[3] = 0xff7280;
    asrc_regs->ASRPMn[4] = 0xff7280;

    /*set mandatory value to task queue FIFO*/
    asrc_regs->ASRTFR1 = 0x001f00;

    /*set processing clock for 76Khz, FsASRC is ipg clock 66Mhz*/
    //asrc_regs->ASR76K = 0x6D6;
    //asrc_regs->ASR76K = 0x30E;
    asrc_regs->ASR76K = 0x36B;

    /*set processing clock for 56Khz, FsASRC is ipg clock 66Mhz*/
    //asrc_regs->ASR56K = 0x947;
    //asrc_regs->ASR56K = 0x426;
    asrc_regs->ASR56K = 0x4A3;

    /*default 2:2:2 channels assignment, pair A use 2 channels*/
    asrc_regs->ASRCNCR = (0x2<<8 | 0x2<<4 | 0x2);

}


/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_hw_set_channel_num
* Returned Value   : void
* Comments         :
*    This function applys channel number to one ASRC pair
*
*END*********************************************************************/
static void vybrid_asrc_hw_set_channel_num
(
    /* [IN] asrc pair id*/
    uint32_t asrc_set_id,

    /* [IN] registers base pointer for thie ASRC device*/
    ASRC_MemMapPtr asrc_regs,

    /* [IN] channels number to be set*/
    uint8_t channel_num
)
{
    uint32_t regVal = 0;

    if (asrc_set_id >= 3)
        return;

    regVal = asrc_regs->ASRCNCR;

    regVal &= ~(ASRC_ASRCNCR_ANC_MASK(asrc_set_id));

    regVal |= (uint32_t)(channel_num << ASRC_ASRCNCR_ANC_SHIFT(asrc_set_id));

    asrc_regs->ASRCNCR = regVal;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_dump_regs
* Returned Value   : void
* Comments         :
*    This function dumps all ASRC registers
*
*END*********************************************************************/
void vybrid_asrc_dump_regs(ASRC_MemMapPtr asrc_regs)
{
    ASRC_VYBRID_INTERNAL_DEBUG("============DUMP ASRC============\n");
    ASRC_VYBRID_INTERNAL_DEBUG("ASRCTR=0x%x\n", asrc_regs->ASRCTR);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRIER=0x%x\n", asrc_regs->ASRIER);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRCNCR=0x%x\n", asrc_regs->ASRCNCR);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRCFG=0x%x\n", asrc_regs->ASRCFG);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRCSR=0x%x\n", asrc_regs->ASRCSR);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRCDR1=0x%x\n", asrc_regs->ASRCDR1);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRCDR2=0x%x\n", asrc_regs->ASRCDR2);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRSTR=0x%x\n", asrc_regs->ASRSTR);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRPMN1=0x%x\n", asrc_regs->ASRPMN1);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRPMN2=0x%x\n", asrc_regs->ASRPMN2);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRPMN3=0x%x\n", asrc_regs->ASRPMN3);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRPMN4=0x%x\n", asrc_regs->ASRPMN4);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRPMN5=0x%x\n", asrc_regs->ASRPMN5);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRTFR1=0x%x\n", asrc_regs->ASRTFR1);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRCCR=0x%x\n", asrc_regs->ASRCCR);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRDIDRHA=0x%x\n", asrc_regs->ASRDIDRHA);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRDIDRLA=0x%x\n", asrc_regs->ASRDIDRLA);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRDIDRHB=0x%x\n", asrc_regs->ASRDIDRHB);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRDIDRLB=0x%x\n", asrc_regs->ASRDIDRLB);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRDIDRHC=0x%x\n", asrc_regs->ASRDIDRHC);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRDIDRLC=0x%x\n", asrc_regs->ASRDIDRLC);
    ASRC_VYBRID_INTERNAL_DEBUG("ASR76K=0x%x\n", asrc_regs->ASR76K);
    ASRC_VYBRID_INTERNAL_DEBUG("ASR56K=0x%x\n", asrc_regs->ASR56K);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRMCRA=0x%x\n", asrc_regs->ASRMCRA);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRFSTA=0x%x\n", asrc_regs->ASRFSTA);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRMCRB=0x%x\n", asrc_regs->ASRMCRB);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRFSTB=0x%x\n", asrc_regs->ASRFSTB);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRMCRC=0x%x\n", asrc_regs->ASRMCRC);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRFSTC=0x%x\n", asrc_regs->ASRFSTC);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRMCR1A=0x%x\n", asrc_regs->ASRMCR1A);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRMCR1B=0x%x\n", asrc_regs->ASRMCR1B);
    ASRC_VYBRID_INTERNAL_DEBUG("ASRMCR1C=0x%x\n", asrc_regs->ASRMCR1C);
    ASRC_VYBRID_INTERNAL_DEBUG("============DUMP ASRC END============\n");

}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_start
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function starts one ASRC pair to work
*
*END*********************************************************************/
int32_t vybrid_asrc_start
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    uint32_t regVal = 0;
    ASRC_MemMapPtr asrc_regs;

    if (NULL == asrc_set_node ||
        NULL == asrc_set_node->asrc_info_ptr ||
        NULL == asrc_set_node->asrc_info_ptr->asrc_regs)
        return -1;

    if (ASRC_PAIRS_NUM <= asrc_set_node->asrc_pair_id)
        return -2;

    _mutex_lock(&asrc_set_node->access_mutex);

    if (!vybrid_asrc_check_stop_state(asrc_set_node)) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -3;
    }

    asrc_regs = asrc_set_node->asrc_info_ptr->asrc_regs;

    /*enable asrc pair*/
    regVal = asrc_regs->ASRCTR;
    regVal |= ASRC_ASRCTR_ASRE(asrc_set_node->asrc_pair_id);
    asrc_regs->ASRCTR = regVal;

    regVal = asrc_regs->ASRCFG;
    while (!(regVal & ASRC_ASRCFG_INIRQ(asrc_set_node->asrc_pair_id)))
        regVal= asrc_regs->ASRCFG;

    asrc_set_node->state = ASRC_RUNNING;

    _mutex_unlock(&asrc_set_node->access_mutex);

    return 0;

}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_stop
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function stops one ASRC pair working
*
*END*********************************************************************/
int32_t vybrid_asrc_stop
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    ASRC_MemMapPtr asrc_regs;
    uint32_t regVal = 0;

    if (NULL == asrc_set_node ||
        NULL == asrc_set_node->asrc_info_ptr ||
        NULL == asrc_set_node->asrc_info_ptr->asrc_regs)
        return -1;

    if (ASRC_PAIRS_NUM <= asrc_set_node->asrc_pair_id)
        return -2;

    _mutex_lock(&asrc_set_node->access_mutex);

    if (!vybrid_asrc_check_running_state(asrc_set_node)) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return 0;
    }

    asrc_regs = asrc_set_node->asrc_info_ptr->asrc_regs;

    /*disable asrc pair A*/
    regVal = asrc_regs->ASRCTR;
    regVal &= ~(ASRC_ASRCTR_ASRE(asrc_set_node->asrc_pair_id));
    asrc_regs->ASRCTR = regVal;

    asrc_set_node->state = ASRC_STOP;

    _mutex_unlock(&asrc_set_node->access_mutex);

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_set_channels
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function configures the channels number for every ASRC pair
*
*END*********************************************************************/

int32_t vybrid_asrc_set_channels
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node,

    /* [IN] channels number for this pair*/
    uint8_t channels_number
)
{
    if (NULL == asrc_set_node)
        return -1;

    _mutex_lock(&asrc_set_node->access_mutex);

    if (!vybrid_asrc_check_config_state(asrc_set_node)) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -2;
    }

    asrc_set_node->pair_channels = channels_number;

    vybrid_asrc_hw_set_channel_num(asrc_set_node->asrc_pair_id,
        asrc_set_node->asrc_info_ptr->asrc_regs, channels_number);

    _mutex_unlock(&asrc_set_node->access_mutex);

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_asrc_remap_clk
* Returned Value   : the definition in the Clock Source Register
* Comments         :
*    This function maps the asrc clock sources from the API definitions to register definitions
*
*END*********************************************************************/
static VYBRID_ASRC_CLK _vybrid_asrc_remap_clk
   (
      /* [IN] A string that identifies the device for fopen */
      ASRC_CLK   asrc_clk
   )
{
    switch (asrc_clk) {
        case ASRC_CLK_NONE:
            return VYBRID_CLK_NONE;
        case ASRC_CLK_ESAI_RX:
            return VYBRID_CLK_ESAI_RX;
        case ASRC_CLK_SAI0_RX:
            return VYBRID_CLK_SAI0_RX;
        case ASRC_CLK_SAI1_RX:
            return VYBRID_CLK_SAI1_RX;
        case ASRC_CLK_SAI2_RX:
            return VYBRID_CLK_SAI2_RX;
        case ASRC_CLK_SPDIF_RX:
            return VYBRID_CLK_SPDIF_RX;
        case ASRC_CLK_MLB_CLK:
            return VYBRID_CLK_MLB_CLK;
        case ASRC_CLK_ESAI_TX:
            return VYBRID_CLK_ESAI_TX;
        case ASRC_CLK_SAI0_TX:
            return VYBRID_CLK_SAI0_TX;
        case ASRC_CLK_SAI3_TX:
            return VYBRID_CLK_SAI3_TX;
        case ASRC_CLK_SAI1_TX:
            return VYBRID_CLK_SAI1_TX;
        case ASRC_CLK_SPDIF_TX:
            return VYBRID_CLK_SPDIF_TX;
        case ASRC_CLK_CUSTOM_CLK:
            return VYBRID_CLK_PLL4_DIV;
        case ASRC_CLK_EXT_AUD_CLK:
            return VYBRID_CLK_EXT_AUD_CLK;
        case ASRC_CLK_NA:
            return VYBRID_CLK_NA;
        default:
            return VYBRID_CLK_NA;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_asrc_set_input_divider
* Returned Value   : return 0 if success, <0 if failure
* Comments         :
*    This function updates the input divider value according with the ASRC configuration
*
*END*********************************************************************/
static int32_t _vybrid_asrc_set_input_divider
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    uint8_t asrc_pair_id,

    /* [IN] the pointer to asrc configuration structure*/
    ASRC_SET_CONFIG_STRUCT_PTR new_config,

    /* [IN&OUT] the ASRC_ASRCDR register value, output the updated CDR value*/
    uint32_t *cdr_value
)
{
    uint32_t icd_shift, icd_mask, icp_shift, icp_mask;
    uint32_t regVal = *cdr_value;
    int32_t ret = 0;
    uint32_t div = 0,prescale = 0;

    switch (asrc_pair_id) {
        case 0:
            icd_shift = ASRC_ASRCDR1_AICDA_SHIFT;
            icd_mask  = ASRC_ASRCDR1_AICDA_MASK;
            icp_shift = ASRC_ASRCDR1_AICPA_SHIFT;
            icp_mask  = ASRC_ASRCDR1_AICPA_MASK;
            break;
        case 1:
            icd_shift = ASRC_ASRCDR1_AICDB_SHIFT;
            icd_mask  = ASRC_ASRCDR1_AICDB_MASK;
            icp_shift = ASRC_ASRCDR1_AICPB_SHIFT;
            icp_mask  = ASRC_ASRCDR1_AICPB_MASK;
            break;
        case 2:
            icd_shift = ASRC_ASRCDR2_AICDC_SHIFT;
            icd_mask  = ASRC_ASRCDR2_AICDC_MASK;
            icp_shift = ASRC_ASRCDR2_AICPC_SHIFT;
            icp_mask  = ASRC_ASRCDR2_AICPC_MASK;
            break;
        default:
            return -1;
    }


    regVal &= ~(icp_mask);
    regVal &= ~(icd_mask);
    switch (new_config->input_ref_clk) {
        case ASRC_CLK_SPDIF_RX:
            regVal |= (7 << icp_shift);
            break;
        case ASRC_CLK_SPDIF_TX:
            regVal |= (6 << icp_shift);
            break;
        case ASRC_CLK_CUSTOM_CLK:
            {
                uint32_t aud_clk = _bsp_asrc_get_custom_clock();
                uint32_t target_factor = 0;

                if (aud_clk % new_config->input_sample_rate) {
                    ASRC_VYBRID_INTERNAL_ERROR("ASRCK1 not support "
                        "this sample rate, the input_sample_rate = 0x%x\n"
                        "on clock source[%d]",
                        new_config->input_sample_rate,
                        new_config->input_ref_clk);
                    return -3;
                }

                target_factor = aud_clk /
                    new_config->input_sample_rate;

                if (target_factor % (new_config->input_slot_width << 1)) {
                    ASRC_VYBRID_INTERNAL_ERROR("Not support "
                        "this input clock div, slot = 0x%x\n"
                        "on clock source[%d]",
                        new_config->input_slot_width,
                        new_config->input_ref_clk);
                    return -4;
                }

                ret = _vybrid_asrc_calc_clock_divider(target_factor,
                    &div, &prescale);
                if (0 == ret) {
                    regVal |= (div << icd_shift);
                    regVal |= (prescale << icp_shift);
                } else {
                    ASRC_VYBRID_INTERNAL_ERROR("ASRCK1 fails to support "
                        "this sample rate, the input_sample_rate = 0x%x\n",
                        new_config->input_sample_rate);
                    return -5;
                }
                break;
            }
        case ASRC_CLK_NONE:
            break;
        default:
            if (8 == new_config->input_slot_width) {
                regVal |= (4 << icp_shift);
            } else if (16 == new_config->input_slot_width) {
                regVal |= (5 << icp_shift);
            } else if (24 == new_config->input_slot_width) {
                regVal |= (4 << icp_shift);
                regVal |= (2 << icd_shift); /*3*/
            } else if (32 == new_config->input_slot_width) {
                regVal |= (6 << icp_shift);
            } else {
                ASRC_VYBRID_INTERNAL_ERROR("Not support "
                    "this input clock div, slot = 0x%x\n",
                    new_config->input_slot_width);
                return -3;
            }
            break;
    }

    *cdr_value = regVal;

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_asrc_set_output_divider
* Returned Value   : return 0 if success, <0 if failure
* Comments         :
*    This function updates the output divider value according with the ASRC configuration
*
*END*********************************************************************/
static int32_t _vybrid_asrc_set_output_divider
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    uint8_t asrc_pair_id,

    /* [IN] the pointer to asrc configuration structure*/
    ASRC_SET_CONFIG_STRUCT_PTR new_config,

    /* [IN/OUT] the ASRC_ASRCDR register value, output the updated CDR value*/
    uint32_t *cdr_value
)
{
    uint32_t ocd_shift, ocd_mask, ocp_shift, ocp_mask;
    uint32_t regVal = *cdr_value;
    int32_t ret = 0;
    uint32_t div = 0, prescale = 0;

    switch (asrc_pair_id) {
        case 0:
            ocd_shift = ASRC_ASRCDR1_AOCDA_SHIFT;
            ocd_mask  = ASRC_ASRCDR1_AOCDA_MASK;
            ocp_shift = ASRC_ASRCDR1_AOCPA_SHIFT;
            ocp_mask  = ASRC_ASRCDR1_AOCPA_MASK;
            break;
        case 1:
            ocd_shift = ASRC_ASRCDR1_AOCDB_SHIFT;
            ocd_mask  = ASRC_ASRCDR1_AOCDB_MASK;
            ocp_shift = ASRC_ASRCDR1_AOCPB_SHIFT;
            ocp_mask  = ASRC_ASRCDR1_AOCPB_MASK;
            break;
        case 2:
            ocd_shift = ASRC_ASRCDR2_AOCDC_SHIFT;
            ocd_mask  = ASRC_ASRCDR2_AOCDC_MASK;
            ocp_shift = ASRC_ASRCDR2_AOCPC_SHIFT;
            ocp_mask  = ASRC_ASRCDR2_AOCPC_MASK;
            break;
        default:
            return -1;
    }

    regVal &= ~(ocp_mask);
    regVal &= ~(ocd_mask);
    switch (new_config->output_ref_clk) {
        case ASRC_CLK_SPDIF_RX:
            regVal |= (7 << ocp_shift);
            break;
        case ASRC_CLK_SPDIF_TX:
            regVal |= (6 << ocp_shift);
            break;
        case ASRC_CLK_CUSTOM_CLK:
        case ASRC_CLK_EXT_AUD_CLK:
            {
                uint32_t aud_clk;
                uint32_t target_factor = 0;

                if (ASRC_CLK_EXT_AUD_CLK == new_config->output_ref_clk)
                    aud_clk = _bsp_asrc_get_audio_extal();
                else
                    aud_clk = _bsp_asrc_get_custom_clock();

                if (aud_clk % new_config->output_sample_rate) {
                    ASRC_VYBRID_INTERNAL_ERROR("ASRCK1 not support "
                        "this sample rate, the output_sample_rate = 0x%x\n"
                        "on clock source[%d]",
                        new_config->output_sample_rate,
                        new_config->output_ref_clk);
                    return -3;
                }

                target_factor = aud_clk /
                    new_config->output_sample_rate;

                if (target_factor % (new_config->output_slot_width << 1)) {
                    ASRC_VYBRID_INTERNAL_ERROR("Not support "
                        "this output clock div, slot = 0x%x\n"
                        "on clock source[%d]",
                        new_config->output_slot_width,
                        new_config->output_ref_clk);
                    return -4;
                }

                ret = _vybrid_asrc_calc_clock_divider(target_factor,
                    &div, &prescale);
                if (0 == ret) {
                    regVal |= (div << ocd_shift);
                    regVal |= (prescale << ocp_shift);
                } else {
                    ASRC_VYBRID_INTERNAL_ERROR("ASRCK1 fails to support "
                        "this sample rate, the output_sample_rate = 0x%x\n",
                        new_config->output_sample_rate);
                    return -5;
                }
            }
            break;
        default:
            if (8 == new_config->output_slot_width) {
                regVal |= (4 << ocp_shift);
            } else if (16 == new_config->output_slot_width) {
                regVal |= (5 << ocp_shift);
            } else if (24 == new_config->output_slot_width) {
                regVal |= (4 << ocp_shift);
                regVal |= (2 << ocd_shift);
            } else if (32 == new_config->output_slot_width){
                regVal |= (6 << ocp_shift);
            } else {
                ASRC_VYBRID_INTERNAL_ERROR("Not support "
                    "this output clock div, slot = 0x%x\n",
                    new_config->output_slot_width);

                return -6;
            }
            break;
    }

    *cdr_value = regVal;

    return 0;
}


/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_config
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function applys the configuration to ASRC hw
*
*END*********************************************************************/
int32_t vybrid_asrc_config
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node,

    /* [IN] the pointer to asrc configuration structure*/
    ASRC_SET_CONFIG_STRUCT_PTR new_config
)
{
    uint32_t regVal = 0;
    ASRC_MemMapPtr asrc_regs;
    volatile uint32_t *asrc_reg_ptr = NULL;
    int32_t ret = 0, result = 0, i = 0, num;
    uint32_t clk;

    if (NULL == asrc_set_node || NULL == new_config)
        return -1;

    if (new_config->input_fifo_threshold >= ASRC_FIFO_MAX_SIZE ||
        new_config->output_fifo_threshold >= ASRC_FIFO_MAX_SIZE) {
        ASRC_VYBRID_INTERNAL_ERROR("CONFIG threshold error, "
            "input_threshold = 0x%x, output_threshold= 0x%x\n",
            new_config->input_fifo_threshold,
            new_config->output_fifo_threshold);
        return -2;
    }

    if (new_config->input_fifo_fmt >= ASRC_FIFO_FORMAT_NA ||
        new_config->output_fifo_fmt >= ASRC_FIFO_FORMAT_NA) {
        ASRC_VYBRID_INTERNAL_ERROR("CONFIG fifo fmt error, "
            "input_fifo_fmt = 0x%x, output_fifo_fmt= 0x%x\n",
            new_config->input_fifo_fmt,
            new_config->output_fifo_fmt);
        return -2;
    }

    if ((ASRC_CLK_NONE != new_config->input_ref_clk) &&
        (64 != new_config->input_slot_width &&
        32 != new_config->input_slot_width &&
        24 != new_config->input_slot_width &&
        16 != new_config->input_slot_width &&
        8 != new_config->input_slot_width)){
        ASRC_VYBRID_INTERNAL_ERROR("CONFIG input slot width error, "
            "input_slot_width = 0x%x\n", new_config->input_slot_width);
        return -2;
    }

    if ((ASRC_CLK_CUSTOM_CLK != new_config->output_ref_clk) &&
        (64 != new_config->output_slot_width &&
        32 != new_config->output_slot_width &&
        24 != new_config->output_slot_width &&
        16 != new_config->output_slot_width &&
        8 != new_config->output_slot_width)){
        ASRC_VYBRID_INTERNAL_ERROR("CONFIG output slot width error, "
            "output_slot_width = 0x%x\n", new_config->output_slot_width);
        return -2;
    }

    if (ASRC_CLK_NA <= new_config->input_ref_clk) {

        ASRC_VYBRID_INTERNAL_ERROR("CONFIG input reference clock error, "
            "input_ref_clk = 0x%x\n", new_config->input_ref_clk);
        return -2;
    }

    if (ASRC_CLK_NA <= new_config->output_ref_clk ||
        ASRC_CLK_NONE == new_config->output_ref_clk) {

        ASRC_VYBRID_INTERNAL_ERROR("CONFIG output reference clock error, "
            "output_ref_clk = 0x%x\n", new_config->output_ref_clk);
        return -2;
    }

    num = sizeof(asrc_input_sample_rates)/sizeof(asrc_input_sample_rates[0]);
    for (i=0; i<num; i++) {
        if (new_config->input_sample_rate == asrc_input_sample_rates[i]) {
            break;
        }
    }
    if (num == i) {
        ASRC_VYBRID_INTERNAL_ERROR("CONFIG input sample rate not support, "
            "input_sample_rate = 0x%x\n", new_config->input_sample_rate);
        return -2;
    }

    num = sizeof(asrc_output_sample_rates)/sizeof(asrc_output_sample_rates[0]);
    for (i=0; i<num; i++) {
        if (new_config->output_sample_rate == asrc_output_sample_rates[i]) {
            break;
        }
    }
    if (num == i) {
        ASRC_VYBRID_INTERNAL_ERROR("CONFIG output sample rate not support, "
            "output_sample_rate = 0x%x\n", new_config->output_sample_rate);
        return -2;
    }

    _mutex_lock(&asrc_set_node->access_mutex);

    if (NULL == asrc_set_node->asrc_info_ptr ||
        NULL == asrc_set_node->asrc_info_ptr->asrc_regs ||
        ASRC_PAIRS_NUM <= asrc_set_node->asrc_pair_id) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -3;
    }

    if (!vybrid_asrc_check_config_state(asrc_set_node)) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -4;
    }

    asrc_regs = asrc_set_node->asrc_info_ptr->asrc_regs;

    /*set input ref clock and output ref clock*/

    regVal = asrc_regs->ASRCSR;
    regVal &= ~(ASRC_ASRCSR_AICS_MASK(asrc_set_node->asrc_pair_id));
    regVal &= ~(ASRC_ASRCSR_AOCS_MASK(asrc_set_node->asrc_pair_id));

    clk = _vybrid_asrc_remap_clk(new_config->input_ref_clk);
    regVal |= (clk <<
        ASRC_ASRCSR_AICS_SHIFT(asrc_set_node->asrc_pair_id));

    clk = _vybrid_asrc_remap_clk(new_config->output_ref_clk);
    regVal |= (clk <<
        ASRC_ASRCSR_AOCS_SHIFT(asrc_set_node->asrc_pair_id));
    asrc_regs->ASRCSR = regVal;

    /*set divider*/
    if (0 == asrc_set_node->asrc_pair_id || 1 == asrc_set_node->asrc_pair_id) {
        regVal = asrc_regs->ASRCDR1;

        ret = _vybrid_asrc_set_input_divider(asrc_set_node->asrc_pair_id,
            new_config, &regVal);
        if (0 != ret)
            result = -5;

        ret = _vybrid_asrc_set_output_divider(asrc_set_node->asrc_pair_id,
            new_config, &regVal);
        if (0 != ret)
            result = -6;

        asrc_regs->ASRCDR1 = regVal;

    } else { /*pair id == 2*/
        regVal = asrc_regs->ASRCDR2;

        ret = _vybrid_asrc_set_input_divider(asrc_set_node->asrc_pair_id,
            new_config, &regVal);
        if (0 != ret)
            result = -5;

        ret = _vybrid_asrc_set_output_divider(asrc_set_node->asrc_pair_id,
            new_config, &regVal);
        if (0 != ret)
            result = -6;

        asrc_regs->ASRCDR2 = regVal;
    }

    if (ASRC_CLK_NONE == new_config->input_ref_clk) {
        /*decide whether to use ideal ratio and manual procession*/
        regVal = asrc_regs->ASRCTR;
        regVal &= ~(ASRC_ASRCTR_ATS(asrc_set_node->asrc_pair_id));
        regVal |= (ASRC_ASRCTR_USR(asrc_set_node->asrc_pair_id));
        regVal |= (ASRC_ASRCTR_IDR(asrc_set_node->asrc_pair_id));
        asrc_regs->ASRCTR = regVal;

        _vybrid_asrc_set_clock_ratio(asrc_set_node,
            (int32_t)new_config->input_sample_rate,
            (int32_t)new_config->output_sample_rate);

        /*set processes configuration*/
        _vybrid_asrc_set_process_configuration(asrc_set_node,
            new_config->input_sample_rate, new_config->output_sample_rate);
    } else {
        /*enable auto procession*/
        regVal = asrc_regs->ASRCTR;
        regVal |= (ASRC_ASRCTR_ATS(asrc_set_node->asrc_pair_id));
        regVal |= (ASRC_ASRCTR_USR(asrc_set_node->asrc_pair_id));
        regVal &= ~(ASRC_ASRCTR_IDR(asrc_set_node->asrc_pair_id));
        asrc_regs->ASRCTR = regVal;
    }

    /*set input and output threshold*/
    asrc_reg_ptr =(volatile uint32_t *)&(asrc_regs->ASRMCRA);
    asrc_reg_ptr = asrc_reg_ptr + (asrc_set_node->asrc_pair_id << 1);
    regVal = *asrc_reg_ptr;
    regVal &= ~(ASRC_ASRMCRA_OUTFIFO_THRESHOLDA_MASK);
    regVal &= ~(ASRC_ASRMCRA_INFIFO_THRESHOLDA_MASK);
    regVal &= ~ASRC_ASRMCRA_ZEROBUFA_MASK;
    regVal |= ASRC_ASRMCRA_BUFSTALLA_MASK;
    regVal |= ASRC_ASRMCRA_EXTTHRSHA_MASK;
    regVal |= (new_config->output_fifo_threshold
        << ASRC_ASRMCRA_OUTFIFO_THRESHOLDA_SHIFT);
    regVal |= (new_config->input_fifo_threshold
        << ASRC_ASRMCRA_INFIFO_THRESHOLDA_SHIFT);
    *asrc_reg_ptr = regVal;

    /*set input and output fifo word format, 16bits LSB aligned*/
    asrc_reg_ptr =(volatile uint32_t *)&(asrc_regs->ASRMCR1[0]);
    asrc_reg_ptr = asrc_reg_ptr + asrc_set_node->asrc_pair_id;
    regVal = *asrc_reg_ptr;
    switch (new_config->input_fifo_fmt) {
        case ASRC_FIFO_8BITS_LSB:
            regVal &= ~(ASRC_ASRMCR1_IWD_MASK);
            regVal &= ~(ASRC_ASRMCR1_IMSB_MASK);
            regVal |= (0x2 << ASRC_ASRMCR1_IWD_SHIFT);
            break;
        case ASRC_FIFO_8BITS_MSB:
            regVal &= ~(ASRC_ASRMCR1_IWD_MASK);
            regVal |= (0x2 << ASRC_ASRMCR1_IWD_SHIFT);
            regVal |= ASRC_ASRMCR1_IMSB_MASK;
            break;
        case ASRC_FIFO_16BITS_LSB:
            regVal &= ~(ASRC_ASRMCR1_IWD_MASK);
            regVal &= ~(ASRC_ASRMCR1_IMSB_MASK);
            regVal |= (0x1 << ASRC_ASRMCR1_IWD_SHIFT);
            break;
        case ASRC_FIFO_16BITS_MSB:
            regVal &= ~(ASRC_ASRMCR1_IWD_MASK);
            regVal |= (0x1 << ASRC_ASRMCR1_IWD_SHIFT);
            regVal |= ASRC_ASRMCR1_IMSB_MASK;
            break;
        case ASRC_FIFO_24BITS_LSB:
            regVal &= ~(ASRC_ASRMCR1_IWD_MASK);
            regVal &= ~(ASRC_ASRMCR1_IMSB_MASK);
            break;
        case ASRC_FIFO_24BITS_MSB:
            regVal &= ~(ASRC_ASRMCR1_IWD_MASK);
            regVal |= ASRC_ASRMCR1_IMSB_MASK;
            break;
        default:
            ASRC_VYBRID_INTERNAL_ERROR("Error input_fifo_fmt, "
                "input_fifo_fmt = 0x%x\n", new_config->input_fifo_fmt);
            result = -7;
            break;
    }

    switch (new_config->output_fifo_fmt) {
        case ASRC_FIFO_16BITS_LSB:
            regVal |= ASRC_ASRMCR1_OW16_MASK;
            regVal &= ~(ASRC_ASRMCR1_OMSB_MASK);
            regVal &= ~(ASRC_ASRMCR1_OSGN_MASK);
            break;
        case ASRC_FIFO_16BITS_MSB:
            regVal |= ASRC_ASRMCR1_OW16_MASK;
            regVal |= ASRC_ASRMCR1_OMSB_MASK;
            regVal &= ~(ASRC_ASRMCR1_OSGN_MASK);
            break;
        case ASRC_FIFO_24BITS_LSB:
            regVal &= ~(ASRC_ASRMCR1_OW16_MASK);
            regVal &= ~(ASRC_ASRMCR1_OMSB_MASK);
            regVal &= ~(ASRC_ASRMCR1_OSGN_MASK);
            break;
        case ASRC_FIFO_24BITS_MSB:
            regVal &= ~(ASRC_ASRMCR1_OW16_MASK);
            regVal |= ASRC_ASRMCR1_OMSB_MASK;
            regVal &= ~(ASRC_ASRMCR1_OSGN_MASK);
            break;
        default:
            ASRC_VYBRID_INTERNAL_ERROR("Error output_fifo_fmt, "
                "output_fifo_fmt = 0x%x\n", new_config->output_fifo_fmt);
            result = -8;
            break;
    }
    *asrc_reg_ptr = regVal;

    _mutex_unlock(&asrc_set_node->access_mutex);

    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_install_service_source
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function installs the source dma for service provider mode
*
*END*********************************************************************/

int32_t vybrid_asrc_install_service_source
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node,

    /* [IN] request service type, identify the dma channel type*/
    ASRC_SERVICE_TYPE service_type,

    /* [OUT] dma channel id*/
    DMA_CHANNEL_HANDLE *channel
)
{
    int32_t dma_request;
    int32_t result = 0;

    if (NULL == asrc_set_node || NULL == channel)
        return -1;

    *channel = 0;

    _mutex_lock(&asrc_set_node->access_mutex);

    if (ASRC_PAIRS_NUM <= asrc_set_node->asrc_pair_id) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -2;
    }

    if (!vybrid_asrc_check_config_state(asrc_set_node)) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -3;
    }

    if (NULL != asrc_set_node->input_dma_channel) {
        *channel = asrc_set_node->input_dma_channel;
        _mutex_unlock(&asrc_set_node->access_mutex);
        return 0;
    }

    switch (service_type) {
        case ASRC_SERVICE_MEM:
            dma_request = DMA_REQ_ASRCA_RX + asrc_set_node->asrc_pair_id;
            break;

        default:
            ASRC_VYBRID_INTERNAL_ERROR("Can't install source, "
                "service type %d is not supported\n", service_type);
            _mutex_unlock(&asrc_set_node->access_mutex);
            return -4;
    }

    result = dma_channel_claim(&asrc_set_node->input_dma_channel, ASRCA_DMA_RX_CHANNEL);

    result = dma_channel_setup(asrc_set_node->input_dma_channel, 8, 0);

    result = dma_request_source(asrc_set_node->input_dma_channel, dma_request);

    if (MQX_OK != result) {
        ASRC_VYBRID_INTERNAL_ERROR("source dma_request failed, request = %d\n",
            dma_request);
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -5;
    }

    *channel = asrc_set_node->input_dma_channel;

    if (NULL != asrc_set_node->output_dma_channel) {
        asrc_set_node->state= ASRC_STOP;
    }

    _mutex_unlock(&asrc_set_node->access_mutex);

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_install_service_dest
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function installs the destination dma for service provider mode
*
*END*********************************************************************/
int32_t vybrid_asrc_install_service_dest
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node,

    /* [IN] request service type, identify the dma channel type*/
    ASRC_SERVICE_TYPE service_type,

    /* [OUT] dma channel id*/
    DMA_CHANNEL_HANDLE *channel
)
{
    int32_t dma_request;
    int32_t result = 0;

    if (NULL == asrc_set_node || NULL == channel)
        return -1;

    *channel = 0;

    _mutex_lock(&asrc_set_node->access_mutex);

    if (ASRC_PAIRS_NUM <= asrc_set_node->asrc_pair_id) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -2;
    }

    if (!vybrid_asrc_check_config_state(asrc_set_node)) {
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -3;
    }

    if (NULL != asrc_set_node->output_dma_channel) {
        *channel = asrc_set_node->output_dma_channel;
        _mutex_unlock(&asrc_set_node->access_mutex);
        return 0;
    }

    switch (service_type) {
        case ASRC_SERVICE_MEM:
            dma_request = DMA_REQ_ASRCA_TX + asrc_set_node->asrc_pair_id;
            break;
        case ASRC_SERVICE_ESAI:
            dma_request = DMA_REQ_ASRCA_TX + asrc_set_node->asrc_pair_id;
            break;
        default:
            ASRC_VYBRID_INTERNAL_ERROR("Can't install destination, "
                "service type %d is not supported\n", service_type);
            _mutex_unlock(&asrc_set_node->access_mutex);
            return -4;
    }

    result = dma_channel_claim(&asrc_set_node->output_dma_channel, ASRCA_DMA_TX_CHANNEL);

    result = dma_channel_setup(asrc_set_node->output_dma_channel, 1, DMA_CHANNEL_FLAG_LOOP_MODE);

    result = dma_request_source(asrc_set_node->output_dma_channel, dma_request);

    if (MQX_OK != result) {
        ASRC_VYBRID_INTERNAL_ERROR("Destination dma_request failed, request = %d\n",
            dma_request);
        _mutex_unlock(&asrc_set_node->access_mutex);
        return -5;
    }

    *channel = asrc_set_node->output_dma_channel;

    ASRC_VYBRID_INTERNAL_DEBUG("input_dma_channel = %d, output_dma_channel = %d\n",
        asrc_set_node->input_dma_channel, asrc_set_node->output_dma_channel);
    if (NULL != asrc_set_node->input_dma_channel) {
        asrc_set_node->state = ASRC_STOP;
    }

    _mutex_unlock(&asrc_set_node->access_mutex);

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_uninstall_source
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function uninstall the source port, close the dma on this port
*
*END*********************************************************************/
int32_t vybrid_asrc_uninstall_source
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    if (NULL == asrc_set_node)
        return -1;

    if (!vybrid_asrc_check_config_state(asrc_set_node))
        return -2;

    if (NULL != asrc_set_node->input_dma_channel) {
        dma_channel_release(asrc_set_node->input_dma_channel);
        asrc_set_node->input_dma_channel = NULL;
        asrc_set_node->state = ASRC_INIT;
    } else {
        return 0;
    }

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_uninstall_dest
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function uninstall the destination port, close the dma on this port
*
*END*********************************************************************/
int32_t vybrid_asrc_uninstall_dest
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    if (NULL == asrc_set_node)
        return -1;

    if (!vybrid_asrc_check_config_state(asrc_set_node))
        return -2;

    if (NULL != asrc_set_node->output_dma_channel) {
        dma_channel_release(asrc_set_node->output_dma_channel);
        asrc_set_node->output_dma_channel = NULL;
        asrc_set_node->state = ASRC_INIT;
    } else {
        return 0;
    }

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_init
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function initilizates the ASRC device, enable the ASRC hw
*
*END*********************************************************************/
int32_t vybrid_asrc_init
(
    /* [IN] the pointer to asrc info structure for this device*/
    VYBRID_ASRC_INFO_STRUCT_PTR io_info_ptr
)
{
    uint32_t total_channels = 0;
    ASRC_MemMapPtr asrc_regs;
    uint32_t regVal = 0;

    if (NULL == io_info_ptr)
        return -1;

    asrc_regs = io_info_ptr->asrc_regs;

    _vybrid_asrc_ccm_clocks_enable();

    vybrid_asrc_hw_reset(asrc_regs);

    total_channels = io_info_ptr->INIT.PAIR_A_CHANNELS +
        io_info_ptr->INIT.PAIR_B_CHANNELS +
        io_info_ptr->INIT.PAIR_C_CHANNELS;

    if (ASRC_CHANNELS_NUM < total_channels) {
        return -2;
    }

    regVal = asrc_regs->ASRCNCR;

    regVal &= ~(ASRC_ASRCNCR_ANCA_MASK | ASRC_ASRCNCR_ANCB_MASK |
        ASRC_ASRCNCR_ANCC_MASK);

    regVal |= (uint32_t)(io_info_ptr->INIT.PAIR_A_CHANNELS
        << ASRC_ASRCNCR_ANCA_SHIFT);

    regVal |= (uint32_t)(io_info_ptr->INIT.PAIR_B_CHANNELS
        << ASRC_ASRCNCR_ANCB_SHIFT);

    regVal |= (uint32_t)(io_info_ptr->INIT.PAIR_C_CHANNELS
        << ASRC_ASRCNCR_ANCC_SHIFT);

    asrc_regs->ASRCNCR = regVal;

    return 0;

}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_deinit
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function deinitilizates the ASRC device, disable the hw
*
*END*********************************************************************/
int32_t vybrid_asrc_deinit
(
    /* [IN] the pointer to asrc info structure for this device*/
    VYBRID_ASRC_INFO_STRUCT_PTR io_info_ptr
)
{
    /*stop asrc*/
    uint32_t regVal = 0;
    ASRC_MemMapPtr asrc_regs;

    if (NULL == io_info_ptr)
        return -1;

    asrc_regs = io_info_ptr->asrc_regs;
    regVal = 0;
    asrc_regs->ASRCTR = regVal;

    return 0;

}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_pair_init
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function initilizates one asrc pair
*
*END*********************************************************************/
int32_t vybrid_asrc_pair_init
(
    /* [IN] the pointer to asrc info structure for this device*/
    VYBRID_ASRC_INFO_STRUCT_PTR io_info_ptr,

    /* [OUT] the pointer to asrc set node structure for this pair, return to caller*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR *asrc_set_node_pptr,

    /* [IN] current ASRC pair id*/
    uint8_t pair_id,

    /* [IN] open mode for this ASRC pair*/
    uint8_t mode
)
{
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node_ptr;

    *asrc_set_node_pptr = NULL;

    if (NULL == io_info_ptr || NULL == asrc_set_node_pptr)
        return -1;

    if (ASRC_PAIRS_NUM <= pair_id)
        return -3;


    /*allocate a asrc set node*/
    asrc_set_node_ptr =
        (VYBRID_ASRC_SET_NODE_STRUCT_PTR)_mem_alloc_system_zero(
        (uint32_t)sizeof (VYBRID_ASRC_SET_NODE_STRUCT));
    if (NULL == asrc_set_node_ptr) {
        return -4;
    }

    _mem_set_type (asrc_set_node_ptr, MEM_TYPE_IO_ASRC_SET_NODE_STRUCT);

    asrc_set_node_ptr->state = ASRC_INIT;
    asrc_set_node_ptr->mode = mode;
    asrc_set_node_ptr->asrc_pair_id = pair_id;
    asrc_set_node_ptr->pair_channels = 2;

    _mutex_init(&asrc_set_node_ptr->access_mutex, NULL);

    asrc_set_node_ptr->input_dma_channel = NULL;
    asrc_set_node_ptr->output_dma_channel = NULL;
    asrc_set_node_ptr->asrc_info_ptr = io_info_ptr;

    *asrc_set_node_pptr = asrc_set_node_ptr;

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : vybrid_asrc_pair_deinit
* Returned Value   : 0 success, < 0 failure
* Comments         :
*    This function deinitilizates one asrc pair
*
*END*********************************************************************/
int32_t vybrid_asrc_pair_deinit
(
    /* [IN] the pointer to asrc set node structure for this pair*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR asrc_set_node
)
{
    uint32_t ret;

    vybrid_asrc_stop(asrc_set_node);

    vybrid_asrc_uninstall_source(asrc_set_node);
    vybrid_asrc_uninstall_dest(asrc_set_node);

    ASRC_VYBRID_INTERNAL_DEBUG("destroy access_mutex 0x%x\n",
        &asrc_set_node->access_mutex);
    ret = _mutex_destroy(&asrc_set_node->access_mutex);
    if (MQX_EOK != ret) {
        ASRC_VYBRID_INTERNAL_ERROR("Fail to destroy access_mutex, ret %d\n",
            ret);
    }

    _mem_free(asrc_set_node);
    return 0;
}

