
#ifndef HAL_TFT_H
#define HAL_TFT_H

#include <stdint.h>

#define TFT_W 320
#define TFT_H 240

/* RGB565 */
#define RGB565(r,g,b) \
    ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

/* Ky hieu do */
#define CH_DEG "\177"

/* Khoi tao va dieu khien man hinh */
void hal_tft_init(void);

void hal_tft_set_window(uint16_t x0, uint16_t y0,
                        uint16_t x1, uint16_t y1);

void hal_tft_fill_rect(uint16_t x, uint16_t y,
                       uint16_t w, uint16_t h, uint16_t color);

void hal_tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

void hal_tft_draw_hline(uint16_t x, uint16_t y,
                        uint16_t w, uint16_t color);

void hal_tft_draw_vline(uint16_t x, uint16_t y,
                        uint16_t h, uint16_t color);

void hal_tft_draw_line(int16_t x0, int16_t y0,
                       int16_t x1, int16_t y1, uint16_t color);

void hal_tft_draw_rect(uint16_t x, uint16_t y,
                       uint16_t w, uint16_t h, uint16_t color);

void hal_tft_draw_bar(uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h, uint8_t pct,
                      uint16_t fg, uint16_t bg, uint16_t border);

/* Truyen mang pixel RGB565 */
void hal_tft_write_pixels(const uint16_t *px, uint32_t count);

/* Ve chu */
void hal_tft_draw_char(uint16_t x, uint16_t y, char c,
                       uint16_t fg, uint16_t bg, uint8_t scale);

void hal_tft_draw_string(uint16_t x, uint16_t y, const char *s,
                         uint16_t fg, uint16_t bg, uint8_t scale);

uint16_t hal_tft_text_width(const char *s, uint8_t scale);

#endif
