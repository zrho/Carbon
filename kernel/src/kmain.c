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

#include <api/types.h>
#include <api/bootinfo.h>
#include <api/string.h>

#include <memory.h>
#include <debug.h>
#include <cpu.h>
#include <irq.h>
#include <multitasking.h>
#include <binary.h>
#include <syscall.h>
#include <fpu.h>

static boot_info_t *info;

static boot_info_mod_t *_find_root_mod() {
    boot_info_mod_t *mod = info->mods;

    while (0 != mod) {
        if ((int8_t *) mod->name == strstr((int8_t *) mod->name, "/boot/root.bin"))
            return mod;

        mod = mod->next;
    }

    return 0;
}

/**
 * Prints a welcome message and some values from the info structure.
 *
 * Requires:
 *  * Info structure
 *  * Console
 */
static void kmain_header() {
    DEBUG("---------------------------------------\n");
    DEBUG("Carbon AMD64 Microkernel 0.1\n");
    DEBUG("Copyright (c) 2011 by Lukas Heidemann\n");
    DEBUG("---------------------------------------\n");

    DEBUG("Loader: ");
    DEBUG(info->loader_name);
    DEBUG("\nEntry point: ");
    DEBUG_HEX(info->entry_point);
    DEBUG("\nEnd of allocated memory: ");
    DEBUG_HEX(info->free_mem_begin);
    DEBUG("\n");
}

/**
 * Initializes kernel functionality and prints debug output.
 *
 * Requires:
 *  * Info structure
 *  * Console
 */
static void kmain_init() {
    DEBUG("---------------------------------------\n");

    // Virtual Memory Management
    DEBUG("Initializing virtual memory management...\n");
    memory_space_initial = memory_space_get();

    // Physical Memory Management
    DEBUG("Initializing physical memory management...\n");
    frame_init(info);

    // Interrupts
    DEBUG("Initializing interrupt management...\n");

    irq_pic_init(); // Initialize PIC (reroutes and masks all IRQs)
    cpu_int_init(); // Initializes the IDT to handle interrupts
    cpu_int_enable(); // Enables interrupts
    cpu_tss_create(); // For UserMode to Kernel interrupts

    // FPU
    DEBUG("Initializing floating point unit...\n");

    fpu_init();

    // System calls
    cpu_int_register(SYSCALL_INT_VECTOR, &syscall_handler_int);
}

static void kmain_root() {
    DEBUG("---------------------------------------\n");

    // Search for root binary module
    DEBUG("Searching for root module...\n");
    boot_info_mod_t *root_mod = _find_root_mod(info);
    
    if (0 == root_mod)
	PANIC("Could not find root binary module.\n"
	      "Please make sure to attach the root binary as a module\n"
	      "with name '/boot/root.bin'.");

    // Create new, clean address space
    memory_space_switch(memory_space_create());

    // Create root process
    uint16_t pflags = PAGE_FLAG_USER;

    DEBUG("Creating process...\n");
    process_t *proc = process_spawn(memory_space_get(), 0);

    DEBUG("Loading binary...\n");
    uintptr_t entry_addr = binary_load_elf64((void *) root_mod->mapping, pflags);

    DEBUG("Starting thread...\n");
    thread_thaw(thread_spawn(proc, entry_addr), 0);

    // Enable timer
    // No operations should be performed in this 'thread'
    // after enabling the timer to prevent concurrency
    // issues (disabled interrupts while handling an interrupt
    // acts as a kernel lock).
    DEBUG("Passing control to root process...\n");
    DEBUG("---------------------------------------\n");
    irq_pit_init();
}

void kmain(void);
void kmain() {
    // Prepare console
    console_videomem = (uint16_t *) MEMORY_VIDEO_VADDR;
    console_clear();

    // Get boot info
    info = (boot_info_t *) (MEMORY_BOOT_INFO_VADDR);

    // Phases
    kmain_header();
    kmain_init();
    kmain_root();
}

