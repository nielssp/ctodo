// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#include "stdarg.h"

#include "error.h"
#include "stream.h"

char *error_string = NULL;

void error(const char *format, ...) {
  va_list va;
  char *old = error_string;
  va_start(va, format);
  error_string = string_vprintf(format, va);
  va_end(va);
  if (old) {
    free(old);
  }
}

int has_error() {
  return error_string != NULL;
}

void unset_error() {
  if (error_string) {
    free(error_string);
    error_string = NULL;
  }
}

char *get_last_error() {
  return error_string;
}
