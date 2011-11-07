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
#include <errno.h>
#include <carbon/mutex.h>

int fseek(FILE *stream, long offset, int origin) {
    // Lock stream
    mutex_lock(&stream->lock);

    // Calculate new position
    fpos_t position;

    if (SEEK_SET == origin)
        position = (fpos_t) offset;
    else if (SEEK_CUR == origin)
        position = (fpos_t) ((long) stream->position + offset);
    else if (SEEK_END == origin) {
        // Length callback given?
        if (0 == stream->callback_length)
            return -1;

        // Get length and calculate position
        size_t length = stream->callback_length(stream);
        position = (fpos_t) ((long) length + offset);

    } else {
        // Fail
        mutex_unlock(&stream->lock);
        return -1;
    }

    // Set new position
    stream->position = position;

    // Reset EOF flag
    stream->flags &= ~FILE_FLAG_EOF;

    // Unlock file
    mutex_unlock(&stream->lock);
    return 0;
}
