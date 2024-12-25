#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "rtdef.h"
#include "rtthread.h"

// 40101
typedef struct sys_param
{
	float In_a2; // 鐢垫祦姣斾緥绯绘暟a	  y = a2x^2 + a1x +b
	float In_a1;
	float In_b; // 鐢垫祦姣斾緥绯绘暟b
	float Un_a; // SCR鐢靛帇姣斾緥绯绘暟a
	float Un_b; // SCR鐢靛帇甯告暟b
} sys_para_t;

extern sys_para_t g_sys_param;
// struct ccr_datastruct
//{
//
//	struct sys_param  m_sysparam;
//	/* Read Only 鍖?*/
//	rt_uint16_t i_load;		//鐏?鍏夊洖璺?鐢垫祦		0.01A
//	rt_uint16_t i_scr;		//鍙?鎺х?呭洖璺?鐢垫??	0.01A
//
//	rt_uint32_t v_scr;	   	//鍙?鎺х?呰緭鍑虹數鍘? 	0.1v
//	rt_uint32_t v_load;		//鐏?鍏夊洖璺?鐢靛帇  	0.1v
//
//	rt_uint16_t freq;	  	 //鐢垫簮棰戠巼 0.1hz
//	rt_uint16_t half_period;  //鍗婃尝鍛ㄦ湡
//	rt_uint16_t angle;		  //瀵奸�氳??
//
//	rt_uint16_t status;		//璇﹁?佽繍琛岀姸鎬佸瓧璇存槑  bit0: 1:run
//
//	float  CCRInt0Time;
//
//
// };
typedef struct hardWareTest_
{
	rt_uint8_t intputP0;
	rt_uint8_t intputP1;
	rt_uint8_t rs_485 : 1;
	rt_uint8_t can : 1;
	rt_uint8_t i2c : 1;

} HardWareSt_t;

struct ccr_datastruct
{

	rt_uint16_t i_load;		   // 回路电流	0.01A	//0x79
	rt_uint16_t v_load;		   // 回路电压  	0.1v
	rt_uint16_t real_idensity; // 实际光级

	rt_uint8_t group_id;
	rt_uint8_t node_id;

	rt_uint8_t INPUT_P0; /**< addr 3*/
	rt_uint8_t INPUT_P1;
	rt_uint8_t OUTPUT_P0;
	rt_uint8_t cmd_idensity; // 命令光级
	rt_uint16_t ccr_local_remote : 1;
	rt_uint16_t ccr_relay_state : 1;
	rt_uint16_t ccr_power_on_state : 1;
	rt_uint16_t ccr_reserved : 13;
	union
	{
		rt_uint16_t ccr_alarm_state;
		struct
		{
			rt_uint8_t power_off : 1;	 // 市电掉电关机
			rt_uint8_t open_circuit : 1; // 回路开路关机
			rt_uint8_t oc_fault : 1;	 // 过流保护关机
			rt_uint8_t ccr_module_fault : 1;
			rt_uint8_t rv_dv : 1;				  // bit4 rv/dv
			rt_uint8_t idensity_fail : 1;		  // 光级不符
			rt_uint8_t open_circuit_prealarm : 1; // 回路开路预警
			rt_uint8_t open_circuit_alarm : 1;	  // bit7 回路开路报警

			rt_uint8_t VA_drop_10_percent : 1;		// 回路短路预警 /VA跌落10%
			rt_uint8_t short_circuit_alarm : 1;		// 回路短路报警
			rt_uint8_t insulation_res_prealarm : 1; // bit 10 绝缘电阻越限预警
			rt_uint8_t insulatin_res_alarm : 1;		// bit 11 绝缘电阻越限报警
			rt_uint8_t lamp_prealarm : 1;			// bit 12 坏等数越限预警
			rt_uint8_t lamp_alarm : 1;				// bit 13 坏等数越限报警
			rt_uint8_t insulatin_module_alarm : 1;	// bit 14 绝缘电阻模块故障
			rt_uint8_t ccr_module_alarm : 1;		// bit 15 调光器模块故障
		};
	};
	// union
	// {
	// 	rt_uint16_t ccr_alarm_state;
	// 	struct
	// 	{
	// 		rt_uint16_t alarm_happend:1;
	// 		rt_uint16_t power_abnomal:1; 		//市电异常
	// 		rt_uint16_t power_down:1; 			//市电掉电
	// 		rt_uint16_t oc_5percent:1; 			//输出5%过流
	// 		rt_uint16_t oc_25percent:1; 		//输出25%过流
	// 		rt_uint16_t ov_30percent:1;			//输出30%过压
	// 		rt_uint16_t lv_10percent:1;			//输出10%欠压
	// 		rt_uint16_t breakoff:1;				//开路故障
	// 		rt_uint16_t RegFault:1;				//bit8 光级不符
	// 		rt_uint16_t efd_prealrm:1;			//绝缘电阻预警
	// 		rt_uint16_t efd_alrm:1;				//绝缘电阻报警
	// 		rt_uint16_t lfd_prealarm:1;			//坏等数预警
	// 		rt_uint16_t ccr_module_fault:1;	    //调光器模块故障
	// 		rt_uint16_t lfd_alarm:1;	    	//坏等数报警
	// 		rt_uint16_t reserved:2;	    		//预留
	// 	};
	// };

