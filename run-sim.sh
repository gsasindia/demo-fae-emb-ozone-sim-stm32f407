#!/usr/bin/env bash
#
# run-sim.sh - Build (if possible) and run the STM32F407 Blinky in SEGGER Ozone-Sim.
#
# Ozone-Sim executes the firmware on a simulated Cortex-M4 with no hardware and
# no debug probe. The "Sim" build is bare metal: it prints its blink activity
# over semihosting and ends with a semihosting exit, so this script returns the
# firmware's exit code (0 on success) - ready to drop into CI.
#
# Usage:   ./run-sim.sh
# Env:     OZONE_SIM_BIN   override the path to the ozone-sim executable
#
set -euo pipefail

cd "$(dirname "$0")"

CTX="Blinky.Sim+STM32F407G-DISC1"
CHIP="STM32F407VE"     # The DISC1 MCU is an STM32F407VG. Ozone-Sim's device
                       # database ships the pin-compatible STM32F407VE - same
                       # Cortex-M4 core, and this small image fits its memory.

ELF_BUILT="out/Blinky/STM32F407G-DISC1/Sim/Blinky.axf"
ELF_PREBUILT="prebuilt/Blinky-sim.axf"

# 1. Build with the CMSIS-Toolbox if it is on PATH (e.g. after 'vcpkg activate'
#    or inside the Arm Keil Studio environment). Otherwise fall back to a
#    previously built or the committed prebuilt ELF.
if command -v cbuild >/dev/null 2>&1; then
  echo ">> Building $CTX ..."
  cbuild Blinky.csolution.yml --context "$CTX"
fi

if   [[ -f "$ELF_BUILT"    ]]; then ELF="$ELF_BUILT"
elif [[ -f "$ELF_PREBUILT" ]]; then ELF="$ELF_PREBUILT"
else
  echo "ERROR: no ELF found. Build the '$CTX' context first (open the solution" >&2
  echo "       in VS Code / Keil Studio, or run 'cbuild' in an activated"       >&2
  echo "       CMSIS-Toolbox environment)."                                     >&2
  exit 1
fi

# 2. Locate the ozone-sim executable (ships inside SEGGER Ozone >= V3.50).
OZONE_SIM="${OZONE_SIM_BIN:-}"
if [[ -z "$OZONE_SIM" ]]; then
  if command -v ozone-sim >/dev/null 2>&1; then
    OZONE_SIM="$(command -v ozone-sim)"
  else
    OZONE_SIM="$(ls -d /Applications/SEGGER/Ozone*/Ozone.app/Contents/MacOS/ozone-sim \
                       /opt/SEGGER/ozone*/ozone-sim 2>/dev/null | sort -r | head -1 || true)"
  fi
fi
if [[ -z "$OZONE_SIM" || ! -x "$OZONE_SIM" ]]; then
  echo "ERROR: ozone-sim not found. Install SEGGER Ozone (>= V3.50) or set"      >&2
  echo "       OZONE_SIM_BIN to the ozone-sim executable."                       >&2
  exit 1
fi

# 3. Run headless. exec so the simulator's exit code becomes this script's.
echo ">> Running $ELF in Ozone-Sim ($CHIP) ..."
exec "$OZONE_SIM" --standalone --chip "$CHIP" "$ELF"
