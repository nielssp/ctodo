/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

/* Provides task and todo list data structures, and utilities for manipulating
 * them. */
#ifndef TASK_H
#define TASK_H

/* A task. Part of doubly linked list. */
typedef struct TASK {
  char *message; /* Task text */
  int done; /* Done (1) or not done (0). */
  int priority; /* Unused. */
  struct TASK *next; /* Next task in list. */
  struct TASK *prev; /* Previous task in list. */
} TASK;

/* An option. Part of singly linked list. */
typedef struct OPTION {
  char *key; /* Option key, left side of '='. */
  char *value; /* Option value, right side of '='. */
  struct OPTION *next; /* Next option in list. */
} OPTION;

/* A list of tasks and options. */
typedef struct {
  char *title; /* List title. */
  TASK *first; /* First task in list. */
  TASK *last; /* Last task in list. */
  OPTION *first_option; /* First option in list. */
  OPTION *last_option; /* Last option in list. */
} TODOLIST;

/* Delete a list and all associated tasks and options. */
void delete_todolist(TODOLIST *todolist);

/* Remove a task from a list and delete it. */
void delete_task(TASK *delete, TODOLIST *list);
/* Add a task to the end of a list. */
void add_task(TODOLIST *list, char *message, int done, int priority);
/* Insert a task before another one. */
void insert_task(TODOLIST *list, TASK *next, char *message, int done, int priority);
/* Move a task up. */
void move_task_up(TODOLIST *list, TASK *task);
/* Move a task down. */
void move_task_down(TODOLIST *list, TASK *task);

/* Get the value of an option. */
char *get_option(TODOLIST *todolist, const char *key);
/* Get a copy of the value of an option, i.e. the value persists even if the
 * list is later deleted. Must be freed manually. */
char *copy_option(TODOLIST *todolist, const char *key);
/* Get binary value of an option (0 if not set or "0", 1 otherwise). */
int get_option_bit(TODOLIST *todolist, const char *key);
/* Set value of an option. */
void set_option(TODOLIST *list, const char *key, const char *value);
/* Get binary value of an option. */
void set_option_bit(TODOLIST *todolist, const char *key, int bit);

#endif
