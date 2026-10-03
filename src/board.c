#include "board.h"

#include "stm32f4xx.h"

#include <stdatomic.h>

#define AF_USART 7u /* USART1-3 all use alternate function 7 */
#define TICK_HZ 1000u

static _Atomic uint32_t uptime_ms; /* written only by SysTick_Handler */

void board_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_USART3EN;
    /* Read back so the clocks are running before their registers are touched (STM32F4 errata). */
    (void)RCC->APB1ENR;

    /* Console: PA2 (TX) and PA3 (RX) as USART2. */
    GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3)) | GPIO_MODER_MODE2_1 |
                   GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3)) |
                    (AF_USART << GPIO_AFRL_AFSEL2_Pos) | (AF_USART << GPIO_AFRL_AFSEL3_Pos);
    /* Pull RX up so a disconnected line idles high instead of reading noise. */
    GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPD3) | GPIO_PUPDR_PUPD3_0;

    /* Telemetry: PD8 (TX) and PD9 (RX) as USART3, pins the Discovery's onboard parts leave free. */
    GPIOD->MODER = (GPIOD->MODER & ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9)) | GPIO_MODER_MODE8_1 |
                   GPIO_MODER_MODE9_1;
    GPIOD->AFR[1] = (GPIOD->AFR[1] & ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9)) |
                    (AF_USART << GPIO_AFRH_AFSEL8_Pos) | (AF_USART << GPIO_AFRH_AFSEL9_Pos);
    GPIOD->PUPDR = (GPIOD->PUPDR & ~GPIO_PUPDR_PUPD9) | GPIO_PUPDR_PUPD9_0;

    /* PD12 (green LED) as a general-purpose output. */
    GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODE12) | GPIO_MODER_MODE12_0;

    /* SystemInit leaves the chip on its 16 MHz internal clock, so this is a 1 ms tick. */
    SysTick_Config(SystemCoreClock / TICK_HZ);
}

void board_led_set(bool on)
{
    /* BSRR changes only PD12 in a single write, so it can't clobber other GPIOD pins. */
    GPIOD->BSRR = on ? GPIO_BSRR_BS12 : GPIO_BSRR_BR12;
}

bool board_led_is_on(void)
{
    return (GPIOD->ODR & GPIO_ODR_OD12) != 0u;
}

uint32_t board_uptime_ms(void)
{
    return atomic_load_explicit(&uptime_ms, memory_order_relaxed);
}

void SysTick_Handler(void)
{
    atomic_fetch_add_explicit(&uptime_ms, 1u, memory_order_relaxed);
}
