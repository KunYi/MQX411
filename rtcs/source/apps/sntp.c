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
*   This file contains an implementation of an
*   SNTP client.
*
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

/* SNTP client version */
#define SNTP_VERSION    0x04

/* Start CR 1052 */
/* Accepts version 3 or higher */
#define SNTP_VER_MIN   0x03
/* End CR */

/* Mode field in the SNTP header */
#define SNTP_MODE_CLIENT     0x03
#define SNTP_MODE_SERVER     0x04
#define SNTP_MODE_BROADCAST  0x05

/* Used to mask out fields in the LI_VN_MODE SNTP header field */
#define SNTP_MASK_MODE  0x07
#define SNTP_MASK_VN    0x38
#define SNTP_MASK_LI    0xC0

/* If this is set in the LI field, the timestamp is invalid */
#define SNTP_LI_ALARM   0x03

/* Minimum and maximum values accepted for the STRATUM field */
#define SNTP_STRATUM_MIN   0x01
#define SNTP_STRATUM_MAX   0x0E

/* Maximum value (in seconds) accepted for the poll interval */
#define SNTP_MAX_POLL      4294967


/* A SNTP header */
typedef struct sntp_header  {
   /*
   ** bits 0-1: Leap Indicator
   ** bits 2-4: Version Number
   ** bits 5-7: Mode
   */
   unsigned char    LI_VN_MODE[1];
   unsigned char    STRATUM[1];
   unsigned char    POLL[1];
   unsigned char    PRECISION[1];
   unsigned char    ROOT_DELAY[4];
   unsigned char    ROOT_DISPERSION[4];
   unsigned char    ROOT_IDENTIFIER[4];
   unsigned char    REFERENCE_TIMESTAMP1[4];
   unsigned char    REFERENCE_TIMESTAMP2[4];
   unsigned char    ORIGINATE_TIMESTAMP1[4];
   unsigned char    ORIGINATE_TIMESTAMP2[4];
   unsigned char    RECEIVE_TIMESTAMP1[4];
   unsigned char    RECEIVE_TIMESTAMP2[4];
   unsigned char    TRANSMIT_TIMESTAMP1[4];
   unsigned char    TRANSMIT_TIMESTAMP2[4];
} SNTP_HEADER, * SNTP_HEADER_PTR;

struct sntpclnt_init {
   uint32_t        poll;
   _ip_address    dest;
};

/* Function prototype */
static void    SNTP_task(void *, void *creator);
static bool SNTP_valid_header(SNTP_HEADER_PTR);
static void    SNTP_build_header(SNTP_HEADER_PTR, uint32_t *, uint32_t *);
static void    SNTP_set_time(SNTP_HEADER_PTR, uint32_t, uint32_t);
static uint32_t SNTP_setup(uint32_t *, _ip_address, uint32_t);
static uint32_t SNTP_timed_send_recv(uint32_t sock, _ip_address *,
   uint32_t *);

static void SNTP_add96(uint32_t *, uint32_t *, uint32_t *);
static void SNTP_sub96(uint32_t *, uint32_t *, uint32_t *);
static void SNTP_shiftr96(uint32_t *, uint32_t *);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_init
* Returned Value   : error code
* Comments  :  Start the SNTP client.
*
*END*-----------------------------------------------------------------*/

uint32_t SNTP_init
   (
      char          *name,
      uint32_t           priority,
      uint32_t           stacksize,
      _ip_address       destination,
      uint32_t           poll
   )
{ /* Body */
   struct sntpclnt_init init;

   init.poll = poll;
   init.dest = destination;
   return RTCS_task_create(name, priority, stacksize, SNTP_task, &init);
} /* Endbody */



/*TASK*----------------------------------------------------------------
*
* Function Name   : SNTP_task
* Returned Value  : void
* Comments        : This task requests the current time from a time server.
*                   Once the time server replies with the current time, the
*                   system time is set and the task ends.
*
*END*-----------------------------------------------------------------*/

