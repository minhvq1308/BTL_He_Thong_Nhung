/* hal_tft.c - Driver ILI9341 (SPI1, 8 bit, KHONG dung DMA) cho TFT 320x240 ngang.
 * Ban don gian hoa: toc do SPI thap de chay on dinh tren day jumper/breadboard. */
#include "hal_tft.h"
#include "hal_delay.h"
#include "hal_font.h"
#include "bsp_pins.h"
#include "stm32f10x.h"

/* SPI1 chay tren APB2 = 72 MHz.
 *   _8  -> 9 MHz  (an toan, mac dinh)
 *   _4  -> 18 MHz (neu man hinh on dinh, co the thu de nhanh hon)
 * KHONG dung _2 (36 MHz): ILI9341 + day noi dai se bi nhieu/trang xam. */
#define TFT_SPI_PRESCALER   SPI_BaudRatePrescaler_8

/* ---- Huong man hinh & mau ----
 * Qua anh chup: voi MADCTL = 0x00, module nay co truc COT (CASET) chay doc theo
 * canh DAI (320) va truc HANG (PASET) chay theo canh NGAN (240).
 * Toa do logic 320x240 ngang  ->  x = CASET, y = PASET (co lat 1 truc de khong bi guong).
 *   TFT_ROT 0: cam man hinh sao cho canh dai nam ngang, giao dien thang.
 *   TFT_ROT 1: nhu tren nhung xoay 180 do (dung khi hinh bi lon nguoc).
 * Neu chu bi lat guong thi bao lai de doi cach lat. */
#define TFT_MADCTL      0x00
#define TFT_ROT         0
#define TFT_SWAP_RB     1       /* 1 = doi do<->xanh duong; doi sang 0 neu mau bi sai */
#define PANEL_LONG      320
#define PANEL_SHORT     240

static uint16_t fix_color(uint16_t c)
{
#if TFT_SWAP_RB
    return (uint16_t)(((c & 0x1F) << 11) | (c & 0x07E0) | (c >> 11));
#else
    return c;
#endif
}

#define CS_LOW()    (TFT_CS_PORT->BRR  = TFT_CS_PIN)
#define CS_HIGH()   (TFT_CS_PORT->BSRR = TFT_CS_PIN)
#define DC_CMD()    (TFT_DC_PORT->BRR  = TFT_DC_PIN)
#define DC_DATA()   (TFT_DC_PORT->BSRR = TFT_DC_PIN)
#define RST_LOW()   (TFT_RST_PORT->BRR  = TFT_RST_PIN)
#define RST_HIGH()  (TFT_RST_PORT->BSRR = TFT_RST_PIN)

/* ---- SPI muc thap ---- */
static void spi_wait_idle(void)
{
    while (!(SPI1->SR & SPI_SR_TXE)) { }
    while (SPI1->SR & SPI_SR_BSY) { }
}

static void spi_write8(uint8_t b)
{
    while (!(SPI1->SR & SPI_SR_TXE)) { }
    SPI1->DR = b;
}

static void write_cmd(uint8_t c)
{
    spi_wait_idle();
    DC_CMD();
    spi_write8(c);
    spi_wait_idle();
    DC_DATA();
}

static void write_data8(uint8_t d) { spi_write8(d); }

static void write_data16(uint16_t d)
{
    spi_write8((uint8_t)(d >> 8));
    spi_write8((uint8_t)d);
}

