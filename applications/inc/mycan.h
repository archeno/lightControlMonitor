///////////////////////////////////////////////////////////////////
/// @file 		mycan.h
/// @brief		Ӣ�����can����
///	@details	
/// @author		by DC(any question plese send 
///	@author				E-mail to 1160029144@qq.com)
/// @version	V1.0.0
/// @date 		2022-10-26
/// @copyright	Copyright (c) 2022 �����к�������ɫ�����������޹�˾
///////////////////////////////////////////////////////////////////

#ifndef __MYCAN_H__
#define __MYCAN_H__

#include <rtthread.h>
#include <rtdevice.h>

enum 
{
	CCR1 = 1,
	CCR2,
	CCR3,
	CCR4,
	CCR_STANDBY,//CCR����
	CCR_ATS		//�л���
};

enum 
{
	GROUP0,
	GROUP1,
	GROUP2,
	GROUP3,
	GROUP4,
	GROUP5,
	GROUP6,
	GROUP7,
	GROUP8,
	GROUP9,
	GROUP10,
	GROUP11
};

enum
{
	MSG_CONFIG = 1, //������
	MSG_CTRL,		//������
	MSG_ALARM,		//�澯��
	MSG_STATUS,		//״̬��
	MSG_ANALOG		//ģ����
};
// typedef struct sys_param
// {
// 	float  In_a;		     //鐢垫祦姣斾緥绯绘暟a	  y = ax+b
//     float  In_b;             //鐢垫祦姣斾緥绯绘暟b
// 	float  Un_a;             //SCR鐢靛帇姣斾緥绯绘暟a    
//     float  Un_b;          	 //SCR鐢靛帇甯告暟b
// }sys_para_t;

typedef union {
		rt_uint16_t alarm;
		struct {
		rt_uint8_t power_off :1;	//市电掉电关机
		rt_uint8_t open_circuit:1;	//回路开路关机
		rt_uint8_t oc_fault:1;	//过流保护关机
		rt_uint8_t ccr_module_fault:1;    
		rt_uint8_t rv_dv:1;        //bit4 rv/dv
		rt_uint8_t idensity_fail:1;  //光级不符
		rt_uint8_t open_circuit_prealarm:1;		//回路开路预警
		rt_uint8_t open_circuit_alarm:1;  	// bit7 回路开路报警
		
		rt_uint8_t  VA_drop_10_percent:1; 	//回路短路预警 /VA跌落10%
		rt_uint8_t  short_circuit_alarm:1;	  //回路短路报警
		rt_uint8_t  insulation_res_prealarm:1; //bit 10 绝缘电阻越限预警
		rt_uint8_t  insulatin_res_alarm:1;  	//bit 11 绝缘电阻越限报警
		rt_uint8_t  lamp_prealarm:1;  			//bit 12 坏等数越限预警
		rt_uint8_t  lamp_alarm:1;  				//bit 13 坏等数越限报警
		rt_uint8_t	insulatin_module_alarm:1;  //bit 14 绝缘电阻模块故障
		rt_uint8_t  ccr_module_alarm:1;			//bit 15 调光器模块故障
		
		};
}alarm_state_u;


