#include <stdint.h>

#define RCC_AHB1ENR  (*(volatile uint32_t*) 0x40023830)
#define GPIOA_MODER  (*(volatile uint32_t*) 0x40020000)
#define GPIOA_IDR    (*(volatile uint32_t*) 0x40020010)
#define GPIOD_MODER  (*(volatile uint32_t*) 0x40020C00)
#define GPIOD_ODR    (*(volatile uint32_t*) 0x40020C14)

int main(void){

    RCC_AHB1ENR |= (1U << 0);   // Enable GPIOA clock
    RCC_AHB1ENR |= (1U << 3);   // Enable GPIOD clock

    GPIOA_MODER &= ~(3U << 0);  // PA0 as input (00)

    GPIOD_MODER &= ~(3U << 30); // Clear PD15 bits
    GPIOD_MODER |=  (1U << 30); // PD15 as output

    while(1){
        if(GPIOA_IDR & (1U << 0)){
            GPIOD_ODR |=  (1U << 15); // LED ON
        } else {
            GPIOD_ODR &= ~(1U << 15); // LED OFF
        }
    }

    return 0;
}
