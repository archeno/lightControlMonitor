
#include "i2c_app.h"
#include "at24cxx.h"
#include "crc16.h"
#include "database.h"

#define DBG_TAG "i2c_app.c"
#include "rtdbg.h"
#define EEPROM_ADDR 0Xa0 >> 1 // 外设器件地址
#define I2C_NAME "i2c1"
#define SYS_PARA_ADDR 0 // 系统参数EERPROM 地址
#define SYS_DATA_SIZE sizeof(struct sys_param)

// #define  eeprom init(void
at24cxx_device_t at24c02_dev;

struct rt_semaphore g_write_cmd_sem;

int eeprom_init()
{
	at24c02_dev = at24cxx_init(I2C_NAME, EEPROM_ADDR);
	if (at24c02_dev == RT_NULL)
	{
		LOG_E("at24c02 init failed");
	}

	read_sysdata_from_eeprom();
	return 0;
}
INIT_DEVICE_EXPORT(eeprom_init);

const static struct sys_param sys_default_para =
	{

		// .In_a = 14.7f,
		// .In_b = 0.0f,
		.In_a2 = 0,
		// .In_a1 =22.213f,
		.In_a1 = 10.42f,
		// .In_a2 = 0,
		// .In_a1 = 14.7f,
		// .In_b = -0.217f,
		.In_b = 0,

		//.Un_a = 794.37f,
		.Un_a = 577.7f,
		//.Un_b = -0.872f,
		.Un_b = -9.04f,
};

// void update_sys_data_to_ccr_data()
// {
// 	rt_memcpy(&g_ccr_data.ccr_info.on_off, &g_sys_param, 6);
// 	// g_ccr_data.ccr_info.status = g_sys_param.status;
// 	rt_memcpy(&g_ccr_data.ccr_info.run_time_lv1, &g_sys_param.run_time_lv1, 12);
// 	rt_memcpy(&g_ccr_data.ccr_info.In_a, &g_sys_param.In_a, 24);
// }

// void update_ccr_data_to_sys_data()
// {
// 	rt_memcpy(&g_sys_param, &g_ccr_data.ccr_info.on_off, 6);
// 	// g_sys_param.statu = g_ccrs_data.ccr_info.status;
// 	rt_memcpy(&g_sys_param.run_time_lv1, &g_ccr_data.ccr_info.run_time_lv1, 12);
// 	rt_memcpy(&g_sys_param.In_a, &g_ccr_data.ccr_info.In_a, 24);
// }

void reset_eeprom_data(void)
{
	// 3.  数据异常，写入默认值，并报警指�?
	// 1. 读取数据
	//        rt_uint8_t sys_data[SYS_DATA_SIZE+2];
	//   rt_base_t level = rt_hw_interrupt_disable();
	//        at24cxx_page_read(at24c02_dev, SYS_PARA_ADDR,sys_data,SYS_DATA_SIZE+2);
	write_sysdata_to_eeprom(EEPROM_WRITE_DEFAULT_VALUE);
	//        at24cxx_page_read(at24c02_dev, SYS_PARA_ADDR,sys_data,SYS_DATA_SIZE+2);
	//        write_sysdata_to_eeprom(EEPROM_WRITE_DEFAULT_VALUE);
	//   rt_hw_interrupt_enable(level);
	rt_memcpy(&g_sys_param.In_a2, &sys_default_para, SYS_DATA_SIZE);
	// update_sys_data_to_ccr_data();
}
/** 从eeprom 中加载数据到database**/

void read_sysdata_from_eeprom(void)
{
	// 1. ��ȡeeprom ����
	rt_uint8_t sys_data[SYS_DATA_SIZE + 2];
	rt_uint16_t cal_crc;
	if (RT_EOK != at24cxx_page_read(at24c02_dev, SYS_PARA_ADDR, sys_data, SYS_DATA_SIZE + 2))
	{
		LOG_E("at24c02 read syspara failed");
		return;
	}
	// 2. crcУ��
	cal_crc = crc16(sys_data, SYS_DATA_SIZE);
	if (cal_crc != (sys_data[SYS_DATA_SIZE] | (sys_data[SYS_DATA_SIZE + 1] << 8)))
	{
		LOG_E("sysdata crc check failed!");
		// TODO   报警
		// 3.  д��Ĭ�����ݵ�eeprom
		write_sysdata_to_eeprom(EEPROM_WRITE_DEFAULT_VALUE);
		at24cxx_page_read(at24c02_dev, SYS_PARA_ADDR, sys_data, SYS_DATA_SIZE + 2);
		rt_memcpy(&g_sys_param.In_a2, sys_data, SYS_DATA_SIZE);
	}
	else
	{
		// 4数据正常,加载数据到database
		rt_memcpy(&g_sys_param.In_a2, sys_data, SYS_DATA_SIZE);
		//
		LOG_I("read at24c02 syspara success!");
	}
	// 更新数据到ccr_data
	// update_sys_data_to_ccr_data();
}

