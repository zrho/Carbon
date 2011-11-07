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

int fgets(char *string, int length, FILE *stream) {
    size_t offset;

    for (offset = 0; offset < length - 1; ++offset) {
        // Read single char
        int c = fgetc(stream);

        // Is EOF?
        if (-1 == c) break;

        // Set char
        string[offset] = (char) c;

        // Is newline?
        if ('\n' == (char) c) {
            ++offset;
            break;
        }
    }

    string[offset] = 0;
    return 0;
}
