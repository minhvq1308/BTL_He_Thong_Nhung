#ifndef APP_TYPES_H
#define APP_TYPES_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "app_cfg.h"

#define EVT_MODE_AUTO       (1UL << 0)
#define EVT_FAN_ON          (1UL << 1)
#define EVT_LIGHT_ON        (1UL << 2)
#define EVT_TEMP_ALARM      (1UL << 3)
#define EVT_SENSOR_UPDATED  (1UL << 4)
#define EVT_UART_ACTIVE     (1UL << 5)
#define EVT_UI_DIRTY        (1UL << 6)
#define EVT_BLINK           (1UL << 7)
#define EVT_ALL_DISPLAY     (EVT_MODE_AUTO | EVT_FAN_ON | EVT_LIGHT_ON | EVT_TEMP_ALARM | \
                             EVT_SENSOR_UPDATED | EVT_UI_DIRTY | EVT_BLINK)

typedef enum { SRC_BTN = 0, SRC_UART, SRC_UI } CmdSource_t;

typedef enum {
    CMD_MODE_TOGGLE = 0, CMD_MODE_SET,
    CMD_FAN_TOGGLE,      CMD_FAN_SET,
    CMD_LIGHT_TOGGLE,    CMD_LIGHT_SET,
    CMD_SET_TEMP_TH,     CMD_SET_LIGHT_TH
} CmdType_t;

typedef struct { CmdSource_t src; CmdType_t type; int16_t value; } Command_t;

typedef enum { UI_EVT_MENU = 0, UI_EVT_UP, UI_EVT_DOWN } UiEvent_t;

typedef enum { PAGE_DASHBOARD = 0, PAGE_SET_TEMP, PAGE_SET_LIGHT, PAGE_COUNT } UiPage_t;

typedef struct {
    int16_t temp, humi;
    uint8_t dht_ok;
    uint8_t light_pct;
    uint8_t mode_auto, fan, light, alarm;
    int16_t th_temp, th_light;
    uint8_t page;
} SharedData_t;

extern SharedData_t g_data;
#endif
