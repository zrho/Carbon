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
#include <api/bootinfo.h>
#include <lib/multiboot.h>

//- Parsing --------------------------------------------------------------------

boot_info_t *boot_info_parse(multiboot_info_t *mb_info);

//- Relocation -----------------------------------------------------------------

void boot_info_relocate(boot_info_t *info, uint64_t target);
uintptr_t boot_modules_relocate(boot_info_t *info, uintptr_t target);
void boot_modules_map(boot_info_t *info);
