/* Giao dien TFT 320x240 ngang. Chi ve lai vung thay doi (so voi lan ve truoc). */
#include <string.h>
#include "ui.h"
#include "hal_tft.h"
#include "app_cfg.h"

#define C_BG      RGB565(14, 18, 26)
#define C_TITLE   RGB565(24, 52, 100)
#define C_CARD    RGB565(26, 34, 48)
#define C_TEXT    RGB565(235, 240, 245)
#define C_GRAY    RGB565(130, 140, 155)
#define C_GREEN   RGB565(60, 200, 110)
#define C_ORANGE  RGB565(255, 150, 40)
#define C_RED     RGB565(230, 50, 50)
#define C_YELLOW  RGB565(245, 200, 50)
#define C_CYAN    RGB565(80, 190, 240)
#define C_ONBOX   RGB565(30, 150, 85)
#define C_OFFBOX  RGB565(52, 58, 70)
#define C_DARK    RGB565(10, 20, 10)

/* Ban sao du lieu da ve gan nhat */
static struct {
    uint8_t valid, page, mode, bstate, tcol, dht_ok, light_pct, fan, light;
    int16_t temp, humi, th_t, th_l, setv;
} last;

void ui_invalidate(void) { last.valid = 0; }

/* ---- tien ich chuoi (khong dung printf de tiet kiem stack) ---- */
static char *put_str(char *p, const char *s) { while (*s) *p++ = *s++; return p; }
static char *put_int(char *p, int v)
{
    char t[7]; int n = 0;
    if (v < 0) { *p++ = '-'; v = -v; }
    do { t[n++] = (char)('0' + v % 10); v /= 10; } while (v);
    while (n) *p++ = t[--n];
    return p;
}
/* Ve chuoi, dem khoang trang cho du 'width' ky tu de xoa chu cu */
static void text_field(uint16_t x, uint16_t y, const char *s, uint8_t width,
                       uint16_t fg, uint16_t bg, uint8_t scale)
{
    char b[48]; uint8_t n = 0;
    while (*s && n < width && n < sizeof(b) - 1) b[n++] = *s++;
    while (n < width && n < sizeof(b) - 1) b[n++] = ' ';
    b[n] = 0;
    hal_tft_draw_string(x, y, b, fg, bg, scale);
}

/* ---- thanh tieu de ---- */
static void draw_mode(uint8_t mode)
{
    const char *s = mode ? "AUTO" : "MANUAL";
    uint16_t w = hal_tft_text_width(s, 1);
    uint16_t bg = mode ? C_GREEN : C_ORANGE;
    hal_tft_fill_rect(236, 4, 76, 20, bg);
    hal_tft_draw_string(236 + (76 - w) / 2, 6, s, C_DARK, bg, 1);
}

/* ---- dashboard ---- */
static void draw_banner(uint8_t st)
{
    uint16_t bg = (st == 1) ? C_RED : C_BG;
    hal_tft_fill_rect(0, 30, 320, 32, bg);
    if (st) hal_tft_draw_string(40, 30, "!! QUA NHIET !!", (st == 1) ? C_TEXT : C_RED, bg, 2);
}

static void card_static(uint16_t x, const char *label, const char *unit)
{
    hal_tft_fill_rect(x, 64, 151, 84, C_CARD);
    hal_tft_draw_string(x + 8, 72, label, C_GRAY, C_CARD, 1);
    hal_tft_draw_string(x + 88, 108, unit, C_TEXT, C_CARD, 2);
}

static void card_value(uint16_t x, int16_t v, uint8_t ok, uint16_t col)
{
    char b[8], *p = b;
    if (!ok) p = put_str(p, "--"); else p = put_int(p, v);
    *p = 0;
    text_field(x + 8, 92, b, 3, col, C_CARD, 3);          /* so lon 24x48 */
}

static void box(uint16_t x, const char *s, uint8_t on)
{
    uint16_t bg = on ? C_ONBOX : C_OFFBOX;
    uint16_t w = hal_tft_text_width(s, 2);
    hal_tft_fill_rect(x, 192, 151, 32, bg);
    hal_tft_draw_string(x + (151 - w) / 2, 192, s, on ? C_TEXT : C_GRAY, bg, 2);
}

