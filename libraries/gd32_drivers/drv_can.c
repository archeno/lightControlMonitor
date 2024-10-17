/*
 * Copyright (c) 2022 �����к�������ɫ�����������޹�˾
 *
 * 
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     DC      		first implementation
 */

#include "drv_can.h"
#include <string.h>
#include "rtdevice.h"

#ifdef RT_USING_CAN

#if !defined(BSP_USING_CAN0) && !defined(BSP_USING_CAN1)
#error "Please define at least one CANx"

#endif

#include <rtdevice.h>

static can_parameter_struct can_parameter;
static can_filter_parameter_struct can_filter;

static void GD32_CAN_IRQHandler(struct rt_can_device *can, rt_uint8_t fifo);

#if defined(BSP_USING_CAN0)
struct rt_can_device can0;

void CAN0_TX_IRQHandler(void)
{
	rt_interrupt_enter();

	if (can_interrupt_flag_get(CAN0, CAN_INT_FLAG_MTF0) != RESET)
	{		
		if(can_flag_get(CAN0, CAN_FLAG_MTF0) != RESET)
		{
			rt_hw_can_isr(&can0, RT_CAN_EVENT_TX_DONE | 0 << 8);
		}
		else
		{
			rt_hw_can_isr(&can0, RT_CAN_EVENT_TX_FAIL | 0 << 8);
		}
		
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_MTF0);
	}
	else if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_MTF1) != RESET)
	{
		if(can_flag_get(CAN0, CAN_FLAG_MTF1) != RESET)
		{
			rt_hw_can_isr(&can0, RT_CAN_EVENT_TX_DONE | 1 << 8);
		}
		else
		{
			rt_hw_can_isr(&can0, RT_CAN_EVENT_TX_FAIL | 1 << 8);
		}
		
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_MTF1);
	}
	else if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_MTF2) != RESET)
	{
		if(can_flag_get(CAN0, CAN_FLAG_MTF2) != RESET)
		{
			rt_hw_can_isr(&can0, RT_CAN_EVENT_TX_DONE | 2 << 8);
		}
		else
		{
			rt_hw_can_isr(&can0, RT_CAN_EVENT_TX_FAIL | 2 << 8);
		}
		
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_MTF2);
	}
	
	rt_interrupt_leave();
}

void CAN0_RX0_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
	//rt_kprintf("CAN0_RX0_IRQHandler\n");  //debug
    GD32_CAN_IRQHandler(&can0, CAN_FIFO0);

    /* leave interrupt */
    rt_interrupt_leave();
}

void CAN0_RX1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

	//rt_kprintf("CAN0_RX1_IRQHandler\n");  //debug
    GD32_CAN_IRQHandler(&can0, CAN_FIFO1);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_CAN0 */

#if defined(BSP_USING_CAN1)
struct rt_can_device can1;

void CAN1_TX_IRQHandler(void)
{
	rt_interrupt_enter();
	
	if (can_interrupt_flag_get(CAN1, CAN_INT_FLAG_MTF0) != RESET)
	{		
		if(can_flag_get(CAN1, CAN_FLAG_MTF0) != RESET)
		{
			rt_hw_can_isr(&can1, RT_CAN_EVENT_TX_DONE | 0 << 8);
		}
		else
		{
			rt_hw_can_isr(&can1, RT_CAN_EVENT_TX_FAIL | 0 << 8);
		}
		
		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_MTF0);
	}
	else if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_MTF1) != RESET)
	{
		if(can_flag_get(CAN1, CAN_FLAG_MTF1) != RESET)
		{
			rt_hw_can_isr(&can1, RT_CAN_EVENT_TX_DONE | 1 << 8);
		}
		else
		{
			rt_hw_can_isr(&can1, RT_CAN_EVENT_TX_FAIL | 1 << 8);
		}
		
		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_MTF1);
	}
	else if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_MTF2) != RESET)
	{
		if(can_flag_get(CAN1, CAN_FLAG_MTF2) != RESET)
		{
			rt_hw_can_isr(&can1, RT_CAN_EVENT_TX_DONE | 2 << 8);
		}
		else
		{
			rt_hw_can_isr(&can1, RT_CAN_EVENT_TX_FAIL | 2 << 8);
		}
		
		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_MTF2);
	}
	
	rt_interrupt_leave();
}

