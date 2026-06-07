# 07 LED Blink using PLL — Bare Metal STM32F407

## What This Program Does
Configures the STM32F407 Phase Locked Loop (PLL) to run the
processor at its maximum speed of 168MHz using the external
8MHz crystal (HSE) as the clock source. Then blinks the onboard
blue LED at this full speed. This is the most advanced clock
configuration possible on the STM32F407.
No HAL library used.

## Hardware
- Board: STM32F407 Discovery
- LED: LD6 (Blue) — connected to PD15
- External crystal: 8MHz HSE on STM32F407 Discovery board

## Registers Used

| Register | Address | Purpose |
|---|---|---|
| RCC_CR | 0x40023800 | Enable HSE and PLL, check ready flags |
| RCC_PLLCFGR | 0x40023804 | Configure PLL dividers M, N, P |
| RCC_CFGR | 0x40023808 | Select PLL as system clock source |
| FLASH_ACR | 0x40023C00 | Set flash wait states for 168MHz |
| RCC_AHB1ENR | 0x40023830 | Enable GPIOD clock |
| GPIOD_MODER | 0x40020C00 | Configure PD15 as output |
| GPIOD_ODR | 0x40020C14 | Toggle PD15 LED |

## How It Works

### GPIO Setup
```c
RCC_AHB1ENR |= (1U << 3);
GPIOD_MODER &= ~(3U << 30);
GPIOD_MODER |=  (1U << 30);
```
Standard GPIO setup — enable GPIOD clock and configure PD15
as output. Done before clock switching to ensure GPIO works
throughout the configuration process.

### Step 1 — Enable HSE and Wait
```c
RCC_CR |= (1U << 16);               // HSEON bit
while(!(RCC_CR & (1U << 17)));      // wait for HSERDY
```
Bit 16 of RCC_CR enables the High Speed External oscillator
(the 8MHz crystal on the Discovery board).
Bit 17 (HSERDY) is set by hardware when the crystal has
stabilized — the program waits here until it is ready.
Never switch to an unstable clock source.

### Step 2 — Configure PLL Dividers
```c
RCC_PLLCFGR = (8U) | (336U << 6) | (0U << 16) | (1U << 22);
```
Four values packed into one register write:

| Parameter | Value | Bits | Purpose |
|---|---|---|---|
| M | 8 | 5:0 | Divide HSE input: 8MHz ÷ 8 = 1MHz |
| N | 336 | 14:6 | Multiply VCO: 1MHz × 336 = 336MHz |
| P | 0 (÷2) | 17:16 | Divide VCO: 336MHz ÷ 2 = 168MHz |
| Source | 1 (HSE) | 22 | Use HSE not HSI as PLL input |

**PLL calculation:**
```
PLL output = (HSE / M) × N / P
           = (8MHz / 8) × 336 / 2
           = 1MHz × 336 / 2
           = 168MHz
```

### Step 3 — Enable PLL and Wait
```c
RCC_CR |= (1U << 24);               // PLLON bit
while(!(RCC_CR & (1U << 25)));      // wait for PLLRDY
```
Bit 24 enables the PLL. Bit 25 (PLLRDY) is set by hardware
when the PLL has locked to the target frequency. Always wait
for lock before switching the system clock to PLL.

### Step 4 — Set Flash Wait States
```c
FLASH_ACR |= (5U << 0) | (1U << 8);
```
At higher clock speeds the CPU runs faster than the flash
memory can respond. Wait states are inserted to let flash
catch up. At 168MHz, 5 wait states are required per the
STM32F407 datasheet.
Bit 8 enables the prefetch buffer — improves performance
by fetching the next instruction before it is needed.
**This step is critical — skipping it causes unpredictable
behavior or hard faults at high clock speeds.**

### Step 5 — Switch System Clock to PLL
```c
RCC_CFGR |= (2U << 0);
```
Bits 1:0 of RCC_CFGR select the system clock source:
- 00 = HSI (16MHz internal oscillator — default at reset)
- 01 = HSE (8MHz external crystal)
- 10 = PLL output (168MHz)

Writing 2 (binary 10) switches the system clock to PLL.

### Step 6 — Wait for Clock Switch Confirmation
```c
while((RCC_CFGR & (3U << 2)) != (2U << 2));
```
Bits 3:2 of RCC_CFGR are the SWS (System Clock Switch Status)
bits — set by hardware to confirm which clock is actually
running. The program waits until hardware confirms the switch
to PLL is complete before continuing.

---

## The Complete Clock Tree

```
HSE Crystal (8MHz)
       │
       ▼
   ÷ M (÷8)
       │
    1MHz
       │
       ▼
   × N (×336)
       │
   336MHz (VCO)
       │
       ▼
   ÷ P (÷2)
       │
   168MHz ──→ System Clock
```

---

## Why Flash Wait States Matter

```
At  16MHz: 0 wait states needed  (1 cycle = 62.5ns > flash access time)
At  84MHz: 2 wait states needed
At 168MHz: 5 wait states needed  (1 cycle = 5.95ns < flash access time)
```

Without wait states at 168MHz the CPU would fetch invalid
instructions from flash — causing hard faults or completely
wrong program execution.

## Concepts Demonstrated
- Phase Locked Loop (PLL) configuration from scratch
- HSE external oscillator enable and ready polling
- PLL M, N, P divider calculation for target frequency
- Flash wait state configuration for high speed operation
- System clock source switching with confirmation
- RCC clock tree — the complete path from crystal to CPU
- Critical sequencing — order of operations matters

## Correct Configuration Sequence

The order of these steps is mandatory:
```
1. Enable and wait for HSE      ← source must be stable first
2. Configure PLL dividers       ← set before enabling PLL
3. Enable and wait for PLL      ← PLL must lock before switching
4. Set flash wait states        ← must be set before switching
5. Switch clock source to PLL   ← now safe to switch
6. Confirm switch complete      ← verify before continuing
```
Changing this order can cause the system to switch to an
unstable clock — resulting in a hard fault or no operation.

## Difference From Previous Projects
All previous projects ran on the default 16MHz HSI clock.
This project unlocks the full 168MHz capability of the
STM32F407 — the same configuration used in production
embedded systems requiring maximum performance.

## Connection to Reference Manual
All register addresses from RM0090:
- RCC registers: Section 6 — Reset and Clock Control
- RCC_CR: Section 6.3.1 — HSEON bit 16, HSERDY bit 17
- RCC_PLLCFGR: Section 6.3.2 — M, N, P, source fields
- RCC_CFGR: Section 6.3.3 — SW bits 1:0, SWS bits 3:2
- FLASH_ACR: Section 3.4.1 — latency bits 2:0, PRFTEN bit 8
- Flash wait state table: STM32F407 datasheet Table 10

## Notes
- This is the most complex register configuration in this
  repository — it touches the clock tree, flash controller
  and RCC simultaneously
- The PLL values M=8, N=336, P=2 are specifically chosen
  for the 8MHz crystal on the STM32F407 Discovery board
- Any mistake in the PLL configuration or flash wait states
  can lock up the board — if this happens hold the reset
  button while powering on to restart from default clock
- Once running at 168MHz the delay function requires a much
  larger count value to produce the same visible blink period
  compared to the 16MHz projects
