#include "board.h"

#include "stm32f4xx.h"

#define AF_USART2 7u

void board_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    /* Read back so both clocks are running before their registers are touched (STM32F4 errata). */
    (void)RCC->APB1ENR;

    GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3)) | GPIO_MODER_MODE2_1 |
                   GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3)) |
                    (AF_USART2 << GPIO_AFRL_AFSEL2_Pos) | (AF_USART2 << GPIO_AFRL_AFSEL3_Pos);
    /* Pull RX up so a disconnected line idles high instead of reading noise. */
    GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPD3) | GPIO_PUPDR_PUPD3_0;
}
