#include "rtdevice.h"
// #include "database.h"
#include "adc_sample.h"
#include "database.h"
#include "board.h"
#include "drv_adc.h"
#define DBG_TAG "adc_sample"
#define DBG_LVL DBG_INFO
#include "rtdbg.h"

#define  ADC_SAMPLE_STACK_SIZE  512
#define  ADC_SAMPLE_PRIORITY 	15
#define  ADC_SAMPLE_TICK        20


#define ADC_BITS  (1<<12)
#define ADC_VREF  3.3f    


rt_align(RT_ALIGN_SIZE)
rt_uint8_t adc_sample_stack[ADC_SAMPLE_STACK_SIZE];
struct rt_thread  adc_smaple_thread;

rt_device_t  g_adc_simple_timer;
rt_adc_device_t adc_handle;
extern struct rt_semaphore pid_ctrl_sem;

//#define  ADC_VAL_ZERO_CLI(ADC_VAL_LIMIT)  do{\
//if( (RELAY_STATUS_OFF == GET_SYS_STATUS_BIT(RELAY_STATUS_POS)) && (adc_val <= ADC_VAL_LIMIT))\
//{\
//	adc_val = 0;\
//}}while(0);

#define  ADC_VAL_ZERO_CLI(ADC_VAL_LIMIT)  do{\
if( (RELAY_STATUS_OFF == GET_SYS_STATUS_BIT(RELAY_STATUS_POS)) && (adc_calibrate_voltage[i] <= ADC_VAL_LIMIT))\
{\
	adc_calibrate_voltage[i] = 0;\
}}while(0);

// rt_uint32_t  cal_value;
// float current_value1, current_value2, current_value3;


// #define  CCR_MOVE_WINDOW_SIZE 20


// rt_uint32_t adc_read_data[ADC_CANNEL_NUM];        //adc��ȡ����
// float adc_read_voltage[ADC_CANNEL_NUM][CCR_MOVE_WINDOW_SIZE+1];    // adc��ȡ��ѹ
// rt_uint32_t adc_calibrate_voltage[ADC_CANNEL_NUM][CCR_MOVE_WINDOW_SIZE+1];  //У׼���ѹ

// static  rt_uint16_t count = 0;
// rt_uint32_t get_rms(RMS_TYPE type, uint8_t is_average)
// {
//     rt_uint32_t  ret_val;
//     float adc_temp_val;
//     rt_uint32_t adc_temp_cal_val;
   
//     for(int i=0; i<ADC_CANNEL_NUM; i++)
//     {
//         // adc_read_data[i] = rt_adc_read(adc_handle,i);
//         adc_read_voltage[i][count] = rms[i]*1.0f/(ADC_BITS-1)*ADC_VREF; 
//         if(i == E_I_LOAD_RMS)
//         {
//             adc_temp_val = (adc_read_voltage[i][count] * adc_read_voltage[i][count] *
//                                                                             get_In_a2()+ adc_read_voltage[i][count] *get_In_a1() +
//                                                                             get_In_b());
//             if(adc_temp_val < 0)
//             {
//                 adc_temp_val = 0;
//             }                                                                
//            adc_calibrate_voltage[i][count] = (rt_uint32_t)(adc_temp_val * 100);  //0.01A
//         }
//         else if(i== E_U_LOAD_RMS)
//         {
//              adc_calibrate_voltage[i][count] = (rt_uint32_t)((adc_read_voltage[i][count] * get_Un_a()+get_Un_b())*10);  //0.1V
//         }
        
//         adc_temp_cal_val =0;
//         adc_temp_val = 0;
//         for(int j=0; j<CCR_MOVE_WINDOW_SIZE; j++)
//         {
//             adc_temp_val += adc_read_voltage[i][j];    
//             adc_temp_cal_val += adc_calibrate_voltage[i][j];
//         }
      
