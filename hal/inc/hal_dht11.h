#ifndef HAL_DHT11_H
#define HAL_DHT11_H
#include <stdint.h>
#define DHT11_OK            0
#define DHT11_ERR_TIMEOUT  (-1)
#define DHT11_ERR_CHECKSUM (-2)
void hal_dht11_init(void);
/* Buoc 1: keo DATA xuong thap. Task phai vTaskDelay >= 20 ms roi goi buoc 2. */
void hal_dht11_start(void);
/* Buoc 2: nha duong DATA, doc 40 bit trong taskENTER_CRITICAL (~4 ms), kiem tra checksum. */
int  hal_dht11_read(int16_t *temp, int16_t *humi);
#endif
