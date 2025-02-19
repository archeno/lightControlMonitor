/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-02-25     iysheng           first version
 * 2022-05-03     BruceOu           optimization adc
 */

#include "drv_adc.h"
#include "database.h"
#define DBG_TAG "drv.adc"
#define DBG_LVL DBG_INFO

#include <rtdbg.h>

#ifdef RT_USING_ADC

#if defined(BSP_USING_ADC0)
struct rt_adc_device adc0;
#endif

#if defined(BSP_USING_ADC1)
struct rt_adc_device adc1;
#endif

#if defined(BSP_USING_ADC2)
struct rt_adc_device adc2;
#endif

#define MAX_EXTERN_ADC_CHANNEL 16

static const struct gd32_adc adc_obj[] = {
#ifdef BSP_USING_ADC0
    {
        ADC0,
        RCU_ADC0,
        {
            GET_PIN(A, 0),
            GET_PIN(A, 1),
            GET_PIN(A, 2),
            GET_PIN(A, 3),
            GET_PIN(A, 4),
            GET_PIN(A, 5),
            GET_PIN(A, 6),
            GET_PIN(A, 7),
            GET_PIN(B, 0),
            GET_PIN(B, 1),
            GET_PIN(C, 0),
            GET_PIN(C, 1),
            GET_PIN(C, 2),
            GET_PIN(C, 3),
            GET_PIN(C, 4),
            GET_PIN(C, 5),
        },
        &adc0,
        "adc0",
    },
#endif

#ifdef BSP_USING_ADC1
    {
        ADC1,
        RCU_ADC1,
        {
            GET_PIN(A, 0),
            GET_PIN(A, 1),
            GET_PIN(A, 2),
            GET_PIN(A, 3),
            GET_PIN(A, 4),
            GET_PIN(A, 5),
            GET_PIN(A, 6),
            GET_PIN(A, 7),
            GET_PIN(B, 0),
            GET_PIN(B, 1),
            GET_PIN(C, 0),
            GET_PIN(C, 1),
            GET_PIN(C, 2),
            GET_PIN(C, 3),
            GET_PIN(C, 4),
            GET_PIN(C, 5),
        },
        &adc1,
        "adc1",
    },
#endif
#ifdef BSP_USING_ADC2
    {
        ADC2,
        RCU_ADC2,
        {
            GET_PIN(A, 0),
            GET_PIN(A, 1),
            GET_PIN(A, 2),
            GET_PIN(A, 3),
            GET_PIN(A, 4),
            GET_PIN(A, 5),
            GET_PIN(A, 6),
            GET_PIN(A, 7),
            GET_PIN(B, 0),
            GET_PIN(B, 1),
            GET_PIN(C, 0),
            GET_PIN(C, 1),
            GET_PIN(C, 2),
            GET_PIN(C, 3),
            GET_PIN(C, 4),
            GET_PIN(C, 5),
        },
        &adc2,
        "adc2",
    },
#endif
};

#define VOLTAGE_FACTOR() (1.0f / 4095 * 3.3f * get_Un_a() + get_Un_b())
rt_align(4)
    rt_uint16_t adc_value[SAMPLES * ADC_CANNEL_NUM];

