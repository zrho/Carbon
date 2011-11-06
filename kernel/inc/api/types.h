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

#pragma once

//- Sized Types ----------------------------------------------------------------
typedef unsigned char       uint8_t;
typedef          char        int8_t;
typedef unsigned short      uint16_t;
typedef          short       int16_t;
typedef unsigned int        uint32_t;
typedef          int         int32_t;
typedef unsigned long long  uint64_t;
typedef          long long   int64_t;

//- Special Types --------------------------------------------------------------

typedef struct uuid_t {
    uint32_t lower;
    uint32_t upper;
} uuid_t;

//- Boolean --------------------------------------------------------------------
typedef unsigned char bool;

#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

//- Architecture Types ---------------------------------------------------------
#ifdef __AMD64__
    typedef uint64_t uintptr_t;
    typedef  int64_t  intptr_t;
    typedef uint64_t  size_t;
    #define QWORD_PTR(tp) tp *
#endif
#ifdef __X86__
    typedef uint32_t uintptr_t;
    typedef  int32_t  intptr_t;
    typedef uint32_t  size_t;
    #define QWORD_PTR(tp) uint64_t
#endif
