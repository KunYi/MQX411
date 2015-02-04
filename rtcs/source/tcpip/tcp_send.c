/*HEADER**********************************************************************
*
* Copyright 2008, 2014 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   Send Buffer Space:
*   |----+----------+-- - - - |
*   sndbuf                     sndbuf
*   :                       +sndlen
*   :
*   :
*   :
*   Send Sequence Space:     :
*   :
*   0  : 1       2          3          4
*   |-- - -       - -----+----|----------|----------|----------
*   sndiss                 :  snduna     sndnxt     snduna
*   sndbufseq                   +sndwnd
*   0  - very old sequence numbers, acknowledged and "returned" to client(s)
*   1  - old sequence numbers which have been acknowledged
*   2  - sequence numbers of unacknowledged data (may be resent)
*   3  - sequence numbers allowed for new data transmission
*   4  - future sequence numbers which are not yet allowed
*   Notes:
*   - the send buffer may end (sndbuf+sndlen) in section 3 or 4;
*   - the send buffer space is not a contiguous area of memory, but
*   rather a chain of buffer segments, some of which are in the
*   `send' circular ring buffer, some of which are owned by sender
*   clients;
*   - some seq #'s and related values, e.g., sndxmit, sndcwnd, sndstresh,
*   etc., are not shown
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"      /* for error codes */
#include "tcp_prv.h"    /* for TCP internal definitions */


///////////////
#include "ip_prv.h"
///////////////


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Update_send_window
* Returned Values : None.
* Comments        :
*
*  Update the nb of bytes we can send.  This should usually increase,
*   not decrease, unless the peer TCP is broken...
*
*END*-----------------------------------------------------------------*/

