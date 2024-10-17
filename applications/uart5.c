#include "uart5.h"

#define UART_NAME	"uart5"											


static struct rt_semaphore rx_sem;									
static rt_device_t serial;										
static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;	

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{

    rt_sem_release(&rx_sem);

    return RT_EOK;
}

char temp[256];
char rcvDat[256];
int rcv_len =0;
char a;

static void uart5_test(void *parameter)
{
	
    rt_sem_init(&rx_sem, "uart5_rx_sem", 0, RT_IPC_FLAG_FIFO);
	

	serial = rt_device_find(UART_NAME);
	if (serial == RT_NULL)
    {
        rt_kprintf("find %s failed!\n", UART_NAME);
        return;
    }
	
	
	config.baud_rate = BAUD_RATE_19200;			
	config.data_bits = DATA_BITS_8;				
	config.stop_bits = STOP_BITS_1;			
	config.bufsz     = 125;						
	config.parity    = PARITY_NONE;			
	rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
	

    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX );
	
	
    rt_device_set_rx_indicate(serial, uart_input);
	

	char ch;

	while (1)
    {

     
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
     
        ch = ch + 1;
        rt_device_write(serial, 0, &ch, 1);
		rt_kprintf("%c", ch);
    }
}

static int uart5_thread(void)
{
	rt_thread_t thread = RT_NULL;
	
	thread = rt_thread_create("uart5_run", uart5_test, RT_NULL, 1024, 4, 10);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
	else
    {
		return -RT_ERROR;
    }

	return RT_EOK;
}
//INIT_APP_EXPORT(uart5_thread);
