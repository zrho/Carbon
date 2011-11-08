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

#include <memory.h>
#include <debug.h>

// TODO: Refactor this mess!

//- Recursive Mapping Addresses ------------------------------------------------

#define PAGE_PML4E_INDEX(a)         ((a >> 39) & 0x1FF)
#define PAGE_PDPE_INDEX(a)          ((a >> 30) & 0x1FF)
#define PAGE_PDE_INDEX(a)           ((a >> 21) & 0x1FF)
#define PAGE_PTE_INDEX(a)           ((a >> 12) & 0x1FF)

#define PAGE_OFFSET_PML4            0xFFFFFFFFFFFFF000
#define PAGE_OFFSET_PDP             0xFFFFFFFFFFE00000
#define PAGE_OFFSET_PD              0xFFFFFFFFC0000000
#define PAGE_OFFSET_PT              0xFFFFFF8000000000

#define PAGE_VIRT_PML4              PAGE_OFFSET_PML4
#define PAGE_VIRT_PDP(pdp)          (PAGE_OFFSET_PDP | ((uint64_t) pdp << 12))
#define PAGE_VIRT_PD(pdp, pd)       (PAGE_OFFSET_PD | ((uint64_t) pdp << 21) | ((uint64_t) pd << 12))
#define PAGE_VIRT_PT(pdp, pd, pt)   (PAGE_OFFSET_PT | ((uint64_t) pdp << 30) | ((uint64_t) pd << 21) \
                                    | ((uint64_t) pt << 12))
                                    
#define PAGE_VIRT_PML4E(i)          (PAGE_VIRT_PML4 + i * 8)
#define PAGE_VIRT_PDPE(pdp, i)      (PAGE_VIRT_PDP(pdp) + i * 8)
#define PAGE_VIRT_PDE(pdp, pd, i)   (PAGE_VIRT_PD(pdp, pd) + i * 8)
#define PAGE_VIRT_PTE(pdp, pd, pt, i) (PAGE_VIRT_PT(pdp, pd, pt) + i * 8)

#define PAGE_VIRT_PAGE(a)           PAGE_VIRT_PTE( \
                                       PAGE_PML4E_INDEX(a), \
                                       PAGE_PDPE_INDEX(a), \
                                       PAGE_PDE_INDEX(a), \
                                       PAGE_PTE_INDEX(a))

#define PAGE_PHYSICAL(a)            (a & ~0x1FF)

//- Virtual Memory Management --------------------------------------------------

#define PAGE_FLAGS_RECURSIVE PAGE_FLAG_PRESENT | PAGE_FLAG_WRITEABLE
#define PAGE_FLAGS_STRUCTURE PAGE_FLAG_PRESENT | PAGE_FLAG_WRITEABLE | PAGE_FLAG_USER

/**
 * Invalidates the page for the given virtual address on this processor.
 *
 * @param virt The virtual address mapped by the page to invalidate.
 */
static void _memory_invalidate(uintptr_t virt)
{
    asm volatile ("invlpg %0" :: "m" (virt));
}

/**
 * Maps given page to the given physical address and sets the given flags.
 *
 * @param page The page to map.
 * @param phys The physical address to map to.
 * @param flags The flags to set.
 */
static void _memory_map(uint64_t *page, uintptr_t phys, uint16_t flags)
{
    *page = memalign(phys, PAGE_SIZE) | flags | PAGE_FLAG_PRESENT;
}

/**
 * Unmaps the given page.
 *
 * @param page The page to unmap.
 */
static void _memory_unmap(uint64_t *page)
{
    *page &= ~PAGE_FLAG_PRESENT;
}

/**
 * Allocates a new frame and maps the given page to it.
 *
 * @param page The page to map.
 * @param flags The flags to map the flags with.
 * @param virt The virtual address to invalidate.
 */
static void _memory_page_alloc_frame(uint64_t *page, uint16_t flags, uintptr_t virt)
{
    // TODO: PANIC on error
    uintptr_t frame = frame_alloc();
    _memory_map(page, frame, flags);
    _memory_invalidate(virt);
    memset((void *) virt, 0, 0x1000);
}

