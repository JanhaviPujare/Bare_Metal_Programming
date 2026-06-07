# 02 Button Press LED — Bare Metal STM32F407

## What This Program Does
Turns the onboard blue LED (LD6) on when the onboard user button
is pressed and off when released. Uses direct register manipulation
on two GPIO ports simultaneously — GPIOA for button input and
GPIOD for LED output. No HAL library used.

## Hardware
- Board: STM32F407 Discovery
- LED: LD6 (Blue) — connected to PD15 (Port D, Pin 15)
- Button: Onboard user button — connected to PA0 (Port A, Pin 0)

## Registers Used

| Register | Address | Purpose |
|---|---|---|
| RCC_AHB1ENR | 0x40023830 | Enable GPIOA and GPIOD clocks |
| GPIOA_MODER | 0x40020000 | Configure PA0 as input |
| GPIOA_IDR | 0x40020010 | Read PA0 button state |
| GPIOD_MODER | 0x40020C00 | Configure PD15 as output |
| GPIOD_ODR | 0x40020C14 | Control PD15 LED state |

## How It Works

### Step 1 — Enable Clocks for Both Ports
```c
RCC_AHB1ENR |= (1U << 0);   // bit 0 = GPIOA clock
RCC_AHB1ENR |= (1U << 3);   // bit 3 = GPIOD clock
```
Both peripherals need their clocks enabled before their
registers can be accessed.

### Step 2 — Configure PA0 as Input
```c
GPIOA_MODER &= ~(3U << 0);  // clear bits 1:0 = 00 = input
```
Pin 0 uses bits 1:0 of MODER (0 × 2 = 0).
Mode 00 = input — the reset default, but explicitly set for clarity.

### Step 3 — Configure PD15 as Output
```c
GPIOD_MODER &= ~(3U << 30); // clear bits 31:30
GPIOD_MODER |=  (1U << 30); // set to 01 = output
```
Same pattern as LED Blink — pin 15 uses bits 31:30.

### Step 4 — Poll Button and Control LED
```c
if(GPIOA_IDR & (1U << 0)) {
    GPIOD_ODR |=  (1U << 15); // LED ON
} else {
    GPIOD_ODR &= ~(1U << 15); // LED OFF
}
```
Continuously reads bit 0 of GPIOA_IDR — the input data register.
When the button is pressed PA0 goes HIGH — bit 0 becomes 1.
LED is set HIGH or LOW in ODR to match the button state.

## Concepts Demonstrated
- Configuring two GPIO ports simultaneously
- GPIO input configuration using MODER
- Reading pin state using IDR (Input Data Register)
- Polling — continuously checking input state in a loop
- Controlling output based on input state
- Enabling multiple peripheral clocks in one register

## Difference From Previous Project
LED Blink used only ODR (output). This project introduces IDR
(input) — reading the physical state of a pin from the outside
world. Two registers, two ports, two peripherals working together
for the first time.

## Connection to Reference Manual
All register addresses sourced from STM32F407 Reference Manual
(RM0090):
- RCC_AHB1ENR: Section 6.3.10
- GPIOA base address 0x40020000: Memory map table
- GPIOD base address 0x40020C00: Memory map table
- MODER offset 0x00, IDR offset 0x10, ODR offset 0x14:
  GPIO register map table Section 8.4

## Notes
- PA0 is the onboard user button on STM32F407 Discovery —
  no external components needed
- This program uses polling — the CPU constantly checks the
  button state in the while loop. A more efficient approach
  uses interrupts which will be covered in later projects
- No delay is needed here since the LED directly follows
  the button state in real time
