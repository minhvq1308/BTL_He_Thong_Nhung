#include "bsp.h"
#include "stm32f10x.h"

void bsp_system_init(void)
{
    /* 4 bit preemption: bat buoc cho FreeRTOS tren Cortex-M3 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    /* Tat JTAG nhung giu SWD de van nap duoc bang ST-Link */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}
