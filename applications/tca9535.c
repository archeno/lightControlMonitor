
#include "tca9535.h"
#include "event_monitor.h"
static struct rt_i2c_bus_device *i2c_bus = RT_NULL; /* I2C�����豸���? */
rt_thread_t IOcheckThread = RT_NULL;
/* д�������Ĵ��� */
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint16_t addr, rt_uint8_t reg, rt_uint8_t data)
{
	rt_uint8_t buf[3];
	struct rt_i2c_msg msgs;
	rt_uint32_t buf_size = 1;

	buf[0] = reg; // cmd
	buf[1] = data;
	buf_size = 2;

	msgs.addr = addr;
	msgs.flags = RT_I2C_WR;
	msgs.buf = buf;
	msgs.len = buf_size;

	/* ����I2C�豸�ӿڴ������� */
	if (rt_i2c_transfer(bus, &msgs, 1) == 1)
	{
		return RT_EOK;
	}
	else
	{
		return -RT_ERROR;
	}
}

/* ���������Ĵ������� */
static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint16_t addr, rt_uint8_t len, rt_uint8_t *buf)
{
	struct rt_i2c_msg msgs;

	msgs.addr = addr;
	msgs.flags = RT_I2C_RD;
	msgs.buf = buf;
	msgs.len = len;

	/* ����I2C�豸�ӿڴ������� */
	if (rt_i2c_transfer(bus, &msgs, 1) == 1)
	{
		return RT_EOK;
	}
	else
	{
		return -RT_ERROR;
	}
}

// rt_int8_t TCA9535_Write_Port_Bit(TCA9535_PORT_ENUM port_n, E_BIT_NUM bit,rt_bool_t level)
//{
//	rt_uint8_t temp;

//	if(port_n == TCA9535_PORT0)
//	{
//		write_reg(i2c_bus, TCA9535_PORT0_CMD, RT_NULL);      /* �������� */
//	}
//	else if(port_n == TCA9535_PORT1)
//	{
//		write_reg(i2c_bus, TCA9535_PORT1_CMD, RT_NULL);      /* �������� */
//	}
//	else
//	{
//		rt_kprintf("TCA9535 PORT ERROR");
//		return -1;
//	}
//    rt_thread_mdelay(5);
//    read_regs(i2c_bus, 1, &temp);                /* ��ȡ���������� */
//    return temp;
//}

rt_int8_t TCA9535_Write_Port(rt_uint16_t addr, TCA9535_PORT_ENUM port_n, rt_uint8_t data)
{
	rt_uint8_t temp;

	if (port_n == TCA9535_PORT0)
	{
		write_reg(i2c_bus, addr, TCA9535_PORT0_OUTPUT_CMD, data); /* �������� */
	}
	else if (port_n == TCA9535_PORT1)
	{
		write_reg(i2c_bus, addr, TCA9535_PORT1_OUTPUT_CMD, data); /* �������� */
	}
	else
	{
		rt_kprintf("TCA9535 PORT ERROR");
		return -1;
	}
	rt_thread_mdelay(1);
	read_regs(i2c_bus, addr, 1, &temp); /* ��ȡ���������� */
	return temp;
}

rt_int8_t TCA9535_Read_Port(rt_uint16_t addr, TCA9535_PORT_ENUM port_n)
{
	rt_uint8_t temp;

	if (port_n == TCA9535_PORT0)
	{
		write_reg(i2c_bus, addr, TCA9535_PORT0_INPUT_CMD, RT_NULL); /* �������� */
	}
	else if (port_n == TCA9535_PORT1)
	{
		write_reg(i2c_bus, addr, TCA9535_PORT1_INPUT_CMD, RT_NULL); /* �������� */
	}
	else
	{
		rt_kprintf("TCA9535 PORT ERROR");
		return -1;
	}
	rt_thread_mdelay(1);
	read_regs(i2c_bus, addr, 1, &temp);
	return temp;
}

