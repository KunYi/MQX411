#ifndef __addr_info_h__
#define __addr_info_h__
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
*
*END************************************************************************/


/*% INT16 Size */
#define NS_INT16SZ	 				2

/*% IPv4 Address Size */
#define NS_INADDRSZ	 				4

/*% IPv6 Address Size */
#define NS_IN6ADDRSZ				16

#define MAX_HOSTNAME_SIZE     64


#define NET_ORDER			"inet" /* set order inet or inet6 or inet : inet6   */

#define	EAI_OK              0	/* OK 					*/
#define	EAI_BADFLAGS        3	/* invalid value for ai_flags 					*/
#define	EAI_FAIL            4   /* non-recoverable failure in name resolution 	*/
#define	EAI_FAMILY          5	/* ai_family not supported 						*/
#define	EAI_MEMORY          6	/* memory allocation failure 					*/
#define	EAI_NODATA          7	/* no address associated with hostname 			*/
#define	EAI_NONAME          8	/* hostname nor servname provided, or not known */
#define	EAI_SERVICE         9	/* servname not supported for ai_socktype 		*/
#define	EAI_SOCKTYPE        10	/* ai_socktype not supported 					*/
#define	EAI_SYSTEM          11	/* system error returned in errno 				*/

#define API_SOCK_DGRAM      1
#define API_SOCK_STREAM     2
#define API_SOCK_RAW		3


/*
 * Constants for getnameinfo()
 */
#define LWRES_CONTEXT_SERVERMODE	0x00000001U
#define LWRES_CONTEXT_USEIPV4		0x00000002U
#define LWRES_CONTEXT_USEIPV6		0x00000004U

#define LWRES_R_SUCCESS			0
#define LWRES_R_NOMEMORY		1 
#define LWRES_R_NOTFOUND		3
 
#define LWRES_ADDRTYPE_V4		0x00000001U	/*%< ipv4 */
#define LWRES_ADDRTYPE_V6		0x00000002U	/*%< ipv6 */ 
//#define	NI_MAXHOST	1025
#define	NI_MAXHOST	100
#define	NI_MAXSERV	32

#define	NI_NOFQDN		0x00000001
#define	NI_NUMERICHOST	0x00000002
#define	NI_NAMEREQD		0x00000004
#define	NI_NUMERICSERV	0x00000008
#define	NI_DGRAM		0x00000010

/*
** Type definitions
*/
typedef unsigned int   socklen_t;
typedef	uint16_t        sa_family_t;

/*!
 * Used to set various options such as timeout, authentication, etc
 */
#define LWRES_UDP_PORT		921	/*%< UDP Port Number */
#define LWRES_RECVLENGTH	16384 /*%< Maximum Packet Length */
#define LWRES_ADDR_MAXLEN	16	/*%< changing this breaks ABI */
#define LWRES_DEFAULT_TIMEOUT	120	/* 120 seconds for a reply */


typedef void *(*lwres_malloc_t)(void *arg, unsigned int length);
typedef void (*lwres_free_t)(void *arg, void *mem, unsigned int length); 


#define LWRES_LINK(lwres_addr_t) struct { lwres_addr_t *prev, *next; }
#define CTXMALLOC(len)		ctx->malloc(ctx->arg, (len))
#define CTXFREE(addr, len)	ctx->free(ctx->arg, (addr), (len))

#define LWRES_LIST_NEXT(elt, link) ((elt)->link.next)


typedef struct lwres_addr lwres_addr_t;

typedef struct lwres_addr {
	uint32_t					    family;
	uint16_t 					length;
	unsigned char				address[LWRES_ADDR_MAXLEN];
	LWRES_LINK(lwres_addr_t)	link;
} lwres_addr;


#define LWRES_CONFMAXNAMESERVERS 3	/*%< max 3 "nameserver" entries */
#define LWRES_CONFMAXLWSERVERS 1	/*%< max 1 "lwserver" entry */
#define LWRES_CONFMAXSEARCH 8		/*%< max 8 domains in "search" entry */
#define LWRES_CONFMAXLINELEN 256	/*%< max size of a line */
#define LWRES_CONFMAXSORTLIST 10	/*%< max 10 */



typedef struct lwres_context lwres_context_t;

#define LWRES_LIST(type) struct { type *head, *tail; }
typedef LWRES_LIST(lwres_addr_t) lwres_addrlist_t;
typedef uint32_t lwres_result_t;
#define LWRES_LIST_HEAD(list) ((list).head)

