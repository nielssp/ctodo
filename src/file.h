/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

/* Provides functions for saving/loading tasks from files and strings. */
#ifndef FILE_H
#define FILE_H

#include <stdarg.h>

#include "task.h"

/* Load list from file. */
TODOLIST *load_todolist(char *filename);
/* Save list to file. */
int save_todolist(TODOLIST *todolist, char *filename);

/* Parse list from string. */
TODOLIST *parse_todolist(char *source, size_t length);
/* Convert a list back to a string. */
char *stringify_todolist(TODOLIST *todolist);

#endif
