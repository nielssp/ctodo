/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

/* Implements an experimental HTTP sync feature.
 *
 * Use by adding the following options to a ctodo file:
 *   # autosync=1 origin=https://my-server/path
 *
 * ctodo downloads a list of tasks with a GET-request, and uploads a list of
 * tasks with a PUT-request.
 *
 * Can be enabled by running cmake with -DSYNC_ENABLE=ON, the default is
 * currently OFF. May change in later versions. */
#ifndef SYNC_H
#define SYNC_H

#include "task.h"

/* Download a ctodo file. */
TODOLIST *pull_todolist(char *origin);
/* Upload a ctodo file. */
int push_todolist(TODOLIST *todolist, char *origin);

/* Merge two ctodo files (NOT IMPLEMENTED). */
int merge_todolist(TODOLIST *mine, TODOLIST *theirs);

#endif
