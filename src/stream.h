/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

/* A simple stream wrapper that allows ctodo to save/load from both files and
 * strings. Also contains some utilities for working with strings and buffers. */
#ifndef STREAM_H
#define STREAM_H

#include <stdlib.h>
#include <stdarg.h>

/* Stream type */
typedef struct STREAM STREAM;

/* Open a file as a stream (see fopen()). */
STREAM *stream_file(const char *filename, const char *mode);
/* Open a buffer as a stream. */
STREAM *stream_buffer(char *buffer, size_t length);
/* Get buffer content (only for buffer streams). */
char *stream_get_content(STREAM *stream);
/* Get buffer size (only for buffer streams). */
size_t stream_get_size(STREAM *stream);
/* Close stream. */
void stream_close(STREAM *stream);

/* Read from a stream (see fread()). */
size_t stream_read(void *ptr, size_t size, size_t nmemb, STREAM *stream);
/* Read a character (see fgetc()). */
int stream_getc(STREAM *input);
/* Push a character (see ungetc()). */
void stream_ungetc(int c, STREAM *input);
/* Check for EOF (see feof()). */
int stream_eof(STREAM *input);
/* Write a character (see fputc()). */
int stream_putc(int c, STREAM *output);
/* Print to stream (see vfprintf()) */
int stream_vprintf(STREAM *output, const char *format, va_list va);
/* Print to stream (see fprintf()) */
int stream_printf(STREAM *output, const char *format, ...);

/* Like vsprintf(), but automatically creates a large enough buffer. */
char *string_vprintf(const char *format, va_list va);
/* Like sprintf(), but automatically creates a large enough buffer. */
char *string_printf(const char *format, ...);

/* Resize a buffer. */
char *resize_buffer(char *buffer, size_t oldsize, size_t newsize);

#endif
