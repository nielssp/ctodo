// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "stream.h"

#define STREAM_TYPE_FILE 1
#define STREAM_TYPE_BUFFER 2

struct STREAM {
  int type;
  size_t length;
  size_t pos;
  void *obj;
};

STREAM *stream_file(const char *filename, const char *mode) {
  FILE *file = fopen(filename, mode);
  STREAM *stream = NULL;
  if (!file) {
    return NULL;
  }
  stream = (STREAM *)malloc(sizeof(STREAM));
  stream->type = STREAM_TYPE_FILE;
  stream->obj = file;
  return stream;
}

STREAM *stream_buffer(char *buffer, size_t length) {
  STREAM *stream = (STREAM *)malloc(sizeof(STREAM));
  stream->type = STREAM_TYPE_BUFFER;
  stream->obj = buffer;
  stream->length = length;
  stream->pos = 0;
  return stream;
}

char *stream_get_content(STREAM *stream) {
  switch (stream->type) {
    case STREAM_TYPE_FILE:
      return NULL;
    case STREAM_TYPE_BUFFER:
      return stream->obj;
    default:
      return NULL;
  }
}

size_t stream_get_size(STREAM *stream) {
  switch (stream->type) {
    case STREAM_TYPE_FILE:
      return 0;
    case STREAM_TYPE_BUFFER:
      return stream->length;
    default:
      return 0;
  }
}

void stream_close(STREAM *stream) {
  switch (stream->type) {
    case STREAM_TYPE_FILE:
      fclose(stream->obj);
      break;
    case STREAM_TYPE_BUFFER:
      break;
  }
  free(stream);
}

char *resize_buffer(char *buffer, size_t oldsize, size_t newsize) {
  char *new = NULL;
  if (newsize < oldsize) {
    return NULL;
  }
  new = (char *)malloc(newsize);
  if (!new) {
    return NULL;
  }
  memcpy(new, buffer, oldsize);
  free(buffer);
  return new;
}

size_t stream_read(void *ptr, size_t size, size_t nmemb, STREAM *input) {
  size_t bytes, remaining;
  switch (input->type) {
    case STREAM_TYPE_FILE:
      return fread(ptr, size, nmemb, input->obj);
    case STREAM_TYPE_BUFFER:
      bytes = size * nmemb;
      remaining = input->length - input->pos;
      if (remaining <= 0) {
        return 0;
      }
      else if (remaining < bytes) {
        memcpy(ptr, input->obj + input->pos, remaining);
        input->pos += remaining;
        return remaining;
      }
      else {
        memcpy(ptr, input->obj + input->pos, bytes);
        input->pos += bytes;
        return bytes;
      }
    default:
      return 0;
  }
}

int stream_getc(STREAM *input) {
  switch (input->type) {
    case STREAM_TYPE_FILE:
      return fgetc(input->obj);
    case STREAM_TYPE_BUFFER:
      if (input->pos >= input->length) {
        return EOF;
      }
      char *buffer = (char *)input->obj;
      return buffer[input->pos++];
    default:
      return EOF;
  }
}

void stream_ungetc(int c, STREAM *input) {
  switch (input->type) {
    case STREAM_TYPE_FILE:
      ungetc(c, input->obj);
      break;
    case STREAM_TYPE_BUFFER:
      if (input->pos > input->length) {
        input->pos = input->length;
      }
      char *buffer = (char *)input->obj;
      buffer[--input->pos] = (char) c;
      break;
  }
}

int stream_eof(STREAM *input) {
  switch (input->type) {
    case STREAM_TYPE_FILE:
      return feof(input->obj);
    case STREAM_TYPE_BUFFER:
      return input->pos >= input->length;
    default:
      return 1;
  }
}

int stream_vprintf(STREAM *output, const char *format, va_list va) {
  int status = 0, n;
  va_list va2;
  size_t size;
  switch (output->type) {
    case STREAM_TYPE_FILE:
      va_copy(va2, va);
      status = vfprintf(output->obj, format, va2);
      va_end(va2);
      break;
    case STREAM_TYPE_BUFFER:
      size = output->length - output->pos;
      if (size <= 0) {
        output->obj = resize_buffer(output->obj, output->length, output->length + 100);
        output->length += 100;
      }
      while (1) {
        va_copy(va2, va);
        n = vsnprintf(output->obj + output->pos, size, format, va2);
        va_end(va2);
        if (n < 0) {
          return 0;
        }
        if (n < size) {
          output->pos += n;
          break;
        }
        size = n + 1;
        output->obj = resize_buffer(output->obj, output->length, size + output->pos);
        output->length = size + output->pos;
      }
      break;
  }
  return status;
}

int stream_printf(STREAM *output, const char *format, ...) {
  va_list va;
  int status;
  va_start(va, format);
  status = stream_vprintf(output, format, va);
  va_end(va);
  return status;
}

char *string_vprintf(const char *format, va_list va) {
  size_t size = 32;
  char *buffer = (char *)malloc(size);
  va_list va2;
  STREAM *stream = stream_buffer(buffer, size);
  va_copy(va2, va);
  stream_vprintf(stream, format, va2);
  va_end(va2);
  buffer = stream_get_content(stream);
  stream_close(stream);
  return buffer;
}

char *string_printf(const char *format, ...) {
  char *result = NULL;
  va_list va;
  va_start(va, format);
  result = string_vprintf(format, va);
  va_end(va);
  return result;
}
