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
 * Fault Handler: General Protection Fault
 *
 * Kernel: Panic
 * User: Terminate Process
 */
void fault_gp(cpu_int_state_t *state) {
    // Is in kernel?
    if (state->cs == 0x8) {
        console_print("PANIC: General Protection Fault in kernel at ");
        console_print_hex(state->rip);
        console_print(" (error code: ");
        console_print_hex(state->error_code);
        console_print(").\n");
        while (1);
    }

    // TODO: Remove this debug warning
    DEBUG("General Protection Fault at ");
    DEBUG_HEX(state->rip);
    DEBUG(" (error code: ");
    DEBUG_HEX(state->error_code);
    DEBUG(").\n");

    // Terminate process
    process_terminate(process_current->pid);
    thread_switch(scheduler_next(), state);
}
