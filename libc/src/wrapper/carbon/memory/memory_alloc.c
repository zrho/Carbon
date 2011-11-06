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

#include <stdlib.h>
#include <carbon/memory.h>
#include <carbon/ipc.h>
#include <carbon/process.h>

uintptr_t memory_alloc(void) {
	// Allocate buffer
	memory_alloc_req_t *req = (memory_alloc_req_t *) ipc_buffer_size(
			IPC_BUFFER_SEND, sizeof(memory_alloc_req_t));

	// Fill request
	req->header.api_low = MEMORY_API_LOW;
	req->header.api_high = MEMORY_API_HIGH;
	req->header.procedure = MEMORY_PROC_ALLOC;

	// Send message
	ipc_send(sizeof(memory_alloc_req_t), 0, process_parent_id());

	// Get response
	// TODO: Handle error case
	memory_alloc_resp_t *resp = (memory_alloc_resp_t *)
			ipc_buffer_get(IPC_BUFFER_RECV);

	return resp->frame;
}