void CAN1_RX0_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

	//rt_kprintf("CAN1_RX0_IRQHandler\n");  //debug
    GD32_CAN_IRQHandler(&can1, CAN_FIFO0);

    /* leave interrupt */
    rt_interrupt_leave();
}

void CAN1_RX1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

	//rt_kprintf("CAN1_RX1_IRQHandler\n");  //debug
    GD32_CAN_IRQHandler(&can1, CAN_FIFO1);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_CAN1 */

static const struct gd32_can can_obj[] = {
    #ifdef BSP_USING_CAN0
    {
        CAN0,									// can peripheral index
		CAN0_TX_IRQn,							// can tx_iqrn
        CAN0_RX0_IRQn,							// can rx_iqrn
        RCU_CAN0, RCU_GPIOB, RCU_GPIOB,			// periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOB, GPIO_AF_9, GPIO_PIN_9,			// tx port, tx alternate, tx pin
        GPIOB, GPIO_AF_9, GPIO_PIN_8,			// rx port, rx alternate, rx pin
#else
        GPIOA, GPIO_PIN_9,           			// tx port, tx pin
        GPIOA, GPIO_PIN_10,          			// rx port, rx pin
#endif

        &can0,
        "can0",
    },
    #endif

    #ifdef BSP_USING_CAN1
    {
        CAN1,									// can peripheral index
        CAN1_TX_IRQn,							// can tx_iqrn
        CAN1_RX0_IRQn,							// can rx_iqrn
        RCU_CAN1, RCU_GPIOB, RCU_GPIOB,			// periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOB, GPIO_AF_9, GPIO_PIN_13,			// tx port, tx alternate, tx pin
        GPIOB, GPIO_AF_9, GPIO_PIN_12,			// rx port, rx alternate, rx pin
#else
        GPIOA, GPIO_PIN_2,						// tx port, tx pin
        GPIOA, GPIO_PIN_3,						// rx port, rx pin
#endif

        &can1,
        "can1",
    },
    #endif
};

/**
* @brief CAN MSP Initialization
*        This function configures the hardware resources used in this example:
*           - Peripheral's clock enable
*           - Peripheral's GPIO Configuration
*           - NVIC configuration for CAN interrupt request enable
* @param hcan: CAN handle pointer
* @retval None
*/
void gd32_can_gpio_init(struct gd32_can *drv_can)
{
    /* enable CAN clock */
	//When using CAN1 alone, you also need to turn on the clock of CAN0
	rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(drv_can->tx_gpio_clk);
    rcu_periph_clock_enable(drv_can->rx_gpio_clk);
    rcu_periph_clock_enable(drv_can->per_clk);

#if defined SOC_SERIES_GD32F4xx
    /* connect port to CANx_Tx */
    gpio_af_set(drv_can->tx_port, drv_can->tx_af, drv_can->tx_pin);

    /* connect port to CANx_Rx */
    gpio_af_set(drv_can->rx_port, drv_can->rx_af, drv_can->rx_pin);

    /* configure CAN Tx as alternate function push-none */
    gpio_mode_set(drv_can->tx_port, GPIO_MODE_AF, GPIO_PUPD_NONE, drv_can->tx_pin);
    gpio_output_options_set(drv_can->tx_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, drv_can->tx_pin);

    /* configure CAN Rx as alternate function push-none */
    gpio_mode_set(drv_can->rx_port, GPIO_MODE_AF, GPIO_PUPD_NONE, drv_can->rx_pin);
    gpio_output_options_set(drv_can->rx_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, drv_can->rx_pin);

#else
    /* connect port to CANx_Tx */
    gpio_init(drv_can->tx_port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, drv_can->tx_pin);

    /* connect port to CANx_Rx */
    gpio_init(drv_can->rx_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, drv_can->rx_pin);
#endif
}

/**
  * @brief  can configure
  * @param  can, cfg
  * @retval None
  */
