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

#include <carbon/futex.h>
#include <pthread.h>
#include <errno.h>

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    // Backup sequence value
    futex_t seq = cond->seq;

    // Check mutex kind
    if (PTHREAD_MUTEX_RECURSIVE == mutex->kind)
        return EINVAL;

    // Try to bind to mutex
    if (cond->mutex != mutex) {
        // Different mutex already set?
        if (0 != cond->mutex)
            return EINVAL;

        // Try to set
        if (!__sync_bool_compare_and_swap(&cond->mutex, 0, mutex))
            return EINVAL;
    }

    // Unlock the mutex
    pthread_mutex_unlock(mutex);

    // Wait
    futex_wait(&cond->seq, seq);

    // Try to acquire mutex
    pthread_mutex_lock(mutex);

    return 0;
}