static void SNTP_task
   (
      void   *init_ptr,
      void   *creator
   )
{ /* Body */

   struct sntpclnt_init    init = *(struct sntpclnt_init *)init_ptr;
   uint32_t                 sock, error_code, timeout;

   /* Setup socket and check parameters */
   error_code = SNTP_setup(&sock, init.dest, init.poll);

   if (error_code != RTCS_OK) {
      RTCS_task_exit(creator, error_code);
   } else  {
      RTCS_task_resume_creator(creator, RTCS_OK);
   } /* Endif */

   /*
   ** Start an infinite loop and poll for time at regular intervals.
   */
   while (TRUE) {
      timeout = init.poll * 1000;
      error_code = SNTP_timed_send_recv(sock, &init.dest, &timeout);
      RTCS_delay(timeout);
   } /* Endwhile */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_oneshot
* Returned Value   : uint32_t error code
* Comments  :  Blocking function call that gets the time from a NTP/SNTP
*     server and sets the local time accordingly. If the NTP/SNTP server does
*     not reply within a certain timeout, function returns an error.
*
*END*-----------------------------------------------------------------*/

uint32_t SNTP_oneshot
   (
      _ip_address destination,
         /*[IN] Destination IP address */
      uint32_t     timeout
         /*[IN] Maximum time (in milliseconds) to keep trying to get time */
   )
{ /* Body */
   uint32_t  sock, error_code;

   /* Setup socket and check parameters */
   error_code = SNTP_setup(&sock, destination, 1);

   if (error_code != RTCS_OK) {
      return error_code;
   } /* Endif */

   error_code = SNTP_timed_send_recv(sock, &destination, &timeout);

   shutdown(sock, FLAG_CLOSE_TX);
   return error_code;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_valid_header
* Returned Value   : bool
* Comments  :  Checks to see if the packet is a valid SNTP/NTP reply for a
*     ANYCAST or UNICAST client. (Will not work with a MULTICAST client packet)
*
*END*-----------------------------------------------------------------*/

static bool SNTP_valid_header
   (
      SNTP_HEADER_PTR   header_ptr
         /*[IN] pointer to a SNTP header struct */
   )
{ 
    uint32_t    t2[2];
    uint32_t    t3[2];
    bool        result;

    t2[0] = mqx_ntohl(header_ptr->RECEIVE_TIMESTAMP1);
    t2[1] = mqx_ntohl(header_ptr->RECEIVE_TIMESTAMP2);
    t3[0] = mqx_ntohl(header_ptr->TRANSMIT_TIMESTAMP1);
    t3[1] = mqx_ntohl(header_ptr->TRANSMIT_TIMESTAMP2);


    /* Reply shoud be in server mode */
    /* Version should be same as in original message */
    /* LI field should be between 0 and 2 */
    /* Stratum should be between 1 and 14 */
    /* Originate timestamp must be non zero */
    /* Receive timestamp must be non zero */
    /* Transmit timestamp must be non-zero */
    if(
        ((mqx_ntohc(header_ptr->LI_VN_MODE) & SNTP_MASK_MODE) == SNTP_MODE_SERVER) &&
        ((mqx_ntohc(header_ptr->LI_VN_MODE) & SNTP_MASK_VN) >> 3 >= SNTP_VER_MIN) &&
        ((mqx_ntohc(header_ptr->LI_VN_MODE) & SNTP_MASK_LI) >> 6 != SNTP_LI_ALARM) &&
        ( mqx_ntohc(header_ptr->STRATUM) >= SNTP_STRATUM_MIN) &&
        ( mqx_ntohc(header_ptr->STRATUM) <= SNTP_STRATUM_MAX) &&
        ((t2[0] != 0) || (t2[1] != 0)) &&
        ((t3[0] != 0) || (t3[1] != 0)) &&
        /* Correct behaviour requires that Transmit timestamp (t3) must be later than Recceive timestamp (t2)
         * Take into account the overflow case for seconds.*/
         /* Some SNTP Servers can generate wrong Seconds Fraction of timestamp.*/
        ((t3[0] != /* > */ t2[0]) || ((t3[0] == t2[0]) && (t3[1] >= t2[1])))
    )
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return result;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_build_header
* Returned Value   : void
* Comments  :  Builds a sntp header for to be transmitted by a sntp client and
*     returns the originate timestamp
*
*END*-----------------------------------------------------------------*/

static void SNTP_build_header
   (
      SNTP_HEADER_PTR   header_ptr,
         /*[OUT] a build SNTP header */
      uint32_t       *orig_sec_ptr,
         /*[OUT] the orignate time in seconds */
      uint32_t       *orig_fract_ptr
         /*[OUT] the orignate fraction of seconds */
   )
{ /* Body */
   _mem_zero(header_ptr, sizeof(SNTP_HEADER));

   /* Set the version */
   mqx_htonc(header_ptr->LI_VN_MODE, (SNTP_VERSION << 3));

   /* Set the mode */
   mqx_htonc(header_ptr->LI_VN_MODE,
      SNTP_MODE_CLIENT | mqx_ntohc(header_ptr->LI_VN_MODE));

   /* Set the Transmit timestamp */
   RTCS_date_get(orig_sec_ptr, orig_fract_ptr);
   mqx_htonl(header_ptr->TRANSMIT_TIMESTAMP1, *orig_sec_ptr);
   mqx_htonl(header_ptr->TRANSMIT_TIMESTAMP2, *orig_fract_ptr);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_set_time
* Returned Value   : void
* Comments  :  Takes a valid sntp header and uses it to set the local clock
*
* RFC2030: Calculate the time offset:
*    T1: ORIGINATE (local time at which client sent request)
*    T2: RECEIVE (the time at which server received request)
*    T3: TRANSMIT (the time sent be the server)
*    T4: DESTINATION (the local time at which client received request)
*
*    Time offset = ((T2 - T1) + (T3 - T4)) / 2
*
*END*-----------------------------------------------------------------*/

static void SNTP_set_time
   (
      /*[IN] valid sntp header */
      SNTP_HEADER_PTR   header_ptr,
      /*[IN] seconds from timestamp when header was received */
      uint32_t calc_sec,
      /*[IN] fraction of seconds from timestamp when header was received */
      uint32_t calc_fract
   )
{ /* Body */
   uint32_t t1[3], t2[3], t3[3], t4[3], temp1[3], temp2[3];

   t1[1] = mqx_ntohl(header_ptr->ORIGINATE_TIMESTAMP1);
   t1[2] = mqx_ntohl(header_ptr->ORIGINATE_TIMESTAMP2);
   t2[1] = mqx_ntohl(header_ptr->RECEIVE_TIMESTAMP1);
   t2[2] = mqx_ntohl(header_ptr->RECEIVE_TIMESTAMP2);
   t3[1] = mqx_ntohl(header_ptr->TRANSMIT_TIMESTAMP1);
   t3[2] = mqx_ntohl(header_ptr->TRANSMIT_TIMESTAMP2);
   t4[1] = calc_sec;
   t4[2] = calc_fract;

   t1[0] = 0;
   t2[0] = 0;

   /* If t4 is smaller than t1, set top bit to 1 */
   if (t4[1] < t1[1] || (t4[1] == t1[1] && t4[2] < t1[2]))
      t4[0] = 1;
   else
      t4[0] = 0;

   /* If t3 is smaller than t2, set top bit to 1 */
   if (t3[1] < t2[1] || (t3[1] == t2[1] && t3[2] < t2[2]))
      t3[0] = 1;
   else
      t3[0] = 0;

   SNTP_sub96(t2, t1, temp1);
   SNTP_sub96(t3, t4, temp2);
   SNTP_add96(temp1, temp2, t4);
   SNTP_shiftr96(t4, temp2);

   /*
   ** Now the time offset is added to the local time.
   */
   temp1[0] = 0;
   RTCS_date_get(temp1 + 1, temp1 + 2);
   SNTP_add96(temp1, temp2, t4);

   RTCS_date_set(t4[1], t4[2]);

} /* Body */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_setup
* Returned Value   : uint32_t error code
* Comments  :  Checks the parameters for invalid values and tries to create and
*     bind a socket.
*
*END*-----------------------------------------------------------------*/

static uint32_t  SNTP_setup
   (
      uint32_t    *sock_ptr,
        /*[OUT] pointer to the new socket */
      _ip_address    destination,
         /*[IN] the destination address */
      uint32_t        poll
         /*[IN] the polling interval */
   )
{ /* Body */
   sockaddr_in    addr;
   uint32_t        error_code = RTCS_OK;

   /* Check input parameters */
   if (poll == 0 || poll > SNTP_MAX_POLL || destination == 0) {
      return RTCSERR_INVALID_PARAMETER;
   } /* Endif */

   /* Create a socket for the communication */
   if ((*sock_ptr = socket(PF_INET, SOCK_DGRAM, 0)) == RTCS_SOCKET_ERROR)  {
      return RTCS_SOCKET_ERROR;
   } /* Endif */

   /* Local address */
   addr.sin_family      = AF_INET;
   addr.sin_port        = IPPORT_NTP;
   addr.sin_addr.s_addr = INADDR_ANY;

   error_code = bind(*sock_ptr, (const sockaddr *)&addr, sizeof(addr));
   if (error_code) {
      shutdown(*sock_ptr, FLAG_ABORT_CONNECTION);
   } /* Endif */

   return error_code;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_timed_send_recv
* Returned Value   : uint32_t error code
* Comments  :  Tries to send and receive a SNTP packet. Also sets the system
*     time if a valid SNTP packet is received.
*
*END*-----------------------------------------------------------------*/

static uint32_t  SNTP_timed_send_recv
   (

      uint32_t sock,
         /*[IN] socket */
      _ip_address  *destination,
         /*[IN] remote IP NTP service */
      uint32_t  *msec_count_ptr
         /*[IN] millisecond timeout value */
   )
{ /* Body */
   SNTP_HEADER    header;
   sockaddr_in    addr;
   uint16_t        remote_len = sizeof(sockaddr_in);
   int32_t         error;
   uint32_t        timeout, msec_base, diff, error_code, sec, sec_fract,
                  orig_sec, orig_msec, msec_count = *msec_count_ptr;

   /* Setup destination IP */
   addr.sin_family      = AF_INET;
   addr.sin_addr.s_addr = *destination;
   addr.sin_port        = IPPORT_NTP;

   /* Build a packet for unicast/anycast mode and send it  */
   SNTP_build_header(&header, &orig_sec, &orig_msec);

   /* Send packet */
   error = sendto(sock, (char *)&header, sizeof(SNTP_HEADER), 0, (sockaddr *)&addr,
      sizeof(sockaddr_in));

   /* Get reply only if request was sent */
   if (error != sizeof(SNTP_HEADER)) {
      return RTCS_geterror(sock);
   } /* Endif */

   /* Accept only correct replies */
   do  {
      msec_base = RTCS_time_get();
      if (msec_count == 0)  {
         timeout = (uint32_t)-1;
      } else  {
         timeout = msec_count;
      } /* Endif */

      error_code = RTCS_selectset(&sock, 1, timeout);

      /* Calculate elapsed time */
      diff = RTCS_timer_get_interval(msec_base, RTCS_time_get());
      if (diff > msec_count)  {
         msec_count = 0;
      } else  {
         msec_count -= diff;
      } /* Endif */

      if (error_code == sock) { /* If there is activity */
         error = recvfrom(sock, (char *)&header, sizeof(SNTP_HEADER), 0, (sockaddr *)&addr, &remote_len);

         /* If error is critical, quit */
         if (error == RTCS_ERROR) {
            return RTCS_geterror(sock);
         } /* Endif */

         /* If non-critical error, try again */
         if (error != sizeof(SNTP_HEADER) || addr.sin_port != IPPORT_NTP) {
            continue;
         } /* Endif */

         /* If the message is not from the specified server, try again */
         if (!IN_MULTICAST(*destination) && *destination != INADDR_BROADCAST &&
            *destination != addr.sin_addr.s_addr) {
            continue;
         } /* Endif */

         /* Take a timestamp of time received */
         RTCS_date_get(&sec, &sec_fract);

         /* Make sure the reply is not an old duplicate */
         if (mqx_ntohl(header.ORIGINATE_TIMESTAMP1) != orig_sec &&
             mqx_ntohl(header.ORIGINATE_TIMESTAMP2) != orig_msec)  {
            continue;
         } /* Endif */

         /* If the message is valid, then break from the recv loop */
         if (SNTP_valid_header(&header)) {
            SNTP_set_time(&header, sec, sec_fract);
            /* UNICAST/ANYCAST client becomes UNICAST */
            *destination = addr.sin_addr.s_addr;
            break;
         } /* Endif */
      } /* Endif */
   } while (msec_count > 0);

   *msec_count_ptr = msec_count;
   if (error_code == 0) {
      error_code = RTCSERR_TIMEOUT;
   } else  {
      error_code = RTCS_OK;
   } /* Endif */

   return error_code;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_add96
* Returned Value   : void
* Comments  :  Adds the 96 bit numbers x and y, and stores the result in z
*
*END*-----------------------------------------------------------------*/

static void SNTP_add96
   (
      uint32_t *x,
         /* 96 bit number: x0x1x2 */
      uint32_t *y,
         /* 96 bit number: y0y1y2 */
      uint32_t *z
         /* 96 bit number: z0z1z2 */
   )
{ /* Body */
   bool carry;

   /* Add the fraction of seconds */
   z[2] = x[2] + y[2];

   /* Check for carry */
   carry = (z[2] < x[2]);

   /* Add the seconds together, and the carry if it exists */
   z[1] = x[1] + y[1] + (carry?1:0);

   /* Start CR 1051 */
   /* Check for a new carry. Make sure z[1] is not 0 before we sub 1 */
   if (!carry) {
      carry = (z[1] < x[1]);
   } else if (z[1]) {
      carry = (z[1] - 1 < x[1]);
   } /* Endif */
   /* End CR */

   /* Add the high words together and add the carry if it exists */
   z[0] = x[0] + y[0] + (carry?1:0);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_sub96
* Returned Value   : void
* Comments  :  Subtract 96 bit numbers y from x, and stores the result in z
*
*END*-----------------------------------------------------------------*/

static void SNTP_sub96
   (
      uint32_t *x,
         /* 96 bit number: x0x1x2 */
      uint32_t *y,
         /* 96 bit number: y0y1y2 */
      uint32_t *z
         /* 96 bit number: z0z1z2 */
   )
{ /* Body */
   bool carry;

   /* Subtract the fraction of seconds */
   z[2] = x[2] - y[2];

   /* Check for a carry */
   carry = (x[2] < y[2]);

   /* Subtract the seconds, along with carry if it exists */
   z[1] = x[1] - y[1] - (carry?1:0);

   /* Start CR 1051 */
   /* Check for a new carry. Make sure x[1] is not 0 before we sub 1 */
   if (!carry) {
      carry = (x[1] < y[1]);
   } else if (x[1]) {
      carry = (x[1] - 1 < y[1]);
   } /* Endif */
   /* End CR */

   /* Subtract the high words, along with carry if it exists */
   z[0] = x[0] - y[0] - (carry?1:0);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNTP_shiftr96
* Returned Value   : void
* Comments  :  Unsigned right shift on x. Result stores in z
*
*END*-----------------------------------------------------------------*/

static void SNTP_shiftr96
   (
      uint32_t *x,
         /* 96 bit number: x0x1x2 */
      uint32_t *z
         /* 96 bit number: z0z1z2 */
   )
{ /* Body */

   z[0] = x[0] >> 1;

   z[1] = (x[0] << 31) | (x[1] >> 1);

   z[2] = (x[1] << 31) | (x[2] >> 1);

} /* Endbody */

#endif
/* EOF */