static rt_err_t gd32_can_configure(struct rt_can_device *can, struct can_configure *cfg)
{
    struct gd32_can *drv_can;

    RT_ASSERT(can != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);
    drv_can = (struct gd32_can *)can->parent.user_data;
	RT_ASSERT(drv_can != RT_NULL);

    gd32_can_gpio_init(drv_can);

	can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
	can_deinit(drv_can->can_periph);
	
	can_parameter.time_triggered = DISABLE;				//ʱ�䴥��
    can_parameter.auto_bus_off_recovery = ENABLE;		//�Զ�����
    can_parameter.auto_wake_up = ENABLE;				//�Զ�����
    can_parameter.auto_retrans = ENABLE;				//�Զ��ط�
    can_parameter.rec_fifo_overwrite = DISABLE;			//����FIFO������
    can_parameter.trans_fifo_order = ENABLE;			//��ʶ��С���ȷ��ͣ��������ȼ�
    
    switch (cfg->mode)
    {
    case RT_CAN_MODE_NORMAL:
        can_parameter.working_mode = CAN_NORMAL_MODE;	//����ģʽ
        break;
	case RT_CAN_MODE_LISTEN:
        can_parameter.working_mode = CAN_SILENT_MODE;	//��Ĭģʽ
        break;
    case RT_CAN_MODE_LOOPBACK:
        can_parameter.working_mode = CAN_LOOPBACK_MODE;	
        break;
    case RT_CAN_MODE_LOOPBACKANLISTEN:
        can_parameter.working_mode = CAN_SILENT_LOOPBACK_MODE;
        break;
    default:
        can_parameter.working_mode = CAN_NORMAL_MODE;	
        break;
    }

	//GD32F425XX APB1 50MHz(max)
	can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_13TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_2TQ;
	
	switch (cfg->baud_rate)
	{
	case CAN1MBaud:
        can_parameter.prescaler = 5;
        break;
	case CAN500kBaud:
        can_parameter.prescaler = 10;
        break;
    case CAN250kBaud:
        can_parameter.prescaler = 20;
        break;
	case CAN125kBaud:
        can_parameter.prescaler = 25;
        break;
    case CAN100kBaud:
        can_parameter.prescaler = 50;
        break;
	case CAN50kBaud:
        can_parameter.prescaler = 100;
        break;
	case CAN20kBaud:
        can_parameter.prescaler = 250;
        break;
	case CAN10kBaud:
        can_parameter.prescaler = 500;
        break;
    default:
        can_parameter.prescaler = 10;
        break;
	}

	if(SUCCESS != can_init(drv_can->can_periph, &can_parameter))
	{
		rt_kprintf("error!\n");
		return -RT_ERROR;
	}
	
	can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);
	
	/* initialize filter */ 
	 if(CAN0 == drv_can->can_periph)
	{
		can_filter.filter_number = 0;
	}
	else if(CAN1 == drv_can->can_periph)
	{
		can_filter.filter_number = 15;
	}
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000;
	
//	can_filter.filter_mode = CAN_FILTERMODE_MASK;
//    can_filter.filter_bits = CAN_FILTERBITS_16BIT;
//    can_filter.filter_list_high = 0x0000;
//    can_filter.filter_list_low = 0x0000;
//    can_filter.filter_mask_high = 0xFFFF;
//    can_filter.filter_mask_low = 0xFFFF;
	
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
	
    can_filter_init(&can_filter);
	
	NVIC_SetPriority(drv_can->rx_irqn, 0);
	NVIC_SetPriority(drv_can->tx_irqn, 0);
    //NVIC_EnableIRQ(drv_can->irqn);
	
    return RT_EOK;
}

/**
  * @brief  can control
  * @param  can, arg
  * @retval None
  */