//         adc_read_voltage[i][CCR_MOVE_WINDOW_SIZE] =adc_temp_val*1.0f/CCR_MOVE_WINDOW_SIZE;
//         adc_calibrate_voltage[i][CCR_MOVE_WINDOW_SIZE] = adc_temp_cal_val/CCR_MOVE_WINDOW_SIZE;

         
//     }
//     switch(type)
//     {
//         case  E_I_LOAD_RMS:
            
//         ret_val =  is_average ? adc_calibrate_voltage[E_I_LOAD_RMS][CCR_MOVE_WINDOW_SIZE]:adc_calibrate_voltage[E_I_LOAD_RMS][count];
//             break;
//         case E_U_SCR_RMS:
//             ret_val =  is_average ? adc_calibrate_voltage[E_U_SCR_RMS][CCR_MOVE_WINDOW_SIZE]:adc_calibrate_voltage[E_U_SCR_RMS][count];
//             break;
//         case E_U_LOAD_RMS:
//             ret_val =  is_average ? adc_calibrate_voltage[E_U_LOAD_RMS][CCR_MOVE_WINDOW_SIZE]:adc_calibrate_voltage[E_U_LOAD_RMS][count];
//             break;
// 		case E_I_SCR_RMS:
//             ret_val =  is_average ? adc_calibrate_voltage[E_I_SCR_RMS][CCR_MOVE_WINDOW_SIZE]:adc_calibrate_voltage[E_U_LOAD_RMS][count];
//             break;
//         default:
//             break;
//     }
    
// //g_ccr_data.ccr_info.adc_val_i = adc_read_voltage[E_I_LOAD_RMS][CCR_MOVE_WINDOW_SIZE];
// //g_ccr_data.ccr_info.adc_val_vscr = adc_read_voltage[E_U_SCR_RMS][CCR_MOVE_WINDOW_SIZE];

//     count++;
//     if(count>=CCR_MOVE_WINDOW_SIZE)
//     {
//         count = 0;
//         // update_i_scr(adc_calibrate_voltage[E_I_SCR_RMS][CCR_MOVE_WINDOW_SIZE]);    
//         // update_scr_voltage(adc_calibrate_voltage[E_U_SCR_RMS][CCR_MOVE_WINDOW_SIZE]);
//         update_i_load(adc_calibrate_voltage[E_I_LOAD_RMS][CCR_MOVE_WINDOW_SIZE]);    
//         update_load_voltage(adc_calibrate_voltage[E_U_LOAD_RMS][CCR_MOVE_WINDOW_SIZE]);
//         get_real_idensity(g_ccr_data.ccr_info.local_i_load);
//     }
    
//     return ret_val;
// }
//rt_uint32_t get_rms(RMS_TYPE type)
//{
//	rt_uint32_t adc_val;
//    float read_value;

//	adc_val= rt_adc_read(adc_handle,(rt_uint8_t)type);
//	read_value = adc_val*1.0f/(ADC_BITS-1)*ADC_VREF;
//	if(type == E_I_RMS)
//	{
//        current_value1 = read_value;
//	//	adc_val = (rt_uint32_t)((adc_val * get_In())/10.0f);  //0.01A
//        adc_val = (rt_uint32_t)((read_value * get_In_a()+get_In_b())*100);  //0.01A
//				ADC_VAL_ZERO_CLI(100);
//        cal_value = adc_val;
//	}
//	else if(type == E_U_SCR_RMS)
//	{
//        current_value2 = read_value;
//		adc_val = (rt_uint32_t)((read_value * get_Un_a()+get_Un_b())*10); //0.1v
//     //   adc_val = (rt_uint32_t)(read_value *400*10); //0.1v
//		ADC_VAL_ZERO_CLI(150);
//	}
//	else if(type == E_U_LOAD_RMS)
//	{
//        current_value3 =  read_value;
//		adc_val = (rt_uint32_t)((read_value * get_Un_a()+get_Un_b())*10); //0.1v
//		ADC_VAL_ZERO_CLI(150);
//	}
//	else
//	{
//		return  0;
//	}
//	return adc_val;
//}



void check_irms(void)
{
	
}

//void adc_sample_thread_entry(void* parm)
//{

