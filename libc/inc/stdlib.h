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

#ifndef STDLIB_H_
#define STDLIB_H_

#include <stddef.h> // for NULL and size_t

//- Type Conversion ------------------------------------------------------------

double   atof(const char *str);
int      atoi(const char *str);
long int atol(const char *str);

double      strtod (const char *str, char **endptr);
float       strtof (const char *str, char **endptr);
long double strtold(const char *str, char **endptr);

long int          strtol (const char *ptr, char **endptr, int base);
unsigned long int strtoul(const char *ptr, char **endptr, int base);

long long int          strtoll (const char *ptr, char **endptr, int base);
unsigned long long int strtoull(const char *ptr, char **endptr, int base);

//- Pseudo-random sequence generation ------------------------------------------

int rand(void);
void srand(unsigned int seed);

//- Memory Management ----------------------------------------------------------

void *malloc(size_t size);
void *calloc(size_t n, size_t size_each);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

//- Process Control ------------------------------------------------------------

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

extern struct __atexit_func {
	struct __atexit_func *next;
	void (*function)(void);
} *__atexit_func_list;

void abort(void) __attribute__((noreturn));
int atexit(void (*function)(void));
void exit(int status) __attribute__((noreturn));

char *getenv(const char *name);
int system(const char *command);

//- Sorting, searching and comparison ------------------------------------------

const void *bsearch(
		const void *key, const void *base,
		size_t nmemb, size_t size,
		int (*compare)(const void *, const void *));

void qsort(
		void *base, size_t nmemb, size_t size,
		int (*compare)(const void *, const void *));

//- Mathematics ----------------------------------------------------------------

typedef struct {
	int quot;
	int rem;
} div_t;

typedef struct {
	long int quot;
	long int rem;
} ldiv_t;

typedef struct {
	long long int quot;
	long long int rem;
} lldiv_t;

int abs(int number);
long int labs(long int number);
long long int llabs(long long int number);

div_t div(int numerator, int denominator);
ldiv_t ldiv(long numerator, long denominator);
lldiv_t lldiv(long long numerator, long long denominator);

#endif /* STDLIB_H_ */
