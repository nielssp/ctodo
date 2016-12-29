// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#ifndef SYNC_H
#define SYNC_H

#include "task.h"

TODOLIST *pull_todolist(char *origin);
int push_todolist(TODOLIST *todolist, char *origin);

int merge_todolist(TODOLIST *mine, TODOLIST *theirs);

#endif
