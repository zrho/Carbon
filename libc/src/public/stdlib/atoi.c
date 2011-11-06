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
#include <ctype.h>

int atoi(const char *str) {
	// Negative?
	if ('-' == str[0])
		return atoi(&str[1]);

	// Add the digits while multiplying the preceding,
	// partial number with ten for each new digit
	int i, number;

	for (i = 0, number = 0; isdigit(str[i]); ++i) {
		int digit = str[i] - '0';
		number = number * 10 + digit;
	}

	return number;
}
