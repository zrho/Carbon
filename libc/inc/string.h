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

#ifndef STRING_H_
#define STRING_H_

#include <stdint.h>
#include <stddef.h>

//- Memory Functions -----------------------------------------------------------

void *memchr (const void *ptr, uint8_t value, size_t num);
int   memcmp (const void *a, const void *b, size_t size);
void *memcpy (void *dest, const void *src, size_t size);
void *memmove(void *dest, const void *src, size_t size);
void *memset (void *ptr, uint8_t value, size_t size);
void *memclr (void *ptr, size_t size);

//- String - Basic -------------------------------------------------------------

char *strcpy  (char *dest, const char *src);
char *strncpy (char *dest, const char *src, size_t num);
size_t strlcpy(char *dest, const char *src, size_t num);

char *strcat  (char *dest, const char *src);
char *strncat (char *dest, const char *src, size_t num);
size_t strlcat(char *dest, const char *src, size_t num);

int strcmp (const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t num);

size_t strlen (const char *str);
char *strdup (const char *str);

//- String - Advanced ----------------------------------------------------------

char *strchr (const char *str, char chr);
char *strrchr(const char *str, char chr);

char *strstr (const char *haystack, const char *needle);
char *strpbrk(const char *haystack, const char *needle);

size_t strspn (const char *haystack, const char *needle);
size_t strcspn(const char *haystack, const char *needle);

char *strtok (char *str, const char *delimiters);
char *strtok_r(char *str, const char *delimiters, char **ctx);

const char *strerror(int errno);

//- String - Locales -----------------------------------------------------------

int strcoll(const char *dest, const char *src);
size_t strxfrm(char *dest, const char *src, size_t num);

#endif /* STRING_H_ */
