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
#include <irq.h>
#include <cpu.h>
#include <io.h>
#include <debug.h>
#include <multitasking.h>

/**
 * The number of ticks since the system was started.
 *
 * Increased by 1 every time the PIT fires.
 */
time_t irq_pit_ticks = 0;

/**
 * Sets the tick frequency of the PIT.
 *
 * @param hz Tick frequency in Hz.
 */
static void irq_pit_set_freq(uint32_t hz) {
    uint32_t divider;

    divider = 1193180 / hz;

    if ((divider == 0) || (divider >= 65536))
        PANIC("Failed to program PIT to illegal frequency.");

    io_outb(0x43, 0x36);
    io_outb(0x40, (uint8_t) (divider & 0xFF));
    io_outb(0x40, (uint8_t) (divider >> 0x8));
}

/**
 * Initializes the PIT by settings its frequency to IRQ_PIT_FREQ,
 * registering the handler and unmasking the PIT IRQ line.
 */
void irq_pit_init(void) {
    // Set frequency
    irq_pit_set_freq(IRQ_PIT_FREQ);

    // Set handler
    cpu_int_register(IRQ_PIT_VECTOR, &irq_pit_handler);

    // Unmask IRQ line
    irq_pic_unmask(IRQ_PIT_INDEX);
}

/**
 * The interrupt handler for handling PIT timer ticks.
 *
 * @param state The interrupt state.
 */
void irq_pit_handler(cpu_int_state_t *state) {
    // Increase ticks
    ++irq_pit_ticks;

    // Schedule next thread, if current thread's ttl elapsed
    if (0 == thread_current || --thread_current->ttl == 0) {
        thread_switch(scheduler_next(), state);

        if (0 != thread_current)
            thread_current->ttl += THREAD_TTL_GAIN;
    }

    // EOI
    irq_pic_eoi(IRQ_PIT_INDEX);
}
