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
#include <api/types.h>

//- String Utility Functions ---------------------------------------------------

/**
 * Determines the length of the given string.
 *
 * @param str The string to determine the length of.
 * @return The length of the string.
 */
size_t strlen(const int8_t *str);

/**
 * Compares the given strings and returns whether they are equal.
 *
 * @param a The first string.
 * @param b The second string.
 * @return <tt>1</tt> when equal, <tt>0</tt> otherwise.
 */
uint8_t strcmp(const int8_t *a, const int8_t *b);

/**
 * Copies the given source string to the given destination.
 *
 * @param dest The destination to copy the string to.
 * @param src The string to copy.
 * @return The value of <tt>dest</tt>.
 */
int8_t *strcpy(int8_t *dest, const int8_t *src);

/**
 * Tries to find the position of <tt>needle</tt> in haystack.
 *
 * @param haystack The containing string.
 * @param needle The string to search.
 * @return Address of the first occurence of <tt>needle</tt> in <tt>haystack</tt>
 *  or a null-pointer, if <tt>needle</tt> is not a substring of <tt>haystack</tt>.
 */
int8_t *strstr(const int8_t *haystack, const int8_t *needle);

//- Basic Memory Operations ----------------------------------------------------

/**
 * Copies <tt>n</tt> bytes from <tt>src</tt> to <tt>dest</tt>.
 *
 * @param dest The destination in memory.
 * @param src The source in memory.
 * @param length The length of the memory chunk to copy.
 */
void memcpy(void *dest, void *src, size_t length);

/**
 * Copies the given byte to the memory from <tt>dest</tt> to <tt>dest + length</tt>.
 *
 * @param dest The destination in memory.
 * @param c The byte to copy.
 * @param length The length of the memory chunk to write.
 */
void memset(void *dest, uint8_t c, size_t length);

/**
 * Aligns the given address by the given boundary.
 *
 * @param address The address to align.
 * @param boundary The boundary to align by.
 * @return The aligned address.
 */
uintptr_t memalign(uintptr_t address, size_t boundary);

/**
 * Compares <tt>len</tt> bytes pointed to my the given pointers.
 *
 * @param a First memory chunk.
 * @param b Second memory chunk.
 * @param len Length of memory chunk to compare.
 */
bool memcmp(void *a, void *b, size_t len);
