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

#ifndef SCHED_H_
#define SCHED_H_

#define SCHED_OTHER 0   // Other scheduling
#define SCHED_FIFO  1   // FIFO scheduling
#define SCHED_RR    2   // Round robin scheduling

typedef struct sched_param
{
  int sched_priority;
} sched_param_t;

#endif /* SCHED_H_ */
