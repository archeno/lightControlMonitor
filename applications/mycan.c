#include "mycan.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "global.h"
#include "io.h"
#include "data_transfrom.h"
#include "can_response.h"
#include <string.h>

#define DBG_TAG "mycan"
#define DBG_LVL  DBG_LOG
#include "rtdbg.h"

#define  CAN_MB_NUM  10
#define  CAN_EVENT_MB_NUM  8
static struct rt_semaphore can0_rx_sem;     	
static struct rt_semaphore can1_rx_sem;     


static rt_uint8_t can_msg_pool[2][4* CAN_MB_NUM];
static rt_uint8_t can_event_mailbox_pool[2][4*CAN_EVENT_MB_NUM];

static rt_err_t can0_rx_call(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&can0_rx_sem);
    return RT_EOK;
}
static rt_err_t can1_rx_call(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&can1_rx_sem);
    return RT_EOK;
}



/**
 * @brief can_rx_thread
 * 
 * @param parameter 
 */
static void can_rx_thread(void *parameter)
{
    rt_err_t res;
	rt_sem_t rx_sem;
	rt_uint8_t *pcan_comm_status = RT_NULL;
	rt_device_t can_dev = (rt_device_t)parameter;
    struct rt_can_msg rxmsg = {0};

	if(can_dev == sys.can.can0_dev)
	{
		 rx_sem = &can0_rx_sem;
		 rt_device_set_rx_indicate(can_dev, can0_rx_call);
		 pcan_comm_status  = &sys.mycan.para.can0_comm_fail_flag;
	}
	else if(can_dev == sys.can.can1_dev)
	{
		rx_sem = &can1_rx_sem;
		rt_device_set_rx_indicate(can_dev, can1_rx_call);	
		pcan_comm_status = &sys.mycan.para.can1_comm_fail_flag;
	}
	
#ifdef RT_CAN_USING_HDR
	struct rt_can_filter_item items[1] =
    {
        RT_CAN_FILTER_ITEM_INIT(0x00, 0, 0, 0, 0x0ff, RT_NULL, RT_NULL)
    };

    struct rt_can_filter_config cfg = {1, 1, items}; 
    res = rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif

	while (1)
    {
		rxmsg.hdr_index = -1;
        if(rt_sem_take(rx_sem, RT_TICK_PER_SECOND*2)!= RT_EOK)
		{
			*pcan_comm_status = 1;
			continue;
		}
		*pcan_comm_status = 0;
        rt_device_read(can_dev, 0, &rxmsg, sizeof(rxmsg));
		can_msg_parse(can_dev, &rxmsg, &sys.mycan);
    }
}

/**
 * @brief can_autoUpload_thread
 * 
 * @param parameter 
 */
// static  void can_autoUpload_thread(void *parameter)
// {
// 	rt_uint32_t  event_rcv;
// 	rt_device_t can_dev  = (rt_device_t)parameter;
// 	rt_event_t event = (can_dev == sys.can.can0_dev)? &sys.mycan.ccr_event_updata0: &sys.mycan.ccr_event_updata1;
// 	while(1)
// 	{
// 		rt_event_recv(event, (EVENT_INTPUT_CHANGED | EVENT_OUTPUT_CHANGED),
// 		(RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR), RT_WAITING_FOREVER, &event_rcv);
// 		if(event_rcv & (EVENT_INTPUT_CHANGED|EVENT_OUTPUT_CHANGED))
// 		{
// 			send_standard_frame(can_dev, CMD_PACK_CCR_IO, NULL);	
// 		}
// 	    rt_thread_mdelay(50);
// 	}
// }

/**
 * @brief can_sync_thread
 * 
 * @param parameter 
 */
static  void can_sync_upload_thread(void *parameter)
{
	rt_uint32_t  event_rcv;
	rt_ubase_t cmd;
	rt_device_t can_dev  = (rt_device_t)parameter;
	rt_mailbox_t can_mb = (sys.can.can0_dev == can_dev)? &sys.mycan.can_sync_mb0: &sys.mycan.can_sync_mb1;
	while(1)
	{
		rt_mb_recv(can_mb, &cmd, RT_WAITING_FOREVER);
		send_standard_frame(can_dev, cmd, NULL);	
	    // rt_thread_mdelay(40);
	}
}

