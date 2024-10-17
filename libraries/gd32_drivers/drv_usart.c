/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-20     BruceOu      first implementation
 */

#include "drv_usart.h"

#ifdef RT_USING_SERIAL

#if !defined(BSP_USING_UART0) && !defined(BSP_USING_UART1) && \
    !defined(BSP_USING_UART2) && !defined(BSP_USING_UART3) && \
    !defined(BSP_USING_UART4) && !defined(BSP_USING_UART5) && \
    !defined(BSP_USING_UART6) && !defined(BSP_USING_UART7)
#error "Please define at least one UARTx"

#endif

#include <rtdevice.h>

static void GD32_UART_IRQHandler(struct rt_serial_device *serial);

enum {
#if defined BSP_USING_UART0
    UART0_INDEX,
#endif
#if defined BSP_USING_UART1
    UART1_INDEX,
#endif
#if defined BSP_USING_UART2
    UART2_INDEX,
#endif
#if defined BSP_USING_UART3
    UART3_INDEX,
#endif
#if defined BSP_USING_UART4
    UART4_INDEX,
#endif
#if defined BSP_USING_UART5
    UART5_INDEX,
#endif
#if defined BSP_USING_UART6
    UART6_INDEX,
#endif
#if defined BSP_USING_UART7
    UART7_INDEX,
#endif
    UART_TOTAL_NUM
};

#ifdef RT_SERIAL_USING_DMA
static struct dma_config tx_dma_configs[] =
{
    //USART0 TODO
#if defined BSP_USING_UART0   
    {
        .dma_periph = DMA1,
        .channel = DMA_CH7,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = NULL,
    },
#endif
    //USART1
#if defined BSP_USING_UART1  
    {
        .dma_periph = DMA0,
        .channel = DMA_CH6,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = NULL,
    },
#endif
    //USART2
#if defined BSP_USING_UART2   
    {
        .dma_periph = DMA0,
        .channel = DMA_CH3,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = NULL,
    },
#endif
    //USART3
#if defined BSP_USING_UART3  
    {
        .dma_periph = DMA0,
        .channel = DMA_CH4,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel4_IRQn,
    },
#endif
    //USART4
#if defined BSP_USING_UART4   
    {
        .dma_periph = DMA0,
        .channel = DMA_CH7,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel7_IRQn,
    },
#endif
    //USART5
#if defined BSP_USING_UART5   
    {
        .dma_periph = DMA1,
        .channel = DMA_CH7,
        .peripheral = DMA_SUBPERI5,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = NULL,
    },
#endif
    //USART6
#if defined BSP_USING_UART6   
    {
        .dma_periph = DMA0,
        .channel = DMA_CH1,
        .peripheral = DMA_SUBPERI5,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = NULL,
    },
#endif
    //USART7
#if defined BSP_USING_UART7  
    {
        .dma_periph = DMA0,
        .channel = DMA_CH0,
        .peripheral = DMA_SUBPERI5,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = NULL,
    }
#endif
};
 
static struct dma_config rx_dma_configs[] =
{
    //USART0
    {
        .dma_periph = DMA1,
        .channel = DMA_CH5,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA1_Channel5_IRQn,
    },
    //USART1
    {
        .dma_periph = DMA0,
        .channel = DMA_CH5,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel5_IRQn,
    },
    //USART2
    {
        .dma_periph = DMA0,
        .channel = DMA_CH1,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel1_IRQn,
    },
    //USART3
    {
        .dma_periph = DMA0,
        .channel = DMA_CH2,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel2_IRQn,
    },
    //USART4
    {
        .dma_periph = DMA0,
        .channel = DMA_CH0,
        .peripheral = DMA_SUBPERI4,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel0_IRQn,
    },
    //USART5
    {
        .dma_periph = DMA1,
        .channel = DMA_CH2,
        .peripheral = DMA_SUBPERI5,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA1_Channel2_IRQn,
    },
    //USART6
    {
        .dma_periph = DMA0,
        .channel = DMA_CH3,
        .peripheral = DMA_SUBPERI5,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel3_IRQn,
    },
    //USART7
    {
        .dma_periph = DMA0,
        .channel = DMA_CH6,
        .peripheral = DMA_SUBPERI5,
        .priority = DMA_PRIORITY_ULTRA_HIGH,
		.dma_irqn = DMA0_Channel6_IRQn,
    }
};

