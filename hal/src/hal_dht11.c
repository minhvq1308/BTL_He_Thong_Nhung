#include "hal_dht11.h"
#include "bsp_pins.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

#define DHT_HIGH()   (DHT11_PORT->BSRR = DHT11_PIN)    /* open-drain: ghi 1 = nha duong */
#define DHT_LOW()    (DHT11_PORT->BRR  = DHT11_PIN)
#define DHT_READ()   ((DHT11_PORT->IDR & DHT11_PIN) ? 1 : 0)

void hal_dht11_init(void)
{
    GPIO_InitTypeDef g;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    DHT_HIGH();
    g.GPIO_Pin   = DHT11_PIN;
    g.GPIO_Mode  = GPIO_Mode_Out_OD;     /* open-drain: keo thap va doc duoc IDR */
    g.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(DHT11_PORT, &g);
    /* Can dien tro keo len 4.7k..10k o chan DATA (module 3 chan thuong co san). */
}

void hal_dht11_start(void) { DHT_LOW(); }

/* Cho DATA dat muc 'level'. Tra ve thoi gian cho (us), -1 neu qua han. Dung TIM2 (1 MHz). */
static int16_t wait_level(uint8_t level, uint16_t timeout_us)
{
    uint16_t t0 = (uint16_t)TIM2->CNT;
    while (DHT_READ() != level) {
        if ((uint16_t)(TIM2->CNT - t0) > timeout_us) return -1;
    }
    return (int16_t)(uint16_t)(TIM2->CNT - t0);
}

int hal_dht11_read(int16_t *temp, int16_t *humi)
{
    uint8_t d[5] = {0, 0, 0, 0, 0};
    int rc = DHT11_OK;

    taskENTER_CRITICAL();
    DHT_HIGH();                                   /* nha duong, DHT11 phan hoi sau 20-40 us */

    if (wait_level(0, 100) < 0 || wait_level(1, 120) < 0 || wait_level(0, 120) < 0) {
        rc = DHT11_ERR_TIMEOUT;
    } else {
        for (int i = 0; i < 40; i++) {
            int16_t w;
            if (wait_level(1, 100) < 0) { rc = DHT11_ERR_TIMEOUT; break; }
            w = wait_level(0, 120);               /* do do rong xung cao */
            if (w < 0) { rc = DHT11_ERR_TIMEOUT; break; }
            d[i >> 3] <<= 1;
            if (w > 40) d[i >> 3] |= 1;           /* ~27us = bit 0, ~70us = bit 1 */
        }
    }
    taskEXIT_CRITICAL();

    if (rc != DHT11_OK) return rc;
    if ((uint8_t)(d[0] + d[1] + d[2] + d[3]) != d[4]) return DHT11_ERR_CHECKSUM;

    *humi = d[0];
    *temp = d[2];
    return DHT11_OK;
}
