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

#ifndef STDIO_H_
#define STDIO_H_

#include <carbon/mutex.h>
#include <carbon/ipc.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

//- Types ----------------------------------------------------------------------

typedef uint64_t fpos_t;

//- Structures -----------------------------------------------------------------

typedef struct __file {

	// The unique file descriptor
	uint64_t descriptor;

	// Lock for this file
	mutex_t lock;

	// Current stream position
	fpos_t position;

	// Flags (Error/EOF etc.)
	uint8_t flags;

	// The file's buffer
	void *buffer;

	// The current length of the buffer.
	size_t buffer_len;

	// The maximum size of the buffer
	size_t buffer_size;

	// The buffer mode
	uint8_t buffer_mode;

	// Callback for writing to the file
	void (*callback_write)(struct __file *, fpos_t pos, size_t size, const void *data);

	// Callback for reading from the file
	size_t (*callback_read)(struct __file *, fpos_t pos, size_t size, void *data);

	// Callback for determining the length of the file
	size_t (*callback_length)(struct __file *);

	// Next file in linked list of files.
	struct __file *next;

} FILE;

//- Definitions ----------------------------------------------------------------

#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define STDIN_FILENO                        0
#define STDOUT_FILENO                       1
#define STDERR_FILENO                       2

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define FILE_FLAG_EOF (1 << 0)

//- Standard Streams -----------------------------------------------------------

FILE *stdout;
FILE *stdin;
FILE *stderr;

//- File List ------------------------------------------------------------------

extern FILE *__stdio_files;
extern mutex_t __stdio_files_mutex;

//- File Manipulation Functions ------------------------------------------------

FILE *fopen(const char *path, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *fp);

FILE *tmpfile(void);

int rename(const char *oldname, const char *newname);
int remove(const char *filename);

int fclose(FILE *fp);

void rewind(FILE *stream);

//- I/O Manipulation Functions -------------------------------------------------

#define feof(fp) (0 != (fp & FILE_FLAG_EOF))
#define ftell(fp) (fp->position)

#define fgetpos(fp, ptr) (ptr = &fp->position)
#define fsetpos(fp, ptr) (fp->position = *ptr)

#define getc(stream)    fgetc(stream)
#define getchar()       fgetc(stdin)
#define gets(str)       fgets(str, INT32_MAX, stdin)

#define puts(str)       fputs(str, stdout)
#define putc(c, stream) fputc(c, stream)
#define putchar(c)      putc(c, stdout)

int fflush(FILE *fp);

int fgetc(FILE *fp);
int fgets(char *string, int length, FILE *stream);
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);

int fputc(int c, FILE *fp);
int fputs(const char *str, FILE *stream);
size_t fwrite(const void *array, size_t size, size_t count, FILE *stream);

int fprintf(FILE *stream, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list args);
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int vsprintf(char *buffer, const char *format, va_list args);
char *vsaprintf(const char *format, va_list args);

int fseek(FILE *stream, long offset, int origin);

//- Helper Functions -----------------------------------------------------------

void __stdio_write(FILE *stream, size_t size, const void *data);
size_t __stdio_read(FILE *stream, size_t size, void *data);

char *__format(const char **format, va_list args);


//- Messages -------------------------------------------------------------------

// API identifiers
#define STDIO_API_LOW  0x17cb381ae4834502
#define STDIO_API_HIGH 0xb400f932cfd83921

// Procedure identifiers
#define STDIO_PROC_OPEN  0
#define STDIO_PROC_CLOSE 1
#define STDIO_PROC_READ  2
#define STDIO_PROC_WRITE 3

// Request message: fopen/freopen
typedef struct stdio_open_req_t {
	ipc_request_header_t header;
	uint8_t mode_flags;
	uint32_t path_length;
	uint64_t descriptor;
} __attribute__((packed)) stdio_open_req_t;

// Response message: fopen/freopen
typedef struct stdio_open_resp_t {
	ipc_response_header_t header;
	uint64_t descriptor;
} __attribute__((packed)) stdio_open_resp_t;

// Request message: fclose
typedef struct stdio_close_req_t {
	ipc_request_header_t header;
	uint64_t descriptor;
} __attribute__((packed)) stdio_close_req_t;

// Request message: fwrite/fputc/fputs/...
typedef struct stdio_write_req_t {
	ipc_request_header_t header;
	uint64_t descriptor;
	uint64_t position;
	uint32_t size;
} __attribute__((packed)) stdio_write_req_t;

// Response message: fwrite/fputc/fputs/...
typedef struct stdio_write_resp_t {
	ipc_response_header_t header;
	uint32_t written_size;
} __attribute__((packed)) stdio_write_resp_t;

// Request message: fread/fgetc/fgets/...
typedef struct stdio_read_req_t {
	ipc_request_header_t header;
	uint64_t descriptor;
	uint64_t position;
	uint32_t size;
} __attribute__((packed)) stdio_read_req_t;

// Response message: fread/fgetc/fgets/...
typedef struct stdio_read_resp_t {
	ipc_response_header_t header;
	uint32_t read_size;
} __attribute__((packed)) stdio_read_resp_t;

#endif /* STDIO_H_ */
