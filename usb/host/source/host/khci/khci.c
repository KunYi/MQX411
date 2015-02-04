/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains KHCI-specific implementations of USB interfaces
*
*
*END************************************************************************/
#include <mqx.h>
#include <lwevent.h>
#include <lwmsgq.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_dev_list.h"


#include "mqx_host.h"
#include "khci.h"
#include "khci_prv.h"

#ifdef KHCI_DEBUG
struct debug_messaging {
  char inout;
  TR_MSG_TYPE type;
  uint8_t ep;
  uint16_t size;
};
volatile static uint16_t dm_index = 0;
volatile static struct debug_messaging dm[1024] = { 0 }; /* note, the array is for 1024 records only */
#   define KHCI_DEBUG_LOG(a, b, c, d) \
  { \
    dm[dm_index].inout = a; \
      dm[dm_index].type = b; \
      dm[dm_index].ep = c; \
      dm[dm_index].size = d; \
      dm_index++; \
  }
#else
#   define KHCI_DEBUG_LOG(a, b, c, d) {}
#endif

// KHCI task parameters
#define USB_KHCI_TASK_NUM_MESSAGES         16
#define USB_KHCI_TASK_TEMPLATE_INDEX       0
#define USB_KHCI_TASK_ADDRESS              _usb_khci_task
#define USB_KHCI_TASK_STACKSIZE            1600
#define USB_KHCI_TASK_NAME                 "KHCI Task"
#define USB_KHCI_TASK_ATTRIBUTES           0
#define USB_KHCI_TASK_CREATION_PARAMETER   0
#define USB_KHCI_TASK_DEFAULT_TIME_SLICE   0

// atom transaction error results
#define KHCI_ATOM_TR_PID_ERROR  (-1)
#define KHCI_ATOM_TR_EOF_ERROR  (-2)
#define KHCI_ATOM_TR_CRC_ERROR  (-4)
#define KHCI_ATOM_TR_TO         (-16)
#define KHCI_ATOM_TR_DMA_ERROR  (-32)
#define KHCI_ATOM_TR_BTS_ERROR  (-128)
#define KHCI_ATOM_TR_NAK        (-256)
#define KHCI_ATOM_TR_DATA_ERROR (-512)
#define KHCI_ATOM_TR_STALL      (-1024)
#define KHCI_ATOM_TR_RESET      (-2048)

#if defined( __ICCCF__ ) || defined( __ICCARM__ )
    #pragma segment="USB_BDT_Z"
    #pragma data_alignment=512
    __no_init static uint8_t bdt[512] @ "USB_BDT_Z";
#elif defined (__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".usb_bdt"),unused)) static uint8_t bdt[512] = { 1 };
#else
  #pragma define_section usb_bdt ".usb_bdt" RW
  __declspec(usb_bdt) static uint8_t bdt[512] = { 1 };    // DO NOT REMOVE INITIALIZATION !!! bug in CW (cw 7.1) - generate wrong binary code, data
#endif

#if defined(KHCICFG_4BYTE_ALIGN_FIX)
static uint8_t *_usb_khci_swap_buf_ptr = NULL;
#endif

