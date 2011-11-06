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
#include <api/string.h>
#include <api/bootinfo.h>
#include <api/map.h>

#include <debug.h>
#include <memory.h>
#include <binary.h>
#include <info.h>

#include <lib/multiboot.h>

//- Main Function --------------------------------------------------------------

extern uint8_t end;
extern uint64_t boot_trampoline_callback;

static boot_info_mod_t *_boot_find_kernel(boot_info_t *info) {
    boot_info_mod_t *module = (boot_info_mod_t *) (uintptr_t) info->mods;
    
    while (0 != module) {
        if (strcmp((int8_t *) (uintptr_t) module->name, "/boot/kernel.bin"))
            return module;
            
        module = (boot_info_mod_t *) (uintptr_t) module->next;
    }
    
    return 0;
}

int init(uint32_t mboot_magic, multiboot_info_t *mboot_info);
int init(uint32_t mboot_magic, multiboot_info_t *mboot_info) {
    // Header
    console_clear();
    DEBUG("------------------------------------------\n");
    DEBUG("Welcome to Hydrogen AMD64 Loader 0.1\n");
    DEBUG("Copyright (c) 2011 by Lukas Heidemann\n");
    DEBUG("------------------------------------------\n");

    // Check multiboot magic
    if (mboot_magic != MULTIBOOT_BOOTLOADER_MAGIC)
        PANIC("Bad multiboot magic value supplied by bootloader.\n"
            "Please make sure, that your bootloader implements the Multiboot specification"
            " in version 1 or higher.");

    // Parse boot info and relocate modules
    boot_info_t *info = boot_info_parse(mboot_info);
    frame_placement = memalign(boot_modules_relocate(info, (uintptr_t) &end), 0x1000);
    boot_modules_map(info);

    DEBUG("Boot loader name: ");
    DEBUG((int8_t *) (uintptr_t) info->loader_name);
    DEBUG("\nLoader arguments: ");
    DEBUG((int8_t *) (uintptr_t) mboot_info->cmdline);
    DEBUG("\n");

    // Find the kernel module
    boot_info_mod_t *kernel_mod = _boot_find_kernel(info);

    if (0 == kernel_mod)
        PANIC("Could not find kernel binary.\n"
            "The Hydrogen AMD64 loader expects an ELF64 kernel binary to be given as a "
	        "module with the name '/boot/kernel.bin'. Please make sure that your kernel "
	        "binary is named as such and is passed by the bootloader.");

    // Map system structures
    memory_map_system(info);

    // Load the kernel
    uint16_t pflags = PAGE_FLAG_GLOBAL;
    info->entry_point = binary_load_elf64((void *) (uintptr_t) kernel_mod->address, pflags);

    DEBUG("Kernel Entry Point: ");
    DEBUG_HEX(info->entry_point);
    DEBUG("\n");

    // Set beginning of free memory
    info->free_mem_begin = memalign(frame_placement, 0x1000);

    // Prepare long mode trampoline code
    boot_trampoline_callback = info->entry_point;

    // Relocate info structure to higher half
    boot_info_relocate(info, MEMORY_BOOT_INFO_VADDR);
    
    return 0;
}