void TCP_Update_send_window
   (
      TCB_STRUCT_PTR    tcb,  /* IN/OUT - TCP context */
      TCP_HEADER   *seg   /* IN     - the incoming ACK packet */
   )
{ /* Body */
   uint32_t  ack;
   uint16_t  win;

   ack = mqx_ntohl(seg->ack);
   win = mqx_ntohs(seg->window);

   if ( LT32( ack + win, tcb->snduna + tcb->sndwnd ) ) {

      switch ( tcb->state ) {
         case SYN_SENT:
         case SYN_RECEIVED:
         case ESTABLISHED:
         case CLOSE_WAIT:

            /* Aargh!  Window shrink! */
            RTCS_log_error( ERROR_TCP,
                                 RTCSERR_TCP_SHRINKER_HOST,
                                 (uint32_t)tcb->remote_host,
                                 0, 0 );
            break;

         default:
            break;

      } /* End Switch */

      tcb->sndprobeto = tcb->sndrto;      /* in case window becomes 0 */

      /*  We may have to decrease sndnxt, this a dubious thing to do,
       *   but then again we're taking recovery measures because of a
       *   misbehaving host.
       *  Assume seg->ack+seg->window won't actually be less than snduna
       */
      if ( GT32(tcb->sndnxt, ack + win) ) {
         tcb->sndnxt = ack + win;
      } /* Endif */

   } /* Endif */

   tcb->sndwnd = win;
   tcb->sndwl1 = mqx_ntohl(seg->seq);
   tcb->sndwl2 = ack;
   if ( tcb->sndwnd > tcb->sndwndmax ) {
      tcb->sndwndmax = tcb->sndwnd;
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_RTT_measure
* Returned Values : SndClock *
* Comments        :
*
*  We just received a packet which ack'ed something we sent;
*   given the timestamp of receipt, look for the latest transmission
*   timestamp which got ack'ed (if any) and use it for a measure M of
*   RTT (Round Trip Time), to compute new RTT estimates (average and variance)
*   and RTO (Retransmission TimeOut);
*
*  There shouldn't be any transmission timestamps stored for retransmitted
*   segments, so that we know what we have is a valid measure, without
*   retransmission ambiguity.
*
*  Returns pointer to first remaining transmission timestamp entry if any,
*   or 0 if none (this is for efficiency's sake, since this is readily
*   available here, and possibly needed by caller)
*
*END*-----------------------------------------------------------------*/

SndClock  *TCP_Process_RTT_measure
   (
      TCB_STRUCT_PTR tcb,     /* IN/OUT - TCP context */
      int32_t         rcvtime  /* IN     - time packet was received */
   )
{ /* Body */
   SndClock      *tail;
   int32_t         M;

   /*  If xmit timestamp queue is empty, or its first entry is beyond all
    *   ack'ed segments, quit
    */
   tail = tcb->sndclktail;

   if ( tail == tcb->sndclkhead && tcb->sndclkavail > 0 ) {
      /* No timestamps active */
      return NULL;
   } /* Endif */

   if ( GT32(tail->seq, tcb->snduna) ) {
      /* Last seq # of earliest timestamp not yet acknowledged */
      return tail;
   } /* Endif */

   /*  Get latest ack'ed xmit timestamp (remove it and all previous ones)
    */
   do {
      M = tail->time;      /* xmit timestamp */
      tcb->sndclkavail++;
      if ( ++tail >= tcb->sndclkend ) {
         tail = tcb->sndclk;
      } /* Endif */
   } while (tail != tcb->sndclkhead && LE32(tail->seq, tcb->snduna));

   tcb->sndclktail = tail;
   M = rcvtime - M;        /* RTT measure */

   /* This is from Van Jacobson's algorithm.
    * Note that sndrtta is the average xmt time scaled up by a
    * factor of 8, and sndrttd is the average xmt time deviation
    * scaled up by a factor of 4 (explaining the shifting going on).
    */
   M -= (tcb->sndrtta >> 3);
   tcb->sndrtta += M;

   if ( M < 0 ) {
      M = -M;
   } /* Endif */
   M -= (tcb->sndrttd >> 2);
   tcb->sndrttd += M;
   /*2.02 */
   tcb->sndrto = (((tcb->sndrtta >> 2) + tcb->sndrttd) >> 1) + TICK_LENGTH;

   if ( tcb->sndrto < _TCP_rto_min ) {
      tcb->sndrto = _TCP_rto_min;
   } /* Endif */

   if ( tcb->sndrto > tcb->sndrtomax ) {
      tcb->sndrto = tcb->sndrtomax;
   } /* Endif */

   return tail;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP6_Send_IP_packet
* Returned Values : int32_t
* Comments        :
*
*  Compute checksum and send packet to IP layer;
*   all packets are sent through here.
*
*  Returns RTCS_OK if packet accepted.
*
*  If an error is returned, the PCB is gone (freed by IP_send)
*   but the request still must be returned.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6 
uint32_t TCP6_Send_IP_packet
   (
      RTCSPCB_PTR          pcb,         /* IN/OUT - packet to send */
      in6_addr            *srchost,    /* IN     - IPv6 source address */
      in6_addr            *desthost,   /* IN     - IPv6 dest address */
      bool              bypass      /* IN     - TRUE to bypass checksum */
   )
{ 
    TCP_HEADER         *seg = (TCP_HEADER *)RTCSPCB_DATA(pcb);
    int32_t              err;
#if BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
    in6_addr            *if_addr;
    IP_IF_PTR           if_ptr;
#endif

#if BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
    /* HW-offload.*/
    if_ptr = pcb->IFSRC; /* Detect output interface.*/
    if(if_ptr == NULL)
    {
        if_addr = IP6_route_find(desthost);
        if(if_addr)
        {
            if_ptr = ip6_if_get_by_addr(if_addr);
        }
    }

    if(if_ptr 
        && (if_ptr->FEATURES & IP_IF_FEATURE_HW_TX_PROTOCOL_CHECKSUM)
        && (IP6_will_fragment(if_ptr, RTCSPCB_SIZE(pcb)) == FALSE))
    {
        pcb->TYPE |= RTCSPCB_TYPE_HW_PROTOCOL_CHECKSUM;
    }
    else
#endif
    if ( ! bypass )
    {
        /*
        ** Calculate the checksum for the packet now, and let IP
        ** add the IP pseudo checksum later.
        */
        pcb->IP_SUM     = IP_Sum_PCB(RTCSPCB_SIZE(pcb), pcb);
        pcb->IP_SUM_PTR = seg->checksum;
    }

    err = IP6_send(pcb, IPPROTO_TCP, srchost, desthost, pcb->IFSRC);

    if ( err != RTCS_OK )
    {
        DEBUGTCP(printf("\nTCP: TCP_Send_IP_packet: errno %x on IP_send.", err));

        /* error will be recorded in TCB by caller... */
    }

   return err;
}
#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Send_IP_packet
* Returned Values : int32_t
* Comments        :
*
*  Compute checksum and send packet to IP layer;
*   all packets are sent through here.
*
*  Returns RTCS_OK if packet accepted.
*
*  If an error is returned, the PCB is gone (freed by IP_send)
*   but the request still must be returned.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
uint32_t TCP_Send_IP_packet
   (
      RTCSPCB_PTR          pcb,         /* IN/OUT - packet to send */
      _ip_address          srchost,    /* IN     - source address */
      _ip_address          desthost,   /* IN     - dest address */
      bool              bypass      /* IN     - TRUE to bypass checksum */
   )
{ 
    TCP_HEADER         *seg = (TCP_HEADER *)RTCSPCB_DATA(pcb);
    int32_t              err;
#if BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
    _ip_address         if_addr;
    IP_IF_PTR           if_ptr;
#endif

   /*
   ** Finish things up, checksum, etc
   */


#if BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
    /* HW-offload.*/
    if_addr = IP_route_find(desthost, 1);
    if_ptr = IP_find_if(if_addr);

    if(if_ptr 
        && (if_ptr->FEATURES & IP_IF_FEATURE_HW_TX_PROTOCOL_CHECKSUM)
        && (IP_will_fragment(if_ptr, RTCSPCB_SIZE(pcb)) == FALSE))
    {
        pcb->TYPE |= RTCSPCB_TYPE_HW_PROTOCOL_CHECKSUM;
    }
    else
#endif
    {
        if ( ! bypass )
        {
            /*
            ** Calculate the checksum for the packet now, and let IP
            ** add the IP pseudo checksum later.
            */
            pcb->IP_SUM     = IP_Sum_PCB(RTCSPCB_SIZE(pcb), pcb);
            pcb->IP_SUM_PTR = seg->checksum;
        }
    }

    err = IP_send(pcb, IPPROTO_TCP, srchost, desthost, 0);

    if ( err != RTCS_OK )
    {
        DEBUGTCP(printf("\nTCP: TCP_Send_IP_packet: errno %x on IP_send.", err));
        /* error will be recorded in TCB by caller... */
    }
    return err;
}
#endif


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Send_packet
* Returned Values : None.
* Comments        :
*
*  Send a packet, starting at data sequence `seq';
*
*  Returns OK or error code (RTCSERR_TCP_NOT_CONN, RTCSERR_OUT_OF_MEMORY or
*   IP_send() error)  (return value ignored at this time).
*
*  If error returned, the upper layer request will have been
*   returned to the upper layer and the PCB will have been freed.
*
*  Called by TCP_Transmit() only.
*
*END*-----------------------------------------------------------------*/

void TCP_Send_packet
   (
      TCB_STRUCT_PTR          tcb,     /* IN/OUT - TCP layer context */
      uint32_t                 seq,     /* IN     - data sequence number */
      TCP_CFG_STRUCT_PTR      tcp_cfg  /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   TCP_HEADER       *seg;
   RTCSPCB_PTR       pcb;
   unsigned char         *data;
   uint32_t           sent;
   int32_t            window,
                     err;
   bool           allowdata;     /* true if data can be sent in packet */
   uint16_t           size;          /* size of TCP portion of packet */
   uint16_t           flags = 0;
   uint32_t           dataseq = seq; /* seq # of 1st data byte sent */
   uint32_t           dataendseq;    /* seq # of 1st byte after data sent */
   uint32_t           rexmit_endseq;
   uint32_t           old_sndnxt;    /* stores original sndnxt */
   
   RTCSLOG_FNE4(RTCSLOG_FN_TCP_Send_packet, tcb, seq, tcp_cfg);

   switch ( tcb->state ) {      /* can we put data in the packet? */

      case ESTABLISHED:
      case FINWAIT_1:
      case CLOSING:
      case CLOSE_WAIT:
      case LAST_ACK:
         allowdata = TRUE;
         break;

      case SYN_SENT:
      case SYN_RECEIVED:
         if ( seq == tcb->sndiss ) {
            flags |= SYN;
            dataseq++;
         } /* Endif */
         allowdata = FALSE;
         break;

      case FINWAIT_2:
      case TIME_WAIT:
         /*
         ** This should only occur in TIME_WAIT state for the
         ** ACK of the FIN that transitted us from FIN_WAIT_2
         ** to TIME_WAIT state.
         */
         allowdata = FALSE;
         break;

      case LISTEN:
      case CLOSED:
      default:

         /* This shouldn't normally happen */
         /* Can't send in LISTEN or CLOSED state */
         RTCS_log_error( ERROR_TCP, RTCSERR_TCP_SP_BAD_SEND_STATE, (uint32_t)tcb, 0, 0 );
         RTCSLOG_FNX2(RTCSLOG_FN_TCP_Send_packet, -1);
         return;

   } /* End Switch */

   pcb = RTCSPCB_alloc_send(); /* get a packet */
   if ( pcb == NULL ) {

      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_MISSED++);
      RTCS_log_error( ERROR_TCP,
                           RTCSERR_TCP_SP_OUT_OF_PCBS,
                           (uint32_t)tcb, 0, 0);
      
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Send_packet, -2);
      return;

   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb);

   /*
   ** Determine the size of the TCP header.  Warning: this calculation
   ** has to be consistent with the algorithm below.
   */
   size = sizeof(TCP_HEADER);
   if ((flags & SYN) != 0 /* && tcb->rcvmss != TCP_MSS_DEFAULT */)  {
      size += 4;
   } /* Endif */

   /*
   ** Skip packet's lower-level header(s), fill TCP header;
   **  We should allow space for IP options, if we ever add any...
   */
   RTCSPCB_insert_header(pcb, size);
   seg = (TCP_HEADER *)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_TRANSPORT(pcb) = RTCSPCB_DATA(pcb);
   mqx_htons(seg->source_port, tcb->local_port);
   mqx_htons(seg->dest_port, tcb->remote_port);
   mqx_htonl(seg->seq, seq);
   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_TCP), 0);

   /*
   ** These aren't done in ESTABLISHED state
   */

   if ( tcb->state == SYN_SENT ) {
      mqx_htonl(seg->ack, 0);
   } else {
      flags     |= ACK;
      
       /* draft-ietf-tcpm-tcpsecure-00.txt section 3.2 */
      if(tcb->tcpsecuredraft0) {
        mqx_htonl(seg->ack, tcb->rcvnxt - tcb->ackmodifier);
      }
      else
      {
        mqx_htonl(seg->ack, tcb->rcvnxt);
      }
      
      tcb->ackmodifier = 0;
   } /* Endif */

   tcb->rcvuna = tcb->rcvnxt;
   mqx_htons(seg->window, tcb->rcvwndedge - tcb->rcvnxt);
   if(((tcb->options & TCPO_ACTIVE) != 0) && ((flags & SYN) != 0))
   {
     /* if this is active open and we are sending SYN */
     uint16_t window = 0;
     if(tcb->rcvlen > tcb->rcvwndmax)
     {
       window = (uint16_t)tcb->rcvwndmax;
     }
     else
     {
       window = (uint16_t)tcb->rcvlen;
     }
     mqx_htons(seg->window, window);
   }

   if ( (tcb->status & TCPS_TURGENT) != 0 ) {

      { /* Scope */

         int32_t urgentptr = tcb->sndup - seq - 1;

         if ( urgentptr >= 0 ) {

            if ( urgentptr > 0xffff ) {
               mqx_htons(seg->urgent, 0xffff);
            } else {
               mqx_htons(seg->urgent, urgentptr);
            } /* Endif */
            flags |= URG;

         } else {
            mqx_htons(seg->urgent, 0);
         } /* Endif */

      } /* Endscope */

   } else {
      mqx_htons(seg->urgent, 0);
   } /* Endif */

   size = sizeof(TCP_HEADER);
   data = (unsigned char *)seg + size;

   /*
   ** Add options - always send mss (with SYN)
   */
   if ((flags & SYN) != 0 /* && tcb->rcvmss != TCP_MSS_DEFAULT */) {

      /* See RFC1122 4.2.2.6 */
      mqx_htonc(data, OPT_MSS);
      data++;
      mqx_htonc(data, 4);
      data++;
      mqx_htons(data, tcb->rcvmss);
      data++;
      data++;
      size += 4;

   } /* Endif */


   flags |= size << (DATAOFS_SHIFT-2); /* -2 to give 32 bit multiples */

   /*
   ** Add data if there is any to send.
   */
   if ( allowdata ) {

      /*
      ** Compute amount of data to send
      */
      sent = tcb->sndbufseq + tcb->sndlen - dataseq;  /* amnt available */
      window = tcb->snduna + tcb->sndwnd - dataseq;
      if ( (int32_t)sent > window ) {

         sent = window;

         /* If the send window is closed, send a probe byte */
         if (tcb->sndwnd == 0) {
            sent = 1;
         } /* Endif */

      } /* Endif */

      /*
      ** I'm not counting SYN,FIN; (we only send MSS option with SYN,
      ** where we don't send data):
      */
      if ( (int32_t)sent > tcb->sndmax ) {
         sent = tcb->sndmax;
      } /* Endif */

      /*
      ** Don't chop-off sent data at tcb->sndpush even if there
      ** is a PUSH in the middle of the data to send
      */

      /*
      ** Copy data to send into the PCB.
      */

      if ( (int32_t)sent > 0 ) {
         tcb->send_error =
            TCP_Copy_from_buffer(tcb,  /* TCP context */
                        data,          /* dest buffer */
                        &sent,         /* no. of bytes to copy */
                        dataseq - tcb->sndbufseq,
                                       /* offset from start of xmit buf */
                        pcb            /* the PCB in use */
                     );
         size += sent;

      } else {
         sent = 0;      /* sent was negative or 0, clamp to 0 */
      } /* Endif */

   } else {
      sent = 0;
   } /* Endif */

   /*
   ** Seq # of 1st byte past data being sent
   */
   dataendseq = dataseq + sent;

   /* advance (re)transmission ptr (but not past FIN) */
   tcb->sndxmit = dataendseq;

   /*
   ** Have we sent all data for the connection, so that we can send FIN?
   */
   if ( GT32(dataendseq, tcb->sndnxt)     &&   /* not retxmt */
      (tcb->status & TCPS_FINTOSEND) != 0 &&   /* client wants to close */
      dataendseq == (tcb->sndbufseq + tcb->sndlen))       /* sent all data */
   {

      TCP_Process_effective_close(tcb, tcp_cfg);

   } /* Endif */

   /*
   ** When in one of these 'closing' states, the end of the send buffer
   **   (sndbufseq + sndlen) is the FIN's sequence #, so check to add FIN
   */
   if ( tcb->state != ESTABLISHED ) {   /* save a little time in mainline */

      switch ( tcb->state ) {
         case FINWAIT_1:
         case FINWAIT_2:
         case CLOSING:
         case TIME_WAIT:
         case LAST_ACK:
            if ( dataendseq == (tcb->sndbufseq + tcb->sndlen) ) {
               flags |= FIN;
               dataendseq++;   /* count FIN */
            } /* Endif */
            break;

         default:
            break;

      } /* End Switch */

   } /* Endif */

   /*
   ** Advance sndnxt as necessary
   */
   old_sndnxt = tcb->sndnxt;        /* save old ending sequence # */
   if ( GT32(dataendseq, tcb->sndnxt) ) {

      /*
      ** New data is being sent
      */
      rexmit_endseq = tcb->sndnxt;   /* after last rexmitted byte */
      tcb->sndnxt   = dataendseq;

      /*
      ** Timestamp segment being sent, for rtt measurement.
      **    Note that new clks go on the ring head, not tail.
      */
      if ( tcb->sndclkavail > 0 ) {

         { /* Scope */

            SndClock      *clk = tcb->sndclkhead;

            clk->time = RTCS_time_get();
            clk->seq  = dataendseq;
            if ( ++clk >= tcb->sndclkend ) {
               clk = tcb->sndclk;
            } /* Endif */
            tcb->sndclkhead = clk;
            tcb->sndclkavail--;

         } /* Endscope */

      } /* Endif */

   } else {
      rexmit_endseq = dataendseq;   /* after last would-be rexmitted byte */
                                    /*   (if we are retransmitting) */
   } /* Endif */

   /*
   ** If part of the packet is a retransmission, then everything before
   **   rexmit_endseq is a retransmission, so invalidate all timestamps
   **   up to rexmit_endseq.
   **   (can't use a timestamp if round-trip-time becomes ambiguous by
   **    retransmission)
   */
   if ( LT32(seq, old_sndnxt) ) {

      { /* Scope */

         SndClock      *tail = tcb->sndclktail;

         if ( tail != tcb->sndclkhead || tcb->sndclkavail == 0 ) {

            while ( LE32(tail->seq, rexmit_endseq) ) {

               /*  Remove timestamp...
                */
               tcb->sndclkavail++;
               if ( ++tail >= tcb->sndclkend ) {
                  tail = tcb->sndclk;
               } /* Endif */
               if ( tail == tcb->sndclkhead ) {
                  break;
               } /* Endif */

            } /* Endwhile */
            tcb->sndclktail = tail;

         } /* Endif */

      } /* Endscope */

   } /* Endif */

   /*
   ** Send a PSH only if this is the last segment (packet) of data to
   **   be pushed
   */
   //if ( ((tcb->status & TCPS_TPUSH) != 0  &&
   //    GE32(tcb->sndpush, dataseq)      &&
   //    LE32(tcb->sndpush,dataseq+sent)) ||
   //   (flags & FIN))
   if(!(flags & SYN)) /* No PSH when SYN */
   {
      flags |= PSH;
   }

   /*
   ** Finish things up (clearing the RESERVED field)
   */
   flags = flags & TCP_RSRV_FIELD_MASK;
   mqx_htons(seg->flags, flags);
   mqx_htons(seg->checksum, 0);

   /* Just get the TX option from the TCB, as it is always stored there now */
   pcb->LINK_OPTIONS.TX = tcb->TX;

   DEBUGTCP(printf("\nTCP_SendPkt: sport=%d, dport=%d, seq=%X, ack=%X, flags=%X, wndw=%d ",
      mqx_ntohs(seg->source_port), mqx_ntohs(seg->dest_port), mqx_ntohl(seg->seq), mqx_ntohl(seg->ack),
      mqx_ntohs(seg->flags), mqx_ntohs(seg->window)));
      
     
#if RTCSCFG_ENABLE_IP6     
   if(tcb->af == AF_INET6)
   { 
       /* Set pcb interface in according with scope_id
        * Scope_id was taken from 
        *   1) remoute scope_id(if not NULL), 
        *   2) local scope_id(if not NULL)
        */
        if(tcb->if_scope_id)
        {
            pcb->IFSRC = ip6_if_get_by_scope_id(tcb->if_scope_id);
        }
        else
        {   /* if scope_id was not setted let use local address interface */
            pcb->IFSRC = ip6_if_get_by_addr(&tcb->local_ipv6_host);
        }

        err = TCP6_Send_IP_packet(   pcb,                   /* packet to send */
                                    &tcb->local_ipv6_host,       /* local address */
                                    &tcb->remote_ipv6_host,      /* dest address */
                                    tcb->bypass_tx_chksum  /* bypass checksum? */
                                 );
   }
   else
#endif 
#if RTCSCFG_ENABLE_IP4
   if( (tcb->af == AF_INET ) ||(tcb->af == AF_UNSPEC))
   {
  

        err = TCP_Send_IP_packet(   pcb,                   /* packet to send */
                                    tcb->local_host,       /* local address */
                                    tcb->remote_host,      /* dest address */
                                    tcb->bypass_tx_chksum  /* bypass checksum? */
                                );
   } 
   else
#endif
   {}

   
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_TOTAL++);
   if ( err != RTCS_OK ) {
      tcb->lasterror = err;
      TCP_Event(tcb, TCPS_SOFTERROR);
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_MISSED++);
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_DISCARDED++);
   } /* Endif */
   
   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Send_packet, 0);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP6_Send_reply_reset
* Returned Values : None.
* Comments        :
*
*  Send a RST segment as a reply to some erroneous/unwanted segment.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6

void TCP6_Send_reply_reset
   (
      /* Need access to TX options */
      TCB_STRUCT_PTR       tcb,        /* IN/OUT - TCP context */
      in6_addr             *source,    /* IN     - destination for reset */
      TCP_HEADER      *received,   /* IN     - received TCP packet */
      uint16_t              rlen,       /* IN     - length of received packet */
      TCP_CFG_STRUCT_PTR   tcp_cfg,    /* IN/OUT - TCP layer constants */
      bool              bypass_chk  /* IN     - TRUE to bypass checksum */
   )
{ /* Body */
   TCP_HEADER       *seg;
   RTCSPCB_PTR       pcb;
   uint32_t           len;
   uint16_t           port;

   pcb = RTCSPCB_alloc_send(); /* get the packet */
   if ( pcb == NULL ) {
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_MISSED++);
      RTCS_log_error( ERROR_TCP,
                           RTCSERR_TCP_SRR_OUT_OF_PCBS,
                           (uint32_t)source,
                           (uint32_t)received,
                           0 );
      return;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb);
   RTCSPCB_insert_header(pcb, sizeof(TCP_HEADER));
   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_TCP), 0);

   /*  Fill header...  no data, no complications, very short & sweet
    */
   seg = (TCP_HEADER *)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_TRANSPORT(pcb) = RTCSPCB_DATA(pcb);
   port = mqx_ntohs(received->dest_port);
   mqx_htons(seg->source_port, port);
   port = mqx_ntohs(received->source_port);
   mqx_htons(seg->dest_port, port);

   /*
   **  The following (found all over the RFC, where appropriate), is a way to
   **  make our ACK acceptable to the connection we are resetting:
   */
   if ( mqx_ntohs(received->flags) & ACK ) {
      len = mqx_ntohl(received->ack);
      mqx_htonl(seg->seq, len);
      mqx_htonl(seg->ack, 0);
      mqx_htons(seg->flags, RST | ( sizeof(TCP_HEADER) << (DATAOFS_SHIFT-2) ));
   } else {
      len = mqx_ntohl(received->seq) + rlen;
      mqx_htonl(seg->seq, 0);
      mqx_htonl(seg->ack, len);
      mqx_htons(seg->flags, RST | ACK | ( sizeof(TCP_HEADER) << (DATAOFS_SHIFT-2) ));
   } /* Endif */

   mqx_htons(seg->window,   0);
   mqx_htons(seg->checksum, 0);
   mqx_htons(seg->urgent,   0);

   if (tcb!=NULL) {
      pcb->LINK_OPTIONS.TX = tcb->TX;
   }
   
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_TOTAL++);
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
   TCP6_Send_IP_packet(
                        pcb,                                /* packet to send */
                        (in6_addr*)(&(in6addr_any)),        /* local address */
                        source,                             /* dest address */
                        bypass_chk                          /* bypass checksum? */
                      );

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP6 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Send_reply_reset
* Returned Values : None.
* Comments        :
*
*  Send a RST segment as a reply to some erroneous/unwanted segment.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void TCP_Send_reply_reset
   (
      /* Need access to TX options */
      TCB_STRUCT_PTR       tcb,        /* IN/OUT - TCP context */
      _ip_address          source,     /* IN     - destination for reset */
      TCP_HEADER      *received,   /* IN     - received TCP packet */
      uint16_t              rlen,       /* IN     - length of received packet */
      TCP_CFG_STRUCT_PTR   tcp_cfg,    /* IN/OUT - TCP layer constants */
      bool              bypass_chk  /* IN     - TRUE to bypass checksum */
   )
{ /* Body */
   TCP_HEADER       *seg;
   RTCSPCB_PTR       pcb;
   uint32_t           len;
   uint16_t           port;

   pcb = RTCSPCB_alloc_send(); /* get the packet */
   if ( pcb == NULL ) {
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_MISSED++);
      RTCS_log_error( ERROR_TCP,
                           RTCSERR_TCP_SRR_OUT_OF_PCBS,
                           (uint32_t)source,
                           (uint32_t)received,
                           0 );
      return;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb);
   RTCSPCB_insert_header(pcb, sizeof(TCP_HEADER));
   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_TCP), 0);

   /*  Fill header...  no data, no complications, very short & sweet
    */
   seg = (TCP_HEADER *)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_TRANSPORT(pcb) = RTCSPCB_DATA(pcb);
   port = mqx_ntohs(received->dest_port);
   mqx_htons(seg->source_port, port);
   port = mqx_ntohs(received->source_port);
   mqx_htons(seg->dest_port, port);

   /*
   **  The following (found all over the RFC, where appropriate), is a way to
   **  make our ACK acceptable to the connection we are resetting:
   */
   if ( mqx_ntohs(received->flags) & ACK ) {
      len = mqx_ntohl(received->ack);
      mqx_htonl(seg->seq, len);
      mqx_htonl(seg->ack, 0);
      mqx_htons(seg->flags, RST | ( sizeof(TCP_HEADER) << (DATAOFS_SHIFT-2) ));
   } else {
      len = mqx_ntohl(received->seq) + rlen;
      mqx_htonl(seg->seq, 0);
      mqx_htonl(seg->ack, len);
      mqx_htons(seg->flags, RST | ACK | ( sizeof(TCP_HEADER) << (DATAOFS_SHIFT-2) ));
   } /* Endif */

   mqx_htons(seg->window,   0);
   mqx_htons(seg->checksum, 0);
   mqx_htons(seg->urgent,   0);

   if (tcb!=NULL) {
      pcb->LINK_OPTIONS.TX = tcb->TX;
   }
   
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_TOTAL++);
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
   TCP_Send_IP_packet(pcb,             /* packet to send */
                      INADDR_ANY,      /* local address */
                      source,          /* dest address */
                      bypass_chk       /* bypass checksum? */
                     );

}
#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Send_reset
* Returned Values : None.
* Comments        :
*
*  Send an unsolicited reset (RST) segment.
*
*END*-----------------------------------------------------------------*/

void TCP_Send_reset
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   TCP_HEADER       *seg;
   RTCSPCB_PTR       pcb;
   uint32_t           err;

   pcb = RTCSPCB_alloc_send(); /* get the packet */
   if ( pcb == NULL ) {
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_MISSED++);
      RTCS_log_error( ERROR_TCP,
                           RTCSERR_TCP_SR_OUT_OF_PCBS,
                           (uint32_t)tcb,
                           0, 0 );
      return;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb);
   RTCSPCB_insert_header(pcb, sizeof(TCP_HEADER));
   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_TCP), 0);

   /*  Fill header...  even simpler than TCP_Send_reply_reset()...
    */
   seg = (TCP_HEADER *)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_TRANSPORT(pcb) = RTCSPCB_DATA(pcb);
   mqx_htons(seg->source_port, tcb->local_port);
   mqx_htons(seg->dest_port,   tcb->remote_port);
   mqx_htons(seg->flags,       RST | ( (sizeof(TCP_HEADER)>>2) << DATAOFS_SHIFT ));
   mqx_htonl(seg->seq,         tcb->sndnxt);
   mqx_htonl(seg->ack,         0);
   mqx_htons(seg->window,      0);
   mqx_htons(seg->checksum,    0);
   mqx_htons(seg->urgent,      0);

   /* Just get the TX option from the TCB, as it is always stored there now */
   pcb->LINK_OPTIONS.TX = tcb->TX;

   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_TX_RESET++);
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_TOTAL++);
   
   
   
  // err = TCP_Send_IP_packet(pcb,                   /* packet to send */
  //                          tcb->local_host,       /* local address */
  //                          tcb->remote_host,      /* dest address */
  //                          tcb->bypass_tx_chksum  /* bypass checksum? */
  //                         );
   
#if RTCSCFG_ENABLE_IP6     
   if(tcb->af == AF_INET6)
   { 
        err = TCP6_Send_IP_packet(   pcb,                   /* packet to send */
                                    &tcb->local_ipv6_host,       /* local address */
                                    &tcb->remote_ipv6_host,      /* dest address */
                                    tcb->bypass_tx_chksum  /* bypass checksum? */
                                 );
   }
   else
#endif
#if RTCSCFG_ENABLE_IP4
   if(tcb->af == AF_INET)
   {
   
        err = TCP_Send_IP_packet(   pcb,                   /* packet to send */
                                    tcb->local_host,       /* local address */
                                    tcb->remote_host,      /* dest address */
                                    tcb->bypass_tx_chksum  /* bypass checksum? */
                                );
   } 
   else
#endif 
   {}
   
   if ( err != RTCS_OK ) {
      /* Note that the PCB would have already been freed */
      tcb->lasterror = err;
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_DISCARDED++);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Send_keepalive
* Returned Values : None.
* Comments        :
*     Send a keepalive probe.
*
*END*-----------------------------------------------------------------*/

void TCP_Send_keepalive
   (
      TCB_STRUCT_PTR       tcb,     /* IN - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN - TCP layer data */
   )
{ /* Body */

   int32_t  result;

   tcb->keepalive = TRUE;
   TCP_Timer_oneshot_max(  &tcb->sndacktq.t, /* the timer */
                           tcb->keepaliveto, /* the timeout value */
                           tcp_cfg->qhead ); /* the timer queue head */

   if (!tcb->sndtohard) {

      TCP_Timer_stop(&tcb->sndtq.t, tcp_cfg->qhead);       /* ACK timeout */


      if ( tcb->sndto_2 != 0 ) {        /* start hard timeout... */
         result =
            TCP_Timer_start( &tcb->sndtq.t,/* the timer */
                        (tcb->sndto_2+tcb->sndto_1),     /* the timeout value */
                        0,                /* no reload */
                        tcp_cfg->qhead ); /* the timer queue head */
         if ( result != RTCS_OK ) {
            RTCS_log_error( ERROR_TCP, RTCSERR_TCP_TIMER_FAILURE,
               (uint32_t)result, (uint32_t)tcb, 1 );
         } /* Endif */
         tcb->sndtohard = TRUE;
      } /* Endif */

   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Transmit
* Returned Values : None.
* Comments        :
*
*  See if we can transmit anything, if so, transmit as much as we are
*   allowed.
*
*  The Send request will be returned to the upper layer
*   if the packet cannot be sent.  The PCB would also be freed.  If the
*   packet was passed to ENET, the Send request should be returned on
*   receipt of the ACK in TCP_Process_ack.
*
*  If `retransmit' is true, the (re)transmission pointer is reset to the
*   beginning of unacknowledged data in the send buffer, so that it will
*   get retransmitted (if we send any data);
*
*  Guarantees to send at least one packet if `force' is TRUE.
*
*END*-----------------------------------------------------------------*/

void TCP_Transmit
   (
      TCB_STRUCT_PTR       tcb,        /* IN/OUT - TCP layer context */
      TCP_CFG_STRUCT_PTR   tcp_cfg,    /* IN/OUT - TCP layer constants */
      bool              retransmit, /* IN     - Should reset trans. ptr? */
      bool              force       /* IN     - Guarantee send? */
   )
{ /* Body */
   uint32_t  xmit;
   int32_t   window,
            cansend,
            tosend;
   bool  sent = FALSE, probe;
   int32_t   result;
   
   RTCSLOG_FNE5(RTCSLOG_FN_TCP_Transmit, tcb, tcp_cfg, retransmit, force);

   switch ( tcb->state ) {   /* don't transmit in these states */
      case LISTEN:
      case CLOSED:
         RTCSLOG_FNX2(RTCSLOG_FN_TCP_Transmit, -1);
         return;

      default:
         break;
   } /* End Switch */

   probe = (tcb->sndwnd == 0        &&
            tcb->state != FINWAIT_1 &&
            tcb->state != FINWAIT_2 &&
            tcb->state != TIME_WAIT &&
            tcb->state != CLOSING   &&
            tcb->state != LAST_ACK);
   if (probe) {
      retransmit = TRUE;
   } /* Endif */

   /*  Where to start transmitting...
    */
   if ( retransmit || GT32(tcb->snduna, tcb->sndxmit) ) {
      tcb->sndxmit = tcb->snduna;
   } /* Endif */

   /*  Compute effective send window; note that tcb->sndcwnd >= tcb->sndwnd
    */
   window = tcb->sndcwnd;
   if ( window > tcb->sndwnd ) {
      window = tcb->sndwnd;
   } /* Endif */

   while (1) {

      xmit = tcb->sndxmit;

      /*
      ** Check if we should be sending data now.  Send if we have a full
      **    packet or if there is PUSHed data to send.  If not, break out
      **    of the loop.
      **
      ** Also check if we are waiting to send a FIN, in which case all
      **      the data must go when possible.
      */
      tosend = tcb->sndbufseq + tcb->sndlen - xmit;
      if ( tosend < tcb->sndmax               &&  /* not full packet AND */
         ((tcb->status & TCPS_TPUSH) == 0 ||      /* (not pushing OR     */
          LE32(tcb->sndpush, xmit))           &&  /*  pushed data already sent) AND */
         (tcb->status & TCPS_FINTOSEND) == 0)     /* not waiting to send FIN */
      {
        
         break;
      } /* Endif */


      /*
      ** Check to see if we should be probing.  Note that
      **    the same time queue is used in TIME_WAIT state,
      **    so ensure we don't start this xmit timer if
      **    we're in TIME_WAIT state (happens when ACK is
      **    sent for received FIN).
      */
      if (probe)
      {   /* probing */
         if ( tcb->sndxmitwhat != TCP_PROBE ) {

            tcb->sndxmitwhat = TCP_PROBE;
            tcb->sndprobeto = tcb->sndrto;
            result =
               TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                           tcb->sndprobeto,     /* the timeout value */
                           0,                   /* no reload */
                           tcp_cfg->qhead );    /* the timer queue head */
            if ( result != RTCS_OK ) {
               RTCS_log_error( ERROR_TCP,
                                    RTCSERR_TCP_TIMER_FAILURE,
                                    (uint32_t)result,
                                    (uint32_t)tcb,
                                    4 );
            } /* Endif */

            /* start user timeout timer */
            tcb->sndtohard = FALSE;
            result =
               TCP_Timer_start( &tcb->sndtq.t,       /* the timer */
                           tcb->sndto_1,        /* the timeout value */
                           0,                   /* no reload */
                           tcp_cfg->qhead );    /* the timer queue head */
            if ( result != RTCS_OK ) {
               RTCS_log_error( ERROR_TCP,
                                    RTCSERR_TCP_TIMER_FAILURE,
                                    (uint32_t)result,
                                    (uint32_t)tcb,
                                    5 );
            } /* Endif */

         } /* Endif */
         break;
      } /* Endif */
      
       if (tcb->sndlen <= 0) {                   /* nothing to send */
         break;
      } /* Endif */

      cansend = tcb->snduna + window - xmit;
      if ( cansend <= 0 ) { /* nothing to send */
         break;
      } /* Endif */

      if ( tosend < cansend ) {  /* can't send more than we have */
         cansend = tosend;
      } /* Endif */

      /*
      ** See RFC1122 4.2.3.4, Page 99
      */

      if ( cansend < tcb->sndmax ) {   /* not a full packet... */

         /*  If Nagle algorithm is in effect, only send less-than-full
          *   packet when there is no sent data outstanding:
          */
         if ( (tcb->options & TCPO_NONAGLE) == 0 &&
            tcb->sndnxt != tcb->snduna)
         {
            break;
         } /* Endif */

         /*
         ** If we can't send everything, nor send half the biggest send
         **   window we ever had, delay before sending; this
         **   is for sender SWS (Silly Window Syndrome) avoidance.
         */
         if ( tosend > cansend                &&
            cansend < (tcb->sndwndmax >> 1) &&
            ! force)
         {

            tcb->keepalive = FALSE;
            TCP_Timer_oneshot_max(  &tcb->sndacktq.t, /* the timer */
                              TCP_SWSDELAY,     /* the timeout */
                              tcp_cfg->qhead ); /* the timer queue head */
            sent = FALSE;   /* avoid stopping sndacktq */
            break;

         } /* Endif */

      } /* Endif */

      TCP_Send_packet(tcb, xmit, tcp_cfg);
      sent = TRUE;

      if ( tcb->sndxmit == xmit ) {
         break;
      } /* Endif */

   } /* Endwhile */

   /* Send a packet anyway if we must... */
   if ( force && ! sent ) {

      if ((tcb->sndlen == 0) && tcb->keepalive && (tcb->state == ESTABLISHED)) {
         /* decrement sequence number for keepalive probes */
         TCP_Send_packet(tcb, tcb->sndxmit-1, tcp_cfg);
      } else {
         TCP_Send_packet(tcb, tcb->sndxmit, tcp_cfg);
      } /* Endif */
      sent = TRUE;
   } /* Endif */

   if ( sent ) {

      /*  If packet sent, cancel sndacktq (no need to send ack, we sent it),
       *   or restart keepalive timeout if there is one...
       */
      (void)TCP_Timer_stop(&tcb->sndacktq.t, tcp_cfg->qhead);
      if ( tcb->keepaliveto != 0 ) {
         TCP_Send_keepalive(tcb, tcp_cfg);
      } /* Endif */

      /*  Also, if we are probing and a packet was sent, we can restart
       *   the probe timer...
       */
      if ( tcb->sndxmitwhat == TCP_PROBE &&
         tcb->state != TIME_WAIT)
      {
         result =
            TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                        tcb->sndprobeto,     /* the timeout value */
                        0,                   /* no reload */
                        tcp_cfg->qhead );    /* the timer queue head */
         if ( result != RTCS_OK ) {
            RTCS_log_error( ERROR_TCP,
                                 RTCSERR_TCP_TIMER_FAILURE,
                                 (uint32_t)result,
                                 (uint32_t)tcb,
                                 6 );
         } /* Endif */
      } /* Endif */

   } /* Endif */

   /* Now transmitting; if rto timer not started, start it */
   if ( tcb->sndnxt != tcb->snduna    &&
      tcb->sndxmitwhat != TCP_XMIT  &&
      tcb->state != TIME_WAIT)
   {

      tcb->sndxmitwhat = TCP_XMIT;
      result =
         TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                     tcb->sndrto,         /* the timeout value */
                     0,                   /* no reload */
                     tcp_cfg->qhead );    /* the timer queue head */
      if ( result != RTCS_OK ) {
         RTCS_log_error( ERROR_TCP,
                              RTCSERR_TCP_TIMER_FAILURE,
                              (uint32_t)result,
                              (uint32_t)tcb,
                              7 );
      } /* Endif */

      /* start user timeout timer */
      tcb->sndtohard = FALSE;
      result =
         TCP_Timer_start( &tcb->sndtq.t,       /* the timer */
                     tcb->sndto_1,        /* the timeout value */
                     0,                   /* no reload */
                     tcp_cfg->qhead );    /* the timer queue head */
      if ( result != RTCS_OK ) {
         RTCS_log_error( ERROR_TCP,
                              RTCSERR_TCP_TIMER_FAILURE,
                              (uint32_t)result,
                              (uint32_t)tcb,
                              8 );
      } /* Endif */

   } /* Endif */
   
   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Transmit, 0);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Start_sending
* Returned Values : None.
* Comments        :
*
*  Called when we start sending anything on a connection...
*
*END*-----------------------------------------------------------------*/

void TCP_Start_sending
   (
      TCB_STRUCT_PTR          tcb,     /* IN/OUT - TCP layer context */
      TCP_CFG_STRUCT_PTR      tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   uint32_t iss;

   /*  Select an ISN (Initial send Sequence Number)
   **
   ** RFC 1122, section 4.2.2.9 requires TCP to have an ISN bound to a
   ** 32-bit (pseudo-)clock which is incremented every 4 microseconds.
   **
   ** However, this makes TCP vulnerable to ISN-prediction attacks.
   ** Instead, we pick a random ISN.
   **
   */

   iss = RTCS_rand();

   tcb->sndringtail = 0;
   tcb->sndringhead = 0;
   tcb->sndringavail = tcb->sndringlen;

   tcb->sndiss    = iss;
   tcb->snduna    = iss;
   tcb->sndnxt    = iss + 1;
   tcb->sndxmit   = iss;
   tcb->sndbufseq = iss + 1;
   tcb->sndwl2    = iss;
   tcb->sndxmitwhat = TCP_IDLE;
   TCP_Transmit(tcb, tcp_cfg, FALSE, TRUE);    /* Send SYN segment... */

} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_send
* Returned Values : None.
* Comments        :
*
*  Process TcpSend() requests.
*
*END*-----------------------------------------------------------------*/
void TCP_Process_send
   (
      TCP_PARM_PTR      req      /* IN/OUT - upper layer Send request */
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;    /* TCP layer data */
   TCB_STRUCT_PTR       tcb;        /* TCP connection */
   SbufNode            *buf;
   bool              block = FALSE; /* will client block? */
   uint32_t              reply_buflen;
   int32_t               result;
   uint32_t              status;

   tcp_cfg = RTCS_getcfg(TCP);
   tcb = req->TCB_PTR;
   reply_buflen = 0;

   if ( tcb == NULL ) { 
      TCP_PS_REPLY(RTCSERR_TCP_CONN_RLSD); 
   } 

   /* if ( tcb->VALID != TCB_VALID_ID || tcb->usecount <= 0 ) { */
   if ( tcb->VALID != TCB_VALID_ID ) {
      TCP_PS_REPLY(RTCSERR_TCP_CONN_RLSD);
   } /* Endif */

   if ( (tcb->status & TCPS_FINTOSEND) != 0 ) {
      TCP_PS_REPLY(RTCSERR_TCP_CONN_CLOSING);
   } /* Endif */

   switch ( tcb->state ) {

      case LISTEN:
      case SYN_SENT:
      case SYN_RECEIVED:
      case ESTABLISHED:
      case CLOSE_WAIT:

         DEBUGTCP(printf("\nTCP: PrcSnd: sending, flags = %x, sndringavail = %d",req->OPTIONS,tcb->sndringavail));

         if ( (req->OPTIONS & (TCPO_WAIT | TCPO_NOWAIT)) ==
            (TCPO_WAIT | TCPO_NOWAIT))
         {
            TCP_PS_REPLY(RTCSERR_TCPIP_INVALID_ARGUMENT);
         } /* Endif */

         result = RTCS_OK;

         if ( req->BUFF_LENG != 0 )
         {
            /*  Send requester has 3 options:
             *   - block until all data sent & ack'ed (TCPO_WAIT)
             *   - buffer the data and return immediately, or
             *      block if buffer not large enough (until all data
             *      are buffered) (default)
             *   - buffer as much data as possible and return
             *      immediately without blocking (TCPO_NOWAIT)
             */
            
            /*  Block if no available buffer space:
             */
            if ( (req->OPTIONS & TCPO_WAIT) != 0 )
            {
               block = TRUE;
            }
            else
            { 
               /*  Just queue as much data as possible without blocking...
                */
                if ( req->BUFF_LENG > tcb->sndringavail )
                {
                    req->BUFF_LENG = tcb->sndringavail;
                    if ( req->BUFF_LENG == 0 )
                    {
                        goto emptysend;
                    } 
                    DEBUGTCP(printf("\nTCP: PrcSnd: sending only %d bytes",req->BUFF_LENG));
                } 
            } 

            /*  We now have unacknowledged data being sent
             */
            tcb->status &= ~TCPS_TACKED;

            if ( tcp_cfg->SbufNodefree != NULL )
            {
                buf = tcp_cfg->SbufNodefree;
                tcp_cfg->SbufNodefree = buf->next;
            } 
            else
            {
                buf = (SbufNode *)_mem_alloc_zero(sizeof(SbufNode));
            }

            if ( buf == NULL )
            {
                     RTCS_log_error( ERROR_TCP,
                                          RTCSERR_TCP_PS_FAILED_GET_SBUF,
                                          (uint32_t)tcb,
                                          0, 0);
                     TCP_PS_REPLY(RTCSERR_OUT_OF_MEMORY); /* return */ 
            } 
            
            _mem_set_type(buf, MEM_TYPE_SbufNode);

            buf->next     = NULL;
                  
            if ( block ) 
            {
                buf->data      = req->BUFFER_PTR;
                buf->size      = req->BUFF_LENG;
                buf->req       = req;
                buf->use_cbuf  = TRUE;

                DEBUGTCP(printf("\nTCP: PrcSnd: [blocking] new node of size %d at %x.", buf->size, buf->data));                  
            }
            else
            {                    
                buf->data     = (unsigned char *) tcb->sndringhead;
                buf->size     = req->BUFF_LENG;
                buf->req      = NULL;
                buf->use_cbuf = FALSE;

                DEBUGTCP(printf("\nTCP: PrcSnd: [non-blk] new node of size %d at %x.", buf->size, buf->data));

                /*
                ** Copy client data to send buffer
                */
                status = TCP_Copy_to_ring( req->BUFFER_PTR,  /* source data buffer */
                                    req->BUFF_LENG,   /* data length */
                                    tcb->sndringhead, /* where in ring to put data */
                                    tcb->sndringbuf,  /* start of ring buffer */
                                    tcb->sndringlen );/* total length of ring buff */

                if ( status != RTCS_OK )
                {
                  TCP_PS_REPLY( status );
                }

                tcb->sndringhead = (tcb->sndringhead + req->BUFF_LENG) %
                                   tcb->sndringlen;

                tcb->sndringavail -= req->BUFF_LENG;
                if ( tcb->sndringavail == 0 )
                {
                    tcb->status &= ~TCPS_TREADY;
                    DEBUGTCP(printf("\nTCP: PrcSnd: sndringavail is 0."));
                }

                reply_buflen = req->BUFF_LENG;

            }  /* blocking or non-blocking */
            
            
            
            /*
            ** If new node, insert it into send buffer
            */
            if ( buf != NULL ) {
               if ( tcb->sndbuftail != NULL ) {
                  tcb->sndbuftail->next = buf;
               } else {
                  tcb->sndbuf = buf;
               } /* Endif */
               tcb->sndbuftail = buf;
            } /* Endif */

            tcb->sndlen += req->BUFF_LENG;

         } /* buff len NE 0 */

   emptysend:

         DEBUGTCP(printf("\nTCP: PrcSnd: at emptysend"));
         if ( (req->OPTIONS & TCPO_URGENT) != 0 ) {
            tcb->sndup = tcb->sndbufseq + tcb->sndlen;
            TCP_Event(tcb, TCPS_TURGENT);
         } /* Endif */

         if ( (req->OPTIONS & TCPO_PUSH) != 0 ) {
            tcb->sndpush = tcb->sndbufseq + tcb->sndlen;
            TCP_Event(tcb, TCPS_TPUSH);
         } /* Endif */

         TCP_Transmit(tcb, tcp_cfg, FALSE, FALSE); /* Returns request if error */

         /*
         ** NORMAL CASE RETURN
         */
         if ( block ) {
            return;
         } /* Endif */

         TCP_PS_REPLY(RTCS_OK);

      case FINWAIT_1:
      case FINWAIT_2:
      case CLOSING:
      case LAST_ACK:
      case TIME_WAIT:
         TCP_PS_REPLY(RTCSERR_TCP_SHUTDOWN);

      case CLOSED:
         TCP_PS_REPLY(RTCSERR_TCP_CONN_ABORTED);

      default:
         TCP_PS_REPLY(RTCSERR_TCP_NOT_CONN);

   } /* End Switch */

   /*
   ** TCP_PS_REPLY sends you here.
   */
   reply_send:

      /*
      ** Update number of bytes sent
      */
      req->BUFF_LENG = reply_buflen;
      RTCSCMD_complete(req, result);
      return;

}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_ack
* Returned Values : None.
* Comments        :
*
*  Process an ACK;
*   assumes that the given segment indeed contains a valid ACK,
*   and that seg->ack >= tcb->snduna
*
*END*-----------------------------------------------------------------*/

void TCP_Process_ack
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_HEADER      *seg,     /* IN     - packet header */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   SbufNode      *buf;
   SndClock      *clk;
   int32_t         result;
   uint32_t        ack;
   uint32_t        curr_time;

   ack = mqx_ntohl(seg->ack);

   /* Return if no new data is acked */
   if ( ack == tcb->snduna ) {

      /* But if we are zero-window probing, restart send timeout */
      if ( tcb->sndwnd == 0 && tcb->sndlen > 0 ) {
         tcb->sndtohard = FALSE;
         result =
            TCP_Timer_start( &tcb->sndtq.t,       /* the timer */
                        tcb->sndto_1,        /* the timeout value */
                        0,                   /* no reload */
                        tcp_cfg->qhead );    /* the timer queue head */
         if ( result != RTCS_OK ) {
            RTCS_log_error( ERROR_TCP,
                                 RTCSERR_TCP_TIMER_FAILURE,
                                 (uint32_t)result,
                                 (uint32_t)tcb,
                                 9 );
         } /* Endif */

      /* If there is no data in transit, stop the hard timeout */
      } else if (tcb->sndnxt == tcb->snduna) {

         tcb->sndtohard = FALSE;
         TCP_Timer_stop(&tcb->sndtq.t, tcp_cfg->qhead);

      } /* Endif */
      return;

   } /* Endif */


   /*  Thing are getting through; positive advice to IP layer
   ** IP_Advise_delivery_problem( 1, tcb->icb, tcb->rem_host, tcb->tos );
   */

   /*  Update congestion window re: Jacobson slow-start/cong. avoidance
    */
   if ( tcb->sndcwnd < tcb->sndstresh ) {
      /*  slow-start: open window exponentially
       */
      tcb->sndcwnd += tcb->sndmax;

   } else {
      /*  congestion avoidance: open window linearly;
       */
      if (tcb->sndcwnd == 0) {
         tcb->sndcwnd =1;
      } else if (tcb->sndcwnd > 0xffff) {
         tcb->sndcwnd += ( tcb->sndmax * tcb->sndmax ) /  0xffff;
      } else { 
         tcb->sndcwnd += ( tcb->sndmax * tcb->sndmax ) /
                         (uint16_t)tcb->sndcwnd;
      }
      
      /* Keep it within 16 bits */
      if ( tcb->sndcwnd > TCP_WINDOW_MAX ) {
         tcb->sndcwnd = TCP_WINDOW_MAX;
      } /* Endif */

   } /* Endif */

   /* Keep track of maximum for debugging */
   if ( tcb->sndcwnd > tcb->sndcwndmax ) {
      tcb->sndcwndmax = tcb->sndcwnd;
   } /* Endif */

   /*  New data is acked
    */
   tcb->snduna = ack;

   /*  Remove all send buffers thus acknowledged;
    *   return to client if necessary
    */
   buf = tcb->sndbuf;
   while ( buf != NULL &&
         (GE32(ack, tcb->sndbufseq + buf->size)))
   {

      tcb->sndbufseq += buf->size;
      tcb->sndlen    -= buf->size;
      tcb->sndbuf     = buf->next;
      if ( tcb->sndbuf == NULL ) {
         tcb->sndbuftail = NULL;
      } /* Endif */

      if ( buf->use_cbuf ) {

         /*
         ** Return to client, all its data was sent successfully
         */
         if ( buf->req != NULL ) {

            buf->req->BUFF_LENG  = buf->size;
            RTCSCMD_complete(buf->req, tcb->send_error);

            /*
            ** Clear the last global send error,
            */
            tcb->send_error = RTCS_OK;

         } /* Endif */

      } else {

         { /* Scope */
               register int32_t   new_avail,
                              len_thresh;

            /*
            ** No client, so the data comes from the circular send buffer;
            **   return storage to buffer.
            **  Assuming buf->data EQ tcb->sndringbuf + tcb->sndringtail
            **   because storage is returned in the same order it was taken.
            */
            if ( (buf->size + tcb->sndringavail) > tcb->sndringlen ) {

               /*
               ** This should never happen. Try to recover by setting
               **    the entire buffer free.
               */
               buf->size = tcb->sndringlen - tcb->sndringavail;
               RTCS_log_error( ERROR_TCP,
                                    RTCSERR_TCP_PA_BUFF_CORRUPT,
                                    buf->size,
                                    tcb->sndringavail,
                                    (uint32_t)tcb );

            } /* Endif */

            tcb->sndringtail = (tcb->sndringtail + buf->size) %
                                 tcb->sndringlen;
            new_avail  = tcb->sndringavail + buf->size;
            len_thresh = tcb->sndringlen >> 1;

            /*  If at least half of the send buffer becomes available,
             *   signal the client that it can send more data;
             *   the event is only triggered when a certain buffer-size
             *   threshold (half sndringlen) is crossed.
             */
            if ( tcb->sndringavail < len_thresh &&
               new_avail >= len_thresh
            ) {
               TCP_Event(tcb, TCPS_TREADY);
            } /* Endif */
            tcb->sndringavail = new_avail;

         } /* Endscope */

      } /* Endif */

      _mem_free(buf);

      if ( tcb->sndlen == 0 ) {
         /*
         ** Notify socket layer in case some task is blocked on select()
         ** waiting for this data.
         */
         _SOCK_select_signal(tcb->SOCKET, tcb->state, 1);
        
         TCP_Event(tcb, TCPS_TACKED);   /* all data to send got acked */
      } /* Endif */

      buf = tcb->sndbuf;

   } /* Endwhile */

   /*  Update pointers to things being sent, if they got ACKed
    */
   if ( (tcb->status & TCPS_TURGENT) && GE32(ack, tcb->sndup) ) {
      tcb->status &= ~TCPS_TURGENT;
   } /* Endif */

   if ( (tcb->status & TCPS_TPUSH) && GE32(ack, tcb->sndpush) ) {
      tcb->status &= ~TCPS_TPUSH;
   } /* Endif */

   /*  New data was acked, we may be able to do a round-trip-time measurement:
    */
   curr_time = RTCS_time_get();
   clk = TCP_Process_RTT_measure( tcb, (int32_t)curr_time);

   /*  Adjust/restart xmittq and sndtq timers...
    */
   if ( GT32( tcb->sndnxt, tcb->snduna) ) {   /* data in transit remaining */

 

         int32_t   to;

         /* restart send timeout */
         tcb->sndtohard = FALSE;
         result =
            TCP_Timer_start( &tcb->sndtq.t,       /* the timer */
                        tcb->sndto_1,        /* the timeout value */
                        0,                   /* no reload */
                        tcp_cfg->qhead );    /* the timer queue head */
         if ( result != RTCS_OK ) {
            RTCS_log_error( ERROR_TCP,
                                 RTCSERR_TCP_TIMER_FAILURE,
                                 (uint32_t)result,
                                 (uint32_t)tcb,
                                 10 );
         } 

         /*  Advance retransmission timeout, as if it had been started
          *   when earliest unacked packet was sent; if no transmission time
          *   is available for any previously sent (unacked) packet, just
          *   restart the rto as if we had just transmitted a packet...
          */
         to = tcb->sndrto;
         if ( clk != NULL ) {
            to += clk->time - (int32_t)curr_time;
            if ( to <= 0 ) {
               to = TICK_LENGTH;          /* minimum timeout */
            } /* Endif */
         } /* Endif */
         result =
            TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                        to,                  /* the timeout value */
                        0,                   /* no reload */
                        tcp_cfg->qhead );    /* the timer queue head */
         if ( result != RTCS_OK ) {
            RTCS_log_error( ERROR_TCP,
                                 RTCSERR_TCP_TIMER_FAILURE,
                                 (uint32_t)result,
                                 (uint32_t)tcb,
                                 11 );
         }



   } else {  /* no more data in transit */

      tcb->sndtohard = FALSE;
      (void)TCP_Timer_stop(&tcb->sndtq.t, tcp_cfg->qhead);       /* ACK timeout */

      (void)TCP_Timer_stop(&tcb->sndxmittq.t, tcp_cfg->qhead);   /* Rexmt timeout */
      tcb->sndxmitwhat = TCP_IDLE;

   } /* Endif */

   /*  There may be more data to transmit;
    *   also, windows may have increased, we may be able to transmit more data;
    *   so transmit what we can...
    */
   TCP_Transmit(tcb, tcp_cfg, FALSE, (tcb->sndbufseq + tcb->sndlen - tcb->sndxmit) > 0);


}

/* EOF */
