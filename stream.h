// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#ifndef STREAM_H
#define STREAM_H

typedef struct STREAM STREAM;

STREAM *stream_file(const char *filename, const char *mode);
STREAM *stream_buffer(char *buffer, size_t length);
char *stream_get_content(STREAM *stream);
size_t stream_get_size(STREAM *stream);
void stream_close(STREAM *stream);

size_t stream_read(void *ptr, size_t size, size_t nmemb, STREAM *stream);
int stream_getc(STREAM *input);
void stream_ungetc(int c, STREAM *input);
int stream_eof(STREAM *input);
int stream_vprintf(STREAM *output, const char *format, va_list va);
int stream_printf(STREAM *output, const char *format, ...);

char *string_vprintf(const char *format, va_list va);
char *string_printf(const char *format, ...);

char *resize_buffer(char *buffer, size_t oldsize, size_t newsize);

#endif
