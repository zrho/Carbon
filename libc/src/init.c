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
#include <carbon/process.h>

/**
 * Default write handler for stdout.
 */
static void __stdout_write(FILE *file, fpos_t pos, size_t size, const void *data) {
    extern void debug(const char *str);
    debug((const char *) data);
}

/**
 * Default write handler for stderr.
 */
static void __stderr_write(FILE *file, fpos_t pos, size_t size, const void *data) {
    extern void debug(const char *str);
    debug((const char *) data);
}

/**
 * Initializes the current process.
 */
void _init(void);
void _init(void) {
    extern int main(int argc, char **argv);
    extern FILE _stdout;
    extern char _stdout_buf[];
    extern FILE _stderr;
    extern char _stderr_buf[];

    // Initialize stdout
    stdout = &_stdout;
    stdout->lock = 0;
    stdout->position = 0;
    stdout->flags = 0;
    stdout->buffer = &_stdout_buf;
    stdout->buffer_len = 0;
    stdout->buffer_size = 512 * sizeof(char);
    stdout->buffer_mode = _IONBF;
    stdout->callback_length = 0;
    stdout->callback_read = 0;
    stdout->callback_write = &__stdout_write;
    stdout->next = 0;

    // Initialize stderr
    stderr = &_stderr;
    memcpy(stderr, stdout, sizeof(FILE));
    stderr->callback_write = &__stderr_write;
    stderr->buffer = &_stderr_buf;

    // Execute main program
    main(0, 0);

    // Terminate process
    process_exit();
}
