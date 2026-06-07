#include <stdint.h>

// RCC
#define RCC_AHB1ENR  (*(volatile uint32_t*) 0x40023830)
#define RCC_APB1ENR  (*(volatile uint32_t*) 0x40023880)

// GPIOA
#define GPIOA_MODER  (*(volatile uint32_t*) 0x40020000)
#define GPIOA_AFRL   (*(volatile uint32_t*) 0x40020020)

// USART2
#define USART2_SR    (*(volatile uint32_t*) 0x40004400)
#define USART2_DR    (*(volatile uint32_t*) 0x40004404)
#define USART2_BRR   (*(volatile uint32_t*) 0x40004408)
#define USART2_CR1   (*(volatile uint32_t*) 0x4000440C)

void delay(volatile uint32_t count){
    while(count--);
}

void uart_send_char(char c){
    while(!(USART2_SR & (1U << 7))); // wait for TXE
    USART2_DR = c;
}

void uart_send_string(char *str){
    while(*str){
        uart_send_char(*str);
        str++;
    }
}

int main(void){

    // 1. Enable clocks
    RCC_AHB1ENR |= (1U << 0);   // GPIOA clock
    RCC_APB1ENR |= (1U << 17);  // USART2 clock

    // 2. Configure PA2 as Alternate Function in MODER
    GPIOA_MODER &= ~(3U << 4);  // clear bits 5:4
    GPIOA_MODER |=  (2U << 4);  // set AF mode (10)

    // 3. Set AF7 for PA2 in AFRL
    GPIOA_AFRL &= ~(0xF << 8);  // clear 4 bits for PA2
    GPIOA_AFRL |=  (7U  << 8);  // write AF7

    // 4. Configure USART2
    USART2_BRR = 139;            // 115200 baud at 16MHz
    USART2_CR1 |= (1U << 3);    // TE: enable transmitter
    USART2_CR1 |= (1U << 13);   // UE: enable USART

    // 5. Send string repeatedly
    while(1){
        uart_send_string("Hello\n");
        delay(500000);
    }

    return 0;
}
