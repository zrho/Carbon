/**
 * Carbon Operating System
 * Copyright (C) 2011 Lukas Heidemann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <api/types.h>
#include <api/compiler.h>

//- State ----------------------------------------------------------------------

typedef struct cpu_state_t {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
} PACKED cpu_state_t;

//- Interrupts -----------------------------------------------------------------

typedef struct cpu_int_state_t {

    // State of the CPU
    uint64_t ds;
    cpu_state_t state;
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;

} PACKED cpu_int_state_t;

// Callback for interrupt handlers
typedef void (* cpu_int_handler_t)(cpu_int_state_t *);

void cpu_int_init(void);
void cpu_int_enable(void);
void cpu_int_disable(void);

void cpu_int_register(uint8_t vector, cpu_int_handler_t callback);

//- TSS ------------------------------------------------------------------------

#define TSS_GDT_OFFSET 0x28

// The structre of the Task Segment Selector.
//
// Normally used for hardware multitasking, on X86 and AMD64 at least one TSS
// per core is required for switching the stack when entering the kernel.
typedef struct cpu_tss_t
{
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t io_map_base;
} PACKED cpu_tss_t;

// Structure for System-Segment Descriptors used in the GDT to link in the
// platform's TSSs.
typedef struct cpu_tss_ptr_t
{
    // Lower 2 bytes of the limit.
    uint16_t limitLow;
    
    // Lower 2 bytes of the base address.
    uint16_t baseLow;
    
    // Middle low byte of the base address.
    uint8_t baseMiddleLow;
    
    // Flags.
    uint8_t flags;
    
    // Additional flags and highest 4 bits of the limit.
    uint8_t flags_limitHigh;
    
    // Middle high byte of the base address.
    uint8_t baseMiddleHigh;
    
    // Highest dword of the base address.
    uint32_t baseHigh;
    
    // Reserved fields.
    uint32_t reserved;
    
} PACKED cpu_tss_ptr_t;

void cpu_tss_create(void);