/* Prototypes of functions */
static USB_STATUS _usb_khci_preinit(_usb_host_handle *handle);
static USB_STATUS _usb_khci_init(_usb_host_handle handle);
static USB_STATUS _usb_khci_shutdown(_usb_host_handle handle);
static USB_STATUS _usb_khci_send(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_send_setup(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_recv(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_cancel(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_bus_control(_usb_host_handle handle, uint8_t bus_control);
static uint32_t _usb_khci_get_frame_number(_usb_host_handle handle);

static USB_STATUS _usb_khci_host_close_pipe(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr);

static void _usb_khci_process_tr_complete(KHCI_PIPE_STRUCT_PTR pipe_ptr, KHCI_TR_STRUCT_PTR tr_ptr, uint32_t required, uint32_t remain, int32_t err);
static int32_t _usb_khci_atom_tr(USB_KHCI_HOST_STATE_STRUCT_PTR host_ptr, uint32_t type, KHCI_PIPE_STRUCT_PTR pipe_desc_ptr, uint8_t *buf, uint32_t len);

const struct usb_host_callback_functions_struct _usb_khci_host_callback_table = {
   /* The Host/Device preinit function */
   _usb_khci_preinit,

  /* The Host/Device init function */
   _usb_khci_init,

   /* The function to shutdown the host/device */
   _usb_khci_shutdown,

   /* The function to send data */
   _usb_khci_send,

   /* The function to send setup data */
   _usb_khci_send_setup,

   /* The function to receive data */
   _usb_khci_recv,

   /* The function to cancel the transfer */
   _usb_khci_cancel,

   /* The function for USB bus control */
   _usb_khci_bus_control,

   NULL,

   /* The function to close pipe */
   _usb_khci_host_close_pipe,

   /* Get frame number function */
   _usb_khci_get_frame_number,

   /* Get microframe number function - not applicable on FS */
   NULL,

   /* Open pipe function */
   NULL,

   NULL,

   NULL
};

// KHCI event bits
#define KHCI_EVENT_ATTACH       0x01
#define KHCI_EVENT_RESET        0x02
#define KHCI_EVENT_TOK_DONE     0x04
#define KHCI_EVENT_SOF_TOK      0x08
#define KHCI_EVENT_DETACH       0x10
#define KHCI_EVENT_SHUTDOWN     0x20
#define KHCI_EVENT_MASK         0x3f

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_isr
*  Returned Value : None
*  Comments       :
*        Service all the interrupts in the kirin usb hardware
*END*-----------------------------------------------------------------*/
static void _usb_khci_isr(_usb_host_handle handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR)handle;
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;
    unsigned char status;

    while (1)
    {
        status = (uint8_t)(usb_ptr->ISTAT & usb_ptr->INTEN);

        if (!status)
            break;

        usb_ptr->ISTAT = status;

        if (status & USB_ISTAT_ATTACH_MASK)
        {
            /* USB device is (being) attached */
            usb_ptr->INTEN &= ~USB_INTEN_ATTACHEN_MASK;
            _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_ATTACH);
        }

        if (status & USB_ISTAT_TOKDNE_MASK)
        {
            // atom transaction done - token done

            _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_TOK_DONE);
        }

        if (status & USB_ISTAT_USBRST_MASK)
        {
            // usb reset
            _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_RESET);
        }
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_init_int_tr
*  Returned Value : None
*  Comments       :
*        Initialize interrupt transaction que
*END*-----------------------------------------------------------------*/
static void _usb_khci_init_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {
    _mqx_int i;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;

    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
        tr->message.type = TR_MSG_UNKNOWN;
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_add_int_tr
*  Returned Value : -1 que is full
*  Comments       :
*        Add new interrupt transaction to que
*END*-----------------------------------------------------------------*/
static int _usb_khci_add_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr, MSG_STRUCT *msg, _mqx_uint period) {
    _mqx_int i = 0;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;
    TIME_STRUCT tm;

    // find free position
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++)
    {
        if (tr->message.type == TR_MSG_UNKNOWN)
        {
            tr->period = period;

            _time_get(&tm);
            tr->time.SECONDS = tm.SECONDS;
            tr->time.MILLISECONDS = tm.MILLISECONDS + period;

            tr->time.SECONDS += tr->time.MILLISECONDS / 1000;
            tr->time.MILLISECONDS %= 1000;

            USB_mem_copy(msg, &tr->message, sizeof(MSG_STRUCT));

            break;
        }
    }

    return (i < USBCFG_KHCI_MAX_INT_TR) ? i : -1;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_rm_int_tr
*  Returned Value : 0 successful
*  Comments       :
*        Remove interrupt transaction from que
*END*-----------------------------------------------------------------*/
static int _usb_khci_rm_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr, MSG_STRUCT *msg) {
    _mqx_int i = 0;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;

    // find record
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++)
    {
        if (tr->message.msg.tr.pipe_desc == msg->msg.tr.pipe_desc && tr->message.msg.tr.pipe_tr == msg->msg.tr.pipe_tr)
        {
            //USB_mem_zero(tr, sizeof(TR_INT_QUE_ITM_STRUCT));
            tr->message.type = TR_MSG_UNKNOWN;
            return 0;
        }
    }

    return -1;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_get_hot_int_tr
*  Returned Value : 0 successful
*  Comments       :
*        Make a message copy for transaction which need evaluation
*END*-----------------------------------------------------------------*/
static int _usb_khci_get_hot_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr, MSG_STRUCT *msg) {
    _mqx_int i, res = -1;
    register TIME_STRUCT hot_time;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;
    TR_INT_QUE_ITM_STRUCT_PTR hot_tr = NULL;
    TIME_STRUCT tm;

    hot_time.SECONDS = hot_time.MILLISECONDS = 0xFFFFFFFF;

    /* find hottest transaction */
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
        if (tr->message.type != TR_MSG_UNKNOWN) {
            if (hot_time.SECONDS > tr->time.SECONDS) {
                hot_time.SECONDS = tr->time.SECONDS;
                hot_time.MILLISECONDS = tr->time.MILLISECONDS;
                hot_tr = tr;
            }
            else if ((hot_time.SECONDS == tr->time.SECONDS) && (hot_time.MILLISECONDS > tr->time.MILLISECONDS)) {
                hot_time.MILLISECONDS = tr->time.MILLISECONDS;
                hot_tr = tr;
            }
        }
    }

    if (hot_tr) {
        /* test if hottest transaction was the last one with timeout - if yes, don't allow to block USB transfers with this interrupt */
        if (usb_host_ptr->last_to_pipe == hot_tr->message.msg.tr.pipe_desc) {
            usb_host_ptr->last_to_pipe = NULL; //it is allowed to perform this interupt next time, but not now
            return res;
        }

        /* test if hottest transaction is enough hot to fire */
        _time_get(&tm);

        if ((hot_time.SECONDS < tm.SECONDS) || ((hot_time.SECONDS == tm.SECONDS) && (hot_time.MILLISECONDS < tm.MILLISECONDS))) {
            /* update time to next fire = now + period */
            hot_tr->time.SECONDS = tm.SECONDS;
            hot_tr->time.MILLISECONDS = tm.MILLISECONDS + hot_tr->period;

            hot_tr->time.SECONDS += hot_tr->time.MILLISECONDS / 1000;
            hot_tr->time.MILLISECONDS %= 1000;

            USB_mem_copy(&hot_tr->message, msg, sizeof(MSG_STRUCT));
            res = 0;
        }
    }

    return res;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_attach
