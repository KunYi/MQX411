/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the S-Record decoder for the
*   Exec function of the RTCS Communication Library.
*
*
*END************************************************************************/

#include <rtcs.h>


/* The S-Record decoder information to pass to RTCS_load() */
uint32_t SREC_start  (void *ff_data);
uint32_t SREC_decode (uint32_t size, unsigned char *data);
uint32_t SREC_eod    (void);
void    SREC_exec   (void);

static FF_CALL_STRUCT ff_srec = {
   SREC_start,
   SREC_decode,
   SREC_eod,
   SREC_exec
};

const FF_CALL_STRUCT_PTR FF_SREC = &ff_srec;


#define NEXT_STATE_BREAK(s)  \
      SREC_config.state = s; \
      break;

#define NEXT_STATE_FALLTHROUGH(s) \
      if (!size) {                \
         SREC_config.state = s;   \
         break;                   \
      } /* Endif */               \
      size--;                     \
      c = mqx_ntohc(data);            \
      data++;


#define SREC_HI_CHAR(c,n) \
           if ((c >= '0') && (c <= '9')) n = (c-'0'+ 0) << 4; \
      else if ((c >= 'A') && (c <= 'F')) n = (c-'A'+10) << 4; \
      else if ((c >= 'a') && (c <= 'f')) n = (c-'a'+10) << 4; \
      else return RTCSERR_SREC_CHAR;

#define SREC_LO_CHAR(c,n) \
           if ((c >= '0') && (c <= '9')) n += (c-'0'+ 0); \
      else if ((c >= 'A') && (c <= 'F')) n += (c-'A'+10); \
      else if ((c >= 'a') && (c <= 'f')) n += (c-'a'+10); \
      else return RTCSERR_SREC_CHAR;


/*
** The state diagram is:
**
**      -------------------------
**     (                       ) \
**  -----> s -> l -> a ---> d -   )
**                      \        /
**                       -> i -----> end
**
** S0,S5    records follow s-l-a-i-s
** S1,S2,S3 records follow s-l-a-d-s
** S7,S8,S9 records follow s-l-a-i-end
**
*/
typedef enum {
   hex_shi, hex_slo, hex_lhi, hex_llo, hex_ahi, hex_alo,
   hex_dhi, hex_dlo, hex_ihi, hex_ilo, hex_end
} SREC_state;


/* S-Record decoder state information */
static struct {
   SREC_state     state;   /* the current state */
   SREC_state     dstate;  /* the state that follows a */
   SREC_state     estate;  /* the state that follows d or i */
   uint32_t        addr;    /* the address field */
   unsigned char          data;    /* one data byte */
   unsigned char          sum;     /* one's complement checksum */
   unsigned char          alen;    /* size of the address field */
   unsigned char          dlen;    /* length of the S-Record */
} SREC_config;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SREC_start
* Returned Value   : error code
* Comments  :  Routine to begin decoding a HEX file.
*
*END*-----------------------------------------------------------------*/

