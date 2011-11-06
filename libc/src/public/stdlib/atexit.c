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

struct __atexit_func *__atexit_func_list = 0;

int atexit(void (*function)(void)) {
	// Allocate new structure
	struct __atexit_func *func =
			(struct __atexit_func *) malloc(sizeof(struct __atexit_func));

	func->function = function;
	func->next = __atexit_func_list;

	// Insert
	__atexit_func_list = func;

	return 0;
}
