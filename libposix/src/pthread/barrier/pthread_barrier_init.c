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

#include <pthread.h>
#include <errno.h>

int pthread_barrier_init(
        pthread_barrier_t *barrier,
        const pthread_barrierattr_t *attr,
        unsigned int count) {
    // Count of zero?
    if (0 == count)
        return EINVAL;

    // Initialize barrier
    barrier->count_init = count;
    barrier->count_left = count;
    barrier->event = 0;

    // Initialize mutex
    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_NORMAL);

    pthread_mutex_init(&barrier->lock, &mutexattr);

    pthread_mutexattr_destroy(&mutexattr);
    return 0;
}
