
/*HEADER**********************************************************************
*
* Copyright 2008-2012 Freescale Semiconductor, Inc.
* Copyright 2004-2009 Embedded Access Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
 This file was generated by "gawk -f def2c.awk" script.
*
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include "snmpcfg.h"
#include "asn1.h"
#include "snmp.h"


#if RTCSCFG_ENABLE_SNMP

extern RTCSMIB_NODE MIBNODE_enterprises;

extern RTCSMIB_NODE MIBNODE_your_company;
extern const RTCSMIB_NODE MIBNODE_mqx_demo;
extern const RTCSMIB_NODE MIBNODE_demo1;
extern const RTCSMIB_NODE MIBNODE_data;
extern const RTCSMIB_NODE MIBNODE_hello_string;
extern const RTCSMIB_NODE MIBNODE_counter;
extern const RTCSMIB_NODE MIBNODE_alarm_limit;
extern const RTCSMIB_NODE MIBNODE_traps;
extern const RTCSMIB_NODE MIBNODE_trapmsg1;

extern const RTCSMIB_VALUE MIBVALUE_your_company;
extern const RTCSMIB_VALUE MIBVALUE_mqx_demo;
extern const RTCSMIB_VALUE MIBVALUE_demo1;
extern const RTCSMIB_VALUE MIBVALUE_data;
extern const RTCSMIB_VALUE MIBVALUE_hello_string;
extern const RTCSMIB_VALUE MIBVALUE_counter;
extern const RTCSMIB_VALUE MIBVALUE_alarm_limit;
extern const RTCSMIB_VALUE MIBVALUE_traps;
extern const RTCSMIB_VALUE MIBVALUE_trapmsg1;



uint32_t MIB_set_alarm_limit         (void *, unsigned char *, uint32_t);

RTCSMIB_NODE MIBNODE_your_company = {
   33118,

   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_mqx_demo,
   (RTCSMIB_NODE_PTR)&MIBNODE_enterprises,

   0,
   NULL,
   NULL, 0, NULL, NULL
};

const RTCSMIB_NODE MIBNODE_mqx_demo = {
   1,

   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_demo1,
   (RTCSMIB_NODE_PTR)&MIBNODE_your_company,

   0,
   NULL,
   NULL, 0, NULL, NULL
};

const RTCSMIB_NODE MIBNODE_demo1 = {
   1,

   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_data,
   (RTCSMIB_NODE_PTR)&MIBNODE_mqx_demo,

   0,
   NULL,
   NULL, 0, NULL, NULL
};

const RTCSMIB_NODE MIBNODE_data = {
   1,

   (RTCSMIB_NODE_PTR)&MIBNODE_traps,
   (RTCSMIB_NODE_PTR)&MIBNODE_hello_string,
   (RTCSMIB_NODE_PTR)&MIBNODE_demo1,

   0,
   NULL,
   NULL, 0, NULL, NULL
};

const RTCSMIB_NODE MIBNODE_hello_string = {
   1,

   (RTCSMIB_NODE_PTR)&MIBNODE_counter,
   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_data,

   RTCSMIB_ACCESS_READ,
   NULL,
   MIB_instance_zero, ASN1_TYPE_OCTET,
   (RTCSMIB_VALUE_PTR)&MIBVALUE_hello_string,
   NULL
};

const RTCSMIB_NODE MIBNODE_counter = {
   2,

   (RTCSMIB_NODE_PTR)&MIBNODE_alarm_limit,
   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_data,

   RTCSMIB_ACCESS_READ,
   NULL,
   MIB_instance_zero, ASN1_TYPE_INTEGER,
   (RTCSMIB_VALUE_PTR)&MIBVALUE_counter,
   NULL
};

const RTCSMIB_NODE MIBNODE_alarm_limit = {
   3,

   NULL,
   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_data,

   RTCSMIB_ACCESS_READ | RTCSMIB_ACCESS_WRITE,
   NULL,
   MIB_instance_zero, ASN1_TYPE_INTEGER,
   (RTCSMIB_VALUE_PTR)&MIBVALUE_alarm_limit,
   MIB_set_alarm_limit
};

const RTCSMIB_NODE MIBNODE_traps = {
   2,

   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_trapmsg1,
   (RTCSMIB_NODE_PTR)&MIBNODE_demo1,

   0,
   NULL,
   NULL, 0, NULL, NULL
};

const RTCSMIB_NODE MIBNODE_trapmsg1 = {
   1,

   NULL,
   NULL,
   (RTCSMIB_NODE_PTR)&MIBNODE_traps,

   RTCSMIB_ACCESS_READ,
   NULL,
   MIB_instance_zero, ASN1_TYPE_OCTET,
   (RTCSMIB_VALUE_PTR)&MIBVALUE_trapmsg1,
   NULL
};



#endif

#if ! RTCSCFG_ENABLE_IP4 
  #error This application requires RTCSCFG_ENABLE_IP4 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#if ! RTCSCFG_ENABLE_SNMP 
  #error This application requires RTCSCFG_ENABLE_SNMP defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


/****************************************************************/
#include <mqx.h>
#include <bsp.h>