rt_int8_t TCA9535_Inversion_PortBits(rt_uint16_t addr, TCA9535_PORT_ENUM port_n, rt_uint8_t inver_bit)
{
	rt_uint8_t temp;

	if (port_n == TCA9535_PORT0)
	{
		write_reg(i2c_bus, addr, TCA9535_PORT0_POLR_CMD, inver_bit); /* �������� */
	}
	else if (port_n == TCA9535_PORT1)
	{
		write_reg(i2c_bus, addr, TCA9535_PORT1_POLR_CMD, inver_bit); /* �������� */
	}
	else
	{
		rt_kprintf("TCA9535 PORT ERROR");
		return -1;
	}
	rt_thread_mdelay(1);
	read_regs(i2c_bus, addr, 1, &temp);
	return temp;
}

rt_uint8_t portData[2];
rt_uint8_t portLastData[2];


static void update_ccr_alarm_status(void)
{
	// alarm status
	sys.mycan.para.oc_fault = (sys.mycan.para.INPUT_P0 & (1 << 0)) ? 1 : 0;
	sys.mycan.para.open_circuit = (sys.mycan.para.INPUT_P0 & (1 << 1)) ? 1 : 0;
	sys.mycan.para.VA_drop_10_percent = (sys.mycan.para.INPUT_P0 & (1 << 2)) ? 1 : 0;
	sys.mycan.para.idensity_fail = (sys.mycan.para.INPUT_P0 & (1 << 3)) ? 1 : 0;
	sys.mycan.para.insulation_res_prealarm = (sys.mycan.para.INPUT_P0 & (1 << 4)) ? 1 : 0;
	sys.mycan.para.lamp_prealarm = (sys.mycan.para.INPUT_P0 & (1 << 5)) ? 1 : 0;
	sys.mycan.para.power_off = !sys.mycan.para.ccr_power_on_state;//(sys.mycan.para.INPUT_P0 & (1 << 6)) ? 1 : 0;
}