// static void can_alarmEventUpload_thread(void * parameter)
// {
// 	rt_uint32_t  event_rcv;
// 	rt_ubase_t eventCode;
// 	rt_device_t can_dev  = (rt_device_t)parameter;
// 	rt_mailbox_t can_mb = (sys.can.can0_dev == can_dev)? &sys.mycan.ccr_event_mb0: &sys.mycan.ccr_event_mb1;
// 	while(1)
// 	{
// 		rt_mb_recv(can_mb, &eventCode, RT_WAITING_FOREVER);
// 		if((eventCode & 0x7fff) < UPS_EVENT_CODE_COMM_FAIL )
// 		{
// 			// ccr alarm event
// 			LOG_D("send ccr alarm frame");
// 			send_standard_frame(can_dev, CMD_PACK_CCR_ALARM_EVENT, (void*)eventCode);		
// 		}
// 		else
// 		{
// 			eventCode -= 0x0C;
// 			LOG_D("send ups alarm frame");
// 			send_standard_frame(can_dev, CMD_PACK_UPS_ALARM_EVENT, (void*)eventCode);	
// 			// ups alarm event
// 		}
// 	    rt_thread_mdelay(1);
// 	}
// }




/**
 * @brief send_standard_frame according to the msg_cmd 
 * 
 * @param dev 
 * @param msg_cmd 
 */
void  send_standard_frame(rt_device_t dev, uint16_t msg_cmd,  void *arg)
{
	struct rt_can_msg msg;
	dataStream_box_t data;
	memset(&data, 0, sizeof(dataStream_box_t));
	rt_int8_t len=0;
	if(g_send_enable == 0)
        return;
	msg.id = (msg_cmd & 0xff00) | sys.mycan.myCANID;
	msg.ide = RT_CAN_STDID;
	msg.rtr = RT_CAN_DTR;
	msg.data[0] = 0x00;
	msg.data[1] = msg_cmd & 0xff;
	// msg.hdr_index = -1;
	
	if((msg_cmd >>8) == 0x02)
	{
		msg_cmd = 0x02ff;
	}
	can_msg_response_data_pack(msg_cmd, arg, &data, &len);
	if(len >= 0 && len<=6)
	{
		rt_memcpy(&msg.data[2], &data, len);
		msg.len = 2+len;
		rt_device_write(dev, 0, &msg, sizeof(msg));
	}
}



/**
 * @brief can receive message parse
 * 
 * @param can_dev 
 * @param canMsg 
 * @param pcan 
 */
void can_msg_parse(rt_device_t can_dev, rt_can_msg_t canMsg,myCAN * pcan )
{	
	rt_uint16_t cmd;
	data_box_t data;
	can_user_data_s  user_data;;
	// can_data_feild_u data_recv;
	can_id_feild_u  id_recv;

	data.u32 = 0;
	rt_memcpy(user_data.data_feild.data, canMsg->data, canMsg->len);
	user_data.can_dev = can_dev;
	id_recv.canID = canMsg->id;//server id

	cmd = (id_recv.msgID << 8) | (user_data.data_feild.serviceCode);

	if((user_data.data_feild.destMACID != 0xff) && (user_data.data_feild.destMACID != pcan->myCANID))
	{
		return;
	}
	send_standard_frame(can_dev, cmd, &user_data);				
}


/**
 * @brief can_thtead_create
 * 
 * @return int 
 */