static rt_err_t gd32_can_control(struct rt_can_device *can, int cmd, void *arg)
{
	rt_uint32_t argval;
    struct gd32_can *drv_can;
//	struct rt_can_filter_config *filter_cfg;

    RT_ASSERT(can != RT_NULL);
    drv_can = (struct gd32_can *)can->parent.user_data;
	RT_ASSERT(drv_can != RT_NULL);
	
    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
		argval = (rt_uint32_t) arg;
		if (argval == RT_DEVICE_FLAG_INT_RX)
		{
			/* disable rx irq */
			NVIC_DisableIRQ(drv_can->rx_irqn);
			/* disable interrupt */
			can_interrupt_disable(drv_can->can_periph, CAN_INT_RFNE0);
			can_interrupt_disable(drv_can->can_periph, CAN_INT_RFF0);
			can_interrupt_disable(drv_can->can_periph, CAN_INT_RFO0);
		}
		else if (argval == RT_DEVICE_FLAG_INT_TX)
		{
			/* disable tx irq */
			NVIC_DisableIRQ(drv_can->tx_irqn);
			/* disable interrupt */
			can_interrupt_disable(drv_can->can_periph, CAN_INT_TME);
		}
        break;
    case RT_DEVICE_CTRL_SET_INT:
		argval = (rt_uint32_t) arg;
		if (argval == RT_DEVICE_FLAG_INT_RX)
		{
			/* enable rx irq */
			NVIC_EnableIRQ(drv_can->rx_irqn);
			/* enable interrupt */
			can_interrupt_enable(drv_can->can_periph, CAN_INT_RFNE0);
			can_interrupt_enable(drv_can->can_periph, CAN_INT_RFF0);
			can_interrupt_enable(drv_can->can_periph, CAN_INT_RFO0);
		}
		else if (argval == RT_DEVICE_FLAG_INT_TX)
		{
			/* enable tx irq */
			NVIC_EnableIRQ(drv_can->tx_irqn);
			/* enable interrupt */
			can_interrupt_enable(drv_can->can_periph, CAN_INT_TME);
		}
        break;
	case RT_CAN_CMD_SET_FILTER:
		if (RT_NULL == arg)
		{
			if(CAN0 == drv_can->can_periph)
			{
				can_filter.filter_number = 0;
			}

			else if(CAN1 == drv_can->can_periph)
			{
				can_filter.filter_number = 15;
			}
			/* default filter config */
            can_filter_init(&can_filter);
		}
		else
		{
			struct rt_can_filter_config *filter_cfg = (struct rt_can_filter_config*)arg;
				
			/* get default filter */
			for (int i = 0; i < filter_cfg->count; i++)
			{
				  if (filter_cfg->items[i].hdr_bank == -1)
                {
                    /* use default filter bank settings */
				    if(CAN0 == drv_can->can_periph)
					{
						can_filter.filter_number = i;
					}
					else if(CAN1 == drv_can->can_periph)
					{
						can_filter.filter_number = 14+i;
					}
                }
                else
                {
                    /* use user-defined filter bank settings */
                    can_filter.filter_number = filter_cfg->items[i].hdr_bank;
                }
				
				
				can_filter.filter_list_high = filter_cfg->items[i].id<<5;
				can_filter.filter_list_low = filter_cfg->items[i].ide << 2 | 
												(filter_cfg->items[i].rtr << 1);
				
				
				can_filter.filter_mask_high = filter_cfg->items[i].mask<<5;
				can_filter.filter_mask_low = filter_cfg->items[i].ide << 2 | 
												(filter_cfg->items[i].rtr << 1);
				
//				can_filter.filter_list_high = (filter_cfg->items[i].id >> 13) & 0xFFFF;
//				can_filter.filter_list_low = ((filter_cfg->items[i].id << 3) | 
//												(filter_cfg->items[i].ide << 2) | 
//												(filter_cfg->items[i].rtr << 1)) & 0xFFFF;
//				can_filter.filter_mask_high = (filter_cfg->items[i].mask >> 16) & 0xFFFF;
//				can_filter.filter_mask_low = filter_cfg->items[i].mask & 0xFFFF;
				
				can_filter.filter_mode = filter_cfg->items[i].mode;
				can_filter.filter_fifo_number =    CAN_FIFO0;
				can_filter.filter_enable = ENABLE;
			    can_filter_init(&can_filter);
			}
		}
		break;
	case RT_CAN_CMD_SET_MODE:
		argval = (rt_uint32_t) arg;
		if (argval != RT_CAN_MODE_NORMAL &&
                argval != RT_CAN_MODE_LISTEN &&
                argval != RT_CAN_MODE_LOOPBACK &&
                argval != RT_CAN_MODE_LOOPBACKANLISTEN)
        {
            return -RT_ERROR;
        }
		if (argval != drv_can->can->config.mode)
        {
            drv_can->can->config.mode = argval;
            return gd32_can_configure(drv_can->can, &drv_can->can->config);
        }
		break;
	case RT_CAN_CMD_SET_BAUD:
		argval = (rt_uint32_t) arg;
		if (argval != CAN1MBaud &&
                argval != CAN500kBaud &&
                argval != CAN250kBaud &&
				argval != CAN125kBaud &&
                argval != CAN100kBaud &&
                argval != CAN50kBaud  &&
				argval != CAN20kBaud  &&
                argval != CAN10kBaud)
        {
            return -RT_ERROR;
        }
		if (argval != drv_can->can->config.baud_rate)
        {
            drv_can->can->config.baud_rate = argval;
            return gd32_can_configure(drv_can->can, &drv_can->can->config);
        }
		break;
	case RT_CAN_CMD_SET_PRIV:
		argval = (rt_uint32_t) arg;
		if (argval != RT_CAN_MODE_PRIV &&
                argval != RT_CAN_MODE_NOPRIV)
        {
            return -RT_ERROR;
        }
		if (argval != drv_can->can->config.privmode)
        {
            drv_can->can->config.privmode = argval;
            return gd32_can_configure(drv_can->can, &drv_can->can->config);
        }
        break;
	case RT_CAN_CMD_GET_STATUS:
		break;
    }

    return RT_EOK;
}

