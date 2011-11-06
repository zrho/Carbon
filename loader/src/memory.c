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
#include <api/map.h>

#include <memory.h>
#include <debug.h>

//- Frame Heap -----------------------------------------------------------------

/**
 * Placement address for new frames. Should be page aligned.
 */
uintptr_t frame_placement = 0x0;

/**
 * Allocates a frame of physical memory.
 *
 * @return The physical address of the allocated frame.
 */
uintptr_t frame_alloc() {
    uintptr_t frame = frame_placement;
    frame_placement += 0x1000;
    memset((void *) frame, 0, 0x1000);
    return frame;
}


//- Mapping --------------------------------------------------------------------

#define PAGE_PHYSICAL_MASK ~(((uint64_t) 1 << 63) | 0xFFF)
#define PAGE_ENTRY(table, idx) (uint64_t *) (((uintptr_t) table) + idx * 8)
#define _MEMORY_PAGE_GETTER(name, shift)				\
    static void *_memory_##name(void *parent, uint64_t virtual_addr, bool create) { \
	uint16_t index = (virtual_addr >> shift) & 0x1FF;		\
	uint64_t *entry_parent = PAGE_ENTRY(parent, index);		\
									\
	if (0 == (*entry_parent & PAGE_FLAG_PRESENT)) {			\
	    if (create) {						\
		*entry_parent = frame_alloc() | PAGE_FLAG_PRESENT |	\
		    PAGE_FLAG_USER;					\
            } else							\
		return (void *) 0;                                      \
        }                                                               \
									\
	return (void *) (uintptr_t) (*entry_parent & PAGE_PHYSICAL_MASK); \
    }

extern uint8_t boot_pml4;
extern uint8_t gdtr64;

_MEMORY_PAGE_GETTER(pdp, 39)
_MEMORY_PAGE_GETTER(pd, 30)
_MEMORY_PAGE_GETTER(pt, 21)

/**
 * Returns a pointer to the PTE for the given virtual address.
 *
 * @param virtual_addr The virtual address to return the PTE for.
 * @param create Whether or not to create the higher level structures, if they do not exist.
 * @return Pointer to the PTE or null-pointer, if there is no such PTE and the create flag is
 *  clear.
 */
static uint64_t *_memory_page(uint64_t virtual_addr, bool create) {
    void *pdp = _memory_pdp((void *) &boot_pml4, virtual_addr, create);
    if (0 == pdp) return (uint64_t *) 0;

    void *pd = _memory_pd(pdp, virtual_addr, create);
    if (0 == pd) return (uint64_t *) 0;

    void *pt = _memory_pt(pd, virtual_addr, create);
    if (0 == pt) return (uint64_t *) 0;

    uint16_t index = (virtual_addr >> 12) & 0x1FF;
    return (uint64_t *) (((uintptr_t) pt) + index * 8);
}

/**
 * Maps a page in virtual memory.
 *
 * @param virtual_addr The virtual address of the page to map.
 * @param physical_addr The physical address of the page.
 * @param flags The flags for the page. Present flag will be set automatically.
 */
void memory_map(uint64_t virtual_addr, uint64_t physical_addr, uint16_t flags) {
    flags |= PAGE_FLAG_PRESENT;

    uint64_t *pte = _memory_page(virtual_addr, true);
    *pte = flags | physical_addr;
}

void memory_map_system(boot_info_t *info) {
    // Map video memory
    memory_map(
            MEMORY_VIDEO_VADDR, MEMORY_VIDEO_PADDR,
            PAGE_FLAG_WRITABLE | PAGE_FLAG_GLOBAL);
    
    // Map boot info structure
    memory_map(
            MEMORY_BOOT_INFO_VADDR, (uintptr_t) info,
            PAGE_FLAG_GLOBAL);
    
    // Map GDT
    memory_map(
            MEMORY_GDT64_VADDR, (uintptr_t) &gdtr64,
            PAGE_FLAG_GLOBAL);
    
    // Setup recursive mapping
    *PAGE_ENTRY(&boot_pml4, 511) = 
            ((uint64_t) (uintptr_t) &boot_pml4) | PAGE_FLAG_PRESENT;
    
    // Identity map first 2MB
    uintptr_t addr;
    for (addr = 0; addr < 512 * 0x1000; addr += 0x1000)
        memory_map(addr, addr, PAGE_FLAG_WRITABLE);
}
