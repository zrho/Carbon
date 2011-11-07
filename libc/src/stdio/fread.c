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

#include <stdio.h>
#include <carbon/mutex.h>

size_t fread(void *ptr, size_t size, size_t count, FILE *stream) {
    // Lock stream
    mutex_lock(&stream->lock);

    // Read bytes
    size_t length = size * count;
    size_t bytes_read = __stdio_read(stream, length, ptr);
    size_t blocks_read = bytes_read / size;

    // Set EOF flag?
    if (bytes_read < length)
        stream->flags |= FILE_FLAG_EOF;

    // Unlock stream
    mutex_unlock(&stream->lock);
    return blocks_read;
}