#include "snmp_demo.h"
#include "snmpcfg.h"
#include "snmp.h"

#define COUNTER_OVERFLOW    5
#define COUNTER_DELAY       5000

#define TRAP_SPEC   3

#define SEND_TRAP_V1    1
#define SEND_TRAP_V2    1

void MIBdemo_init(void);
static uint32_t i = 0;
static uint32_t alarm_limit = COUNTER_OVERFLOW;

void MIBdemo_init(void)
{
	RTCSMIB_mib_add(&MIBNODE_your_company);
}


uint32_t Get_timer( void *dummy )
{
	return(i);
}

static void Set_timer( uint32_t value)
{
	i = value;
}

uint32_t Get_alarm_limit( void *dummy )
{
	return(alarm_limit);
}

uint32_t MIB_set_alarm_limit (void *dummy, unsigned char *varptr, uint32_t varlen)
{ /* Body */
    int32_t varval = RTCSMIB_int_read(varptr, varlen);
    alarm_limit = varval;   
    return(0); /*SNMP_ERROR_noError;*/
} /* Endbody */

void Snmp_task( uint32_t temp )
{
	
	// show welcome screen on serial
	printf("\n\rSNMP demo started. You can change the counter value \
		through the SNMP protocole. \n\rCouner value: \n\r");	
	for(;;)
	{
		printf("%d, ", i);
		_time_delay(COUNTER_DELAY);
		i++;				
		// test for alarm limit
		if(i > alarm_limit)
		{	
			// Send the alarm trap and clear counter
			#ifdef SEND_TRAP_V2
			  SNMPv2_trap_userSpec( (RTCSMIB_NODE *)&MIBNODE_trapmsg1 );
			#endif
			#ifdef SEND_TRAP_V1
			  SNMP_trap_userSpec( (RTCSMIB_NODE *)&MIBNODE_counter, TRAP_SPEC, 
			      (RTCSMIB_NODE *)&MIBNODE_your_company );
			#endif
			printf("\n\rTrap sent; Counter value:\n\r");
			Set_timer(0);
		}
	}
}

/****************************************************************/
const RTCSMIB_VALUE MIBVALUE_your_company = {
	RTCSMIB_NODETYPE_INT_CONST,
	NULL
};

const RTCSMIB_VALUE MIBVALUE_demo1 = {
	RTCSMIB_NODETYPE_INT_CONST,
	NULL
};

const RTCSMIB_VALUE MIBVALUE_mqx_demo = {
	RTCSMIB_NODETYPE_INT_CONST,
	NULL
};

const RTCSMIB_VALUE MIBVALUE_traps = {
	RTCSMIB_NODETYPE_INT_CONST,
	NULL
};
const RTCSMIB_VALUE MIBVALUE_data = {
	RTCSMIB_NODETYPE_INT_CONST,
	NULL
};

const RTCSMIB_VALUE MIBVALUE_hello_string = {
	RTCSMIB_NODETYPE_DISPSTR_PTR,
	(void *)"MQX SNMP demo"
};

const RTCSMIB_VALUE MIBVALUE_counter = {
	RTCSMIB_NODETYPE_UINT_FN,
	(void *)Get_timer
};

const RTCSMIB_VALUE MIBVALUE_alarm_limit = {
	RTCSMIB_NODETYPE_UINT_FN,
	(void *)Get_alarm_limit
};

const RTCSMIB_VALUE MIBVALUE_trapmsg1 = {
	RTCSMIB_NODETYPE_DISPSTR_PTR,
	(void *)"Counter reset"
};

/* EOF */