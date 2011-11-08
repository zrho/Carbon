//
// pthread.h
//
// POSIX threads library
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef PTHREAD_H
#define PTHREAD_H

#include <sys/types.h>
#include <carbon/mutex.h>
#include <carbon/futex.h>

//
// Basic types
//

#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
struct timespec
{
  long tv_sec;
  long tv_nsec;
};
#endif

#ifndef _SCHED_PARAM_DEFINED
#define _SCHED_PARAM_DEFINED
struct sched_param
{
  int sched_priority;
};
#endif

//
// POSIX options
//

#define _POSIX_THREADS
//#define _POSIX_READER_WRITER_LOCKS
#define _POSIX_SPIN_LOCKS
#define _POSIX_BARRIERS
//#define _POSIX_THREAD_SAFE_FUNCTIONS
//#define _POSIX_THREAD_ATTR_STACKSIZE
//#define _POSIX_THREAD_PRIORITY_SCHEDULING

//
// POSIX limits
//

#define PTHREAD_KEYS_MAX                      64
#define PTHREAD_STACK_MIN                     0
#define PTHREAD_THREADS_MAX                   2019

//
// POSIX pthread types
//

typedef handle_t pthread_t;
//typedef tls_t pthread_key_t;
typedef int pthread_key_t;

//
// POSIX thread attribute values
//

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

//
// Threads
//

struct pthread_attr
{
  int detachstate;
  struct sched_param param;
};

typedef struct pthread_attr pthread_attr_t;

//
// Once key
//

struct pthread_once
{
  volatile int done;        // Indicates if user function executed
  long started;             // First thread to increment this value
                            // to zero executes the user function
};

typedef struct pthread_once pthread_once_t;

#define PTHREAD_ONCE_INIT       {0, -1}

//
// Mutex
//

#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

struct pthread_mutexattr
{
#ifdef _POSIX_THREAD_PROCESS_SHARED
  int pshared;
#endif
  int kind;
};

typedef struct pthread_mutexattr pthread_mutexattr_t;

struct pthread_mutex
{
  mutex_t lock;
  mutex_t lock_struct;             // 1 locked, 0 unlocked

  long recursion;           // Number of unlocks a thread needs to perform
                            // before the lock is released (recursive mutexes only)
  int kind;                 // Mutex type
  pthread_t owner;          // Thread owning the mutex
};

typedef struct pthread_mutex pthread_mutex_t;

//
// Condition variables
//

struct pthread_condattr
{
  int pshared;
};

typedef struct pthread_condattr pthread_condattr_t;

struct pthread_cond
{
  pthread_mutex_t *mutex;
  futex_t seq;
};

typedef struct pthread_cond pthread_cond_t;

#define PTHREAD_COND_INITIALIZER ((pthread_cond_t) -1)

//
// Barriers
//

#ifdef _POSIX_BARRIERS

#define PTHREAD_BARRIER_SERIAL_THREAD 1

struct pthread_barrierattr
{
  int pshared;
};

typedef struct pthread_barrierattr pthread_barrierattr_t;

struct pthread_barrier
{
  pthread_mutex_t lock;
  futex_t event;
  unsigned int count_left;
  unsigned int count_init;
};

typedef struct pthread_barrier pthread_barrier_t;

#endif

//
// Read-write locks
//

#ifdef _POSIX_READER_WRITER_LOCKS

struct pthread_rwlockattr
{
  int pshared;
};

typedef struct pthread_rwlockattr pthread_rwlockattr_t;

struct pthread_rwlock
{
  pthread_mutex_t mutex;
  handle_t shared_waiters;
  handle_t exclusive_waiters;
  int num_shared_waiters;
  int num_exclusive_waiters;
  int num_active;
  pthread_t owner;
};

typedef struct pthread_rwlock pthread_rwlock_t;

#define PTHREAD_RWLOCK_INITIALIZER {PTHREAD_MUTEX_INITIALIZER, 0, 0, 0, 0, 0, 0}

#endif

//
// Spinlocks
//

#ifdef _POSIX_SPIN_LOCKS

#define SPINLOCK_UNLOCKED    1
#define SPINLOCK_LOCKED      2

struct pthread_spinlock
{
  int lock;
};

typedef struct pthread_spinlock pthread_spinlock_t;

#define PTHREAD_SPINLOCK_INITIALIZER {0, 0};

#endif

//
// POSIX thread routines
//

#ifdef  __cplusplus
extern "C" {
#endif

//
// Thread attribute functions (17/17)
//

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

//
// Thread functions (6/11)
//

//+
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);
//#
int pthread_detach(pthread_t thread);
//+
int pthread_equal(pthread_t t1, pthread_t t2);
//+
void pthread_exit(void *value_ptr);
//+
int pthread_join(pthread_t thread, void **value_ptr);
//+
pthread_t pthread_self(void);
//+
int pthread_cancel(pthread_t thread);
//#
int pthread_setcancelstate(int state, int *oldstate);
//#
int pthread_setcanceltype(int type, int *oldtype);
//#
void pthread_testcancel(void);
//#
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

//
// Scheduling functions (4/4)
//

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param);
int pthread_setconcurrency(int level);
int pthread_getconcurrency(void);

//
// Thread specific data functions (0/4)
//

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
int pthread_key_delete(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);

//
// Mutex attribute functions (4/4+2)
//

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
#ifdef _POSIX_THREAD_PROCESS_SHARED
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
#endif
int pthread_mutexattr_gettype(pthread_mutexattr_t *attr, int *kind);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);

//
// Mutex functions (5/6)
//

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

//
// Condition variable attribute functions (2/2+2)
//

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
#ifdef _POSIX_THREAD_PROCESS_SHARED
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
#endif

//
// Condition variable functions (6/6)
//

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

//
// Barrier attribute functions (4/4)
//

#ifdef _POSIX_BARRIERS
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

//
// Barrier functions (3/3)
//

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier);
#endif

//
// Read-write lock attribute functions (0/0+4)
//

#ifdef _POSIX_READER_WRITER_LOCKS
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

//
// Read-write lock functions (0/0+9)
//

int pthread_rwlock_init(pthread_rwlock_t *lock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *lock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *lock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *lock);
int pthread_rwlock_rdlock(pthread_rwlock_t *lock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *lock, const struct timespec *abstime);
int pthread_rwlock_wrlock(pthread_rwlock_t *lock);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *lock, const struct timespec *abstime);
int pthread_rwlock_unlock(pthread_rwlock_t *lock);
#endif

//
// Spinlock functions (5/5)
//

#ifdef _POSIX_SPIN_LOCKS
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
#endif

//
// Helper functions (0/1)
//

long __abstime2timeout(const struct timespec *abstime);

#ifdef  __cplusplus
}
#endif

#endif

