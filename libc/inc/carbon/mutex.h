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

#pragma once
#include <stdbool.h>
#include <carbon/futex.h>

//- API - Multitasking - Synchronization ---------------------------------------

/**
 * Type for mutexes.
 */
typedef futex_t mutex_t;

/**
 * Initial value for (unlocked) mutexes.
 */
#define MUTEX_INIT 0

/**
 * Waits until the mutex has been unlocked and locks it.
 *
 * @param mutex The mutex to be locked.
 */
void mutex_lock(mutex_t *mutex);

/**
 * Tries to acquire the mutex, but does not wait if it is already locked.
 *
 * @param mutex The mutex to be locked.
 * @return Whether the mutex has been locked.
 */
bool mutex_trylock(mutex_t *mutex);

/**
 * Unlocks the mutex.
 *
 * @param mutex To mutex to be unlocked.
 */
void mutex_unlock(mutex_t *mutex);
