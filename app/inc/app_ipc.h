#ifndef APP_IPC_H
#define APP_IPC_H
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "app_types.h"

extern QueueHandle_t      g_cmdQueue;
extern QueueHandle_t      g_uiQueue;
extern EventGroupHandle_t g_events;
extern SemaphoreHandle_t  g_dataMutex;
extern SemaphoreHandle_t  g_uartMutex;

BaseType_t app_ipc_init(void);
BaseType_t app_data_snapshot(SharedData_t *out);
BaseType_t app_data_update(void (*fn)(SharedData_t *d, void *arg), void *arg);
#endif
