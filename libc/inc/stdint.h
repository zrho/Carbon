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

#ifndef STDINT_H_
#define STDINT_H_

//- Types ----------------------------------------------------------------------

// Exact
typedef unsigned char       uint8_t;
typedef          char        int8_t;
typedef unsigned short      uint16_t;
typedef          short       int16_t;
typedef unsigned int        uint32_t;
typedef          int         int32_t;
typedef unsigned long long  uint64_t;
typedef          long long   int64_t;

// Minimum
typedef uint64_t uint_least8_t;
typedef int64_t   int_least8_t;
typedef uint64_t uint_least16_t;
typedef int64_t   int_least16_t;
typedef uint64_t uint_least32_t;
typedef int64_t   int_least32_t;
typedef uint64_t uint_least64_t;
typedef int64_t   int_least64_t;

// Fast
typedef uint64_t uint_fast8_t;
typedef int64_t   int_fast8_t;
typedef uint64_t uint_fast16_t;
typedef int64_t   int_fast16_t;
typedef uint64_t uint_fast32_t;
typedef int64_t   int_fast32_t;
typedef uint64_t uint_fast64_t;
typedef int64_t   int_fast64_t;

// Pointer
typedef uint64_t uintptr_t;
typedef  int64_t  intptr_t;

// Maximum
typedef uint64_t uintmax_t;
typedef  int64_t  intmax_t;

// Size
typedef uint64_t size_t;

//- Limits ---------------------------------------------------------------------

#define INT8_MIN -0x80
#define INT8_MAX  0x7F
#define UINT8_MAX 0xFF

#define INT16_MIN -0x8000
#define INT16_MAX 0x7FFF
#define UINT16_MAX 0xFFFF

#define INT32_MIN -0x80000000
#define INT32_MAX 0x7FFFFFFF
#define UINT32_MAX 0xFFFFFFFF

#define INT64_MIN -0x8000000000000000
#define INT64_MAX 0x7FFFFFFFFFFFFFFF
#define UINT64_MAX 0xFFFFFFFFFFFFFFFF

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST8_MAX INT8_MAX
#define UINT_LEAST8_MAX UINT8_MAX

#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST16_MAX INT16_MAX
#define UINT_LEAST16_MAX UINT16_MAX

#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST32_MAX INT32_MAX
#define UINT_LEAST32_MAX UINT32_MAX

#define INT_LEAST64_MIN INT64_MIN
#define INT_LEAST64_MAX INT64_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INT_FAST8_MIN INT64_MIN
#define INT_FAST8_MAX INT64_MAX
#define UINT_FAST8_MAX UINT64_MAX

#define INT_FAST16_MIN INT64_MIN
#define INT_FAST16_MAX INT64_MAX
#define UINT_FAST16_MAX UINT64_MAX

#define INT_FAST32_MIN INT64_MIN
#define INT_FAST32_MAX INT64_MAX
#define UINT_FAST32_MAX UINT64_MAX

#define INT_FAST64_MIN INT64_MIN
#define INT_FAST64_MAX INT64_MAX
#define UINT_FAST64_MAX UINT64_MAX

#define INTPTR_MIN INT64_MIN
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX

#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

#define PTRDIFF_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX

#define WCHAR_MIN 0
#define WCHAR_MAX UINT32_MAX

#define SIZE_MAX UINT64_MAX

#endif /* STDINT_H_ */
