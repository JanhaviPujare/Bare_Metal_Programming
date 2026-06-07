#include <stdint.h>

#define RCC_AHB1ENR (*(volatile uint32_t*) 0x40023830)
#define GPIOD_MODER (*(volatile uint32_t*) 0x40020C00)
#define GPIOD_ODR   (*(volatile uint32_t*) 0x40020C14)

void delay(volatile uint32_t count){
    while(count--);
}

int main(void){

    RCC_AHB1ENR |= (1U << 3);   // Enable GPIOD clock

    GPIOD_MODER &= ~(3U << 30); // Clear bits 31:30 (pin 15)
    GPIOD_MODER |=  (1U << 30); // Set bit 30 = 01 (output)

    while(1){
        GPIOD_ODR ^= (1U << 15); // Toggle PD15 (LD6 Blue)
        delay(500000);
    }

    return 0;
}
