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
#include <string.h>
#include <carbon/mutex.h>

size_t fwrite(const void *array, size_t size, size_t count, FILE *stream) {
    // Lock file
    mutex_lock(&stream->lock);

    // Has write callback?
    if (0 == stream->callback_write) {
        mutex_unlock(&stream->lock);
        return 0;
    }

    // Calculate total length
    size_t length = size * count;

    // Check buffering mode
    if (_IOFBF == stream->buffer_mode) { // Full Buffering
        // Fits in buffer?
        void *buffer = (void *) ((uintptr_t) stream->buffer + stream->buffer_len);
        size_t buffer_remaining = stream->buffer_size - stream->buffer_len;

        while (length > 0) {
            // Fill buffer
            size_t to_copy = (length > buffer_remaining)
                    ? buffer_remaining : length;

            memcpy(buffer, array, to_copy);
            length -= to_copy;
            stream->buffer_len += to_copy;

            // Is the buffer full and still content remaining?
            if (stream->buffer_len >= stream->buffer_size && length > 0) {
                // Flush the buffer
                __stdio_write(stream, stream->buffer_size, stream->buffer);

                // Reset the buffer
                buffer = stream->buffer;
                buffer_remaining = stream->buffer_size;
                stream->buffer_len = 0;
            }
        }

    } else if (_IONBF == stream->buffer_mode) { // No Buffering
        // Write all given data
        __stdio_write(stream, length, array);

    } else if (_IOLBF == stream->buffer_mode) { // Line Buffering
        // Find last newline
        size_t last_nl_offset = 0;
        const char *chars = (const char *) array;

        for (last_nl_offset = length - 1; last_nl_offset > 0; --last_nl_offset) {
            if ('\n' == chars[last_nl_offset])
                break;
        }

        // When a newline is present
        if (-1 != last_nl_offset) {
            // Flush the buffer
            __stdio_write(stream, stream->buffer_len, stream->buffer);

            // Reset the buffer
            stream->buffer_len = 0;

            // Write everything up to the newline (inclusive)
            __stdio_write(stream, last_nl_offset + 1, array);

            length -= last_nl_offset + 1;
            array = (void *) ((uintptr_t) array + last_nl_offset + 1);
        }

        // Rest fits in buffer?
        if (length <= stream->buffer_size) {
            // Copy to buffer
            memcpy(stream->buffer, array, length);
            stream->buffer_len = length;

        } else {
            // Write directly
            __stdio_write(stream, length, array);
        }
    }

    // Unlock file
    mutex_unlock(&stream->lock);

    // Return the full size
    return size;
}
