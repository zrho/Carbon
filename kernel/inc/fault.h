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
#include <api/types.h>
#include <cpu.h>

#define FAULT_GP_VECTOR 13
#define FAULT_PF_VECTOR 14
#define FAULT_XF_VECTOR 19

void fault_install(void);
void fault_gp(cpu_int_state_t *state);
void fault_pf(cpu_int_state_t *state);
void fault_xf(cpu_int_state_t *state);