static void gd32_dma_rx_config(struct gd32_uart *uart)
{                    
    RT_ASSERT(uart != RT_NULL);
    
    struct rt_serial_rx_fifo *rx_fifo;
    
    rx_fifo = (struct rt_serial_rx_fifo *)uart->serial->serial_rx;
    
	dma_single_data_parameter_struct dma_init_struct;
    
    if(uart->dma_rx != 0)
    {
        /* enable DMA */
        rcu_periph_clock_enable(RCU_DMA0);  
        rcu_periph_clock_enable(RCU_DMA1);
        
        /* USART DMA enable for reception */
        usart_dma_receive_config(uart->uart_periph, USART_RECEIVE_DMA_ENABLE);
        
        /* deinitialize DMA channel */
        dma_deinit(uart->dma_rx->dma_periph, uart->dma_rx->channel);
        
        /* set the DMA struct */
        dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
        dma_init_struct.memory0_addr = (uint32_t)(rx_fifo->buffer);
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.number = uart->serial->config.bufsz;
        dma_init_struct.periph_addr = (uint32_t)&USART_DATA(uart->uart_periph);
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
        dma_init_struct.priority = uart->dma_rx->priority;
		
		dma_single_data_mode_init(uart->dma_rx->dma_periph, uart->dma_rx->channel, &dma_init_struct);
		
        /* configure DMA mode */
        dma_circulation_disable(uart->dma_rx->dma_periph, uart->dma_rx->channel); 
		dma_channel_subperipheral_select(uart->dma_rx->dma_periph, uart->dma_rx->channel, uart->dma_rx->peripheral);
		
        /* enable DMA channel */
        dma_channel_enable(uart->dma_rx->dma_periph, uart->dma_rx->channel);
        
        /* enable rx irq */
        NVIC_EnableIRQ(uart->irqn);
        /* enable interrupt */
        usart_interrupt_enable(uart->uart_periph, USART_INT_IDLE);
		
		/* enable DMA rx irq */
		NVIC_EnableIRQ(uart->dma_rx->dma_irqn);
        /* enable DMA full transfer finish interrupt */
        dma_interrupt_enable(uart->dma_rx->dma_periph,uart->dma_rx->channel,DMA_CHXCTL_FTFIE); 
        
        uart->dma_rx->last_index = 0;
    }
}
 
static void gd32_dma_tx_config(struct gd32_uart *uart)
{                    
    RT_ASSERT(uart != RT_NULL);
    
    if(uart->dma_tx != 0)
    {
        /* enable DMA */
        rcu_periph_clock_enable(RCU_DMA0);  
        rcu_periph_clock_enable(RCU_DMA1);
        
        /* clean TC flag */
        usart_flag_clear(uart->uart_periph, USART_FLAG_TC);
        /* enable rx irq */
        NVIC_EnableIRQ(uart->irqn);
        /* enable interrupt */
        usart_interrupt_enable(uart->uart_periph, USART_INT_TC);
		
    }
}
static void gd32_dma_rx_disable(struct gd32_uart *uart)
{                    
    RT_ASSERT(uart != RT_NULL);
    
    /* USART DMA disable for reception */
    usart_dma_receive_config(uart->uart_periph, USART_RECEIVE_DMA_DISABLE); 
    
    /* disable DMA channel */   
    if(uart->dma_rx != 0)
    {
        dma_channel_disable(uart->dma_rx->dma_periph, uart->dma_rx->channel); 
    }
}   
static void gd32_dma_tx_disable(struct gd32_uart *uart)
{                    
    RT_ASSERT(uart != RT_NULL);
    
    /* USART DMA disable for transmission */
    usart_dma_transmit_config(uart->uart_periph, USART_TRANSMIT_DMA_DISABLE);    
    
    /* disable DMA channel */  
    if(uart->dma_tx != 0)
    {
        dma_channel_disable(uart->dma_tx->dma_periph, uart->dma_tx->channel);
    }
}
 
