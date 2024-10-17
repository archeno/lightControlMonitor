#include "global.h"
//#include <fal.h>
//#include <easyflash.h>

#define DBG_TAG "global"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

System sys;
rt_uint8_t g_send_enable = 0;
static void msgInit(void)
{
	rt_err_t result;
	rt_memset(sys.comm.msgPool, 0, sizeof(sys.comm.msgPool));
    result = rt_mq_init(&sys.comm.myMsg,
                        "myMsg",
                        sys.comm.msgPool,					/* �ڴ��ָ��msg_pool */
                        6,								/* ÿ����Ϣ�Ĵ�С */
                        sizeof(sys.comm.msgPool),			/* �ڴ�صĴ�С��msg_pool�Ĵ�С */
                        RT_IPC_FLAG_FIFO);				/* ����ж���̵߳ȴ������������ȵõ��ķ���������Ϣ */
						
	if (result != RT_EOK)
    {
        rt_kprintf("init message queue failed.\n");
        return;
    }
}

static void canInit(void)
{
	
	if((sys.can.id < 2) || (sys.can.id > 128))
	{
		sys.can.id = 2;
	}
	sys.comm.holdingRegist0[5] = sys.can.id;
	
	myCANInit(&sys.mycan);
}

static void segmentsInit(void) 
{
	sys.comm.segmentsNum = 0;
	sys.mycan.para.sys_com_indicator = 0xff; //led off
	rt_memset(sys.comm.segmentTable, 0, sizeof(sys.comm.segmentTable));
}

static void clearPara(int argc, char**argv)
{
	sys.comm.segmentsNum = 0;
	rt_memset(sys.comm.segmentTable, 0, sizeof(sys.comm.segmentTable));
}
MSH_CMD_EXPORT(clearPara, parameter reset to factory);

static void printPara(int argc, char**argv)
{
	int i = 0;
	LOG_I("can0_com_status: %d", !sys.mycan.para.can0_comm_fail_flag);
	LOG_I("can1_com_status: %d", !sys.mycan.para.can1_comm_fail_flag);
	LOG_I("ups_com_status: %d", !sys.mycan.para.ups_comm_fail);
	LOG_I("upboard_com_status: %d", !sys.mycan.para.up_board_com_fail_flag);

	LOG_I("INPUT0: 0X%02X",  sys.mycan.para.INPUT_P0);
	LOG_I("INPUT1: 0X%02X",  sys.mycan.para.INPUT_P1);
	LOG_I("OUTPUT: 0X%02X",  sys.mycan.para.OUTPUT_P0);
	LOG_I("cmd_idensity: %d",  sys.mycan.para.cmd_idensity);
	LOG_I("real_idensity: %d",  sys.mycan.para.real_idensity);
	LOG_I("canID: %d (groupId:%d nodeId:%d)",  sys.mycan.myCANGroupID<<3| sys.mycan.myCANNodeID,
								sys.mycan.myCANGroupID, sys.mycan.myCANNodeID);
}

MSH_CMD_EXPORT(printPara, print key parameters);



static int paraInit(void)
{
	//fal_init();
	//easyflash_init();
	msgInit();
	
	sys.softwareInfo.majorVersionNum = 1;
	sys.softwareInfo.minorVersionNum = 0;
	sys.softwareInfo.revisionNum = 0;
	sys.softwareInfo.nonLabel = 0;
	sys.softwareInfo.hardWareVersionNum = 1;
	
	rt_memset(sys.comm.holdingRegist0, 0, sizeof(sys.comm.holdingRegist0));
	rt_memcpy(sys.comm.holdingRegist0, &sys.softwareInfo, sizeof(sys.softwareInfo));
	
	rt_memset(sys.comm.holdingRegist1, 0, sizeof(sys.comm.holdingRegist1));
	rt_memset(sys.comm.holdingRegist2, 0, sizeof(sys.comm.holdingRegist2));
	
	rt_memset(sys.comm.recvData, 0, sizeof(sys.comm.recvData));
	rt_memset(sys.can.objData, 0, sizeof(sys.can.objData));

	canInit();
	segmentsInit();
	
	return RT_EOK;
}

INIT_PREV_EXPORT(paraInit);