*  Returned Value : none
*  Comments       :
*        KHCI attach event
*END*-----------------------------------------------------------------*/
static void _usb_khci_attach(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;

    usb_ptr->ADDR = 0;
    _time_delay(100);

    // speed check, set
    usb_host_ptr->G.SPEED = (uint8_t)((usb_ptr->CTL & USB_CTL_JSTATE_MASK) ? USB_SPEED_FULL : USB_SPEED_LOW);
    if (usb_host_ptr->G.SPEED == USB_SPEED_FULL)
	{
        usb_ptr->ADDR &= ~USB_ADDR_LSEN_MASK;
	}
    usb_ptr->ISTAT = 0xff;   // clean each int flags
    usb_ptr->INTEN = \
        USB_INTEN_TOKDNEEN_MASK |
        USB_INTEN_USBRSTEN_MASK;

    // bus reset
    usb_ptr->CTL |= USB_CTL_RESET_MASK;
    _time_delay(30); //wait for 30 milliseconds (2.5 is minimum for reset, 10 recommended)
    usb_ptr->CTL &= ~USB_CTL_RESET_MASK;

    // Delay after reset was provided to be sure about speed- HS / FS. Since the KHCI does not run HS, the delay is redundant.
    // Some kinetis devices cannot recover after the delay, so it is better not to have delayed speed detection and SOF packet generation
    // This is potentional risk as any high priority task will get CPU now, the host will not begin the enumeration process.
    //_time_delay(10);

    // enable SOF sending
    usb_ptr->CTL |= USB_CTL_USBENSOFEN_MASK;

    _time_delay(100);

    usb_ptr->INTEN = \
        USB_INTEN_TOKDNEEN_MASK |
        USB_INTEN_USBRSTEN_MASK;

    usb_dev_list_attach_device((void *)usb_host_ptr, (uint8_t)(usb_host_ptr->G.SPEED), 0, 0);

      _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_ATTACH);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_reset
*  Returned Value : none
*  Comments       :
*        KHCI reset event
*END*-----------------------------------------------------------------*/
static void _usb_khci_reset(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;

    // clear attach flag
    usb_ptr->ISTAT |= USB_ISTAT_ATTACH_MASK;

    /* Test the presence of USB device */
    if (usb_ptr->ISTAT & USB_ISTAT_ATTACH_MASK) {
        /* device attached, so really normal reset was performed */
        usb_ptr->ADDR = 0;
        usb_ptr->ENDPOINT[0].ENDPT |= USB_ENDPT_HOSTWOHUB_MASK;
    }
    else {
        /* device was detached, the reset event is false- nevermind, notify about detach */
        _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_DETACH);
    }

    _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_RESET | KHCI_EVENT_TOK_DONE);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_detach
*  Returned Value : none
*  Comments       :
*        KHCI detach event
*END*-----------------------------------------------------------------*/
static void _usb_khci_detach(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;

    usb_dev_list_detach_device((void *)usb_host_ptr, 0, 0);

    /* Cleaning interrupt transaction queue from device is done
    ** by calling _usb_khci_host_close_pipe from upper layer
    */

    /* Now, disable bus control for any events, disable SOFs,
    ** just prepare for attach in host mode.
    */
    usb_ptr->CTL = USB_CTL_HOSTMODEEN_MASK;
    /* This will clear all pending interrupts... In fact, there shouldnt be any
    ** after detaching a device.
    */
    usb_ptr->ISTAT = 0xFF;
    /* Now, enable only USB interrupt attach for host mode */
    usb_ptr->INTEN = USB_INTEN_ATTACHEN_MASK;

    _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_DETACH);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task
*  Returned Value : none
*  Comments       :
*        KHCI task
*END*-----------------------------------------------------------------*/
static void _usb_khci_task(uint32_t dev_inst_ptr) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) dev_inst_ptr;
    _mqx_uint seq_ints = 10;
    MSG_STRUCT msg;
    uint32_t remain = 0;
    uint32_t required = 0;
    _mqx_int res = 0;
    uint8_t *buf;

    /* The _lwmsgq_init accepts the size of MSG_STRUCT as a multiplier of sizeof(_mqx_max_type) */