#define LWRES_LIST_UNLINK(list, elt, link) \
	do { \
		if ((elt)->link.next != NULL) \
			(elt)->link.next->link.prev = (elt)->link.prev; \
		else \
			(list).tail = (elt)->link.prev; \
		if ((elt)->link.prev != NULL) \
			(elt)->link.prev->link.next = (elt)->link.next; \
		else \
			(list).head = (elt)->link.next; \
		(elt)->link.prev = (void *)(-1); \
		(elt)->link.next = (void *)(-1); \
	} while (0)




typedef struct {
	lwres_context_t *lwctx;
	lwres_addr_t    nameservers[LWRES_CONFMAXNAMESERVERS];
	uint8_t	nsnext;		/*%< index for next free slot */

	lwres_addr_t	lwservers[LWRES_CONFMAXLWSERVERS];
	uint8_t	lwnext;		/*%< index for next free slot */

	char	       *domainname;

	char 	       *search[LWRES_CONFMAXSEARCH];
	uint8_t	searchnxt;	/*%< index for next free slot */

	struct {
		lwres_addr_t addr;
		/*% mask has a non-zero 'family' and 'length' if set */
		lwres_addr_t mask;
	} sortlist[LWRES_CONFMAXSORTLIST];
	uint8_t	sortlistnxt;

	uint8_t	resdebug;      /*%< non-zero if 'options debug' set */
	uint8_t	ndots;	       /*%< set to n in 'options ndots:n' */
	uint8_t	no_tld_query;  /*%< non-zero if 'options no_tld_query' */
} lwres_conf_t;


struct lwres_context {
	unsigned int		r_timeout;	/*%< time to wait for reply */
	uint32_t			    serial;		/*%< serial number state */
    /* For network I/O.*/
	int					sock;		/*%< socket to send on */
    lwres_addr_t		address;	/*%< address to send to */
	int					use_ipv4;	/*%< use IPv4 transaction */
	int					use_ipv6;	/*%< use IPv6 transaction */

	/*@{*/
	/*
	 * Function pointers for allocating memory.
	 */
	lwres_malloc_t		malloc;
	lwres_free_t		free;
	void		       *arg;
	/*@}*/

	/*%
	 * resolv.conf-like data
	 */
	lwres_conf_t		confdata;
};


/*% lwres_gnbaresponse_t */
typedef struct {
	/* public */
	uint32_t 					flags;
	uint16_t 					naliases;
	char			            *realname;
	char			            **aliases;
	uint16_t 					realnamelen;
	uint16_t 		            *aliaslen;
	/*! if base != NULL, it will be freed when this structure is freed. */
	void			            *base;
	unsigned int				baselen;
} lwres_gnbaresponse_t;



/*% lwres_gabnresponse_t */
typedef struct {
	/* public */
	uint32_t 					flags;
	uint16_t 					naliases;
	uint16_t 					naddrs;
	char			       		*realname;
	char			     		**aliases;
	uint16_t 					realnamelen;
	uint16_t 		       		*aliaslen;
	lwres_addrlist_t			addrs;
	/*! if base != NULL, it will be freed when this structure is freed. */
	void			       		*base;
	unsigned int				baselen;
} lwres_gabnresponse_t;

struct servent {
	char	*s_name		;		/* official service name 	*/
	char	**s_aliases	;		/* alias list 				*/
	int		s_port		;		/* port # 					*/
	char	*s_proto	;		/* protocol to use 			*/
};


extern char * strdup( const char *s);


extern void *lwres_malloc
(
	void            *arg, 
	unsigned int    len
); 

void lwres_free
(
	void *arg, 
	void *mem, 
	unsigned int len
); 

void lwres_conf_init
(
	lwres_context_t *ctx
); 

void lwres_resetaddr
(
	lwres_addr_t *addr
); 



uint32_t lwres_context_create
(
	lwres_context_t 	  **contextp, 
	void 				   *arg,
	lwres_malloc_t 			malloc_function,
	lwres_free_t 			free_function,
	unsigned int 			flags
);

void lwres_gnbaresponse_free
(
	lwres_context_t   	   *ctx, 
	lwres_gnbaresponse_t  **structp
);

void lwres_conf_clear
(
	lwres_context_t 	   *ctx
); 

void lwres_context_destroy
(
	lwres_context_t **contextp
); 



#endif