// rt_uint32_t  out_rms[3];
rt_uint32_t rms[ADC_CANNEL_NUM];
float active_power;
rt_uint32_t apparent_power;
float rms_adc_voltage[ADC_CANNEL_NUM][RMS_WINDOWN_SIZE + 1];
float p_adc_value[RMS_WINDOWN_SIZE + 1];
rt_uint16_t rms_adc_cal_voltage_avg[ADC_CANNEL_NUM];
// x/4095 *3.348 = 1.496 --> x = 1829
#define VREFCONST 1840 // 1826
#define BM_THRESHOLD 150
// #define BM_BIAS_THRESHOLD
rt_uint32_t tmp_diff_result;
// rt_uint32_t dma_value[ADC_CANNEL_NUM][SAMPLES]; //
void cal_rms(rt_uint16_t *cur)
{
    rt_uint32_t sample_square[ADC_CANNEL_NUM]; // ��һ�д洢�����ܵ�ֵ���ڶ��и�����ֵ
    rt_uint32_t sample_value[ADC_CANNEL_NUM];  //
    rt_uint32_t diff_max = 0;
    rt_uint16_t cnt_p_n[ADC_CANNEL_NUM];

    rt_int32_t tmp;
    rt_uint32_t tmp_square;
    rt_uint32_t tmp_diff = 0;
    static rt_uint8_t bm_cnt = 0;
    static rt_uint8_t count = 0;
    // rt_memset(dma_value, 0, sizeof(dma_value));
    rt_memset(sample_square, 0, sizeof(sample_square));
    rt_memset(cnt_p_n, 0, sizeof(cnt_p_n));
    uint32_t temp_active_power = 0;
    // rt_memset(diff_max, 0, sizeof(diff_max));
    for (int i = 0; i < SAMPLES * ADC_CANNEL_NUM; i += ADC_CANNEL_NUM)
    {
        // temp_active_power += cur[i];
        for (int j = 0; j < ADC_CANNEL_NUM; j++)
        {
            tmp = (cur[i + j] - VREFCONST);
            tmp_square = tmp * tmp;
            sample_value[j] = tmp;
            sample_square[j] += tmp_square;
        }
        // dma_value[E_U_LOAD_RMS][i] = tmp;
        // dma_value[E_I_LOAD_RMS][i + 1] = tmp;
        temp_active_power += (sample_value[E_U_LOAD_RMS]) * (sample_value[E_I_LOAD_RMS]);
    }

    // �����й�����
    // active_power = temp_active_power / SAMPLES * 1.0f / 4095 * 3.3f * 4.28f + 7.04;
    p_adc_value[count] = temp_active_power / SAMPLES * 1.0f / 4095 * 3.3f * 4.28f + 7.04;
    for (int i = 0; i < ADC_CANNEL_NUM; i++)
    {

        rms[i] = sample_square[i] / SAMPLES;
        rms[i] = sqrt(rms[i]);

        // rms[i] = rms[i]/4095.0f * 3.3 *100;
    }

    rms_adc_voltage[E_U_LOAD_RMS][count] = rms[E_U_LOAD_RMS] * 1.0f / 4095 * 3.3f; // (rms[E_U_LOAD_RMS] * get_Un_a()+get_Un_b())*10; //0.1V
    rms_adc_voltage[E_I_LOAD_RMS][count] = rms[E_I_LOAD_RMS] * 1.0f / 4095 * 3.3f; // * get_In_a1()+get_In_b())*100; //0.01A

    count++;
    float voltage_temp[2] = {0};
    float p_temp = 0;
    // rt_uint32_t voltage_temp_cal[2] ={0};
    for (int j = 0; j < ADC_CANNEL_NUM; j++)
    {
        for (int i = 0; i < RMS_WINDOWN_SIZE; i++)
        {
            voltage_temp[j] += rms_adc_voltage[j][i];
        }
        voltage_temp[j] /= RMS_WINDOWN_SIZE;
        rms_adc_voltage[j][RMS_WINDOWN_SIZE] = voltage_temp[j];
    }
    for (int i = 0; i < RMS_WINDOWN_SIZE; i++)
    {
        p_temp += p_adc_value[i];
    }
    p_temp /= RMS_WINDOWN_SIZE;
    p_adc_value[RMS_WINDOWN_SIZE] = p_temp;
    rms_adc_cal_voltage_avg[E_U_LOAD_RMS] = (rms_adc_voltage[E_U_LOAD_RMS][RMS_WINDOWN_SIZE] * get_Un_a() + get_Un_b()) * 10;   // (rms[E_U_LOAD_RMS] * get_Un_a()+get_Un_b())*10; //0.1V
    rms_adc_cal_voltage_avg[E_I_LOAD_RMS] = (rms_adc_voltage[E_I_LOAD_RMS][RMS_WINDOWN_SIZE] * get_In_a1() + get_In_b()) * 100; // * get_In_a1()+get_In_b())*100; //0.01A
    if (count >= RMS_WINDOWN_SIZE)
    {
        update_i_load(rms_adc_cal_voltage_avg[E_I_LOAD_RMS]);
        update_load_voltage(rms_adc_cal_voltage_avg[E_U_LOAD_RMS]);
        update_active_power((uint16_t)p_adc_value[RMS_WINDOWN_SIZE]);
        count = 0;
    }
}
// if(sample_square[1][0] > sample_square[1][1])
// {
//     tmp_diff = (sample_square[1][0] - sample_square[1][1]);
// }
// else
// {
//     tmp_diff = (sample_square[1][1] - sample_square[1][0]);
// }

