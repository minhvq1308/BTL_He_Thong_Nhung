#include "hal_uart.h"
#include "bsp_pins.h"
#include "app_cfg.h"        /* UART_RX_RING_SIZE */
#include "stm32f10x.h"

static volatile uint8_t  s_ring[UART_RX_RING_SIZE];
static volatile uint16_t s_head;            /* ISR ghi */
static volatile uint16_t s_tail;            /* task doc */
static hal_uart_line_cb_t s_lineCb;

void hal_uart_init(void)
{
    GPIO_InitTypeDef g;
    USART_InitTypeDef u;
    NVIC_InitTypeDef n;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    g.GPIO_Pin = GPIO_Pin_9;  g.GPIO_Mode = GPIO_Mode_AF_PP;        g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &g);
    g.GPIO_Pin = GPIO_Pin_10; g.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &g);

    u.USART_BaudRate = UART_BAUD;
    u.USART_WordLength = USART_WordLength_8b;
    u.USART_StopBits = USART_StopBits_1;
    u.USART_Parity = USART_Parity_No;
    u.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    u.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &u);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Uu tien 6 >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY (5): duoc goi ...FromISR */
    n.NVIC_IRQChannel = USART1_IRQn;
    n.NVIC_IRQChannelPreemptionPriority = 6;
    n.NVIC_IRQChannelSubPriority = 0;
    n.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&n);

    USART_Cmd(USART1, ENABLE);
}

void hal_uart_set_line_hook(hal_uart_line_cb_t cb) { s_lineCb = cb; }

void USART1_IRQHandler(void)
{
    uint32_t sr = USART1->SR;
    if (sr & (USART_SR_RXNE | USART_SR_ORE)) {
        uint8_t c = (uint8_t)USART1->DR;            /* doc DR xoa RXNE va ORE */
        uint16_t next = (uint16_t)((s_head + 1) % UART_RX_RING_SIZE);
        if (next != s_tail) { s_ring[s_head] = c; s_head = next; }
        if (c == '\n' && s_lineCb) s_lineCb();
    }
}

int hal_uart_read_line(char *buf, uint16_t max)
{
    uint16_t n = 0;
    for (;;) {
        if (s_tail == s_head) return -1;
        uint8_t c = s_ring[s_tail];
        s_tail = (uint16_t)((s_tail + 1) % UART_RX_RING_SIZE);
        if (c == '\n') break;
        if (c == '\r') continue;
        if (n < max - 1) buf[n++] = (char)c;
    }
    buf[n] = '\0';
    return n;
}

void hal_uart_send_byte(uint8_t b)
{
    while (!(USART1->SR & USART_SR_TXE)) { }
    USART1->DR = b;
}

void hal_uart_send_str(const char *s)
{
    while (*s) hal_uart_send_byte((uint8_t)*s++);
    while (!(USART1->SR & USART_SR_TC)) { }
}
