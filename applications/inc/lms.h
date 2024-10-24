/*
 * LMS.h
 *
 * Created: 2016/11/18 9:08:30
 *  Author: Administrator
 */ 
#ifndef LMS_H_
#define LMS_H_
#include "stdint.h"
typedef uint8_t BYTE;
typedef uint16_t WORD;

#define MaxLMSNumInGroupe          30                   //在一个组内配置LMS的最大个数
//TC1电流宽度
#define MaxCaptureCurWidth_ms      19.5f                //unit:ms
#define MinCaptureCurWidth_ms      9.5f                 //unit:ms

#define TStd                       0.0001f              //Unit:Second,100us
#define PowerHalfCycle_ms          (1000/(PowerFreq*2))
//
// BYTE gSampleCTC=0;
// //
// BYTE gLMSType=0;				 //gLMSType=0x00=SIU,gLMSType=0x01=LMS
//******************通信接收*******************//
//1.帧定义
//同步帧LS/SS(8bit,含3bit校验位)		: startBit(1) g2g1g0(3bit/组号) ID（1bit/1=LS,0=SS） ccc(3bit)
//命令帧LS (16bit,含4bit校验位)		：startBit(1) ConsegmentAddr(7bit),
//									  OrderCode(4bit) CRC4(4bit).
//调制器LS OrderCode(4bit)
#define  ProgData0_Order          0x00             //SCM发送数据为ProgData0编程数据
#define  ProgData1_Order          0x01             //SCM发送数据为ProgData1编程数据
#define  OpenLamp_Order			  0x02             //SCM发送数据为LMS关灯
#define  CloseLamp_Order          0x03             //SCM发送数据为LMS亮灯

#define  FlashSync_Order          0x04

#define  Intensity0_Order         0x05
#define  Intensity1_Order         0x06
#define  Intensity2_Order         0x07
#define  Intensity3_Order         0x08
#define  Intensity5_Order         0x09


#define SHORT_CMD 0X00
#define LONG_CMD 0X01

BYTE GenSyncFrame(BYTE byGroupNo,BYTE IDType);
BYTE crc4_itu(BYTE * message,BYTE length);
#endif /* LMS_H_ */