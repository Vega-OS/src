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

#ifndef _ACPI_TABLES_H_
#define _ACPI_TABLES_H_

#include <sys/types.h>
#include <sys/cdefs.h>

typedef struct 
{
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t rev;
    uint32_t rsdtaddr;
} __packed acpi_rsdp_t;


typedef struct 
{
    acpi_rsdp_t rsdp1;
    uint32_t length;
    uint64_t xsdtAddr;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} __packed acpi_rsdp2_t;


typedef struct 
{
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t rev;
    uint32_t rsdt;
    uint32_t length;
    uint64_t xsdt;
    uint8_t extended_checksum;
} __packed acpi_xsdp_t;


typedef struct 
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem[6];
    char oem_table[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __packed acpi_header_t;


typedef struct 
{
    acpi_header_t header;
    uint32_t tables[];
} __packed acpi_rsdt_t;


typedef struct 
{
    acpi_header_t header;
    uint64_t tables[];
} __packed acpi_xsdt_t;


typedef struct 
{
    uint8_t address_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t base;
} __packed acpi_gas_t;


typedef struct 
{
    acpi_header_t header;
    uint32_t firmware_control;
    uint32_t dsdt; // pointer to dsdt

    uint8_t reserved;

    uint8_t profile;
    uint16_t sci_irq;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_control;
    uint32_t pm1a_event_block;
    uint32_t pm1b_event_block;
    uint32_t pm1a_control_block;
    uint32_t pm1b_control_block;
    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;
    uint8_t pm_timer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t cstate_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;

    // cmos registers
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    // ACPI 2.0 fields
    uint16_t iapc_boot_flags;
    uint8_t reserved2;
    uint32_t flags;

    acpi_gas_t reset_register;
    uint8_t reset_command;
    uint16_t arm_boot_flags;
    uint8_t minor_version;

    uint64_t x_firmware_control;
    uint64_t x_dsdt;

    acpi_gas_t x_pm1a_event_block;
    acpi_gas_t x_pm1b_event_block;
    acpi_gas_t x_pm1a_control_block;
    acpi_gas_t x_pm1b_control_block;
    acpi_gas_t x_pm2_control_block;
    acpi_gas_t x_pm_timer_block;
    acpi_gas_t x_gpe0_block;
    acpi_gas_t x_gpe1_block;
} __packed acpi_fadt_t;


typedef struct 
{
    acpi_header_t header;
    uint32_t lapic_addr;
    uint32_t flags;
} __packed acpi_madt_t;

typedef struct 
{
    uint8_t type;
    uint8_t length;
} __packed apic_header_t;

typedef struct 
{
    apic_header_t header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __packed local_apic_t;


typedef struct 
{
    apic_header_t header;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t global_system_interrupt_base;
} __packed io_apic_t;

typedef struct 
{
    apic_header_t header;
    uint8_t bus;
    uint8_t source;					// IRQ.
    uint32_t interrupt;				// GSI.
    uint16_t flags;
} __packed apic_interrupt_override_t;

typedef struct acpi_hpet_t 
{
    acpi_header_t header;
    
    uint8_t    hardware_rev_id;
    uint8_t    comparator_count : 5;
    uint8_t    counter_size : 1;
    uint8_t    reserved : 1;
    uint8_t    legacy_replace : 1;
    uint16_t   pci_vendor_id;

    acpi_gas_t    base;
    uint8_t       hpet_id;
    uint16_t      minimum_tick;
    uint8_t       page_protection;
} __packed acpi_hpet_t;


#endif
