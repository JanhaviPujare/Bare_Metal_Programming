# 05 Button Press LED Counter — Bare Metal STM32F407

## What This Program Does
Counts button presses and lights up a different onboard LED for
each press in a cycle of four. Each button press advances to the
next LED — green, orange, red, blue — then cycles back to green.
Includes software debouncing to prevent false triggers from
mechanical button bounce. No HAL library used.

## Hardware
- Board: STM32F407 Discovery
- Button: Onboard user button — PA0 (Port A, Pin 0)
- LED LD4 (Green)  — PD12
- LED LD3 (Orange) — PD13
- LED LD5 (Red)    — PD14
- LED LD6 (Blue)   — PD15

## Registers Used

| Register | Address | Purpose |
|---|---|---|
| RCC_AHB1ENR | 0x40023830 | Enable GPIOA and GPIOD clocks |
| GPIOA_MODER | 0x40020000 | Configure PA0 as input |
| GPIOA_IDR | 0x40020010 | Read button state |
| GPIOD_MODER | 0x40020C00 | Configure PD12-PD15 as outputs |
| GPIOD_ODR | 0x40020C14 | Control which LED is ON |

## How It Works

### Step 1 — Clock and Pin Configuration
```c
RCC_AHB1ENR |= (1U << 0);      // GPIOA clock
RCC_AHB1ENR |= (1U << 3);      // GPIOD clock
GPIOD_MODER &= ~(255U << 24);  // clear pins 12-15
GPIOD_MODER |=  (85U  << 24);  // set pins 12-15 as output
GPIOA_MODER &= ~(3U << 0);     // PA0 as input
```
Same patterns as previous projects — four LEDs configured
simultaneously using 255U and 85U masks.

### Step 2 — Detect a Clean Button Press
```c
while(!(GPIOA_IDR & (1U << 0)));  // wait for press
delay(50000);                      // debounce delay
while(GPIOA_IDR & (1U << 0));     // wait for release
delay(50000);                      // debounce delay
count++;                           // confirmed press
```
Three stages ensure one clean press is counted:
1. Wait until button goes HIGH (pressed)
2. Delay 50000 cycles — lets mechanical bounce settle
3. Wait until button goes LOW (released)
4. Delay again — lets release bounce settle
5. Increment count — one confirmed press registered

### Step 3 — Select LED Based on Count
```c
if     (count % 4 == 1) GPIOD_ODR = (1U << 12);  // green
else if(count % 4 == 2) GPIOD_ODR = (1U << 13);  // orange
else if(count % 4 == 3) GPIOD_ODR = (1U << 14);  // red
else                    GPIOD_ODR = (1U << 15);  // blue
```
Modulo 4 (%) keeps the pattern cycling — press 5 behaves
the same as press 1, press 6 same as press 2, and so on.
Direct ODR assignment ensures only one LED is ON at a time.

## Concepts Demonstrated
- Software debouncing — handling mechanical button bounce
- Press and release detection — not just press detection
- Modulo operator for cycling through states
- Combining input reading and output control
- Multi-LED control based on a counter variable

## What is Button Debounce?

When a physical button is pressed, the metal contacts bounce
against each other several times before settling. Without
debouncing the CPU sees many rapid presses instead of one:

```
Without debounce:   press seen as 5-10 rapid presses
With debounce:      press seen as exactly 1 press
```

The delay after detecting press and release gives the contacts
time to settle before the program continues.

## Difference From Previous Projects
Button LED (project 02) directly mapped button state to LED state.
This project counts discrete press events and uses modulo
arithmetic to cycle through states — a more realistic pattern
for real embedded UI interactions like menu navigation.

## Connection to Reference Manual
All register addresses sourced from STM32F407 Reference Manual
(RM0090):
- RCC_AHB1ENR: Section 6.3.10
- GPIOA base 0x40020000, GPIOD base 0x40020C00: Memory map table
- MODER offset 0x00, IDR offset 0x10, ODR offset 0x14:
  GPIO register map Section 8.4

## Notes
- The debounce delay value (50000) was chosen empirically —
  the exact value depends on the specific button hardware
- A more robust debounce implementation would use a hardware
  timer for a precise millisecond delay
- count is declared as uint32_t — it will roll over after
  4,294,967,295 presses which is not a practical concern
- This project combines concepts from all previous projects:
  clock enable, multi-pin output config, input reading,
  debouncing and state-based LED control
