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
#include <cpu.h>
#include <debug.h>

extern cpu_int_handler_t cpu_int_handlers;
extern cpu_int_handler_t cpu_int_handlers;
extern uintptr_t cpu_int_stubs;
extern cpu_int_entry_t cpu_int_idt;
extern void cpu_int_lidt(void);

/**
 * Initializes interrupt handling by preparing and loading the IDT.
 */
void cpu_int_init() {
    // Clear handler table
    memset(
       (void *) &cpu_int_handlers,
       0, sizeof(uintptr_t) * 256);

    // Clear IDT
    memset(
       (void *) &cpu_int_idt,
       0, sizeof(cpu_int_entry_t) * 256);

    // Set IDT entries
    size_t vector;
    for (vector = 0; vector < 256; ++vector) {
        uintptr_t offset = (&cpu_int_stubs)[vector];
        cpu_int_entry_t *entry = &(&cpu_int_idt)[vector];

        entry->offsetLow = (uint16_t) offset;
        entry->offsetMiddle = (uint16_t) (offset >> 16);
        entry->offsetHigh = (uint32_t) (offset >> 32);
        entry->zero0 = entry->zero1 = 0;
        entry->cs = 0x08;
        entry->flags = 0x8E | 0x60; // TODO: Only set privilege level for selected interrupts
    }

    // Load idt
    cpu_int_lidt();
}

/**
 * Handles an interrupt by dispatching it to the correct handler.
 *
 * Called by cpu_int_handler_common.
 *
 * @todo: Move the #PF and #GP handlers to separate handler function.
 * @param state The state the processor has been interrupted in. May be modified
 *  to alter the return state.
 */
void cpu_int_handle(cpu_int_state_t *state);
void cpu_int_handle(cpu_int_state_t *state) {
    // TODO: Make this faster!
    cpu_int_handler_t handler = (&cpu_int_handlers)[state->vector];

    if (14 == state->vector) {
        DEBUG("PANIC: Page Fault (address ");
        DEBUG_HEX(state->state.r15);
        DEBUG(") caused by ");
        DEBUG_HEX(state->rip);
        DEBUG(".\n");
        while (1);
    }

    if (13 == state->vector)
        PANIC("General Protection Fault!");

    if (0 == handler)
        return;

    handler(state);
}

/**
 * Registers an interrupt handler for the given vector.
 *
 * @param vector The vector to register the handler for.
 * @param callback The handler to register for the given vector.
 */
inline void cpu_int_register(uint8_t vector, cpu_int_handler_t callback) {
    (&cpu_int_handlers)[vector] = callback;
}