static int can_thtead_create(void)
{
	rt_err_t res;
    rt_thread_t thread;

	rt_mb_init(&sys.mycan.can_sync_mb0, "can0_mb_sync", can_msg_pool[0], CAN_MB_NUM, RT_IPC_FLAG_FIFO);
	// rt_mb_init(&sys.mycan.ccr_event_mb0, "can0_mb_event", can_event_mailbox_pool[0],CAN_EVENT_MB_NUM, RT_IPC_FLAG_FIFO );

	sys.can.can0_dev = rt_device_find("can0");
	rt_sem_init(&can0_rx_sem, "can0_rx_sem", 0, RT_IPC_FLAG_FIFO);
	res = rt_device_open(sys.can.can0_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
	RT_ASSERT(res == RT_EOK);


    thread = rt_thread_create("can0_rx", can_rx_thread, sys.can.can0_dev, 2048, 12, 10);
	if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("create can0_rx thread failed!\n");
    }
	thread = rt_thread_create("can0_sync_upload", can_sync_upload_thread, sys.can.can0_dev, 2048, 14, 10);
	if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
	

    // thread = rt_thread_create("can0_autoUpload", can_autoUpload_thread, sys.can.can0_dev, 1024, 10, 10);
	// if (thread != RT_NULL)
    // {
    //     rt_thread_startup(thread);
    // }
    // else
    // {
    //     rt_kprintf("create can0_autoUpload thread failed!\n");
    // }
    // // alarm event upload
	// thread = rt_thread_create("can0_alarmEventUpload", can_alarmEventUpload_thread, sys.can.can0_dev, 1024, 8, 10);
	// if (thread != RT_NULL)
    // {
    //     rt_thread_startup(thread);
    // }
    // else
    // {
    //     rt_kprintf("create can0_alarmEventUpload thread failed!\n");
    // }

	rt_mb_init(&sys.mycan.can_sync_mb1, "can1_mb_sync", can_msg_pool[1], CAN_MB_NUM, RT_IPC_FLAG_FIFO);
	// rt_mb_init(&sys.mycan.ccr_event_mb1, "can1_mb_event", can_event_mailbox_pool[1],CAN_EVENT_MB_NUM, RT_IPC_FLAG_FIFO );
	sys.can.can1_dev = rt_device_find("can1");
	rt_sem_init(&can1_rx_sem, "can1_rx_sem", 0, RT_IPC_FLAG_FIFO);
	res = rt_device_open(sys.can.can1_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
	RT_ASSERT(res == RT_EOK);
	
    thread = rt_thread_create("can1_rx", can_rx_thread, sys.can.can1_dev, 1024, 13, 10);
	if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("create can1_rx thread failed!\n");
    }

	thread = rt_thread_create("can1_sync_upload", can_sync_upload_thread, sys.can.can1_dev, 2048, 15, 10);
	if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    // thread = rt_thread_create("can1_autoUpload", can_autoUpload_thread, sys.can.can1_dev, 1024, 11, 10);
	// if (thread != RT_NULL)
    // {
    //     rt_thread_startup(thread);
    // }
    // else
    // {
    //     rt_kprintf("create can1_autoUpload thread failed!\n");
    // }
	// alarm event upload
	// thread = rt_thread_create("can1_alarmEventUpload", can_alarmEventUpload_thread, sys.can.can1_dev, 1024, 9, 10);
	// if (thread != RT_NULL)
    // {
    //     rt_thread_startup(thread);
    // }
    // else
    // {
    //     rt_kprintf("create can1_alarmEventUpload thread failed!\n");
    // }
	return res;
}

INIT_APP_EXPORT(can_thtead_create);











static rt_timer_t timer;


static void timeout(void *parameter)
{
	myCAN* pcan = (myCAN*)parameter;
	
	if(pcan->para.ms < 65530)
	{
		pcan->para.ms++;
	}
	
	if(pcan->para.ms >= 1000)
	{
		pcan->para.ms = 0;
		pcan->para.seconds++;
//		rt_kprintf("1S\n");
	}
	
	if(pcan->timer < 65530)
	{
		pcan->timer++;
	}

	if(pcan->timer > 1000)
	{
		pcan->timer -= 1000;
	}
}

void myCANInit(myCAN* pcan)
{
	pcan->myCANGroupID = GROUP_ID;
	pcan->myCANNodeID = NODE_ID;  

	rt_event_init(&pcan->ccr_event_updata0, "event_update0", RT_IPC_FLAG_FIFO);
	rt_event_init(&pcan->ccr_event_updata1, "event_update1", RT_IPC_FLAG_FIFO);

	pcan->init = myCANInit;
	timer = rt_timer_create("timer", timeout, (void*)pcan, 1, RT_TIMER_FLAG_PERIODIC);
	if (timer != RT_NULL)
	{
		rt_timer_start(timer);
	}
}









