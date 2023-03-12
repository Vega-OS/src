/*
 * Copyright (c) 2023 Ian Marco Moffett and the VegaOS team.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of VegaOS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#include <sys/pal.h>
#include <sys/printk.h>
#include <sys/module.h>
#include <sys/types.h>
#include <mm/pmm.h>
#include <firmware/acpi/core.h>

#if defined(__x86_64__)
# include <amd64/exceptions.h>
# include <amd64/idt.h>
#elif defined(__aarch64__)
# include <aarch64/exceptions.h>
# include <aarch64/mm/mmu.h>
#endif

MODULE("kinit");

#if defined(__aarch64__)
static const char *get_vendor(uint32_t midr_el1)
{
  switch ((midr_el1 >> 24) & 0xFF)
  {
    case 0xC0:
      return "Ampere Computing";
    case 0x41:
      return "Arm Limited";
    case 0x42:
      return "Broadcom Corporation";
    case 0x43:
      return "Cavium Inc";
    case 0x44:
      return "Digital Equipment Corporation";
    case 0x46:
      return "Fujitsu Ltd";
    case 0x49:
      return "Infineon Technologies AG";
    case 0x4D:
      return "Motorola or Freescale Semiconductor Inc";
    case 0x4E:
      return "NVIDIA Corporation";
    case 0x50:
      return "Applied Micro Circuits Corporation";
    case 0x51:
      return "Qualcomm Inc";
    case 0x56:
      return "Marvell International Ltd";
    case 0x69:
      return "Intel Corporation";
    default:
      return "Unrecognized vendor";
  }
}

static void get_board_info(void)
{
  uint32_t midr_el1;
  __asm("mrs %x0, midr_el1" : "=r" (midr_el1));

  switch ((midr_el1 >> 4) & 0xFFF)
  {
    case 0xB76:           /* MMIO: 0x20000000 */
      kinfo("Detected board: Raspberry Pi 1\n");
      break;
    case 0xC07:           /* MMIO: 0x3F000000 */
      kinfo("Detected board: Raspberry Pi 2\n");
      break;
    case 0xD03:           /* MMIO: 0x3F000000 */
      kinfo("Detected board: Raspberry Pi 3\n");
      break;
    case 0xD08:            /* MMIO: 0xFE000000 */
      kinfo("Detected board: Raspberry Pi 4\n");
      break;
    default:               /* MMIO: 0x20000000 */
      kinfo("Unknown \n");
      break;
  }

  kinfo("Detected vendor: %s\n", get_vendor(midr_el1));
}
#endif

static void arch_init(void)
{
#if defined(__x86_64__)
  idt_load();
  exceptions_init();
#elif defined(__aarch64__)
  exceptions_init();
  aarch64_mmu_init();
  get_board_info();
#endif
}

__dead void _start(void)
{
  printk("VegaOS v%s - Copyright (c) 2023 Ian Marco Moffett\n",
         VEGA_VERSION
  );

  pmm_init();
  arch_init();
  acpi_init();

  for (;;)
  {
    halt();
  }
}
