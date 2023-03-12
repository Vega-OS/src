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

#include <firmware/acpi/core.h>
#include <firmware/acpi/tables.h>
#include <mm/vmm.h>
#include <sys/panic.h>
#include <sys/printk.h>
#include <sys/module.h>
#include <string.h>

MODULE("acpi");

static volatile struct limine_rsdp_request rsdp_req = {
  .id = LIMINE_RSDP_REQUEST,
  .revision = 0
};

static acpi_rsdt_t *rsdt = NULL;
static acpi_madt_t *madt = NULL;
static size_t rsdt_entry_count = 0;

static uint8_t do_checksum(const acpi_header_t *header) 
{
  uint8_t sum = 0;

  for (uint32_t i = 0; i < header->length; ++i) 
  {
    sum += ((char *)header)[i];
  }

  return sum % 0x100 == 0;
}

void *acpi_query(const char *query)
{ 
  for (size_t i = 0; i < rsdt_entry_count; ++i)
  {
    acpi_header_t *current = (acpi_header_t *)((uint64_t)rsdt->tables[i] + VMM_HIGHER_HALF);
    if (memcmp(current->signature, query, strlen(query)) == 0)
    {
      return do_checksum(current) ? (void *)current : NULL;
    }
  }

  return NULL;
}

void acpi_init(void)
{
  if (rsdp_req.response == NULL)
  {
    panic("Couldn't fetch RSDP from the bootloader\n");
  }

  acpi_rsdp_t *rsdp = rsdp_req.response->address;
  rsdt = (acpi_rsdt_t *)((uintptr_t)rsdp->rsdtaddr + VMM_HIGHER_HALF);
  
  if (!do_checksum(&rsdt->header))
  {
    panic("Could not validate ACPI RSDT checksum\n");
  }

  rsdt_entry_count = (rsdt->header.length - sizeof(rsdt->header)) / 4;
  madt = acpi_query("APIC");

  if (madt == NULL)
  {
    panic("Query for ACPI MADT failed\n");
  }

  if (!do_checksum(&madt->header))
  {
    panic("Could not validate ACPI MADT checksum\n");
  }

  kinfo("All checks passed\n");
}
