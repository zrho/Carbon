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

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	// Lock structure
	mutex_spin(&mutex->lock_struct);

	// Recursive?
	if (PTHREAD_MUTEX_RECURSIVE == mutex->kind) {
		// Locked by current thread?
		if (0 == mutex->lock && mutex->owner == pthread_self()) {
			// Decrease recursion
			if (0 != mutex->recursion) {
				--mutex->recursion;
				return 0;
			}
		}
	}

	// Unlock
	mutex_free(&mutex->lock);
	mutex_free(&mutex->lock_struct);

	return 0;
}
