# demo-fae-emb-ozone-sim-stm32f407

Hardware-free STM32F407 firmware demo for **SEGGER Ozone-Sim** — run and
regression-test embedded firmware on a *simulated* Cortex-M4, with no board and
no J-Link. Built on the Arm Keil RTX5 Blinky reference project.

> Topics:  `demo` `embedded-systems` `fae` `stm32` `arm-cortex` `keil-mdk` `segger`

## What this demonstrates

[SEGGER Ozone-Sim](https://www.segger.com/products/development-tools/ozone-j-link-debugger/technology/ozone-sim/)
executes a compiled ELF on a simulated Cortex-M core and services its semihosting
I/O — so firmware can be run and self-checked in CI without any hardware. This
repo builds one STM32F407 project two ways:

| Build | Runs on | Purpose |
|-------|---------|---------|
| `Debug` / `Release` | STM32F407G-DISC1 board | Full CMSIS-RTOS2 (Keil RTX5) Blinky — LED + button threads |
| `Sim` | Ozone-Sim (no hardware) | Bare-metal reduction that prints the blink activity over semihosting and exits — a hardware-free CI test |

## Hardware required

**None** for the simulator demo — the `Sim` build runs entirely inside Ozone-Sim.
The board builds target the STMicroelectronics **STM32F407G-DISC1** Discovery kit
if you want to run on real hardware.

## Quick start (< 5 minutes)

Requires only **SEGGER Ozone ≥ V3.50** (it ships the `ozone-sim` executable). A
prebuilt simulator ELF is committed under `prebuilt/`, so no toolchain is needed
just to see it run:

```bash
./run-sim.sh
```

Expected output:

```
GSAS STM32F407 Blinky - running hardware-free in SEGGER Ozone-Sim

cycle  0: LED0 on
cycle  0: LED0 off
...
cycle  4: LED0 off

5 blink cycles complete - ending Ozone-Sim run.
Target application performed semihosting exit with return code 0 (0x00) ...
```

`run-sim.sh` returns the firmware's semihosting exit code (`0` = pass), so it
drops straight into a CI pipeline. The equivalent direct command is:

```bash
ozone-sim --standalone --chip STM32F407VE prebuilt/ozone-sim-blinky-sim.axf
```

## Building from source

This is a CMSIS-Toolbox `csolution` project (Arm Keil MDK). Open it in **VS Code
with the Arm CMSIS Solution extension** (Keil Studio) and build the
`ozone-sim-blinky.Sim+STM32F407G-DISC1` context, or from a CMSIS-Toolbox command line:

```bash
cbuild ozone-sim-blinky.csolution.yml --context ozone-sim-blinky.Sim+STM32F407G-DISC1   # simulator build
cbuild ozone-sim-blinky.csolution.yml --context ozone-sim-blinky.Debug+STM32F407G-DISC1 # board build
```

Tools: CMSIS-Toolbox 2.14, Arm Compiler for Embedded (AC6) 6.24, CMSIS 6,
CMSIS-RTX 5.9.1, STM32F4xx_DFP 3.1.


[docs/ozone-sim-demo.md](docs/ozone-sim-demo.md).

## Board build behaviour (real hardware)

- On start: prints `Blinky example` to the ST-Link UART (115200 bps) and blinks
  `vioLED0` at a 1 s interval.
- `vioBUTTON0` changes the blink rate and starts/stops `vioLED1`.

### CMSIS-Driver Virtual I/O mapping

| CMSIS-Driver VIO | Board component |
|:-----------------|:----------------|
| vioBUTTON0       | USER button (B1) |
| vioLED0          | LED red   (LD5) |
| vioLED1          | LED green (LD4) |

## Partner tools

- **SEGGER Ozone-Sim** (Ozone ≥ V3.50) — the simulator (30-day evaluation; a
  license is required beyond that, including for unattended CI).
- **Arm Keil MDK / CMSIS-Toolbox** — build system and RTX5.
- **Arm Compiler for Embedded (AC6)** — compiler.


---

