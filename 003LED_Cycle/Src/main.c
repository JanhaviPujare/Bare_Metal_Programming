#include <stdint.h>

#define RCC_AHB1ENR (*(volatile uint32_t*) 0x40023830)
#define GPIOD_MODER (*(volatile uint32_t*) 0x40020C00)
#define GPIOD_ODR   (*(volatile uint32_t*) 0x40020C14)

void delay(volatile uint32_t count){
    while(count--);
}

int main(void){

    RCC_AHB1ENR |= (1U << 3);      // Enable GPIOD clock

    GPIOD_MODER &= ~(255U << 24);  // Clear bits for PD12-PD15
    GPIOD_MODER |=  (85U  << 24);  // Set PD12-PD15 as output

    while(1){
        GPIOD_ODR = (1U << 12);    // PD12 ON, rest OFF
        delay(500000);
        GPIOD_ODR = (1U << 13);    // PD13 ON, rest OFF
        delay(500000);
        GPIOD_ODR = (1U << 14);    // PD14 ON, rest OFF
        delay(500000);
        GPIOD_ODR = (1U << 15);    // PD15 ON, rest OFF
        delay(500000);
    }

    return 0;
}
