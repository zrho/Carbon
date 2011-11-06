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
#include <stddef.h>
#include <carbon/process.h>

//- API - Inter-Process Communication ------------------------------------------

// Buffer identifiers
#define IPC_BUFFER_SEND 0
#define IPC_BUFFER_RECV 1

// Buffer size
#define IPC_BUFFER_SIZE 0x200000

// Message flags
#define IPC_FLAG_RESPONSE        (1 << 0)
#define IPC_FLAG_IGNORE_RESPONSE (1 << 1)

// Type for handler callbacks
typedef void (*ipc_handler_t)(void *, size_t, pid_t, pid_t);

// Recommended form of request headers to avoid collisions.
typedef struct ipc_request_header_t {
	uint64_t api_low;
	uint64_t api_high;
	uint64_t procedure;
} __attribute__((packed)) ipc_request_header_t;

// Recommended form of response headers.
typedef struct ipc_response_header_t {
	uint64_t error_code;
} __attribute__((packed)) ipc_response_header_t;

/**
 * Sends an IPC message from IPC_BUFFER_SEND to the given target process.
 *
 * If the IPC_FLAG_IGNORE_RESPONSE flag is not set, the response will be moved
 * to IPC_BUFFER_RECV and the current thread will block until the response is
 * ready.
 *
 * @param length The length of the message.
 * @param flags Flags for sending the message.
 * @param target The target process.
 * @return Size of the response message.
 */
size_t ipc_send(size_t length, uint8_t flags, pid_t target);

/**
 * Responds to the message handled by the current message handler, sending the
 * contents of IPC_BUFFER_SEND.
 *
 * Won't return, when this is a valid handler.
 *
 * @param length Length of the message.
 * @param flags Flags for the message.
 */
void ipc_respond(size_t length, uint8_t flags);

/**
 * Resizes one of the current thread's buffers and returns its address.
 *
 * @param buffer The number of the buffer to resize.
 * @param size The new size of the buffer.
 * @return Pointer to beginning of the buffer.
 */
void *ipc_buffer_size(uint8_t buffer, size_t size);

/**
 * Returns the address of one of the current thread's buffers.
 *
 * @param buffer The number of the buffer whose address to return.
 * @return pointer to beginning of the buffer.
 */
void *ipc_buffer_get(uint8_t buffer);

/**
 * Sets the current process's messgae handler.
 *
 * @param handler Pointer to the new message handler, NULL for none.
 */
void ipc_handler(ipc_handler_t handler);
