// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#ifndef FILE_H
#define FILE_H

#include "task.h"

typedef struct STREAM STREAM;

TODOLIST *load_todolist(char *filename);
TODOLIST *parse_todolist(char *source, size_t length);
char *stringify_todolist(TODOLIST *todolist);
int save_todolist(TODOLIST *todolist, char *filename);

#endif
