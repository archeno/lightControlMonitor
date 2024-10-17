
#include "data_transfrom.h"
#include "global.h"
#include "can_response.h"
#include "rtdef.h"

  
typedef  void const (* can_msg_package_handler)(void *arg, dataStream_box_t *data, rt_int8_t *len);

typedef struct
{
    rt_uint16_t msg_cmd;  //msg_type HB service_code LB
    can_msg_package_handler msg_handler;
}can_msg_handler_t;

static void ccr_sync_time(void *arg, dataStream_box_t *data, rt_int8_t *len)
{ 

    data_box_t  tmpdata;
    // get recv value and then update system time
    can_user_data_s *user_data = (can_user_data_s*)arg;
    rt_memcpy(tmpdata.array, &(user_data->data_feild.data[2]), 4);
    sys.mycan.para.seconds =  tmpdata.u32;
    rt_memcpy(tmpdata.array, &(user_data->data_feild.data[6]), 2);
    sys.mycan.para.ms = tmpdata.u16;
    
    //broadcase have no ack
    *len = -1;
}

/**
 * @brief ccr_sync
 * 
 * @param arg 
 * @param data 
 * @param len 
 */
static void ccr_sync(void *arg, dataStream_box_t *data, rt_int8_t *len)
{ 
    rt_uint16_t cmd_list[] = {CMD_PACK_CCR_IO, CMD_PACK_CCR_LOOP_V_I_INFO,
                            CMD_PACK_UPS_ANALOG,CMD_PACK_UPS_IO};
    can_user_data_s *user_data = (can_user_data_s*)arg;
    if(sys.can.can0_dev == user_data->can_dev)
    {
        for(int i=0; i<sizeof(cmd_list)/sizeof(cmd_list[0]); i++)
        {
            rt_mb_send(&sys.mycan.can_sync_mb0,  cmd_list[i]);
        }
    }
    else if(sys.can.can1_dev == user_data->can_dev)
    {
        for(int i=0; i<sizeof(cmd_list)/sizeof(cmd_list[0]); i++)
        {
            rt_mb_send(&sys.mycan.can_sync_mb1,  cmd_list[i]);
        }
    }
    
    //broadcase have no ack
    *len = -1;
}


static void ccr_ctrl_light_lvl(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    dataStream_box_t * pdata = data;
    if(sys.mycan.para.ccr_local_remote == 1) //remote
    {
        can_data_feild_u *data_feild = (can_data_feild_u*)arg;
        rt_memcpy(pdata->array, data_feild->data, 2);
        sys.mycan.para.cmd_idensity =  pdata->array[1] - 1;
        sys.mycan.para.OUTPUT_P0 =  sys.mycan.para.cmd_idensity ? (1<<(sys.mycan.para.cmd_idensity-1)):0;
        // sys.mycan.para.OUTPUT_P0 = pdata->array[1] - 1; //service code -- cmd light level
        *len = 0;
    }
    else
    {
           *len = -1;
    }
}
/**
 * @brief ccr alarm event pack
 *        msg cmd  03 01
 * @param data 
 * @param len 
 */
static void  ccr_alarm_event_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    rt_uint16_t  eventCode  = (rt_uint32_t)arg;
    dataStream_box_t *pdata = data;
    // pdata->u16_array[0] = sys.mycan.ccr_event_code | (sys.mycan.ccr_event_type<<15);
    pdata->u16_array[0] = eventCode;
    pdata->u16_array[1] = sys.mycan.para.seconds&0xffff;
    pdata->u16_array[2] = sys.mycan.para.ms;
    *len  = 6;
}

/**
 * @brief  ccr_alarm_status_pack
 *          msg cmd  03 02
 * @param data 
 * @param len 
 */
static void ccr_alarm_status_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    pdata->u16_array[0] = sys.mycan.para.ccr_alarm_state;
    *len  = 2;
}

static void ccr_io_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    rt_uint8_t index = 0;
    dataStream_box_t *pdata = data;
    rt_uint16_t io_state = 0;
    pdata->u16_array[index++] =  sys.mycan.para.ccr_alarm_state;  //±¨¾¯×´Ì¬
    io_state = sys.mycan.para.real_idensity | (sys.mycan.para.cmd_idensity<<3) | (sys.mycan.para.ccr_local_remote<<8)
                | (sys.mycan.para.ccr_relay_state<<7) | (1<<15);
    pdata->u16_array[index++] = io_state;  //IO×´Ì¬
    
   // pdata->u16_array[0] =  sys.mycan.para.real_idensity;
    *len  = index*2;
}



