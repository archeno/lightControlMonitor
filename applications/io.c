#include "io.h"
#include "global.h"
#include <tca9535.h>

rt_uint8_t g_pin_release_cnt;

void release_rleay_ctrl_pin(void)
{
		if(g_pin_release_cnt >0)
		{
			g_pin_release_cnt--;
			if(g_pin_release_cnt==0)
			{
				rt_pin_write(O0_RESET_PIN, PIN_HIGH);
				rt_pin_write(O1_RESET_PIN, PIN_HIGH);
				rt_pin_write(O2_RESET_PIN, PIN_HIGH);
				rt_pin_write(O3_RESET_PIN, PIN_HIGH);
				rt_pin_write(O4_RESET_PIN, PIN_HIGH);
				// rt_pin_write(O5_RESET_PIN, PIN_HIGH);
				rt_pin_write(O6_RESET_PIN, PIN_HIGH);
				rt_pin_write(O7_RESET_PIN, PIN_HIGH);

				rt_pin_write(O0_SET_PIN, PIN_HIGH);
				rt_pin_write(O1_SET_PIN, PIN_HIGH);
				rt_pin_write(O2_SET_PIN, PIN_HIGH);
				rt_pin_write(O3_SET_PIN, PIN_HIGH);
				rt_pin_write(O4_SET_PIN, PIN_HIGH);
			    // rt_pin_write(O5_SET_PIN, PIN_HIGH);
				rt_pin_write(O6_SET_PIN, PIN_HIGH);
			    rt_pin_write(O7_SET_PIN, PIN_HIGH);
			}
		}
}




rt_base_t set_pin_array[] = 
{  	O0_SET_PIN, 
	O1_SET_PIN,
	O2_SET_PIN,
	O3_SET_PIN,
	O4_SET_PIN,
	// O5_SET_PIN,
	O6_SET_PIN,
	O7_SET_PIN
};


rt_base_t reset_pin_array[] = 
{  	O0_RESET_PIN, 
	O1_RESET_PIN,
	O2_RESET_PIN,
	O3_RESET_PIN,
	O4_RESET_PIN,
	// O5_RESET_PIN,
	O6_RESET_PIN,
	O7_RESET_PIN
};

rt_base_t idensity_input_pin_array[] = 
{  	
	O0_IN_PIN, 
	O1_IN_PIN,
	O2_IN_PIN, 
	O3_IN_PIN,
	O4_IN_PIN
};

static void pin_output(rt_uint8_t port_register)
{
		rt_uint8_t  port_data = port_register;

		switch(port_data)
		{
			case 0:
				CCR_OFF();
				break;
			case 1:
				CCR_G1();
				break;
			case 2:
				CCR_G2();
				break;
			case 3:
				CCR_G3();
				break;
			case 4:
				CCR_G4();
				break;
			case 5:
				CCR_G5();
				break;
			default:
			break;
		}
	
	// for (int i=0; i<8; i++)
	// {
	// 	if(port_data & (1<<i))
	// 	{
	// 		 rt_pin_write(set_pin_array[i], PIN_LOW);
	// 	     g_pin_release_cnt = RT_TICK_PER_SECOND/100;
	// 	}
	// 	else
	// 	{
	// 		 rt_pin_write(reset_pin_array[i], PIN_LOW);
	// 	     g_pin_release_cnt = RT_TICK_PER_SECOND/100;
	// 	}			
	// }		
}
/**
 * @brief 检测光级引脚电平，并更新光级信息，
 * 由于磁保持继电器，就不用掉电保存光级信息。
 * 
 */
