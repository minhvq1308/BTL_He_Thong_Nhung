/* UartTask (prio 3): nhan dong lenh tu ISR, phan tich, gui phan hoi va bao cao dinh ky */
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "task_uart.h"
#include "app_ipc.h"
#include "hal_uart.h"

static TaskHandle_t s_uartTask;

/* Goi TRONG ISR khi nhan '\n': danh thuc task bang task notification */
static void line_hook(void)
{
    BaseType_t woken = pdFALSE;
    if (s_uartTask) vTaskNotifyGiveFromISR(s_uartTask, &woken);
    portYIELD_FROM_ISR(woken);
}

static void uart_send(const char *s)
{
    if (xSemaphoreTake(g_uartMutex, pdMS_TO_TICKS(100)) == pdPASS) {
        hal_uart_send_str(s);
        xSemaphoreGive(g_uartMutex);
    }
}

static void send_status(void)
{
    SharedData_t d;
    char ts[8], hs[8], buf[100];
    if (app_data_snapshot(&d) != pdPASS) return;
    if (d.dht_ok) { snprintf(ts, sizeof ts, "%d", d.temp); snprintf(hs, sizeof hs, "%d", d.humi); }
    else          { strcpy(ts, "--"); strcpy(hs, "--"); }
    snprintf(buf, sizeof buf, "T=%s,H=%s,L=%d,MODE=%s,FAN=%d,LIGHT=%d,TH_T=%d,TH_L=%d,ALARM=%d\r\n",
             ts, hs, d.light_pct, d.mode_auto ? "AUTO" : "MANUAL",
             d.fan, d.light, d.th_temp, d.th_light, d.alarm);
    uart_send(buf);
}

/* Tach chuoi theo khoang trang, tra ve so token */
static int split(char *s, char *tok[], int max)
{
    int n = 0;
    while (*s && n < max) {
        while (*s == ' ' || *s == '\t') *s++ = 0;
        if (!*s) break;
        tok[n++] = s;
        while (*s && *s != ' ' && *s != '\t') s++;
    }
    return n;
}

static int parse_uint(const char *s, int *out)
{
    int v = 0, len = 0;
    if (!*s) return 0;
    while (*s) {
        if (*s < '0' || *s > '9' || ++len > 3) return 0;
        v = v * 10 + (*s++ - '0');
    }
    *out = v;
    return 1;
}

static void post(CmdType_t t, int v)
{
    Command_t c;
    c.src = SRC_UART; c.type = t; c.value = (int16_t)v;
    if (xQueueSend(g_cmdQueue, &c, pdMS_TO_TICKS(50)) == pdPASS) uart_send("OK\r\n");
    else                                                         uart_send("ERR BUSY\r\n");
}

static void process_line(char *line)
{
    char *t[4];
    int n, v;
    int isAuto;

    for (char *q = line; *q; q++) if (*q >= 'a' && *q <= 'z') *q -= 32;   /* khong phan biet hoa thuong */
    n = split(line, t, 4);
    if (n == 0) return;
    xEventGroupSetBits(g_events, EVT_UART_ACTIVE);
    isAuto = (xEventGroupGetBits(g_events) & EVT_MODE_AUTO) ? 1 : 0;

    if (n == 2 && !strcmp(t[0], "FAN") && (!strcmp(t[1], "ON") || !strcmp(t[1], "OFF"))) {
        if (isAuto) uart_send("ERR AUTO MODE\r\n");
        else        post(CMD_FAN_SET, !strcmp(t[1], "ON"));
    } else if (n == 2 && !strcmp(t[0], "LIGHT") && (!strcmp(t[1], "ON") || !strcmp(t[1], "OFF"))) {
        if (isAuto) uart_send("ERR AUTO MODE\r\n");
        else        post(CMD_LIGHT_SET, !strcmp(t[1], "ON"));
    } else if (n == 2 && !strcmp(t[0], "MODE") && (!strcmp(t[1], "AUTO") || !strcmp(t[1], "MANUAL"))) {
        post(CMD_MODE_SET, !strcmp(t[1], "AUTO"));
    } else if (n == 3 && !strcmp(t[0], "SET") && !strcmp(t[1], "TEMP")) {
        if (!parse_uint(t[2], &v))                          uart_send("ERR BAD VALUE\r\n");
        else if (v < TEMP_TH_MIN || v > TEMP_TH_MAX)        uart_send("ERR RANGE 20..50\r\n");
        else                                                post(CMD_SET_TEMP_TH, v);
    } else if (n == 3 && !strcmp(t[0], "SET") && !strcmp(t[1], "LIGHT")) {
        if (!parse_uint(t[2], &v))                          uart_send("ERR BAD VALUE\r\n");
        else if (v < LIGHT_TH_MIN || v > LIGHT_TH_MAX)      uart_send("ERR RANGE 5..95\r\n");
        else                                                post(CMD_SET_LIGHT_TH, v);
    } else if (n == 2 && !strcmp(t[0], "GET") && !strcmp(t[1], "STATUS")) {
        send_status();
        uart_send("OK\r\n");
    } else {
        uart_send("ERR UNKNOWN CMD\r\n");
    }
}

static void prvUartTask(void *pv)
{
    char line[UART_LINE_MAX];
    TickType_t nextRep = xTaskGetTickCount() + pdMS_TO_TICKS(UART_REPORT_PERIOD_MS);
    (void)pv;

    for (;;) {
        int32_t wait = (int32_t)(nextRep - xTaskGetTickCount());
        uint32_t n = 0;

        if (wait > 0) n = ulTaskNotifyTake(pdTRUE, (TickType_t)wait);   /* so dong hoan chinh */
        while (n--) {
            if (hal_uart_read_line(line, sizeof line) >= 0) process_line(line);
        }

        if ((int32_t)(xTaskGetTickCount() - nextRep) >= 0) {
            send_status();                                              /* bao cao moi 2 s */
            xEventGroupClearBits(g_events, EVT_UART_ACTIVE);
            nextRep += pdMS_TO_TICKS(UART_REPORT_PERIOD_MS);
        }
    }
}

BaseType_t task_uart_create(void)
{
    hal_uart_set_line_hook(line_hook);
    return xTaskCreate(prvUartTask, "UART", TASK_UART_STACK, NULL, TASK_UART_PRIO, &s_uartTask);
}
