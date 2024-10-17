#ifndef __CAN_RESPONSE_H__
#define __CAN_RESPONSE_H__
#include "data_transfrom.h"

#define  CMD_SYNC_TIME                  0X0101          //同步时间
#define  CMD_CTRL_LIGHT_LVL             0X02FF          //控制光级
#define  CMD_SYNC                       0X0401         //同步信号
#define  CMD_PACK_CCR_ALARM_EVENT       0X0301          //调光器报警事件
#define  CMD_PACK_CCR_ALARM_STATUS      0X0302          //调光器报警状态

#define  CMD_PACK_UPS_ALARM_EVENT       0X0330          //UPS报警事件
#define  CMD_PACK_UPS_ALARM_STATUS      0X0331          //UPS报警状态

#define  CMD_PACK_CCR_IO                0X0402          //调光器开关量

#define CMD_PACK_UPS_IO                 0X0430          //UPS状态信息
#define CMD_PACK_UPS_ANALOG             0X0530          //UPS模拟量


#define  CMD_PACK_CCR_V_INPUT           0X0501          //调光器输入电压
#define  CMD_PACK_CCR_I_INPUT           0X0502          //调光器输入电流
#define  CMD_PACK_CCR_LOOP_V_I_INFO     0X0503          //回路电流电压

#define  CMD_PACK_CCR_V_OUTPUT          0X0504          //调光器输出电压
#define  CMD_PACK_CCR_I_OUTPUT          0X0505          //调光器输出电流
#define  CMD_PACK_CCR_F_OUTPUT          0X0506          //调光器输出频率
#define  CMD_PACK_CCR_INSULATION        0X0507          //回路绝缘值
#define  CMD_PACK_CCR_LIGHT_FAIL_NUM    0X0508          //回路坏灯

void can_msg_response_data_pack(rt_uint16_t msg_cmd, void *arg,
                                dataStream_box_t *data, rt_int8_t *len);

#endif