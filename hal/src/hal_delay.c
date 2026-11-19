#include "hal_delay.h"
#include "stm32f10x.h"

void hal_delay_init(void)
{
    TIM_TimeBaseInitTypeDef t;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    t.TIM_Prescaler     = 72 - 1;          /* TIM2 clock = 72 MHz -> 1 MHz */
    t.TIM_Period        = 0xFFFF;
    t.TIM_ClockDivision = TIM_CKD_DIV1;
    t.TIM_CounterMode   = TIM_CounterMode_Up;
    t.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &t);
    TIM_Cmd(TIM2, ENABLE);
}

void hal_delay_us(uint16_t us)
{
    uint16_t t0 = (uint16_t)TIM2->CNT;
    while ((uint16_t)(TIM2->CNT - t0) < us) { }
}

void hal_delay_ms(uint32_t ms)
{
    while (ms--) hal_delay_us(1000);
}
