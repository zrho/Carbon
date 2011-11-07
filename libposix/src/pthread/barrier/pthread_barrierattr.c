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

int pthread_barrierattr_init(pthread_barrierattr_t *attr) {
    // Do nothing
    return 0;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {
    // Do nothing
    return 0;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared) {
    return attr->pshared;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
    attr->pshared = pshared;
    return 0;
}