static void  idensity_check(void)
{
	for(int i=0; i<5; i++)
	{
	  if(PIN_LOW == rt_pin_read(idensity_input_pin_array[i]))
	  {
			sys.mycan.para.cmd_idensity = i+1;
			break;
	  }
	}
}
void output_check(void* para)
{
	static rt_uint8_t cmd_idensity_last;
	idensity_check();
	const rt_uint8_t idensity_out_map[] = {0, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	while(1)
	{
		if(cmd_idensity_last != sys.mycan.para.cmd_idensity )
		{	
			rt_uint8_t outport = idensity_out_map[sys.mycan.para.cmd_idensity];
			outport = inverse_byte(outport);
			TCA9535_Write_Port(TCA9535_LED_OUT2_ADDR, TCA9535_PORT0, ~outport);
			pin_output(sys.mycan.para.cmd_idensity);			
			cmd_idensity_last = sys.mycan.para.cmd_idensity;
			// if(!sys.mycan.para.can0_comm_fail_flag)
			// {
			// 	rt_event_send(&sys.mycan.ccr_event_updata0, EVENT_OUTPUT_CHANGED);	
			// }
			// else if(!sys.mycan.para.can1_comm_fail_flag)
			// {
			// 	rt_event_send(&sys.mycan.ccr_event_updata1, EVENT_OUTPUT_CHANGED);		
			// }
		}			
		rt_thread_mdelay(50);		
	}
}

int RT_IO_Init()
{
	rt_pin_mode(O0_SET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O0_SET_PIN, PIN_HIGH);
	rt_pin_mode(O0_RESET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O0_RESET_PIN, PIN_HIGH);
	rt_pin_mode(O0_IN_PIN, PIN_MODE_INPUT);
	
    rt_pin_mode(O1_SET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O1_SET_PIN, PIN_HIGH);		 
	rt_pin_mode(O1_RESET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O1_RESET_PIN, PIN_HIGH);			 
	rt_pin_mode(O1_IN_PIN, PIN_MODE_INPUT);

    rt_pin_mode(O2_SET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O2_SET_PIN, PIN_HIGH);		 
	rt_pin_mode(O2_RESET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O2_RESET_PIN, PIN_HIGH);			 
	rt_pin_mode(O2_IN_PIN, PIN_MODE_INPUT);
	
	rt_pin_mode(O3_SET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O3_SET_PIN, PIN_HIGH);		 
	rt_pin_mode(O3_RESET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O3_RESET_PIN, PIN_HIGH);			 
	// rt_pin_mode(O3_IN_PIN, PIN_MODE_INPUT);
	
	rt_pin_mode(O4_SET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O4_SET_PIN, PIN_HIGH);		 
	rt_pin_mode(O4_RESET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O4_RESET_PIN, PIN_HIGH);			 
	// rt_pin_mode(O4_IN_PIN, PIN_MODE_INPUT);
	
	// rt_pin_mode(O5_SET_PIN, PIN_MODE_OUTPUT);
	// rt_pin_write(O5_SET_PIN, PIN_HIGH);		 
	// rt_pin_mode(O5_RESET_PIN, PIN_MODE_OUTPUT);
	// rt_pin_write(O5_RESET_PIN, PIN_HIGH);			 
	// rt_pin_mode(O5_IN_PIN, PIN_MODE_INPUT);
	
	rt_pin_mode(O6_SET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O6_SET_PIN, PIN_HIGH);		 
	rt_pin_mode(O6_RESET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O6_RESET_PIN, PIN_HIGH);			 
	// rt_pin_mode(O6_IN_PIN, PIN_MODE_INPUT);
	
	rt_pin_mode(O7_SET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O7_SET_PIN, PIN_HIGH);		 
	rt_pin_mode(O7_RESET_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(O7_RESET_PIN, PIN_HIGH);			 
	// rt_pin_mode(O7_IN_PIN, PIN_MODE_INPUT);

	// TCA9535_init();
	rt_thread_t output_check_thread = rt_thread_create("outputCheck", output_check,RT_NULL, 1024, 23, 10);
	if(RT_NULL != output_check_thread)
	{
		rt_thread_startup(output_check_thread);
	}
	else
	{
		rt_kprintf("output_check_thread create failed!");
	}
	
	
	return 0;
}

// INIT_DEVICE_EXPORT(RT_IO_Init);



