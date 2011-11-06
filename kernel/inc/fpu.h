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

//- Floating Point Units -------------------------------------------------------

/**
 * Initializes the CPU's SSE, x87 FPU and MMX units.
 */
void fpu_init(void);

/**
 * Saves the SSE, x87 FPU and MMX states to the given region.
 *
 * The region must be 512 bytes in size and aligned on a 16 byte boundary.
 *
 * @param region Region in memory to store the fpu state to.
 */
void fpu_save(void *region);

/**
 * Loads the SSE, x87 FPU and MMX states from the given region.
 *
 * @param region Region in memory to load the fpu state from.
 */
void fpu_load(void *region);
