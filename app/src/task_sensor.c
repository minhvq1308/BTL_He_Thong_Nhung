/* SensorTask (prio 2): doc DHT11 va ADC moi 2 s */
#include "FreeRTOS.h"
#include "task.h"
#include "task_sensor.h"
#include "app_ipc.h"
#include "hal_adc.h"
#include "hal_dht11.h"

typedef struct { int16_t t, h; int rc; uint8_t light; } Sample_t;

static void apply(SharedData_t *d, void *a)
{
    const Sample_t *s = (const Sample_t *)a;
    d->light_pct = s->light;
    if (s->rc == DHT11_OK) { d->temp = s->t; d->humi = s->h; d->dht_ok = 1; }   /* loi: giu gia tri cu */
    else                   { d->dht_ok = 0; }                                    /* danh dau loi */
}

static void prvSensorTask(void *pv)
{
    Sample_t s;
    TickType_t last;
    (void)pv;

    s.t = s.h = 0; s.rc = -1;
    s.light = hal_adc_read_light_pct();               /* co gia tri anh sang ngay tu dau */
    app_data_update(apply, &s);
    xEventGroupSetBits(g_events, EVT_SENSOR_UPDATED);
    vTaskDelay(pdMS_TO_TICKS(1000));                  /* DHT11 can ~1 s on dinh sau cap nguon */
    last = xTaskGetTickCount();

    for (;;) {
        s.light = hal_adc_read_light_pct();
        hal_dht11_start();                            /* keo DATA thap >= 18 ms */
        vTaskDelay(pdMS_TO_TICKS(20));
        s.rc = hal_dht11_read(&s.t, &s.h);            /* ~4 ms trong critical section */
        app_data_update(apply, &s);
        xEventGroupSetBits(g_events, EVT_SENSOR_UPDATED);
        vTaskDelayUntil(&last, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

BaseType_t task_sensor_create(void)
{
    return xTaskCreate(prvSensorTask, "SENSOR", TASK_SENSOR_STACK, NULL, TASK_SENSOR_PRIO, NULL);
}
