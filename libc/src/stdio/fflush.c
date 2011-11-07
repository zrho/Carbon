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

int fflush(FILE *fp) {
    // Lock the file
    mutex_lock(&fp->lock);

    // When no write handler is given, discard the buffer
    if (0 == fp->callback_write) {
        fp->buffer_len = 0;
        mutex_unlock(&fp->lock);
        return 0;
    }

    // Write the buffer
    __stdio_write(fp, fp->buffer_len, fp->buffer);

    // And discard it
    fp->buffer_len = 0;

    // Unlock the file
    mutex_unlock(&fp->lock);
    return 0;
}
