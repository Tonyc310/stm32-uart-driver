#include "board.h"

#include "stm32f4xx.h"

#include <stdatomic.h>

#define AF_USART2 7u
#define TICK_HZ 1000u

static _Atomic uint32_t uptime_ms;

void board_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    /* Read back so the clocks are running before their registers are touched (STM32F4 errata). */
    (void)RCC->APB1ENR;

    GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3)) | GPIO_MODER_MODE2_1 |
                   GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3)) |
                    (AF_USART2 << GPIO_AFRL_AFSEL2_Pos) | (AF_USART2 << GPIO_AFRL_AFSEL3_Pos);
    /* Pull RX up so a disconnected line idles high instead of reading noise. */
    GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPD3) | GPIO_PUPDR_PUPD3_0;

    GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODE12) | GPIO_MODER_MODE12_0;

    SysTick_Config(SystemCoreClock / TICK_HZ);
}

void board_led_set(bool on)
{
    GPIOD->BSRR = on ? GPIO_BSRR_BS12 : GPIO_BSRR_BR12;
}

uint32_t board_uptime_ms(void)
{
    return atomic_load_explicit(&uptime_ms, memory_order_relaxed);
}

void SysTick_Handler(void)
{
    atomic_fetch_add_explicit(&uptime_ms, 1u, memory_order_relaxed);
}
