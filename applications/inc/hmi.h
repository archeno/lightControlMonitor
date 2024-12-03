#ifndef __HMI_H__
#define __HMI_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

typedef struct
{
	uint8_t show_result_flag; // 打印结果标志
	uint8_t reset_factory_flag;
	uint8_t write_eeprom_flag;
} hmi_state_t;

extern hmi_state_t g_hmi_data;

#endif