static bool _memory_struct_exists(
		uint16_t pml4e_idx,
		uint16_t pdpe_idx,
		uint16_t pde_idx,
		uint8_t struct_idx,
		bool create) {
	// Finished?
	if (struct_idx >= PAGE_STRUCT_PML4)
		return true;

	// PML4E
	uint64_t *pml4e = (uint64_t *) PAGE_VIRT_PML4E(pml4e_idx);

	if (!(*pml4e & PAGE_FLAG_PRESENT)) {
		if (create)
			_memory_page_alloc_frame(
				pml4e,
				PAGE_FLAGS_STRUCTURE,
				PAGE_VIRT_PDP(pml4e_idx));
		else
			return false;
	}

	// Finished?
	if (struct_idx >= PAGE_STRUCT_PDP)
		return true;

	// PDPE
	uint64_t *pdpe = (uint64_t *) PAGE_VIRT_PDPE(
		pml4e_idx, pdpe_idx);

	if (!(*pdpe & PAGE_FLAG_PRESENT)) {
		if (create)
			_memory_page_alloc_frame(
				pdpe,
				PAGE_FLAGS_STRUCTURE,
				PAGE_VIRT_PD(
					pml4e_idx,
					pdpe_idx));
		else
			return false;
	}

	// Finished?
	if (struct_idx >= PAGE_STRUCT_PD)
		return true;

	// PDE
	uint64_t *pde = (uint64_t *) PAGE_VIRT_PDE(
		pml4e_idx,
		pdpe_idx,
		pde_idx);

	if (!(*pde & PAGE_FLAG_PRESENT)) {
		if (create)
			_memory_page_alloc_frame(
				pde,
				PAGE_FLAGS_STRUCTURE,
				PAGE_VIRT_PT(
					pml4e_idx,
					pdpe_idx,
					pde_idx));
		else
			return false;
	}

	// Exists
	return true;
}

/**
 * Checks whether the page for the given virtual address exists in the current
 * address space.
 *
 * @param virt The virtual address that belongs to the page to create.
 * @param create Whether to create the page, if it does not exist.
 * @return Returns whether the page exists now.
 */
static bool _memory_page_exists(uintptr_t virt, bool create)
{
    return _memory_struct_exists(
        PAGE_PML4E_INDEX(virt),
        PAGE_PDPE_INDEX(virt),
        PAGE_PDE_INDEX(virt),
        PAGE_STRUCT_PT,
        create);
}

void memory_map(uint64_t virt, uint64_t phys, uint16_t flags)
{
    // Create the page (if it does not already exist)
	_memory_page_exists(virt, true);
    
    // Map page
    _memory_map((uint64_t *) PAGE_VIRT_PAGE(virt), phys, flags);
    
    // Invalidate TLB entry
    _memory_invalidate(virt);
}

void memory_unmap(uint64_t virt)
{
    // Check if the page exists
    if (_memory_page_exists(virt, false)) {
        // Remove present flag
        _memory_unmap((uint64_t *) (PAGE_VIRT_PAGE(virt)));
	
		// Invalidate TLB
		_memory_invalidate(virt);
    }
}

uint64_t memory_physical(uint64_t virt)
{
    // Align (down) virtual address
    uintptr_t aligned = virt & ~0xFFF;
    
    // Calculate offset in page
    uintptr_t offset = virt - aligned;
    
    // Check if page exists
    uintptr_t phys = (uintptr_t) -1;
    
    if (_memory_page_exists(virt, false)) {
        // Align (down) page value
        phys = *((uint64_t *) PAGE_VIRT_PAGE(virt));
        phys &= ~0xFFF;
        
        // Add offset
        phys += offset;
    }
    
    return phys;
}

