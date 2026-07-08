# Ozone-Sim demo — engineering notes

How the STM32F407 Blinky was adapted to run inside SEGGER Ozone-Sim, and why the
simulator build is shaped the way it is. Useful when adapting other firmware for
Ozone-Sim.

## What Ozone-Sim models — and what it does not

`ozone-sim` (bundled with SEGGER Ozone ≥ V3.50) is an instruction-set simulator
for the Cortex-M core. In `--standalone` mode it loads an ELF, runs it, services
**Arm semihosting**, and stops when the application makes a semihosting `SYS_EXIT`
call (or when an exception occurs).

It models:

- the Cortex-M4 instruction set and general-purpose registers,
- the memory map given by `--chip` (or `--mem`),
- Arm semihosting (`SYS_WRITEC`, `SYS_WRITE0`, `SYS_EXIT`, …).

It does **not** model the Cortex-M system peripherals or device peripherals:

- **SysTick** (`0xE000E010`) — not mapped,
- **SCB / VTOR / NVIC** (`0xE000ED00`, …) — not mapped (register writes are
  dropped, reads fault),
- device peripherals such as **RCC / FLASH / GPIO / USART** (`0x4002xxxx`) — not
  mapped (accesses raise "Illegal memory read").

That is by design: Ozone-Sim targets bare-metal, computation-plus-semihosting
regression tests (see SEGGER's own CortexM regression example).

## Consequence: the simulator build must be bare metal

A stock CMSIS/HAL/RTX firmware cannot run in Ozone-Sim, for three independent
reasons discovered while bringing this up (each was a distinct failure):

1. **RTX5 needs SysTick and exception vectoring.** RTX enters the kernel with an
   `SVC`, and its tick is driven by SysTick. Neither is modelled, so the first
   `osKernelInitialize()` `SVC` vectors to a null handler → *"Undefined
   instruction 00000000"*. Setting `VTOR` doesn't help — the SCB isn't modelled.
2. **The C library pulls RTX in.** Because RTX5 is linked, the Arm C library
   routes `printf()`'s stdout lock through an RTX mutex, which also uses an `SVC`.
   The mutex is even initialised during C-library start-up, *before* `main()`.
3. **HAL bring-up touches peripherals.** `HAL_Init()` reads `FLASH->ACR`
   (`0x40023C00`) and `SystemClock_Config()` waits on RCC — both unmapped →
   *"Illegal memory read"*.

So the `Sim` build is deliberately RTOS-free and skips hardware bring-up.

## How the `Sim` build is put together

Selected by the `OZONE_SIM` define (set by the `Sim` build-type in
`Blinky.csolution.yml`):

- **No RTOS.** `Blinky.cproject.yml` excludes `CMSIS:RTOS2:Keil RTX5&Source` and
  `CMSIS:OS Tick:SysTick` from the `Sim` context (`not-for-context: .Sim`). With
  RTX unlinked, the C library falls back to its single-threaded (no-`SVC`) locks.
- **Skip hardware bring-up.** The CubeMX `main()` calls `app_main()` from its
  `USER CODE BEGIN 1` region — *before* `HAL_Init()` / `SystemClock_Config()` —
  when `OZONE_SIM` is defined. `app_main()` runs the demo and never returns, so
  the peripheral init below it never executes. (`USER CODE` regions survive
  CubeMX regeneration.)
- **Bare-metal blink + semihosting output.** `Blinky.c`'s `OZONE_SIM` path prints
  the blink cycles and exits. Output uses **direct semihosting** writes
  (`SYS_WRITE0`) rather than `printf()`, to stay off the C library entirely
  (`retarget_stdio.c`).
- **Clean exit.** `sim_exit()` issues semihosting `SYS_EXIT` with
  `ADP_Stopped_ApplicationExit`, which Ozone-Sim reports as *"semihosting exit
  with return code 0"* and turns into process exit code 0 — the CI gate.

The board builds (`Debug`, `Release`) are untouched: full RTX5 Blinky over UART.

## Running

```bash
ozone-sim --standalone --chip STM32F407VE prebuilt/Blinky-sim.axf
# or:
./run-sim.sh
```

- **Chip:** the DISC1 MCU is an `STM32F407VG`, which is not in Ozone-Sim's device
  database; the pin-compatible `STM32F407VE` is used (same Cortex-M4 core, and the
  ~26 KB image fits its memory).
- The `WARNING: Symbol '__stack_end__' not found` line is harmless — Ozone-Sim
  takes the initial stack pointer from the vector table instead.

## Known limits

- Only the bare-metal `Sim` build runs in Ozone-Sim. Running the RTX5 build there
  is not possible (no SysTick/NVIC/SCB) and is not a goal.
- Ozone-Sim is a licensed product with a 30-day evaluation; unattended CI needs a
  license.
