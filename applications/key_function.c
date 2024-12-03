#include "key_function.h"
#include "board.h"
#define DBG_TAG "key_function.c"
#include "rtdbg.h"
#include "i2c_app.h"
#include "database.h"
#include "oled_driver.h"
#include <hmi.h>
uint8_t button_id[4] = {KEY_SET, KEY_UP, KEY_DOWN, KEY_ENTER};
struct Button button[4];

menu_e g_cur_menu;

#define KEY_SET_PIN GET_PIN(C, 0)
#define KEY_UP_PIN GET_PIN(C, 1)
#define KEY_DOWN_PIN GET_PIN(C, 2)
#define KEY_ENTER_PIN GET_PIN(C, 3)

rt_uint32_t key_pin[4] = {KEY_SET_PIN, KEY_UP_PIN, KEY_DOWN_PIN, KEY_ENTER_PIN};

uint8_t read_button_GPIO(uint8_t button_id)
{
	// you can share the GPIO read function with multiple Buttons
	switch (button_id)
	{
	case KEY_SET:
		return rt_pin_read(KEY_SET_PIN);
	case KEY_UP:
		return rt_pin_read(KEY_UP_PIN);
	case KEY_DOWN:
		return rt_pin_read(KEY_DOWN_PIN);
	case KEY_ENTER:
		return rt_pin_read(KEY_ENTER_PIN);
	default:
		return 0;
	}
}

typedef void (*key_function)(void);

static void inline page_down(void)
{
	if (g_cur_menu < MENU_NUM - 2)
	{
		g_cur_menu++;
	}
	else
	{
		g_cur_menu = MENU_HOME;
	}
}

static void inline page_up(void)
{
	if (g_cur_menu > 0)
	{
		g_cur_menu--;
	}
	else
	{
		g_cur_menu = MENU_NUM - 2;
	}
}

static void inline page_home(void)
{
	g_cur_menu = 0;
}

static void inline page_settings(void)
{
	g_cur_menu = MENU_SETTING;
}
void menu_home_key_set_func(void)
{
	page_settings();
}

void menu_home_key_up_func(void)
{
	page_up();
}

void menu_home_key_down_func(void)
{
	page_down();
}
void menu_home_key_enter_func(void)
{
	page_home();
}

void menu_alarm_key_up_func(void)
{
	page_up();
}

void menu_alarm_key_down_func(void)
{
	page_down();
}
void menu_alarm_key_enter_func(void)
{

	g_hmi_data.show_result_flag = 1;
}

void menu_alarm_key_set_func(void)
{
	// page_settings();
}

void menu_io_key_set_func(void)
{
}

void menu_io_key_up_func(void)
{
	page_up();
}

void menu_io_key_down_func(void)
{
	page_down();
}
void menu_io_key_enter_func(void)
{

	g_hmi_data.show_result_flag = 1;
}

void menu_ups_key_set_func(void)
{
}

void menu_ups_key_up_func(void)
{
	page_up();
}

void menu_ups_key_down_func(void)
{
	page_down();
}
void menu_ups_key_enter_func(void)
{

	g_hmi_data.show_result_flag = 1;
}

void menu_setting_key_set_func(void)
{
	g_hmi_data.write_eeprom_flag = 1;
}

void menu_setting_key_up_func(void)
{
	page_up();
	g_hmi_data.reset_factory_flag = 0;
}

void menu_setting_down_func(void)
{
	page_down();
	g_hmi_data.reset_factory_flag = 0;
}
void menu_setting_key_enter_func(void)
{
	g_hmi_data.reset_factory_flag = 0;
	page_home();
	//    g_ccr_data.ccr_info.Un_a = 869.9f,
	//    g_ccr_data.ccr_info.Un_b =-11.5f,
	//    write_sysdata_to_eeprom(EEPROM_WRITE_DATABASE_VALUE);
}

key_function key_func_table[MENU_NUM][4] = {

	{page_settings, page_up, page_down, page_home},
	{page_settings, page_up, page_down, menu_home_key_enter_func},
	{menu_ups_key_set_func, page_up, page_down, menu_ups_key_enter_func},
	{menu_io_key_set_func, page_up, page_down, menu_io_key_enter_func},
	// 	{menu_com_key_set_func, menu_com_key_up_func, menu_com_key_down_func, menu_com_key_enter_func},
	{menu_setting_key_set_func, menu_setting_key_up_func, menu_setting_down_func, menu_setting_key_enter_func},
};

static void key_set_cb(void *param)
{
	key_func_table[g_cur_menu][KEY_SET]();
}
static void key_up_cb(void *param)
{
	key_func_table[g_cur_menu][KEY_UP]();
}
static void key_down_cb(void *param)
{
	key_func_table[g_cur_menu][KEY_DOWN]();
}
static void key_enter_cb(void *param)
{
	key_func_table[g_cur_menu][KEY_ENTER]();
}

BtnCallback btn_cb[4] = {
	key_set_cb, key_up_cb, key_down_cb, key_enter_cb};

static rt_err_t key_poll_timer_cb(rt_device_t dev, rt_size_t size)
{
	button_ticks();
}
// 按键轮询 5ms定时器
static void key_poll_timer_init()
{
	rt_hwtimerval_t timeout_s; /* 定时器超时值 */
	rt_hwtimer_mode_t mode = HWTIMER_MODE_PERIOD;
	rt_device_t key_timer = rt_device_find("timer0");

	if (RT_NULL == key_timer)
	{
		LOG_E("cannot find timer0");
		return;
	}
	rt_err_t result;
	result = rt_device_open(key_timer, RT_DEVICE_FLAG_RDWR);
	if (result != RT_EOK)
	{
		LOG_E(" open key_timer failed!");
	}

	rt_device_set_rx_indicate(key_timer, key_poll_timer_cb);

	timeout_s.sec = 0;
	timeout_s.usec = 5000;

	rt_device_control(key_timer, HWTIMER_CTRL_MODE_SET, &mode);
	rt_device_write(key_timer, 0, &timeout_s, sizeof(timeout_s));
	// NVIC_SetPriority(TIMER6_IRQn, 6);
}

static int buttons_init(void)
{
	for (int i = 0; i < 4; i++)
	{
		rt_pin_mode(key_pin[i], PIN_MODE_INPUT_PULLUP);
		button_init(&button[i], read_button_GPIO, 0, i);
		button_attach(&button[i], SINGLE_CLICK, btn_cb[i]);
		button_start(&button[i]);
	}

	key_poll_timer_init();
	return 0;
}

INIT_DEVICE_EXPORT(buttons_init);