/**
  * @brief  can sendmsg
  * @param  can, buf, box_num
  * @retval None
  */
static int gd32_can_sendmsg(struct rt_can_device *can, const void *buf, rt_uint32_t box_num)
{
    struct gd32_can *drv_can;
    RT_ASSERT(can != RT_NULL);
	
    drv_can = (struct gd32_can *)can->parent.user_data;
	RT_ASSERT(drv_can != RT_NULL);
	
	struct rt_can_msg *pmsg = (struct rt_can_msg *) buf;
	
	can_trasnmit_message_struct transmit_message;
	can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &transmit_message);
	/* Check the parameters */
    RT_ASSERT(pmsg->len <= 8U);
	
	switch(box_num)
	{
	case CAN_MAILBOX0:
		if(CAN_TSTAT_TME0 != (CAN_TSTAT(drv_can->can_periph)&CAN_TSTAT_TME0))
		{
			return -RT_ERROR;
		}
		break;
	case CAN_MAILBOX1:
		if(CAN_TSTAT_TME1 == (CAN_TSTAT(drv_can->can_periph)&CAN_TSTAT_TME1))
		{
			return -RT_ERROR;
		}
		break;
	case CAN_MAILBOX2:
		if(CAN_TSTAT_TME2 == (CAN_TSTAT(drv_can->can_periph)&CAN_TSTAT_TME2))
		{
			return -RT_ERROR;
		}
		break;
	default:
		RT_ASSERT(0);
		break;
	}
	
	if (RT_CAN_STDID == pmsg->ide)
	{
		transmit_message.tx_ff = CAN_FF_STANDARD;
		RT_ASSERT(pmsg->id <= 0x7FFU);
		transmit_message.tx_sfid = pmsg->id;
	}
	else
	{
		transmit_message.tx_ff = CAN_FF_EXTENDED;
		RT_ASSERT(pmsg->id <= 0x1FFFFFFFU);
		transmit_message.tx_efid = pmsg->id;
	}
	
	if (RT_CAN_DTR == pmsg->rtr)
	{
		transmit_message.tx_ft = CAN_FT_DATA;
	}
	else
	{
		transmit_message.tx_ft = CAN_FT_REMOTE;
	}
	
	/* Set up the DLC */
    transmit_message.tx_dlen = pmsg->len & 0x0FU;
	
	for(int i = 0; i < transmit_message.tx_dlen; i++)
	{
		transmit_message.tx_data[i] = pmsg->data[i];
	}
	
	CAN_TMI(drv_can->can_periph, box_num) &= CAN_TMI_TEN;
	if(CAN_FF_STANDARD == transmit_message.tx_ff){
        /* set transmit mailbox standard identifier */
        CAN_TMI(drv_can->can_periph, box_num) |= (uint32_t)(TMI_SFID(transmit_message.tx_sfid) | \
                                                transmit_message.tx_ft);
    }else{
        /* set transmit mailbox extended identifier */
        CAN_TMI(drv_can->can_periph, box_num) |= (uint32_t)(TMI_EFID(transmit_message.tx_efid) | \
                                                transmit_message.tx_ff | \
                                                transmit_message.tx_ft);
    }
	
	
	transmit_message.tx_dlen &= (uint8_t)(CAN_TMP_DLENC);
    CAN_TMP(drv_can->can_periph, box_num) &= ((uint32_t)~CAN_TMP_DLENC);
    CAN_TMP(drv_can->can_periph, box_num) |= transmit_message.tx_dlen;
	
	/* set the data */
    CAN_TMDATA0(drv_can->can_periph, box_num) = TMDATA0_DB3(transmit_message.tx_data[3]) | \
                                              TMDATA0_DB2(transmit_message.tx_data[2]) | \
                                              TMDATA0_DB1(transmit_message.tx_data[1]) | \
                                              TMDATA0_DB0(transmit_message.tx_data[0]);
    CAN_TMDATA1(drv_can->can_periph, box_num) = TMDATA1_DB7(transmit_message.tx_data[7]) | \
                                              TMDATA1_DB6(transmit_message.tx_data[6]) | \
                                              TMDATA1_DB5(transmit_message.tx_data[5]) | \
                                              TMDATA1_DB4(transmit_message.tx_data[4]);
    /* enable transmission */
    CAN_TMI(drv_can->can_periph, box_num) |= CAN_TMI_TEN;
	
	//uint8_t mailbox_number = can_message_transmit(drv_can->can_periph, &transmit_message);

	//uint32_t timeout = 0xFFFF;
	/* waiting for transmit completed */
