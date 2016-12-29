// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#include "error.h"

char *error_string = "undefined";

void error(char *error) {
  error_string = error;
}

char *get_last_error() {
  return error_string;
}
