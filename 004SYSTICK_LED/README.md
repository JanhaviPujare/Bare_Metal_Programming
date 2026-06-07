# 04 SysTick LED Blink — Bare Metal STM32F407

## What This Program Does
Blinks the onboard blue LED (LD6) using the Cortex-M4 SysTick
timer instead of a busy-wait delay loop. The SysTick timer counts
down from a preset value and sets a flag when it reaches zero —
the program waits for this flag before toggling the LED.
No HAL library used.

## Hardware
- Board: STM32F407 Discovery
- LED: LD6 (Blue) — connected to PD15 (Port D, Pin 15)

## Registers Used

| Register | Address | Purpose |
|---|---|---|
| RCC_AHB1ENR | 0x40023830 | Enable GPIOD peripheral clock |
| GPIOD_MODER | 0x40020C00 | Configure PD15 as output |
| GPIOD_ODR | 0x40020C14 | Toggle PD15 LED state |
| SYST_CSR | 0xE000E010 | SysTick Control and Status Register |
| SYST_RVR | 0xE000E014 | SysTick Reload Value Register |
| SYST_CVR | 0xE000E018 | SysTick Current Value Register |

## How It Works

### Step 1 — GPIO Setup
```c
RCC_AHB1ENR |= (1U << 3);
GPIOD_MODER &= ~(3U << 30);
GPIOD_MODER |=  (1U << 30);
```
Same as previous projects — enable GPIOD clock and configure
PD15 as output.

### Step 2 — Configure SysTick Timer
```c
SYST_RVR = 8000000 - 1;
```
Sets the reload value — the number the timer counts down from.
At 16MHz each tick is 1/16,000,000 seconds.
8,000,000 ticks × (1/16,000,000 seconds) = 0.5 seconds = 500ms.
Subtract 1 because the counter counts from RVR down to 0
inclusive — so 8,000,000 - 1 gives exactly 8,000,000 ticks.

```c
SYST_CVR = 0;
```
Clears the current value register — resets the counter to start
fresh from the reload value.

```c
SYST_CSR = (1U << 0) | (1U << 2);
```
Two bits in the Control and Status Register:
- Bit 0 (ENABLE) = 1 → starts the timer counting
- Bit 2 (CLKSOURCE) = 1 → use processor clock (16MHz)

### Step 3 — Wait for Timer and Toggle LED
```c
while(!(SYST_CSR & (1U << 16)));
GPIOD_ODR ^= (1U << 15);
```
Bit 16 of SYST_CSR is the COUNTFLAG — it is automatically set
to 1 by hardware when the counter reaches zero. The program
polls this flag, waiting until it goes HIGH, then toggles the LED.
Reading SYST_CSR automatically clears COUNTFLAG back to 0.

## Concepts Demonstrated
- SysTick timer — the Cortex-M system timer built into every
  ARM Cortex-M processor core
- Timer-based delays vs busy-wait delays
- Hardware status flag polling
- SYST_CSR, SYST_RVR, SYST_CVR register configuration
- Cortex-M core peripheral registers (0xE000E000 address range)

## SysTick vs Busy-Wait Delay

| | Busy-Wait Delay | SysTick Timer |
|---|---|---|
| Accuracy | Varies with optimization | Precise — hardware counted |
| CPU usage | 100% — stuck in loop | Still 100% polling here |
| Portability | Changes with clock speed | Predictable with correct RVR |
| Real use | Quick testing only | Foundation for real timing |

## Why SysTick Registers Are at 0xE000Exxx

GPIO registers live in the peripheral address space (0x40000000).
SysTick lives at 0xE000E000 — this is the **Cortex-M system
control space**, a fixed address range defined by ARM for all
Cortex-M processors regardless of manufacturer. SysTick works
identically on STM32, NXP, TI, and every other Cortex-M chip.

## Connection to Reference Manual
SysTick registers are defined in the ARM Cortex-M4 Technical
Reference Manual, not RM0090 — because SysTick is part of the
ARM core, not the STM32 peripherals:
- SYST_CSR: Section 4.4.1
- SYST_RVR: Section 4.4.2
- SYST_CVR: Section 4.4.3

GPIO registers from RM0090 as before.

## Notes
- This is the first project using a Cortex-M core peripheral
  rather than an STM32-specific peripheral — SysTick exists
  on every ARM Cortex-M microcontroller
- The next step beyond polling COUNTFLAG is enabling the
  SysTick interrupt (bit 1 of SYST_CSR) so the CPU can do
  other work while waiting — this is how RTOS tick timers work
- 8000000 - 1 is written explicitly rather than 7999999
  to make the timing calculation visible in the code