//    while((CAN_TRANSMIT_OK != can_transmit_states(drv_can->can_periph, mailbox_number)) && (0 != timeout)){
//        timeout--;
//    }
//	rt_kprintf("timeout = %d\n",timeout);
	
    return RT_EOK;
}

/**
  * @brief  can recvmsg
  * @param  can, buf, fifo
  * @retval None
  */
static int gd32_can_recvmsg(struct rt_can_device *can, void *buf, rt_uint32_t fifo)
{
	//rt_kprintf("gd32_can_recvmsg\n");  //debug
    struct gd32_can *drv_can;
    RT_ASSERT(can != RT_NULL);
	
    drv_can = (struct gd32_can *)can->parent.user_data;
	RT_ASSERT(drv_can != RT_NULL);
	
	struct rt_can_msg *pmsg = (struct rt_can_msg *) buf;
	
	can_receive_message_struct receive_message;
	can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &receive_message);
	
	//rt_kprintf("gd32_can_recvmsg1\n");  //debug
	can_message_receive(drv_can->can_periph, (uint8_t)fifo, &receive_message);
	//rt_kprintf("gd32_can_recvmsg2\n");  //debug
	
	/* get id */
    if (CAN_FF_STANDARD == receive_message.rx_ff)
    {
        pmsg->ide = RT_CAN_STDID;
        pmsg->id = receive_message.rx_sfid;
    }
    else
    {
        pmsg->ide = RT_CAN_EXTID;
        pmsg->id = receive_message.rx_efid;
    }
	
	/* get type */
    if (CAN_FT_DATA == receive_message.rx_ft)
    {
        pmsg->rtr = RT_CAN_DTR;
    }
    else
    {
        pmsg->rtr = RT_CAN_RTR;
    }
	
	/* get len */
    pmsg->len = receive_message.rx_dlen;
	
	for(int i = 0; i < receive_message.rx_dlen; i++)
	{
		pmsg->data[i] = receive_message.rx_data[i];
	}
	
	/* get hdr */
	if(CAN0 == drv_can->can_periph)
	{
		pmsg->hdr_index = (receive_message.rx_fi + 1) >> 1;
	}
	else if(CAN1 == drv_can->can_periph)
	{
		pmsg->hdr_index = (receive_message.rx_fi >> 1) + 14;
	}
	
    return RT_EOK;
}

