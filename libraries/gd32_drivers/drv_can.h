/*
 * Copyright (c) 2022 深圳市海洋王绿色照明技术有限公司 
 *
 * 
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     DC      	first implementation
 */

#ifndef __DRV_CAN_H__
#define __DRV_CAN_H__

#include <rthw.h>
#include <rtthread.h>
#include <board.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CAN_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define CAN_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

/* GD32 can driver */
// Todo: compress can info
struct gd32_can
{
    uint32_t can_periph;			//Todo: 3bits
	IRQn_Type tx_irqn;              //Todo: 7bits
    IRQn_Type rx_irqn;              //Todo: 7bits
    rcu_periph_enum per_clk;        //Todo: 5bits
    rcu_periph_enum tx_gpio_clk;    //Todo: 5bits
    rcu_periph_enum rx_gpio_clk;    //Todo: 5bits
    uint32_t tx_port;               //Todo: 4bits
#if defined SOC_SERIES_GD32F4xx
    uint16_t tx_af;                 //Todo: 4bits
#endif
    uint16_t tx_pin;                //Todo: 4bits
    uint32_t rx_port;               //Todo: 4bits
#if defined SOC_SERIES_GD32F4xx
    uint16_t rx_af;                 //Todo: 4bits
#endif
    uint16_t rx_pin;                //Todo: 4bits

    struct rt_can_device * can;
    char *device_name;
};

#ifdef __cplusplus
}
#endif

#endif /* __DRV_CAN_H__ */
