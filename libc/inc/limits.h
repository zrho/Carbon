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

#ifndef LIMITS_H
#define LIMITS_H

#include <carbon/thread.h>

//- Runtime Invariant Values ---------------------------------------------------

#define PAGESIZE            0x1000
#define PAGE_SIZE           PAGESIZE

//- Numerical Limits -----------------------------------------------------------

#define CHAR_BIT        8
#define SCHAR_MIN       0x80
#define SCHAR_MAX       0x7F
#define UCHAR_MAX       0xFF
#define CHAR_MIN        0x80
#define CHAR_MAX        0x7F

#define MB_LEN_MAX      4

#define SHRT_MIN        0x8000
#define SHRT_MAX        0x7FFF
#define USHRT_MAX       0xFFFF

#define INT_MIN         0x80000000
#define INT_MAX         0x7FFFFFFF
#define UINT_MAX        0xFFFFFFFF

#define LONG_MIN        0x80000000
#define LONG_MAX        0x7FFFFFFF
#define ULONG_MAX       0xFFFFFFFF

#define LLONG_MIN       0x8000000000000000LL
#define LLONG_MAX       0x7FFFFFFFFFFFFFFFLL
#define ULLONG_MAX      0xFFFFFFFFFFFFFFFFULL

#endif/*LIMITS_H*/

