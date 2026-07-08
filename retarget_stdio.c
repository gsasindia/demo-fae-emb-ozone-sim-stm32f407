/*---------------------------------------------------------------------------
 * Copyright (c) 2024 Arm Limited (or its affiliates).
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
 *
 *      Name:    retarget_stdio.c
 *      Purpose: Retarget stdio to a CMSIS-Driver UART (real hardware) or to
 *               Arm semihosting (SEGGER Ozone-Sim, no peripherals).
 *
 * Two back-ends selected at build time:
 *
 *   OZONE_SIM defined   -> stdout/stderr go to Arm semihosting so that
 *                          printf()/puts() appear in the Ozone-Sim terminal
 *                          without any UART. Also provides sim_exit(), which
 *                          ends the simulation via the semihosting exit call.
 *   OZONE_SIM undefined  -> stdout/stderr go to a CMSIS-Driver UART, exactly
 *                          as on the STM32F407G-DISC1 board (unchanged).
 *---------------------------------------------------------------------------*/

#include <stdint.h>

/* Exported functions (identical signatures for both back-ends) */
extern int stdio_init     (void);
extern int stderr_putchar (int ch);
extern int stdout_putchar (int ch);
extern int stdin_getchar  (void);

#ifdef OZONE_SIM
/*===========================================================================
 * Semihosting back-end - used when running inside SEGGER Ozone-Sim.
 *
 * Arm semihosting is a debugger-hosted service: the target issues "BKPT 0xAB"
 * with an operation number in r0 and an argument pointer in r1, and the
 * simulator (acting as the host) performs the I/O. No peripherals required.
 *===========================================================================*/

/* Arm semihosting operation numbers (see "Arm semihosting" spec). */
#define SEMIHOST_SYS_WRITEC        0x03U   /* r1 -> single character to write   */
#define SEMIHOST_SYS_WRITE0        0x04U   /* r1 -> null-terminated string      */
#define SEMIHOST_SYS_EXIT          0x18U   /* r1  = reason code                 */
#define ADP_STOPPED_APPLICATIONEXIT 0x20026U

/* Issue one semihosting call and return the host's result. */
static int semihost_call (int op, void *arg) {
  register int          r0 __asm__("r0") = op;
  register void * const r1 __asm__("r1") = arg;
  __asm__ volatile ("bkpt 0xAB" : "+r"(r0) : "r"(r1) : "memory");
  return r0;
}

/* No hardware to bring up in the simulator. */
int stdio_init (void) {
  return 0;
}

/* Write one character to the host terminal via semihosting. */
int stdout_putchar (int ch) {
  char c = (char)ch;
  semihost_call(SEMIHOST_SYS_WRITEC, &c);
  return ch;
}

int stderr_putchar (int ch) {
  return stdout_putchar(ch);
}

/* No console input under Ozone-Sim. */
int stdin_getchar (void) {
  return -1;
}

/* Write a null-terminated string straight to the host terminal.
   Note: the simulator build uses these direct semihosting writes instead of
   printf(). Because RTX5 is linked, the C library routes printf()'s stdout lock
   through an RTX mutex, and RTX enters the kernel with an SVC - which Ozone-Sim
   cannot dispatch (it models the core + memory + semihosting, not SysTick/NVIC/
   SCB). Going straight to semihosting keeps the demo free of the RTOS. */
void sim_write (const char *s) {
  semihost_call(SEMIHOST_SYS_WRITE0, (void *)s);
}

/* Write an unsigned value in decimal (no C library / printf involved). */
void sim_write_u32 (uint32_t value) {
  char buf[11];
  unsigned i = sizeof(buf) - 1U;
  buf[i] = '\0';
  do {
    buf[--i] = (char)('0' + (value % 10U));
    value /= 10U;
  } while (value != 0U);
  sim_write(&buf[i]);
}

/* End the simulation via the semihosting exit call. Ozone-Sim reports the
   application exit and stops with return code 0. (This demo only ever exits
   successfully; `code` is accepted for API symmetry.) */
void sim_exit (int code) {
  (void)code;
  semihost_call(SEMIHOST_SYS_EXIT, (void *)ADP_STOPPED_APPLICATIONEXIT);
  for (;;) { }   /* not reached */
}

#else
/*===========================================================================
 * CMSIS-Driver UART back-end - used on the STM32F407G-DISC1 board.
 *===========================================================================*/

#ifdef   CMSIS_target_header
#include CMSIS_target_header
#else
#include "Driver_USART.h"
#endif

#ifndef RETARGET_STDIO_UART
#error "RETARGET_STDIO_UART not defined!"
#endif

// Compile-time configuration
#define UART_BAUDRATE     115200

#ifndef CMSIS_target_header
extern ARM_DRIVER_USART   ARM_Driver_USART_(RETARGET_STDIO_UART);
#endif

#define ptrUSART          (&ARM_Driver_USART_(RETARGET_STDIO_UART))

/**
  Initialize stdio

  \return          0 on success, or -1 on error.
*/
int stdio_init (void) {

  if (ptrUSART->Initialize(NULL) != ARM_DRIVER_OK) {
    return -1;
  }

  if (ptrUSART->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
    return -1;
  }

  if (ptrUSART->Control(ARM_USART_MODE_ASYNCHRONOUS |
                        ARM_USART_DATA_BITS_8       |
                        ARM_USART_PARITY_NONE       |
                        ARM_USART_STOP_BITS_1       |
                        ARM_USART_FLOW_CONTROL_NONE,
                        UART_BAUDRATE) != ARM_DRIVER_OK) {
    return -1;
  }

  if (ptrUSART->Control(ARM_USART_CONTROL_RX, 1U) != ARM_DRIVER_OK) {
    return -1;
  }

  if (ptrUSART->Control(ARM_USART_CONTROL_TX, 1U) != ARM_DRIVER_OK) {
    return -1;
  }

  return 0;
}

/**
  Put a character to the stderr

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stderr_putchar (int ch) {
  uint8_t buf[1];

  buf[0] = (uint8_t)ch;

  if (ptrUSART->Send(buf, 1U) != ARM_DRIVER_OK) {
    return -1;
  }

  while (ptrUSART->GetStatus().tx_busy != 0U);

  return ch;
}

/**
  Put a character to the stdout

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stdout_putchar (int ch) {
  uint8_t buf[1];

  buf[0] = (uint8_t)ch;

  if (ptrUSART->Send(buf, 1U) != ARM_DRIVER_OK) {
    return -1;
  }

  while (ptrUSART->GetStatus().tx_busy != 0U);

  return ch;
}

/**
  Get a character from the stdio

  \return     The next character from the input, or -1 on read error.
*/
int stdin_getchar (void) {
  uint8_t buf[1];

  if (ptrUSART->Receive(buf, 1U) != ARM_DRIVER_OK) {
    return -1;
  }

  while (ptrUSART->GetStatus().rx_busy != 0U);

  return (int)buf[0];
}

#endif /* OZONE_SIM */
