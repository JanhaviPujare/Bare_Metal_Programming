#include <stdint.h>

#define RCC_CR      (*(volatile uint32_t*) 0x40023800)
#define RCC_PLLCFGR (*(volatile uint32_t*) 0x40023804)
#define RCC_CFGR    (*(volatile uint32_t*) 0x40023808)
#define FLASH_ACR   (*(volatile uint32_t*) 0x40023C00)
#define RCC_AHB1ENR (*(volatile uint32_t*) 0x40023830)
#define GPIOD_MODER (*(volatile uint32_t*) 0x40020C00)
#define GPIOD_ODR   (*(volatile uint32_t*) 0x40020C14)

void delay(volatile uint32_t count){
    while(count--);
}

int main(void){

    // GPIO setup
    RCC_AHB1ENR |= (1U << 3);
    GPIOD_MODER &= ~(3U << 30);    // Clear PD15 bits
    GPIOD_MODER |=  (1U << 30);    // PD15 as output

    // 1. Enable HSE and wait for it to be ready
    RCC_CR |= (1U << 16);
    while(!(RCC_CR & (1U << 17)));

    // 2. Configure PLL: M=8, N=336, P=2, source=HSE
    RCC_PLLCFGR = (8U) | (336U << 6) | (0U << 16) | (1U << 22);

    // 3. Enable PLL and wait for it to be ready
    RCC_CR |= (1U << 24);
    while(!(RCC_CR & (1U << 25)));

    // 4. Set flash latency and prefetch
    FLASH_ACR |= (5U << 0) | (1U << 8);

    // 5. Switch system clock to PLL
    RCC_CFGR |= (2U << 0);

    // 6. Wait for confirmation
    while((RCC_CFGR & (3U << 2)) != (2U << 2));

    // Blink LED at 168MHz
    while(1){
        GPIOD_ODR ^= (1U << 15);
        delay(500000);
    }

    return 0;
}
