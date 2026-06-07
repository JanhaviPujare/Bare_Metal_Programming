#include <stdint.h>

#define RCC_AHB1ENR (*(volatile uint32_t*) 0x40023830)
#define GPIOD_MODER (*(volatile uint32_t*) 0x40020C00)
#define GPIOD_ODR   (*(volatile uint32_t*) 0x40020C14)
#define GPIOA_MODER (*(volatile uint32_t*) 0x40020000)
#define GPIOA_IDR   (*(volatile uint32_t*) 0x40020010)

void delay(volatile uint32_t count){
    while(count--);
}

int main(void){

    RCC_AHB1ENR |= (1U << 0);      // GPIOA Clock Enable
    RCC_AHB1ENR |= (1U << 3);      // GPIOD Clock Enable

    GPIOD_MODER &= ~(255U << 24);  // Clear GPIOD Pins 12,13,14,15
    GPIOD_MODER |=  (85U  << 24);  // Set GPIOD Pins 12,13,14,15 as output

    GPIOA_MODER &= ~(3U << 0);     // Set GPIOA Pin 0 as Input

    uint32_t count = 0;

    while(1){

        // Wait for button press
        while(!(GPIOA_IDR & (1U << 0)));
        delay(50000);   // debounce

        // Wait for button release
        while(GPIOA_IDR & (1U << 0));
        delay(50000);   // debounce

        count++;        // one confirmed press

        if(count % 4 == 1){
            GPIOD_ODR = (1U << 12);  // PD12 on
        }
        else if(count % 4 == 2){
            GPIOD_ODR = (1U << 13);  // PD13 on
        }
        else if(count % 4 == 3){
            GPIOD_ODR = (1U << 14);  // PD14 on
        }
        else{
            GPIOD_ODR = (1U << 15);  // PD15 on
        }
    }

    return 0;
}
