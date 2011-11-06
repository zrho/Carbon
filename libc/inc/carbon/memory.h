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
#include <carbon/process.h>
#include <carbon/ipc.h>

//- API - Memory ---------------------------------------------------------------

// Flags for memory mapping
#define MEMORY_FLAG_WRITEABLE (1 << 0)
#define MEMORY_FLAG_NX        (1 << 1)

/**
 * Allocates a frame of physical memory.
 *
 * May be denied (if not root).
 *
 * @return Physical address of the allocated frame or zero if no frame
 *  could be allocated.
 */
uintptr_t memory_alloc(void);

/**
 * Frees an allocated frame of physical memory.
 *
 * May be denied (if not root).
 *
 * @param phys Physical address of the frame to free.
 */
void memory_free(uintptr_t phys);

/**
 * Maps the given virtual address to the physical one in the address space of a
 * process, given its pid.
 *
 * May be denied (if not root).
 *
 * @param virt The virtual address to map.
 * @param phys The physical address to map to.
 * @param pid The id of the process in whose address space to perform the
 *  mapping.
 * @param flags The flags with which to perform the mapping.
 */
void memory_map(uintptr_t virt, uintptr_t phys, uint8_t flags, pid_t pid);

/**
 * Unmaps the page at the given virtual address in the address space of a process,
 * given its id.
 *
 * May be denied (if not root).
 *
 * @param virt The virtual address of the page to unmap.
 * @param pid The id of the process in whose address space to unmap the page.
 */
void memory_unmap(uintptr_t virt, pid_t pid);

//- API - Memory - Messages ----------------------------------------------------

// API identifiers
#define MEMORY_API_LOW  0xd1f79a2dda5b4bbc
#define MEMORY_API_HIGH 0x92c06d23177324e8

// Procedure identifiers
#define MEMORY_PROC_MAP   0
#define MEMORY_PROC_UNMAP 1
#define MEMORY_PROC_FREE  2
#define MEMORY_PROC_ALLOC 3

// Request message: memory_map
typedef struct memory_map_req_t {
	// The message's header
	ipc_request_header_t header;

	// The virtual address to map.
	uint64_t virtual;

	// The physical address to map to.
	uint64_t physical;

	// The flags for mapping.
	uint8_t flags;

	// The id of the process in whose address space to map.
	pid_t pid;
} __attribute__((packed)) memory_map_req_t;

// Request message: memory_unmap
typedef struct memory_unmap_req_t {
	ipc_request_header_t header;
	uint64_t virtual;
	pid_t pid;
} __attribute__((packed)) memory_unmap_req_t;

// Request message: memory_free
typedef struct memory_free_req_t {
	ipc_request_header_t header;
	uint64_t physical;
} __attribute__((packed)) memory_free_req_t;

// Request message: memory_alloc
typedef struct memory_alloc_req_t {
	ipc_request_header_t header;
} __attribute__((packed)) memory_alloc_req_t;

// Response message: memory_alloc
typedef struct memory_alloc_resp_t {
	ipc_response_header_t header;
	uint64_t frame;
} __attribute__((packed)) memory_alloc_resp_t;