typedef struct myPara
{
	rt_uint16_t i_load;			//回路电流	0.01A	//0x79		
	rt_uint16_t v_load;			//回路电压  	0.1v
	rt_uint16_t real_idensity;  //实际光级  
	rt_uint8_t group_id;
	rt_uint8_t node_id;
	
	rt_uint8_t INPUT_P0;    /**< addr 3*/
	rt_uint8_t INPUT_P1;  
	rt_uint8_t OUTPUT_P0;  
	rt_uint8_t cmd_idensity;   //命令光级
	rt_uint16_t ccr_local_remote:1;
	rt_uint16_t ccr_relay_state:1;
	rt_uint16_t ccr_power_on_state:1;
	rt_uint16_t ccr_reserved:13;

	union {
		rt_uint16_t ccr_alarm_state;
		struct {
		rt_uint8_t power_off :1;	//市电掉电关机
		rt_uint8_t open_circuit:1;	//回路开路关机
		rt_uint8_t oc_fault:1;	//过流保护关机
		rt_uint8_t ccr_module_fault:1;    
		rt_uint8_t rv_dv:1;        //bit4 rv/dv
		rt_uint8_t idensity_fail:1;  //光级不符
		rt_uint8_t open_circuit_prealarm:1;		//回路开路预警
		rt_uint8_t open_circuit_alarm:1;  	// bit7 回路开路报警
		
		rt_uint8_t  VA_drop_10_percent:1; 	//回路短路预警 /VA跌落10%
		rt_uint8_t  short_circuit_alarm:1;	  //回路短路报警
		rt_uint8_t  insulation_res_prealarm:1; //bit 10 绝缘电阻越限预警
		rt_uint8_t  insulatin_res_alarm:1;  	//bit 11 绝缘电阻越限报警
		rt_uint8_t  lamp_prealarm:1;  			//bit 12 坏等数越限预警
		rt_uint8_t  lamp_alarm:1;  				//bit 13 坏等数越限报警
		rt_uint8_t	insulatin_module_alarm:1;  //bit 14 绝缘电阻模块故障
		rt_uint8_t  ccr_module_alarm:1;			//bit 15 调光器模块故障
		
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

	rt_uint16_t ups_input_voltage;  //7
	rt_uint16_t ups_output_voltage;
	rt_uint16_t ups_battery_voltage;
	rt_uint16_t ups_battery_capacity;
	rt_uint16_t ups_battery_duration_min;
	rt_uint16_t ups_battery_duration_s;
	rt_uint16_t ups_read_alarm_status;
	union{
		rt_uint16_t ups_alarm_status;  //bit0:com failed  bit1：power down  bit2: battery low
		struct 
		{
			rt_uint16_t ups_comm_fail:1;
			rt_uint8_t  ups_power_down:1;
			rt_uint16_t ups_battery_low:1;
			rt_uint16_t ups_reserverd:13;
		};
	};
	rt_uint16_t ups_run_stataus;  //1:ups supply  0：power supply
	rt_uint16_t up_board_com_fail_flag;

	rt_uint8_t can0_comm_fail_flag;
	rt_uint8_t can1_comm_fail_flag;

	rt_uint16_t insResistanceValue;
	rt_uint16_t insResistanceUnit;
	rt_uint16_t badLampNum;  //19
	
	rt_uint8_t  sys_com_indicator;
	rt_uint32_t seconds;
	rt_uint16_t ms;

}myPara;


typedef union
{
	struct
	{
		rt_uint8_t destMACID;
		rt_uint8_t serviceCode;
	};
	rt_uint8_t data[8];
}can_data_feild_u;


typedef struct
{
	can_data_feild_u data_feild;
	rt_device_t can_dev;
}can_user_data_s;

typedef union
{
	struct
	{
		union
		{
			struct
			{
				rt_uint8_t nodeID : 3;
				rt_uint8_t groupID : 5;
			};
			rt_uint8_t sourceMACID;
		};
		rt_uint8_t msgID : 3;
	};	
	rt_uint16_t canID;
}can_id_feild_u;

typedef struct myCAN
{
	union
	{
		struct
		{
			rt_uint8_t myCANNodeID : 3;
			rt_uint8_t myCANGroupID : 5;
		};
		rt_uint8_t myCANID;  
	};

	rt_uint16_t timer;
    struct rt_semaphore semUpdate;
	struct rt_mailbox can_sync_mb0;
	struct rt_mailbox ccr_event_mb0;
	struct rt_mailbox ups_event_mb0;
	struct rt_event  ccr_event_updata0;
	struct rt_event  ups_event_updata0;
	struct rt_mailbox can_sync_mb1;
	struct rt_mailbox ccr_event_mb1;
	struct rt_mailbox ups_event_mb1;
	struct rt_event  ccr_event_updata1;
	struct rt_event  ups_event_updata1;
	rt_uint8_t ccr_event_code;
	rt_uint8_t ccr_event_type;  //0:over 1:start
	rt_uint8_t ups_event_type; //0: over 1:start
	rt_uint8_t ups_event_code; //
	union{
		myPara para;
		rt_uint16_t mb_buffer[30];
	};
	
	void (*init)(struct myCAN*);
	void (*autoUpload)(rt_device_t, struct rt_can_msg*, struct myCAN*);
	void (*update)(rt_device_t, struct rt_can_msg*, struct myCAN*);
	
}myCAN;



void  send_standard_frame(rt_device_t dev, uint16_t msg_cmd, void *arg);
void myCANInit(myCAN* can);
void myCANAutoUpload(rt_device_t dev, struct rt_can_msg* canMsg, myCAN* pcan);
void myCANUpdate(rt_device_t dev, struct rt_can_msg* canMsg, myCAN* pcan);
void can_msg_parse(rt_device_t can_dev, rt_can_msg_t pmsg,myCAN * pcan );
void response_data_package(rt_uint8_t *data, myCAN * pcan);
#endif
