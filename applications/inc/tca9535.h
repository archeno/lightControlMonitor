#ifndef __TCA9535_H__
#define __TCA9535_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "tca9535.h"
#include "global.h"
#define TCA9535_I2C_BUS_NAME          "i2c1"  /* ���������ӵ�I2C�����豸���� */
#define TCA9535_INPUT_IO_ADDR                  0x20    /* �ӻ���ַ */
#define TCA9535_LED_OUT1_ADDR             0X21
#define TCA9535_LED_OUT2_ADDR             0X22

#define TCA9535_PORT0_INPUT_CMD       	  0x01    /* PORT0 �˿� ����*/
#define TCA9535_PORT1_INPUT_CMD       	  0x00    /* PORT1 �˿� ����*/

#define TCA9535_PORT0_OUTPUT_CMD       	  0x02    /* PORT0 �˿� ���*/
#define TCA9535_PORT1_OUTPUT_CMD       	  0x03    /* PORT1 �˿� ���*/

#define TCA9535_PORT0_POLR_CMD       	  0x04    /* PORT0 �˿� ���Է�ת*/
#define TCA9535_PORT1_POLR_CMD       	  0x05    /* PORT1 �˿� ���Է�ת*/

#define TCA9535_PORT0_CFG_CMD       	  0x06    /* PORT0 �˿�  ���� */
#define TCA9535_PORT1_CFG_CMD       	  0x07    /* PORT1 �˿�  ����*/

#define TCA9535_PORT1_CMD       	  0x01    /* PORT1 �˿� */

//#define TCA9535_NORMAL_CMD            0xA8    /* һ������ */
//#define TCA9535_GET_DATA              0xAC    /* ��ȡ�������� */
typedef enum  BITS_NUM
{
	BIT_0 =0,
	BIT_1,
	BIT_2,
	BIT_3,
	BIT_4,
	BIT_5,
	BIT_6,
	BIT_7
}E_BIT_NUM;
typedef enum {
	TCA9535_PORT0=0,
	TCA9535_PORT1
}TCA9535_PORT_ENUM;

int TCA9535_init(void);

 rt_uint8_t  inverse_byte(rt_uint8_t byte);
rt_int8_t TCA9535_Write_Port(rt_uint16_t addr, TCA9535_PORT_ENUM port_n, rt_uint8_t data);
rt_int8_t TCA9535_Read_Port(rt_uint16_t addr , TCA9535_PORT_ENUM port_n);
rt_int8_t TCA9535_Inversion_PortBits(rt_uint16_t addr, TCA9535_PORT_ENUM port_n, rt_uint8_t inver_bit);

#endif
