/* HSE 8 MHz -> PLL x9 = 72 MHz, APB1 = 36 MHz, APB2 = 72 MHz, Flash latency 2 */
#include "stm32f10x.h"

uint32_t SystemCoreClock = 72000000;
void SystemCoreClockUpdate(void) { }

void SystemInit(void)
{
    uint32_t timeout = 0;

    RCC->CR   |= RCC_CR_HSION;
    RCC->CFGR &= 0xF8FF0000UL;
    RCC->CR   &= 0xFEF6FFFFUL;
    RCC->CR   &= 0xFFFBFFFFUL;
    RCC->CFGR &= 0xFF80FFFFUL;
    RCC->CIR   = 0x009F0000UL;

    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {
        if (++timeout > 0x5000) { SystemCoreClock = 8000000; return; }  /* HSE hong: chay HSI */
    }

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9;

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) { }

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { }

    SCB->VTOR = FLASH_BASE;
}
