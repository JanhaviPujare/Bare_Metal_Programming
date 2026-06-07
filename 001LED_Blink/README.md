# 01 LED Blink — Bare Metal STM32F407

## What This Program Does
Blinks the onboard blue LED (LD6) on the STM32F407 Discovery board
by toggling GPIO pin PD15 using direct register manipulation.
No HAL library is used — all hardware is configured through
direct register writes using addresses from the RM0090 reference manual.

## Hardware
- Board: STM32F407 Discovery
- LED: LD6 (Blue) — connected to PD15 (Port D, Pin 15)

## Registers Used

| Register | Address | Purpose |
|---|---|---|
| RCC_AHB1ENR | 0x40023830 | Enable GPIOD peripheral clock |
| GPIOD_MODER | 0x40020C00 | Configure PD15 as output |
| GPIOD_ODR | 0x40020C14 | Toggle PD15 output state |

## How It Works

### Step 1 — Enable GPIOD Clock
```c
RCC_AHB1ENR |= (1U << 3);
```
Bit 3 of RCC_AHB1ENR enables the GPIOD peripheral clock.
On STM32, every peripheral clock is disabled by default to save
power — this must be done before accessing any GPIO register.

### Step 2 — Configure PD15 as Output
```c
GPIOD_MODER &= ~(3U << 30);  // clear bits 31:30
GPIOD_MODER |=  (1U << 30);  // set to 01 = output
```
MODER uses 2 bits per pin. Pin 15 uses bits 31:30 (15 × 2 = 30).
Clearing first then setting to 01 configures push-pull output.

### Step 3 — Toggle LED in Loop
```c
GPIOD_ODR ^= (1U << 15);
```
XOR toggles bit 15 of ODR — if HIGH goes LOW, if LOW goes HIGH.
Combined with a delay this creates a visible blink.

## Concepts Demonstrated
- RCC clock enable before peripheral use
- GPIO mode configuration using MODER register
- Bitwise operations on hardware registers — AND, OR, XOR
- Volatile pointers to memory-mapped peripheral registers
- Busy-wait delay using a decrementing counter

## Connection to Reference Manual
All register addresses sourced from STM32F407 Reference Manual
(RM0090):
- RCC_AHB1ENR: Section 6.3.10 — offset 0x30 from RCC base 0x40023800
- GPIOD_MODER: Section 8.4.1 — offset 0x00 from GPIOD base 0x40020C00
- GPIOD_ODR: Section 8.4.6 — offset 0x14 from GPIOD base 0x40020C00

## Notes
- This is the foundational bare metal program — every subsequent
  project builds on the same three steps: enable clock, configure
  mode, read or write data
- The delay function uses a volatile counter to prevent the
  compiler from optimizing the loop away
- PD15 was chosen because it connects to the onboard blue LED
  requiring no external components
