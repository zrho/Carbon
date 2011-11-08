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

#include <errno.h>
#include <pthread.h>

int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	attr->kind = PTHREAD_MUTEX_DEFAULT;
	return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
	// Does nothing
	return 0;
}

int pthread_mutexattr_gettype(pthread_mutexattr_t *attr, int *kind) {
	*kind = attr->kind;
	return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind) {
	switch (kind) {
	case PTHREAD_MUTEX_NORMAL:
	case PTHREAD_MUTEX_RECURSIVE:
	case PTHREAD_MUTEX_ERRORCHECK:
		attr->kind = kind;
		return 0;

	default:
		return EINVAL;
	}
}
