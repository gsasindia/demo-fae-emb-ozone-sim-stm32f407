/*---------------------------------------------------------------------------
 * Copyright (c) 2024-2025 Arm Limited (or its affiliates).
 * All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

/*
  This application has two build variants that share app_main():

    * Board builds (Debug / Release) run the full CMSIS-RTOS2 (Keil RTX5)
      Blinky: an LED thread and a Button thread on the STM32F407G-DISC1.

    * The Sim build (OZONE_SIM) runs a bare-metal reduction of the same blink
      narrative for the SEGGER Ozone-Sim instruction-set simulator. Ozone-Sim
      models the Cortex-M4 core, memory and semihosting, but NOT the system
      peripherals an RTOS needs (SysTick for the kernel tick, SVC/PendSV
      vectoring via the SCB/NVIC). So the Sim build links no RTOS at all (see
      Blinky.cproject.yml) and simply prints the blink activity over semihosting
      and exits - which is exactly what makes it a clean, hardware-free CI test.
*/

#include <stdio.h>
#include "main.h"

#ifdef OZONE_SIM
/*===========================================================================
 * Simulator build - bare metal, semihosting, no RTOS.
 *===========================================================================*/

/* Direct semihosting output helpers (implemented in retarget_stdio.c). These
   are used instead of printf() so the demo never touches the C library's
   thread-safe stdout lock, which would otherwise call into RTX. */
extern void sim_write     (const char *s);
extern void sim_write_u32 (uint32_t value);
extern void sim_exit      (int code);

/* Number of blink cycles to run before ending the simulation. */
#define SIM_BLINK_CYCLES   5U

int app_main (void) {
  sim_write("\nGSAS STM32F407 Blinky - running hardware-free in SEGGER Ozone-Sim\n\n");

  for (uint32_t cycle = 0U; cycle < SIM_BLINK_CYCLES; cycle++) {
    sim_write("cycle "); sim_write_u32(cycle); sim_write(": LED0 on\n");
    sim_write("cycle "); sim_write_u32(cycle); sim_write(": LED0 off\n");
  }

  sim_write("\n");
  sim_write_u32(SIM_BLINK_CYCLES);
  sim_write(" blink cycles complete - ending Ozone-Sim run.\n");

  sim_exit(0);                                  // Semihosting exit, code 0
  return 0;                                     // Not reached
}

#else
/*===========================================================================
 * Board build - CMSIS-RTOS2 (Keil RTX5) Blinky on STM32F407G-DISC1.
 *===========================================================================*/

#include "cmsis_os2.h"
#include "cmsis_vio.h"

/* Thread attributes for the app_main thread */
static const osThreadAttr_t thread_attr_main = { .name = "app_main" };

/* Thread attributes for the LED thread */
static const osThreadAttr_t thread_attr_LED = { .name = "LED" };

/* Thread attributes for the Button thread */
static const osThreadAttr_t thread_attr_Button = { .name = "Button" };

/* Thread ID for the LED thread */
static osThreadId_t tid_LED;

/* Thread ID for the Button thread */
static osThreadId_t tid_Button;

/*
  Thread that blinks LED.
*/
static __NO_RETURN void thread_LED (void *argument) {
  uint32_t active_flag = 0U;

  (void)argument;

  for (;;) {
    if (osThreadFlagsWait(1U, osFlagsWaitAny, 0U) == 1U) {
      active_flag ^= 1U;
    }

    if (active_flag == 1U) {
      vioSetSignal(vioLED0, vioLEDoff);         // Switch LED0 off
      vioSetSignal(vioLED1, vioLEDon);          // Switch LED1 on
      osDelay(100U);                            // Delay 100 ms
      vioSetSignal(vioLED0, vioLEDon);          // Switch LED0 on
      vioSetSignal(vioLED1, vioLEDoff);         // Switch LED1 off
      osDelay(100U);                            // Delay 100 ms
    }
    else {
      vioSetSignal(vioLED0, vioLEDon);          // Switch LED0 on
      osDelay(500U);                            // Delay 500 ms
      vioSetSignal(vioLED0, vioLEDoff);         // Switch LED0 off
      osDelay(500U);                            // Delay 500 ms
    }
  }
}

/*
  Thread that checks Button state.
*/
static __NO_RETURN void thread_Button (void *argument) {
  uint32_t last = 0U;
  uint32_t state;

  (void)argument;

  for (;;) {
    state = (vioGetSignal(vioBUTTON0));         // Get pressed Button state
    if (state != last) {
      if (state == 1U) {
        osThreadFlagsSet(tid_LED, 1U);          // Set flag to thread_LED
      }
      last = state;
    }
    osDelay(100U);
  }
}

/*
  Application main thread.
*/
__NO_RETURN void app_main_thread (void *argument) {
  (void)argument;

  printf("Blinky example\n");

  /* Create LED and Button threads */
  tid_LED = osThreadNew(thread_LED, NULL, &thread_attr_LED);
  tid_Button = osThreadNew(thread_Button, NULL, &thread_attr_Button);

  for (;;) {
    /* Delay indefinitely */
    osDelay(osWaitForever);
  }
}

/*
  Application initialization.
*/
int app_main (void) {
  osKernelInitialize();                         // Initialize CMSIS-RTOS2
  osThreadNew(app_main_thread, NULL, &thread_attr_main);
  osKernelStart();                              // Start thread execution
  return 0;
}

#endif /* OZONE_SIM */
