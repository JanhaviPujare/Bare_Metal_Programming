# 06 Hello using USART — Bare Metal STM32F407

## What This Program Does
Transmits "Hello" repeatedly over USART2 to a PC terminal at
115200 baud rate. Configures PA2 as the UART transmit pin using
the GPIO alternate function system. This is the first project
using a non-GPIO peripheral requiring APB bus clock enable and
alternate function pin configuration. No HAL library used.

## Hardware
- Board: STM32F407 Discovery
- UART: USART2 — TX on PA2
- Connection: PA2 → USB-to-Serial adapter or onboard ST-Link
- Terminal: Any serial terminal at 115200 baud, 8N1

## Registers Used

| Register | Address | Purpose |
|---|---|---|
| RCC_AHB1ENR | 0x40023830 | Enable GPIOA clock |
| RCC_APB1ENR | 0x40023880 | Enable USART2 clock |
| GPIOA_MODER | 0x40020000 | Set PA2 to alternate function mode |
| GPIOA_AFRL | 0x40020020 | Select AF7 (USART2) for PA2 |
| USART2_SR | 0x40004400 | Status register — check TXE flag |
| USART2_DR | 0x40004404 | Data register — write byte to send |
| USART2_BRR | 0x40004408 | Baud rate register |
| USART2_CR1 | 0x4000440C | Control register — enable TX and USART |

## How It Works

### Step 1 — Enable Clocks
```c
RCC_AHB1ENR |= (1U << 0);   // GPIOA on AHB1 bus
RCC_APB1ENR |= (1U << 17);  // USART2 on APB1 bus
```
USART2 is on the APB1 bus — different from GPIO which is on
AHB1. Each bus has its own clock enable register in RCC.
Bit 17 of RCC_APB1ENR enables the USART2 clock.

### Step 2 — Configure PA2 as Alternate Function
```c
GPIOA_MODER &= ~(3U << 4);  // clear bits 5:4 (pin 2 × 2 = 4)
GPIOA_MODER |=  (2U << 4);  // set to 10 = alternate function
```
PA2 cannot be used as UART TX while configured as a regular
GPIO output. Mode 10 (value 2) connects the pin to the
alternate function multiplexer inside the chip.

### Step 3 — Select AF7 for PA2
```c
GPIOA_AFRL &= ~(0xF << 8);  // clear 4 bits for PA2 (2 × 4 = 8)
GPIOA_AFRL |=  (7U  << 8);  // write AF7 = USART2_TX
```
Each pin has 16 possible alternate functions (AF0-AF15).
Each uses 4 bits in the AFR register. PA2 uses bits 11:8.
AF7 connects PA2 to USART2 TX — defined in the STM32F407
datasheet alternate function table.

### Step 4 — Configure USART2
```c
USART2_BRR = 139;
```
Baud rate = processor clock / BRR value
16,000,000 / 139 ≈ 115,107 ≈ 115200 baud

```c
USART2_CR1 |= (1U << 3);   // bit 3 = TE (transmitter enable)
USART2_CR1 |= (1U << 13);  // bit 13 = UE (USART enable)
```
Transmitter must be enabled before USART enable.

### Step 5 — Send String
```c
void uart_send_char(char c) {
    while(!(USART2_SR & (1U << 7)));  // wait for TXE flag
    USART2_DR = c;
}
```
TXE (Transmit Data Register Empty) flag in bit 7 of SR goes
HIGH when the data register is ready for the next byte.
Writing to DR loads the byte into the transmit shift register.

```c
void uart_send_string(char *str) {
    while(*str) {
        uart_send_char(*str);
        str++;
    }
}
```
Walks through the string character by character using pointer
arithmetic until the null terminator (value 0) is reached.

## Concepts Demonstrated
- APB1 peripheral clock enable — different bus from GPIO
- GPIO alternate function mode configuration
- AFRL register — selecting which alternate function to use
- USART baud rate calculation from clock frequency
- UART transmit using polling on TXE status flag
- String transmission using pointer arithmetic
- Two-function approach — send char and send string

## New Concept — Alternate Functions

GPIO pins are connected to multiple internal peripherals through
a multiplexer. The alternate function system selects which
peripheral gets control of the pin:

```
PA2 can be:
  Regular GPIO output     → MODER = 01
  USART2 TX (AF7)        → MODER = 10, AFRL = 7
  TIM2 CH3 (AF1)         → MODER = 10, AFRL = 1
  SPI1 (AF5)             → MODER = 10, AFRL = 5
```

Two register writes are always needed — MODER to select
alternate function mode, and AFR to select which function.

## Baud Rate Calculation
```
BRR = f_clock / baud_rate
BRR = 16,000,000 / 115,200
BRR = 138.88 ≈ 139
```

## Difference From Previous Projects
All previous projects used GPIO registers on the AHB1 bus.
This is the first project using a peripheral on the APB1 bus
and the first to use the alternate function system — two new
concepts that apply to every non-GPIO peripheral on STM32.

## Connection to Reference Manual
All register addresses from RM0090:
- RCC_APB1ENR: Section 6.3.12 — USART2EN bit 17
- GPIOA_AFRL: Section 8.4.9 — offset 0x20 from GPIO base
- USART2 base 0x40004400: Memory map table
- USART2_BRR, CR1, SR, DR: Section 19.6 USART register map
- AF7 for PA2: STM32F407 datasheet Table 9 — alternate
  function mapping

## Notes
- This project introduces the two-stage pin configuration
  that all communication peripherals require — MODER + AFR
- USART2 was chosen because its TX pin (PA2) is accessible
  on the STM32F407 Discovery board
- Only TX is configured here — a complete UART implementation
  would also configure PA3 as RX using the same pattern
- The \n in "Hello\n" sends a newline — most terminals also
  need \r for proper line display, use "Hello\r\n" if text
  appears on the same line in your terminal