/** 写系统参数到eeproom**/
/** type:  0:Ĭ��ֵ  1: database �ڴ�ֵ**/

void write_sysdata_to_eeprom(rt_uint8_t type)
{
	rt_uint16_t crc;
	rt_uint8_t cnt = 3;
	rt_err_t result;
	rt_uint8_t tmp[SYS_DATA_SIZE + 2];
	if (type == EEPROM_WRITE_DATABASE_VALUE)
	{
		// update_ccr_data_to_sys_data();
		rt_memcpy(tmp, (rt_uint8_t *)&g_sys_param.In_a2, SYS_DATA_SIZE);
	}
	else
	{
		rt_memcpy(tmp, (rt_uint8_t *)&sys_default_para, SYS_DATA_SIZE);
	}
	crc = crc16(tmp, SYS_DATA_SIZE);
	tmp[SYS_DATA_SIZE] = (rt_uint8_t)(crc & 0xff);
	tmp[SYS_DATA_SIZE + 1] = (rt_uint8_t)(crc >> 8);
	do
	{
		cnt--;
		result = at24cxx_page_write(at24c02_dev, SYS_PARA_ADDR, tmp, SYS_DATA_SIZE + 2);

	} while ((result != RT_EOK) && cnt);
	if (result != RT_EOK)
	{
		LOG_E("at24cxx_page_write failed");
	}
}

// 写数�?

rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
	rt_uint8_t buf[3];
	struct rt_i2c_msg msgs;
	rt_uint32_t buf_size = 1;

	buf[0] = reg; // cmd
	if (data != RT_NULL)
	{
		buf[1] = data[0];
		buf[2] = data[1];
		buf_size = 3;
	}

	msgs.addr = EEPROM_ADDR;
	msgs.flags = RT_I2C_WR;
	msgs.buf = buf;
	msgs.len = buf_size;

	/* 调用I2C设备接口传输数据 */
	if (rt_i2c_transfer(bus, &msgs, 1) == 1)
	{
		return RT_EOK;
	}
	else
	{
		return -RT_ERROR;
	}
}

// 读数�?
rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
	struct rt_i2c_msg msgs;

	msgs.addr = EEPROM_ADDR;
	msgs.flags = RT_I2C_RD;
	msgs.buf = buf;
	msgs.len = len;

	/* 调用I2C设备接口传输数据 */
	if (rt_i2c_transfer(bus, &msgs, 1) == 1)
	{
		return RT_EOK;
	}
	else
	{
		return -RT_ERROR;
	}
}

// static void write_cmd_thread_entry(void *parameter)
//{
//	static rt_uint16_t on_off_last;
//	while(1)
//	{
//		rt_sem_take(&g_write_cmd_sem, RT_WAITING_FOREVER);
//		if(g_ccr_data.ccr_info.on_off != on_off_last)
//		{
//			on_off_last = g_ccr_data.ccr_info.on_off;
//			if(g_ccr_data.ccr_info.on_off)
//			{
//				RELAY_ON();
//				SET_SYS_STATUS_BIT(RELAY_STATUS_POS);
//			}
//			else
//			{
//				RELAY_OFF();
//				CLR_SYS_STATUS_BIT(RELAY_STATUS_POS);
//			}
//		}
//		write_sysdata_to_eeprom(EEPROM_WRITE_DATABASE_VALUE);
//		rt_thread_mdelay(5);
//	}
// }

// static  int write_cmd_receive_thread_init(void)
//{
//	if(RT_EOK != rt_sem_init(&g_write_cmd_sem, "write_cmd_sem", 0, RT_IPC_FLAG_FIFO))
//	{
//		LOG_E("rt_sem_init eeprom_save_sem failed" );
//	}
//	rt_thread_t  thread = rt_thread_create("write_cmd_thread",write_cmd_thread_entry, 0, 1000,20, 10);
//	if(thread != RT_NULL)
//	{
//		if(RT_EOK != rt_thread_startup(thread))
//		{
//			LOG_E("rt_thread_startup write_cmd_thread failed");
//		}
//	}
//	else
//	{
//		LOG_E("rt_thread_create write_cmd_thread failed");
//	}
//	return 0;
//
// }
// INIT_COMPONENT_EXPORT(write_cmd_receive_thread_init);
