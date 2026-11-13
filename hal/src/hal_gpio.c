#include "hal_gpio.h"
#include "bsp_pins.h"
#include "stm32f10x.h"

/* on = 1: kich hoat; act = muc logic thuc te khi kich hoat */
#define OUT(port, pin, on, act)  do { if ((on) ? (act) : !(act)) (port)->BSRR = (pin); \
                                      else (port)->BRR = (pin); } while (0)

void hal_gpio_init(void)
{
    GPIO_InitTypeDef g;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    g.GPIO_Pin  = BTN_MODE_PIN | BTN_MENU_PIN | BTN_UP_PIN | BTN_DOWN_PIN |
                  BTN_FAN_PIN | BTN_LIGHT_PIN;
    g.GPIO_Mode = GPIO_Mode_IPU;
    g.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(BTN_PORT, &g);

    /* Ghi trang thai TAT truoc khi chuyen sang output de khong giat relay */
    OUT(RELAY_PORT,  RELAY_PIN,  0, RELAY_ACTIVE_LEVEL);
    OUT(LED_PORT,    LED_PIN,    0, LED_ACTIVE_LEVEL);
    OUT(BUZZER_PORT, BUZZER_PIN, 0, BUZZER_ACTIVE_LEVEL);

    g.GPIO_Pin  = RELAY_PIN | LED_PIN | BUZZER_PIN;
    g.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(RELAY_PORT, &g);

    OUT(SYSLED_PORT, SYSLED_PIN, 0, SYSLED_ACTIVE_LEVEL);
    g.GPIO_Pin = SYSLED_PIN;
    GPIO_Init(SYSLED_PORT, &g);
}

uint8_t hal_btn_read(void)
{
    uint16_t in = GPIO_ReadInputData(BTN_PORT);
    uint8_t m = 0;
    if (!(in & BTN_MODE_PIN))  m |= 1u << HAL_BTN_MODE;
    if (!(in & BTN_MENU_PIN))  m |= 1u << HAL_BTN_MENU;
    if (!(in & BTN_UP_PIN))    m |= 1u << HAL_BTN_UP;
    if (!(in & BTN_DOWN_PIN))  m |= 1u << HAL_BTN_DOWN;
    if (!(in & BTN_FAN_PIN))   m |= 1u << HAL_BTN_FAN;
    if (!(in & BTN_LIGHT_PIN)) m |= 1u << HAL_BTN_LIGHT;
    return m;
}

void hal_relay_set(uint8_t on)  { OUT(RELAY_PORT,  RELAY_PIN,  on, RELAY_ACTIVE_LEVEL); }
void hal_led_set(uint8_t on)    { OUT(LED_PORT,    LED_PIN,    on, LED_ACTIVE_LEVEL); }
void hal_buzzer_set(uint8_t on) { OUT(BUZZER_PORT, BUZZER_PIN, on, BUZZER_ACTIVE_LEVEL); }
void hal_sysled_set(uint8_t on) { OUT(SYSLED_PORT, SYSLED_PIN, on, SYSLED_ACTIVE_LEVEL); }
void hal_sysled_toggle(void)    { SYSLED_PORT->ODR ^= SYSLED_PIN; }