bool memory_user_accessible(uint64_t virt) {
    // Align (down) virtual address
    virt &= ~0xFFF;

    // Page exists?
    if (!_memory_page_exists(virt, false))
    	return false;

    // Check PML4E
    uint16_t pml4e_idx = PAGE_PML4E_INDEX(virt);
    uint64_t *pml4e = (uint64_t *) PAGE_VIRT_PML4E(pml4e_idx);

    if ((*pml4e & PAGE_FLAG_USER) == 0)
    	return false;

    // Check PDPE
    uint16_t pdpe_idx = PAGE_PDPE_INDEX(virt);
    uint64_t *pdpe = (uint64_t *) PAGE_VIRT_PDPE(pml4e_idx, pdpe_idx);

    if ((*pdpe & PAGE_FLAG_USER) == 0)
    	return false;

    // Check PDE
    uint16_t pde_idx = PAGE_PDE_INDEX(virt);
    uint64_t *pde = (uint64_t *) PAGE_VIRT_PDE(pml4e_idx, pdpe_idx, pde_idx);

    if ((*pde & PAGE_FLAG_USER) == 0)
    	return false;

    // Check PTE
    uint64_t *pte = (uint64_t *) PAGE_VIRT_PAGE(virt);
    return ((*pte & PAGE_FLAG_USER) != 0);
}

bool memory_region_accessible(uint64_t virtual_addr, uint64_t length) {
    uint64_t ptr = memalign(virtual_addr, 0x1000);

    for (; ptr < virtual_addr + length; ptr += 0x1000)
        if (!memory_user_accessible(ptr))
            return false;

    return true;
}

static uint64_t *_memory_struct_parent(uint64_t virtual_addr, uint8_t struct_idx) {
	// PML4 does not have a parent
	if (struct_idx >= PAGE_STRUCT_PML4)
		PANIC("PML4 does not have a parent structure.");

	// Get indices
	uint16_t pml4e_idx = PAGE_PML4E_INDEX(virtual_addr);
	uint16_t pdpe_idx = PAGE_PDPE_INDEX(virtual_addr);
	uint16_t pde_idx = PAGE_PDE_INDEX(virtual_addr);

	// Get parent
	uint64_t *parent;

	if (PAGE_STRUCT_PDP == struct_idx) {
		parent = (uint64_t *) PAGE_VIRT_PML4E(pml4e_idx);

	} else if (PAGE_STRUCT_PD == struct_idx) {
		parent = (uint64_t *) PAGE_VIRT_PDPE(
				pml4e_idx, pdpe_idx);

	} else if (PAGE_STRUCT_PT == struct_idx) {
		parent = (uint64_t *) PAGE_VIRT_PDE(
				pml4e_idx, pdpe_idx, pde_idx);
	}

	return parent;
}

static void _memory_struct_dispose(
		uint16_t pml4e,
		uint16_t pdpe,
		uint16_t pde,
		uint8_t struct_idx) {

	// Clear unused indices
	if (struct_idx > PAGE_STRUCT_PT)
		pde = 0;

	if (struct_idx > PAGE_STRUCT_PD)
		pdpe = 0;

	// Dispose child structures
	if (struct_idx > PAGE_STRUCT_PT) {
		uint16_t i;
		for (i = 0; i < 512; ++i) {
			// Increase index
			if (PAGE_STRUCT_PDP == struct_idx)
				++pdpe;
			else if (PAGE_STRUCT_PD == struct_idx)
				++pde;

			// Dispose
			_memory_struct_dispose(pml4e, pdpe, pde, struct_idx - 1);
		}
	}

	// Get page
	uint64_t *page;

	if (PAGE_STRUCT_PDP == struct_idx) {
		page = (uint64_t *) PAGE_VIRT_PML4E(pml4e);

	} else if (PAGE_STRUCT_PD == struct_idx) {
		page = (uint64_t *) PAGE_VIRT_PDPE(pml4e, pdpe);

	} else if (PAGE_STRUCT_PT == struct_idx) {
		page = (uint64_t *) PAGE_VIRT_PDE(pml4e, pdpe, pde);
	}

	// Free page
	frame_free(PAGE_PHYSICAL(*page));
}

