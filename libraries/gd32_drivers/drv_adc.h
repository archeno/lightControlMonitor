/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-03     BruceOu      first implementation
 */

#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include <rthw.h>
#include <rtthread.h>
#include <board.h>
#include "drv_gpio.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


void  get_dma_data(rt_uint16_t * buf);
void cal_rms(rt_uint16_t * cur);
#define  SAMPLES 200 //400 // 250//200
#define ADC_CANNEL_NUM 2
typedef enum
{
	E_U_LOAD_RMS=0,
    E_I_LOAD_RMS
}RMS_TYPE;
/* gd32 adc dirver class */
struct gd32_adc
{
    uint32_t adc_periph;
    rcu_periph_enum adc_clk;
    rt_base_t adc_pins[16];
    struct rt_adc_device *adc;
    char *device_name;
};
#define RMS_WINDOWN_SIZE 20
extern float rms_adc_voltage[ADC_CANNEL_NUM][RMS_WINDOWN_SIZE + 1];
extern rt_uint16_t rms_adc_cal_voltage_avg[ADC_CANNEL_NUM];
#ifdef __cplusplus
}
#endif

#endif /* __DRV_ADC_H__ */
