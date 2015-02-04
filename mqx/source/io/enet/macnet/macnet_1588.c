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
*   This file contains IEEE 1588 interface functions of the MACNET driver.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "enet.h"
#include "enetprv.h"
#include "macnet_prv.h"
#include "macnet_1588.h"
#include <string.h>

#if ENETCFG_SUPPORT_PTP

/* Global variables */
uint64_t  MACNET_PTP_seconds = 0;
bool  MACNET_PTP_set_rtc_time_flag = FALSE;

static ENET_MemMapPtr MACNET_PTP_master_addr;

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_swap8bytes
*  Returned Value : uint64_t
*  Comments       : Swap 8 bytes
*
*END*-----------------------------------------------------------------*/
static uint64_t MACNET_ptp_swap8bytes(uint64_t n)
{
	unsigned char temp[8];

    temp[7] = *((unsigned char *)&n);
    temp[6] = ((unsigned char *)&n)[1];
    temp[5] = ((unsigned char *)&n)[2];
    temp[4] = ((unsigned char *)&n)[3];
    temp[3] = ((unsigned char *)&n)[4];
    temp[2] = ((unsigned char *)&n)[5];
    temp[1] = ((unsigned char *)&n)[6];
    temp[0] = ((unsigned char *)&n)[7];
    return (*(uint64_t *)temp);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_init_circ
*  Returned Value :
*  Comments       : Alloc the ring resource
*
*
*END*-----------------------------------------------------------------*/
static bool MACNET_ptp_init_circ(MACNET_PTP_CIRCULAR *buf)
{
	buf->DATA_BUF = (MACNET_PTP_DATA *) _mem_alloc_system_zero((MACNET_PTP_DEFAULT_RX_BUF_SZ+1) *	sizeof(MACNET_PTP_DATA));

	if (!buf->DATA_BUF)
		return FALSE;
	buf->FRONT = 0;
	buf->END = 0;
	buf->SIZE = (MACNET_PTP_DEFAULT_RX_BUF_SZ + 1);

	return TRUE;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_calc_index
*  Returned Value :
*  Comments       : Calculate index of the element
*
*
*END*-----------------------------------------------------------------*/
static inline uint32_t MACNET_ptp_calc_index(uint32_t size, uint32_t curr_index, uint32_t offset)
{
	return ((curr_index + offset) % size);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_is_empty
*  Returned Value :
*  Comments       : Return TRUE if the buffer is empty
*
*
*END*-----------------------------------------------------------------*/
static bool MACNET_ptp_is_empty(MACNET_PTP_CIRCULAR *buf)
{
	return (buf->FRONT == buf->END);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_nelems
*  Returned Value :
*  Comments       : Calculates the number of elements in the ring buffer
*
*
*END*-----------------------------------------------------------------*/
static uint32_t MACNET_ptp_nelems(MACNET_PTP_CIRCULAR *buf)
{
	const uint32_t FRONT = buf->FRONT;
	const uint32_t END = buf->END;
	const uint32_t SIZE = buf->SIZE;
	uint32_t n_items;

	if (END > FRONT)
		n_items = END - FRONT;
	else if (END < FRONT)
		n_items = SIZE - (FRONT - END);
	else
		n_items = 0;

	return n_items;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_is_full
*  Returned Value :
*  Comments       : Returns TRUE if the buffer is full
*
*
*END*-----------------------------------------------------------------*/
static bool MACNET_ptp_is_full(MACNET_PTP_CIRCULAR *buf)
{
	if (MACNET_ptp_nelems(buf) == (buf->SIZE - 1))
		return TRUE;
	else
		return FALSE;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_insert
*  Returned Value :
*  Comments       : Inserts new element into the ring buffer
*
*
*END*-----------------------------------------------------------------*/
bool MACNET_ptp_insert(MACNET_PTP_CIRCULAR *buf,
				          MACNET_PTP_DATA *data)
{
	MACNET_PTP_DATA *tmp;
	uint32_t         index;

	if (MACNET_ptp_is_full(buf))
		return FALSE;

	tmp = (buf->DATA_BUF + buf->END);

	tmp->KEY = data->KEY;
	tmp->TS_TIME.SEC = data->TS_TIME.SEC;
	tmp->TS_TIME.NSEC = data->TS_TIME.NSEC;
	for(index=0;index<MACNET_PTP_CLOCKID_SIZE;index++)
        tmp->CLOCKID[index] = data->CLOCKID[index];

	buf->END = MACNET_ptp_calc_index(buf->SIZE, buf->END, 1);

	return TRUE;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_find_and_remove
*  Returned Value :
*  Comments       : Based on sequence ID and the clock ID
*                   find the element in the ring buffer and remove it
*
*END*-----------------------------------------------------------------*/
static int32_t MACNET_ptp_find_and_remove(MACNET_PTP_CIRCULAR *buf,
			                             uint32_t key,
			                             uint8_t* clkid,
			                             MACNET_PTP_DATA *data)
{
	uint32_t i;
	uint32_t SIZE = buf->SIZE, END = buf->END;

	if (MACNET_ptp_is_empty(buf))
		return ENET_ERROR;

	i = buf->FRONT;
	while (i != END) {
		if (((buf->DATA_BUF + i)->KEY == key) && (0==(memcmp(((const void*)&(buf->DATA_BUF + i)->CLOCKID[0]), (const void*)clkid, MACNET_PTP_CLOCKID_SIZE))))
			break;
		i = MACNET_ptp_calc_index(SIZE, i, 1);
	}

	if (i == END) {
		buf->FRONT = buf->END;
		return ENET_ERROR;
	}

	data->TS_TIME.SEC = (buf->DATA_BUF + i)->TS_TIME.SEC;
	data->TS_TIME.NSEC = (buf->DATA_BUF + i)->TS_TIME.NSEC;

	buf->FRONT = MACNET_ptp_calc_index(SIZE, i, 1);

	return ENET_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_init
*  Returned Value :
*  Comments       : Initialize required ring buffers
*
*
*END*-----------------------------------------------------------------*/
uint32_t MACNET_ptp_init(ENET_CONTEXT_STRUCT_PTR enet_ptr)
{
	MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
	MACNET_PTP_PRIVATE         *priv = (MACNET_PTP_PRIVATE_PTR)macnet_context_ptr->PTP_PRIV;

    if(!(enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PTP_INBAND)) {
        MACNET_ptp_init_circ(&(priv->RX_TIME_SYNC));
        MACNET_ptp_init_circ(&(priv->RX_TIME_DEL_REQ));
        MACNET_ptp_init_circ(&(priv->RX_TIME_PDEL_REQ));
        MACNET_ptp_init_circ(&(priv->RX_TIME_PDEL_RESP));
    }
    MACNET_ptp_init_circ(&(priv->TX_TIME));
    return 0;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_cleanup
*  Returned Value :
*  Comments       : Free all ring buffers
*
*
*END*-----------------------------------------------------------------*/
void MACNET_ptp_cleanup(ENET_CONTEXT_STRUCT_PTR enet_ptr)
{
	MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
	MACNET_PTP_PRIVATE         *priv = (MACNET_PTP_PRIVATE_PTR)macnet_context_ptr->PTP_PRIV;

    if(!(enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PTP_INBAND)) {
    	if (priv->RX_TIME_SYNC.DATA_BUF)
    		_mem_free((void *)priv->RX_TIME_SYNC.DATA_BUF);
    	if (priv->RX_TIME_DEL_REQ.DATA_BUF)
    		_mem_free((void *)priv->RX_TIME_DEL_REQ.DATA_BUF);
    	if (priv->RX_TIME_PDEL_REQ.DATA_BUF)
    		_mem_free((void *)priv->RX_TIME_PDEL_REQ.DATA_BUF);
    	if (priv->RX_TIME_PDEL_RESP.DATA_BUF)
    		_mem_free((void *)priv->RX_TIME_PDEL_RESP.DATA_BUF);
    }
    if (priv->TX_TIME.DATA_BUF)
    	_mem_free((void *)priv->TX_TIME.DATA_BUF);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_get_master_base_address
*  Returned Value :
*  Comments         :
*    This function returns pointer to base address of the MACNET device
*    in PTP master mode.
*
*END*-----------------------------------------------------------------*/
ENET_MemMapPtr MACNET_ptp_get_master_base_address(void)
{
    return MACNET_PTP_master_addr;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_set_master_base_address
*  Returned Value :
*  Comments         :
*    This function sets pointer to base address of the MACNET device
*    in PTP master mode.
*
*END*-----------------------------------------------------------------*/
void MACNET_ptp_set_master_base_address(ENET_MemMapPtr addr)
{
    MACNET_PTP_master_addr = addr;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_start
*  Returned Value :
*  Comments       : 1588 Module initialization
*
*
*END*-----------------------------------------------------------------*/
uint32_t MACNET_ptp_start(MACNET_PTP_PRIVATE *priv, bool master_mode)
{
	MACNET_PTP_PRIVATE *fpp = priv;

#if defined(BSP_TWR_K60N512) || defined(BSP_TWR_K60D100M)
    /* Select the 1588 timer source clock - EXTAL clock */
    OSC_CR |= OSC_CR_ERCLKEN_MASK;
    SIM_SOPT2 = ((SIM_SOPT2) & ~SIM_SOPT2_TIMESRC_MASK) | SIM_SOPT2_TIMESRC(0x2);
#endif

	/* Enable module for starting Tmr Clock */
    fpp->MACNET_PTR->ATCR = ENET_ATCR_RESTART_MASK;
    fpp->MACNET_PTR->ATINC = MACNET_1588_CLOCK_INC;
    fpp->MACNET_PTR->ATPER = MACNET_1588_ATPER_VALUE;

#if (MK60_REV_1_0 || MK60_REV_1_1 || MK60_REV_1_2)
    /* Workaround for e2579: ENET: No support for IEEE 1588, TS_TIMER, timestamp timer overflow interrupt */
    fpp->MACNET_PTR->CHANNEL[MACNET_PTP_TIMER].TCCR = MACNET_1588_ATPER_VALUE - MACNET_1588_CLOCK_INC;
    fpp->MACNET_PTR->CHANNEL[MACNET_PTP_TIMER].TCSR = (((0x5)<<ENET_TCSR_TMODE_SHIFT)&ENET_TCSR_TMODE_MASK) |
                                                      ENET_TCSR_TIE_MASK;
    fpp->MACNET_PTR->CHANNEL[MACNET_PTP_TIMER].TCCR = MACNET_1588_ATPER_VALUE - MACNET_1588_CLOCK_INC;
#endif

    /* Period PIN & EVT bits must be set for rollover detect */
    fpp->MACNET_PTR->ATCR = ENET_ATCR_PEREN_MASK | ENET_ATCR_PINPER_MASK;

    /* Set the slave mode in case the MACNET is not handling the 1588timer */
    if(!master_mode)
	    fpp->MACNET_PTR->ATCR |= ENET_ATCR_SLAVE_MASK;

    /* Start counter */
    fpp->MACNET_PTR->ATCR |= ENET_ATCR_EN_MASK;
    return 0;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_stop
*  Returned Value :
*  Comments       : Cleanup routine for 1588 module.
*                   When PTP is disabled this routing is called.
*
*END*-----------------------------------------------------------------*/
void MACNET_ptp_stop(ENET_MemMapPtr macnet_ptr)
{
	macnet_ptr->ATCR = 0;
	macnet_ptr->ATCR |= ENET_ATCR_RESTART_MASK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_get_curr_cnt
*  Returned Value :
*  Comments       : Gets current value of the precise 1588 counter
*
*
*END*-----------------------------------------------------------------*/
static void MACNET_ptp_get_curr_cnt(ENET_MemMapPtr macnet_ptr,
                                    MACNET_PTP_TIME *curr_time)
{
 	curr_time->SEC = MACNET_PTP_seconds;
	/* To read the current value, issue a capture command (set
       ENETn_ATCR[CAPTURE]) prior to reading this register */
	macnet_ptr->ATCR |= ENET_ATCR_CAPTURE_MASK;
	macnet_ptr->ATCR |= ENET_ATCR_CAPTURE_MASK;
	curr_time->NSEC = macnet_ptr->ATVR;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_set_1588cnt
*  Returned Value :
*  Comments       : Set the precise 1588 counter
*
*
*END*-----------------------------------------------------------------*/
static void MACNET_ptp_set_1588cnt(ENET_MemMapPtr macnet_ptr,
			                       MACNET_PTP_TIME *fec_time)
{
	MACNET_int_disable();
	MACNET_PTP_seconds = fec_time->SEC;
	macnet_ptr->ATVR = fec_time->NSEC;
	MACNET_PTP_set_rtc_time_flag = TRUE;
	MACNET_int_enable();
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_set_correction
*  Returned Value :
*  Comments       : Sets 1588counter corrections
*                   to eliminate the clock drift
*
*END*-----------------------------------------------------------------*/
static void MACNET_ptp_set_correction(MACNET_PTP_PRIVATE *priv, int32_t drift)
{
	int32_t  atcor_value;
	uint32_t atinc_tmp;
	int8_t   inc;

	if(drift != 0) {
        /* Correction counter wrap-around value calculation */
        atcor_value = ((int32_t)MACNET_1588_CLOCK_SRC)/drift;
        if(atcor_value == 0) {
            /* Drift is greater than the 1588 source clock;
	           the correction increment should be applied every tic of the 1588 timer
	           to speed up/slow very rapidly */
	        priv->MACNET_PTR->ATCOR  = ENET_ATCOR_COR(1);

            /* Calculate the correction increment to slow/speed up the clock
               more than +-1 per one tick */
            inc = MACNET_1588_CLOCK_INC + (drift/(int32_t)MACNET_1588_CLOCK_SRC);
            if(inc<0)
                inc = 0;
            if(inc>(ENET_ATINC_INC_CORR_MASK>>ENET_ATINC_INC_CORR_SHIFT))
                inc = (ENET_ATINC_INC_CORR_MASK>>ENET_ATINC_INC_CORR_SHIFT);
            atinc_tmp = (priv->MACNET_PTR->ATINC) & (~ENET_ATINC_INC_CORR_MASK);
            priv->MACNET_PTR->ATINC = atinc_tmp | ENET_ATINC_INC_CORR(inc);
        } else {
	        /* Drift is less than the 1588 source clock;
	           the correction increment should be applied once per atcor_value tics
	           of the 1588 timer */
	        if(atcor_value > 0) {
	            priv->MACNET_PTR->ATCOR  = ENET_ATCOR_COR(atcor_value);
	            atinc_tmp = (priv->MACNET_PTR->ATINC) & (~ENET_ATINC_INC_CORR_MASK);
	            priv->MACNET_PTR->ATINC = atinc_tmp | ENET_ATINC_INC_CORR(MACNET_1588_CLOCK_INC+1);
	        } else {
	            priv->MACNET_PTR->ATCOR  = ENET_ATCOR_COR(-atcor_value);
	            atinc_tmp = (priv->MACNET_PTR->ATINC) & (~ENET_ATINC_INC_CORR_MASK);
	            priv->MACNET_PTR->ATINC = atinc_tmp | ENET_ATINC_INC_CORR(MACNET_1588_CLOCK_INC-1);
	        }
        }
	} else {
	    /* Clock drift is reset */
	    priv->MACNET_PTR->ATCOR  = ENET_ATCOR_COR(0);
	    atinc_tmp = (priv->MACNET_PTR->ATINC) & (~ENET_ATINC_INC_CORR_MASK);
	    priv->MACNET_PTR->ATINC = atinc_tmp | ENET_ATINC_INC_CORR(MACNET_1588_CLOCK_INC);
	}
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_store_txstamp
*  Returned Value :
*  Comments       : This interrupt service routine stores the captured
*                   tx timestamp in the MACNET context structure
*
*END*-----------------------------------------------------------------*/
void MACNET_ptp_store_txstamp
   (
         /* [IN] the Ethernet state structure */
      void    *enet
   )
{ /* Body */

   ENET_CONTEXT_STRUCT_PTR    enet_ptr = (ENET_CONTEXT_STRUCT_PTR)enet;
   MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   ENET_MemMapPtr             macnet_ptr= macnet_context_ptr->MACNET_ADDRESS;
   uint32_t                    events, master_macnet_events;
   ENET_MemMapPtr             master_macnet_ptr;
   MACNET_PTP_TIME            curr_time;

   if (macnet_ptr == NULL)
       return;

   events = macnet_ptr->EIR;
   while (events & ENET_EIR_TS_AVAIL_MASK) {

      /* Clear the TS_AVAIL interrupt */
      macnet_ptr->EIR = ENET_EIR_TS_AVAIL_MASK;

      /* Store nanoseconds */
      macnet_context_ptr->PTP_PRIV->TXSTAMP.NSEC = macnet_ptr->ATSTMP;

      /* Store seconds, correct/avoid seconds incrementation when timestamp
         captured at the edge between two seconds (nanoseconds counter overflow) */
      if(MACNET_PTP_set_rtc_time_flag == FALSE) {
          master_macnet_ptr = MACNET_ptp_get_master_base_address();
          MACNET_int_disable();
          MACNET_ptp_get_curr_cnt(master_macnet_ptr, &curr_time);
          master_macnet_events = master_macnet_ptr->EIR;
          if(curr_time.NSEC > macnet_context_ptr->PTP_PRIV->TXSTAMP.NSEC)
              macnet_context_ptr->PTP_PRIV->TXSTAMP.SEC  = MACNET_PTP_seconds;
          else if(master_macnet_events & ENET_EIR_TS_TIMER_MASK)
              macnet_context_ptr->PTP_PRIV->TXSTAMP.SEC  = MACNET_PTP_seconds;
          else
              macnet_context_ptr->PTP_PRIV->TXSTAMP.SEC  = MACNET_PTP_seconds - 1;
          MACNET_int_enable();
      } else {
          /* Do not check nanoseconds counter overflow in case MACNET_PTP_SET_RTC_TIME ioctl was called */
          macnet_context_ptr->PTP_PRIV->TXSTAMP.SEC  = MACNET_PTP_seconds;
          MACNET_PTP_set_rtc_time_flag = FALSE;
      }

      _lwevent_set(&(macnet_context_ptr->PTP_PRIV->LWEVENT_PTP), MACNET_PTP_LWEVENT_TX_TS_INTR);

      events = macnet_ptr->EIR;
   }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_store_rxstamp
*  Returned Value :
*  Comments       : This routine is called from the MACNET RX ISR
*                   to store the captured rx timestamp
*                   in the adequate ring buffer
*END*-----------------------------------------------------------------*/
void MACNET_ptp_store_rxstamp(ENET_CONTEXT_STRUCT_PTR enet_ptr, PCB_PTR pcb_ptr, VENET_BD_STRUCT_PTR bdp)
{
	MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
	MACNET_PTP_PRIVATE         *priv = (MACNET_PTP_PRIVATE_PTR)macnet_context_ptr->PTP_PRIV;
	uint32_t                    msg_type, control, index, master_macnet_events;
	MACNET_PTP_DATA            tmp_rx_time;
	bool                    ptp_msg_received = FALSE;
    ENET_MemMapPtr             master_macnet_ptr;
    MACNET_PTP_TIME            curr_time;
    unsigned char                  *skb = pcb_ptr->FRAG[0].FRAGMENT;
    uint16_t                    chksum, len, num_added_bytes = HOST_TO_BE_SHORT_CONST(sizeof(MACNET_PTP_TIME));

	/* PTP over Ethernet: Check the PTPv2 over Ethernet type (identifier) */
	if (*(uint16_t *)(skb + MACNET_PTP_ETHER_PKT_TYPE_OFFS) == HOST_TO_BE_SHORT_CONST(MACNET_PACKET_TYPE_IEEE_802_3)) {
		tmp_rx_time.KEY = HOST_TO_BE_SHORT(*((uint16_t *)(skb + MACNET_PTP_ETHER_SEQ_ID_OFFS)));
        for(index=0;index<MACNET_PTP_CLOCKID_SIZE;index++)
            tmp_rx_time.CLOCKID[index] = *((uint8_t *)(skb + MACNET_PTP_ETHER_CLOCKID + index));
        control = *((uint8_t *)(skb + MACNET_PTP_ETHER_CTRL_OFFS));
	    msg_type = (*((uint8_t *)(skb + MACNET_PTP_ETHER_MSG_TYPE_OFFS))) & 0x0F;
        ptp_msg_received = TRUE;
    }
	/* PTP over UDP: Check if port is 319 for PTP Event, and check for UDP */
	else if (((*((uint16_t *)(skb + MACNET_PTP_UDP_PORT_OFFS))) == HOST_TO_BE_SHORT_CONST(MACNET_PTP_EVNT_PORT)) &&
	    (*(unsigned char *)(skb + MACNET_PTP_UDP_PKT_TYPE_OFFS) == MACNET_PACKET_TYPE_UDP)) {
		tmp_rx_time.KEY = HOST_TO_BE_SHORT(*((uint16_t *)(skb + MACNET_PTP_UDP_SEQ_ID_OFFS)));
        for(index=0;index<MACNET_PTP_CLOCKID_SIZE;index++)
            tmp_rx_time.CLOCKID[index] = *((uint8_t *)(skb + MACNET_PTP_UDP_CLOCKID + index));
        control = *((uint8_t *)(skb + MACNET_PTP_UDP_CTRL_OFFS));
        msg_type = (*((uint8_t *)(skb + MACNET_PTP_UDP_MSG_TYPE_OFFS))) & 0x0F;
        ptp_msg_received = TRUE;
    }

    if(ptp_msg_received == TRUE) {
		/* Store nanoseconds */
		tmp_rx_time.TS_TIME.NSEC = LONG_BE_TO_HOST((uint32_t)(bdp->TIMESTAMP));

		/* Store seconds, correct/avoid seconds incrementation when timestamp
           captured at the edge between two seconds (nanoseconds counter overflow) */
        if(MACNET_PTP_set_rtc_time_flag == FALSE) {
            master_macnet_ptr = MACNET_ptp_get_master_base_address();
            MACNET_int_disable();
            MACNET_ptp_get_curr_cnt(master_macnet_ptr, &curr_time);
            master_macnet_events = master_macnet_ptr->EIR;
            if(curr_time.NSEC > tmp_rx_time.TS_TIME.NSEC)
                tmp_rx_time.TS_TIME.SEC = MACNET_PTP_seconds;
            else if(master_macnet_events & ENET_EIR_TS_TIMER_MASK)
                tmp_rx_time.TS_TIME.SEC = MACNET_PTP_seconds;
            else
                tmp_rx_time.TS_TIME.SEC = MACNET_PTP_seconds - 1;
            MACNET_int_enable();
        } else {
            /* Do not check nanoseconds counter overflow in case MACNET_PTP_SET_RTC_TIME ioctl was called */
            tmp_rx_time.TS_TIME.SEC = MACNET_PTP_seconds;
            MACNET_PTP_set_rtc_time_flag = FALSE;
        }

		/* In INBAND mode, append the timestamp to the packet */
		if(enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PTP_INBAND) {
		    /* Enlarge the msg. */
		    pcb_ptr->FRAG[0].LENGTH += sizeof(MACNET_PTP_TIME);

		    /* Copy timestamp at the end of the msg. */
#if (PSP_ENDIAN == MQX_BIG_ENDIAN)
		    _mem_copy( &tmp_rx_time.TS_TIME, (void *)(pcb_ptr->FRAG[0].FRAGMENT + MACNET_PTP_EVENT_MSG_FRAME_SIZE + MACNET_PTP_INBAND_SEC_OFFS), sizeof(MACNET_PTP_TIME) );
#else
		    *(uint64_t *)(pcb_ptr->FRAG[0].FRAGMENT + MACNET_PTP_EVENT_MSG_FRAME_SIZE + MACNET_PTP_INBAND_SEC_OFFS) = MACNET_ptp_swap8bytes(tmp_rx_time.TS_TIME.SEC);
		    *(uint32_t *)(pcb_ptr->FRAG[0].FRAGMENT + MACNET_PTP_EVENT_MSG_FRAME_SIZE + MACNET_PTP_INBAND_NANOSEC_OFFS	) = HOST_TO_BE_LONG(tmp_rx_time.TS_TIME.NSEC);
#endif
		    /* Change the lenght field in the IP header */
		    len = HOST_TO_BE_SHORT(*(uint16_t *)(skb + MACNET_PTP_UDP_IPLENGHT_OFFS));
		    len += sizeof(MACNET_PTP_TIME);
		    *(uint16_t *)(skb + MACNET_PTP_UDP_IPLENGHT_OFFS) = HOST_TO_BE_SHORT(len);
		    /* Correct the IP header checksum */
		    chksum = HOST_TO_BE_SHORT(*(uint16_t *)(skb + MACNET_PTP_UDP_IPCHECKSUM_OFFS));
		    chksum = (((chksum) == 0xFFFF) ? (chksum) : ~(chksum) & 0xFFFF);
		    chksum = _mem_sum_ip(chksum, sizeof(uint16_t), &num_added_bytes);
		    chksum = (((chksum) == 0xFFFF) ? (chksum) : ~(chksum) & 0xFFFF);
		    *(uint16_t *)(skb + MACNET_PTP_UDP_IPCHECKSUM_OFFS) = HOST_TO_BE_SHORT(chksum);

		    /* Change the lenght field in the UDP header */
		    len = HOST_TO_BE_SHORT(*(uint16_t *)(skb + MACNET_PTP_UDP_UDPLENGHT_OFFS));
		    len += sizeof(MACNET_PTP_TIME);
		    *(uint16_t *)(skb + MACNET_PTP_UDP_UDPLENGHT_OFFS) = HOST_TO_BE_SHORT(len);
		    /* Correct the UDP header checksum */
		    chksum = HOST_TO_BE_SHORT(*(uint16_t *)(skb + MACNET_PTP_UDP_UDPCHECKSUM_OFFS));
		    chksum = (((chksum) == 0xFFFF) ? (chksum) : ~(chksum) & 0xFFFF);
		    chksum = _mem_sum_ip(chksum, sizeof(MACNET_PTP_TIME), (pcb_ptr->FRAG[0].FRAGMENT + MACNET_PTP_EVENT_MSG_FRAME_SIZE + MACNET_PTP_INBAND_SEC_OFFS));
		    chksum = _mem_sum_ip(chksum, sizeof(uint16_t), &num_added_bytes);
		    chksum = _mem_sum_ip(chksum, sizeof(uint16_t), &num_added_bytes);
		    chksum = (((chksum) == 0xFFFF) ? (chksum) : ~(chksum) & 0xFFFF);
		    *(uint16_t *)(skb + MACNET_PTP_UDP_UDPCHECKSUM_OFFS) = HOST_TO_BE_SHORT(chksum);
		} else {
		/* In OUTBAND mode, store the captured rx timestamp in the adequate ring buffer */
    		switch (control) {

    		case MACNET_PTP_MSG_SYNC:
    			MACNET_ptp_insert(&(priv->RX_TIME_SYNC), &tmp_rx_time);
    			break;

    		case MACNET_PTP_MSG_DEL_REQ:
    			MACNET_ptp_insert(&(priv->RX_TIME_DEL_REQ), &tmp_rx_time);
    			break;

    		case MACNET_PTP_MSG_ALL_OTHER:
    			switch (msg_type) {
    			case MACNET_PTP_MSG_P_DEL_REQ:
    				MACNET_ptp_insert(&(priv->RX_TIME_PDEL_REQ), &tmp_rx_time);
    				break;
    			case MACNET_PTP_MSG_P_DEL_RESP:
    				MACNET_ptp_insert(&(priv->RX_TIME_PDEL_RESP), &tmp_rx_time);
    				break;
    			default:
    				break;
    			}
    			break;
    		default:
    			break;
    		}
        }
	}
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_get_tx_timestamp
*  Returned Value :
*  Comments       : Returns tx timestamp of the message with the specific
*                   sequence ID and source clock ID
*
*END*-----------------------------------------------------------------*/
int32_t MACNET_ptp_get_tx_timestamp(MACNET_PTP_PRIVATE *priv,  void *param_ptr,
                                   MACNET_PTP_TIME *tx_time)
{
	MACNET_PTP_DATA tmp;

	if(ENET_OK == MACNET_ptp_find_and_remove(&(priv->TX_TIME),
	                                         ((MACNET_PTP_TS_DATA *)param_ptr)->SEQ_ID,
	                                         &((MACNET_PTP_TS_DATA *)param_ptr)->SPID[0],
	                                         &tmp)) {

        tx_time->SEC = tmp.TS_TIME.SEC;
        tx_time->NSEC = tmp.TS_TIME.NSEC;
        return ENET_OK;
	} else {
	    return ENET_ERROR;
	}
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_get_rx_timestamp
*  Returned Value :
*  Comments       : Returns rx timestamp of the message with the specific
*                   sequence ID and source clock ID
*
*
*END*-----------------------------------------------------------------*/
int32_t MACNET_ptp_get_rx_timestamp(MACNET_PTP_PRIVATE *priv, void *param_ptr,
		                           MACNET_PTP_TIME *rx_time)
{
	MACNET_PTP_DATA tmp;
	int32_t  error;

	switch (((MACNET_PTP_TS_DATA *)param_ptr)->MESSAGE_TYPE) {
	case MACNET_PTP_RX_TIMESTAMP_SYNC:
		error = MACNET_ptp_find_and_remove(&(priv->RX_TIME_SYNC),
		                                   ((MACNET_PTP_TS_DATA *)param_ptr)->SEQ_ID,
		                                   &((MACNET_PTP_TS_DATA *)param_ptr)->SPID[0],
		                                   &tmp);
		break;
	case MACNET_PTP_RX_TIMESTAMP_DEL_REQ:
		error = MACNET_ptp_find_and_remove(&(priv->RX_TIME_DEL_REQ),
		                                   ((MACNET_PTP_TS_DATA *)param_ptr)->SEQ_ID,
		                                   &((MACNET_PTP_TS_DATA *)param_ptr)->SPID[0],
		                                   &tmp);
		break;

	case MACNET_PTP_RX_TIMESTAMP_PDELAY_REQ:
		error = MACNET_ptp_find_and_remove(&(priv->RX_TIME_PDEL_REQ),
		                                   ((MACNET_PTP_TS_DATA *)param_ptr)->SEQ_ID,
		                                   &((MACNET_PTP_TS_DATA *)param_ptr)->SPID[0],
		                                   &tmp);
		break;
	case MACNET_PTP_RX_TIMESTAMP_PDELAY_RESP:
		error = MACNET_ptp_find_and_remove(&(priv->RX_TIME_PDEL_RESP),
		                                   ((MACNET_PTP_TS_DATA *)param_ptr)->SEQ_ID,
		                                   &((MACNET_PTP_TS_DATA *)param_ptr)->SPID[0],
		                                   &tmp);
		break;

	default:
		error = ENET_ERROR;
		break;
	}

	if (error==ENET_OK) {
		rx_time->SEC = tmp.TS_TIME.SEC;
		rx_time->NSEC = tmp.TS_TIME.NSEC;
		return ENET_OK;
	} else {
		return error;
	}
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_L2queue_init
* Returned Value : -
* Comments       : Initialize the queue for PTP-Ethernet packets
*
*END*-----------------------------------------------------------------*/
static void MACNET_ptp_L2queue_init(MACNET_PTP_L2QUEUE *ps_queue)
{
    uint32_t slot;
    ps_queue->WR_IDX = 0;
    ps_queue->RD_IDX = 0;
    for (slot = 0; slot < MACNET_PTP_DEFAULT_L2PCK_BUF_SZ; slot ++) {
        ps_queue->L2PCK[slot].LENGTH = 0;
    }
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_L2queue_is_empty
* Returned Value : -
* Comments       : Checks if the queue is empty
*
*END*-----------------------------------------------------------------*/
static bool MACNET_ptp_L2queue_is_empty(const MACNET_PTP_L2QUEUE *ps_queue)
{
    if (ps_queue->WR_IDX == ps_queue->RD_IDX) {
        return TRUE;
    }
    return FALSE;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_L2queue_add_packet
* Returned Value : -
* Comments       : Insert the PTP-Ethernet packet to the queue
*
*END*-----------------------------------------------------------------*/
static void MACNET_ptp_L2queue_add_packet(MACNET_PTP_L2QUEUE *ps_queue,
                                          const uint8_t *pb_buf,
                                          uint16_t len)
{
    if (ps_queue->L2PCK[ps_queue->WR_IDX].LENGTH != 0) {
      /* No free slots in the queue */
        return;
    }

    /* Store the packet */
    ps_queue->L2PCK[ps_queue->WR_IDX].LENGTH = len;
    _mem_copy((void *)pb_buf, (void *)ps_queue->L2PCK[ps_queue->WR_IDX].PACKET, len);

    /* Get the next queue slot to write */
    ps_queue->WR_IDX = (ps_queue->WR_IDX + 1) % MACNET_PTP_DEFAULT_L2PCK_BUF_SZ;

    return;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_L2queue_get_packet
* Returned Value : -
* Comments       : Gets the PTP-Ethernet packet from the queue
*
*END*-----------------------------------------------------------------*/
static bool MACNET_ptp_L2queue_get_packet(MACNET_PTP_L2QUEUE *ps_queue,
                                             uint8_t  *pb_buf,
                                             uint16_t *pLENGTH)
{
    bool ret_val = FALSE;

    /* Check queue argument */
    if( ps_queue == NULL ) {
        return FALSE;
    }

    /* Disable MACNET interrupts in order to avoid buffer data overwrite */
    MACNET_int_disable();

    /* check queue empty */
    if(MACNET_ptp_L2queue_is_empty(ps_queue)) {
        ret_val = FALSE;
    } else {
        /* Get the packet */
        *pLENGTH = ps_queue->L2PCK[ps_queue->RD_IDX].LENGTH;
        _mem_copy((void *)ps_queue->L2PCK[ps_queue->RD_IDX].PACKET, (void *)pb_buf, *pLENGTH);

        /* Clear the queue slot */
        ps_queue->L2PCK[ps_queue->RD_IDX].LENGTH = 0;

        /* Get the next queue slot to read */
        ps_queue->RD_IDX = (ps_queue->RD_IDX + 1) % MACNET_PTP_DEFAULT_L2PCK_BUF_SZ;

        ret_val = TRUE;
    }

    MACNET_int_enable();

    return ret_val;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_free_pcb
* Returned Value : -
* Comments       : This function enqueues the PCB allocated when seding
*                  layer2 packets (0x88F7 identifier).
*
*END*-----------------------------------------------------------------*/
static void MACNET_ptp_free_pcb
   (
         /* [IN] the PCB to enqueue */
      PCB_PTR  pcb_ptr
   )
{
   _mem_free((void *)pcb_ptr);
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_service_L2pckts
* Returned Value : -
* Comments       : This is the callback function for Ethernet 1588
*                  layer2 packets (0x88F7 identifier). It adds the
*                  PTP-Ethernet packet to the queue.
*
*END*-----------------------------------------------------------------*/
void MACNET_ptp_service_L2pckts
   (
      PCB_PTR  pcb,
         /* [IN] the received packet */
      void    *handle
         /* [IN] the IP interface structure */
   )
{
    ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR) handle;
    MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    MACNET_PTP_PRIVATE_PTR priv = macnet_context_ptr->PTP_PRIV;

    MACNET_ptp_L2queue_add_packet(priv->L2PCKS_PTR, pcb->FRAG[0].FRAGMENT, pcb->FRAG[0].LENGTH);

    PCB_free(pcb);
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_send_L2pckts
* Returned Value : -
* Comments       : Sends PTP-Ethernet packets.
*
*END*-----------------------------------------------------------------*/
int32_t MACNET_ptp_send_L2pckts(_enet_handle handle, void *param_ptr)
{
    PCB_PTR     pcb_ptr;
    unsigned char   *mem_ptr;
    ENET_HEADER_PTR  packet_ptr;
    uint16_t     len;

    len = ((MACNET_PTP_ETHERTYPE_PCK *)param_ptr)->LENGTH;

    /* Allocate memory */
    mem_ptr = _mem_alloc_system_zero(sizeof(PCB)+sizeof(PCB_FRAGMENT)+sizeof(ENET_HEADER)+len);
    if (mem_ptr==NULL) {
       return (ENETERR_ALLOC_PCB);
    }
    pcb_ptr = (PCB_PTR) mem_ptr;
    packet_ptr = (ENET_HEADER_PTR)&mem_ptr[sizeof(PCB)+sizeof(PCB_FRAGMENT)];

    /* Ethernet msg. header */
    htone(packet_ptr->DEST, ((MACNET_PTP_ETHERTYPE_PCK *)param_ptr)->DEST_MAC);
    htone(packet_ptr->SOURCE, ((ENET_CONTEXT_STRUCT_PTR)handle)->ADDRESS);
    mqx_htons(packet_ptr->TYPE, MQX1588_PTP_ETHERTYPE_1588);

    /* Payload */
    _mem_copy( (void *)(((MACNET_PTP_ETHERTYPE_PCK *)param_ptr)->PTP_MSG), (void *)&mem_ptr[sizeof(PCB)+sizeof(PCB_FRAGMENT)+sizeof(ENET_HEADER)], len );

    pcb_ptr->FREE = MACNET_ptp_free_pcb;
    pcb_ptr->FRAG[0].LENGTH = sizeof(ENET_HEADER) + len;
    pcb_ptr->FRAG[0].FRAGMENT = (unsigned char *)packet_ptr;

    return(ENET_send_raw((ENET_CONTEXT_STRUCT_PTR)handle, pcb_ptr));
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : MACNET_ptp_recv_L2pckts
* Returned Value : -
* Comments       : Withdraw the PTP-Ethernet packets from the queue.
*
*END*-----------------------------------------------------------------*/
int32_t MACNET_ptp_recv_L2pckts(_enet_handle handle, void *param_ptr)
{
    ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR) handle;
    MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    MACNET_PTP_PRIVATE_PTR priv = macnet_context_ptr->PTP_PRIV;

    if (FALSE == MACNET_ptp_L2queue_get_packet(priv->L2PCKS_PTR,
                                               ((MACNET_PTP_ETHERTYPE_PCK *)param_ptr)->PTP_MSG,
                                               &((MACNET_PTP_ETHERTYPE_PCK *)param_ptr)->LENGTH)) {
        return ENET_ERROR;
    }
    eaddrcpy(((MACNET_PTP_ETHERTYPE_PCK *)param_ptr)->DEST_MAC, enet_ptr->ADDRESS);
    return ENET_OK;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_ioctl
*  Returned Value :
*  Comments       :
*
*
*END*-----------------------------------------------------------------*/
uint32_t MACNET_ptp_ioctl(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint32_t command_id, void *inout_param)
{
    MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
    MACNET_PTP_PRIVATE *priv = macnet_context_ptr->PTP_PRIV;
    MACNET_PTP_TIME *cnt;
    MACNET_PTP_TIME rx_time, tx_time, curr_time;
    ENET_MemMapPtr master_macnet_ptr;
    int32_t retval = ENET_OK;

    master_macnet_ptr = MACNET_ptp_get_master_base_address();
    switch (command_id) {

    case MACNET_PTP_GET_RX_TIMESTAMP:
        /* Do not read rx timestamps from buffers in the inbound mode */
        if(enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PTP_INBAND) {
            retval = ENET_ERROR;
            break;
        }
        retval = MACNET_ptp_get_rx_timestamp(priv, inout_param, &rx_time);
        if (retval == ENET_OK)
            ((MACNET_PTP_TS_DATA *)inout_param)->TS = rx_time;
    break;

    case MACNET_PTP_GET_TX_TIMESTAMP:
        retval = MACNET_ptp_get_tx_timestamp(priv, inout_param, &tx_time);
        if (retval == ENET_OK)
            ((MACNET_PTP_TS_DATA *)inout_param)->TS = tx_time;
    break;

    case MACNET_PTP_GET_CURRENT_TIME:
        MACNET_ptp_get_curr_cnt(master_macnet_ptr, &curr_time);
        ((MACNET_PTP_RTC_TIME *)inout_param)->RTC_TIME = curr_time;
    break;

    case MACNET_PTP_SET_RTC_TIME:
        cnt = &(((MACNET_PTP_RTC_TIME *)inout_param)->RTC_TIME);
        MACNET_ptp_set_1588cnt(master_macnet_ptr, cnt);
    break;

    case MACNET_PTP_FLUSH_TIMESTAMP:
        /* reset sync buffer */
        priv->RX_TIME_SYNC.FRONT = 0;
        priv->RX_TIME_SYNC.END = 0;
        priv->RX_TIME_SYNC.SIZE = (MACNET_PTP_DEFAULT_RX_BUF_SZ + 1);
        /* reset delay_req buffer */
        priv->RX_TIME_DEL_REQ.FRONT = 0;
        priv->RX_TIME_DEL_REQ.END = 0;
        priv->RX_TIME_DEL_REQ.SIZE = (MACNET_PTP_DEFAULT_RX_BUF_SZ + 1);
        /* reset pdelay_req buffer */
        priv->RX_TIME_PDEL_REQ.FRONT = 0;
        priv->RX_TIME_PDEL_REQ.END = 0;
        priv->RX_TIME_PDEL_REQ.SIZE = (MACNET_PTP_DEFAULT_RX_BUF_SZ + 1);
        /* reset pdelay_resp buffer */
        priv->RX_TIME_PDEL_RESP.FRONT = 0;
        priv->RX_TIME_PDEL_RESP.END = 0;
        priv->RX_TIME_PDEL_RESP.SIZE = (MACNET_PTP_DEFAULT_RX_BUF_SZ + 1);
    break;

    case MACNET_PTP_SET_COMPENSATION:
        MACNET_ptp_set_correction(priv, ((MACNET_PTP_SET_COMP *)inout_param)->DRIFT);
    break;

    case MACNET_PTP_GET_ORIG_COMP:
    break;

    case MACNET_PTP_REGISTER_ETHERTYPE_PTPV2:
        /* Registers the PTPV2 protocol type on an Ethernet channel */
        retval = ENET_open(enet_ptr, MQX1588_PTP_ETHERTYPE_1588, MACNET_ptp_service_L2pckts, enet_ptr);
        /* Allocate the queue for this type of messages */
        priv->L2PCKS_PTR = (MACNET_PTP_L2QUEUE_PTR) _mem_alloc_system_zero(sizeof(MACNET_PTP_L2QUEUE));
        MACNET_ptp_L2queue_init(priv->L2PCKS_PTR);
    break;

    case MACNET_PTP_UNREGISTER_ETHERTYPE_PTPV2:
        /* Un-registers the PTPV2 protocol type on an Ethernet channel */
        retval = ENET_close(enet_ptr, MQX1588_PTP_ETHERTYPE_1588);
        /* Free the queue for this type of messages */
        _mem_free(priv->L2PCKS_PTR);
    break;

    case MACNET_PTP_SEND_ETHERTYPE_PTPV2_PCK:
        retval = MACNET_ptp_send_L2pckts(enet_ptr, inout_param);
    break;

    case MACNET_PTP_RECV_ETHERTYPE_PTPV2_PCK:
        retval = MACNET_ptp_recv_L2pckts(enet_ptr, inout_param);
    break;
    case MACNET_PTP_IS_IN_INBAND_MODE:
        if(enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PTP_INBAND) {
            *(uint8_t*)inout_param = TRUE;
        } else {
            *(uint8_t*)inout_param = FALSE;
        }
    break;
    default:
        return IO_ERROR_INVALID_IOCTL_CMD;
    }
    return retval;
}
#endif /* ENETCFG_SUPPORT_PTP */
