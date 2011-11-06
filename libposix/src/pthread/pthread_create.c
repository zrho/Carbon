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

#include <carbon/thread.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>

extern void __pthread_ret(void);

int pthread_create(
		pthread_t *thread,
		const pthread_attr_t *attr,
		void *(*start)(void *),
		void *args) {
	// Start new thread (TODO: Possible errors)
	tid_t tid = thread_spawn((void *) start, args, (void *) __pthread_ret);

	// Return tid
	*thread = (pthread_t) tid;
	return 0;
}
