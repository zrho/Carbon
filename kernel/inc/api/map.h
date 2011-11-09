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

//- Memory Map -----------------------------------------------------------------

#define MEMORY_BOOT_INFO_VADDR    (MEMORY_SPACE_RECURSIVE_VADDR - 0x001000)
#define MEMORY_VIDEO_VADDR        (MEMORY_SPACE_RECURSIVE_VADDR - 0x002000)
#define MEMORY_GDT64_VADDR        (MEMORY_SPACE_RECURSIVE_VADDR - 0x003000)
#define MEMORY_LAPIC_VADDR        (MEMORY_SPACE_RECURSIVE_VADDR - 0x004000)
#define MEMORY_SPACE_HELPER_VADDR (MEMORY_SPACE_RECURSIVE_VADDR - 0x005000)
#define MEMORY_PROCESS_MAP_VADDR  (MEMORY_SPACE_RECURSIVE_VADDR - 0x007000) // 8kB
#define MEMORY_THREAD_MAP_VADDR   (MEMORY_SPACE_RECURSIVE_VADDR - 0x207000) // 2MB

#define MEMORY_VIDEO_PADDR           0xB8000

#define MEMORY_MODULES_VADDR         0xFFFFFF2000000000
#define MEMORY_FRAMES_VADDR          0xFFFFFF4000000000
#define MEMORY_HEAP_VADDR            0xFFFFFF6000000000

#define MEMORY_SPACE_RECURSIVE_VADDR 0xFFFFFF8000000000
#define MEMORY_SPACE_PML4_VADDR      0xFFFFFFFFFFFFF000

#define MEMORY_USER_STACK_VADDR      0xFFFFFE8000000000
#define MEMORY_IPC_SEND_BUFFER_VADDR 0xFFFFFE0000000000
#define MEMORY_IPC_RECV_BUFFER_VADDR 0xFFFFFD8000000000
