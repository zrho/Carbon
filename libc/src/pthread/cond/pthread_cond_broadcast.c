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

int pthread_cond_broadcast(pthread_cond_t *cond) {
    // Check for mutex
    if (0 == cond->mutex)
        return 0;

    __sync_fetch_and_add(&cond->seq, 1);
    while (!futex_cmp_requeue(
            &cond->seq,
            cond->seq,
            1,
            (futex_t *) &cond->mutex->lock,
            INT32_MAX));
    return 0;
}