#define MSG_SIZE_IN_MAX_TYPE (1 + (sizeof(MSG_STRUCT) - 1) / sizeof(_mqx_max_type))
    if (_lwmsgq_init(usb_host_ptr->tr_que, USBCFG_KHCI_TR_QUE_MSG_CNT, MSG_SIZE_IN_MAX_TYPE) != MQX_OK) {
        // lwmsgq_init failed
        _mqx_exit(0);
    }

    _usb_khci_init_int_tr(usb_host_ptr);

    while (1) {
        if (usb_host_ptr->G.DEVICE_LIST_PTR) {

            msg.type = TR_MSG_UNKNOWN;

            // look for interrupt transaction
            if ((_usb_khci_get_hot_int_tr(usb_host_ptr, &msg) != 0) || (!seq_ints--)) {
                seq_ints = 10;
                if (_lwmsgq_receive(usb_host_ptr->tr_que, (uint32_t *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, USBCFG_KHCI_WAIT_TICK, 0) == MQX_OK)
                    if (msg.msg.tr.pipe_desc->G.PIPETYPE == USB_INTERRUPT_PIPE)
                        _usb_khci_add_int_tr(usb_host_ptr, &msg, msg.msg.tr.pipe_desc->G.INTERVAL);
            }

            if (msg.type != TR_MSG_UNKNOWN) {
                // evaluate message

                switch (msg.type) {
                    case SHDWN_MSG:
                        _lwsem_post(msg.msg.shdwn.end_of_task);
                        /* returning from the infinite loop finishes the current khci task */
                        return;

                    case TR_MSG_SETUP:
//                        msg.pipe_desc->G.NEXTDATA01 = 0; done in upper layer
                        _usb_khci_atom_tr(usb_host_ptr, TR_CTRL, msg.msg.tr.pipe_desc, (uint8_t *)&msg.msg.tr.pipe_tr->G.setup_packet, 8);

                        // send next token
                        if (msg.msg.tr.pipe_tr->G.RX_LENGTH) {
                            buf = msg.msg.tr.pipe_tr->G.RX_BUFFER;
                            required = remain = msg.msg.tr.pipe_tr->G.RX_LENGTH;
                            KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required)

                            while (remain > 0) {
                                res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.msg.tr.pipe_desc, buf, remain);

                                if (res < 0)
                                    break;

                                buf += res;
                                remain -=res;
                            }

                            msg.msg.tr.pipe_desc->G.NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_OUT, msg.msg.tr.pipe_desc, 0, 0);
                        }
                        else if (msg.msg.tr.pipe_tr->G.TX_LENGTH) {
                            buf = msg.msg.tr.pipe_tr->G.TX_BUFFER;
                            required = remain = msg.msg.tr.pipe_tr->G.TX_LENGTH;
                            KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required)

                            while (remain > 0) {
                                res = _usb_khci_atom_tr(usb_host_ptr, TR_OUT, msg.msg.tr.pipe_desc, buf, remain);

                                if (res < 0)
                                    break;

                                buf += res;
                                remain -=res;
                            }

                            msg.msg.tr.pipe_desc->G.NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.msg.tr.pipe_desc, 0, 0);
                        }
                        else {
                            msg.msg.tr.pipe_desc->G.NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.msg.tr.pipe_desc, 0, 0);
                            remain = 0;
                        }
                        break;

                    case TR_MSG_RECV:
                        buf = msg.msg.tr.pipe_tr->G.RX_BUFFER;
                        required = remain = msg.msg.tr.pipe_tr->G.RX_LENGTH;
                        KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required);

                        do {
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.msg.tr.pipe_desc, buf, remain);

                            if (res < 0)
                                break;

                            buf += res;
                            remain -=res;

                            if ((remain == 0) || (res < msg.msg.tr.pipe_desc->G.MAX_PACKET_SIZE))
                            {
                                res = remain;
                                break;
                            }

                        } while (1);
                        break;

                    case TR_MSG_SEND:
                        buf = msg.msg.tr.pipe_tr->G.TX_BUFFER;
                        required = remain = msg.msg.tr.pipe_tr->G.TX_LENGTH;
                        KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required)

                        do {
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_OUT, msg.msg.tr.pipe_desc, buf, remain);

                            if (res < 0)
                                break;

                            buf += res;
                            remain -=res;

                            if ((remain == 0) || (res < msg.msg.tr.pipe_desc->G.MAX_PACKET_SIZE))
                            {
                                res = remain;
                                break;
                            }

                        } while (1);
                        break;

                    default:
                       break;
                }

                if (msg.msg.tr.pipe_desc->G.PIPETYPE == USB_INTERRUPT_PIPE) {
                    /* for interrupt pipes, callback only if some data was received or serious error occured */
                    if ((required != remain) || (res != KHCI_ATOM_TR_NAK)) {
                        _usb_khci_process_tr_complete(msg.msg.tr.pipe_desc, msg.msg.tr.pipe_tr, required, remain, res);
                        _usb_khci_rm_int_tr(usb_host_ptr, &msg);
                    }
                }
                else {
                    _usb_khci_process_tr_complete(msg.msg.tr.pipe_desc, msg.msg.tr.pipe_tr, required, remain, res);
                }
            }
        }
        else {
            // wait for event if device is not attached
            _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE, 0);
        }

        if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_MASK) {
            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_ATTACH)
                _usb_khci_attach(usb_host_ptr);

            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_RESET)
                _usb_khci_reset(usb_host_ptr);

            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_DETACH)
                _usb_khci_detach(usb_host_ptr);

            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SHUTDOWN) {
                do {
                    if (_lwmsgq_receive(usb_host_ptr->tr_que, (uint32_t *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0) != MQX_OK) {
                        msg.type = TR_MSG_UNKNOWN;
                        break;
                    }
                } while (msg.type != SHDWN_MSG);
                if (msg.type == SHDWN_MSG) {
                    _lwsem_post(msg.msg.shdwn.end_of_task);
                    /* returning from the infinite loop finishes the current khci task */
                    return;
                }
            }
        }
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task_create
*  Returned Value : error or USB_OK
*  Comments       :
*        Create KHCI task
*END*-----------------------------------------------------------------*/
static USB_STATUS _usb_khci_task_create(_usb_host_handle handle) {
    //USB_STATUS status;
    _task_id task_id;
    TASK_TEMPLATE_STRUCT task_template;

    // check wait time for events - must be > 0
    if (USBCFG_KHCI_WAIT_TICK < 1)
        return USBERR_UNKNOWN_ERROR;

    /* create task for processing interrupt deferred work */
    task_template.TASK_TEMPLATE_INDEX = USB_KHCI_TASK_TEMPLATE_INDEX;
    task_template.TASK_ADDRESS = USB_KHCI_TASK_ADDRESS;
    task_template.TASK_STACKSIZE = USB_KHCI_TASK_STACKSIZE;
    task_template.TASK_PRIORITY = USBCFG_KHCI_TASK_PRIORITY;
    task_template.TASK_NAME = USB_KHCI_TASK_NAME;
    task_template.TASK_ATTRIBUTES = USB_KHCI_TASK_ATTRIBUTES;
    task_template.CREATION_PARAMETER = (uint32_t)handle;   //USB_KHCI_TASK_CREATION_PARAMETER;
    task_template.DEFAULT_TIME_SLICE = USB_KHCI_TASK_DEFAULT_TIME_SLICE;

    task_id = _task_create_blocked(0, 0, (uint32_t)&task_template);

    if (task_id == 0) {
        return USBERR_UNKNOWN_ERROR;
    }

    _task_ready(_task_get_td(task_id));

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_preinit
*  Returned Value : error or USB_OK
*  Comments       :
*        Allocate the structures for KHCI
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_preinit(_usb_host_handle *handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) USB_mem_alloc_zero(sizeof(USB_KHCI_HOST_STATE_STRUCT));
    KHCI_PIPE_STRUCT_PTR p, pp;
    int i;

    if (NULL != usb_host_ptr) {
        /* Allocate the USB Host Pipe Descriptors */
        usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR = (PIPE_STRUCT_PTR)USB_mem_alloc(sizeof(KHCI_PIPE_STRUCT) * USBCFG_MAX_PIPES);
        if (usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR == NULL) {
            USB_mem_free(usb_host_ptr);

            return USBERR_ALLOC;
        }
        USB_mem_zero(usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR, sizeof(KHCI_PIPE_STRUCT) * USBCFG_MAX_PIPES);

        p = (KHCI_PIPE_STRUCT_PTR) usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR;
        pp = NULL;
        for (i = 0; i < USBCFG_MAX_PIPES; i++) {
           if (pp != NULL) {
              pp->G.NEXT = (PIPE_STRUCT_PTR) p;
           }
           pp = p;
           p++;
        }

        usb_host_ptr->G.PIPE_SIZE = sizeof(KHCI_PIPE_STRUCT);
        usb_host_ptr->G.TR_SIZE = sizeof(KHCI_TR_STRUCT);

    } /* Endif */

    *handle = (_usb_host_handle) usb_host_ptr;
#if defined(KHCICFG_4BYTE_ALIGN_FIX)
    if (NULL == (_usb_khci_swap_buf_ptr = (uint8_t *)USB_mem_alloc_uncached_zero(USBCFG_KHCI_SWAP_BUF_MAX + 4)))
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_khci_swap_buf_ptr- memory allocation failed");
#endif
        return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
    }
