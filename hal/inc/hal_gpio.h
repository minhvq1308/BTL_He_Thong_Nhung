#ifndef HAL_GPIO_H
#define HAL_GPIO_H
#include <stdint.h>
enum { HAL_BTN_MODE = 0, HAL_BTN_MENU, HAL_BTN_UP, HAL_BTN_DOWN, HAL_BTN_FAN, HAL_BTN_LIGHT, HAL_BTN_COUNT };
void    hal_gpio_init(void);
uint8_t hal_btn_read(void);          /* mat na cac nut dang nhan (nhan = 1) */
void    hal_relay_set(uint8_t on);   /* quat */
void    hal_led_set(uint8_t on);     /* den chieu sang */
void    hal_buzzer_set(uint8_t on);
void    hal_sysled_set(uint8_t on);  /* PC13 */
void    hal_sysled_toggle(void);
#endif
