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
#include <api/map.h>
#include <api/string.h>
#include <api/bootinfo.h>

#include <memory.h>
#include <debug.h>
#include <info.h>
#include <lib/multiboot.h>

//- Info Buffer ----------------------------------------------------------------

extern int8_t boot_info_end;
static uintptr_t boot_info_placement;

static void *_boot_info_place(size_t size) {
    if (boot_info_placement + size > (uintptr_t) &boot_info_end)
        PANIC("Boot info structure too large!");
    
    void *ptr = (void *) (boot_info_placement);
    memset(ptr, 0, size);
    
    boot_info_placement += size;
    return ptr;
}

static int8_t *_boot_info_place_str(const int8_t *src) {
    int8_t *str = (int8_t *) _boot_info_place(strlen(src));
    strcpy(str, src);
    return str;
}

//- Parsing --------------------------------------------------------------------

static boot_info_mmap_t *_boot_info_parse_mmap(multiboot_info_t *mb_info) {
    // Iterate over memory segments
    boot_info_mmap_t *prev = 0;
    boot_info_mmap_t *first = 0;
    
    multiboot_memory_map_t *mb_mem = (multiboot_memory_map_t*) mb_info->mmap_addr;
    
    while ((uintptr_t) mb_mem < mb_info->mmap_addr + mb_info->mmap_length) {
        // Create structure for segment
        boot_info_mmap_t *seg = (boot_info_mmap_t *) _boot_info_place(sizeof(boot_info_mmap_t));
        
        seg->address = mb_mem->addr;
        seg->length = mb_mem->len;
        seg->available = (1 == mb_mem->type) ? 1 : 0;
        
        // Link
        if (0 != prev)
            prev->next = (uintptr_t) seg;
        else
            first = seg;
        
        // Next
        mb_mem = (multiboot_memory_map_t *)
	    (((uintptr_t) mb_mem) + mb_mem->size + sizeof(uint32_t));
        prev = seg;
    }
    
    return first;
}

static boot_info_mod_t *_boot_info_parse_mods(multiboot_info_t *mb_info) {
    size_t i;
    boot_info_mod_t *prev = 0;
    boot_info_mod_t *first = 0;
    
    for (i = 0; i < mb_info->mods_count; ++i) {
        // Current mod
        multiboot_module_t *mb_mod = (multiboot_module_t *) (mb_info->mods_addr + i * sizeof(multiboot_module_t));
        
        // Create structure
        boot_info_mod_t *mod = (boot_info_mod_t *) _boot_info_place(sizeof(boot_info_mod_t));
        
        mod->address = mb_mod->mod_start;
        mod->length = mb_mod->mod_end - mb_mod->mod_start;
        
        // Copy name
        mod->name = (uintptr_t) _boot_info_place_str((int8_t *) mb_mod->cmdline);
        
        // Link
        if (0 != prev)
            prev->next = (uintptr_t) mod;
        else
            first = mod;
        
        prev = mod;
    }
    
    return first;
}

boot_info_t *boot_info_parse(multiboot_info_t *mb_info) {
    // Root info structure
    boot_info_t *info = (boot_info_t *) _boot_info_place(sizeof(boot_info_t));
    
    // Loader name
    info->loader_name = (uintptr_t) _boot_info_place_str((int8_t *) mb_info->boot_loader_name);
    
    // Arguments
    info->args = (uintptr_t) _boot_info_place_str((int8_t *) mb_info->cmdline);
    
    // Memory map
    info->mmap = (uintptr_t) _boot_info_parse_mmap(mb_info);
    
    // Modules
    info->mods = (uintptr_t) _boot_info_parse_mods(mb_info);
    
    return info;
}

//- Relocation -----------------------------------------------------------------

#define BOOT_INFO_RELOCATE(new_addr)					\
    (target - ((uint64_t) (uintptr_t) info) + (uint64_t) new_addr)

void boot_info_relocate(boot_info_t *info, uint64_t target)
{
    // Root structure
    info->loader_name = BOOT_INFO_RELOCATE(info->loader_name);
    info->args = BOOT_INFO_RELOCATE(info->args);
        
    // Memory map
    boot_info_mmap_t *mmap = (boot_info_mmap_t *) (uintptr_t) info->mmap;
    
    while (0 != mmap) {
        uintptr_t old_next = (uintptr_t) mmap->next;
        
        if (0 != mmap->next)
            mmap->next = (uint64_t) BOOT_INFO_RELOCATE(mmap->next);
            
        mmap = (boot_info_mmap_t *) old_next;
    }
    
    info->mmap = BOOT_INFO_RELOCATE(info->mmap);
        
    // Modules
    boot_info_mod_t *mod = (boot_info_mod_t *) (uintptr_t) info->mods;
    
    while (0 != mod) {
        mod->name = BOOT_INFO_RELOCATE(mod->name);
    
        uintptr_t old_next = (uintptr_t) mod->next;
        if (0 != mod->next)
            mod->next = BOOT_INFO_RELOCATE(mod->next);
        mod = (boot_info_mod_t *) old_next;
    }
    
    info->mods = BOOT_INFO_RELOCATE(info->mods);
}

uintptr_t boot_modules_relocate(boot_info_t *info, uintptr_t target)
{
    // Align target
    target = memalign(target, 0x1000);

    // Sort modules by begin address (ASC)
    boot_info_mod_t *sorted = 0;
    boot_info_mod_t *sorted_end = sorted;
    
    while (0 != info->mods) {
        // Find first module
        boot_info_mod_t *current = (boot_info_mod_t *) (uintptr_t) info->mods;
        boot_info_mod_t *prev = 0;
        boot_info_mod_t *min_prev = 0;
        boot_info_mod_t *min = 0;
        
        while (0 != current) {
            if (0 == min || current->address < min->address) {
                min = current;
                min_prev = prev;
            }
            
            // Next
            prev = current;
            current = (boot_info_mod_t *) (uintptr_t) current->next;
        }
        
        // Remove from list
        if (0 == min_prev)
            info->mods = min->next;
        else
            min_prev->next = min->next;
            
        // Add to sorted list
        if (0 == sorted_end)
            sorted = sorted_end = min;
        else {
            sorted_end->next = (uintptr_t) min;
            sorted_end = min;
        }
    }

    // Replace lists
    info->mods = (uintptr_t) sorted;
    
    // Move modules
    boot_info_mod_t *current = (boot_info_mod_t *) (uintptr_t) info->mods;

    while (0 != current) {
        // Move
        memcpy(
            (void *) target,
            (void *) ((uintptr_t) current->address),
            (size_t) current->length);

        current->address = (uint64_t) target;
        target += (uintptr_t) current->length;
        target = memalign(target, 0x1000);

        // Next
        current = (boot_info_mod_t *) (uintptr_t) current->next;
    }
    
    return target;
}

void boot_modules_map(boot_info_t *info) {
    boot_info_mod_t *current = (boot_info_mod_t *) (uintptr_t) info->mods;
    uint64_t virtual_placement = MEMORY_MODULES_VADDR;

    while (0 != current) {
        // Map
        uintptr_t offset;
        uintptr_t offset_end = memalign(current->length, 0x1000);
        uint16_t flags = PAGE_FLAG_GLOBAL;

        current->mapping = virtual_placement;

        for (offset = 0; offset < offset_end; offset += 0x1000) {
            memory_map(virtual_placement + offset,
                   current->address + offset,
                   flags);
        }

        // Add length to virtual placement
        virtual_placement += offset_end;

        // Next
        current = (boot_info_mod_t *) (uintptr_t) current->next;
    }
}