// static void ccr_v_input_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
// {
//     RT_UNUSED(arg);
//     dataStream_box_t *pdata = data;
//     pdata->u16_array[0] =  sys.mycan.para.inPhaseVltA;
//     pdata->u16_array[1] =  sys.mycan.para.inPhaseVltB;
//     pdata->u16_array[2] =  sys.mycan.para.inPhaseVltC;
//     *len = 6;
// }
// static void ccr_i_input_pack(void *arg,  dataStream_box_t *data, rt_int8_t *len)
// {
//     RT_UNUSED(arg);
//     dataStream_box_t *pdata = data;
//     pdata->u16_array[0] =  sys.mycan.para.inPhaseCurrA;
//     pdata->u16_array[1] =  sys.mycan.para.inPhaseCurrB;
//     pdata->u16_array[2] =  sys.mycan.para.inPhaseCurrC;
//     *len = 6;
// }
static void ccr_loop_i_v_info_pack(void *arg,  dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    rt_uint8_t  index = 0;
    pdata->u16_array[index++] =  sys.mycan.para.i_load;
    pdata->u16_array[index++] =  sys.mycan.para.v_load;
    pdata->u16_array[index++] =  5000; //ÆµÂÊ
    *len = index*2;
}
static void ccr_v_output_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    pdata->u16_array[0] =  sys.mycan.para.v_load;
    *len = 2;
}
static void ccr_i_output_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    pdata->u16_array[0] =  sys.mycan.para.i_load;
    *len = 2;
}
// static void ccr_f_output_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
// {
//     RT_UNUSED(arg);
//     dataStream_box_t *pdata = data;
//     pdata->u16_array[0] =  sys.mycan.para.outFreq;
//     *len = 2;
// }
static void ccr_insulation_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    pdata->u16_array[0] =  sys.mycan.para.insResistanceValue;
    pdata->u16_array[1] =  sys.mycan.para.insResistanceUnit;
    *len = 4;
}
static void ccr_light_fail_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    pdata->u16_array[0] =  sys.mycan.para.badLampNum;
    *len = 2;
}

static void ups_io_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    
    pdata->array[0] =  sys.mycan.para.ups_run_stataus | (sys.mycan.para.ups_alarm_status<<1);

   
    pdata->array[1] = sys.mycan.para.ups_battery_capacity;
     
    pdata->u16_array[1] = sys.mycan.para.ups_battery_duration_min;  
    *len = 4;
}

static void ups_alarm_status_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    pdata->u16_array[0] =  sys.mycan.para.ups_alarm_status&0xff;
    *len = 1;
}
static void ups_alarm_event_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    rt_uint16_t eventCode = (rt_uint32_t)arg;
    dataStream_box_t *pdata = data;
    // pdata->u16_array[0] = sys.mycan.ups_event_code | (sys.mycan.ups_event_type<<15);
    pdata->u16_array[0] = eventCode;
    pdata->u16_array[1] = sys.mycan.para.seconds&0xffff;
    pdata->u16_array[2] = sys.mycan.para.ms;
    *len  = 6;
}
static void ups_analog_pack(void *arg, dataStream_box_t *data, rt_int8_t *len)
{
    RT_UNUSED(arg);
    dataStream_box_t *pdata = data;
    pdata->u16_array[0] =  sys.mycan.para.ups_input_voltage;
    pdata->u16_array[1] =  sys.mycan.para.ups_output_voltage;
    pdata->u16_array[2] =  sys.mycan.para.ups_battery_voltage;
    *len = 6;
}

static const can_msg_handler_t can_msg_handler_map[] = 
{
    {CMD_SYNC, ccr_sync},
    // {CMD_PACK_CCR_ALARM_EVENT, ccr_alarm_event_pack},
    // {CMD_PACK_CCR_ALARM_STATUS, ccr_alarm_status_pack},
    {CMD_PACK_CCR_IO, ccr_io_pack},
    {CMD_SYNC_TIME, ccr_sync_time},
    {CMD_CTRL_LIGHT_LVL, ccr_ctrl_light_lvl},
    // {CMD_PACK_UPS_ALARM_EVENT, ups_alarm_event_pack},
    {CMD_PACK_UPS_ALARM_STATUS, ups_alarm_status_pack},
    {CMD_PACK_UPS_IO, ups_io_pack},
    {CMD_PACK_UPS_ANALOG, ups_analog_pack},
    // {CMD_PACK_CCR_V_INPUT, ccr_v_input_pack},
    // {CMD_PACK_CCR_I_INPUT, ccr_i_input_pack},
    {CMD_PACK_CCR_LOOP_V_I_INFO, ccr_loop_i_v_info_pack},
    // {CMD_PACK_CCR_V_OUTPUT, ccr_v_output_pack},
    // {CMD_PACK_CCR_I_OUTPUT, ccr_i_output_pack},
    // {CMD_PACK_CCR_F_OUTPUT, ccr_f_output_pack}, 
    {CMD_PACK_CCR_INSULATION, ccr_insulation_pack},
    {CMD_PACK_CCR_LIGHT_FAIL_NUM, ccr_light_fail_pack}
};


static can_msg_package_handler find_cmd_with_msg_cmd(rt_uint16_t cmd)
{
    rt_uint8_t total_item = sizeof(can_msg_handler_map)/sizeof(can_msg_handler_map[0]);
    
    for(int index = 0;  index < total_item; index++)
    {
        if(can_msg_handler_map[index].msg_cmd == cmd)
        {
            return can_msg_handler_map[index].msg_handler;
        }
    }

    // cmd not found then return NULL pointer
    return NULL;
}
/**
 * @brief package data according to the msg_class and service code
 * @param data the data pointer where to pack the data
 * @param len the total length of packed data
 */
void can_msg_response_data_pack(rt_uint16_t msg_cmd, void *arg, dataStream_box_t * data, rt_int8_t *len)
{
    can_msg_package_handler msg_handler;
    rt_uint16_t cmd  = msg_cmd;
    msg_handler  = find_cmd_with_msg_cmd(cmd);
    
    //package data 
    if(msg_handler != NULL)
    {
        (*msg_handler)(arg, data, len);
    }
    
}