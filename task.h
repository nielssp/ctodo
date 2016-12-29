// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#ifndef TASK_H
#define TASK_H

typedef struct TASK {
  char *message;
  int done;
  int priority;
  struct TASK *next;
  struct TASK *prev;
} TASK;

typedef struct OPTION {
  char *key;
  char *value;
  struct OPTION *next;
} OPTION;

typedef struct {
  char *title;
  TASK *first;
  TASK *last;
  OPTION *first_option;
  OPTION *last_option;
} TODOLIST;

void delete_task(TASK *delete, TODOLIST *list);
void add_task(TODOLIST *list, char *message, int done, int priority);
void insert_task(TODOLIST *list, TASK *next, char *message, int done, int priority);
void move_task_up(TODOLIST *list, TASK *task);
void move_task_down(TODOLIST *list, TASK *task);

char *get_option(TODOLIST *todolist, char *key);
char *copy_option(TODOLIST *todolist, char *key);
int get_option_bit(TODOLIST *todolist, char *key);
void set_option(TODOLIST *list, char *key, char *value);
void set_option_bit(TODOLIST *todolist, char *key, int bit);
void delete_todolist(TODOLIST *todolist);

#endif
