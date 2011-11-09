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

#ifndef PTHREAD_H
#define PTHREAD_H

#include <sys/types.h>
#include <sys/sched.h>
#include <carbon/mutex.h>
#include <carbon/futex.h>
#include <limits.h>
#include <time.h>

//- POSIX Options --------------------------------------------------------------

#define _POSIX_THREADS
#define _POSIX_SPIN_LOCKS
#define _POSIX_BARRIERS

//- POSIX Limits ---------------------------------------------------------------

#define PTHREAD_STACK_MIN   0x0
#define PTHREAD_THREADS_MAX THREAD_MAX
#define PTHREAD_KEYS_MX     64

//- POSIX pthread types --------------------------------------------------------

typedef handle_t pthread_t;
typedef int pthread_key_t;

//- POSIX thread attribute values ----------------------------------------------

#define PTHREAD_CREATE_JOINABLE       0
#define PTHREAD_CREATE_DETACHED       1

#define PTHREAD_INHERIT_SCHED         0
#define PTHREAD_EXPLICIT_SCHED        1

#define PTHREAD_SCOPE_PROCESS         0
#define PTHREAD_SCOPE_SYSTEM          1

#define PTHREAD_CANCEL_ENABLE         0
#define PTHREAD_CANCEL_DISABLE        1

#define PTHREAD_CANCEL_ASYNCHRONOUS   0
#define PTHREAD_CANCEL_DEFERRED       1

#define PTHREAD_PROCESS_PRIVATE       0
#define PTHREAD_PROCESS_SHARED        1

#define PTHREAD_SCHED_DEFAULT         0

//- Constants ------------------------------------------------------------------

#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

//- Structures -----------------------------------------------------------------

typedef struct pthread_attr {
    int detachstate;
    sched_param_t param;
} pthread_attr_t;

typedef struct pthread_once {
    int counter;
} pthread_once_t;

typedef struct pthread_mutexattr {
    int kind;
} pthread_mutexattr_t;

typedef struct pthread_mutex {
    mutex_t lock;
    mutex_t lock_struct;

    int kind;
    pthread_t owner;
    long recursion;
} pthread_mutex_t;

typedef struct pthread_condattr {
    // Nothing here
} pthread_condattr_t;

typedef struct pthread_cond {
    pthread_mutex_t *mutex;
    futex_t seq;
} pthread_cond_t;

typedef struct pthread_barrierattr {
    int pshared;
} pthread_barrierattr_t;

typedef struct pthread_barrier {
    pthread_mutex_t lock;
    futex_t event;
    unsigned int count_left;
    unsigned int count_init;
} pthread_barrier_t;

typedef struct pthread_spinlock {
    int lock;
} pthread_spinlock_t;

//- Threads --------------------------------------------------------------------

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedpolicy(pthread_attr_t *attr, int *policy);
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getinheritsched(pthread_attr_t *attr, int *inheritsched);
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);
int pthread_detach(pthread_t thread);
int pthread_equal(pthread_t t1, pthread_t t2);
void pthread_exit(void *value_ptr);
int pthread_join(pthread_t thread, void **value_ptr);
pthread_t pthread_self(void);
int pthread_cancel(pthread_t thread);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
void pthread_testcancel(void);
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

//- Scheduling -----------------------------------------------------------------

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param);
int pthread_setconcurrency(int level);
int pthread_getconcurrency(void);

//- Key ------------------------------------------------------------------------

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
int pthread_key_delete(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);

//- Mutex ----------------------------------------------------------------------

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_gettype(pthread_mutexattr_t *attr, int *kind);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

//- Condition Variable ---------------------------------------------------------

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

//- Barrier --------------------------------------------------------------------

int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier);

//- Spinlock -------------------------------------------------------------------

int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);

#endif