rt_size_t dma_tx_xfer(struct rt_serial_device *serial, rt_uint8_t *buf, rt_size_t size, int direction)
{
    struct gd32_uart *uart = (struct gd32_uart *) serial->parent.user_data; 
    rt_size_t xfer_size = 0;      
    
//    dma_parameter_struct dma_init_struct; 
	dma_single_data_parameter_struct dma_init_struct;
 
    RT_ASSERT(uart != RT_NULL);
    
    if(0 != size )
    {
        if (RT_SERIAL_DMA_TX == direction)
        {   
          
//			while()
//			{
//			   rt_size_t ++;
//			   if(rt_size_t >= 10000) break;
//			}
			/* deinitialize DMA channel */
            dma_deinit(uart->dma_tx->dma_periph, uart->dma_tx->channel);
		
			dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
			dma_init_struct.memory0_addr = (uint32_t)buf;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
			dma_init_struct.number = size;
			dma_init_struct.periph_addr = (uint32_t)&USART_DATA(uart->uart_periph);
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			
			dma_single_data_mode_init(uart->dma_tx->dma_periph, uart->dma_tx->channel, &dma_init_struct);
            /* configure DMA mode */
            dma_circulation_disable(uart->dma_tx->dma_periph, uart->dma_tx->channel);
			dma_channel_subperipheral_select(uart->dma_tx->dma_periph, uart->dma_tx->channel, uart->dma_tx->peripheral);
			
			/* enable DMA channel7 */
			dma_channel_enable(uart->dma_tx->dma_periph, uart->dma_tx->channel);
			/* USART DMA enable for transmission  */
			usart_dma_transmit_config(uart->uart_periph, USART_TRANSMIT_DMA_ENABLE);
            xfer_size = size;
        }
    }
    return xfer_size;
}
#endif
#if defined(BSP_USING_UART0)
struct rt_serial_device serial0;

void USART0_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial0);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_UART0 */

#if defined(BSP_USING_UART1)
struct rt_serial_device serial1;

void USART1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial1);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_UART1 */

#if defined(BSP_USING_UART2)
struct rt_serial_device serial2;

void USART2_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial2);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_UART2 */

#if defined(BSP_USING_UART3)
struct rt_serial_device serial3;

void UART3_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial3);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_UART3 */

#if defined(BSP_USING_UART4)
struct rt_serial_device serial4;

void UART4_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial4);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART4 */

#if defined(BSP_USING_UART5)
struct rt_serial_device serial5;

void USART5_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial5);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_UART5 */

#if defined(BSP_USING_UART6)
struct rt_serial_device serial6;

void UART6_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial6);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_UART6 */

#if defined(BSP_USING_UART7)
struct rt_serial_device serial7;

void UART7_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    GD32_UART_IRQHandler(&serial7);

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* BSP_USING_UART7 */

