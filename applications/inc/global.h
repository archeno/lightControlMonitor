///////////////////////////////////////////////////////////////////
/// @file 		global.h
/// @brief		ȫ�ֱ���
///	@details	ȫ�ֱ������弰��ʼ��
/// @author		by DC(any question plese send 
///	@author				E-mail to 1160029144@qq.com)
/// @version	V1.0.0
/// @date 		2022-08-01
/// @copyright	Copyright (c) 2022 �����к�������ɫ�����������޹�˾
///////////////////////////////////////////////////////////////////
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <rtthread.h>
#include "mycan.h"

#define REGISTERNUM0			100
#define REGISTERNUM1			1000
#define REGISTERNUM2			300
#define MAXSEGMENTS				50
#define OBJECTDICNUM			100
#define BASEINDEX				0x2000

typedef struct SoftwareInfo
{
	uint16_t majorVersionNum;				//���汾��
	uint16_t minorVersionNum;				//�ΰ汾��
	uint16_t revisionNum;					//�޶��汾��
	uint16_t nonLabel;						//�����Ǳ��??
	uint16_t hardWareVersionNum;			//Ӳ���汾��
}SoftwareInfo;

typedef struct Comm
{
	uint16_t holdingRegist0[REGISTERNUM0];	//LCD����չ��ͨѶ��LCD��չ�����еļĴ���
	uint16_t holdingRegist1[REGISTERNUM1];	//LCD����չ��ͨѶ����չ��������豸ͨѶ�ļĴ���??
	uint16_t holdingRegist2[REGISTERNUM2];	//LCD����չ��ͨѶ����չ��������豸ͨѶ���Բ����ļĴ���??
	uint8_t	segmentsNum;					//��holding�Ĵ����ķֶ���
	uint16_t segmentTable[MAXSEGMENTS][2];	//��holding�Ĵ����ķֶ����ݣ�[][0]Ϊ�ֶ���ʼ��ַ��[][1]Ϊ�ֶεĳ���
	struct rt_messagequeue myMsg;			//��Ϣ���п��ƿ�
	uint16_t msgPool[15];					//��Ϣ�������õ��ķ�����Ϣ���ڴ��??
	uint16_t recvData[REGISTERNUM1];		//֧��д����holding�Ͷ��holding
}Comm;

typedef struct CAN
{
	uint16_t id;
	uint32_t objData[OBJECTDICNUM];			//PC����չ��ͨѶ��PC����չ��֮��Ķ����ֵ�??(��CANopen SDO��ʽͨѶ)
	rt_device_t can0_dev;            		// CAN�豸���??
	rt_device_t can1_dev;            		// CAN�豸���??

	enum CAN_MODE
	{
		CAN_READ4BYTE = 0,
		CAN_WRITE2BYTE = 2,
		CAN_WRITE4BYTE = 4
	}can_mode;
	
}CAN;

typedef struct System
{
	struct SoftwareInfo softwareInfo;		//�����汾��Ϣ
	struct Comm comm;						//RS485ͨѶ
	struct CAN can;							//CANͨѶ
	uint16_t interfaceType;					//ң�ؽӿ�����
	uint16_t CCRConfig;						//������ѡ������
	
	myCAN mycan;
	
	uint64_t recvCnt;
	uint64_t sendCnt;
}System;

extern System sys;
extern rt_uint8_t g_send_enable;


#define GROUP_ID 1 
#define NODE_ID  1


#define CCR_EVENT_CODE_POWER_DOWN  	0X02
#define CCR_EVENT_CODE_OC   	   	0X04
#define CCR_EVENT_CODE_VA_DROPDOWN 	0X06
#define CCR_EVENT_CODE_BREAK_OFF   	0X07
#define CCR_EVENT_CODE_REG_FAULT   	0X08
#define CCR_EVENT_CODE_EFD_ALRM    	0X0A
#define CCR_EVENT_CODE_LFD_ALAMR   	0X0B
#define CCR_EVENT_CODE_MODULE_FALT 	0X0C

#define EVENT_OUTPUT_CHANGED  		0x01
#define EVENT_INTPUT_CHANGED  		0X02




#define UPS_EVENT_CODE_COMM_FAIL 0X0D
#define UPS_EVENT_CODE_POWER_FAIL 0X0E
#define UPS_EVENT_CODE_BATERY_LOW 0X0F


#endif
