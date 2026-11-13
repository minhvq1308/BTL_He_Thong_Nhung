#ifndef BSP_PINS_H
#define BSP_PINS_H
#include "stm32f10x.h"

/* 1 = kich muc cao, 0 = kich muc thap (relay module thuong kich thap) */
#define RELAY_ACTIVE_LEVEL      1
#define BUZZER_ACTIVE_LEVEL     1
#define LED_ACTIVE_LEVEL        1
#define SYSLED_ACTIVE_LEVEL     0

#define DHT11_PORT              GPIOA
#define DHT11_PIN               GPIO_Pin_1
#define LDR_PORT                GPIOA
#define LDR_PIN                 GPIO_Pin_0
#define LDR_ADC_CHANNEL         ADC_Channel_0

#define TFT_SPI                 SPI1
#define TFT_SPI_PORT            GPIOA
#define TFT_SCK_PIN             GPIO_Pin_5
#define TFT_MOSI_PIN            GPIO_Pin_7
#define TFT_CS_PORT             GPIOA
#define TFT_CS_PIN              GPIO_Pin_4
#define TFT_DC_PORT             GPIOB
#define TFT_DC_PIN              GPIO_Pin_10
#define TFT_RST_PORT            GPIOB
#define TFT_RST_PIN             GPIO_Pin_11

#define BTN_PORT                GPIOB
#define BTN_MODE_PIN            GPIO_Pin_0
#define BTN_MENU_PIN            GPIO_Pin_1
#define BTN_UP_PIN              GPIO_Pin_5
#define BTN_DOWN_PIN            GPIO_Pin_8
#define BTN_FAN_PIN             GPIO_Pin_9
#define BTN_LIGHT_PIN           GPIO_Pin_15

#define RELAY_PORT              GPIOB
#define RELAY_PIN               GPIO_Pin_12
#define LED_PORT                GPIOB
#define LED_PIN                 GPIO_Pin_13
#define BUZZER_PORT             GPIOB
#define BUZZER_PIN              GPIO_Pin_14
#define SYSLED_PORT             GPIOC
#define SYSLED_PIN              GPIO_Pin_13

#define UART_BAUD               115200
/* KHONG dung PB3, PB4, PA13, PA14 */
#endif
