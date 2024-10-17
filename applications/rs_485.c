#include <rs_485.h>

#define DBG_TAG "rs-485"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>


static void usart0_dr_pin_init()
{
    rt_pin_mode(USART0_DR_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(USART0_DR_PIN, PIN_LOW);
}

typedef struct
{
    rt_device_t dev;
    char uart_name[8];
    struct rt_semaphore rx_sem;
} rs_485_obj_t;

rs_485_obj_t rs_485_objs[RS_485_NUM];

static rt_err_t uart0_ind_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rs_485_objs[RS_485_UART0].rx_sem);
}
static rt_err_t uart2_ind_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rs_485_objs[RS_485_UART2].rx_sem);
}

typedef rt_err_t (*rx_ind)(rt_device_t dev, rt_size_t size);

typedef struct
{
    char *uart_name;
    char *rx_sem_name;
    rx_ind rx_indicate;
} rs_485_config_t;

const rs_485_config_t rs_485_config[RS_485_NUM] = {
    {.uart_name = "uart0", .rx_sem_name = "uart0_sem", uart0_ind_cb},
    {.uart_name = "uart2", .rx_sem_name = "uart2_sem", uart2_ind_cb}
};


int  rs_485_send_data(RS_485_PORT port, uint8_t *buf, int len)
{
    int send_len;
    send_len = rt_device_write(rs_485_objs[port].dev, 0, buf, len);
    return send_len;
}

int rs_485_receive(RS_485_PORT port, uint8_t *buf, int bufsz, int timeout, int bytes_timeout)
{
    int len =0;
    while (1) 
    {
        rt_sem_control(&rs_485_objs[port].rx_sem, RT_IPC_CMD_RESET, RT_NULL);

        int rc = rt_device_read(rs_485_objs[port].dev, 0, buf+len, bufsz);
        if(rc >0)
        {
            timeout = bytes_timeout;
            len += rc;
            bufsz -= rc;
            if (bufsz == 0)
            {
                break;
            }
            continue;
        }
        
        if (rt_sem_take(&rs_485_objs[port].rx_sem, rt_tick_from_millisecond(timeout)) != RT_EOK)
            break;
        timeout = bytes_timeout;
    }
    return len;
}



static int rs_485_init(void)
{

    usart0_dr_pin_init();
    rt_err_t res;
    for (int i = 0; i < RS_485_NUM; i++)
    {
        rs_485_objs[i].dev = rt_device_find(rs_485_config[i].uart_name);
        if (rs_485_objs[i].dev != RT_NULL)
        {
            rt_sem_init(&rs_485_objs[i].rx_sem, rs_485_config[i].rx_sem_name, 0, RT_IPC_FLAG_FIFO);
            rt_device_set_rx_indicate(rs_485_objs[i].dev, rs_485_config[i].rx_indicate);
        }

        res = rt_device_open(rs_485_objs[i].dev, RT_DEVICE_FLAG_INT_RX);
        if(res != RT_EOK)
        {
            LOG_E("open rs485 failed!");
        }
    }
        //ups baud 2400
        /* step2：修改串口配置参数 */
        struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */
        config.baud_rate = BAUD_RATE_9600;        //修改波特率为 2400
        rt_device_control(rs_485_objs[RS_485_UART2].dev, RT_DEVICE_CTRL_CONFIG,  &config);

    return 0;
}

INIT_DEVICE_EXPORT(rs_485_init);
