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
#include <api/compiler.h>
#include <api/string.h>

#include <memory.h>
#include <debug.h>

// This allocator defines a slot to be the smallest unit of organization. Slot
// have a size if a power of two, between 2 ^ 3 = 8 and 2 ^ 12 = 4096. Slots
// store the actual data when they are allocated or a structure pointing to the
// next free slot of the same size, when free.

// Slabs are a page-sized collection of n = 2 ^ 12 / s slots of size s. Their
// header is stored in their region's header frame.

// A region is a 1MB area, consisting of 255 slabs and a header frame.
// The header frame contains the region's header in the first 16 bytes, and the
// slab headers in the following.

//- Free Slot Storage ----------------------------------------------------------

/**
 * The contents of a free slot in a slab.
 *
 * Used to keep track of available slots.
 */
typedef struct heap_slot_t {
	struct heap_slot_t *next;
} PACKED heap_slot_t;

/**
 * The size of slots in a slab with the given index.
 */
#define HEAP_SLOT_SIZE(index) (1 << (index + 3))

/**
 * The number of slots in a slab with the given index.
 */
#define HEAP_SLOT_COUNT(index) (1 << (9 - index))

/**
 * Array with linked list of free slots per size.
 *
 * The size of a slot in the list can be calculated as follows:
 * size(index) = 1 << (index + 3)
 *
 * Therefore, the minimum slot size is 1 << 3 = 8 (to fit the
 * heap_free_slot_t structure), and the maximum size is 1 << 12
 * = 0x1000.
 */
static heap_slot_t *heap_slots[9];

//- Free Slab Storage ----------------------------------------------------------

#define HEAP_SLAB_HEADER(region, offset) (region + ((offset & ~0xFFF) >> 8))

/**
 * Info about a frame, stored in the corresponding place in the
 * respective management frame.
 *
 * When the usecount is zero, the slab is empty and can be freed.
 */
typedef struct heap_slab_t {
	uint16_t size;
	uint16_t size_index;
	uint32_t usecount;
	struct heap_slab_t *next;
} PACKED heap_slab_t;

/**
 * A list of free slabs.
 */
static heap_slab_t *heap_slabs = 0;

//- Regions --------------------------------------------------------------------

/**
 * Calculates the address of the region's header frame.
 */
#define HEAP_REGION_HEADER(addr) (addr & ~0xfffff)

/**
 * Calculates the offset of an address in a region.
 */
#define HEAP_REGION_OFFSET(addr) (addr & 0xfffff)

/**
 * The size of a region.
 */
#define HEAP_REGION_SIZE 0x100000

/**
 * The header of a region.
 */
typedef struct heap_region_t {
	uint64_t usecount;
} PACKED heap_region_t;

/**
 * The numbers of available regions.
 */
static uint8_t heap_region_count = 0;

//- Internal -------------------------------------------------------------------

static void _heap_alloc_region() {
	// Address of next region
	uintptr_t region_addr = MEMORY_HEAP_VADDR + HEAP_REGION_SIZE * heap_region_count++;

	// Map header frame
	memory_map(region_addr, frame_alloc(), PAGE_FLAG_GLOBAL | PAGE_FLAG_WRITEABLE);

	// Write slab list
	uint16_t i;
	heap_slab_t *slabs = (heap_slab_t *) region_addr;

	for (i = 1; i < 256; ++i) {
		slabs[i].next = heap_slabs;
		heap_slabs = &slabs[i];
	}
}

static void _heap_alloc_slab(uint8_t slot_idx) {
	// No free slabs?
	if (!heap_slabs)
		_heap_alloc_region();

	// Pick first free slab
	heap_slab_t *slab = heap_slabs;
	heap_slabs = slab->next;

	// Calculate address
	uintptr_t region_addr = HEAP_REGION_HEADER((uintptr_t) slab);
	uintptr_t slab_offset = HEAP_REGION_OFFSET((uintptr_t) slab);
	uintptr_t slab_addr = (slab_offset << 8) + region_addr;

	// Set size
	slab->size = HEAP_SLOT_SIZE(slot_idx);
	slab->size_index = slot_idx;

	// Map slab
	memory_map(slab_addr, frame_alloc(), PAGE_FLAG_GLOBAL | PAGE_FLAG_WRITEABLE);

	// Add slots
	uint16_t count = HEAP_SLOT_COUNT(slot_idx);
	uint16_t i;

	for (i = 0; i < count; ++i) {
		// Write slot header and add slot
		heap_slot_t *slot = (heap_slot_t *) (i * slab->size + slab_addr);
		slot->next = heap_slots[slot_idx];
		heap_slots[slot_idx] = slot;
	}
}

//- Allocation -----------------------------------------------------------------

void *heap_alloc(size_t size) {
	// Greater than a page?
	if (size >= 0x1000)
		PANIC("Cannot heap_alloc more than one page at once.");

	// Get slot size
	size_t slot_idx;

	for (slot_idx = 0; slot_idx <= 9; ++slot_idx)
		if (HEAP_SLOT_SIZE(slot_idx) >= size)
			break;

	// Check if there is a free slot
	if (!heap_slots[slot_idx])
		_heap_alloc_slab(slot_idx);

	// Allocate slot
	heap_slot_t *slot = heap_slots[slot_idx];
	heap_slots[slot_idx] = slot->next;

	// Clear slot
	memset((void *) slot, 0, HEAP_SLOT_SIZE(slot_idx));

	// Return slot
	return (void *) slot;
}

void heap_free(void *ptr) {
	// Get addresses
	uintptr_t region_addr = HEAP_REGION_HEADER((uintptr_t) ptr);
	uintptr_t slab_offset = HEAP_REGION_OFFSET((uintptr_t) ptr);

	// Get slab header
	heap_slab_t *slab = (heap_slab_t *) HEAP_SLAB_HEADER(region_addr, slab_offset);

	// Add slot
	heap_slot_t *slot = (heap_slot_t *) ptr;
	slot->next = heap_slots[slab->size_index];
	heap_slots[slab->size_index] = slot;
}
