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
#include <stdint.h>

int pthread_barrier_wait(pthread_barrier_t *barrier) {
    // Lock barrier
    pthread_mutex_lock(&barrier->lock);

    // Barrier break?
    if (0 < --barrier->count_left) {
        futex_t event = barrier->event;
        pthread_mutex_unlock(&barrier->lock);
        do {
            futex_wait(&barrier->event, event);
        } while (barrier->event == event);

    } else {
        ++barrier->event;
        barrier->count_left = barrier->count_init;
        futex_wake(&barrier->event, INT32_MAX);
        pthread_mutex_unlock(&barrier->lock);
    }

    return 0;
}