uint32_t SREC_start
   (
      void    *ff_data
   )
{ /* Body */

   SREC_config.state = hex_shi;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SREC_decode
* Returned Value   : error code
* Comments  :  Routine to decode S-Records.
*
*  SREC_decode is implemented as a finite state machine.
*
*  All S-Records begin at state hex_shi.  Once a 'S' is parsed we
*  go to state hex_slo.
*
*  In state hex_slo we determine which type of S-Record we have,
*  and we set the following fields in SREC_config:
*
*   - alen is the size of the address field:  4 for S3 and S7,
*     3 for S2 and S8, and 2 for all others.
*
*   - dstate is the state to go to after the address and length
*     fields are parsed.  S1,S2,S3 go to state hex_dhi/hex_dlo
*     (which reads bytes and writes them to memory).  All other
*     record types go to state hex_ihi/hex_ilo (which validates
*     the checksum, but otherwise ignores all data bytes).
*
*   - estate is the state to go to after all data is parsed and
*     the checksum is validated.  S7,S8,S9 go to state hex_end.
*     All other record types return to state hex_shi/hex_slo to
*     parse another S-Record.
*
*  State hex_slo is followed by hex_lhi and hex_llo, which read
*  the one-byte length field of the S-Record, and store it in
*  the dlen field of SREC_config.
*
*  States hex_ahi and hex_alo are then repeated alen times to
*  read the address field of the S-Record.  The address gets
*  stored in the addr field of SREC_config.  Additionally, dlen
*  is decremented by alen, since dlen includes the address field.
*
*  Next, control is passed either to hex_dhi/hex_dlo or to hex_ihi/
*  hex_ilo, depending on the contents of dstate determined in state
*  hex_slo.  The dstate pair (hex_dhi/hex_dlo or hex_ihi/hex_ilo)
*  will be repeated dlen times.  On the last time through (when
*  dlen == 1), the checksum is validated by comparing it to 0xFF.
*  (The checksum is actually incremented and compared to 0.)
*
*  Once the checksum is validated, control is passed either to
*  hex_shi or to hex_end, depending on the contents of estate
*  determined in state hex_slo.  hex_shi, of course, returns to
*  the beginning of the state machine and parses the next S-Record.
*  In the case of a S7, S8 or S9 record, though, control goes to
*  state hex_end.
*
*  State hex_end accepts only whitespace, since no other S-Records
*  are expected, and this state must be reached in order for the
*  decoding to be successful.
*
*
*  Four code macros are used throughout this function.
*
*     SREC_HI_CHAR converts an ASCII character to the high
*     nybble of a byte.
*
*     SREC_LO_CHAR converts an ASCII character to the low
*     nybble of a byte.
*
*     NEXT_STATE_FALLTHROUGH is used whenever one state is always
*     succeeded by the same state.  It allows control to be passed
*     to the next state without breaking out of the switch statement,
*     testing the while condition, and branching back into the switch
*     statement.
*
*     When NEXT_STATE_FALLTHROUGH cannot be used, NEXT_STATE_BREAK is
*     used to change states.  It uses the more conventional method,
*     i.e. a break statement.
*
*END*-----------------------------------------------------------------*/

uint32_t SREC_decode
   (
      uint32_t size,
         /* [IN] number of bytes to decode */
      unsigned char *data
         /* [IN] bytes to decode */
   )
{ /* Body */
   unsigned char c;

   while (size--) {
      c = mqx_ntohc(data);
      data++;
      switch (SREC_config.state) {
         /*
         ** Parse 'S' followed by an ASCII digit.
         */
      case hex_shi:
         if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') || (c == '\0')) break;
         if ((c != 'S') && (c != 's')) return RTCSERR_SREC_CHAR;
         NEXT_STATE_FALLTHROUGH(hex_slo)

      case hex_slo:
         switch (c) {

         case '0':
         case '5':
            SREC_config.alen   = 2;
            SREC_config.dstate = hex_ihi;
            SREC_config.estate = hex_shi;
            break;

         case '1':
         case '2':
         case '3':
            SREC_config.alen   = c-'1'+2;
            SREC_config.dstate = hex_dhi;
            SREC_config.estate = hex_shi;
            break;

         case '7':
         case '8':
         case '9':
            SREC_config.alen   = 2+'9'-c;
            SREC_config.dstate = hex_ihi;
            SREC_config.estate = hex_end;
            break;

         default:
            return RTCSERR_SREC_RECORD;
         } /* Endswitch */
         NEXT_STATE_FALLTHROUGH(hex_lhi)

         /*
         ** Parse the length and address fields.
         */
      case hex_lhi:
         SREC_HI_CHAR(c,SREC_config.dlen)
         NEXT_STATE_FALLTHROUGH(hex_llo)

      case hex_llo:
         SREC_LO_CHAR(c,SREC_config.dlen)
         if (SREC_config.dlen <= SREC_config.alen) return RTCSERR_SREC_SHORT;
         SREC_config.addr  = 0;
         SREC_config.sum   = SREC_config.dlen;
         SREC_config.dlen -= SREC_config.alen;
         NEXT_STATE_FALLTHROUGH(hex_ahi)

      case hex_ahi:
         SREC_HI_CHAR(c,SREC_config.data)
         NEXT_STATE_FALLTHROUGH(hex_alo)

      case hex_alo:
         SREC_LO_CHAR(c,SREC_config.data)
         SREC_config.addr <<= 8;
         SREC_config.addr  += SREC_config.data;
         SREC_config.sum   += SREC_config.data;
         if (--SREC_config.alen) {
            NEXT_STATE_BREAK(hex_ahi)
         } else {
            NEXT_STATE_BREAK(SREC_config.dstate)
         } /* Endif */

         /*
         ** Parse S1,S2,S3 data:
         ** write data bytes to memory, and verify the checksum.
         */
      case hex_dhi:
         SREC_HI_CHAR(c,SREC_config.data)
         NEXT_STATE_FALLTHROUGH(hex_dlo)

      case hex_dlo:
         SREC_LO_CHAR(c,SREC_config.data)
         SREC_config.sum += SREC_config.data;
         if (!--SREC_config.dlen) {
            if (++SREC_config.sum & 0xFF) return RTCSERR_SREC_CHECKSUM;
            NEXT_STATE_BREAK(SREC_config.estate)
         } /* Endif */
         *(unsigned char *)SREC_config.addr = SREC_config.data;
         SREC_config.addr++;
         NEXT_STATE_BREAK(hex_dhi)

         /*
         ** Parse S0,S5,S7,S8,S9 data:
         ** ignore data bytes, but verify the checksum.
         */
      case hex_ihi:
         SREC_HI_CHAR(c,SREC_config.data)
         NEXT_STATE_FALLTHROUGH(hex_ilo)

      case hex_ilo:
         SREC_LO_CHAR(c,SREC_config.data)
         SREC_config.sum += SREC_config.data;
         if (--SREC_config.dlen) {
            NEXT_STATE_BREAK(hex_ihi)
         } else {
            if (++SREC_config.sum & 0xFF) return RTCSERR_SREC_CHECKSUM;
            NEXT_STATE_BREAK(SREC_config.estate)
         } /* Endif */

         /*
         ** After S7,S8,S9 records, accept only whitespace.
         */
      case hex_end:
         if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') || (c == '\0')) break;
         return RTCSERR_SREC_CHAR;

         /*
         ** Should never get here.
         */
      default:
         return RTCSERR_SREC_ERROR;
      } /* Endswitch */
   } /* Endwhile */
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SREC_eod
* Returned Value   : error code
* Comments  :  Routine to end decoding a HEX file.
*
*END*-----------------------------------------------------------------*/

uint32_t SREC_eod
   (
      void
   )
{ /* Body */

   switch (SREC_config.state) {
   case hex_end:
      return RTCS_OK;
   case hex_shi:
      return RTCSERR_SREC_END;
   default:
      return RTCSERR_SREC_EOF;
   } /* Endswitch */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SREC_exec
* Returned Value   : none
* Comments  :  Routine to execute a decoded HEX file.
*
*END*-----------------------------------------------------------------*/

void SREC_exec
   (
      void
   )
{ /* Body */

   /* 
   ** Start P122-0270-01
   ** Disable interrupts before executing new image 
   */
   _int_disable();
   /* END SPR P122-0270-01 */
   (*(void(_CODE_PTR_)(void))(SREC_config.addr))();

} /* Endbody */


/* EOF */
