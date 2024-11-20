#ifndef __KEY_FUNCTION_H__
#define __KEY_FUNCTION_H__

#include "multi_button.h"

typedef enum
{
	MENU_HOME = 0,
	MENU_UPS,
	MENU_IO,
	MENU_SETTING,
	MENU_NUM,
} menu_e;

extern menu_e g_cur_menu;

enum
{
	KEY_SET = 0,
	KEY_UP,
	KEY_DOWN,
	KEY_ENTER,
};

#endif
