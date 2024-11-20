#ifndef __OLED_DRIVER_H__
#define __OLED_DRIVER_H__

#include <rtdef.h>
#define OLED_X_POITNS 240
#define OLED_Y_POITNS 128

typedef struct{
	uint8_t x_pos;
	uint8_t y_pos;	
}oled_pos_t;


void oled_init();
void oled_clear(void);
void oled_clear_line(rt_uint8_t line);
void oled_displayMix(rt_uint8_t y,rt_uint8_t x, const rt_uint8_t *s);
void oled_displayChar(rt_uint8_t char_with,rt_uint8_t y, rt_uint8_t x, const rt_uint8_t *s);
void oled_displayGBK_16(rt_uint8_t y, rt_uint8_t x, rt_uint8_t *s);


void draw_circle(uint8_t x,uint8_t y,uint8_t r,uint8_t
fil,uint8_t z);

#endif