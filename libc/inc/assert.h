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

#ifndef ASSERT_H
#define ASSERT_H

#include <stdio.h>
#include <stdlib.h>

// Undefine if already defined
#ifdef assert
#undef assert
#endif

// Define, depending on NDEBUG presence
#ifndef NDEBUG
    #define assert(expr) \
    do {\
        if (!(expr)) {\
            fprintf(stderr, "assertion failed: %s, file %s, line %s, function %s", #expr, __FILE__, __LINE__, __func__); \
            abort();\
        }\
    } while (0);
#else
    #define assert(expr) assert(ignore) ((void) 0)
#endif

#endif/*ASSERT_H*/
