#include "uart.h"

#include "ring_buffer.h"
#include "stm32f4xx.h"

#include <stdatomic.h>

#define RX_BUFFER_SIZE 256u
#define TX_BUFFER_SIZE 256u

_Static_assert((RX_BUFFER_SIZE & (RX_BUFFER_SIZE - 1u)) == 0u, "RX buffer must be a power of two");
_Static_assert((TX_BUFFER_SIZE & (TX_BUFFER_SIZE - 1u)) == 0u, "TX buffer must be a power of two");

typedef struct {
    USART_TypeDef *regs;
    IRQn_Type irq;
    ring_buffer_t rx;
    ring_buffer_t tx;
    uint8_t rx_storage[RX_BUFFER_SIZE];
    uint8_t tx_storage[TX_BUFFER_SIZE];
    /* Counters are written only by the ISR. */
    _Atomic uint32_t rx_bytes;
    _Atomic uint32_t tx_bytes;
    _Atomic uint32_t rx_dropped;
} uart_t;

/* To add a port: an entry here, its IRQ handler below, and its clocks and pins in board.c. */
static uart_t ports[UART_COUNT] = {
    [UART_CONSOLE] = {.regs = USART2, .irq = USART2_IRQn},
    [UART_TELEMETRY] = {.regs = USART3, .irq = USART3_IRQn},
};

static uint32_t apb1_clock_hz(void)
{
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    return SystemCoreClock >> APBPrescTable[ppre1];
}

void uart_init(uart_id_t id, uint32_t baud)
{
    uart_t *port = &ports[id];
    USART_TypeDef *regs = port->regs;

    /* Can't fail: the static asserts above guarantee power-of-two sizes. */
    (void)rb_init(&port->rx, port->rx_storage, sizeof port->rx_storage);
    (void)rb_init(&port->tx, port->tx_storage, sizeof port->tx_storage);
    atomic_init(&port->rx_bytes, 0u);
    atomic_init(&port->tx_bytes, 0u);
    atomic_init(&port->rx_dropped, 0u);

    regs->CR1 = 0u;
    /* With 16x oversampling, BRR is the bus clock divided by the baud rate, rounded. */
    regs->BRR = (apb1_clock_hz() + baud / 2u) / baud;
    regs->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

    NVIC_EnableIRQ(port->irq);
}

size_t uart_write(uart_id_t id, const uint8_t *data, size_t len)
{
    uart_t *port = &ports[id];
    size_t queued = 0;

    while (queued < len && rb_push(&port->tx, data[queued])) {
        queued++;
    }
    if (queued > 0u) {
        /* Racing the ISR here is harmless: at worst one TXE interrupt finds the buffer empty. */
        port->regs->CR1 |= USART_CR1_TXEIE;
    }
    return queued;
}

size_t uart_read(uart_id_t id, uint8_t *data, size_t len)
{
    uart_t *port = &ports[id];
    size_t count = 0;

    while (count < len && rb_pop(&port->rx, &data[count])) {
        count++;
    }
    return count;
}

uart_stats_t uart_stats(uart_id_t id)
{
    uart_t *port = &ports[id];

    return (uart_stats_t){
        .rx_bytes = atomic_load_explicit(&port->rx_bytes, memory_order_relaxed),
        .tx_bytes = atomic_load_explicit(&port->tx_bytes, memory_order_relaxed),
        .rx_dropped = atomic_load_explicit(&port->rx_dropped, memory_order_relaxed),
    };
}

static void handle_irq(uart_t *port)
{
    USART_TypeDef *regs = port->regs;
    uint32_t status = regs->SR;

    if (status & (USART_SR_RXNE | USART_SR_ORE)) {
        /* Reading DR after SR clears both RXNE and ORE. */
        uint8_t byte = (uint8_t)regs->DR;
        /* ORE means the hardware dropped a byte that arrived before DR was read. */
        uint32_t lost = (status & USART_SR_ORE) ? 1u : 0u;

        atomic_fetch_add_explicit(&port->rx_bytes, 1u, memory_order_relaxed);

        if (!rb_push(&port->rx, byte)) {
            lost++;
        }
        if (lost > 0u) {
            atomic_fetch_add_explicit(&port->rx_dropped, lost, memory_order_relaxed);
        }
    }

    /* TXE stays set while the transmitter is idle, so act on it only while TXEIE is on. */
    if ((status & USART_SR_TXE) && (regs->CR1 & USART_CR1_TXEIE)) {
        uint8_t byte;

        if (rb_pop(&port->tx, &byte)) {
            regs->DR = byte;
            atomic_fetch_add_explicit(&port->tx_bytes, 1u, memory_order_relaxed);
        } else {
            /* Nothing left to send: silence TXE until uart_write() queues more. */
            regs->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

void USART2_IRQHandler(void)
{
    handle_irq(&ports[UART_CONSOLE]);
}

void USART3_IRQHandler(void)
{
    handle_irq(&ports[UART_TELEMETRY]);
}
