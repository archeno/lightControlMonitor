
#include "lmsuart.h"
#include "rs_485.h"
#include "lms.h"
#include "board.h"
void timer0_txDelay_stop(void);
void timer0_txDelay_start(void);

struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[128];

static void lms_mailbox_init()
{
    rt_err_t result;
    /* 初始化一个 mailbox */
    result = rt_mb_init(&mb,
                        "lms_mbt",           /* 名称是 mbt */
                        &mb_pool[0],         /* 邮箱用到的内存池是 mb_pool */
                        sizeof(mb_pool) / 4, /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                        RT_IPC_FLAG_FIFO);   /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
    }
}

lms_t g_lms;
/**
 * @brief 发送一个字节数据，过零时切换为接收状态
 *
 * @param byte 待发送的数据
 */
void lms_uart_send_byte(uint8_t byte)
{
    USART2_SEND_MODE();

    usart_data_transmit(USART2, byte);
    // while ((usart_flag_get(USART2, USART_FLAG_TBE) == RESET))
    //     ;
}
/**
 * @brief 根据接收的命令构建待发送的数据填充cmd_info信息
 *
 * @param group_id 控制对象组号
 * @param seg_id 控制对象段号
 * @param cmd    控制命令 关灯0x02  开灯0x03
 *
 *  注：如果上次命令未发送完成，则本次命令不予处理
 */
void build_ctrl_cmd(uint8_t group_id, uint8_t seg_id, uint8_t cmd)
{
    if (g_lms.cmd_info.write_lock)
        return;
    uint8_t temp_buf[2];
    temp_buf[0] = seg_id;
    temp_buf[1] = cmd << 4;

    g_lms.cmd_info.write_lock = 1;
    g_lms.cmd_info.group_id = group_id;
    g_lms.cmd_info.seg_id = seg_id;
    g_lms.cmd_info.cmd_toSend = cmd;

    g_lms.cmd_info.cmd_data[0] = GenSyncFrame(group_id, LONG_CMD);
    g_lms.cmd_info.cmd_data[1] = seg_id;
    g_lms.cmd_info.cmd_data[2] = crc4_itu(temp_buf, 2) | temp_buf[1];
    g_lms.send_state = STATE_TX_LS;
}

void lms_init(lms_t *lms)
{
    lms->state = STATE_INIT;
    lms->send_state = STATE_TX_SS;
    lms->polling_complete_flag = 0;
    lms->cmd_info.group_id = 0x00;
    lms->cmd_info.seg_id = 0x00;
    lms->cmd_info.write_lock = 0;
    lms->cmd_info.cmd_current_send_index = 0;
    lms->cmd_info.cmd_toSend = 0;
    rt_memset(lms->cmd_info.cmd_data, 0, 3);
    lms->waitToRecvicntLeft = WAIT_CROSS_CNT - 1;
    lms->terminal_index = 0;
    rt_memset(lms->terminalState, 0xff, TERMIANL_NUM);

    lms_mailbox_init();
}
uint8_t lms_uart_recv_byte()
{
    uint8_t received_data = 0xff;
    if (usart_flag_get(USART2, USART_FLAG_RBNE))
    {
        // 读取数据寄存器
        received_data = usart_data_receive(USART2);
    }
    return received_data;
}
void send_state_machine(lms_t *lms)
{
    uint8_t sendbuf[1] = {0x15};
    switch (lms->send_state)
    {
    case STATE_TX_SS:

        // rs_485_send_data(RS_485_UART2, sendbuf, 1);
        lms_uart_send_byte(0x15);
        lms->state = STATE_WAITRCV;
        lms->waitToRecvicntLeft = WAIT_CROSS_CNT - 1;
        break;
    case STATE_TX_LS:
        if (lms->cmd_info.cmd_current_send_index <= 2)
        {
            lms_uart_send_byte(lms->cmd_info.cmd_data[lms->cmd_info.cmd_current_send_index]);
            lms->cmd_info.cmd_current_send_index++;
        }
        else
        {
            lms->cmd_info.cmd_current_send_index = 0;
            lms->cmd_info.write_lock = 0;
            lms->send_state = STATE_TX_SS;
        }
        break;
    default:
        break;
    }
}

// STATE_INIT,        // 初始阶段,等待同步信号进入发送态
//     STATE_TX_SS,   // 检测到过零信号，延时2.5ms进入发送态（有命令则发送命令，没有则发送同步命令）
//     STATE_TX_LS,   // 发送长命令
//     STATE_WAITRCV, // 等待3个过零信号读取第一个
//     STATE_RECVING, // 接收状态
//     STATE_COMPLETE // 接收完成
void lms_state_machine(lms_t *lms)
{

    switch (lms->state)
    {
    case STATE_INIT:
        // todo: 如果满足发送条件则进入发送状态
        lms->state = STATE_TX;
        USART2_SEND_MODE(); // 设置为发送模式,准备数据
        // start 2.5ms timer, in timer0callback to decide to State_tx_ss or State_tx_ss
        //  or to state_tx_ls
        break;
    case STATE_TX:
        timer0_txDelay_start();
        // start 2.5ms timer, in timer0callback to decide to State_tx_ss or State_tx_ss
        break;
    case STATE_WAITRCV:
        if (lms->waitToRecvicntLeft > 0)
        {
            lms->waitToRecvicntLeft--;
        }
        else
        {
            USART2_RECV_MODE(); // 设置为接收模式,等待接收数据
            lms->state = STATE_RECVING;
        }
        break;
    case STATE_RECVING: // 接收数据
        if (lms->terminal_index < TERMIANL_NUM)
        {
            uint8_t recvdata = lms_uart_recv_byte();
            if (recvdata != lms->terminalState[lms->terminal_index])
            {
                // lms_changed_info_t *lms_info = (lms_changed_info_t *)rt_malloc(sizeof(lms_changed_info_t));
                // lms_info->lms_index = lms->terminal_index;
                // lms_info->data = recvdata;
                lms->terminalState[lms->terminal_index] = recvdata;
                rt_mb_send(&mb, (rt_ubase_t)lms->terminal_index);
            }

            lms->terminal_index++;
        }
        else
        {
            lms->terminal_index = 0;
            lms->polling_complete_flag = 1;
            lms->state = STATE_TX;
        }
        break;
    default:
        break;
    }
}