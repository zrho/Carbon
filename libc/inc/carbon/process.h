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
#include <stdint.h>

//- API - Multitasking - Process -----------------------------------------------

#define PROCESS_MAX 1024

/**
 * Type for process ids.
 */
typedef uint32_t pid_t;

/**
 * Returns the id of the current process.
 *
 * @return Id of current process.
 */
pid_t process_id(void);

/**
 * Returns the id of the current process's parent.
 *
 * @return pid The id of the parent process.
 */
pid_t process_parent_id(void);

/**
 * Kills a process, given its id.
 *
 * May be denied for processes other than the current (if not root).
 *
 * @param pid The id of the process to kill.
 */
void process_kill(pid_t pid);

/**
 * Kills the current process.
 *
 * Same as:
 * <code>process_kill(PROCESS_SELF);</code>
 */
void process_exit() __attribute__((noreturn));