	rt_uint16_t ups_input_voltage; // 7
	rt_uint16_t ups_output_voltage;
	rt_uint16_t ups_battery_voltage;
	rt_uint16_t ups_battery_capacity;
	rt_uint16_t ups_battery_duration_min;
	rt_uint16_t ups_battery_duration_s;
	rt_uint16_t ups_read_alarm_status;
	union
	{
		rt_uint16_t ups_alarm_status; // bit0:com failed  bit1：power down  bit2: battery low
		struct
		{
			rt_uint16_t ups_comm_fail : 1;
			rt_uint8_t ups_power_down : 1;
			rt_uint16_t ups_battery_low : 1;
			rt_uint16_t ups_reserverd : 13;
		};
	};
	rt_uint16_t ups_run_stataus; // 1:ups supply  0：power supply
	rt_uint16_t down_board_com_fail_flag;

	rt_uint8_t can0_comm_fail_flag;
	rt_uint8_t can1_comm_fail_flag;

	rt_uint16_t insResistanceValue;
	rt_uint16_t insResistanceUnit;
	rt_uint16_t badLampNum; // 19

	rt_uint32_t seconds;
	rt_uint16_t ms;

	// rt_uint16_t i_load;			//回路电流	0.01A	//0x79
	// rt_uint16_t v_load;			//回路电压  	0.1v
	// rt_uint16_t real_idensity;  //实际光级

	// rt_uint16_t down_board_com_fail_flag; //
	// rt_uint16_t run_time_lv1;
	// rt_uint16_t run_time_lv2;
	// rt_uint16_t run_time_lv3;
	// rt_uint16_t run_time_lv4;
	// rt_uint16_t run_time_lv5;
	// rt_uint16_t run_time_avg_lv5;`
	// rt_uint16_t status;    //鐘舵�佸瓧
	// rt_uint16_t run_status; //杩愯?岀姸鎬佸瓧
	// rt_uint16_t alarm_switch;

	// float  In_a;		     //鐢垫祦姣斾緥绯绘暟a	  y = ax+b
	// float  In_b;             //鐢垫祦姣斾緥绯绘暟b
	// float  Un_a;             //SCR鐢靛帇姣斾緥绯绘暟a
	// float  Un_b;          	 //SCR鐢靛帇甯告暟b
	rt_uint16_t local_i_load;		 // 回路电流	0.01A	//0x79
	rt_uint16_t local_v_load;		 // 回路电压  	0.1v
	rt_uint16_t local_real_idensity; // 实际光级
	rt_uint16_t local_active_power;	 // 回路有功功率  W

	float adc_val_iload;
	float adc_val_vload;

	HardWareSt_t hardwareState;
	//	float  CCRInt0Time;//
};
#define CCR_BUF_LEN sizeof(struct ccr_datastruct)
union ccr_data
{
	rt_uint16_t buf[CCR_BUF_LEN];
	struct ccr_datastruct ccr_info;
};

typedef union ccr_data ccr_data_t;

extern ccr_data_t g_ccr_data;

#define RELAY_STATUS_OFF 0
#define RELAY_STATUS_ON 1

// 绯荤粺鐘舵�?  bit 0~ 3
#define RELAY_STATUS_POS 0

// FA

#define SYS_STATUS_W g_ccr_data.ccr_info.run_status
#define SYS_ALARM_REG g_ccr_data.ccr_info.status
#define SET_SYS_STATUS_BIT(BIT) (SYS_STATUS_W = SYS_STATUS_W | (1 << BIT))
#define CLR_SYS_STATUS_BIT(BIT) (SYS_STATUS_W = SYS_STATUS_W & (~(1 << BIT)))
#define GET_SYS_STATUS_BIT(BIT) (SYS_STATUS_W & (1 << BIT))