/* ---- Khoi tao ---- */
void hal_tft_init(void)
{
    GPIO_InitTypeDef g;
    SPI_InitTypeDef  s;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO  | RCC_APB2Periph_SPI1, ENABLE);

    g.GPIO_Pin = TFT_SCK_PIN | TFT_MOSI_PIN;
    g.GPIO_Mode = GPIO_Mode_AF_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TFT_SPI_PORT, &g);

    g.GPIO_Pin = TFT_CS_PIN;
    g.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(TFT_CS_PORT, &g);

    g.GPIO_Pin = TFT_DC_PIN | TFT_RST_PIN;
    GPIO_Init(TFT_DC_PORT, &g);

    CS_HIGH(); DC_DATA(); RST_HIGH();

    s.SPI_Direction = SPI_Direction_1Line_Tx;
    s.SPI_Mode = SPI_Mode_Master;
    s.SPI_DataSize = SPI_DataSize_8b;
    s.SPI_CPOL = SPI_CPOL_Low;
    s.SPI_CPHA = SPI_CPHA_1Edge;
    s.SPI_NSS = SPI_NSS_Soft;
    s.SPI_BaudRatePrescaler = TFT_SPI_PRESCALER;
    s.SPI_FirstBit = SPI_FirstBit_MSB;
    s.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &s);
    SPI_Cmd(SPI1, ENABLE);

    /* Reset cung */
    RST_HIGH(); hal_delay_ms(10);
    RST_LOW();  hal_delay_ms(20);
    RST_HIGH(); hal_delay_ms(150);

    CS_LOW();
    write_cmd(0x01); spi_wait_idle(); CS_HIGH(); hal_delay_ms(150);   /* Software reset */
    CS_LOW();
    write_cmd(0x28);                                                  /* Display OFF */

    write_cmd(0xCF); write_data8(0x00); write_data8(0xC1); write_data8(0x30);
    write_cmd(0xED); write_data8(0x64); write_data8(0x03); write_data8(0x12); write_data8(0x81);
    write_cmd(0xE8); write_data8(0x85); write_data8(0x00); write_data8(0x78);
    write_cmd(0xCB); write_data8(0x39); write_data8(0x2C); write_data8(0x00); write_data8(0x34); write_data8(0x02);
    write_cmd(0xF7); write_data8(0x20);
    write_cmd(0xEA); write_data8(0x00); write_data8(0x00);
    write_cmd(0xC0); write_data8(0x23);                       /* Power control 1 */
    write_cmd(0xC1); write_data8(0x10);                       /* Power control 2 */
    write_cmd(0xC5); write_data8(0x3E); write_data8(0x28);    /* VCOM 1 */
    write_cmd(0xC7); write_data8(0x86);                       /* VCOM 2 */
    write_cmd(0x36); write_data8(TFT_MADCTL);                 /* MADCTL: huong man hinh */
    write_cmd(0x3A); write_data8(0x55);                       /* 16 bit/pixel RGB565 */
    write_cmd(0xB1); write_data8(0x00); write_data8(0x18);    /* Frame rate */
    write_cmd(0xB6); write_data8(0x08); write_data8(0x82); write_data8(0x27);  /* Display function */
    write_cmd(0xF2); write_data8(0x00);                       /* 3 gamma off */
    write_cmd(0x26); write_data8(0x01);                       /* Gamma curve 1 */
    write_cmd(0xE0);
    { static const uint8_t pg[15] = {0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00};
      for (int i = 0; i < 15; i++) write_data8(pg[i]); }
    write_cmd(0xE1);
    { static const uint8_t ng[15] = {0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F};
      for (int i = 0; i < 15; i++) write_data8(ng[i]); }

    write_cmd(0x11); spi_wait_idle(); CS_HIGH(); hal_delay_ms(150);  /* Sleep out */
    CS_LOW();
    write_cmd(0x29); spi_wait_idle(); CS_HIGH();                      /* Display ON */
    hal_delay_ms(20);

    /* Tu kiem tra: do -> xanh la -> xanh duong -> den.
     * Neu thay dung 3 mau nay thi SPI + man hinh da OK. */
    hal_tft_fill_rect(0, 0, TFT_W, TFT_H, RGB565(255, 0, 0));   hal_delay_ms(250);
    hal_tft_fill_rect(0, 0, TFT_W, TFT_H, RGB565(0, 255, 0));   hal_delay_ms(250);
    hal_tft_fill_rect(0, 0, TFT_W, TFT_H, RGB565(0, 0, 255));   hal_delay_ms(250);
    hal_tft_fill_rect(0, 0, TFT_W, TFT_H, 0x0000);
}

/* ---- Cua so ve (toa do LOGIC 320x240) ---- */
void hal_tft_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint16_t c0, c1, p0, p1;
#if TFT_ROT == 0
    c0 = x0;                        c1 = x1;
    p0 = (PANEL_SHORT - 1) - y1;    p1 = (PANEL_SHORT - 1) - y0;
#else
    c0 = (PANEL_LONG - 1) - x1;     c1 = (PANEL_LONG - 1) - x0;
    p0 = y0;                        p1 = y1;
