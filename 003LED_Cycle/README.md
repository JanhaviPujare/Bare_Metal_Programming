# 03 LED Cycle — Bare Metal STM32F407

## What This Program Does
Cycles through all four onboard LEDs on the STM32F407 Discovery
board in sequence — green, orange, red, blue — one at a time
with a delay between each. All four GPIO pins are configured
simultaneously using a single register write. No HAL library used.

## Hardware
- Board: STM32F407 Discovery
- LED LD4 (Green)  — PD12
- LED LD3 (Orange) — PD13
- LED LD5 (Red)    — PD14
- LED LD6 (Blue)   — PD15

## Registers Used

| Register | Address | Purpose |
|---|---|---|
| RCC_AHB1ENR | 0x40023830 | Enable GPIOD peripheral clock |
| GPIOD_MODER | 0x40020C00 | Configure PD12-PD15 as outputs |
| GPIOD_ODR | 0x40020C14 | Control which LED is ON |

## How It Works

### Step 1 — Enable GPIOD Clock
```c
RCC_AHB1ENR |= (1U << 3);
```
Bit 3 enables the GPIOD peripheral clock.

### Step 2 — Configure PD12–PD15 as Output
```c
GPIOD_MODER &= ~(255U << 24);  // clear 8 bits for pins 12-15
GPIOD_MODER |=  (85U  << 24);  // set all 4 pins to output
```
Each pin uses 2 bits in MODER. Four pins = 8 bits total.

Pin 12 starts at bit 24 (12 × 2 = 24).

**Why 255U to clear?**
255 in binary = `11111111` — eight 1s covering bits 31:24
which are exactly the 8 bits for pins 12, 13, 14 and 15.

**Why 85U to set output?**
85 in binary = `01010101` — sets every alternate bit to 1.
This places `01` (output mode) in each of the four 2-bit
slots for pins 12, 13, 14 and 15 simultaneously:
Bits 31:24 = 01 01 01 01
P15 P14 P13 P12

### Step 3 — Cycle LEDs in Loop
```c
GPIOD_ODR = (1U << 12);  // only PD12 ON
delay(500000);
GPIOD_ODR = (1U << 13);  // only PD13 ON
delay(500000);
GPIOD_ODR = (1U << 14);  // only PD14 ON
delay(500000);
GPIOD_ODR = (1U << 15);  // only PD15 ON
delay(500000);
```
Direct assignment to ODR sets exactly one bit at a time —
all other pins go LOW automatically. This is cleaner than
using SET/CLEAR for a chase pattern.

## Concepts Demonstrated
- Configuring multiple pins simultaneously with one register write
- Understanding 255U and 85U as multi-bit masks
- Direct ODR assignment vs SET/CLEAR bit operations
- Chase/sequence pattern using GPIO
- Multi-LED control on a single port

## Key Insight — 255U and 85U Explained
255U = 11111111  → clears all 8 bits for pins 12-15
85U = 01010101  → sets output mode for all 4 pins at once

This is more efficient than configuring each pin separately.
Real embedded drivers use this pattern to initialize entire
ports in a single register write.

## Difference From Previous Projects
LED Blink toggled one pin. This project controls four pins
simultaneously using multi-bit masking — a more realistic
pattern for real embedded GPIO initialization code.

## Connection to Reference Manual
All register addresses sourced from STM32F407 Reference Manual
(RM0090):
- RCC_AHB1ENR: Section 6.3.10
- GPIOD base address 0x40020C00: Memory map table
- MODER offset 0x00, ODR offset 0x14: GPIO register map Section 8.4

## Notes
- The four onboard LEDs on STM32F407 Discovery are all on
  Port D pins 12-15 — no external components needed
- Using direct ODR assignment instead of XOR toggle means
  only one LED is ever ON at a time — no residual state
  from the previous iteration carries over
- 85 = 64 + 16 + 4 + 1 = 2^6 + 2^4 + 2^2 + 2^0
  which places 1s at every even bit position in the byte
