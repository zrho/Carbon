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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <carbon/thread.h>

extern void debug(const char *msg);
extern void debug_hex(uint64_t num);

void *worker(void *number_ptr) {
	uintptr_t number = *((uintptr_t *) number_ptr);
	return (void *) (number * 2);
}

int main(void) {
	pthread_t worker_tid;
	uintptr_t arg, result;

	arg = 0x80;
	result = 0;

	pthread_create(&worker_tid, 0, &worker, &arg);

	debug("Thread created: [tid = ");
	debug_hex(worker_tid);
	debug("]\n");

	pthread_join(worker_tid, (void **) &result);

	debug("Result: ");
	debug_hex(result);
	debug("\n");

    return 0;
}
