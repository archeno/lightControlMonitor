#include "hmi.h"

#include "database.h"
#include "key_function.h"
#include <oled_driver.h>

hmi_state_t g_hmi_data;

extern const char *ccr_alarm_define_str[];

// rt_uint8_t num_table[][16] = {

//     {0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00}, /*"0",0*/

//     {0x00, 0x00, 0x00, 0x10, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00}, /*"1",1*/

//     {0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x04, 0x04, 0x08, 0x10, 0x20, 0x42, 0x7E, 0x00, 0x00}, /*"2",2*/

//     {0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x04, 0x18, 0x04, 0x02, 0x02, 0x42, 0x44, 0x38, 0x00, 0x00}, /*"3",3*/

//     {0x00, 0x00, 0x00, 0x04, 0x0C, 0x14, 0x24, 0x24, 0x44, 0x44, 0x7E, 0x04, 0x04, 0x1E, 0x00, 0x00}, /*"4",4*/

//     {0x00, 0x00, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x58, 0x64, 0x02, 0x02, 0x42, 0x44, 0x38, 0x00, 0x00}, /*"5",5*/

//     {0x00, 0x00, 0x00, 0x1C, 0x24, 0x40, 0x40, 0x58, 0x64, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00}, /*"6",6*/

//     {0x00, 0x00, 0x00, 0x7E, 0x44, 0x44, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00}, /*"7",7*/

//     {0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x24, 0x18, 0x24, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00}, /*"8",8*/

//     {0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x26, 0x1A, 0x02, 0x02, 0x24, 0x38, 0x00, 0x00}, /*"9",9*/

//     {0x00, 0x00, 0x00, 0x10, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00}, /*"1",1*/

//     {0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x04, 0x04, 0x08, 0x10, 0x20, 0x42, 0x7E, 0x00, 0x00}, /*"2",2*/

//     {0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x04, 0x18, 0x04, 0x02, 0x02, 0x42, 0x44, 0x38, 0x00, 0x00}, /*"3",3*/

//     {0x00, 0x00, 0x00, 0x04, 0x0C, 0x14, 0x24, 0x24, 0x44, 0x44, 0x7E, 0x04, 0x04, 0x1E, 0x00, 0x00}, /*"4",4*/

//     {0x00, 0x00, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x58, 0x64, 0x02, 0x02, 0x42, 0x44, 0x38, 0x00, 0x00}, /*"5",5*/

// };

/******** ????????????? ********/
// void INTER_chhzes(rt_uint8_t x,rt_uint8_t y,rt_uint8_t m,rt_uint8_t n,rt_uint16_t k,rt_uint8_t rev,rt_uint8_t *s)
//{
//   rt_uint8_t i;
//   oled_cmd_send(CMD_LOCAL_CHAR_DISP);
//   oled_data_send(x);  //??????
//   oled_data_send(y);  //?????
//   oled_data_send(m);  //????????????
//   oled_data_send(n);  //???????????
//   oled_data_send(k);  //???????
//   oled_data_send(rev);  //rev=0:?????????rev=1:???????

//  while(*s!='\0')
//    oled_data_send(*s++);
//

//}//*/
///******** ??????????********/
// void chhzes(rt_uint8_t x,rt_uint8_t y,rt_uint8_t m,rt_uint8_t n,rt_uint8_t
// k,rt_uint8_t rev,rt_uint8_t *s)
//{
//	rt_uint8_t i;
//	rt_uint32_t  j;
//	oled_data_send(0x5A);
//	oled_data_send(x); //??????
//	oled_data_send(y); //?????
//	oled_data_send(m); //????????????
//	oled_data_send(n); //???????????
//	oled_data_send(k); //???????
//	oled_data_send(rev); //rev=0:?????????rev=1:???????
//	for(i=0;i<k;i++)
//	for(j=0;j<(n+7)/8*m;j++)
//	oled_data_send(*s++);
// }

/******** ????????????? ********/

