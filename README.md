# Bare Metal Programming — STM32F407

A collection of bare metal embedded C programs written for the
STM32F407 Discovery board as part of my learning journey in
embedded systems programming. All programs are written using
direct register manipulation — no HAL or abstraction libraries
are used. Register addresses and configurations are sourced
directly from the STM32F407 Reference Manual (RM0090) and the
ARM Cortex-M4 Technical Reference Manual.

## About This Repository

These are practice programs written while learning low-level
embedded systems programming. Each program targets a specific
concept — from basic GPIO control to PLL clock configuration —
and demonstrates direct interaction with hardware peripherals
at the register level.

## Development Environment

- **IDE:** STM32CubeIDE
- **Toolchain:** ARM GCC
- **Board:** STM32F407 Discovery
- **Reference:** RM0090 STM32F407 Reference Manual

## Projects

| # | Project | Concepts |
|---|---|---|
| 01 | [LED Blink](./001LED_Blink/) | GPIO output, RCC clock enable, register manipulation |
| 02 | [Button Press LED](./002ButtonPress_LED/) | GPIO input, IDR polling, two-port configuration |
| 03 | [LED Cycle](./003LED_Cycle/) | Multi-pin GPIO, bitmasking, sequential LED control |
| 04 | [SysTick LED Blink](./004SYSTICK_LED/) | Cortex-M SysTick timer, hardware flag polling |
| 05 | [Button Press LED Counter](./005ButtonPress_LED_Counter/) | Debouncing, press detection, modulo counter |
| 06 | [Hello World using USART](./006HELLO_using_USART/) | USART2, alternate functions, APB1 clock, baud rate |
| 07 | [LED Blink using PLL](./007LED_using_PLL/) | PLL configuration, clock tree, flash wait states, 168MHz |

## Peripheral Knowledge Demonstrated

**GPIO** — mode configuration, output control, input reading,
multi-pin initialization using bitmasking

**RCC** — AHB1 and APB1 clock enable, HSE oscillator,
PLL configuration and lock detection, system clock switching

**USART** — baud rate calculation, transmitter configuration,
alternate function pin mapping, TXE flag polling

**SysTick** — Cortex-M system timer, reload value calculation,
COUNTFLAG polling

**Flash Controller** — wait state configuration for high
frequency operation

## Register Access Pattern

All peripherals are accessed using volatile pointer macros:

```c
#define RCC_AHB1ENR  (*(volatile uint32_t*) 0x40023830)
#define GPIOD_MODER  (*(volatile uint32_t*) 0x40020C00)
#define GPIOD_ODR    (*(volatile uint32_t*) 0x40020C14)
```

All bit manipulation follows standard embedded C patterns:

```c
// Set a bit
RCC_AHB1ENR |= (1U << 3);

// Clear a bit
GPIOD_MODER &= ~(3U << 30);

// Toggle a bit
GPIOD_ODR ^= (1U << 15);

// Read a bit
if (GPIOA_IDR & (1U << 0)) { ... }
```

## Learning Progression

Each project builds on the previous one:

```
01 LED Blink          → fundamental register access pattern
02 Button LED         → adding input alongside output
03 LED Cycle          → multi-pin control and bitmasking
04 SysTick Blink      → moving beyond busy-wait delays
05 Button Counter     → debouncing and state management
06 USART Hello World  → first non-GPIO peripheral, APB bus
07 PLL Configuration  → full clock tree, maximum performance
```

## Note

These programs are written for learning purposes. Code is kept
deliberately simple and direct to make the register operations
clearly readable. Each project folder contains a README
explaining the registers used, how the configuration works,
and how it connects to the reference manual.
