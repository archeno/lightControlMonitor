#include <global.h>
#include <agile_modbus.h>
#include <rs_485.h>
#include <agile_modbus_slave_util.h>
#include <event_monitor.h>
#include <tca9535.h>
#define DBG_TAG "modbus_master"
#define DBG_LVL DBG_ERROR
#include <rtdbg.h>

static struct rt_mutex slave_mtx;

static uint16_t _tab_registers[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

static int get_map_buf(void *buf, int bufsz)
{
    uint16_t *ptr = (uint16_t *)buf;

    rt_mutex_take(&slave_mtx, RT_WAITING_FOREVER);
    for (int i = 0; i < sizeof(sys.mycan.mb_buffer) / sizeof(sys.mycan.mb_buffer[0]); i++)
    {
        ptr[i] = sys.mycan.mb_buffer[i];
    }
    rt_mutex_release(&slave_mtx);
    return 0;
}

static int set_map_buf(int index, int len, void *buf, int bufsz)
{
    uint16_t *ptr = (uint16_t *)buf;

    rt_mutex_take(&slave_mtx, RT_WAITING_FOREVER);

    for (int i = 0; i < len; i++)
    {
       sys.mycan.mb_buffer[index + i] = ptr[index + i];
    }
    //if write canid, then save data to flash
    if(index  == 3)  //
    {
        
        //write_to_flash_//
         
    }
    rt_mutex_release(&slave_mtx);

    return 0;
}

const agile_modbus_slave_util_map_t register_maps[1] = {
    {0x0, 0x15, get_map_buf, set_map_buf}};

static int addr_check(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info)
{
    int slave = slave_info->sft->slave;
    if ((slave != ctx->slave) && (slave != AGILE_MODBUS_BROADCAST_ADDRESS) && (slave != 0xFF))
        return -AGILE_MODBUS_EXCEPTION_UNKNOW;

    return 0;
}
const agile_modbus_slave_util_t slave_util = {
    NULL,
    0,
    NULL,
    0,
    register_maps,
    sizeof(register_maps) / sizeof(register_maps[0]),
    NULL,
    0,
    addr_check,
    NULL,
    NULL};

static void update_ups_data(uint16_t *buf);
int ups_reg_map[] = {
    0X60, 0X69, 0X7C, 0X7E,0x80,
    0X81, 0X30};
static uint16_t ups_hold_register[34];
static void ups_master_poll(void *parameter)
{
    uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
    uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];

    agile_modbus_rtu_t ctx_rtu;
    agile_modbus_t *ctx = &ctx_rtu._ctx;
    agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf),
                          ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, 1);

    int ups_register_num = sizeof(ups_reg_map) / sizeof(ups_reg_map[0]);
    int send_len;
    int read_len;
    int comm_fail_retry_cnt = 0;

    while (1)
    {
        rt_thread_mdelay(200);
        for (int i = 0; i < ups_register_num; i++)
        {
            send_len = agile_modbus_serialize_read_registers(ctx, ups_reg_map[i], 1);
            rs_485_send_data(RS_485_UART2, ctx->send_buf, send_len);
            read_len = rs_485_receive(RS_485_UART2, ctx->read_buf, ctx->read_bufsz, 1000, 20);
            if (read_len <= 0)
            {
                comm_fail_retry_cnt++;
                if (comm_fail_retry_cnt == 3)
                {
                    comm_fail_retry_cnt = 0;
                    sys.mycan.para.ups_comm_fail = 1;
                    goto update;
                }
                continue;
            }
            sys.mycan.para.ups_comm_fail = 0;
            int rc = agile_modbus_deserialize_read_registers(ctx, read_len, &ups_hold_register[i]);
            if (rc < 0)
            {
                continue;
            }
        update:
            update_ups_data(ups_hold_register);
            rt_thread_mdelay(50);
        }
    }
}

/**
 * @brief
 *
 * @param parameter
 */
// static void up_borad_slave(void *parameter)
// {
//     uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
//     uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
//     rt_mutex_init(&slave_mtx, "slave_mtx", RT_IPC_FLAG_FIFO);
//     agile_modbus_rtu_t ctx_rtu;
//     agile_modbus_t *ctx = &ctx_rtu._ctx;
//     agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf),
//                           ctx_read_buf, sizeof(ctx_read_buf));
//     agile_modbus_set_slave(ctx, 1);

//     int send_len;
//     int read_len;
//     int comm_fail_retry_cnt = 0;
//     while (1)
//     {
//         // rt_thread_mdelay(200);
//         read_len = rs_485_receive(RS_485_UART5, ctx->read_buf, ctx->read_bufsz, 1000, 20);
//         if (read_len <= 0)
//         {
//             comm_fail_retry_cnt++;
//             if (comm_fail_retry_cnt == 3)
//             {
//                 comm_fail_retry_cnt = 0;
                
//                 sys.mycan.para.up_board_com_fail_flag = 1;
//             }
//             LOG_W("upboard com timeout.");
//             continue;
//         }
//         sys.mycan.para.up_board_com_fail_flag = 0;
//         int send_len = agile_modbus_slave_handle(ctx, read_len, 0, agile_modbus_slave_util_callback, &slave_util, NULL);
//         // serial_flush(_fd);
//         if (send_len > 0)
//         {
//             rs_485_send_data(RS_485_UART5, ctx->send_buf, send_len);
//         }
//     }
// }

static int mb_master_init(void)
{
    rt_thread_t tid1 = RT_NULL, tid2 = RT_NULL, tid3 = RT_NULL;

    tid1 = rt_thread_create("md_ups_poll", ups_master_poll, RT_NULL, 2048, 15, 10);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
    }

    // tid2 = rt_thread_create("md_up_borad_slave", up_borad_slave, RT_NULL, 2048, 13, 10);
    // if (tid2 != RT_NULL)
    // {
    //     rt_thread_startup(tid2);
    // }
    return RT_EOK;
}

INIT_APP_EXPORT(mb_master_init);

static void update_ups_data(uint16_t *buf)
{
    // static rt_uint8_t ups_power_down_last = 0;
    // static rt_uint8_t ups_battery_low_last = 0;
    // static rt_uint8_t ups_comm_fail_last =0;
    static rt_uint16_t ups_alarm_last;
    static const rt_uint16_t ups_event_code_tab[] = {UPS_EVENT_CODE_COMM_FAIL,
                                                     UPS_EVENT_CODE_POWER_FAIL, UPS_EVENT_CODE_BATERY_LOW};
    // rt_uint16_t ups_alarm;

    // rt_uint8_t alarm_changed =0;
    rt_memcpy(&sys.mycan.para.ups_input_voltage, buf, 7 * 2);
    sys.mycan.para.ups_power_down = (sys.mycan.para.ups_read_alarm_status >> 7) & 0x01;
    sys.mycan.para.ups_run_stataus = (sys.mycan.para.ups_power_down) ? 1 : 0;
    sys.mycan.para.ups_battery_low = (sys.mycan.para.ups_read_alarm_status >> 6) & 0x01;

    alarm_event_monitor(sys.mycan.para.ups_alarm_status, &ups_alarm_last,
                        ups_event_code_tab, sizeof(ups_event_code_tab) / sizeof(ups_event_code_tab[0]));
}