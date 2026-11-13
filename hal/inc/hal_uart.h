#ifndef HAL_UART_H
#define HAL_UART_H
#include <stdint.h>
typedef void (*hal_uart_line_cb_t)(void);   /* goi TRONG ISR khi nhan duoc '\n' */
void hal_uart_init(void);
void hal_uart_set_line_hook(hal_uart_line_cb_t cb);
/* Lay mot dong (bo '\r','\n'), ket thuc '\0'. Tra ve do dai, -1 neu ring trong. */
int  hal_uart_read_line(char *buf, uint16_t max);
void hal_uart_send_byte(uint8_t b);
void hal_uart_send_str(const char *s);      /* polling, goi duoi g_uartMutex */
#endif
