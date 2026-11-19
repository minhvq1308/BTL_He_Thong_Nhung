/* DisplayTask (prio 1): nhan su kien UI, doi ngưỡng, ve lai phan thay doi */
#include "FreeRTOS.h"
#include "task.h"
#include "task_display.h"
#include "app_ipc.h"
#include "ui.h"

static void set_page_cb(SharedData_t *d, void *a) { d->page = *(uint8_t *)a; }

static void send_set(CmdType_t t, int v)
{
    Command_t c;
    c.src = SRC_UI; c.type = t; c.value = (int16_t)v;
    xQueueSend(g_cmdQueue, &c, pdMS_TO_TICKS(20));
}

static void handle_ui(UiEvent_t ev)
{
    SharedData_t d;
    int dir, v;
    if (app_data_snapshot(&d) != pdPASS) return;

    if (ev == UI_EVT_MENU) {
        uint8_t pg = (uint8_t)((d.page + 1) % PAGE_COUNT);
        app_data_update(set_page_cb, &pg);
        return;
    }
    dir = (ev == UI_EVT_UP) ? 1 : -1;

    if (d.page == PAGE_SET_TEMP) {
        v = d.th_temp + dir * TEMP_TH_STEP;
        if (v >= TEMP_TH_MIN && v <= TEMP_TH_MAX) send_set(CMD_SET_TEMP_TH, v);
    } else if (d.page == PAGE_SET_LIGHT) {
        v = d.th_light + dir * LIGHT_TH_STEP;
        if (v >= LIGHT_TH_MIN && v <= LIGHT_TH_MAX) send_set(CMD_SET_LIGHT_TH, v);
    }
}

static void prvDisplayTask(void *pv)
{
    (void)pv;
    ui_invalidate();

    for (;;) {
        UiEvent_t ev;
        SharedData_t d;

        while (xQueueReceive(g_uiQueue, &ev, 0) == pdPASS) handle_ui(ev);

        /* Chi copy du lieu ra bien cuc bo, nha mutex roi moi ve TFT */
        if (app_data_snapshot(&d) == pdPASS) {
            uint8_t blink = (xEventGroupGetBits(g_events) & EVT_BLINK) ? 1 : 0;
            ui_render(&d, blink);
        }

        /* Cho co thay doi (cam bien/UI), tu xoa bit; timeout 1 s de tu ve lai du phong */
        xEventGroupWaitBits(g_events, EVT_SENSOR_UPDATED | EVT_UI_DIRTY,
                            pdTRUE, pdFALSE, pdMS_TO_TICKS(1000));
    }
}

BaseType_t task_display_create(void)
{
    return xTaskCreate(prvDisplayTask, "DISPLAY", TASK_DISPLAY_STACK, NULL, TASK_DISPLAY_PRIO, NULL);
}