static const struct gd32_uart uart_obj[] = {
    #ifdef BSP_USING_UART0
    {
        USART0,                                 // uart peripheral index
        USART0_IRQn,                            // uart iqrn
        RCU_USART0, RCU_GPIOA, RCU_GPIOA,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOA, GPIO_AF_7, GPIO_PIN_9,           // tx port, tx alternate, tx pin
        GPIOA, GPIO_AF_7, GPIO_PIN_10,          // rx port, rx alternate, rx pin
#else
        GPIOA, GPIO_PIN_9,           // tx port, tx pin
        GPIOA, GPIO_PIN_10,          // rx port, rx pin
#endif
        &serial0,
        "uart0",
#ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DAM_TX | RT_DEVICE_FLAG_RX,
        .dma_rx = rx_dma_configs[UART0_INDEX],
        .dma_tx = tx_dma_configs[UART0_INDEX]
#endif 
    },
    #endif

    #ifdef BSP_USING_UART1
    {
        USART1,                                 // uart peripheral index
        USART1_IRQn,                            // uart iqrn
        RCU_USART1, RCU_GPIOA, RCU_GPIOA,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOA, GPIO_AF_7, GPIO_PIN_2,           // tx port, tx alternate, tx pin
        GPIOA, GPIO_AF_7, GPIO_PIN_3,           // rx port, rx alternate, rx pin
#else
        GPIOA, GPIO_PIN_2,                      // tx port, tx pin
        GPIOA, GPIO_PIN_3,                      // rx port, rx pin
#endif
        &serial1,
        "uart1",
 #ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DAM_TX | RT_DEVICE_FLAG_RX,
        .dma_rx = rx_dma_configs[UART1_INDEX],
        .dma_tx = tx_dma_configs[UART1_INDEX]
#endif 
    },
    #endif

    #ifdef BSP_USING_UART2
    {
        USART2,                                 // uart peripheral index
        USART2_IRQn,                            // uart iqrn
        RCU_USART2, RCU_GPIOB, RCU_GPIOB,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOC, GPIO_AF_7, GPIO_PIN_10,          // tx port, tx alternate, tx pin
        GPIOC, GPIO_AF_7, GPIO_PIN_11,          // rx port, rx alternate, rx pin
#else
        GPIOB, GPIO_PIN_10,          // tx port, tx pin
        GPIOB, GPIO_PIN_11,          // rx port, rx pin
#endif
        &serial2,
        "uart2",
#ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DAM_TX | RT_DEVICE_FLAG_RX,
        .dma_rx = rx_dma_configs[UART2_INDEX],
        .dma_tx = tx_dma_configs[UART2_INDEX]
#endif 
    },
    #endif

    #ifdef BSP_USING_UART3
    {
        UART3,                                 // uart peripheral index
        UART3_IRQn,                            // uart iqrn
        RCU_UART3, RCU_GPIOC, RCU_GPIOC,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOC, GPIO_AF_8, GPIO_PIN_10,         // tx port, tx alternate, tx pin
        GPIOC, GPIO_AF_8, GPIO_PIN_11,         // rx port, rx alternate, rx pin
#else
        GPIOC, GPIO_PIN_10,         // tx port, tx pin
        GPIOC, GPIO_PIN_11,         // rx port, rx pin
#endif
        &serial3,
        "uart3",
#ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
        .dma_rx = &rx_dma_configs[UART3_INDEX],
        .dma_tx = &tx_dma_configs[UART3_INDEX]
#endif 
    },
    #endif

    #ifdef BSP_USING_UART4
    {
        UART4,                                 // uart peripheral index
        UART4_IRQn,                            // uart iqrn
        RCU_UART4, RCU_GPIOC, RCU_GPIOD,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOC, GPIO_AF_8, GPIO_PIN_12,         // tx port, tx alternate, tx pin
        GPIOD, GPIO_AF_8, GPIO_PIN_2,          // rx port, rx alternate, rx pin
#else
        GPIOC, GPIO_PIN_12,         // tx port, tx pin
        GPIOD, GPIO_PIN_2,          // rx port, rx pin
#endif
        &serial4,
        "uart4",
#ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DMA_TX | RT_DEVICE_FLAG_DMA_RX,
        .dma_rx = &rx_dma_configs[UART4_INDEX],
        .dma_tx = &tx_dma_configs[UART4_INDEX]
#endif 
    },
    #endif

    #ifdef BSP_USING_UART5
    {
        USART5,                                 // uart peripheral index
        USART5_IRQn,                            // uart iqrn
        RCU_USART5, RCU_GPIOA, RCU_GPIOA,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOA, GPIO_AF_8, GPIO_PIN_11,           // rx port, rx alternate, rx pin
        GPIOA, GPIO_AF_8, GPIO_PIN_12,           // tx port, tx alternate, tx pin
#else
        GPIOC, GPIO_PIN_6,           // tx port, tx pin
        GPIOC, GPIO_PIN_7,           // rx port, rx pin
#endif
        &serial5,
        "uart5",
#ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DAM_TX | RT_DEVICE_FLAG_RX,
        .dma_rx = rx_dma_configs[UART5_INDEX],
        .dma_tx = tx_dma_configs[UART5_INDEX]
#endif 

    },
    #endif

    #ifdef BSP_USING_UART6
    {
        UART6,                                 // uart peripheral index
        UART6_IRQn,                            // uart iqrn
        RCU_UART6, RCU_GPIOE, RCU_GPIOE,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOE, GPIO_AF_8, GPIO_PIN_7,          // tx port, tx alternate, tx pin
        GPIOE, GPIO_AF_8, GPIO_PIN_8,          // rx port, rx alternate, rx pin
#else
        GPIOE, GPIO_PIN_7,          // tx port, tx pin
        GPIOE, GPIO_PIN_8,          // rx port, rx pin
#endif
        &serial6,
        "uart6",
#ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DAM_TX | RT_DEVICE_FLAG_RX,
        .dma_rx = rx_dma_configs[UART6_INDEX],
        .dma_tx = tx_dma_configs[UART6_INDEX]
#endif 
    },
    #endif

    #ifdef BSP_USING_UART7
    {
        UART7,                                 // uart peripheral index
        UART7_IRQn,                            // uart iqrn
        RCU_UART7, RCU_GPIOE, RCU_GPIOE,       // periph clock, tx gpio clock, rt gpio clock
#if defined SOC_SERIES_GD32F4xx
        GPIOE, GPIO_AF_8, GPIO_PIN_0,          // tx port, tx alternate, tx pin
        GPIOE, GPIO_AF_8, GPIO_PIN_1,          // rx port, rx alternate, rx pin
#else
        GPIOE, GPIO_PIN_0,          // tx port, tx pin
        GPIOE, GPIO_PIN_1,          // rx port, rx pin
#endif
        &serial7,
        "uart7",
#ifdef RT_SERIAL_USING_DMA
        .dma_flag = RT_DEVICE_FLAG_DAM_TX | RT_DEVICE_FLAG_RX,
        .dma_rx = rx_dma_configs[UART7_INDEX],
        .dma_tx = tx_dma_configs[UART7_INDEX]
#endif 
    },
    #endif
};


