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

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
	*detachstate = attr->detachstate;
	return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	attr->detachstate = detachstate;
	return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr) {
	return ENOSYS;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
	return ENOSYS;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
	return ENOSYS;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	return ENOSYS;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param) {
	param->sched_priority = attr->param.sched_priority;
	return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param) {
	attr->param.sched_priority = param->sched_priority;
	return 0;
}

int pthread_attr_getschedpolicy(pthread_attr_t *attr, int *policy) {
	return ENOSYS;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
	return ENOSYS;
}

int pthread_attr_getinheritsched(pthread_attr_t *attr, int *inheritsched) {
	return ENOSYS;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
	return ENOSYS;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope) {
	*contentionscope = PTHREAD_SCOPE_SYSTEM;
	return 0;
}
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
	if (PTHREAD_SCOPE_SYSTEM == contentionscope)
		return 0;
	else
		return EINVAL;
}
