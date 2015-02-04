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
*   This include file implements the registers level operations for ESAI peripheral
*
*
*END************************************************************************/
#include "mqx_inc.h"
#include "esai_fifo_hw.h"
#include "fio.h"

#define ENABLE_ESAI_TEST 0

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_init
* Returned Value   : void
* Comments         :
*    initilizate the ESAI hardware
*
*END*********************************************************************/
void esai_hw_init
(
    ESAI_MemMapPtr esai_hw
)
{

    /*enable ESAI core logic block*/
    esai_hw->ECR = ESAI_ECR_ESAIEN_MASK;

    /*reset esai core logic block*/
    esai_hw->ECR |= ESAI_ECR_ERST_MASK;
    esai_hw->ECR &= ~ESAI_ECR_ERST_MASK;

    /*disable the ESAI FIFO*/
    esai_hw->TFCR &= ~ESAI_TFCR_TFE_MASK;
    esai_hw->RFCR &= ~ESAI_RFCR_RFE_MASK;

    /*reset ESAI FIFO*/
    esai_hw->TFCR |= ESAI_TFCR_TFR_MASK;
    esai_hw->RFCR |= ESAI_RFCR_RFR_MASK;

    /*reset out ESAI FIFO*/
    esai_hw->TFCR &= ~ESAI_TFCR_TFR_MASK;
    esai_hw->RFCR &= ~ESAI_RFCR_RFR_MASK;

    esai_hw->PCRC = 0xFFF;
    esai_hw->PRRC = 0xFFF;

    /*put transmit to personal reset*/
    esai_hw->TCR |= ESAI_TCR_TPR_MASK;

    /*put receive to personal reset*/
    esai_hw->RCR |= ESAI_RCR_RPR_MASK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_enable_syn
* Returned Value   : void
* Comments         :
*    Configure ESAI to synchronous mode
*
*END*********************************************************************/
void esai_enable_syn
(
    ESAI_MemMapPtr esai_hw
)
{
    esai_hw->SAICR |= ESAI_SAICR_SYN_MASK;
}

#if ENABLE_ESAI_TEST
/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_test
* Returned Value   : void
* Comments         :
*    test program for the ESAI hardware
*
*END*********************************************************************/
void esai_hw_test
(
    ESAI_MemMapPtr esai_hw
)
{
    /*1111 1100 1100 0011 0000 0000*/
    uint32_t tccr_i2s = 0xFCC300;
    /*0001 0011 0111 1101 0000 0000*/ /*32bit slot, 24 word */
    uint32_t tcr_i2s = 0x137D00;
    /*1111 0100 1100 0011 0000 0000*/
    uint32_t tccr_left = 0xF4C300;
    /*0001 0010 0111 1101 0000 0000*/
    uint32_t tcr_left = 0x127D00;

    esai_hw->ECR = 0x2;

    /*enable ESAI core logic block*/
    esai_hw->ECR = 0x80001;

    //esai_hw->PCRC = 0xFFF;
    //esai_hw->PRRC = 0xFFF;
    //while(1){}

    esai_hw->TCR |= ESAI_TCR_TPR_MASK;
    esai_hw->RCR |= ESAI_RCR_RPR_MASK;
    esai_hw->TFCR |= ESAI_TFCR_TFR_MASK;

    printf("1\n");

    //esai_hw->PCRC = 0x0;
    //esai_hw->PRRC = 0x0;

    esai_hw->SAICR = 0;

    //esai_hw->TCR = 0x4b4900;
    //esai_hw->TCR = 0x434900;

    esai_hw->TCR = tcr_left;
    esai_hw->TCCR = tccr_left;

    esai_hw->TSMA = 0xFFFF;
    esai_hw->TSMB = 0xFFFF;


    printf("2\n");

    esai_hw->TFCR = 0x000A4005;

    esai_hw->ETDR = 0x00000000;
    //esai_hw->ETDR = 0x22222222;
    //esai_hw->ETDR = 0x33333333;
    //esai_hw->ETDR = 0x44444444;
    //esai_hw->ETDR = 0x55555555;
    //esai_hw->ETDR = 0x66660000;
    //esai_hw->ETDR = 0x00777777;


    printf("3\n");
    printf("ECR=0x%x\n", esai_hw->ECR);
    printf("ESR=0x%x\n", esai_hw->ESR);
    printf("TFCR=0x%x\n", esai_hw->TFCR);
    printf("TFSR=0x%x\n", esai_hw->TFSR);
    printf("RFCR=0x%x\n", esai_hw->RFCR);
    printf("RFSR=0x%x\n", esai_hw->RFSR);
    printf("TSR=0x%x\n", esai_hw->TSR);
    printf("SAISR=0x%x\n", esai_hw->SAISR);
    printf("SAICR=0x%x\n", esai_hw->SAICR);
    printf("TCR=0x%x\n", esai_hw->TCR);
    printf("TCCR=0x%x\n", esai_hw->TCCR);
    printf("RCR=0x%x\n", esai_hw->RCR);
    printf("RCCR=0x%x\n", esai_hw->RCCR);
    printf("TSMA=0x%x\n", esai_hw->TSMA);
    printf("TSMB=0x%x\n", esai_hw->TSMB);
    printf("PRRC=0x%x\n", esai_hw->PRRC);
    printf("PCRC=0x%x\n", esai_hw->PCRC);


    while ((esai_hw->TFSR & 0xff) < 0x7f)
    {
        printf("TFSR = 0x%x\n", esai_hw->TFSR);
        esai_hw->ETDR = 0;
    }


    printf("4\n");

    esai_hw->TCR = (tcr_left | 0x1);

    esai_hw->PCRC = 0xFFF;
    esai_hw->PRRC = 0xFFF;

    /*
    esai_hw->TX[0] = 0x11111111;
    esai_hw->TX[0] = 0x22222222;
    esai_hw->TX[0] = 0x33333333;
    esai_hw->TX[0] = 0x44444444;
    esai_hw->TX[0] = 0x55555555;
    esai_hw->TX[0] = 0x66660000;
    esai_hw->TX[0] = 0x00777777;
*/
    printf("5\n");
    printf("ECR=0x%x\n", esai_hw->ECR);
    printf("ESR=0x%x\n", esai_hw->ESR);
    printf("TFCR=0x%x\n", esai_hw->TFCR);
    printf("TFSR=0x%x\n", esai_hw->TFSR);
    printf("RFCR=0x%x\n", esai_hw->RFCR);
    printf("RFSR=0x%x\n", esai_hw->RFSR);
    printf("TSR=0x%x\n", esai_hw->TSR);
    printf("Tx[0]=0x%x\n", esai_hw->TX[0]);
    printf("Tx[1]=0x%x\n", esai_hw->TX[1]);
    printf("Tx[2]=0x%x\n", esai_hw->TX[2]);
    printf("Tx[3]=0x%x\n", esai_hw->TX[3]);
    printf("Tx[4]=0x%x\n", esai_hw->TX[4]);
    printf("Tx[5]=0x%x\n", esai_hw->TX[5]);
    printf("SAISR=0x%x\n", esai_hw->SAISR);
    printf("SAICR=0x%x\n", esai_hw->SAICR);
    printf("TCR=0x%x\n", esai_hw->TCR);
    printf("TCCR=0x%x\n", esai_hw->TCCR);
    printf("RCR=0x%x\n", esai_hw->RCR);
    printf("RCCR=0x%x\n", esai_hw->RCCR);
    printf("TSMA=0x%x\n", esai_hw->TSMA);
    printf("TSMB=0x%x\n", esai_hw->TSMB);
    printf("PRRC=0x%x\n", esai_hw->PRRC);
    printf("PCRC=0x%x\n", esai_hw->PCRC);

    while (1)
    {
        while (!(esai_hw->ESR & ESAI_ESR_TFE_MASK))
        {
            _time_delay(1000);
            printf("TFSR = 0x%x, SAISR = 0x%x\n",
                esai_hw->TFSR, esai_hw->SAISR);
        }

        esai_hw->ETDR = 0x00;
        printf("send\n");
    }
}
#endif

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_reset
* Returned Value   : void
* Comments         :
*    reset the transmit section of ESAI
*
*END*********************************************************************/
void esai_hw_tx_reset
(
    ESAI_MemMapPtr esai_hw
)
{
    uint32_t tfcr_val = 0;
    /*put transmit to personal reset*/

    tfcr_val = esai_hw->TFCR;

    /*disable transmit FIFO*/
    tfcr_val &= ~ESAI_TFCR_TFE_MASK;

    tfcr_val &= ~(ESAI_TFCR_TE0_MASK|ESAI_TFCR_TE1_MASK|ESAI_TFCR_TE2_MASK
        |ESAI_TFCR_TE3_MASK|ESAI_TFCR_TE4_MASK|ESAI_TFCR_TE5_MASK);

    /*reset transmit FIFO */
    tfcr_val |= ESAI_TFCR_TFR_MASK;

    esai_hw->TFCR = tfcr_val;

    tfcr_val &= ~ESAI_TFCR_TFR_MASK;

    esai_hw->TFCR = tfcr_val;

    esai_hw->TCR &= ~(ESAI_TCR_TE0_MASK|ESAI_TCR_TE1_MASK|ESAI_TCR_TE2_MASK
        |ESAI_TCR_TE3_MASK|ESAI_TCR_TE4_MASK|ESAI_TCR_TE5_MASK);
    esai_hw->TCR |= ESAI_TCR_TPR_MASK;

}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_out_of_reset
* Returned Value   : void
* Comments         :
*    put the transmit section of ESAI out of reset state
*
*END*********************************************************************/
void esai_hw_tx_out_of_reset
(
    ESAI_MemMapPtr esai_hw
)
{
    esai_hw->TCR &= ~ESAI_TCR_TPR_MASK;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_fifo_enable
* Returned Value   : void
* Comments         :
*    Enable the Transmit FIFO
*
*END*********************************************************************/
void esai_hw_tx_fifo_enable
(
    ESAI_MemMapPtr esai_hw,
    uint8_t enabled_tx,
    uint8_t watermark,
    uint32_t fifo_wa
)
{
    uint8_t i = 0;
    uint32_t tfcr_val = 0;


    tfcr_val = esai_hw->TFCR;

    tfcr_val &= ~ESAI_TFCR_TWA_MASK;
    tfcr_val |= (fifo_wa & ESAI_TFCR_TWA_MASK);

    /*enable TEs in transmit FIFO*/
    tfcr_val &= ~(ESAI_TFCR_TE0_MASK|ESAI_TFCR_TE1_MASK|ESAI_TFCR_TE2_MASK
        |ESAI_TFCR_TE3_MASK|ESAI_TFCR_TE4_MASK|ESAI_TFCR_TE5_MASK);

    tfcr_val |= ((enabled_tx << ESAI_TFCR_TE0_SHIFT) &
        (ESAI_TFCR_TE0_MASK|ESAI_TFCR_TE1_MASK|ESAI_TFCR_TE2_MASK
        |ESAI_TFCR_TE3_MASK|ESAI_TFCR_TE4_MASK|ESAI_TFCR_TE5_MASK));

    /*set fifo watermark*/
    tfcr_val &= ~ESAI_TFCR_TFWM_MASK;
    tfcr_val |= ((watermark << ESAI_TFCR_TFWM_SHIFT) & ESAI_TFCR_TFWM_MASK);

    tfcr_val |= ESAI_TFCR_TIEN_MASK;

    esai_hw->TFCR = tfcr_val;

    esai_hw->TFCR |= ESAI_TFCR_TFE_MASK;

    /*init tx data register*/
    for (i=0; i<6; i++)
    {
        if (enabled_tx & (1<<i))
        {
            esai_hw->ETDR = 0x0;
            esai_hw->ETDR = 0x0;
        }
    }

}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_enable
* Returned Value   : void
* Comments         :
*    Enable the Transmit to send out data.
*
*END*********************************************************************/
void esai_hw_tx_enable
(
    ESAI_MemMapPtr esai_hw,
    uint8_t enabled_tx
)
{
    uint32_t tcr_val = 0;

    tcr_val = esai_hw->TCR;
    tcr_val &= ~(ESAI_TCR_TE0_MASK|ESAI_TCR_TE1_MASK|ESAI_TCR_TE2_MASK
        |ESAI_TCR_TE3_MASK|ESAI_TCR_TE4_MASK|ESAI_TCR_TE5_MASK);
    tcr_val |= ((enabled_tx << ESAI_TCR_TE0_SHIFT) &
        (ESAI_TCR_TE0_MASK|ESAI_TCR_TE1_MASK|ESAI_TCR_TE2_MASK
        |ESAI_TCR_TE3_MASK|ESAI_TCR_TE4_MASK|ESAI_TCR_TE5_MASK));
    esai_hw->TCR = tcr_val;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_control_setting
* Returned Value   : void
* Comments         :
*    Apply the transmit control setting to ESAI hardware
*
*END*********************************************************************/
void esai_hw_tx_control_setting
(
    ESAI_MemMapPtr esai_hw,
    ESAI_HW_CR_SETTING_STRUCT_PTR hw_setting
)
{
    uint32_t tcr_val = 0;

    tcr_val = esai_hw->TCR;

    if (hw_setting->fsync_slot_length)
        tcr_val &= ~ESAI_TCR_TFSL_MASK;
    else
        tcr_val |= ESAI_TCR_TFSL_MASK;

    if (hw_setting->fsync_early)
        tcr_val |= ESAI_TCR_TFSR_MASK;
    else
        tcr_val &= ~ESAI_TCR_TFSR_MASK;

    if (hw_setting->left_align)
        tcr_val &= ~ESAI_TCR_TWA_MASK;
    else
        tcr_val |= ESAI_TCR_TWA_MASK;

    if (hw_setting->zero_pad)
        tcr_val |= ESAI_TCR_PADC_MASK;
    else
        tcr_val &= ~ESAI_TCR_PADC_MASK;

    if (hw_setting->msb_shift)
        tcr_val &= ~ESAI_TCR_TSHFD_MASK;
    else
        tcr_val |= ESAI_TCR_TSHFD_MASK;

    tcr_val &= ~ESAI_TCR_TSWS_MASK;
    tcr_val |= (hw_setting->slot_data_width & ESAI_TCR_TSWS_MASK);

    tcr_val &= ~ESAI_TCR_TMOD_MASK;
    tcr_val |= (hw_setting->mode & ESAI_TCR_TMOD_MASK);

    esai_hw->TCR = tcr_val;

    /*apply slot mask*/
    if (hw_setting->slots_num > 16) {
        esai_hw->TSMA = (hw_setting->slots_mask & 0xFFFF);
        esai_hw->TSMB = ((hw_setting->slots_mask >> 16) & 0xFFFF);
    } else {
        esai_hw->TSMA = (hw_setting->slots_mask & 0xFFFF);
        esai_hw->TSMB = 0;
    }

}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_clock_setting
* Returned Value   : void
* Comments         :
*    Apply the transmit clock settting to ESAI hardware
*
*END*********************************************************************/
void esai_hw_tx_clock_setting
(
    ESAI_MemMapPtr esai_hw,
    ESAI_HW_CLK_SETTING_STRUCT_PTR hw_setting
)
{
    uint32_t tccr_val = 0;

    tccr_val = esai_hw->TCCR;


    if (hw_setting->sck_dir)
        tccr_val |= ESAI_TCCR_TCKD_MASK;
    else
        tccr_val &= ~ESAI_TCCR_TCKD_MASK;

    if (hw_setting->fst_dir)
        tccr_val |= ESAI_TCCR_TFSD_MASK;
    else
        tccr_val &= ~ESAI_TCCR_TFSD_MASK;

    if (hw_setting->hck_dir)
        tccr_val |= ESAI_TCCR_THCKD_MASK;
    else
        tccr_val &= ~ESAI_TCCR_THCKD_MASK;


    if (hw_setting->sclk_polar)
        tccr_val |= (ESAI_TCCR_TCKP_MASK | ESAI_TCCR_THCKP_MASK);
    else
        tccr_val &= ~(ESAI_TCCR_TCKP_MASK | ESAI_TCCR_THCKP_MASK);

    if (hw_setting->sync_polar)
        tccr_val |= ESAI_TCCR_TFSP_MASK;
    else
        tccr_val &= ~ESAI_TCCR_TFSP_MASK;


    tccr_val &= ~ESAI_TCCR_TDC_MASK;
    tccr_val |= ((hw_setting->dc_val << ESAI_TCCR_TDC_SHIFT)
        & ESAI_TCCR_TDC_MASK);


    if (hw_setting->source_extal) {

        esai_hw->ECR |= (ESAI_ECR_ETO_MASK | ESAI_ECR_ETI_MASK);

    } else {

        esai_hw->ECR &= ~(ESAI_ECR_ETO_MASK | ESAI_ECR_ETI_MASK);
    }

    tccr_val &= ~ESAI_TCCR_TPM_MASK;
    tccr_val |= ((hw_setting->pm_val << ESAI_TCCR_TPM_SHIFT)
        & ESAI_TCCR_TPM_MASK);

    tccr_val &= ~ESAI_TCCR_TFP_MASK;
    tccr_val |= ((hw_setting->fp_val << ESAI_TCCR_TFP_SHIFT)
        & ESAI_TCCR_TFP_MASK);

    if (hw_setting->psr_val)
        tccr_val |= ESAI_TCCR_TPSR_MASK;
    else
        tccr_val &= ~ESAI_TCCR_TPSR_MASK;

    esai_hw->TCCR = tccr_val;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_fifo_write
* Returned Value   : void
* Comments         :
*    Write a word to Transmit FIFO
*
*END*********************************************************************/
void esai_hw_tx_fifo_write
(
    ESAI_MemMapPtr esai_hw,
    uint32_t data
)
{
    while (!(esai_hw->ESR & ESAI_ESR_TFE_MASK))
    {}

    esai_hw->ETDR = data;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_fifo_empty
* Returned Value   : bool
* Comments         :
*    Check if the transmit fifo is empty
*
*END*********************************************************************/
bool esai_hw_tx_fifo_empty
(
    ESAI_MemMapPtr esai_hw
)
{
    if (0 == (esai_hw->TFSR & ESAI_TFSR_TFCNT_MASK)) {
        return TRUE;
    }

    return FALSE;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_fifo_underrun
* Returned Value   : bool
* Comments         :
*    Check if the transmit fifo is empty
*
*END*********************************************************************/
bool esai_hw_tx_fifo_underrun
(
    ESAI_MemMapPtr esai_hw
)
{
    if (0 != (esai_hw->SAISR & ESAI_SAISR_TUE_MASK)) {
        return TRUE;
    }

    return FALSE;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_tx_enable_interrupts
* Returned Value   : bool
* Comments         :
*    Enable transimit interrupts
*
*END*********************************************************************/
void esai_hw_tx_enable_interrupts
(
    ESAI_MemMapPtr esai_hw,
    bool enable
)
{
    if (enable) {
        esai_hw->TCR |= ESAI_TCR_TEIE_MASK;

    } else {
        esai_hw->TCR &= ~ESAI_TCR_TEIE_MASK;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_reset
* Returned Value   : void
* Comments         :
*    reset the receive section of ESAI
*
*END*********************************************************************/
void esai_hw_rx_reset
(
    ESAI_MemMapPtr esai_hw
)
{
    uint32_t rfcr_val = 0;

    /*put receiver to personal reset*/

    rfcr_val = esai_hw->RFCR;

    /*disable receive FIFO*/
    rfcr_val &= ~ESAI_RFCR_RFE_MASK;

    rfcr_val &= ~(ESAI_RFCR_RE0_MASK | ESAI_RFCR_RE1_MASK | ESAI_RFCR_RE2_MASK
        | ESAI_RFCR_RE3_MASK);

    /*reset receive FIFO */
    rfcr_val |= ESAI_RFCR_RFR_MASK;

    esai_hw->RFCR = rfcr_val;

    rfcr_val &= ~ESAI_RFCR_RFR_MASK;

    esai_hw->RFCR = rfcr_val;

    esai_hw->RCR &= ~(ESAI_RCR_RE0_MASK | ESAI_RCR_RE1_MASK
        | ESAI_RCR_RE2_MASK | ESAI_RCR_RE3_MASK);
    esai_hw->RCR |= ESAI_RCR_RPR_MASK;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_out_of_reset
* Returned Value   : void
* Comments         :
*    put the receiver section of ESAI out of reset state
*
*END*********************************************************************/
void esai_hw_rx_out_of_reset
(
    ESAI_MemMapPtr esai_hw
)
{
    esai_hw->RCR &= ~ESAI_RCR_RPR_MASK;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_fifo_enable
* Returned Value   : void
* Comments         :
*    Enable the Receiver FIFO
*
*END*********************************************************************/
void esai_hw_rx_fifo_enable
(
    ESAI_MemMapPtr esai_hw,
    uint8_t enabled_rx,
    uint8_t watermark,
    uint32_t fifo_wa
)
{
    uint32_t rfcr_val = 0;


    rfcr_val = esai_hw->RFCR;

    rfcr_val &= ~ESAI_RFCR_RWA_MASK;
    rfcr_val |= (fifo_wa & ESAI_RFCR_RWA_MASK);

    /*enable REs in receiver FIFO*/
    rfcr_val &= ~(ESAI_RFCR_RE0_MASK | ESAI_RFCR_RE1_MASK | ESAI_RFCR_RE2_MASK
        | ESAI_RFCR_RE3_MASK);
    rfcr_val |= ((enabled_rx << ESAI_RFCR_RE0_SHIFT) &
        (ESAI_RFCR_RE0_MASK | ESAI_RFCR_RE1_MASK | ESAI_RFCR_RE2_MASK
        | ESAI_RFCR_RE3_MASK));

    /*set fifo watermark*/
    rfcr_val &= ~ESAI_RFCR_RFWM_MASK;
    rfcr_val |= ((watermark << ESAI_RFCR_RFWM_SHIFT) & ESAI_RFCR_RFWM_MASK);

    rfcr_val |= ESAI_RFCR_REXT_MASK;

    esai_hw->RFCR = rfcr_val;

    esai_hw->RFCR |= ESAI_RFCR_RFE_MASK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_enable
* Returned Value   : void
* Comments         :
*    Enable the Receiver to send out data.
*
*END*********************************************************************/
void esai_hw_rx_enable
(
    ESAI_MemMapPtr esai_hw,
    uint8_t enabled_rx
)
{
    uint32_t rcr_val = 0;

    rcr_val = esai_hw->RCR;
    rcr_val &= ~(ESAI_RCR_RE0_MASK | ESAI_RCR_RE1_MASK
        | ESAI_RCR_RE2_MASK | ESAI_RCR_RE3_MASK);
    rcr_val |= ((enabled_rx << ESAI_RCR_RE0_SHIFT) &
        (ESAI_RCR_RE0_MASK | ESAI_RCR_RE1_MASK | ESAI_RCR_RE2_MASK
        | ESAI_RCR_RE3_MASK));
    esai_hw->RCR = rcr_val;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_control_setting
* Returned Value   : void
* Comments         :
*    Apply the receiver control setting to ESAI hardware
*
*END*********************************************************************/
void esai_hw_rx_control_setting
(
    ESAI_MemMapPtr esai_hw,
    ESAI_HW_CR_SETTING_STRUCT_PTR hw_setting
)
{
    uint32_t rcr_val = 0;

    rcr_val = esai_hw->RCR;

    if (hw_setting->fsync_slot_length)
        rcr_val &= ~ESAI_RCR_RFSL_MASK;
    else
        rcr_val |= ESAI_RCR_RFSL_MASK;

    if (hw_setting->fsync_early)
        rcr_val |= ESAI_RCR_RFSR_MASK;
    else
        rcr_val &= ~ESAI_RCR_RFSR_MASK;

    if (hw_setting->left_align)
        rcr_val &= ~ESAI_RCR_RWA_MASK;
    else
        rcr_val |= ESAI_RCR_RWA_MASK;

    if (hw_setting->msb_shift)
        rcr_val &= ~ESAI_RCR_RSHFD_MASK;
    else
        rcr_val |= ESAI_RCR_RSHFD_MASK;

    rcr_val &= ~ESAI_RCR_RSWS_MASK;
    rcr_val |= (hw_setting->slot_data_width & ESAI_RCR_RSWS_MASK);

    rcr_val &= ~ESAI_RCR_RMOD_MASK;
    rcr_val |= (hw_setting->mode & ESAI_RCR_RMOD_MASK);

    esai_hw->RCR = rcr_val;

    /*apply slot mask*/
    if (hw_setting->slots_num > 16) {
        esai_hw->RSMA = (hw_setting->slots_mask & 0xFFFF);
        esai_hw->RSMB = ((hw_setting->slots_mask >> 16) & 0xFFFF);
    } else {
        esai_hw->RSMA = (hw_setting->slots_mask & 0xFFFF);
        esai_hw->RSMB = 0;
    }
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_clock_setting
* Returned Value   : void
* Comments         :
*    Apply the receiver clock settting to ESAI hardware
*
*END*********************************************************************/
void esai_hw_rx_clock_setting
(
    ESAI_MemMapPtr esai_hw,
    ESAI_HW_CLK_SETTING_STRUCT_PTR hw_setting
)
{
    uint32_t rccr_val = 0;

    rccr_val = esai_hw->RCCR;

    if (hw_setting->sck_dir)
        rccr_val |= ESAI_RCCR_RCKD_MASK;
    else
        rccr_val &= ~ESAI_RCCR_RCKD_MASK;

    if (hw_setting->fst_dir)
        rccr_val |= ESAI_RCCR_RFSD_MASK;
    else
        rccr_val &= ~ESAI_RCCR_RFSD_MASK;

    if (hw_setting->hck_dir)
        rccr_val |= ESAI_RCCR_RHCKD_MASK;
    else
        rccr_val &= ~ESAI_RCCR_RHCKD_MASK;

    if (hw_setting->sclk_polar)
        rccr_val |= (ESAI_RCCR_RCKP_MASK | ESAI_RCCR_RHCKP_MASK);
    else
        rccr_val &= ~(ESAI_RCCR_RCKP_MASK | ESAI_RCCR_RHCKP_MASK);

    if (hw_setting->sync_polar)
        rccr_val |= ESAI_RCCR_RFSP_MASK;
    else
        rccr_val &= ~ESAI_RCCR_RFSP_MASK;


    rccr_val &= ~ESAI_RCCR_RDC_MASK;
    rccr_val |= ((hw_setting->dc_val << ESAI_RCCR_RDC_SHIFT)
        & ESAI_RCCR_RDC_MASK);


    if (hw_setting->source_extal) {
        esai_hw->ECR |= (ESAI_ECR_ERO_MASK | ESAI_ECR_ERI_MASK);
    } else {
        esai_hw->ECR &= ~(ESAI_ECR_ERO_MASK | ESAI_ECR_ERI_MASK);
    }

    rccr_val &= ~ESAI_RCCR_RPM_MASK;
    rccr_val |= ((hw_setting->pm_val << ESAI_RCCR_RPM_SHIFT)
        & ESAI_RCCR_RPM_MASK);

    rccr_val &= ~ESAI_RCCR_RFP_MASK;
    rccr_val |= ((hw_setting->fp_val << ESAI_RCCR_RFP_SHIFT)
        & ESAI_RCCR_RFP_MASK);

    if (hw_setting->psr_val)
        rccr_val |= ESAI_RCCR_RPSP_MASK;
    else
        rccr_val &= ~ESAI_RCCR_RPSP_MASK;

    esai_hw->RCCR = rccr_val;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_fifo_write
* Returned Value   : void
* Comments         :
*    Read a word from Receiver FIFO
*
*END*********************************************************************/
void esai_hw_rx_fifo_read
(
    ESAI_MemMapPtr esai_hw,
    uint32_t *data
)
{
    while (!(esai_hw->ESR & ESAI_ESR_RFF_MASK))
    {}

    *data = esai_hw->ERDR;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_fifo_full
* Returned Value   : bool
* Comments         :
*    Check if the receiver fifo is full
*
*END*********************************************************************/
bool esai_hw_rx_fifo_full
(
    ESAI_MemMapPtr esai_hw
)
{
    if ((esai_hw->RFSR & ESAI_RFSR_RFCNT_MASK) >= 127) {
        return TRUE;
    }

    return FALSE;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_fifo_overrun
* Returned Value   : bool
* Comments         :
*    Check if the receiver fifo is full
*
*END*********************************************************************/
bool esai_hw_rx_fifo_overrun
(
    ESAI_MemMapPtr esai_hw
)
{
    if (0 != (esai_hw->SAISR & ESAI_SAISR_ROE_MASK)) {
        return TRUE;
    }

    return FALSE;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_rx_enable_interrupts
* Returned Value   : void
* Comments         :
*    Enable receive interrupts
*
*END*********************************************************************/
void esai_hw_rx_enable_interrupts
(
    ESAI_MemMapPtr esai_hw,
    bool enable
)
{
    if (enable) {
        esai_hw->RCR |= ESAI_RCR_REIE_MASK;

    } else {
        esai_hw->RCR &= ~ESAI_RCR_REIE_MASK;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_hw_esr_reg_val
* Returned Value   : register value
* Comments         :
*
*
*END*********************************************************************/
uint32_t esai_hw_esr_reg_val
(
    ESAI_MemMapPtr esai_hw
)
{
    return esai_hw->ESR;
}


/* EOF */
