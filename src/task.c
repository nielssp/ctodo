// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#include <stdlib.h>
#include <string.h>

#include "task.h"

void delete_task(TASK *delete, TODOLIST *list) {
  if (delete->prev) {
    delete->prev->next = delete->next;
  }
  else {
    list->first = delete->next;
  }
  if (delete->next) {
    delete->next->prev = delete->prev;
  }
  else {
    list->last = delete->prev;
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
  task->prev = list->last;
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
  task->prev = next->prev;
  if (list->first == next) {
    list->first = task;
  }
  else {
    next->prev->next = task;
  }
  next->prev = task;
}

void move_task_up(TODOLIST *list, TASK *task) {
  if (task->prev) {
    TASK *prev = task->prev;
    task->prev = prev->prev;
    if (task->prev) {
      task->prev->next = task;
    }
    else {
      list->first = task;
    }
    prev->prev = task;
    prev->next = task->next;
    if (prev->next) {
      prev->next->prev = prev;
    }
    else {
      list->last = prev;
    }
    task->next = prev;
  }
}

void move_task_down(TODOLIST *list, TASK *task) {
  if (task->next) {
    TASK *next = task->next;
    task->next = next->next;
    if (task->next) {
      task->next->prev = task;
    }
    else {
      list->last = task;
    }
    next->next = task;
    next->prev = task->prev;
    if (next->prev) {
      next->prev->next = next;
    }
    else {
      list->first = next;
    }
    task->prev = next;
  }
}

char *get_option(TODOLIST *todolist, const char *key) {
  OPTION *opt = todolist->first_option;
  while (opt) {
    if (strcmp(key, opt->key) == 0) {
      return opt->value;
    }
    opt = opt->next;
  }
  return NULL;
}

char *copy_option(TODOLIST *todolist, const char *key) {
  char *copy = NULL;
  char *value = get_option(todolist, key);
  if (value) {
    copy = (char *)malloc(strlen(value) + 1);
    strcpy(copy, value);
  }
  return copy;
}

int get_option_bit(TODOLIST *todolist, const char *key) {
  char *value = get_option(todolist, key);
  if (value == NULL) {
    return 0;
  }
  return *value != '0';
}

void set_option(TODOLIST *list, const char *key, const char *value) {
  OPTION *opt = list->first_option;
  while (opt) {
    if (strcmp(key, opt->key) == 0) {
      free(opt->value);
      opt->value = (char *)malloc(strlen(value) + 1);
      strcpy(opt->value, value);
      return;
    }
    opt = opt->next;
  }
  opt = (OPTION *)malloc(sizeof(OPTION));
  if (!opt) {
    return;
  }
  opt->key = (char *)malloc(strlen(key) + 1);
  strcpy(opt->key, key);
  opt->value = (char *)malloc(strlen(value) + 1);
  strcpy(opt->value, value);
  opt->next = NULL;
  if (!list->first_option) {
    list->first_option = opt;
  }
  else {
    list->last_option->next = opt;
  }
  list->last_option = opt;
}

void set_option_bit(TODOLIST *todolist, const char *key, int bit) {
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