/**
* @brief UART MSP Initialization
*        This function configures the hardware resources used in this example:
*           - Peripheral's clock enable
*           - Peripheral's GPIO Configuration
*           - NVIC configuration for UART interrupt request enable
* @param huart: UART handle pointer
* @retval None
*/
void gd32_uart_gpio_init(struct gd32_uart *uart)
{
    /* enable USART clock */
    rcu_periph_clock_enable(uart->tx_gpio_clk);
    rcu_periph_clock_enable(uart->rx_gpio_clk);
    rcu_periph_clock_enable(uart->per_clk);

#if defined SOC_SERIES_GD32F4xx
    /* connect port to USARTx_Tx */
    gpio_af_set(uart->tx_port, uart->tx_af, uart->tx_pin);

    /* connect port to USARTx_Rx */
    gpio_af_set(uart->rx_port, uart->rx_af, uart->rx_pin);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(uart->tx_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, uart->tx_pin);
    gpio_output_options_set(uart->tx_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, uart->tx_pin);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(uart->rx_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, uart->rx_pin);
    gpio_output_options_set(uart->rx_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, uart->rx_pin);

#else
    /* connect port to USARTx_Tx */
    gpio_init(uart->tx_port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, uart->tx_pin);

    /* connect port to USARTx_Rx */
    gpio_init(uart->rx_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, uart->rx_pin);
#endif

    NVIC_SetPriority(uart->irqn, 0);
    NVIC_EnableIRQ(uart->irqn);
}

/**
  * @brief  uart configure
  * @param  serial, cfg
  * @retval None
  */
