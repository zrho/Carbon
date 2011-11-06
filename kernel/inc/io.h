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

//- I/O Ports ------------------------------------------------------------------

/**
 * Writes the given value to the given port.
 *
 * @param port The port to write to.
 * @param value The value to write.
 */
void io_outb(uint16_t port, uint8_t value);

/**
 * Reads the next 8 bits from the given port.
 *
 * @param port The port to read from.
 * @return The read bits.
 */
uint8_t io_inb(uint16_t port);
