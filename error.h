// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#ifndef ERROR_H
#define ERROR_H

void error(const char *format, ...);
int has_error();
void unset_error();
char *get_last_error();

#endif
