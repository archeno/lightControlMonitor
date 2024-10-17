#include <event_monitor.h>
#include <global.h>

#define DBG_TAG "event_monitor"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
rt_uint8_t g_alarm_event_cnt;
/**
 * @brief 
 * 
 * @param alarm_status 
 * @param alarm_old_status 
 * @param event_code_tab 
 * @param event_num 
 * @return rt_uint_t if alarm changed then return 1 else return 0
 */
rt_uint8_t  alarm_event_monitor(rt_uint16_t alarm_status, rt_uint16_t *alarm_old_status, const rt_uint16_t * event_code_tab, rt_uint8_t event_num)
{
	rt_uint8_t change_flag = 0;
	rt_uint16_t eventCode;
	rt_uint16_t xor_reg;
	rt_mailbox_t pMailBox;
	if (*alarm_old_status != alarm_status)
	{
		g_alarm_event_cnt++;
		xor_reg = *alarm_old_status ^ alarm_status;
		for (int i = 0; i < event_num; i++)
		{
			// get event code
			if (xor_reg & (1 << i))
			{
				change_flag = 1;
				if(event_code_tab[i] !=0)
				{
					eventCode = (alarm_status & (1 << i)) ? (event_code_tab[i] | (0x8000)) : event_code_tab[i];
					// send mail
					if (!sys.mycan.para.can0_comm_fail_flag)
					{
						LOG_D("send mb0 event code:0x%04X ", eventCode);
					//	rt_mb_send(&sys.mycan.ccr_event_mb0, eventCode);
						
					}
					else if (!sys.mycan.para.can1_comm_fail_flag)
					{
						LOG_D("send mb1 event code: 0x%04X ", eventCode);
					//	rt_mb_send(&sys.mycan.ccr_event_mb1, eventCode);
					}
				}
			}

		
		}
		//update old alarm status
		*alarm_old_status = alarm_status;
	}
	
	return change_flag;
}