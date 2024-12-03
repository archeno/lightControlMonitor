#ifndef __LMSUART_H__
#define __LMSUART_H__

#include "stdint.h"
#include "gd32f4xx.h"
typedef enum
{
    STATE_TX_SS, // 检测到过零信号，延时2.5ms进入发送态（有命令则发送命令，没有则发送同步命令）
    STATE_TX_LS, // 发送长命令
} Send_State_E;
typedef enum
{
    STATE_INIT,    // 初始阶段,等待同步信号进入发送态
    STATE_TX,      // 发送状态，启动发送延时定时器，具体发送过程在发送延时定时器回调中执行
    STATE_WAITRCV, // 等待3个过零信号读取第一个
    STATE_RECVING, // 接收状态
} LMS_STATE_E;

#define TERMIANL_NUM 32          // 终端数量
#define WAIT_CROSS_CNT 2         // 发送后经过两个过零脉冲，第三个过零脉冲开始接收数据
#define TX_DELAY_US (2.5 * 1000) // 2.5US

typedef struct
{
    uint8_t group_id;               // 控制组号
    uint8_t seg_id;                 // 控制段号
    uint8_t write_lock;             // 接收到命令后写锁=1，当锁定时=1，禁止修改信息，发送完成后解锁=0
    uint8_t cmd_data[3];            // 待发送的命令
    uint8_t cmd_toSend;             // 待发送的命令关灯/开灯。
    uint8_t cmd_current_send_index; // 当前发送的字节索引（0~2）

} cmd_t;
typedef struct
{
    LMS_STATE_E state;
    Send_State_E send_state;
    cmd_t cmd_info;
    uint8_t waitToRecvicntLeft;          // 等待接收过零计数 //默认为3
    uint8_t terminal_index;              // 正在接收的终端索引0~31(32个)
    uint8_t terminalState[TERMIANL_NUM]; // ff:无应答 0x00: 灯正常/SIU已激活且正常 0x0b:灯故障/已激活电源故障
                                         //  0x74: 未激活 0x7f: 检测到飞机
    uint8_t polling_complete_flag;       // 巡检完成标志
} lms_t;
extern lms_t g_lms;

extern struct rt_mailbox mb;
void lms_set_send_cmd(uint8_t cmd);
void lms_uart_send_byte(uint8_t data);
/**
 * @brief can接收到服务器控制命令后，写入数据到cmd_info,等待发送
 *
 *     如果cmd_info.write_lock =1 表示数据还没有发送完，丢弃本次命令，否则则写入命令
 */
void build_ctrl_cmd(uint8_t group_id, uint8_t seg_id, uint8_t cmd);
void lms_init(lms_t *lms);
void lms_uart_send_byte(uint8_t byte);
/**
 * @brief 从串口接收一个字节数据
 *
 * @return uint8_t  接收数据：如果有数据返回接收数据，没有返回0xff
 */
uint8_t lms_uart_recv_byte();
void send_state_machine(lms_t *lms);
void lms_state_machine(lms_t *lms);
#endif