#endif
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initialize the KHCI for host functionality
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_init(_usb_host_handle handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR)handle;
    USB_KHCI_HOST_INIT_STRUCT_PTR param = (USB_KHCI_HOST_INIT_STRUCT_PTR) usb_host_ptr->G.INIT_PARAM;
    USB_MemMapPtr           usb_ptr;
    USB_STATUS              status = USB_OK;

    /* create lwevent group */
    if (_lwevent_create(&usb_host_ptr->khci_event, 0) != MQX_OK) {
        return USBERR_GET_MEMORY_FAILED;
    }

   if (param == NULL)
       return USBERR_INIT_DATA;

    usb_ptr = (USB_MemMapPtr) param->BASE_PTR;
    usb_host_ptr->G.DEV_PTR = (USB_KHCI_HOST_STATE_STRUCT_PTR) usb_ptr;

    _usb_khci_task_create(usb_host_ptr);

    // install isr
    if (!(USB_install_isr(param->VECTOR, _usb_khci_isr, (void *)usb_host_ptr)))
        return USBERR_INSTALL_ISR;

    usb_ptr->ISTAT = 0xFF;
    /* Enable week pull-downs, usefull for detecting detach (effectivelly bus discharge) */
    usb_ptr->USBCTRL |= USB_USBCTRL_PDE_MASK;
    /* Remove suspend state */
    usb_ptr->USBCTRL &= ~USB_USBCTRL_SUSP_MASK;


    usb_ptr->CTL |= USB_CTL_ODDRST_MASK;

    usb_ptr->BDTPAGE1 = (uint8_t)((uint32_t)BDT_BASE >> 8);
    usb_ptr->BDTPAGE2 = (uint8_t)((uint32_t)BDT_BASE >> 16);
    usb_ptr->BDTPAGE3 = (uint8_t)((uint32_t)BDT_BASE >> 24);

    /* Set SOF threshold */
    usb_ptr->SOFTHLD = 1;

    usb_ptr->CTL = USB_CTL_HOSTMODEEN_MASK;

    /* Following is for OTG control instead of internal bus control */
//    usb_ptr->OTGCTL = USB_OTGCTL_DMLOW_MASK | USB_OTGCTL_DPLOW_MASK | USB_OTGCTL_OTGEN_MASK;

    /* Wait for attach interrupt */
    usb_ptr->INTEN = USB_INTEN_ATTACHEN_MASK;

    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_shutdown
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to shutdown the host. Cannot be run from ISR.
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_shutdown(_usb_host_handle handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR)handle;
    LWSEM_STRUCT task_finish;
    MSG_STRUCT msg;
    USB_KHCI_HOST_INIT_STRUCT_PTR khci_init;
    USB_MemMapPtr usb_ptr;
    void (_CODE_PTR_ default_isr)(void *);

    _lwsem_create(&task_finish, 0);
    default_isr = _int_get_default_isr();
    khci_init = (USB_KHCI_HOST_INIT_STRUCT_PTR)_usb_host_get_init(handle);

    if (khci_init == NULL) {
        return USBERR_DRIVER_NOT_INSTALLED;
    }

    usb_ptr = (USB_MemMapPtr) khci_init->BASE_PTR;
    /* No interrupt anymore interrupt */
    usb_ptr->INTEN = 0;
    /* Stop sending SOF */
    usb_ptr->CTL &= ~USB_CTL_USBENSOFEN_MASK;

    if (!(USB_install_isr(khci_init->VECTOR, default_isr, (void *)handle))) {
        /* install default vector = remove the KHCI ISR from the vector */
        return USBERR_INSTALL_ISR;
    }

    msg.type = SHDWN_MSG;
    /* Cheating. We want to use 'transfers' message queue to issue special message for shutdown.
    ** Instead of pipe descr we use
    */
    msg.msg.shdwn.end_of_task = &task_finish;
    _lwmsgq_send(usb_host_ptr->tr_que, (uint32_t *)&msg, LWMSGQ_SEND_BLOCK_ON_FULL);

    _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_SHUTDOWN);
    /* wait till task finishes */
    _lwsem_wait_ticks(&task_finish, 0);
    _lwmsgq_deinit(&usb_host_ptr->tr_que);

    /* safely destroy structures that are not used anymore by the task */
    _lwevent_destroy(&usb_host_ptr->khci_event);
    _lwsem_destroy(&task_finish);

