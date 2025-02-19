
#include "lmsuart.h"
#include "rs_485.h"
#include "lms.h"
#include "board.h"
void timer0_txDelay_stop(void);
void timer0_txDelay_start(void);

struct rt_mailbox mb;
/* ���ڷ��ʼ����ڴ�� */
static char mb_pool[128];

static void lms_mailbox_init()
{
    rt_err_t result;
    /* ��ʼ��һ�� mailbox */
    result = rt_mb_init(&mb,
                        "lms_mbt",           /* ������ mbt */
                        &mb_pool[0],         /* �����õ����ڴ���� mb_pool */
                        sizeof(mb_pool) / 4, /* �����е��ʼ���Ŀ����Ϊһ���ʼ�ռ 4 �ֽ� */
                        RT_IPC_FLAG_FIFO);   /* ���� FIFO ��ʽ�����̵߳ȴ� */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
    }
}

lms_t g_lms;
/**
 * @brief ����һ���ֽ����ݣ�����ʱ�л�Ϊ����״̬
 *
 * @param byte �����͵�����
 */
void lms_uart_send_byte(uint8_t byte)
{
    USART2_SEND_MODE();

    usart_data_transmit(USART2, byte);
    // while ((usart_flag_get(USART2, USART_FLAG_TBE) == RESET))
    //     ;
}
/**
 * @brief ���ݽ��յ�����������͵��������cmd_info��Ϣ
 *
 * @param group_id ���ƶ������
 * @param seg_id ���ƶ���κ�
 * @param cmd    �������� �ص�0x02  ����0x03
 *
 *  ע������ϴ�����δ������ɣ��򱾴�����账��
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
        // ��ȡ���ݼĴ���
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

// STATE_INIT,        // ��ʼ�׶�,�ȴ�ͬ���źŽ��뷢��̬
//     STATE_TX_SS,   // ��⵽�����źţ���ʱ2.5ms���뷢��̬���������������û������ͬ�����
//     STATE_TX_LS,   // ���ͳ�����
//     STATE_WAITRCV, // �ȴ�3�������źŶ�ȡ��һ��
//     STATE_RECVING, // ����״̬
//     STATE_COMPLETE // �������
void lms_state_machine(lms_t *lms)
{

    switch (lms->state)
    {
    case STATE_INIT:
        // todo: ������㷢����������뷢��״̬
        lms->state = STATE_TX;
        USART2_SEND_MODE(); // ����Ϊ����ģʽ,׼������
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
            USART2_RECV_MODE(); // ����Ϊ����ģʽ,�ȴ���������
            lms->state = STATE_RECVING;
        }
        break;
    case STATE_RECVING: // ��������
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