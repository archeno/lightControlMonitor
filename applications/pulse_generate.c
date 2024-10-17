/**
 * @file pulse_generate.c
 * @author fy (yfeng880@163.com)
 * @brief generate zero_cross pulse from op compare output pulse
 * @version 1.0
 * @date 2024-10-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "pulse_generate.h"
#include "board.h"
#include "drv_adc.h"

#define MaxCaptureCurWidth_ms      19.5f                //unit:ms
#define MinCaptureCurWidth_ms      10.5f                 //unit:ms
rt_uint8_t g_calibrate_flag = 0;
rt_uint16_t cur_buf[SAMPLES*ADC_CANNEL_NUM];
rt_device_t hw_timer0;                    /* 定时器设备句柄 */
rt_device_t hw_timer1;                    /* 定时器设备句柄 */
rt_device_t hw_timer2;                    /* 定时器设备句柄 */

rt_hwtimerval_t timeout_s;               /* 定时器超时值 */
#define INT1_PIN        GET_PIN(B,  2)
#define LED2_PIN        GET_PIN(B, 15)

#define PULSE_ON()        rt_pin_write(LED2_PIN, PIN_LOW)
#define PULSE_OFF()       rt_pin_write(LED2_PIN, PIN_HIGH)

uint32_t icr1_time = 0;
static void int1_callback(void * arg);
/**
 * @brief configure INT1 (PB2) as  interrupt intput
 * 
 */
static void compare_output_pin_init(void)
{

    rt_pin_mode(INT1_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(INT1_PIN, PIN_IRQ_MODE_RISING_FALLING, int1_callback, RT_NULL);
    rt_pin_irq_enable(INT1_PIN, PIN_IRQ_ENABLE);
    nvic_irq_enable(EXTI2_IRQn, 0, 0);

}

static void led_gpio_init(void)
{
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED1_PIN, PIN_HIGH);
    rt_pin_write(LED2_PIN, PIN_HIGH);
}


void start_timer2(uint32_t delay_us)
{
    rt_hwtimerval_t timeout;
    timeout.sec = 0;
    timeout.usec = delay_us;
	rt_device_write(hw_timer2,0,&timeout, sizeof(timeout));
}

void stop_timer2()
{
    rt_device_control(hw_timer2, HWTIMER_CTRL_STOP, 0);
}

rt_err_t timer2_cb(rt_device_t  dev, rt_size_t size)
{
    stop_timer2();
    PULSE_OFF();
	return 0;
}
void start_timer1(uint32_t delay_us)
{
    rt_hwtimerval_t timeout;
    timeout.sec = 0;
    timeout.usec = delay_us;
	rt_device_write(hw_timer1,0,&timeout, sizeof(timeout));
}

void stop_timer1()
{
    rt_device_control(hw_timer1, HWTIMER_CTRL_STOP, 0);
}

rt_err_t timer1_cb(rt_device_t  dev, rt_size_t size)
{
    PULSE_ON();
    start_timer2(1000);  //1ms pulse
    if(g_calibrate_flag)
    {
        start_timer1(10000);  //10ms next pulse
        g_calibrate_flag = 0;
        timer_disable(TIMER4);  //stop adc sample
        get_dma_data(cur_buf);
        timer_enable(TIMER4);  //start adc sample
        cal_rms(cur_buf);  
    }
    //generate cross-zero pulse and start 10ms timer for next pulse

	return 0;
}

rt_err_t timer0_cb(rt_device_t  dev, rt_size_t size)
{
    rt_device_read(hw_timer0, 0, &timeout_s, sizeof(timeout_s));
}
uint32_t t1_max = 0;
uint32_t t2_max = 0;

static void int1_callback(void * arg)
{
    rt_uint8_t pin_levle;
    static uint32_t t1_us = 0;
    static uint32_t t2_us = 0;
    static rt_uint32_t cross_pulse_us =0;
    pin_levle = rt_pin_read(INT1_PIN);
    if(!pin_levle) /**< down edage */
    {
        //  PULSE_OFF();
         rt_device_read(hw_timer0, 0, &timeout_s, sizeof(timeout_s));
         if(t1_us > t1_max)
         {
            t1_max = t1_us;
         }
         t1_us = timeout_s.usec;
         if(g_calibrate_flag)
        {
            start_timer1(cross_pulse_us);
        }
         
    }
    else
    { 
        // PULSE_ON();
         rt_device_read(hw_timer0, 0, &timeout_s, sizeof(timeout_s));
         t2_us = timeout_s.usec;
         if(t2_us > t2_max)
         {
            t2_max = t2_us;
         }
         if(t2_us > t1_us)
         {
            icr1_time  = t2_us - t1_us ;
         }
         else
         {
            icr1_time = 1000000 - t1_us + t2_us ;
         }
        
        if((icr1_time >=MinCaptureCurWidth_ms*1000) && (icr1_time <=MaxCaptureCurWidth_ms*1000) )
        {
            cross_pulse_us = icr1_time/2-5*1000;
          
            g_calibrate_flag = 1;
        }
    }
}

static void pulse_timer_init(void)
{
    rt_hwtimer_mode_t mode;    
  
    hw_timer0  = rt_device_find("timer1"); /** icr1 time */
    hw_timer1  = rt_device_find("timer2"); /** zero-cross pulse  */
    hw_timer2  = rt_device_find("timer3"); /** zero- cross pusle with  */

    rt_device_open(hw_timer0, RT_DEVICE_OFLAG_RDWR);
    rt_device_open(hw_timer1, RT_DEVICE_OFLAG_RDWR);
    rt_device_open(hw_timer2, RT_DEVICE_OFLAG_RDWR);

    mode = HWTIMER_MODE_PERIOD;
    rt_device_control(hw_timer0, HWTIMER_CTRL_MODE_SET, &mode);
    rt_device_set_rx_indicate(hw_timer0, timer0_cb);
    mode = HWTIMER_MODE_ONESHOT;
    rt_device_control(hw_timer1, HWTIMER_CTRL_MODE_SET, &mode);
    nvic_irq_enable(TIMER2_IRQn, 0, 1);

    rt_device_set_rx_indicate(hw_timer1, timer1_cb);
    rt_device_control(hw_timer2, HWTIMER_CTRL_MODE_SET, &mode);
    rt_device_set_rx_indicate(hw_timer2, timer2_cb);
    
      /* 设置定时器超时值为5s并启动定时器 */
    timeout_s.sec = 0;      /* 秒 */
    timeout_s.usec = 100000;     /* 微秒 */
    if (rt_device_write(hw_timer0, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
    }

    
}

int pulse_generate_init()
{
    //init int1 gpio
    compare_output_pin_init();
    //init timer
    pulse_timer_init();
    //init pulse_pin-- led2
    led_gpio_init();
}
INIT_DRIVER_EARLY_EXPORT(pulse_generate_init);