static void render_dash(const SharedData_t *d, uint8_t blink, uint8_t full)
{
    static const uint16_t tcolor[4] = { C_GRAY, C_GREEN, C_YELLOW, C_RED };
    uint8_t bst  = (d->alarm && d->mode_auto) ? (uint8_t)(1 + (blink & 1)) : 0;
    uint8_t tcol = !d->dht_ok ? 0 : (d->temp >= d->th_temp) ? 3 :
                   (d->temp >= d->th_temp - TEMP_NEAR_MARGIN) ? 2 : 1;

    if (full) {
        card_static(6,   "NHIET DO", CH_DEG "C");
        card_static(163, "DO AM", "%");
        hal_tft_draw_string(8, 152, "ANH SANG", C_GRAY, C_BG, 1);
    }
    if (full || bst != last.bstate) draw_banner(bst);

    if (full || d->temp != last.temp || d->dht_ok != last.dht_ok || tcol != last.tcol)
        card_value(6, d->temp, d->dht_ok, tcolor[tcol]);
    if (full || d->humi != last.humi || d->dht_ok != last.dht_ok)
        card_value(163, d->humi, d->dht_ok, C_CYAN);

    if (full || d->light_pct != last.light_pct) {
        char b[8], *p = b;
        p = put_int(p, d->light_pct); *p++ = '%'; *p = 0;
        text_field(280, 152, b, 4, C_TEXT, C_BG, 1);
        hal_tft_draw_bar(8, 170, 304, 16, d->light_pct, C_YELLOW, C_CARD, C_GRAY);
    }
    if (full || d->fan != last.fan)     box(6,   d->fan   ? "FAN ON"    : "FAN OFF",    d->fan);
    if (full || d->light != last.light) box(163, d->light ? "LIGHT ON"  : "LIGHT OFF",  d->light);

    if (full || d->th_temp != last.th_t || d->th_light != last.th_l) {
        char b[48], *p = b;
        p = put_str(p, "Nguong: nhiet "); p = put_int(p, d->th_temp); p = put_str(p, CH_DEG "C");
        p = put_str(p, " | sang ");       p = put_int(p, d->th_light); *p++ = '%'; *p = 0;
        text_field(8, 224, b, 38, C_GRAY, C_BG, 1);
    }
    last.bstate = bst; last.tcol = tcol;
}

/* ---- trang cai nguong ---- */
static void render_set(const SharedData_t *d, uint8_t full)
{
    uint8_t isT = (d->page == PAGE_SET_TEMP);
    int16_t v  = isT ? d->th_temp : d->th_light;
    int16_t mn = isT ? TEMP_TH_MIN : LIGHT_TH_MIN;
    int16_t mx = isT ? TEMP_TH_MAX : LIGHT_TH_MAX;
    uint16_t acc = isT ? C_ORANGE : C_YELLOW;
    char b[16], *p;

    if (full) {
        hal_tft_draw_string(16, 44, isT ? "NGUONG NHIET" : "NGUONG ANH SANG", C_TEXT, C_BG, 2);
        p = b; p = put_int(p, mn); p = put_str(p, isT ? CH_DEG "C" : "%"); *p = 0;
        hal_tft_draw_string(20, 194, b, C_GRAY, C_BG, 1);
        p = b; p = put_int(p, mx); p = put_str(p, isT ? CH_DEG "C" : "%"); *p = 0;
        hal_tft_draw_string(300 - hal_tft_text_width(b, 1), 194, b, C_GRAY, C_BG, 1);
        hal_tft_draw_string(64, 216, "UP/DOWN: doi  MENU: tiep", C_GRAY, C_BG, 1);
    }
    if (full || v != last.setv) {
        uint8_t pct = (uint8_t)(((int32_t)(v - mn) * 100) / (mx - mn));
        p = b; p = put_int(p, v); p = put_str(p, isT ? CH_DEG "C" : "%"); *p = 0;
        text_field(112, 100, b, 4, acc, C_BG, 3);
        hal_tft_draw_bar(20, 168, 280, 20, pct, acc, C_CARD, C_GRAY);
    }
    last.setv = v;
}

void ui_render(const SharedData_t *d, uint8_t blink)
{
    uint8_t full = (!last.valid) || (d->page != last.page);

    if (!last.valid) {
        hal_tft_fill_rect(0, 0, 320, 28, C_TITLE);
        hal_tft_draw_string(8, 6, "HE THONG NHA THONG MINH", C_TEXT, C_TITLE, 1);
        last.mode = 0xFF;
    }
    if (full) hal_tft_fill_rect(0, 28, 320, 212, C_BG);
    if (last.mode != d->mode_auto) draw_mode(d->mode_auto);

    if (d->page == PAGE_DASHBOARD) render_dash(d, blink, full);
    else                           render_set(d, full);

    last.valid = 1;  last.page = d->page;  last.mode = d->mode_auto;
    last.temp = d->temp;  last.humi = d->humi;  last.dht_ok = d->dht_ok;
    last.light_pct = d->light_pct;  last.fan = d->fan;  last.light = d->light;
    last.th_t = d->th_temp;  last.th_l = d->th_light;
}