// tmp_diff = tmp_diff/SAMPLES;
// tmp_diff_result = sqrt(tmp_diff);
// if(tmp_diff_result > diff_max)
// {
//     diff_max= tmp_diff_result;
// }

/**
 * @brief ADC MSP Initialization
 *        This function configures the hardware resources.
 * @param adc_clk, pin
 * @retval None
 */
static void gd32_adc_gpio_init(rcu_periph_enum adc_clk, rt_base_t pin)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(adc_clk);

#if defined SOC_SERIES_GD32F4xx
    /* configure adc pin */
    gpio_mode_set(PIN_GDPORT(pin), GPIO_MODE_ANALOG, GPIO_PUPD_NONE, PIN_GDPIN(pin));
#else
    /* configure adc pin */
    gpio_init(PIN_GDPORT(pin), GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, PIN_GDPIN(pin));

#endif
}
/**
 * @brief DMA config
 *
 */
static void dma_config(void)
{
    /* ADC_DMA_channel configuration */

    dma_single_data_parameter_struct dma_data_parameter;
    rcu_periph_clock_enable(RCU_DMA1);
    /* ADC DMA_channel configuration */
    dma_deinit(DMA1, DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory0_addr = (uint32_t)(&adc_value[0]);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_data_parameter.number = ADC_CANNEL_NUM * SAMPLES;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);
    // dma_interrupt_enable(DMA1,DMA_CH0, DMA_CHXCTL_FTFIE);
    // NVIC_SetPriority(DMA1_Channel0_IRQn,6);
    // NVIC_EnableIRQ(DMA1_Channel0_IRQn);
    dma_circulation_enable(DMA1, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
}

/*
    TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK = SystemCoreClock / 200 = 1MHz
    output  pa1
    TIMER1 channel0 duty cycle = (4000/ 16000)* 100  = 25%
*/
#define TIMER_OC_PIN GET_PIN(A, 1)
static void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER4);

    /* ���� PA1 Ϊ���ù���: TIM4_CH1 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

    /* ���ø��ù���Ϊ TIM4 */
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_1);

    timer_deinit(TIMER4);

    /* TIMER1 configuration */
    timer_initpara.prescaler = 99;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 99; // 49;//79;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER4, &timer_initpara);

    /* CH0,CH1 and CH2 configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER4, TIMER_CH_1, &timer_ocintpara);

    /* CH0 configuration in PWM mode0,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER4, TIMER_CH_1, 50);
    timer_channel_output_mode_config(TIMER4, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER4, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER4);
    /* auto-reload preload enable */
    // timer_enable(TIMER4);
}
/** static void init_pin4adc
 *
 * @ rt_uint32_t pin: pin information
 * return: N/A
 */
// static void init_adc_pin(rt_base_t pin)
// {
// 	gpio_init(PIN_GDPORT(pin), GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, PIN_GDPIN(pin));
// }
/**
 * @brief ADC enable
 *        This function enable adc.
 * @param device, channel, enabled
 * @retval None
 */