#if defined(KHCICFG_4BYTE_ALIGN_FIX)
    if (NULL != _usb_khci_swap_buf_ptr)
    {
        USB_mem_free(_usb_khci_swap_buf_ptr);
    }
#endif
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_send
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to send data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_send(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_STATUS status = USB_OK;
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    MSG_STRUCT msg;

    msg.type = TR_MSG_SEND;
    msg.msg.tr.pipe_desc = (KHCI_PIPE_STRUCT_PTR) pipe_ptr;
    msg.msg.tr.pipe_tr = (KHCI_TR_STRUCT_PTR) tr_ptr;

    KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.TX_LENGTH)
    if (MQX_OK != _lwmsgq_send(usb_host_ptr->tr_que, (uint32_t *) &msg, 0)) {
        status = USBERR_TR_FAILED;
    }

    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_send_setup
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to send setup data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_send_setup(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_STATUS status = USB_OK;
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    MSG_STRUCT msg;

    msg.type = TR_MSG_SETUP;
    msg.msg.tr.pipe_desc = (KHCI_PIPE_STRUCT_PTR) pipe_ptr;
    msg.msg.tr.pipe_tr = (KHCI_TR_STRUCT_PTR) tr_ptr;

    if (msg.msg.tr.pipe_tr->G.RX_LENGTH)
      KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.RX_LENGTH)
    else
      KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.TX_LENGTH)

    if (MQX_OK != _lwmsgq_send(usb_host_ptr->tr_que, (uint32_t *) &msg, 0)) {
        status = USBERR_TR_FAILED;
    }

    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_recv
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to receive data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_recv(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_STATUS status = USB_OK;
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    MSG_STRUCT msg;

    msg.type = TR_MSG_RECV;
    msg.msg.tr.pipe_desc = (KHCI_PIPE_STRUCT_PTR) pipe_ptr;
    msg.msg.tr.pipe_tr = (KHCI_TR_STRUCT_PTR) tr_ptr;

    KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.RX_LENGTH)
    if (MQX_OK != _lwmsgq_send(usb_host_ptr->tr_que, (uint32_t *) &msg, 0)) {
        status = USBERR_TR_FAILED;
    }

    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_cancel
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to cancel the transfer
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_cancel(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    MSG_STRUCT msg;

    // remove messages from KHCI msg que
    while (_lwmsgq_receive(usb_host_ptr->tr_que, (uint32_t *) &msg, 0, 0, 0) == MQX_OK)
    { /* void */ }

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_bus_control
*  Returned Value : error or USB_OK
*  Comments       :
*        The function for USB bus control
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_bus_control(_usb_host_handle handle, uint8_t bus_control) {

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_get_frame_number
*  Returned Value : frame number
*  Comments       :
*        The function to get frame number
*END*-----------------------------------------------------------------*/
uint32_t _usb_khci_get_frame_number(_usb_host_handle handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;

    return (usb_ptr->FRMNUMH << 8 | usb_ptr->FRMNUML);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_process_tr_complete
*  Returned Value : none
*  Comments       :
*        Transaction complete
*END*-----------------------------------------------------------------*/
static void _usb_khci_process_tr_complete(
    KHCI_PIPE_STRUCT_PTR       pipe_desc_ptr,
    KHCI_TR_STRUCT_PTR         pipe_tr_ptr,
    uint32_t                    required,
    uint32_t                    remaining,
    int32_t                     err
)
{
    unsigned char *buffer_ptr = NULL;
    uint32_t status = 0;

    KHCI_DEBUG_LOG('o', TR_MSG_UNKNOWN, pipe_desc_ptr->G.ENDPOINT_NUMBER, err)

    if (err == KHCI_ATOM_TR_STALL) {
        status = USBERR_ENDPOINT_STALLED;
    }
    else if ((err == KHCI_ATOM_TR_NAK) || (err >= 0)) {
        /* TODO: only NAK is reported as failure, less data than requested means success */
        if (err == KHCI_ATOM_TR_NAK) //if NAK, then failed, but still make sure that buffer_ptr contains valid data
            status = USBERR_TR_FAILED;
        else
            status = USB_OK;

        if (pipe_desc_ptr->G.PIPETYPE == USB_CONTROL_PIPE) {
            if (pipe_tr_ptr->G.SEND_PHASE) {
                buffer_ptr = pipe_tr_ptr->G.TX_BUFFER;
                pipe_tr_ptr->G.SEND_PHASE = FALSE;
            }
            else {
                buffer_ptr = pipe_tr_ptr->G.RX_BUFFER;
            }
        }
        else {
            if (pipe_desc_ptr->G.DIRECTION) {
                buffer_ptr = pipe_tr_ptr->G.TX_BUFFER;
            }
            else {
                buffer_ptr = pipe_tr_ptr->G.RX_BUFFER;
            }
        }
    }
    else if (err < 0)
        status = USBERR_TR_FAILED;

    pipe_tr_ptr->status = USB_STATUS_IDLE;

    if (pipe_tr_ptr->status == USB_STATUS_IDLE) {
        /* Transfer done. Call the callback function for this
        ** transaction if there is one (usually true).
        */
        if (pipe_tr_ptr->G.CALLBACK != NULL) {
            pipe_tr_ptr->G.CALLBACK((void *)pipe_desc_ptr, pipe_tr_ptr->G.CALLBACK_PARAM,
                buffer_ptr, required - remaining, status);

            /* If the application enqueued another request on this pipe
            ** in this callback context then it will be at the end of the list
            */
        }
    }

    /* don't use pipe anymore */
    pipe_tr_ptr->G.TR_INDEX = 0;

}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_atom_tr
*  Returned Value :
*  Comments       :
*        Atomic transaction
*END*-----------------------------------------------------------------*/
static int32_t _usb_khci_atom_tr(
    USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr,
    uint32_t                    type,
    KHCI_PIPE_STRUCT_PTR       pipe_desc_ptr,
    uint8_t                    *buf_ptr,
    uint32_t                    len
)
{
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;
    uint32_t bd;
    uint32_t *bd_ptr = NULL;
    uint8_t  *buf = buf_ptr;
    int32_t res;
    _mqx_int retry;
    _mqx_int delay_const = 10;
    usb_host_ptr->last_to_pipe = NULL; // at the beginning, consider that there was not timeout

    len = (len > pipe_desc_ptr->G.MAX_PACKET_SIZE) ? pipe_desc_ptr->G.MAX_PACKET_SIZE : len;

    /* ADDR must be written before ENDPT0 (undocumented behavior) for to generate PRE packet */
    usb_ptr->ADDR = (uint8_t)((pipe_desc_ptr->G.SPEED == USB_SPEED_FULL) ? USB_ADDR_ADDR(pipe_desc_ptr->G.DEVICE_ADDRESS) : USB_ADDR_LSEN_MASK | USB_ADDR_ADDR(pipe_desc_ptr->G.DEVICE_ADDRESS));

    if((pipe_desc_ptr->G.PIPETYPE != USB_ISOCHRONOUS_PIPE))
    {
        usb_ptr->ENDPOINT[0].ENDPT =
            (usb_host_ptr->G.SPEED == USB_SPEED_LOW ? USB_ENDPT_HOSTWOHUB_MASK : 0) | USB_ENDPT_RETRYDIS_MASK |
            USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK | USB_ENDPT_EPHSHK_MASK;
    }
    else
    {
        usb_ptr->ENDPOINT[0].ENDPT =
            (usb_host_ptr->G.SPEED == USB_SPEED_LOW ? USB_ENDPT_HOSTWOHUB_MASK : 0) | USB_ENDPT_RETRYDIS_MASK |
            USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK;
    }

    switch (pipe_desc_ptr->G.PIPETYPE) {
        case USB_INTERRUPT_PIPE:
        case USB_ISOCHRONOUS_PIPE:
            retry = 0;
            break;
        default:
            retry = pipe_desc_ptr->G.NAK_COUNT; // set retry count - do not retry interrupt transaction
            break;
    }

    do {
        res = 0;

        // wait for USB ready, but with timeout
        while (usb_ptr->CTL & USB_CTL_TXSUSPENDTOKENBUSY_MASK) {
            if (_lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE, 1) == MQX_OK) {
                res = KHCI_ATOM_TR_RESET;
                break;
            }
        }

        if (!res) {
          // all is ok, do transaction

#if defined(KHCICFG_BASIC_SCHEDULING)
          usb_ptr->ISTAT |= USB_ISTAT_SOFTOK_MASK; //clear SOF
          while (!(usb_ptr->ISTAT & USB_ISTAT_SOFTOK_MASK))
                 /* wait for next SOF */;

          usb_ptr->SOFTHLD = 0;
#else
          if (pipe_desc_ptr->G.SPEED == USB_SPEED_FULL)
              usb_ptr->SOFTHLD = len * 7 / 6 + KHCICFG_THSLD_DELAY;
          else
              usb_ptr->SOFTHLD = len * 12 * 7 / 6 + KHCICFG_THSLD_DELAY;
#endif
          usb_ptr->ISTAT |= USB_ISTAT_SOFTOK_MASK; //clear SOF
          usb_ptr->ERRSTAT = 0xff; //clear error status

#if defined(KHCICFG_4BYTE_ALIGN_FIX)
          if ((TR_IN == type) && (len & USB_DMA_ALIGN_MASK || (_mem_size)buf_ptr & USB_DMA_ALIGN_MASK)) {
              if (_usb_khci_swap_buf_ptr != NULL && len <= USBCFG_KHCI_SWAP_BUF_MAX) {
                  buf = (uint8_t *)USB_MEM4_ALIGN((_mem_size)_usb_khci_swap_buf_ptr);
              }
          }
#endif
          switch (type) {
            case TR_CTRL:
                bd_ptr = (uint32_t*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
                *(bd_ptr + 1) = HOST_TO_LE_LONG((uint32_t)buf);
                *bd_ptr = HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN);
                usb_ptr->TOKEN = (uint8_t)(USB_TOKEN_TOKENENDPT((uint8_t)pipe_desc_ptr->G.ENDPOINT_NUMBER) | USB_TOKEN_TOKENPID_SETUP);
                usb_host_ptr->tx_bd ^= 1;
                break;
            case TR_IN:
                bd_ptr = (uint32_t*) BD_PTR(0, 0, usb_host_ptr->rx_bd);
                *(bd_ptr + 1) = HOST_TO_LE_LONG((uint32_t)buf);
                *bd_ptr = HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN | USB_BD_DATA01(pipe_desc_ptr->G.NEXTDATA01));
                usb_ptr->TOKEN = (uint8_t)(USB_TOKEN_TOKENENDPT(pipe_desc_ptr->G.ENDPOINT_NUMBER) | USB_TOKEN_TOKENPID_IN);
                usb_host_ptr->rx_bd ^= 1;
                break;
            case TR_OUT:
                bd_ptr = (uint32_t*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
                *(bd_ptr + 1) = HOST_TO_LE_LONG((uint32_t)buf);
                *bd_ptr = HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN | USB_BD_DATA01(pipe_desc_ptr->G.NEXTDATA01));
                usb_ptr->TOKEN = (uint8_t)(USB_TOKEN_TOKENENDPT(pipe_desc_ptr->G.ENDPOINT_NUMBER) | USB_TOKEN_TOKENPID_OUT);
                usb_host_ptr->tx_bd ^= 1;
                break;
            default:
                bd_ptr = NULL;
          }

          // wait for end of transaction or other event, max. 100 msec
          if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE, USBCFG_KHCI_WAIT_TICK))
          {
                res = KHCI_ATOM_TR_TO;
                usb_host_ptr->last_to_pipe = pipe_desc_ptr; // remember this pipe as it had timeout last time
                continue;
          }
        }

try_again:
        // check for reset event (detach)
        if (_lwevent_get_signalled() & (KHCI_EVENT_RESET | KHCI_EVENT_SHUTDOWN)) {
            res = KHCI_ATOM_TR_RESET;
            break;
        }

        if (bd_ptr != NULL && (_lwevent_get_signalled() & KHCI_EVENT_TOK_DONE)) {
            // transaction finished
            _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_TOK_DONE);

            // check result
            bd = LONG_LE_TO_HOST(*bd_ptr);

            if (usb_ptr->ERRSTAT & (
                    USB_ERREN_PIDERREN_MASK |
#if defined(KHCICFG_BASIC_SCHEDULING)
                    USB_ERREN_CRC5EOFEN_MASK |
#endif
                    USB_ERREN_CRC16EN_MASK |
                    USB_ERREN_DFN8EN_MASK |
//                    USB_ERREN_BTO_ERR_EN_MASK | //timeout tested elsewhere
                    USB_ERREN_DMAERREN_MASK |
                    USB_ERREN_BTSERREN_MASK))
            {
#if defined(KHCICFG_BASIC_SCHEDULING)
                if (usb_ptr->ERRSTAT & USB_ERREN_CRC5EOFEN_MASK)
                    retry = 0;
#endif
                res = -usb_ptr->ERRSTAT;
                break;
            }
            else
            {
                switch (bd >> 2 & 0xf) {
                    case 0x03:  // DATA0
                    case 0x0b:  // DATA1
                    case 0x02:  // ACK
                        retry = 0;
                        res = (bd >> 16) & 0x3ff;
                        pipe_desc_ptr->G.NEXTDATA01 ^= 1;     // switch data toggle
                        break;

                    case 0x0e:  // STALL
                        res = KHCI_ATOM_TR_STALL;
                        retry = 0;
                        break;

                    case 0x0a:  // NAK
                        res = KHCI_ATOM_TR_NAK;
                        if (retry)
                            _time_delay(delay_const * (pipe_desc_ptr->G.NAK_COUNT - retry));
                        break;

                    case 0x00:  // bus timeout
                        if((pipe_desc_ptr->G.PIPETYPE != USB_ISOCHRONOUS_PIPE))
                        {
                            //wait a bit, but not too much, perhaps some error occurs on the bus
                            if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE, delay_const))
                            {
                                res = KHCI_ATOM_TR_TO;
                                usb_host_ptr->last_to_pipe = pipe_desc_ptr; // remember this pipe as it had timeout last time
                                if (retry)
                                    _time_delay(delay_const * (pipe_desc_ptr->G.NAK_COUNT - retry));
                            }
                            else if (_lwevent_get_signalled() & (KHCI_EVENT_RESET | KHCI_EVENT_SHUTDOWN))
                            {
                                res = KHCI_ATOM_TR_RESET;
                                retry = 0;
                                break;
                            }
                            else
                                goto try_again;
                        }
                        else
                        {
                            retry = 0;
                            res = (bd >> 16) & 0x3ff;
                            pipe_desc_ptr->G.NEXTDATA01 ^= 1;     /* switch data toggle */
                        }
                        break;

                    case 0x0f:  // data error
                        //if this event happens during enumeration, then return means not finished enumeration
                        res = KHCI_ATOM_TR_DATA_ERROR;
                        _time_delay(delay_const * (pipe_desc_ptr->G.NAK_COUNT - retry));
                        break;

                    default:
                        break;
                }
            }
        }
    } while (retry--);

#if defined(KHCICFG_4BYTE_ALIGN_FIX)
    if ((TR_IN == type) && (len & USB_DMA_ALIGN_MASK || (_mem_size)buf_ptr & USB_DMA_ALIGN_MASK)) {
        if (res > 0) {
            USB_mem_copy(buf, buf_ptr, res);
        }
    }
#endif
    return res;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_host_close_pipe
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to close pipe
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_host_close_pipe(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_desc_ptr) {
    USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR)handle;

    if (pipe_desc_ptr->PIPETYPE == USB_INTERRUPT_PIPE) {
        _mqx_int i;
        TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;

        // find record
        for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
            if (tr->message.msg.tr.pipe_desc == (KHCI_PIPE_STRUCT_PTR) pipe_desc_ptr)
                tr->message.type = TR_MSG_UNKNOWN;
        }
    }
    else {
        /* Now, we should check whether there is a transfer in the message pool */
    }
    return USB_OK;
}