typedef void (*disp_func)(void);
rt_uint8_t disp_inver_flag = 0;
void home_disp(void)
{

  char disp_buf[240];
  static rt_uint8_t cnt = 0;
  rt_snprintf(disp_buf, 240, "Relay:%s R/L:%s  光级:%d", g_ccr_data.ccr_info.ccr_relay_state ? "闭合" : "断开", g_ccr_data.ccr_info.ccr_local_remote ? "远程" : "本地", g_ccr_data.ccr_info.real_idensity);

  oled_displayMix(0, 0, disp_buf);

  rt_snprintf(disp_buf, 240, "%d.%02dA ", g_ccr_data.ccr_info.local_i_load / 100,
              g_ccr_data.ccr_info.local_i_load % 100);
  oled_displayChar(16, 3, 1, disp_buf);

  rt_snprintf(disp_buf, 240, "%d.%dV   ", g_ccr_data.ccr_info.local_v_load / 10,
              g_ccr_data.ccr_info.local_v_load % 10);

  oled_displayChar(16, 3, 8, disp_buf);
  // ups:%c inter:%c
  rt_snprintf(disp_buf, 240, "Can0:%s Can1:%s 485:%s", g_ccr_data.ccr_info.can0_comm_fail_flag ? "异常" : "正常",
              g_ccr_data.ccr_info.can1_comm_fail_flag ? "异常" : "正常",
              g_ccr_data.ccr_info.ups_comm_fail ? "异常" : "正常");
  // g_ccr_data.ccr_info.down_board_com_fail_flag?"x":"√");
  oled_displayMix(6, 0, disp_buf);
}

// static void alarm_disp()
// {
//   // ccr_alarm_t ccr_alarm = {0};
//   oled_displayMix(0, 2, "当前报警列表");
//   rt_uint8_t current_line = 0;
//   ccr_get_alarm_str(&ccr_alarm, g_ccr_data.ccr_info.INPUT_P0, ccr_alarm_define_str);
//   if (ccr_alarm.index > 0)
//   {
//     current_line++;
//     for (int i = 0; i < ccr_alarm.index; i++)
//     {
//       oled_displayMix(++current_line, 0, ccr_alarm.pstr[i]);
//     }
//     while (current_line < 7)
//     {
//       oled_clear_line(++current_line);
//     }
//   }
//   else
//   {
//     current_line++;
//     while (current_line < 7)
//     {
//       oled_clear_line(++current_line);
//     }
//   }
// }
static void io_disp()
{

  oled_displayMix(0, 3, "IO状态");

  char disp_buf[240];
  rt_snprintf(disp_buf, 240, "输入P0:    0X%02X", g_ccr_data.ccr_info.INPUT_P0);
  oled_displayMix(3, 0, disp_buf);

  rt_snprintf(disp_buf, 240, "输入P1:    0X%02X", g_ccr_data.ccr_info.INPUT_P1);
  oled_displayMix(4, 0, disp_buf);

  rt_snprintf(disp_buf, 240, "输出P0:    0X%02X", g_ccr_data.ccr_info.OUTPUT_P0);
  oled_displayMix(5, 0, disp_buf);
}

static void ups_disp()
{

  oled_displayMix(0, 2, "UPS运行参数");
  char disp_buf[240];
  rt_snprintf(disp_buf, 240, "输入电压:    %d.%dV", g_ccr_data.ccr_info.ups_input_voltage / 10,
              g_ccr_data.ccr_info.ups_input_voltage % 10);
  oled_displayMix(2, 0, disp_buf);
  // 输出电压
  rt_snprintf(disp_buf, 240, "输出电压:    %d.%dV", g_ccr_data.ccr_info.ups_output_voltage / 10,
              g_ccr_data.ccr_info.ups_output_voltage % 10);
  oled_displayMix(3, 0, disp_buf);
  // 电池电压
  rt_snprintf(disp_buf, 240, "电池电压:    %d.%dV", g_ccr_data.ccr_info.ups_battery_voltage / 10,
              g_ccr_data.ccr_info.ups_battery_voltage % 10);
  oled_displayMix(4, 0, disp_buf);
  // 电池容量
  rt_snprintf(disp_buf, 240, "电池容量:    %d%%", g_ccr_data.ccr_info.ups_battery_capacity);
  oled_displayMix(5, 0, disp_buf);
  // 电池持续时间
  rt_snprintf(disp_buf, 240, "电池持续时间: %d分钟", g_ccr_data.ccr_info.ups_battery_duration_min);
  oled_displayMix(6, 0, disp_buf);
  // 运行状态
  rt_snprintf(disp_buf, 240, "供电状态:   %s供电 ", g_ccr_data.ccr_info.ups_run_stataus ? "UPS" : "市电");
  oled_displayMix(7, 0, disp_buf);
}