//    rt_uint32_t iload;
//    rt_uint32_t vscr;

//    
////	adc_handle= (rt_adc_device_t )rt_device_find("adc0");
////	rt_adc_enable(adc_handle, 7);
//	
//	while(1)
//	{
//        if((RELAY_STATUS_ON == GET_SYS_STATUS_BIT(RELAY_STATUS_POS)))
//		{ 
////            iload =get_rms(E_I_RMS) ;   //0.01A  
////            vscr= get_rms(E_U_SCR_RMS);//0.1v
////            vload = get_rms(E_U_LOAD_RMS);//0.1V
//            
////            update_i_load(iload);
////            update_scr_voltage(vscr);
////            update_load_voltage(vload);

//            check_i_load(get_i_load());
//            Check_SCR(get_scr_voltage());
//            Check_Fuse(get_scr_voltage(),g_ccr_data.ccr_info.half_period);
//            Check_CircuitOpen(vscr, iload);

//            if(IS_SYS_FAULT_EXIST())
//            {
//                print_err_string();
//                update_pass_angle(MIN_PASS_ANGLE);
//                RELAY_OFF();
// 
//            }
//        }
//        else
//        {
//            update_i_load(0);
//            update_scr_voltage(0);
//        }
//		rt_thread_mdelay(20);    
//	}
//		
//}
int adc_sample_init(void)
{
	adc_handle= (rt_adc_device_t )rt_device_find("adc0");
	rt_adc_enable(adc_handle, 14);
    
   // rt_err_t res;
//    res = rt_thread_init(&adc_smaple_thread, "adc_sample", adc_sample_thread_entry, 0, &adc_sample_stack[0],
//                 ADC_SAMPLE_STACK_SIZE,   ADC_SAMPLE_PRIORITY, ADC_SAMPLE_TICK);
//    if(res != RT_EOK)
//    {
//        LOG_E("rt_thread_init adc sample failed!");
//        return -1;
//    }
//    rt_thread_startup(&adc_smaple_thread);
    return 0;
}

INIT_DEVICE_EXPORT(adc_sample_init);

////�ص�����
//rt_err_t single_stable_timer_cb(rt_device_t  dev, rt_size_t size)
//{
//    single_stable_timer_stop();
//    DISABLE_SCR_TRIGER();
//    return 0;
//}

////adc ������ʱ��  20ms
//static int  single_stable_timer_init()
//{
//    rt_hwtimer_mode_t mode = HWTIMER_MODE_ONESHOT;
//     g_adc_simple_timer = rt_device_find("timer6");
//    if (RT_NULL == g_adc_simple_timer)
//    {
//        LOG_E("cannot find timer6");
//		return 0;
//    }
//	rt_err_t result;
//	result = rt_device_open(g_adc_simple_timer, RT_DEVICE_FLAG_RDWR);
//    if(result != RT_EOK)
//    {
//        LOG_E(" open g_adc_simple_timer failed!");
//    }
//	
//	rt_device_set_rx_indicate(g_adc_simple_timer, single_stable_timer_cb);
//		

//	rt_device_control(g_adc_simple_timer, HWTIMER_CTRL_MODE_SET, &mode);
//    NVIC_SetPriority(TIMER6_IRQn, 6);
//    
//    return 0;
//	
//}
//INIT_APP_EXPORT(single_stable_timer_init);

// rt_uint16_t cur_buf[SAMPLES*ADC_CANNEL_NUM];
// void DMA1_Channel0_IRQHandler()
// {
// 	//dma_interrupt_enable(DMA0,DMA_CH0, DMA_INT_FTF);
// 	if( SET == dma_flag_get(DMA1, DMA_CH0, DMA_INTF_FTFIF)){
// 		dma_flag_clear(DMA1,DMA_CH0, DMA_INTF_FTFIF);
// 		timer_disable(TIMER4); 
// 		// get_dma_data(cur_buf);
// 		// cal_rms(cur_buf);
// 		// get_rms(E_I_LOAD_RMS, 0);         
//         timer_enable(TIMER4);	
// 	}

	
// }



