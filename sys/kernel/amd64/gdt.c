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

#include <amd64/gdt.h>
#include <sys/bug.h>

#if defined(__x86_64__)

static struct gdt_entry gdt[256] = {
  /* 0x00 */
  {0},

  /* 0x08 */
  {
    .limit       = 0xFFFF,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0x9A,
    .granularity = 0x00,
    .base_hi     = 0x00
  },

  /* 0x10 */
  {
    .limit       = 0xFFFF,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0x92,
    .granularity = 0x00,
    .base_hi     = 0x00
  },

  /* 0x18 */
  {
    .limit       = 0xFFFF,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0x9A,
    .granularity = 0xCF,
    .base_hi     = 0x00
  },

  /* 0x20 */
  {
    .limit       = 0xFFFF,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0x92,
    .granularity = 0xCF,
    .base_hi     = 0x00
  },

  /* 0x28 */
  {
    .limit       = 0x0000,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0x9A,
    .granularity = 0x20,
    .base_hi     = 0x00
  },

  /* 0x30 */
  {
    .limit       = 0x0000,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0x92,
    .granularity = 0x00,
    .base_hi     = 0x00
  },

  /* 0x38 */
  {
    .limit       = 0x0000,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0xFA,
    .granularity = 0xAF,
    .base_hi     = 0x00
  },

  /* 0x40 */
  {
    .limit       = 0x0000,
    .base_low    = 0x0000,
    .base_mid    = 0x00,
    .access      = 0xF2,
    .granularity = 0x00,
    .base_hi     = 0x00
  },
};
static struct gdtr gdtr = {
  .limit = sizeof(struct gdt_entry) * 256 - 1,
  .offset = (uintptr_t)&gdt[0]
};

void gdt_load(void)
{
  __asm("lgdt %0" :: "m" (gdtr));
}

#endif    /* __x86_64__ */
