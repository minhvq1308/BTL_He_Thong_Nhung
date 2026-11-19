/* ControlTask (prio 4): xu ly lenh, logic tu dong/thu cong, dieu khien relay/LED/coi */
#include "FreeRTOS.h"
#include "task.h"
#include "task_control.h"
#include "app_ipc.h"
#include "hal_gpio.h"

typedef struct {
    uint8_t mode, fan, light, alarm;
    int16_t tht, thl;
} Ctl_t;

static Ctl_t s   = { 1, 0, 0, 0, TEMP_TH_DEFAULT, LIGHT_TH_DEFAULT };   /* trang thai hien tai */
static Ctl_t pub = { 0xFF, 0, 0, 0, 0, 0 };                              /* ban da cong bo (0xFF = chua) */

static void pub_cb(SharedData_t *d, void *a)
{
    const Ctl_t *c = (const Ctl_t *)a;
    d->mode_auto = c->mode;  d->fan = c->fan;  d->light = c->light;  d->alarm = c->alarm;
    d->th_temp = c->tht;     d->th_light = c->thl;
}

static void set_mode(uint8_t m)
{
    s.mode = m;
    if (!m) s.alarm = 0;          /* thu cong: khong coi/canh bao. Giu nguyen quat/den hien tai */
}

static void handle_cmd(const Command_t *c)
{
    switch (c->type) {
    case CMD_MODE_TOGGLE: set_mode(!s.mode);            break;
    case CMD_MODE_SET:    set_mode(c->value ? 1 : 0);   break;
    case CMD_FAN_TOGGLE:   if (!s.mode) s.fan   = !s.fan;           break;   /* tu dong: bo qua */
    case CMD_FAN_SET:      if (!s.mode) s.fan   = c->value ? 1 : 0; break;
    case CMD_LIGHT_TOGGLE: if (!s.mode) s.light = !s.light;           break;
    case CMD_LIGHT_SET:    if (!s.mode) s.light = c->value ? 1 : 0;   break;
    case CMD_SET_TEMP_TH:
        if (c->value >= TEMP_TH_MIN && c->value <= TEMP_TH_MAX)   s.tht = c->value;
        break;
    case CMD_SET_LIGHT_TH:
        if (c->value >= LIGHT_TH_MIN && c->value <= LIGHT_TH_MAX) s.thl = c->value;
        break;
    }
}

/* Logic tu dong dung so lieu cam bien */
static void auto_logic(const SharedData_t *d)
{
    /* Nhiet: bat khi >= nguong; tat khi <= nguong - hysteresis (hoac < nguong neu hysteresis = 0) */
    if (!s.alarm) {
        if (d->temp >= s.tht) s.alarm = 1;
    } else {
#if TEMP_HYSTERESIS > 0
        if (d->temp <= s.tht - TEMP_HYSTERESIS) s.alarm = 0;
#else
        if (d->temp < s.tht) s.alarm = 0;
#endif
    }
    s.fan = s.alarm;

    /* Sang: bat khi < nguong; tat khi >= nguong + hysteresis */
    if (!s.light) {
        if (d->light_pct < s.thl) s.light = 1;
    } else {
        if (d->light_pct >= s.thl + LIGHT_HYSTERESIS) s.light = 0;
    }
}

static int ctl_changed(void)
{
    return pub.mode != s.mode || pub.fan != s.fan || pub.light != s.light ||
           pub.alarm != s.alarm || pub.tht != s.tht || pub.thl != s.thl;
}

static void prvControlTask(void *pv)
{
    uint32_t lastSlot = 0;
    uint8_t  lastBlink = 0;
    (void)pv;

    for (;;) {
        Command_t cmd;
        SharedData_t d;
        TickType_t now;
        uint32_t slot;

        /* Cho lenh toi da CONTROL_TICK_MS roi chay logic */
        if (xQueueReceive(g_cmdQueue, &cmd, pdMS_TO_TICKS(CONTROL_TICK_MS)) == pdPASS)
            handle_cmd(&cmd);

        if (s.mode && app_data_snapshot(&d) == pdPASS)
            auto_logic(&d);

        /* Xuat ra chan */
        now = xTaskGetTickCount();
        hal_relay_set(s.fan);
        hal_led_set(s.light);
        hal_buzzer_set(s.alarm && ((now % (BUZZER_ON_MS + BUZZER_OFF_MS)) < BUZZER_ON_MS));

        /* Cong bo thay doi cho task khac */
        if (ctl_changed()) {
            EventBits_t set = EVT_UI_DIRTY, clr = 0;
            app_data_update(pub_cb, &s);
            if (s.mode)  set |= EVT_MODE_AUTO;   else clr |= EVT_MODE_AUTO;
            if (s.fan)   set |= EVT_FAN_ON;      else clr |= EVT_FAN_ON;
            if (s.light) set |= EVT_LIGHT_ON;    else clr |= EVT_LIGHT_ON;
            if (s.alarm) set |= EVT_TEMP_ALARM;  else clr |= EVT_TEMP_ALARM;
            xEventGroupClearBits(g_events, clr);
            xEventGroupSetBits(g_events, set);
            pub = s;
        }

        /* Nhip 500 ms: LED he thong nhay, banner canh bao nhap nhay */
        slot = now / BLINK_PERIOD_MS;
        if (slot != lastSlot) {
            uint8_t bp = s.alarm ? (uint8_t)(slot & 1u) : 0;
            lastSlot = slot;
            hal_sysled_toggle();
            if (bp != lastBlink) {
                lastBlink = bp;
                if (bp) xEventGroupSetBits(g_events, EVT_BLINK | EVT_UI_DIRTY);
                else  { xEventGroupClearBits(g_events, EVT_BLINK); xEventGroupSetBits(g_events, EVT_UI_DIRTY); }
            }
        }
    }
}

BaseType_t task_control_create(void)
{
    return xTaskCreate(prvControlTask, "CONTROL", TASK_CONTROL_STACK, NULL, TASK_CONTROL_PRIO, NULL);
}