/**
 * can common interrupt process.
 *
 * @param can fifo
 */
static void GD32_CAN_IRQHandler(struct rt_can_device *can, rt_uint8_t fifo)
{
	struct gd32_can *drv_can;
    RT_ASSERT(can != RT_NULL);
	
    drv_can = (struct gd32_can *)can->parent.user_data;
	RT_ASSERT(drv_can != RT_NULL);
	
	switch (fifo)
	{
	case CAN_FIFO0:
//		if((can_interrupt_flag_get(drv_can->can_periph, CAN_INT_FLAG_RFL0) != RESET) && 
//			(can_receive_message_length_get(drv_can->can_periph, CAN_FIFO0)))
		if(can_receive_message_length_get(drv_can->can_periph, CAN_FIFO0))
		{
			//rt_kprintf("GD32_CAN_IRQHandler1\n");  //debug
			rt_hw_can_isr(can, RT_CAN_EVENT_RX_IND | fifo << 8);
		}
		
		if ((can_interrupt_flag_get(drv_can->can_periph, CAN_INT_FLAG_RFF0) != RESET) &&
           (can_flag_get(drv_can->can_periph, CAN_FLAG_RFF0) != RESET))
		{
			rt_kprintf("GD32_CAN_IRQHandler2\n");  //debug
			can_flag_clear(drv_can->can_periph, CAN_FLAG_RFF0);
		}
		
		if ((can_interrupt_flag_get(drv_can->can_periph, CAN_INT_FLAG_RFO0) != RESET) &&
           (can_flag_get(drv_can->can_periph, CAN_FLAG_RFO0) != RESET))
		{
			rt_kprintf("GD32_CAN_IRQHandler3\n");  //debug
			can_flag_clear(drv_can->can_periph, CAN_FLAG_RFO0);
			rt_hw_can_isr(can, RT_CAN_EVENT_RXOF_IND | fifo << 8);
		}
		break;
	case CAN_FIFO1:
		if(can_interrupt_flag_get(drv_can->can_periph, CAN_INT_FLAG_RFL1) != RESET)
		{
			rt_hw_can_isr(can, RT_CAN_EVENT_RX_IND | fifo << 8);
		}
		
		if ((can_interrupt_flag_get(drv_can->can_periph, CAN_INT_FLAG_RFF1) != RESET) &&
           (can_flag_get(drv_can->can_periph, CAN_FLAG_RFF1) != RESET))
		{
			can_flag_clear(drv_can->can_periph, CAN_FLAG_RFF1);
		}
		
		if ((can_interrupt_flag_get(drv_can->can_periph, CAN_INT_FLAG_RFO1) != RESET) &&
           (can_flag_get(drv_can->can_periph, CAN_FLAG_RFO1) != RESET))
		{
			can_flag_clear(drv_can->can_periph, CAN_FLAG_RFO1);
			rt_hw_can_isr(can, RT_CAN_EVENT_RXOF_IND | fifo << 8);
		}
		break;
	}
}

static const struct rt_can_ops gd32_can_ops =
{
    .configure = gd32_can_configure,
    .control = gd32_can_control,
    .sendmsg = gd32_can_sendmsg,
    .recvmsg = gd32_can_recvmsg,
};

/**
  * @brief  can init
  * @param  None
  * @retval None
  */
int gd32_hw_can_init(void)
{
    struct can_configure config = CANDEFAULTCONFIG;
	config.privmode = RT_CAN_MODE_NOPRIV;
	config.ticks = 50;
#ifdef RT_CAN_USING_HDR
	config.maxhdr = 14;
#ifdef CAN1
	config.maxhdr = 28;
#endif
#endif
    int i;
    int result;

    for (i = 0; i < sizeof(can_obj) / sizeof(can_obj[0]); i++)
    {
        can_obj[i].can->config = config;

        /* register CAN device */
        result = rt_hw_can_register(can_obj[i].can,
                              can_obj[i].device_name,
                              &gd32_can_ops,
                              (void *)&can_obj[i]);
        RT_ASSERT(result == RT_EOK);
    }

    return result;
}

INIT_BOARD_EXPORT(gd32_hw_can_init);

#endif
