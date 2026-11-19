#include "hal_adc.h"
#include "bsp_pins.h"
#include "app_cfg.h"        /* LDR_INVERT, LDR_AVG_SAMPLES */
#include "stm32f10x.h"

void hal_adc_init(void)
{
    GPIO_InitTypeDef g;
    ADC_InitTypeDef  a;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                 /* 72/6 = 12 MHz */

    g.GPIO_Pin  = LDR_PIN;
    g.GPIO_Mode = GPIO_Mode_AIN;
    g.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LDR_PORT, &g);

    ADC_DeInit(ADC1);
    a.ADC_Mode               = ADC_Mode_Independent;
    a.ADC_ScanConvMode       = DISABLE;
    a.ADC_ContinuousConvMode = DISABLE;
    a.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    a.ADC_DataAlign          = ADC_DataAlign_Right;
    a.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &a);

    ADC_RegularChannelConfig(ADC1, LDR_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)) { }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)) { }
}

uint16_t hal_adc_read_raw(void)
{
    uint32_t guard = 100000;
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) && --guard) { }
    return ADC_GetConversionValue(ADC1);
}

uint8_t hal_adc_read_light_pct(void)
{
    uint32_t sum = 0, pct;
    for (int i = 0; i < LDR_AVG_SAMPLES; i++) sum += hal_adc_read_raw();
    pct = (sum / LDR_AVG_SAMPLES) * 100u / 4095u;
    if (pct > 100) pct = 100;
#if LDR_INVERT
    pct = 100 - pct;
#endif
    return (uint8_t)pct;
}
