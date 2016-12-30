/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "file.h"
#include "stream.h"
#include "error.h"

void skip_whitespace(STREAM *file) {
  int c;
  while ((c = stream_getc(file)) != EOF && isspace(c)) {
  }
  stream_ungetc(c, file);
}

void skip_horizontal_whitespace(STREAM *file) {
  int c;
  while ((c = stream_getc(file)) != EOF && isspace(c) && c != '\n') {
  }
  stream_ungetc(c, file);
}

void skip_line(STREAM *file) {
  int c;
  while ((c = stream_getc(file)) != EOF && c != '\n') {
  }
  stream_ungetc(c, file);
}

/* Read until the end of the line or EOF. */
char *read_string(STREAM *file) {
  int buffersize = 10;
  int c, i = 0;
  char *newbuffer = NULL;
  char *buffer = (char *)malloc(buffersize);
  while ((c = stream_getc(file)) != EOF && c != '\n') {
    buffer[i++] = c;
    if (i >= buffersize) {
      newbuffer = resize_buffer(buffer, buffersize, buffersize + buffersize);
      if (!newbuffer) {
        free(buffer);
        return NULL;
      }
      buffer = newbuffer;
      buffersize += buffersize;
    }
  }
  buffer[i] = 0;
  return buffer;
}

/* Read until the next space, '=', EOL, or EOF. Escape using backslash. */
char *read_option_string(STREAM *file) {
  int buffersize = 10;
  int c, i = 0;
  char *newbuffer = NULL;
  char *buffer = (char *)malloc(buffersize);
  while ((c = stream_getc(file)) != EOF && c != '\n' && c != '=' && c > ' ') {
    if (c == '\\') {
      c = stream_getc(file);
      if (c == EOF) {
        break;
      }
    }
    buffer[i++] = c;
    if (i >= buffersize) {
      newbuffer = resize_buffer(buffer, buffersize, buffersize + buffersize);
      if (!newbuffer) {
        free(buffer);
        return NULL;
      }
      buffer = newbuffer;
      buffersize += buffersize;
    }
  }
  stream_ungetc(c, file);
  buffer[i] = 0;
  return buffer;
}

void read_next_option(STREAM *file, TODOLIST *list) {
  char c;
  char *key = NULL;
  char *value = NULL;
  while (1) {
    skip_horizontal_whitespace(file);
    key = read_option_string(file);
    if (key[0] == '\0') {
      free(key);
      skip_line(file);
      return;
    }
    skip_horizontal_whitespace(file);
    c = stream_getc(file);
    if (c != '=') {
      stream_ungetc(c, file);
      set_option_bit(list, key, 1);
      continue;
    }
    skip_horizontal_whitespace(file);
    value = read_option_string(file);
    set_option(list, key, value);
    free(key);
    free(value);
  }
}

void read_next_task(STREAM *file, TODOLIST *list) {
  char c;
  char *message = NULL;
  int done = 0;
  int priority = 0;
  skip_whitespace(file);
  c = stream_getc(file);
  if (c != '[') {
    if (c == '#') {
      read_next_option(file, list);
      skip_line(file);
      return;
    }
    stream_ungetc(c, file);
    skip_line(file);
    return;
  }
  c = stream_getc(file);
  switch (c) {
    case 'X':
    case 'x':
      done = 1;
      break;
    case ' ':
      done = 0;
      break;
    default:
      stream_ungetc(c, file);
      skip_line(file);
      return;
  }
  c = stream_getc(file);
  if (c != ']') {
    stream_ungetc(c, file);
    skip_line(file);
    return;
  }
  skip_whitespace(file);
  message = read_string(file);
  add_task(list, message, done, priority);
}

TODOLIST *read_todolist(STREAM *input) {
  TODOLIST *list = (TODOLIST *)malloc(sizeof(TODOLIST));
  if (!list) {
    error("Could not allocate memory");
    return NULL;
  }
  list->first = NULL;
  list->last = NULL;
  list->first_option = NULL;
  list->last_option = NULL;
  list->title = read_string(input);
  while (!stream_eof(input)) {
    read_next_task(input, list);
  }
  return list;
}

int touch_file(char *filename) {
  FILE *file = fopen(filename, "w+");
  if (!file) {
    return 0;
  }
  fclose(file);
  return 1;
}

TODOLIST *load_todolist(char *filename) {
  TODOLIST *list = NULL;
  STREAM *file = stream_file(filename, "r");
  if (!file) {
    if (touch_file(filename)) {
      return load_todolist(filename);
    }
    else {
      error("%s", strerror(errno));
      return NULL;
    }
  }
  list = read_todolist(file);
  stream_close(file);
  return list;
}

TODOLIST *parse_todolist(char *source, size_t length) {
  TODOLIST *list = NULL;
  STREAM *input = stream_buffer(source, length);
  list = read_todolist(input);
  stream_close(input);
  return list;
}

size_t escape_string(STREAM *output, const char *str) {
  size_t l = 0;
  char c;
  while ((c = *str) != '\0') {
    if (c == ' ' || c == '\\' || c == '\n' || c == '=') {
      stream_putc('\\', output);
    }
    stream_putc(c, output);
    str++;
  }
  return l;
}

void write_todolist(STREAM *output, TODOLIST *todolist) {
  OPTION *opt = NULL;
  TASK *task = NULL;
  int width = 0;
  stream_printf(output, "%s\n", todolist->title);
  opt = todolist->first_option;
  if (opt) {
    width += stream_printf(output, "#");
    while (opt) {
      if (width >= 80) {
        stream_printf(output, "\n#");
        width = 1;
      }
      width += 2;
      stream_putc(' ', output);
      width += escape_string(output, opt->key);
      stream_putc('=', output);
      width += escape_string(output, opt->value);
      opt = opt->next;
    }
    stream_printf(output, "\n");
  }
  task = todolist->first;
  while (task) {
    stream_printf(output, "[%c] %s\n",
        task->done ? 'X' : ' ', task->message);
    task = task->next;
  }
}

char *stringify_todolist(TODOLIST *todolist) {
  size_t size = 100;
  char *str = (char *)malloc(size);
  STREAM *output = stream_buffer(str, size);
  write_todolist(output, todolist);
  stream_printf(output, "\0");
  str = stream_get_content(output);
  stream_close(output);
  return str;
}

int save_todolist(TODOLIST *todolist, char *filename) {
  STREAM *file = stream_file(filename, "w");
  if (!file) {
    error("%s", strerror(errno));
    return 0;
  }
  write_todolist(file, todolist);
  stream_close(file);
  return 1;
}
