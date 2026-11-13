#include <string.h>
#include "app_ipc.h"

QueueHandle_t      g_cmdQueue;
QueueHandle_t      g_uiQueue;
EventGroupHandle_t g_events;
SemaphoreHandle_t  g_dataMutex;
SemaphoreHandle_t  g_uartMutex;
SharedData_t       g_data;

BaseType_t app_ipc_init(void)
{
    g_cmdQueue  = xQueueCreate(CMD_QUEUE_LEN, sizeof(Command_t));
    g_uiQueue   = xQueueCreate(UI_QUEUE_LEN,  sizeof(UiEvent_t));
    g_events    = xEventGroupCreate();
    g_dataMutex = xSemaphoreCreateMutex();
    g_uartMutex = xSemaphoreCreateMutex();
    if (!g_cmdQueue || !g_uiQueue || !g_events || !g_dataMutex || !g_uartMutex) return pdFAIL;

    memset(&g_data, 0, sizeof(g_data));
    g_data.mode_auto = 1;
    g_data.light_pct = 100;               /* mac dinh sang cho den khi doc ADC lan dau */
    g_data.th_temp   = TEMP_TH_DEFAULT;
    g_data.th_light  = LIGHT_TH_DEFAULT;
    g_data.page      = PAGE_DASHBOARD;
    xEventGroupSetBits(g_events, EVT_MODE_AUTO | EVT_UI_DIRTY);
    return pdPASS;
}

/* Giu mutex rat ngan: chi copy du lieu */
BaseType_t app_data_snapshot(SharedData_t *out)
{
    if (xSemaphoreTake(g_dataMutex, pdMS_TO_TICKS(20)) != pdPASS) return pdFAIL;
    *out = g_data;
    xSemaphoreGive(g_dataMutex);
    return pdPASS;
}

BaseType_t app_data_update(void (*fn)(SharedData_t *d, void *arg), void *arg)
{
    if (xSemaphoreTake(g_dataMutex, pdMS_TO_TICKS(20)) != pdPASS) return pdFAIL;
    fn(&g_data, arg);
    xSemaphoreGive(g_dataMutex);
    return pdPASS;
}