static rt_err_t gd32_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct gd32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct gd32_uart *)serial->parent.user_data;

    gd32_uart_gpio_init(uart);

    usart_baudrate_set(uart->uart_periph, cfg->baud_rate);

    switch (cfg->data_bits)
    {
    case DATA_BITS_9:
        usart_word_length_set(uart->uart_periph, USART_WL_9BIT);
        break;

    default:
        usart_word_length_set(uart->uart_periph, USART_WL_8BIT);
        break;
    }

    switch (cfg->stop_bits)
    {
    case STOP_BITS_2:
        usart_stop_bit_set(uart->uart_periph, USART_STB_2BIT);
        break;
    default:
        usart_stop_bit_set(uart->uart_periph, USART_STB_1BIT);
        break;
    }

    switch (cfg->parity)
    {
    case PARITY_ODD:
        usart_parity_config(uart->uart_periph, USART_PM_ODD);
        break;
    case PARITY_EVEN:
        usart_parity_config(uart->uart_periph, USART_PM_EVEN);
        break;
    default:
        usart_parity_config(uart->uart_periph, USART_PM_NONE);
        break;
    }

    usart_receive_config(uart->uart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart->uart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(uart->uart_periph);

    return RT_EOK;
}


/**
  * @brief  uart control
  * @param  serial, arg
  * @retval None
  */
static rt_err_t gd32_uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct gd32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct gd32_uart *)serial->parent.user_data;
#ifdef RT_SERIAL_USING_DMA
    rt_ubase_t ctr_arg = (rt_ubase_t)arg;
#endif 
    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        NVIC_DisableIRQ(uart->irqn);
        /* disable interrupt */
        usart_interrupt_disable(uart->uart_periph, USART_INT_RBNE);

        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        NVIC_EnableIRQ(uart->irqn);
        /* enable interrupt */
        usart_interrupt_enable(uart->uart_periph, USART_INT_RBNE);
        break;
#ifdef RT_SERIAL_USING_DMA
    case RT_DEVICE_CTRL_CONFIG:
        if(ctr_arg == RT_DEVICE_FLAG_DMA_RX)
        {
            gd32_dma_rx_config(uart);
        }
        else if(ctr_arg == RT_DEVICE_FLAG_DMA_TX)
        {
            gd32_dma_tx_config(uart);
        }

    break;
#endif
    }


    return RT_EOK;
}

/**
  * @brief  uart put char
  * @param  serial, ch
  * @retval None
  */
static int gd32_uart_putc(struct rt_serial_device *serial, char ch)
{
    struct gd32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct gd32_uart *)serial->parent.user_data;
    
    usart_data_transmit(uart->uart_periph, ch);
    while((usart_flag_get(uart->uart_periph, USART_FLAG_TBE) == RESET));

    return RT_EOK;
}

/**
  * @brief  uart get char
  * @param  serial
  * @retval None
  */
static int gd32_uart_getc(struct rt_serial_device *serial)
{
    int ch;
    struct gd32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct gd32_uart *)serial->parent.user_data;

    ch = -1;
    if (usart_flag_get(uart->uart_periph, USART_FLAG_RBNE) != RESET)
        ch = usart_data_receive(uart->uart_periph);
    return ch;
}

