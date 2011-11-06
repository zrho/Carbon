/**
 * Hydrogen Operating System
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
#include <lib/multiboot.h>

//- Structures -----------------------------------------------------------------

typedef struct boot_info_mmap_t
{
    // The address the memory segment begins at.   
    uint64_t address;
    
    // The length of the memory segment.
    uint64_t length;
    
    // Whether or not the memory segment is available.
    // <tt>1</tt> when available, <tt>0</tt> otherwise.
    uint8_t available;
    
    // Pointer to the next structure or null-pointer if this is the last one.
#ifdef __AMD64__
    struct boot_info_mmap_t *next;
#else
    uint64_t next;
#endif
} PACKED boot_info_mmap_t;

typedef struct boot_info_mod_t
{
    // The physical address the module begins at.
    uint64_t address;
    
    // The virtual address the module begins at (in the kernel).
    uint64_t mapping;
    
    // The length of the module.
    uint64_t length;
    
    // The name of the module.
    uint64_t name;
    
    // Pointer to the next structure or null-pointer if this is the last one.
#ifdef __AMD64__
    struct boot_info_mod_t *next;
#else
    uint64_t next;
#endif
} PACKED boot_info_mod_t;

typedef struct boot_info_t
{
    // The name of the boot loader.
#ifdef __AMD64__
    int8_t *loader_name;
#else
    uint64_t loader_name;
#endif
    
    // Arguments passed to the kernel at boot time.
#ifdef __AMD64__
    int8_t *args;
#else
    uint64_t args;
#endif
    
    // The address of the kernel's entry point.
    uint64_t entry_point;
    
    // The address on which free and usable memory begins (after loaded modules,
    // binaries etc.)
    uint64_t free_mem_begin;
    
    // Pointer to the first entry of the memory map.
#ifdef __AMD64__
    boot_info_mmap_t *mmap;
#else
    uint64_t mmap;
#endif
    
    // Pointer to the first module that was passed to the kernel, or a null-pointer
    // if there's no such module.
#ifdef __AMD64__
    boot_info_mod_t *mods;
#else
    uint64_t mods;
#endif
    
} PACKED boot_info_t;
