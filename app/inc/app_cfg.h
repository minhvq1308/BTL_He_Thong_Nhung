#ifndef APP_CFG_H
#define APP_CFG_H
#include "FreeRTOS.h"
#include "task.h"

/* Priority: so lon = uu tien cao (configMAX_PRIORITIES = 5) */
#define TASK_CONTROL_PRIO       ( tskIDLE_PRIORITY + 4 )
#define TASK_INPUT_PRIO         ( tskIDLE_PRIORITY + 3 )
#define TASK_UART_PRIO          ( tskIDLE_PRIORITY + 3 )
#define TASK_SENSOR_PRIO        ( tskIDLE_PRIORITY + 2 )
#define TASK_DISPLAY_PRIO       ( tskIDLE_PRIORITY + 1 )

/* Stack (word = 4 byte) */
#define TASK_INPUT_STACK        128
#define TASK_CONTROL_STACK      192
#define TASK_SENSOR_STACK       192
#define TASK_UART_STACK         256
#define TASK_DISPLAY_STACK      384

#define CMD_QUEUE_LEN           8
#define UI_QUEUE_LEN            8

#define UART_RX_RING_SIZE       128
#define UART_LINE_MAX           48
#define UART_REPORT_PERIOD_MS   2000

#define SENSOR_PERIOD_MS        2000
#define INPUT_SCAN_MS           10
#define BTN_DEBOUNCE_COUNT      3
#define BTN_REPEAT_DELAY_MS     500
#define BTN_REPEAT_PERIOD_MS    100
#define CONTROL_TICK_MS         50
#define BUZZER_ON_MS            200
#define BUZZER_OFF_MS           300
#define BLINK_PERIOD_MS         500

#define LDR_INVERT              1
#define LDR_AVG_SAMPLES         16

#define TEMP_TH_DEFAULT         35
#define TEMP_TH_MIN             20
#define TEMP_TH_MAX             50
#define TEMP_TH_STEP            1
#define TEMP_HYSTERESIS         1
#define TEMP_NEAR_MARGIN        3
#define LIGHT_TH_DEFAULT        30
#define LIGHT_TH_MIN            5
#define LIGHT_TH_MAX            95
#define LIGHT_TH_STEP           5
#define LIGHT_HYSTERESIS        5
#endif