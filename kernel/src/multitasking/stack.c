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

#include <multitasking.h>
#include <debug.h>
#include <memory.h>

//- Stack ----------------------------------------------------------------------

void stack_create(stack_t *stack, process_t *process) {
	DEBUG("Creating stack: ");

    stack->address = MEMORY_USER_STACK_VADDR + process->stack_offset + STACK_LENGTH_MAX;
    process->stack_offset += STACK_LENGTH_MAX;
    stack->length = 0;

    DEBUG_HEX(stack->address);
    DEBUG("\n");

    if (UNLIKELY(stack->address >= MEMORY_USER_STACK_VADDR + STACK_PROCESS_MAX))
        PANIC("Exceeded maximum number of stacks per process.");

    uintptr_t old_addr_space = memory_space_get();

    if (old_addr_space != process->addr_space)
        memory_space_switch(process->addr_space);

    stack_resize(stack, 0x1000, process);

    if (old_addr_space != process->addr_space)
        memory_space_switch(old_addr_space);
}

void stack_dispose(stack_t *stack, process_t *process) {
    // Resize stack to zero
    stack_resize(stack, 0, process);
}

void stack_resize(stack_t *stack, uintptr_t new_len, process_t *process) {
    // Check address space
    if (UNLIKELY(process->addr_space != memory_space_get()))
    	PANIC("Failed trying to resize a stack for a process while not being in "
    	      "its address space.");

    // Greater than maximum length?
    if (UNLIKELY(new_len > STACK_LENGTH_MAX))
        PANIC("Failed trying to increase a stack's size over the maximum stack "
            "size.");

    // Size increased or decreased?
    if (new_len > stack->length) { // Increased
        // Map region
        uintptr_t reg_end = stack->address - stack->length;
        uintptr_t reg_addr;
        uint16_t flags = PAGE_FLAG_WRITEABLE | PAGE_FLAG_USER;

        for (reg_addr = stack->address - new_len; reg_addr < reg_end; reg_addr += PAGE_SIZE) {
        	uintptr_t phys = frame_alloc();
        	DEBUG("Stack: MMap ");
        	DEBUG_HEX(reg_addr);
        	DEBUG(" -> ");
        	DEBUG_HEX(phys);
        	DEBUG(" [");
        	DEBUG_HEX(flags);
        	DEBUG(", ");
            memory_map(reg_addr, phys, flags);
            DEBUG_HEX(*((uint64_t *) (stack->address - 0x8)));
            DEBUG("]\n");
        }

    } else if (new_len < stack->length) {
        // Unmap region
        uintptr_t reg_end = stack->address - new_len;
        uintptr_t reg_addr;

        for (reg_addr = stack->address - stack->length; reg_addr < reg_end; reg_addr += 0x1000) {
            uintptr_t phys = memory_physical(reg_addr);
            memory_unmap(reg_addr);
            frame_free(phys);
        }
    }

    // Set new size
    stack->length = new_len;
}