#define SET_ALARM_STATUS_BIT(BIT) (SYS_ALARM_REG |= (1 << BIT))
#define CLR_ALARM_STATUS_BIT(BIT) (SYS_ALARM_REG &= ~(1 << BIT))
#define GET_ALARM_STATUS_BIT(BIT) (SYS_ALARM_REG & (1 << BIT))

#define IS_SYS_FAULT_EXIST() (SYS_ALARM_REG & (0x00ff)) // 浣?8浣嶆湁缃?浣嶈?存槑鏈夋晠闅?

///**  freq **/
// rt_inline void update_freq(rt_uint16_t freq)
//{
//	g_ccr_data.ccr_info.freq = freq;
// }

// rt_inline rt_uint16_t get_freq(void)
//{
//	return g_ccr_data.ccr_info.freq;
// }
rt_inline void update_active_power(uint16_t power)
{
	g_ccr_data.ccr_info.local_active_power = power;
}
/** load voltage **/

rt_inline void update_load_voltage(rt_uint32_t voltage)
{
	if (voltage <= 200)
	{
		voltage = 0;
	}
	g_ccr_data.ccr_info.local_v_load = voltage;
	// g_ccr_data.ccr_info.local_v_load = 300;
}

rt_inline rt_uint32_t get_load_voltage(void)
{
	return g_ccr_data.ccr_info.local_v_load;
}

/** I load  **/
rt_inline void update_i_load(rt_uint16_t i_load)
{
	if (i_load <= 110)
	{
		i_load = 0;
	}
	g_ccr_data.ccr_info.local_i_load = i_load;
	// g_ccr_data.ccr_info.local_i_load = 282;
}
rt_inline rt_uint16_t get_i_load(void)
{
	return g_ccr_data.ccr_info.local_i_load;
}

/** Un ****/
rt_inline void update_Un(float Un_a, float Un_b)
{
	g_sys_param.Un_a = Un_a;
	g_sys_param.Un_b = Un_b;
}
// rt_inline rt_uint16_t get_Un(void)
//{
//	return g_ccr_data.ccr_info.Un;
// }

/** In ****/
// rt_inline void update_In(rt_uint16_t In)
//{
//	g_ccr_data.ccr_info.In_a = In;
// }
// rt_inline rt_uint16_t get_In(void)
//{
//	return g_ccr_data.ccr_info.In_a;
// }
rt_inline void update_In(float In_a2, float In_a1, float In_b)
{
	g_sys_param.In_a2 = In_a2;
	g_sys_param.In_a1 = In_a1;
	g_sys_param.In_b = In_b;
}
rt_inline float get_In_a2(void)
{
	return g_sys_param.In_a2;
}
rt_inline float get_In_a1(void)
{
	return g_sys_param.In_a1;
}
rt_inline float get_In_b(void)
{
	return g_sys_param.In_b;
}
rt_inline float get_Un_a(void)
{
	return g_sys_param.Un_a;
}
rt_inline float get_Un_b(void)
{
	return g_sys_param.Un_b;
}

rt_uint8_t get_real_idensity(rt_uint16_t iload);
// rt_inline void update_pass_angle(float angle)
//{
////    if(angle <=MIN_PASS_ANGLE)
////    {
//        g_ccr_data.ccr_info.angle =angle;
////    }
////    else if(angle >=MAX_PASS_ANGLE)
////	{
////		g_ccr_data.ccr_info.angle  = MAX_PASS_ANGLE;
////	}
//}

// rt_inline float get_pass_angle(float angle)
//{
//	return g_ccr_data.ccr_info.angle;
// }
// rt_inline void update_idensity(rt_uint16_t idensity)
//{
//	if(idensity <=5)
//	{
//		g_ccr_data.ccr_info.intensity  = idensity;
//	}
// }

// rt_inline rt_uint8_t get_idensity(void)
//{
//	return g_ccr_data.ccr_info.intensity;
//
// }
/** angle**/

// rt_inline  void update_angle(rt_uint16_t angle)
//{
//	if(angle >= MAX_PASS_ANGLE)
//	{
//		g_ccr_data.ccr_info.angle = MAX_PASS_ANGLE;
//	}
//	else if(angle<= MIN_PASS_ANGLE)
//	{
//
//	}
// }

#endif
