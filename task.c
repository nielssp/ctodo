// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#include <stdlib.h>
#include <string.h>

#include "task.h"

void delete_task(TASK *delete, TODOLIST *list) {
  TASK *task = list->first;
  if (task == delete) {
    list->first = delete->next;
    if (delete == list->last) {
      list->last = NULL;
    }
  }
  else {
    while (task) {
      if (task->next == delete) {
        task->next = delete->next;
        if (delete == list->last) {
          list->last = task;
        }
        break;
      }
      task = task->next;
    }
  }
  free(delete->message);
  free(delete);
}

void add_task(TODOLIST *list, char *message, int done, int priority) {
  TASK *task = (TASK *)malloc(sizeof(TASK));
  if (!task) {
    return;
  }
  task->message = message;
  task->done = done;
  task->priority = priority;
  task->next = NULL;
  if (!list->first) {
    list->first = task;
  }
  else {
    list->last->next = task;
  }
  list->last = task;
}

void insert_task(TODOLIST *list, TASK *next, char *message, int done, int priority) {
  TASK *task = (TASK *)malloc(sizeof(TASK));
  if (!task) {
    return;
  }
  task->message = message;
  task->done = done;
  task->priority = priority;
  task->next = next;
  if (list->first == next) {
    list->first = task;
  }
  else {
    TASK *prev = list->first;
    while (prev) {
      if (prev->next == next) {
        prev->next = task;
        break;
      } else {
        prev = prev->next;
      }
    }
  }
}

char *get_option(TODOLIST *todolist, char *key) {
  OPTION *opt = todolist->first_option;
  while (opt) {
    if (strcmp(key, opt->key) == 0) {
      return opt->value;
    }
    opt = opt->next;
  }
  return NULL;
}

char *copy_option(TODOLIST *todolist, char *key) {
  char *copy = NULL;
  char *value = get_option(todolist, key);
  if (value) {
    int l = strlen(value) + 1;
    copy = (char *)malloc(l);
    memcpy(copy, value, l);
  }
  return copy;
}

int get_option_bit(TODOLIST *todolist, char *key) {
  char *value = get_option(todolist, key);
  if (value == NULL) {
    return 0;
  }
  return *value != '0';
}

void set_option(TODOLIST *list, char *key, char *value) {
  OPTION *opt = list->first_option;
  while (opt) {
    if (strcmp(key, opt->key) == 0) {
      free(opt->value);
      opt->value = value;
      return;
    }
    opt = opt->next;
  }
  opt = (OPTION *)malloc(sizeof(OPTION));
  if (!opt) {
    return;
  }
  opt->key = key;
  opt->value = value;
  opt->next = NULL;
  if (!list->first_option) {
    list->first_option = opt;
  }
  else {
    list->last_option->next = opt;
  }
  list->last_option = opt;
}

void set_option_bit(TODOLIST *todolist, char *key, int bit) {
  char *value = (char *)malloc(2);
  value[0] = bit ? '1' : '0';
  value[1] = '\0';
  set_option(todolist, key, value);
}

void delete_todolist(TODOLIST *todolist) {
  TASK *task = todolist->first;
  OPTION *opt = todolist->first_option;
  TASK *temp = NULL;
  OPTION *temp_opt = NULL;
  while (task) {
    temp = task;
    task = task->next;
    free(temp->message);
    free(temp);
  }
  while (opt) {
    temp_opt = opt;
    opt = opt->next;
    free(temp_opt->key);
    free(temp_opt->value);
    free(temp_opt);
  }
  free(todolist->title);
  free(todolist);
}

