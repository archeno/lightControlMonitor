/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-20     BruceOu      first implementation
 */

#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "io.h"
#include <tca9535.h>
#include "pulse_generate.h"
#include "database.h"
#include "i2c_app.h"
#include "drv_adc.h"
#include "lms.h"
#include "rs_485.h"
#include "lmsuart.h"
#define DBG_TAG "main.c"
#define DBG_LVL DBG_INFO
#include "rtdbg.h"

/* defined the LED1 pin: PA9 */
// #define LED1_PIN GET_PIN(A, 9)
#define ENABLE_WDT

static rt_device_t wdt_device;

void PrintSystemClock(void)
{
	rt_kprintf("SYS CLOCK: %d MHz\n", rcu_clock_freq_get(CK_SYS) / 1000000);
	rt_kprintf("AHB CLOCK: %d MHz\n", rcu_clock_freq_get(CK_AHB) / 1000000);
	rt_kprintf("APB1 CLOCK: %d MHz\n", rcu_clock_freq_get(CK_APB1) / 1000000);
	rt_kprintf("ABP2 CLOCK: %d MHz\n", rcu_clock_freq_get(CK_APB2) / 1000000);
}
static void idle_hook(void)
{
	static rt_uint32_t cnt;
	static uint8_t pin_levle = 0;
	rt_device_control(wdt_device, RT_DEVICE_CTRL_WDT_KEEPALIVE, RT_NULL);
	if (++cnt >= 200000)
	{
		cnt = 0;
		rt_pin_write(LED1_PIN, pin_levle);
		pin_levle = !pin_levle;
	}
}

