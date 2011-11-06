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
#include <io.h>

/**
 * Initializes the PIT, programs it to the IRQ_OFFSET vector and masks
 * all IRQs.
 */
void irq_pic_init() {
    // ICW1: Start initialization sequence
    io_outb(IO_PIC1_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
    io_outb(IO_PIC2_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);

    // ICW2: Define PIC vectors
    io_outb(IO_PIC1_DATA, IRQ_OFFSET);
    io_outb(IO_PIC2_DATA, IRQ_OFFSET + 7);

    // ICW3: Continue init sequence
    io_outb(IO_PIC1_DATA, 4);
    io_outb(IO_PIC2_DATA, 2);

    // ICW4: Additional flags
    io_outb(IO_PIC1_DATA, PIC_ICW4_8086);
    io_outb(IO_PIC2_DATA, PIC_ICW4_8086);

    // Mask all
    io_outb(IO_PIC1_DATA, 0xFF & ~(1 << 2)); // Line 2 for 2nd PIC
    io_outb(IO_PIC2_DATA, 0xFF);
}

/**
 * Masks the IRQ with the given index.
 *
 * @param index The index of the IRQ to mask.
 */
void irq_pic_mask(uint8_t index) {
    // IRQ 2 cannot be masked
    if (UNLIKELY(2 == index)) return;

    // PIC 1 or PIC 2?
    if (index > 7 && index <= 15)
        io_outb(IO_PIC2_DATA, io_inb(IO_PIC2_DATA) | (1 << (index - 8)));
    else if (index <= 7)
        io_outb(IO_PIC1_DATA, io_inb(IO_PIC1_DATA) | (1 << index));
}

/**
 * Unmasks the IRQ with the given index.
 *
 * @param index The index of the IRQ to unmask.
 */
void irq_pic_unmask(uint8_t index) {
    // IRQ 2 is always unmasked
    if (2 == index) return;

    // PIC 1 or PIC 2?
    if (index > 7 && index <= 15)
        io_outb(IO_PIC2_DATA, io_inb(IO_PIC2_DATA) & ~(1 << (index - 8)));
    else if (index <= 7)
        io_outb(IO_PIC1_DATA, io_inb(IO_PIC1_DATA) & ~(1 << index));
}

/**
 * Disables the IRQ by masking all interrupts.
 */
void irq_pic_disable() {
    // Disable PIC2
    io_outb(IO_PIC2_DATA, 0xFF);

    // Disable PIC1
    io_outb(IO_PIC1_DATA, 0xFF);
}

/**
 * Signals an EOI (end of interrupt) for the IRQ with the given index.
 *
 * @param index The index of the IRQ to signal an EOI for.
 */
void irq_pic_eoi(uint8_t index) {
    // PIC2?
    if (index > 7)
        io_outb(IO_PIC2_COMMAND, 0x20);

    // PIC1
    io_outb(IO_PIC1_COMMAND, 0x20);
}