static void update_ccr_io_status(void)
{
	// io status
	sys.mycan.para.ccr_local_remote = (sys.mycan.para.INPUT_P1 & (1 << 0)) ? 1 : 0;
	// if(!sys.mycan.para.ccr_local_remote)
	// {
	// 	sys.mycan.para.OUTPUT_P0 = 0;
	// 	sys.mycan.para.cmd_idensity = 0;
	// }
	sys.mycan.para.ccr_relay_state = (sys.mycan.para.INPUT_P1 & (1 << 1)) ? 1 : 0;
	sys.mycan.para.ccr_power_on_state = (sys.mycan.para.INPUT_P1 & (1 << 2)) ? 1 : 0;
	sys.mycan.para.power_off = !sys.mycan.para.ccr_power_on_state;//(sys.mycan.para.INPUT_P0 & (1 << 6)) ? 1 : 0;
}

 rt_uint8_t  inverse_byte(rt_uint8_t byte)
{
	rt_uint8_t inversed_byte = 0;
	for(int i=0; i<8; i++)
	{
		inversed_byte <<=1;
		inversed_byte |= (byte>>i)&0x01;
	}
	return inversed_byte;

}
static void input_check(void *para)
{
	// static rt_uint8_t cnt = 0;
	// static rt_uint8_t outdat = 0xff;
	// static rt_uint8_t input_state[2] = {0x00};
	static rt_uint16_t input0_last;
	static rt_uint16_t input1_last;
	const rt_uint16_t event_code_tab[] = {
		CCR_EVENT_CODE_OC, CCR_EVENT_CODE_BREAK_OFF,
		CCR_EVENT_CODE_VA_DROPDOWN, CCR_EVENT_CODE_REG_FAULT,
		CCR_EVENT_CODE_EFD_ALRM, CCR_EVENT_CODE_LFD_ALAMR,
		CCR_EVENT_CODE_POWER_DOWN, 0
	};
	while (1)
	{
		int alarm_flag = 0;
		sys.mycan.para.INPUT_P0 = ~(TCA9535_Read_Port(TCA9535_INPUT_IO_ADDR, TCA9535_PORT0));
		sys.mycan.para.INPUT_P1 = ~(TCA9535_Read_Port(TCA9535_INPUT_IO_ADDR, TCA9535_PORT1));
		
		sys.mycan.para.INPUT_P0 = ((sys.mycan.para.INPUT_P1>>2)&0x01)?(sys.mycan.para.INPUT_P0 &(~(1<<6))):(sys.mycan.para.INPUT_P0|(1<<6));
		g_send_enable = 1;
		if (sys.mycan.para.INPUT_P1 != input1_last)
		{
			update_ccr_io_status();
		
			TCA9535_Write_Port(TCA9535_LED_OUT1_ADDR, TCA9535_PORT1, ~sys.mycan.para.INPUT_P1);
			input1_last = sys.mycan.para.INPUT_P1;
			if(!sys.mycan.para.can0_comm_fail_flag)
			{
				rt_event_send(&sys.mycan.ccr_event_updata0, EVENT_INTPUT_CHANGED);
			}
			else if(!sys.mycan.para.can1_comm_fail_flag)
			{
				rt_event_send(&sys.mycan.ccr_event_updata1, EVENT_INTPUT_CHANGED);
			}
		}
		int change_flag = alarm_event_monitor(sys.mycan.para.INPUT_P0, &input0_last, event_code_tab,  sizeof(event_code_tab)/sizeof(event_code_tab[0]));

		if(change_flag)
		{
			update_ccr_alarm_status();
			rt_uint8_t  p0_byte = inverse_byte(sys.mycan.para.INPUT_P0);
			TCA9535_Write_Port(TCA9535_LED_OUT1_ADDR, TCA9535_PORT0, ~p0_byte);
		}
		
		// if (sys.mycan.para.INPUT_P1 != portLastData[1])
		// {
		// 	change_flag = 1;
		// 	sys.mycan.para.ccr_local_remote = sys.mycan.para.INPUT_P1 & 0x01;
		// 	sys.mycan.para.ccr_relay_state = (sys.mycan.para.INPUT_P1 >> 1) & 0x01;
		// 	rt_kprintf("P1: %02X ", sys.mycan.para.INPUT_P1);
		// 	portLastData[1] = sys.mycan.para.INPUT_P1;
		// }
		// if (change_flag == 1)
		// {
		// 	change_flag = 0;
		// 	rt_event_send(&sys.mycan.ccr_event_updata0, EVENT_INTPUT_CHANGED);
		// 	rt_event_send(&sys.mycan.ccr_event_updata1, EVENT_INTPUT_CHANGED);
		// }
		rt_thread_mdelay(80);
	}
}



int TCA9535_init(void)
{
	rt_sem_init(&sys.mycan.semUpdate, "semUpdate", 0, RT_IPC_FLAG_FIFO);
	/* ����I2C�����豸����ȡI2C�����豸���? */
	i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(TCA9535_I2C_BUS_NAME);

	if (i2c_bus == RT_NULL)
	{
		rt_kprintf("can't find %s device!\n", TCA9535_I2C_BUS_NAME);
	}

	// ����ģʽ
	write_reg(i2c_bus, TCA9535_LED_OUT1_ADDR, TCA9535_PORT0_CFG_CMD, 0); /* ����Ϊ���ģ�? */
	write_reg(i2c_bus, TCA9535_LED_OUT1_ADDR, TCA9535_PORT1_CFG_CMD, 0); /* ����Ϊ���ģ�? */
	write_reg(i2c_bus, TCA9535_LED_OUT2_ADDR, TCA9535_PORT0_CFG_CMD, 0); /* ����Ϊ���ģ�? */
	write_reg(i2c_bus, TCA9535_LED_OUT2_ADDR, TCA9535_PORT1_CFG_CMD, 0); /* ����Ϊ���ģ�? */

	//	//��ʼ��i2c��д������
	//	rt_sem_init(&i2c_ops_sem,  "i2c_ops_sem", 1, RT_IPC_FLAG_FIFO);
	// ���� i2c ��������ʵ���˻�����

	IOcheckThread = rt_thread_create("InputCheck", input_check, 0, 1024, 15, 5);
	if (IOcheckThread != RT_NULL)
	{
		rt_thread_startup(IOcheckThread);
	}
	return 0;
}
