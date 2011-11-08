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

//- Physical Memory Management -------------------------------------------------

typedef struct frame_stack_t {
	uint64_t address;
	struct frame_stack_t *next;
} PACKED frame_stack_t;

/**
 * A 4kB heap for storing the first 256 frames until the real heap is ready.
 */
extern frame_stack_t frame_alloc_init;

/**
 * The number of frames that have been allocated from the frame_alloc_init heap.
 */
static uint64_t frame_alloc_count = 0;

/**
 * Stack of free frames.
 */
static frame_stack_t *frame_stack_free = 0;

/**
 * Initializes the frame heap.
 *
 * @param info The boot info table.
 */
void frame_init(boot_info_t *info) {
	// Page align free_mem_begin
	uintptr_t mem_begin = memalign(info->free_mem_begin, 0x1000);

	// Free all available memory
	boot_info_mmap_t *mmap = info->mmap;

	while (0 != mmap) {
		// Available?
		if (1 == mmap->available) {
			// Get begin and end address
			uintptr_t addr_begin = mmap->address;
			if (addr_begin < mem_begin) addr_begin = mem_begin;

			uintptr_t addr_end = (mmap->address + mmap->length) & ~0xFFF;

			if (addr_end >= mem_begin) {
                // Add frames
                uintptr_t addr;

                for (addr = addr_begin; addr < addr_end; addr += 0x1000) {
                    frame_free(addr);
                }
			}
		}

		// Next
		mmap = mmap->next;
	}
}

/**
 * Allocates a 4kB chunk of physical memory.
 *
 * @return Physical address of allocated chunk.
 */
uintptr_t frame_alloc(void) {
	// Out of memory?
	if (0 == frame_stack_free)
		PANIC("Out of memory!");

	// Get and remove frame
	frame_stack_t *frame = frame_stack_free;
	frame_stack_free = frame->next;

	uintptr_t address = frame->address;

	// Not in init allocation page?
	if ((uintptr_t) frame < (uintptr_t) &frame_alloc_init ||
		(uintptr_t) frame > (uintptr_t) &frame_alloc_init + 0x1000)
		heap_free(frame);

    return address;
}

/**
 * Frees a chunk of physical memory.
 *
 * @param addr Physical address of chunk to free.
 */
void frame_free(uintptr_t addr) {
	// Init allocation page?
	frame_stack_t *frame;

	if (frame_alloc_count < 256)
		frame = &((&frame_alloc_init)[frame_alloc_count++]);
	else
		frame = (frame_stack_t *) heap_alloc(sizeof(frame_stack_t));

	// Add to stack
	frame->next = frame_stack_free;
	frame_stack_free = frame;

	frame->address = addr;
}
