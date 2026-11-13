#ifndef HAL_DELAY_H
#define HAL_DELAY_H
#include <stdint.h>
/* TIM2 chay tu do 1 MHz (1 tick = 1 us). Delay ban chi dung luc khoi dong hoac timing ngan.
 * Trong task dung vTaskDelay. */
void hal_delay_init(void);
void hal_delay_us(uint16_t us);
void hal_delay_ms(uint32_t ms);
#endif
