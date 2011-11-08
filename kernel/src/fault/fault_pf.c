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
#include <cpu.h>
#include <fault.h>
#include <debug.h>
#include <multitasking.h>

/**
 * Fault Handler: Page Fault
 *
 * Kernel: Panic
 * User: Handle stack increase or terminate otherwise.
 */
void fault_pf(cpu_int_state_t *state) {
    // Is in kernel?
    if (state->cs == 0x8) {
        console_print("PANIC: Page Fault in kernel at ");
        console_print_hex(state->rip);
        console_print(" regarding address ");
        console_print_hex(state->state.r15); // TODO: Read CR2 directly
        console_print(".\n");
        while (1);
    }

    // Is stack?
    uintptr_t address = state->state.r15;
    uintptr_t stack_end = thread_current->stack.address;

    if (address < stack_end && address >= stack_end - STACK_LENGTH_MAX) {
        // Resize stack
        size_t stack_size = stack_end - (address & ~0xFFF);
        stack_resize(&thread_current->stack, stack_size, process_current);
        return;
    }

    // TODO: Remove this debug warning
    DEBUG("Page Fault at ");
    DEBUG_HEX(state->rip);
    DEBUG(" regarding address ");
    DEBUG_HEX(address);
    DEBUG(".\n");

    // Terminate process
    process_terminate(process_current->pid);
    thread_switch(scheduler_next(), state);
}