#endif
    write_cmd(0x2A); write_data16(c0); write_data16(c1);
    write_cmd(0x2B); write_data16(p0); write_data16(p1);
    write_cmd(0x2C);
}

void hal_tft_write_pixels(const uint16_t *px, uint32_t count)
{
    while (count--) write_data16(*px++);
}

/* ---- Fill vung (khong DMA, gui thang tung byte) ---- */
void hal_tft_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint32_t total;
    uint8_t hi, lo;

    color = fix_color(color);
    hi = (uint8_t)(color >> 8); lo = (uint8_t)color;

    if (!w || !h || x >= TFT_W || y >= TFT_H) return;
    if (x + w > TFT_W) w = TFT_W - x;
    if (y + h > TFT_H) h = TFT_H - y;
    total = (uint32_t)w * h;

    CS_LOW();
    hal_tft_set_window(x, y, x + w - 1, y + h - 1);
    while (total--) { spi_write8(hi); spi_write8(lo); }
    spi_wait_idle();
    CS_HIGH();
}

void hal_tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    hal_tft_fill_rect(x, y, 1, 1, color);
}

void hal_tft_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color) { hal_tft_fill_rect(x, y, w, 1, color); }
void hal_tft_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color) { hal_tft_fill_rect(x, y, 1, h, color); }

void hal_tft_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t dx = (x1 > x0) ? x1 - x0 : x0 - x1;
    int16_t dy = (y1 > y0) ? y0 - y1 : y1 - y0;       /* dy am */
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;
    for (;;) {
        if (x0 >= 0 && y0 >= 0) hal_tft_draw_pixel((uint16_t)x0, (uint16_t)y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void hal_tft_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    hal_tft_draw_hline(x, y, w, color);
    hal_tft_draw_hline(x, y + h - 1, w, color);
    hal_tft_draw_vline(x, y, h, color);
    hal_tft_draw_vline(x + w - 1, y, h, color);
}

void hal_tft_draw_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t pct,
                      uint16_t fg, uint16_t bg, uint16_t border)
{
    uint16_t iw, fill;
    if (pct > 100) pct = 100;
    hal_tft_draw_rect(x, y, w, h, border);
    iw   = w - 2;
    fill = (uint16_t)((uint32_t)iw * pct / 100u);
    if (fill)      hal_tft_fill_rect(x + 1, y + 1, fill, h - 2, fg);
    if (fill < iw) hal_tft_fill_rect(x + 1 + fill, y + 1, iw - fill, h - 2, bg);
}

/* ---- Ve chu 8x16, phong to 'scale' lan ---- */
#define MAX_SCALE 4
void hal_tft_draw_char(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale)
{
    const uint8_t *gl;
    int W, H;
    if (scale < 1) scale = 1;
    if (scale > MAX_SCALE) scale = MAX_SCALE;
    if ((uint8_t)c < 32 || (uint8_t)c > 127) c = '?';
    gl = font8x16[(uint8_t)c - 32];
    W = 8 * scale;  H = 16 * scale;

    if (x + W > TFT_W || y + H > TFT_H) return;       /* tran man hinh: bo qua */

    fg = fix_color(fg);  bg = fix_color(bg);

    CS_LOW();
    hal_tft_set_window(x, y, x + W - 1, y + H - 1);
    /* Thu tu gui: CASET (nhanh) roi PASET (cham) */
#if TFT_ROT == 0
    for (int ry = H - 1; ry >= 0; ry--) {              /* PASET tang = y logic giam */
        for (int cx = 0; cx < W; cx++) {               /* CASET tang = x logic tang */
#else
    for (int ry = 0; ry < H; ry++) {                   /* PASET tang = y logic tang */
        for (int cx = W - 1; cx >= 0; cx--) {          /* CASET tang = x logic giam */
#endif
            uint16_t col = (gl[ry / scale] & (0x80 >> (cx / scale))) ? fg : bg;
            write_data16(col);
        }
    }
    spi_wait_idle();
    CS_HIGH();
}

void hal_tft_draw_string(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg, uint8_t scale)
{
    while (*s) {
        hal_tft_draw_char(x, y, *s++, fg, bg, scale);
        x += 8 * scale;
    }
}

uint16_t hal_tft_text_width(const char *s, uint8_t scale)
{
    uint16_t n = 0;
    while (*s++) n++;
    return n * 8 * scale;
}