uint64_t memory_struct_remove(uint64_t virtual_addr, uint8_t struct_idx) {
	// Get parent
	uint64_t *parent = _memory_struct_parent(virtual_addr, struct_idx);

	// Get structure
	uint64_t struct_ptr = PAGE_PHYSICAL(*parent);

	// Remove structure
	*parent = 0;
	return struct_ptr;
}

void memory_struct_insert(
		uint64_t virtual_addr,
		uint8_t struct_idx,
		uint64_t struct_ptr) {
	// Make sure parent structure exists
	_memory_struct_exists(
			PAGE_PML4E_INDEX(virtual_addr),
			PAGE_PDPE_INDEX(virtual_addr),
			PAGE_PDE_INDEX(virtual_addr),
			struct_idx - 1,
			true);

	// Get parent
	uint64_t *parent = _memory_struct_parent(virtual_addr, struct_idx);

	// Dispose previous structure
	if (0 != (*parent & PAGE_FLAG_PRESENT))
		_memory_struct_dispose(
				PAGE_PML4E_INDEX(virtual_addr),
				PAGE_PDPE_INDEX(virtual_addr),
				PAGE_PDE_INDEX(virtual_addr),
				struct_idx);

	// Set structure
	*parent = PAGE_PHYSICAL(struct_ptr) | PAGE_FLAG_USER | PAGE_FLAG_PRESENT;
}

//- Address Spaces -------------------------------------------------------------

uintptr_t memory_space_initial = 0;

uintptr_t memory_space_create() {
    // Allocate frame for PML4
    uintptr_t pml4_phys = frame_alloc();

    // Map PML4 to helper page in current address space
    memory_map(
	     MEMORY_SPACE_HELPER_VADDR, pml4_phys,
	     PAGE_FLAG_WRITEABLE);

    uint64_t *pml4_ptr = (uint64_t *) (MEMORY_SPACE_HELPER_VADDR);

    // Get current PML4
    uint64_t *current_pml4_ptr = (uint64_t *) (MEMORY_SPACE_PML4_VADDR);

    // Setup recursive mapping
    pml4_ptr[511] = pml4_phys | PAGE_FLAGS_RECURSIVE;

    // Setup kernel mapping
    pml4_ptr[510] = current_pml4_ptr[510];

    // Unmap helper page
    memory_unmap(MEMORY_SPACE_HELPER_VADDR);

    return pml4_phys;
}

void memory_space_dispose() {
    // Is initial PML4?
    if (memory_space_get() == memory_space_initial)
	PANIC("Failed trying to dipose initial address space.");

    // Dispose structures (except the kernel and recursive ones)
    uintptr_t pml4 = memory_space_get();
    size_t pml4e, pdpe, pde, pte;
    uint64_t *page;
    
    for (pml4e = 0; pml4e < 512; ++pml4e) {
        // Kernel or recursive?
        if (pml4e >= 510)
            break;
        
        // Children
        for (pdpe = 0; pdpe < 512; ++pdpe) {
            // Children
            for (pde = 0; pde < 512; ++pde) {
                // Children
                for (pte = 0; pte < 512; ++pte) {
                    // Get PTE
                    page = (uint64_t *) PAGE_VIRT_PTE(pml4e, pdpe, pde, pte);
                
                    // Free frame
                    // Makes the assumption that all frames belonged to the
                    // disposed address space
                    frame_free(PAGE_PHYSICAL(*page));
                }
                
                // Get PDE
                page = (uint64_t *) PAGE_VIRT_PDE(pml4e, pdpe, pde);
                
                // Free frame
                frame_free(PAGE_PHYSICAL(*page));
            }
            
            // Get PDPE
            page = (uint64_t *) PAGE_VIRT_PDPE(pml4e, pdpe);
            
            // Free frame
            frame_free(PAGE_PHYSICAL(*page));
        }
        
        // Get PML4E
        page = (uint64_t *) PAGE_VIRT_PML4E(pml4e);
        
        // Free frame
        frame_free(PAGE_PHYSICAL(*page));
    }

    // Switch to initial space
    memory_space_switch(memory_space_initial);

    // Free PML4
    frame_free(pml4);
}