int main(void)
{

	PrintSystemClock();
	// usart0_
	rt_uint32_t timeout = 1; // unit: s
							 // rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
#ifdef ENABLE_WDT
	wdt_device = rt_device_find("wdt");
	if (RT_NULL != wdt_device)
	{
		rt_device_init(wdt_device);
		rt_device_control(wdt_device, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
		rt_device_control(wdt_device, RT_DEVICE_CTRL_WDT_START, RT_NULL);
		rt_thread_idle_sethook(idle_hook);
	}
#endif
	sys.mycan.para.sys_com_indicator = 0xff;
	while (1)
	{

		// lms_uart_send_byte(0x88);
		rt_thread_mdelay(1000);
		if (sys.mycan.para.can0_comm_fail_flag && sys.mycan.para.can1_comm_fail_flag)
		{
			sys.mycan.para.sys_com_indicator &= ~(1 << 7);
		}
		else if (sys.mycan.para.can0_comm_fail_flag || sys.mycan.para.can1_comm_fail_flag)
		{
			sys.mycan.para.sys_com_indicator ^= (1 << 7);
		}
		else
		{
			sys.mycan.para.sys_com_indicator |= (1 << 7);
		}

		if (sys.mycan.para.ups_comm_fail)
		{
			sys.mycan.para.sys_com_indicator &= ~(1 << 6);
		}
		else
		{
			sys.mycan.para.sys_com_indicator |= (1 << 6);
		}

		// TCA9535_Write_Port(TCA9535_LED_OUT2_ADDR, TCA9535_PORT1, sys.mycan.para.sys_com_indicator);
		// int send_len = agile_modbus_serialize_read_registers(ctx, 0x60, 34);
		// rs_485_send_data(RS_485_UART3, ctx->send_buf, send_len);
		// int read_len = rs_485_receive(RS_485_UART3, ctx->read_buf, ctx->read_bufsz, 1000, 20);
		// if (read_len <= 0)
		// {
		// 	LOG_W("receive timeout.");
		// 	continue;
		// }
		// int rc = agile_modbus_deserialize_read_registers(ctx, read_len, hold_register);
		// if (rc < 0)
		// {
		// 	LOG_W("receive faild.");
		// 	continue;
		// }
		// update_ups_data()
	}
	return RT_EOK;
}
static rt_int16_t my_atoi(const char *str)
{
	rt_uint16_t result = 0;
	rt_uint8_t sign = 0;
	const char *pstr = str;
	if (pstr == NULL)
	{
		return 0;
	}
	rt_uint8_t cnt = 0;
	if (*pstr == '-')
	{
		sign = 1;
		pstr++;
	}
	while (*pstr != '\0')
	{

		cnt++;
		result = 10 * result + (*pstr - '0');
		pstr++;
	}

	return sign ? -result : result;
}

static int set_un(int argc, char *argv[])
{
	rt_int16_t un_a, un_b;
	if (argc < 3)
	{
		LOG_E("set un: a  b");
		// LOG_I("系数均扩大10倍输入");
		return 0;
	}

	un_a = my_atoi(argv[1]);
	un_b = my_atoi(argv[2]);

	g_sys_param.Un_a = un_a / 10.0f;
	g_sys_param.Un_b = un_b / 10.0f;
	write_sysdata_to_eeprom(EEPROM_WRITE_DATABASE_VALUE);
	return 0;
}
MSH_CMD_EXPORT(set_un, "set un");

static int set_In(int argc, char *argv[])
{
	rt_int16_t In_a, In_b;
	if (argc < 3)
	{
		LOG_E("set In: In_a  In_b");
		return 0;
	}

	In_a = my_atoi(argv[1]);
	In_b = my_atoi(argv[2]);

	g_sys_param.In_a1 = In_a / 100.0f;
	g_sys_param.In_b = In_b / 100.0f;

	write_sysdata_to_eeprom(EEPROM_WRITE_DATABASE_VALUE);
	return 0;
}
MSH_CMD_EXPORT(set_In, "set In");
static int show_adc_info(void)
{
	// LOG_I("freq:\t%d.%dHz", g_ccr_data.ccr_info.freq/10,g_ccr_data.ccr_info.freq%10 );
	// LOG_I("Iload:\t%d.%02dA", g_ccr_data.ccr_info.i_load/100, g_ccr_data.ccr_info.i_load%100);
	// LOG_I("Uscr:\t%d.%dV", g_ccr_data.ccr_info.v_scr/10, g_ccr_data.ccr_info.v_scr%10);
	// LOG_I("PassAngle:\t%d°", (rt_uint16_t)(g_ccr_data.ccr_info.angle));
	// LOG_I("i_lvl:\t%d", g_ccr_data.ccr_info.intensity);
	LOG_I("ADC_I_VAL:\t%d.%03dV", (rt_uint32_t)(rms_adc_voltage[1][20] * 1000) / 1000, (rt_uint32_t)(rms_adc_voltage[1][20] * 1000) % 1000);
	LOG_I("ADC_U_VAL: \t%d.%03dV", (rt_uint32_t)(rms_adc_voltage[0][20] * 1000) / 1000, (rt_uint32_t)(rms_adc_voltage[0][20] * 1000) % 1000);
	LOG_I("I_VAL:\t%d.%02dA", (rt_uint32_t)(rms_adc_cal_voltage_avg[1]) / 100, (rt_uint32_t)(rms_adc_cal_voltage_avg[1]) % 100);
	LOG_I("U_VAL: \t%d.%dV", (rt_uint32_t)(rms_adc_cal_voltage_avg[0]) / 10, (rt_uint32_t)(rms_adc_cal_voltage_avg[0]) % 10);
	LOG_I("In_a: \t%d  In_b: %c%d", (rt_uint32_t)(g_sys_param.In_a1 * 100), g_sys_param.In_b < 0 ? '-' : ' ', (rt_int32_t)(fabs(g_sys_param.In_b) * 100));
	LOG_I("Un_a: \t%d  Un_b: %c%d", (rt_uint32_t)(g_sys_param.Un_a * 10), g_sys_param.Un_b < 0 ? '-' : ' ', (rt_int32_t)(fabs(g_sys_param.Un_b) * 10));
	return 0;
}
MSH_CMD_EXPORT(show_adc_info, "show adc info");

static int light_on(int argc, char *argv[])
{
	rt_int16_t groupId, segId;
	if (argc < 3)
	{
		LOG_E("light_on [groupId] [segId]");
		return 0;
	}

	groupId = my_atoi(argv[1]);
	segId = my_atoi(argv[2]);
	build_ctrl_cmd(groupId, segId, CloseLamp_Order);
	// g_sys_param.In_a1 = In_a / 100.0f;
	// g_sys_param.In_b = In_b / 100.0f;
	// uint8_t seg_cmd_buf[2];
	// uint8_t ctrl_buf[3];

	// seg_cmd_buf[0] = segId;
	// seg_cmd_buf[1] = CloseLamp_Order << 4;

	// ctrl_buf[0] = GenSyncFrame(groupId, LONG_CMD);
	// ctrl_buf[1] = segId;
	// ctrl_buf[2] = crc4_itu(seg_cmd_buf, 2) | seg_cmd_buf[1];
	// rs_485_send_data(RS_485_UART2, ctrl_buf, 3);
	// write_sysdata_to_eeprom(EEPROM_WRITE_DATABASE_VALUE);
	return 0;
}
MSH_CMD_EXPORT(light_on, "light ctrl on");
static int light_off(int argc, char *argv[])
{
	rt_int16_t groupId, segId;
	if (argc < 3)
	{
		LOG_E("light_off [groupId] [segId]");
		return 0;
	}

	groupId = my_atoi(argv[1]);
	segId = my_atoi(argv[2]);
	build_ctrl_cmd(groupId, segId, OpenLamp_Order);

	// g_sys_param.In_a1 = In_a / 100.0f;
	// g_sys_param.In_b = In_b / 100.0f;
	// uint8_t seg_cmd_buf[2];
	// uint8_t ctrl_buf[3];

	// seg_cmd_buf[0] = segId;
	// seg_cmd_buf[1] = OpenLamp_Order<<4;

	// ctrl_buf[0] = GenSyncFrame(groupId, LONG_CMD);
	// ctrl_buf[1] = segId;
	// ctrl_buf[2] = crc4_itu(seg_cmd_buf, 2)| seg_cmd_buf[1];
	// rs_485_send_data(RS_485_UART2, ctrl_buf, 3);
	// write_sysdata_to_eeprom(EEPROM_WRITE_DATABASE_VALUE);
	return 0;
}
MSH_CMD_EXPORT(light_off, "light ctrl off");

static int disp_lms_state(int argc, char *argv[])
{
	const char *tab_prefix[4] = {"1-8:\t", "9-16:\t", "17-24:\t", "25-32:\t"};
	rt_kprintf("%s", tab_prefix[0]);
	int line = 0;
	for (int i = 0; i < TERMIANL_NUM; i++)
	{

		rt_kprintf("%X ", g_lms.terminalState[i]);
		if (((i + 1) & 0x07) == 0)
		{
			line++;
			rt_kprintf("\r\n");
			rt_kprintf("%s", tab_prefix[line]);
		}
	}
	return 0;
}
MSH_CMD_EXPORT(disp_lms_state, "disp_lms_state");

static int change_lms_state(int argc, char *argv[])
{
	rt_int8_t index, data;
	if (argc < 3)
	{
		LOG_E("lms_change [index] [data]");
		return 0;
	}

	index = my_atoi(argv[1]);
	data = my_atoi(argv[2]);

	g_lms.terminalState[index] = data;
	rt_mb_send(&mb, (rt_ubase_t)index);
	return 0;
}
MSH_CMD_EXPORT(change_lms_state, "change_lms_state");