/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void GD32_UART_IRQHandler(struct rt_serial_device *serial)
{
    struct gd32_uart *uart = (struct gd32_uart *) serial->parent.user_data;

    RT_ASSERT(uart != RT_NULL);
#ifdef RT_SERIAL_USING_DMA  
    rt_size_t total_index, recv_len, dma_cnt;
    rt_base_t level;
#endif
    /* UART in mode Receiver -------------------------------------------------*/
    if ((usart_interrupt_flag_get(uart->uart_periph, USART_INT_FLAG_RBNE) != RESET) &&
            (usart_flag_get(uart->uart_periph, USART_FLAG_RBNE) != RESET))
    {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        usart_flag_clear(uart->uart_periph, USART_FLAG_RBNE);
    }
#ifdef RT_SERIAL_USING_DMA   
	
	/* check DMA flag is set or not */
    if(dma_interrupt_flag_get(uart->dma_rx->dma_periph,uart->dma_rx->channel,DMA_INT_FLAG_FTF) != RESET) 
	{
	 	 /* clear DMA a channel flag */
		dma_interrupt_flag_clear(uart->dma_rx->dma_periph, uart->dma_rx->channel,DMA_INT_FLAG_FTF);
		
        usart_dma_receive_config(uart->uart_periph, USART_RECEIVE_DMA_DISABLE);
        dma_channel_disable(uart->dma_rx->dma_periph, uart->dma_rx->channel);
        dma_transfer_number_config(uart->dma_rx->dma_periph, uart->dma_rx->channel, uart->serial->config.bufsz );
        dma_channel_enable(uart->dma_rx->dma_periph, uart->dma_rx->channel);
        usart_dma_receive_config(uart->uart_periph, USART_RECEIVE_DMA_ENABLE);
 
//	  	gd32_dma_rx_config(uart);
	}
	
    if(usart_interrupt_flag_get(uart->uart_periph, USART_INT_FLAG_IDLE) != RESET) 
    {
        usart_flag_get(uart->uart_periph,USART_FLAG_IDLE);
        usart_data_receive(uart->uart_periph);
        
        level = rt_hw_interrupt_disable();
 
        dma_cnt = dma_transfer_number_get(uart->dma_rx->dma_periph, uart->dma_rx->channel);
        total_index = uart->serial->config.bufsz - dma_cnt;// DMA_CH4CNT(DMA0);     //uart0 use dma0 ch4
        if (total_index > uart->dma_rx->last_index)
        {
            recv_len = total_index - uart->dma_rx->last_index;
        }
        else
        {
            recv_len = total_index + (uart->serial->config.bufsz - uart->dma_rx->last_index);
        }
        
        if ((recv_len > 0) && (recv_len < uart->serial->config.bufsz))
        {
            if(dma_cnt)  //不用环，用环得加DMA接收完成中断
            {
                uart->dma_rx->last_index = total_index;
            }
            else
            {
                uart->dma_rx->last_index = 0;
            }
            rt_hw_interrupt_enable(level);
 
            rt_hw_serial_isr(uart->serial, RT_SERIAL_EVENT_RX_DMADONE | (recv_len << 8));
        }
        else
        {
            rt_hw_interrupt_enable(level);
        }
//        //transfer completed, configurate dma
//        if(dma_cnt == 0)
//        {
//            usart_dma_receive_config(uart->uart_periph, USART_DENR_DISABLE);
//            dma_channel_disable(uart->dma_rx->dma_periph, uart->dma_rx->channel);
//            dma_transfer_number_config(uart->dma_rx->dma_periph, uart->dma_rx->channel, uart->serial->config.bufsz );
//            dma_channel_enable(uart->dma_rx->dma_periph, uart->dma_rx->channel);
//            usart_dma_receive_config(uart->uart_periph, USART_DENR_ENABLE);
//        }
    }
    if(usart_interrupt_flag_get(uart->uart_periph, USART_INT_FLAG_TC) != RESET) 
    {            
        usart_flag_clear(uart->uart_periph, USART_FLAG_TC);
        rt_hw_serial_isr(uart->serial, RT_SERIAL_EVENT_TX_DMADONE);  
    }
#endif

}

static const struct rt_uart_ops gd32_uart_ops =
{
    .configure = gd32_uart_configure,
    .control = gd32_uart_control,
    .putc = gd32_uart_putc,
    .getc = gd32_uart_getc,
    RT_NULL,
};

/**
  * @brief  uart init
  * @param  None
  * @retval None
  */
int rt_hw_usart_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    int i;

    int result;

    for (i = 0; i < sizeof(uart_obj) / sizeof(uart_obj[0]); i++)
    {
        uart_obj[i].serial->ops    = &gd32_uart_ops;
        uart_obj[i].serial->config = config;

        /* register UART1 device */
        result = rt_hw_serial_register(uart_obj[i].serial,
                              uart_obj[i].device_name,
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX|
                              RT_DEVICE_FLAG_DMA_RX| RT_DEVICE_FLAG_DMA_TX,
                              (void *)&uart_obj[i]);
        RT_ASSERT(result == RT_EOK);
    }

    return result;
}

INIT_BOARD_EXPORT(rt_hw_usart_init);

#endif
