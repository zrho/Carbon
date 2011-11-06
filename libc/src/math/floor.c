/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <math.h>
#include <float.h>

/* If GCC/CLang builtins are available, use them */
#ifdef __GNUC__

float floorf(float x) {
	return __builtin_floorf(x);
}

double floor(double x) {
	return __builtin_floor(x);
}

long double floorl(long double x) {
	return __builtin_floorl(x);
}

#else

float floorf(float x) {
	modff(x, &x);
	if (x < 0.0) x -= 1.0;
	return x;
}

double floor(double x) {
	modf(x, &x);
	if (x < 0.0) x -= 1.0;
	return x;
}

long double floorl(long double x) {
	modfl(x, &x);
	if (x < 0.0) x -= 1.0;
	return x;
}

#endif
