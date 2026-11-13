
/* main.c - Init hardware, tao 5 task, vTaskStartScheduler() */

#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "hal_gpio.h"
#include "hal_adc.h"
#include "hal_uart.h"
#include "hal_delay.h"
#include "hal_dht11.h"
#include "hal_tft.h"
#include "app_ipc.h"
#include "task_input.h"
#include "task_control.h"
#include "task_sensor.h"
#include "task_uart.h"
#include "task_display.h"

int main(void)
{
    /* 1. Phan cung va HAL */
    bsp_system_init();
    hal_gpio_init();
    hal_delay_init();
    hal_adc_init();
    hal_dht11_init();
    hal_uart_init();
    hal_tft_init();

    /* 2. Doi tuong IPC */
    if (app_ipc_init() != pdPASS)
    {
        for (;;);
    }

    /* 3. Dung 5 task */
    if (task_control_create() != pdPASS)
    {
        for (;;);
    }

    if (task_input_create() != pdPASS)
    {
        for (;;);
    }

    if (task_uart_create() != pdPASS)
    {
        for (;;);
    }

    if (task_sensor_create() != pdPASS)
    {
        for (;;);
    }

    if (task_display_create() != pdPASS)
    {
        for (;;);
    }

    /* 4. Chay FreeRTOS Scheduler */
    vTaskStartScheduler();

    for (;;);
}

/* Callback khi tran stack */
void vApplicationStackOverflowHook(TaskHandle_t t, char *name)
{
    (void)t;
    (void)name;

    taskDISABLE_INTERRUPTS();

    for (;;);
}

/* Callback khi cap phat bo nho that bai */
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();

    for (;;);
}