static rt_err_t gd32_adc_enabled(struct rt_adc_device *device, rt_int8_t channel, rt_bool_t enabled)
{
    uint32_t adc_periph;
    struct gd32_adc *adc = (struct gd32_adc *)device->parent.user_data;

    if (channel >= MAX_EXTERN_ADC_CHANNEL)
    {
        LOG_E("invalid channel");
        return -RT_EINVAL;
    }

    adc_periph = (uint32_t)(adc->adc_periph);

    if (enabled == ENABLE)
    {
        dma_config();
        timer_config();
        rcu_periph_clock_enable(RCU_GPIOC);
        gd32_adc_gpio_init(adc->adc_clk, adc->adc_pins[channel]);
        gd32_adc_gpio_init(adc->adc_clk, adc->adc_pins[channel + 1]);

        adc_deinit();
        adc_channel_length_config(adc_periph, ADC_ROUTINE_CHANNEL, ADC_CANNEL_NUM);
        adc_data_alignment_config(adc_periph, ADC_DATAALIGN_RIGHT);
        adc_external_trigger_source_config(adc_periph, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T4_CH1);
        adc_external_trigger_config(adc_periph, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_RISING);
        adc_routine_channel_config(adc_periph, 0, channel, ADC_SAMPLETIME_15);
        adc_routine_channel_config(adc_periph, 1, channel + 1, ADC_SAMPLETIME_15);
        adc_special_function_config(adc_periph, ADC_SCAN_MODE, ENABLE);

        // enable ADC dma
        adc_dma_request_after_last_enable(adc_periph);
        adc_dma_mode_enable(adc_periph);

        adc_enable(adc_periph);
        for (int i = 0; i < 1000; i++)
            for (int j = 0; j < 1000; j++)
                ;
        /* ADC calibration and reset calibration */
        adc_calibration_enable(adc_periph);
    }
    else
    {
        adc_disable(adc_periph);
    }
    return 0;
}

/**
 * @brief convert adc.
 *        This function get adc value.
 * @param device, channel, value
 * @retval None
 */
static rt_err_t gd32_adc_convert(struct rt_adc_device *device, rt_int8_t channel, rt_uint32_t *value)
{
    uint32_t adc_periph;
    struct gd32_adc *adc = (struct gd32_adc *)(device->parent.user_data);

    if (!value)
    {
        LOG_E("invalid param");
        return -RT_EINVAL;
    }

    adc_periph = (uint32_t)(adc->adc_periph);
    adc_software_trigger_enable(adc_periph, ADC_ROUTINE_CHANNEL);

    while (!adc_flag_get(adc_periph, ADC_FLAG_EOC))
    {
    };
    // clear flag
    adc_flag_clear(adc_periph, ADC_FLAG_EOC);

    *value = adc_routine_data_read(adc_periph);

    return 0;
}

static struct rt_adc_ops gd32_adc_ops = {
    .enabled = gd32_adc_enabled,
    .convert = gd32_adc_convert,
};

static int rt_hw_adc_init(void)
{
    int ret, i = 0;

    for (; i < sizeof(adc_obj) / sizeof(adc_obj[0]); i++)
    {
        ret = rt_hw_adc_register(adc_obj[i].adc,
                                 (const char *)adc_obj[i].device_name,
                                 &gd32_adc_ops, &adc_obj[i]);
        if (ret != RT_EOK)
        {
            /* TODO err handler */
            LOG_E("failed register %s, err=%d", adc_obj[i].device_name, ret);
        }
    }

    return ret;
}
INIT_BOARD_EXPORT(rt_hw_adc_init);
#endif

void get_dma_data(rt_uint16_t *buf)
{
    rt_memcpy(buf, adc_value, SAMPLES * ADC_CANNEL_NUM * 2);
    dma_config();
}