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

//- IRQs (PIC) -----------------------------------------------------------------

/** @see http://http://wiki.osdev.org/PIC */
#define IO_PIC1                     0x20
#define IO_PIC2                     0xA0
#define IO_PIC1_COMMAND             IO_PIC1
#define IO_PIC1_DATA                (IO_PIC1 + 1)
#define IO_PIC2_COMMAND             IO_PIC2
#define IO_PIC2_DATA                (IO_PIC2 + 1)

#define PIC_ICW1_ICW4	            0x01		/* ICW4 (not) needed */
#define PIC_ICW1_SINGLE	            0x02		/* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4          0x04		/* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL              0x08		/* Level triggered (edge) mode */
#define PIC_ICW1_INIT               0x10		/* Initialization - required! */
 
#define PIC_ICW4_8086               0x01		/* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO               0x02		/* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE          0x08		/* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER         0x0C		/* Buffered mode/master */
#define PIC_ICW4_SFNM               0x10		/* Special fully nested (not) */

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
    io_outb(IO_PIC1_DATA, 0xFF & ~(1 << 2));
    io_outb(IO_PIC2_DATA, 0xFF);
}

void irq_pic_disable() {
    // Disable PIC2
    io_outb(IO_PIC2_DATA, 0xFF);
    
    // Disable PIC1
    io_outb(IO_PIC1_DATA, 0xFF);
} 

void irq_pic_eoi(uint8_t index) {
    // PIC2?
    if (index > 7)
        io_outb(IO_PIC2_COMMAND, 0x20);
    
    // PIC1
    io_outb(IO_PIC1_COMMAND, 0x20);
}

void irq_pic_mask(uint8_t index) {
    // IRQ 2 cannot be masked
    if (UNLIKELY(2 == index)) return;

    // PIC 1 or PIC 2?
    if (index > 7 && index <= 15)
        io_outb(IO_PIC2_DATA, io_inb(IO_PIC2_DATA) | (1 << (index - 8)));
    else if (index <= 7)
        io_outb(IO_PIC1_DATA, io_inb(IO_PIC1_DATA) | (1 << index));
}

void irq_pic_unmask(uint8_t index) {
    // IRQ 2 is always unmasked
    if (2 == index) return;

    // PIC 1 or PIC 2?
    if (index > 7 && index <= 15)
        io_outb(IO_PIC2_DATA, io_inb(IO_PIC2_DATA) & ~(1 << (index - 8)));
    else if (index <= 7)
        io_outb(IO_PIC1_DATA, io_inb(IO_PIC1_DATA) & ~(1 << index));
}
