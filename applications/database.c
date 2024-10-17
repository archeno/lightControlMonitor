#include "database.h"

rt_align(RT_ALIGN_SIZE)
ccr_data_t g_ccr_data;

sys_para_t g_sys_param;

int data_base_init(void)
{
	//g_ccr_data.ccr_info.angle = MIN_PASS_ANGLE;  //
//	g_ccr_data.ccr_info.i_scr = 1691;
//	g_ccr_data.ccr_info.i_load = 341;
//	g_ccr_data.ccr_info.v_scr = 2231;
//	g_ccr_data.ccr_info.v_load = 1234;
	return 0;
    
}

#define ABS(a,b) (a>b)?a-b:b-a 

rt_uint8_t get_real_idensity(rt_uint16_t iload)
{
	static rt_uint8_t idensity = 0;
	const rt_uint16_t  i_ref[6]= {0, 280, 340, 410, 520, 660};
	// const rt_uint16_t  u_ref[6]= {0,1076, 1120, 1180, 1306, 1617};
	const rt_uint16_t  u_ref[6]= {0,4303, 4401, 4534, 4735, 5076};

	
	if(iload < 200)
	{
		idensity = 0;
	}
	else if(iload < 310)
	{
		idensity = 1;
	}
	else if(iload < 375)
	{
		idensity = 2;
	}
	else if(iload < 465)
	{
		idensity = 3;
	}
	else if( iload < 590)
	{
		idensity = 4;
	}
	else {
		idensity  = 5;
	}
	// for(int i=0; i<6; i++)
	// {
	// 	if((ABS(iload, i_ref[i]))<30)
	// 	{
	// 		idensity = i;
	// 		break;
	// 	}
	// }
	// if(idensity == 0)
	// {
	// 	rt_kprintf("fefef");
	// }
	g_ccr_data.ccr_info.local_real_idensity = idensity;	
	g_ccr_data.ccr_info.local_v_load = u_ref[idensity];
	return idensity;
}

INIT_PREV_EXPORT(data_base_init);
