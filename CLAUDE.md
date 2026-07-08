# demo-fae-emb-ozone-sim-stm32f407

## What This Is
A hardware-free STM32F407 firmware demo for **SEGGER Ozone-Sim**. The same Arm
Keil RTX5 Blinky project builds two ways: `Debug`/`Release` run the full
CMSIS-RTOS2 blinky on the STM32F407G-DISC1 board; the `Sim` build is a bare-metal
reduction that runs inside `ozone-sim` (a Cortex-M instruction-set simulator),
prints its blink activity over semihosting, and exits with code 0 — a
hardware-free CI regression test. Used by Field Applications to demo Ozone-Sim.

## Tech Stack
- CMSIS-Toolbox `csolution` project (Arm Keil MDK), built with `cbuild`.
- Arm Compiler for Embedded (AC6) 6.24; CMSIS 6; CMSIS-RTX 5.9.1;
  STM32F4xx_DFP 3.1; CMSIS-Compiler 2.2.
- Runtime: SEGGER Ozone-Sim (bundled with Ozone ≥ V3.50).

## Key Files
- `Blinky.c` — app. `#ifdef OZONE_SIM` = bare-metal semihosting demo; `#else` =
  RTX5 threads (LED + button).
- `retarget_stdio.c` — `OZONE_SIM` = semihosting back-end (`sim_write`,
  `sim_write_u32`, `sim_exit`); `#else` = CMSIS-Driver UART for the board.
- `Blinky.csolution.yml` — defines the `Sim` build-type (`define: OZONE_SIM`).
- `Blinky.cproject.yml` — excludes RTX5 + OS-Tick from the `Sim` context.
- `STM32CubeMX/.../Src/main.c` — `USER CODE BEGIN 1` calls `app_main()` early
  under `OZONE_SIM` to skip HAL/clock bring-up.
- `run-sim.sh` — builds (if `cbuild` present) and runs the ELF in Ozone-Sim.
- `docs/ozone-sim-demo.md` — why the Sim build is bare metal (the full rationale).

## Constraints
- Ozone-Sim models only the CPU core, memory and semihosting — **not** SysTick,
  NVIC, SCB/VTOR, or device peripherals (RCC/FLASH/GPIO/USART). The `Sim` build
  must therefore stay RTOS-free and must not touch peripherals. Do not
  reintroduce RTX, `printf()`, or HAL calls into the `OZONE_SIM` path.
- Keep the board (`Debug`/`Release`) builds byte-for-byte the original RTX5
  Blinky — all simulator changes are guarded by `OZONE_SIM`.
- Sim output uses direct semihosting (not `printf`) on purpose; see docs.
- Repo/branch/commit naming follows gsasindia `onboarding/NAMING-CONVENTIONS.md`.

## Common Tasks
- Build the sim: `cbuild Blinky.csolution.yml --context Blinky.Sim+STM32F407G-DISC1`.
- Run it: `./run-sim.sh` (expects exit code 0).
- Change how many blink cycles run: `SIM_BLINK_CYCLES` in `Blinky.c`.
- Build for the board: `--context Blinky.Debug+STM32F407G-DISC1`.
- Refresh the committed demo binary: copy the built
  `out/Blinky/STM32F407G-DISC1/Sim/Blinky.axf` to `prebuilt/Blinky-sim.axf`.
