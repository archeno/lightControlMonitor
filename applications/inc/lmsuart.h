#ifndef __LMSUART_H__
#define __LMSUART_H__

#include "stdint.h"
#include "gd32f4xx.h"
typedef enum
{
    STATE_TX_SS, // ��⵽�����źţ���ʱ2.5ms���뷢��̬���������������û������ͬ�����
    STATE_TX_LS, // ���ͳ�����
} Send_State_E;
typedef enum
{
    STATE_INIT,    // ��ʼ�׶�,�ȴ�ͬ���źŽ��뷢��̬
    STATE_TX,      // ����״̬������������ʱ��ʱ�������巢�͹����ڷ�����ʱ��ʱ���ص���ִ��
    STATE_WAITRCV, // �ȴ�3�������źŶ�ȡ��һ��
    STATE_RECVING, // ����״̬
} LMS_STATE_E;

#define TERMIANL_NUM 32          // �ն�����
#define WAIT_CROSS_CNT 2         // ���ͺ󾭹������������壬�������������忪ʼ��������
#define TX_DELAY_US (2.5 * 1000) // 2.5US

typedef struct
{
    uint8_t group_id;               // �������
    uint8_t seg_id;                 // ���ƶκ�
    uint8_t write_lock;             // ���յ������д��=1��������ʱ=1����ֹ�޸���Ϣ��������ɺ����=0
    uint8_t cmd_data[3];            // �����͵�����
    uint8_t cmd_toSend;             // �����͵�����ص�/���ơ�
    uint8_t cmd_current_send_index; // ��ǰ���͵��ֽ�������0~2��

} cmd_t;
typedef struct
{
    LMS_STATE_E state;
    Send_State_E send_state;
    cmd_t cmd_info;
    uint8_t waitToRecvicntLeft;          // �ȴ����չ������ //Ĭ��Ϊ3
    uint8_t terminal_index;              // ���ڽ��յ��ն�����0~31(32��)
    uint8_t terminalState[TERMIANL_NUM]; // ff:��Ӧ�� 0x00: ������/SIU�Ѽ��������� 0x0b:�ƹ���/�Ѽ����Դ����
                                         //  0x74: δ���� 0x7f: ��⵽�ɻ�
    uint8_t polling_complete_flag;       // Ѳ����ɱ�־
} lms_t;
extern lms_t g_lms;

extern struct rt_mailbox mb;
void lms_set_send_cmd(uint8_t cmd);
void lms_uart_send_byte(uint8_t data);
/**
 * @brief can���յ����������������д�����ݵ�cmd_info,�ȴ�����
 *
 *     ���cmd_info.write_lock =1 ��ʾ���ݻ�û�з����꣬�����������������д������
 */
void build_ctrl_cmd(uint8_t group_id, uint8_t seg_id, uint8_t cmd);
void lms_init(lms_t *lms);
void lms_uart_send_byte(uint8_t byte);
/**
 * @brief �Ӵ��ڽ���һ���ֽ�����
 *
 * @return uint8_t  �������ݣ���������ݷ��ؽ������ݣ�û�з���0xff
 */
uint8_t lms_uart_recv_byte();
void send_state_machine(lms_t *lms);
void lms_state_machine(lms_t *lms);
#endif