// static void calparam_disp(void)
// {
//     ccr_alarm_t ccr_alarm;
//     oled_displayMix(0, 2, "ADC 校准参数");
//     char disp_buf[240];

//     //光级
//     rt_snprintf(disp_buf, 240, "当前光级:    %d", g_ccr_data.ccr_info.local_real_idensity);
//     oled_displayMix(2, 0, disp_buf);

//     //电流采样值
//     rt_snprintf(disp_buf, 240, "电流采样有效值:   %d.%d A", (uint32_t)(g_ccr_data.ccr_info.local_i_load*1000)/1000,
//                                                   (uint32_t)(g_ccr_data.ccr_info.local_i_load*1000)%1000);
//     oled_displayMix(3, 0, disp_buf);
//     //IN_a2
//     rt_snprintf(disp_buf, 240, "校准系数In_a2:   %d.%d V", (uint32_t)(g_ccr_data.ccr_info.local_v_load*1000)/1000,
//                                                   (uint32_t)(g_ccr_data.ccr_info.local_v_load*1000)%1000);
//     oled_displayMix(4, 0, disp_buf);
//     //In_a1
//     rt_snprintf(disp_buf, 240, "校准系数In_a2:   %d.%d V", (uint32_t)(g_ccr_data.ccr_info.local_v_load*1000)/1000,
//                                                   (uint32_t)(g_ccr_data.ccr_info.local_v_load*1000)%1000);
//     oled_displayMix(4, 0, disp_buf);

// }

rt_uint32_t g_tick_start;
rt_uint32_t g_tick_end;
rt_uint32_t tick_eplase;

void settings_disp(void)
{
  oled_displayGBK_16(0, 3, "恢复校准默认参数");
  if (g_hmi_data.reset_factory_flag)
  {
    oled_displayGBK_16(2, 4, "恢复成功");
  }
}
disp_func display_table[MENU_NUM] = {home_disp, ups_disp, io_disp, settings_disp};

void hmi_test(void *param)
{
  static menu_e last_menu = MENU_NUM;
  oled_init();
  while (1)
  {
    if (g_cur_menu != last_menu)
    {
      last_menu = g_cur_menu;
      oled_clear();
      g_hmi_data.show_result_flag = 0;
    }
    display_table[g_cur_menu]();
    // oled_displayChar(0, 0, "U1: ");
    // oled_displayVoltageAppend(g_ccr_data.ccr_info.v_scr);
    // oled_displayChar(1, 0, "I1: ");
    // oled_displayCurrentAppend(g_ccr_data.ccr_info.i_scr);
    // oled_displayChar(2, 0, "U2: ");
    // oled_displayVoltageAppend(g_ccr_data.ccr_info.v_load);
    // oled_displayChar(3, 0, "I2: ");
    // oled_displayCurrentAppend(g_ccr_data.ccr_info.i_load);
    //  oled_cmd_send(CMD_CLR);
    rt_thread_mdelay(200);
    /**/
  }
}

static int hmi_thread(void)
{
  rt_thread_t thread = RT_NULL;

  thread = rt_thread_create("hmi_thread", hmi_test, RT_NULL, 1024, 2, 8);
  if (thread != RT_NULL)
  {
    rt_thread_startup(thread);
  }
  else
  {
    return -RT_ERROR;
  }

  return RT_EOK;
}
INIT_APP_EXPORT(hmi_thread);
