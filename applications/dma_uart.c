#include "board.h"
#define DBG_TAG "dma_uart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <string.h>
#define UART_NAME "uart3"
static rt_uint8_t send_str[20] = "hello";
static rt_device_t serial;
rt_thread_t thread_uart;
static void uart_thread_entry(void *param)
{
    while (1)
    {
        rt_device_write(serial, 0, send_str, strlen(send_str));
        rt_thread_mdelay(1000);
    }
}
int uart_dma_test(void)
{
    rt_err_t ret;

    serial = rt_device_find(UART_NAME);
    // config uart param
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = BAUD_RATE_9600;
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    LOG_I("enter uar_dma_test");
    ret = rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    if (ret != RT_EOK)
    {
        LOG_I("open serial %s failed ret is %d", UART_NAME, ret);
    }
    thread_uart = rt_thread_create("uart3_dma", uart_thread_entry, 0, 512, 10, 100);
    if (thread_uart != RT_NULL)
    {
        rt_thread_startup(thread_uart);
    }
    rt_device_write(serial, 0, "hello", 5);
}
// INIT_APP_EXPORT(uart_dma_test);
