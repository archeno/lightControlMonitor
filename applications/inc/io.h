#ifndef __IO_H__
#define __IO_H__

#include "board.h"

// #define O0_SET_PIN GET_PIN(B, 4)
// #define O0_RESET_PIN GET_PIN(B, 5)
// #define O0_IN_PIN GET_PIN(C, 8)

// #define O1_SET_PIN GET_PIN(D, 2)
// #define O1_RESET_PIN GET_PIN(A, 2)
// #define O1_IN_PIN GET_PIN(C, 7)

// #define O2_SET_PIN GET_PIN(A, 15)
// #define O2_RESET_PIN GET_PIN(C, 12)
// #define O2_IN_PIN GET_PIN(C, 6)

// #define O3_SET_PIN GET_PIN(C, 9)
// #define O3_RESET_PIN GET_PIN(A, 8)
// #define O3_IN_PIN GET_PIN(B, 15)

#define O4_SET_PIN GET_PIN(C, 8)
#define O4_RESET_PIN GET_PIN(C, 7)
// #define O4_IN_PIN GET_PIN(B, 14)

#define O5_SET_PIN GET_PIN(A, 12)
#define O5_RESET_PIN GET_PIN(A, 11)
// #define O5_IN_PIN		GET_PIN(B, 11)

#define O6_SET_PIN GET_PIN(B, 5)
#define O6_RESET_PIN GET_PIN(B, 4)
// #define O6_IN_PIN		GET_PIN(B, 10)

// #define O7_SET_PIN GET_PIN(A, 4)
#define O7_RESET_PIN GET_PIN(D, 2)
#define O7_IN_PIN GET_PIN(C, 12)

extern rt_uint8_t g_pin_release_cnt;
#define RELAY_ON_TICK (RT_TICK_PER_SECOND / 100) * 2 // 20ms

#define CCR_OFF()                            \
	do                                       \
	{                                        \
		rt_pin_write(O0_RESET_PIN, PIN_LOW); \
		rt_pin_write(O1_RESET_PIN, PIN_LOW); \
		rt_pin_write(O2_RESET_PIN, PIN_LOW); \
		rt_pin_write(O3_RESET_PIN, PIN_LOW); \
		rt_pin_write(O4_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)

#define ALL_ON()                           \
	do                                     \
	{                                      \
		rt_pin_write(O0_SET_PIN, PIN_LOW); \
		rt_pin_write(O1_SET_PIN, PIN_LOW); \
		rt_pin_write(O2_SET_PIN, PIN_LOW); \
		rt_pin_write(O3_SET_PIN, PIN_LOW); \
		rt_pin_write(O4_SET_PIN, PIN_LOW); \
		rt_pin_write(O5_SET_PIN, PIN_LOW); \
		rt_pin_write(O6_SET_PIN, PIN_LOW); \
		rt_pin_write(O7_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)

#define ALL_OFF()                            \
	do                                       \
	{                                        \
		rt_pin_write(O0_RESET_PIN, PIN_LOW); \
		rt_pin_write(O1_RESET_PIN, PIN_LOW); \
		rt_pin_write(O2_RESET_PIN, PIN_LOW); \
		rt_pin_write(O3_RESET_PIN, PIN_LOW); \
		rt_pin_write(O4_RESET_PIN, PIN_LOW); \
		rt_pin_write(O5_RESET_PIN, PIN_LOW); \
		rt_pin_write(O6_RESET_PIN, PIN_LOW); \
		rt_pin_write(O7_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)

#define CCR_G1()                             \
	do                                       \
	{                                        \
		rt_pin_write(O0_SET_PIN, PIN_LOW);   \
		rt_pin_write(O1_RESET_PIN, PIN_LOW); \
		rt_pin_write(O2_RESET_PIN, PIN_LOW); \
		rt_pin_write(O3_RESET_PIN, PIN_LOW); \
		rt_pin_write(O4_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define CCR_G2()                             \
	do                                       \
	{                                        \
		rt_pin_write(O1_SET_PIN, PIN_LOW);   \
		rt_pin_write(O0_RESET_PIN, PIN_LOW); \
		rt_pin_write(O2_RESET_PIN, PIN_LOW); \
		rt_pin_write(O3_RESET_PIN, PIN_LOW); \
		rt_pin_write(O4_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define CCR_G3()                             \
	do                                       \
	{                                        \
		rt_pin_write(O2_SET_PIN, PIN_LOW);   \
		rt_pin_write(O0_RESET_PIN, PIN_LOW); \
		rt_pin_write(O1_RESET_PIN, PIN_LOW); \
		rt_pin_write(O3_RESET_PIN, PIN_LOW); \
		rt_pin_write(O4_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define CCR_G4()                             \
	do                                       \
	{                                        \
		rt_pin_write(O3_SET_PIN, PIN_LOW);   \
		rt_pin_write(O1_RESET_PIN, PIN_LOW); \
		rt_pin_write(O2_RESET_PIN, PIN_LOW); \
		rt_pin_write(O0_RESET_PIN, PIN_LOW); \
		rt_pin_write(O4_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define CCR_G5()                             \
	do                                       \
	{                                        \
		rt_pin_write(O4_SET_PIN, PIN_LOW);   \
		rt_pin_write(O1_RESET_PIN, PIN_LOW); \
		rt_pin_write(O2_RESET_PIN, PIN_LOW); \
		rt_pin_write(O3_RESET_PIN, PIN_LOW); \
		rt_pin_write(O0_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)

#define DO_0_ON()                          \
	do                                     \
	{                                      \
		rt_pin_write(O0_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)
#define DO_0_OFF()                           \
	do                                       \
	{                                        \
		rt_pin_write(O0_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define DO_1_ON()                          \
	do                                     \
	{                                      \
		rt_pin_write(O1_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)
#define DO_1_OFF()                           \
	do                                       \
	{                                        \
		rt_pin_write(O1_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define DO_2_ON()                          \
	do                                     \
	{                                      \
		rt_pin_write(O2_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)
#define DO_2_OFF()                           \
	do                                       \
	{                                        \
		rt_pin_write(O2_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define DO_3_ON()                          \
	do                                     \
	{                                      \
		rt_pin_write(O3_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)
#define DO_3_OFF()                           \
	do                                       \
	{                                        \
		rt_pin_write(O3_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define DO_4_ON()                          \
	do                                     \
	{                                      \
		rt_pin_write(O4_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)
#define DO_4_OFF()                           \
	do                                       \
	{                                        \
		rt_pin_write(O4_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
// #define DO_5_ON()   do{ rt_pin_write(O5_SET_PIN, PIN_LOW);\
// 					g_pin_release_cnt = RELAY_ON_TICK;}\
// 					while(0)
// #define DO_5_OFF()  do{ rt_pin_write(O5_RESET_PIN, PIN_LOW);\
// 					g_pin_release_cnt = RELAY_ON_TICK;}\
// 					while(0)
#define DO_6_ON()                          \
	do                                     \
	{                                      \
		rt_pin_write(O6_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)
#define DO_6_OFF()                           \
	do                                       \
	{                                        \
		rt_pin_write(O6_RESET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK;   \
	} while (0)
#define DIO_7()                            \
	do                                     \
	{                                      \
		rt_pin_write(O7_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)
#define DIO_7()                            \
	do                                     \
	{                                      \
		rt_pin_write(O7_SET_PIN, PIN_LOW); \
		g_pin_release_cnt = RELAY_ON_TICK; \
	} while (0)

void release_rleay_ctrl_pin(void);

#endif
