/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   Header file for vybrid clock nodes implementation
*
*
*END************************************************************************/

#ifndef __bsp_clk_nodes_h__
#define __bsp_clk_nodes_h__

#include "mqx.h"
#include "stdarg.h"
#include "bsp_clk_name.h"

typedef struct clk_node CLK_NODE_T, *P_CLK_NODE_T;

struct clk_node {
    CLOCK_NAME name;
    P_CLK_NODE_T parent;
    P_CLK_NODE_T child;
    P_CLK_NODE_T sibling;
    uint8_t enable_cnt;
    uint8_t descendant_cnt;
    uint8_t active;
#if 0
    uint8_t is_self_counted;        // active is used to indicate if a clock
                                    // is opened in hardware layer, when it is
                                    // the case, it may or may not be counted by
                                    // parent node. this is what "is_self_counted"
                                    // do. It prevent a node to be duplicated counted
                                    // by it parent
#endif

    const uint8_t is_always_open;   // for some node, even enable_cnt and descendant_cnt
                                        // are both 0, the clock node can not be closed.
                                        // Obviously, this property is upward inherit. this
                                        // value will only be set at clk node definition, no
                                        // user can change it
    uint8_t freq_valid; // indicate the "freq" field contain a valid frequency value
    uint32_t freq;      // freq of the current clk node
    uint8_t (*is_physically_open)();    // this is only used in pseudo code, indicate
                                            // the physical status of a clock, in real
                        // world we should check register status
    P_CLK_NODE_T (*get_parent)();
    uint8_t (*check_parent)(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open);
    uint8_t (*physically_set_parent)(P_CLK_NODE_T parent);
    uint8_t (*physically_open_clk)();
    uint8_t (*physically_close_clk)();
    uint8_t (*calc_freq)(P_CLK_NODE_T p_self, uint32_t parent_freq);
    uint8_t (*set_freq)(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val);
    uint8_t (*dump_possible_parent)();
};

void inc_descendant(P_CLK_NODE_T p_clk);
void dec_descendant(P_CLK_NODE_T p_clk);

void clk_add_child(P_CLK_NODE_T parent, P_CLK_NODE_T child);
void clk_remove_child(P_CLK_NODE_T parent, P_CLK_NODE_T child);

extern const P_CLK_NODE_T clk_table[];
int get_nodes_nr(void);
// void dump_all_clocks();

// uint8_t is_clk_physically_open(P_CLK_NODE_T p_clk);
uint8_t is_clk_opened(P_CLK_NODE_T p_clk);
P_CLK_NODE_T find_clk_node(CLOCK_NAME clk_name);
P_CLK_NODE_T get_clk_entry(int i);

uint8_t open_clk(P_CLK_NODE_T p_clk);
uint8_t close_clk(P_CLK_NODE_T p_clk);
P_CLK_NODE_T set_clk_parent(P_CLK_NODE_T p_clk, P_CLK_NODE_T p_parent);
void update_child_freq(P_CLK_NODE_T p_clk, uint8_t old_val, uint8_t new_val);

void init_node_freq(P_CLK_NODE_T p_clk);

char* clk_name_str(CLOCK_NAME clk);

#endif
