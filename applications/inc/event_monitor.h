#ifndef __EVENT_MONITOR_H__
#define __EVENT_MONITOR_H__

#include <rtdef.h>
#include <rtthread.h>
rt_uint8_t   alarm_event_monitor(rt_uint16_t alarm_status, rt_uint16_t *alarm_old_status, 
                            const rt_uint16_t * event_code_tab, rt_uint8_t event_num);


#endif