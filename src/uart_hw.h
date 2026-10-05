#ifndef UART_HW_H
#define UART_HW_H

#include "stm32f4xx.h"
#include "uart.h"

typedef struct {
    USART_TypeDef *regs;
    IRQn_Type irq;
} uart_hw_t;

/** Each port's USART and interrupt, defined by the board alongside its pins and clocks. */
extern const uart_hw_t uart_hw[UART_COUNT];

/** Services a port's interrupt; the board's USARTn_IRQHandler calls it. */
void uart_irq_handler(uart_id_t id);

#endif
