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

#include <carbon/mutex.h>
#include <pthread.h>
#include <errno.h>

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	// Lock structure
	mutex_spin(&mutex->lock_struct);

	// Locked?
	bool locked = false;

	if (!(locked = mutex_trylock(&mutex->lock))) {
		if (PTHREAD_MUTEX_RECURSIVE == mutex->kind) {
			// Locked by current thread?
			if (mutex->owner == pthread_self()) {
				// Lock
				++mutex->recursion;
				locked = true;
			}
		}
	}

	// Unlock structure
	mutex_free(&mutex->lock_struct);

	return locked;
}
