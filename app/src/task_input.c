/* InputTask (prio 3): quet 6 nut moi 10 ms, chong doi, UP/DOWN giu thi lap */
#include "FreeRTOS.h"
#include "task.h"
#include "task_input.h"
#include "app_ipc.h"
#include "hal_gpio.h"

static void send_cmd(CmdType_t t)
{
    Command_t c = { SRC_BTN, t, 0 };
    xQueueSend(g_cmdQueue, &c, 0);
}

static void send_ui(UiEvent_t e)
{
    xQueueSend(g_uiQueue, &e, 0);
    xEventGroupSetBits(g_events, EVT_UI_DIRTY);       /* danh thuc DisplayTask */
}

static void on_press(int btn)
{
    switch (btn) {
    case HAL_BTN_MODE:  send_cmd(CMD_MODE_TOGGLE);  break;
    case HAL_BTN_FAN:   send_cmd(CMD_FAN_TOGGLE);   break;
    case HAL_BTN_LIGHT: send_cmd(CMD_LIGHT_TOGGLE); break;
    case HAL_BTN_MENU:  send_ui(UI_EVT_MENU);       break;
    case HAL_BTN_UP:    send_ui(UI_EVT_UP);         break;
    case HAL_BTN_DOWN:  send_ui(UI_EVT_DOWN);       break;
    default: break;
    }
}

static void prvInputTask(void *pv)
{
    uint8_t  stable = 0;                     /* trang thai da chong doi (1 = dang nhan) */
    uint8_t  cnt[HAL_BTN_COUNT] = {0};
    uint16_t held[HAL_BTN_COUNT] = {0};
    TickType_t last = xTaskGetTickCount();
    (void)pv;

    for (;;) {
        uint8_t raw;
        vTaskDelayUntil(&last, pdMS_TO_TICKS(INPUT_SCAN_MS));
        raw = hal_btn_read();

        for (int i = 0; i < HAL_BTN_COUNT; i++) {
            uint8_t now = (raw >> i) & 1u;
            uint8_t cur = (stable >> i) & 1u;

            if (now == cur) {
                cnt[i] = 0;
            } else if (++cnt[i] >= BTN_DEBOUNCE_COUNT) {
                cnt[i] = 0;
                stable ^= (uint8_t)(1u << i);
                held[i] = 0;
                if (now) on_press(i);                         /* canh nhan */
            }

            /* Nhan giu: chi UP/DOWN lap */
            if (((stable >> i) & 1u) && (i == HAL_BTN_UP || i == HAL_BTN_DOWN)) {
                held[i] += INPUT_SCAN_MS;
                if (held[i] >= BTN_REPEAT_DELAY_MS) {
                    held[i] -= BTN_REPEAT_PERIOD_MS;
                    on_press(i);
                }
            } else {
                held[i] = 0;
            }
        }
    }
}

BaseType_t task_input_create(void)
{
    return xTaskCreate(prvInputTask, "INPUT", TASK_INPUT_STACK, NULL, TASK_INPUT_PRIO, NULL);
}
