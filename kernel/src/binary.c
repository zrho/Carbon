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

#include <api/types.h>
#include <api/string.h>

#include <lib/elf64.h>

#include <debug.h>
#include <memory.h>
#include <binary.h>


//- ELF64 Binary Loading -------------------------------------------------------

static bool _binary_verify_elf64(Elf64_Ehdr *header) {
    return
        header->e_ident[EI_MAG0] == ELFMAG0 &&
        header->e_ident[EI_MAG1] == ELFMAG1 &&
        header->e_ident[EI_MAG2] == ELFMAG2 &&
        header->e_ident[EI_MAG3] == ELFMAG3 &&
        header->e_ident[EI_CLASS] == ELFCLASS64 &&
        header->e_ident[EI_DATA] == ELFDATA2LSB &&
        header->e_ident[EI_VERSION] == EV_CURRENT;
}

uint64_t binary_load_elf64(void *binary, uint16_t pflags) {
    // Get ELF header
    Elf64_Ehdr *header = (Elf64_Ehdr *) binary;

    // Verify
    if (!_binary_verify_elf64(header)) {
        PANIC("The given binary is corrupt.\n"
              "Failed trying to load a corrupt ELF64 binary.");
    }

    // Copy and map segments
    Elf64_Phdr *segment = (Elf64_Phdr *) (uintptr_t)
	(((uintptr_t) binary) + header->e_phoff);
    size_t i;

    for (i = 0; i < header->e_phnum; ++i) {
		// Ignore non-loadable segments
		if (PT_LOAD == segment->p_type) {

			// Infer page flags
			uint16_t page_flags = PAGE_FLAG_PRESENT | pflags;

			if (segment->p_flags & PF_W)
				page_flags |= PAGE_FLAG_WRITEABLE;

			// Map segment and copy data
			uint64_t offset;

			for (offset = 0; offset < segment->p_memsz; offset += 0x1000) {
					uint64_t vaddr = segment->p_vaddr;

			// Map segment
			uint64_t physical = frame_alloc();
			memory_map(vaddr + offset, physical, page_flags);

			uint64_t target = vaddr + offset;

			// Calculate remaining data for this segment
			int64_t data_remaining = ((int64_t) segment->p_filesz) - offset;

			// Copy remaining data
			if (data_remaining > 0)
				memcpy(
				   (void *) (uintptr_t) target,
				   (void *) (uintptr_t) (((uintptr_t) binary) + segment->p_offset + offset),
				   (data_remaining > 0x1000) ? 0x1000 : data_remaining);

				// Fill rest with zeroes
				if (data_remaining < 0x1000) {
					data_remaining = (data_remaining < 0) ? 0 : data_remaining;

					memset(
					   (void *) (uintptr_t) (target + data_remaining), 0,
					   0x1000 - data_remaining);
				}
			}
		}

		// Next segment
		segment = (Elf64_Phdr *) (((uintptr_t) segment) + sizeof(Elf64_Phdr));
    }

    // Return entry point
    return header->e_entry;
}
