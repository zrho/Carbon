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

#include <stdlib.h>

#define LCG_M                   0x1000000000000
#define LCG_A                   25214903917
#define LCG_C                   11

extern unsigned int rand_seed;

int rand(void) {
    // Generate next number
    uint64_t next;
    rand_seed = next = (LCG_A * rand_seed + LCG_C) % LCG_M;

    // Return bits 16-47
    return (int) ((next >> 16) & 0xFFFFFFFF);
}
