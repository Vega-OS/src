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

#include <aarch64/mm/mmu.h>
#include <aarch64/cpu.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <sys/module.h>
#include <sys/math.h>
#include <sys/printk.h>
#include <sys/panic.h>

MODULE("mmu");

#if defined(__aarch64__)

static struct aarch64_pagemap pagemap;

static uintptr_t next_level(uintptr_t level_phys, size_t index, uint8_t alloc)
{
  uintptr_t *level_virt = (uintptr_t *)(level_phys + VMM_HIGHER_HALF);

  if (level_virt[index] & PTE_P)
  {
    return level_virt[index] & PTE_ADDR_MASK;
  }

  if (!alloc)
  {
    return 0;
  }

  uintptr_t next = pmm_alloc(1);
  level_virt[index] = next  |
                      PTE_P |
                      PTE_TBL;
  return alloc;
}

/*
 *  Walks the translation table by
 *  extracting the first-level lookup
 *  from TTBRn (where n is 1 for
 *  higher half virtual addresses
 *  and 0 for lower half virtual addresses),
 *
 *  and doing the following:
 *  
 *
 *  FIRST_LEVEL -> SECOND_LEVEL -------+
 *  THIRD_LEVEL -> PHYSICAL_ADDR       |
 *  ^                                  |
 *  |                                  |
 *  +----------------------------------+
 *
 *  NOTE: Assumes 48-bit physical addresses,
 *        4-level paging and 4K granule.
 *
 */

uintptr_t aarch64_translate_vaddr(struct aarch64_pagemap p, uintptr_t vaddr)
{
  /*
   *  If bit 64 of the virtual address
   *  is set, it's a higher half address.
   *
   *  TTBR0 -> lower half
   *  TTBR1 -> higher half
   */

  uint8_t ttbr_index = vaddr >= VMM_HIGHER_HALF;
  
  size_t level0_index = (vaddr >> 39) & 0x1FF;
  size_t level1_index = (vaddr >> 30) & 0x1FF;
  size_t level2_index = (vaddr >> 21) & 0x1FF;
  size_t level3_index = (vaddr >> 12) & 0x1FF; 
  
  uintptr_t lookup_level_0 = p.ttbr[ttbr_index] & PTE_ADDR_MASK;  
  uintptr_t current_level = next_level(lookup_level_0, level0_index, 0);

  if (current_level == 0)
  {
    return 0;
  }

  current_level = next_level(current_level, level1_index, 0);
  
  if (current_level == 0)
  {
    return 0;
  }

  current_level = next_level(current_level, level2_index, 0);
  
  if (current_level == 0)
  {
    return 0;
  }

  return next_level(current_level, level3_index, 0);
}

void aarch64_mmu_init(void)
{
  size_t fb_attr  = (cpu_read_sysreg(mair_el1) >> 8) & 0xFF;
  size_t id_mmfr0 = cpu_read_sysreg(id_aa64mmfr0_el1);
  size_t pa_bits  = id_mmfr0 & 0xF;
  
  /* 
   * Systems like QEMU map framebuffer as 0xFF.
   * Override it.
   */
  if (fb_attr == 0xFF)
  {
    fb_attr = 0xC;
  }

  size_t mair =
    (fb_attr << 8)     | /* Framebuffer (not always write-combining) */
    (0xFF << 0)        | /* Normal (Write-back, RW allocate, non-transient) */
    (0x00 << 16)       | /* Device (nGnRnE) */
    (0x04 << 24);        /* Normal Uncachable (device) */

  size_t tcr =
    (0x10 << 0)    |      /* T0SZ=16 */
    (0x10 << 16)   |      /* T1SZ=16 */
    (0x02 << 30)   |      /* 4K granule */
    (0x01 << 8)    |      /* TTBR0 Inner WB RW-Allocate */
    (0x01 << 10)   |      /* TTBR0 Outer WB RW-Allocate */
    (0x02 << 12)   |      /* TTBR0 Inner shareable */
    (0x01 << 24)   |      /* TTBR1 Inner WB RW-Allocate */
    (0x01 << 26)   |      /* TTBR1 Outer WB RW-Allocate */
    (0x02 << 28)   |      /* TTBR1 Inner shareable */
    (0x01UL << 36) |      /* Use 16-bit ASIDs */
    (pa_bits << 32);      /* 48-bit intermediate address */
  
  cpu_write_sysreg(mair_el1, mair);
  cpu_write_sysreg(tcr_el1, tcr);

  kinfo("Wrote MAIR and TCR for EL1\n");

  pagemap.ttbr[0] = cpu_read_sysreg(ttbr0_el1);
  pagemap.ttbr[1] = cpu_read_sysreg(ttbr1_el1);
}

#endif
