#ifndef HAL_ADC_H
#define HAL_ADC_H
#include <stdint.h>
void     hal_adc_init(void);
uint16_t hal_adc_read_raw(void);          /* 0..4095 */
uint8_t  hal_adc_read_light_pct(void);    /* 0% = toi, 100% = sang, loc trung binh */
#endif
