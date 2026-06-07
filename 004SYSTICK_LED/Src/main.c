#include <stdint.h>

#define RCC_AHB1ENR (*(volatile uint32_t*) 0x40023830)
#define GPIOD_MODER (*(volatile uint32_t*) 0x40020C00)
#define GPIOD_ODR   (*(volatile uint32_t*) 0x40020C14)
#define SYST_CSR    (*(volatile uint32_t*) 0xE000E010)
#define SYST_RVR    (*(volatile uint32_t*) 0xE000E014)
#define SYST_CVR    (*(volatile uint32_t*) 0xE000E018)

int main(void){

    // GPIO setup
    RCC_AHB1ENR |= (1U << 3);      // Enable GPIOD clock
    GPIOD_MODER &= ~(3U << 30);    // Clear PD15 bits
    GPIOD_MODER |=  (1U << 30);    // PD15 as output

    // SysTick setup
    SYST_RVR = 8000000 - 1;        // 500ms at 16MHz
    SYST_CVR = 0;                  // Clear current value
    SYST_CSR = (1U << 0) | (1U << 2); // Enable, processor clock

    while(1){
        while(!(SYST_CSR & (1U << 16))); // Wait for countdown
        GPIOD_ODR ^= (1U << 15);         // Toggle LED
    }

    return 0;
}
