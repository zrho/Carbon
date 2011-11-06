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
#include <carbon/mutex.h>
#include <pthread.h>
#include <errno.h>

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	// Lock structure
	mutex_spin(&mutex->lock_struct);

	// Normal?
	if (PTHREAD_MUTEX_NORMAL == mutex->kind) {
		// Simply wait to acquire lock
		mutex_spin(&mutex->lock);

	// Currently locked?
	} else if (!mutex_trylock(&mutex->lock)) {

		// Current thread holds lock?
		if (pthread_self() == mutex->owner) {
			// Recursive?
			if (PTHREAD_MUTEX_RECURSIVE == mutex->kind) {
				++mutex->recursion;
				mutex_free(&mutex->lock_struct);
				return 0;

			} else {// must be errorcheck
				mutex_free(&mutex->lock_struct);
				return EDEADLCK;
			}
		}

		// Unlock structure
		mutex_free(&mutex->lock_struct);

		// Wait to acquire lock
		// (Looks like a racing condition first, but what is actually
		//  happening at this point is irrelevant, as long as the current
		//  thread does not lock the mutex. Threads can not be interrupted
		//  so we know exactly that this won't happen)
		mutex_spin(&mutex->lock);

		// Set owner
		mutex->owner = pthread_self();

		return 0;
	}

	// Set owner
	mutex->owner = pthread_self();

	// Unlock structure
	mutex_free(&mutex->lock_struct);

	return 0;
}
