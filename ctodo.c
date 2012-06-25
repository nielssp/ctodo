/**
 * @file
 * A simple ncurses-based tasklist manager
 *
 * This program is open source.
 * For license terms, see the LICENSE file.
 *
 * @version 1.0
 * @author Niels Sonnich Poulsen (http://ctodo.apakoh.dk)
 * @copyright See LICENSE file
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <config.h>

typedef struct TASK {
  char *message;
  int done;
  int priority;
  struct TASK *next;
} TASK;

typedef struct TODOLIST {
  char *title;
  TASK *first;
  TASK *last;
} TODOLIST;

char *error_string = "undefined";

char *status_str[] = {
  "[Saved]",
  "[Unsaved]",
  "[Unable to save]"
};

struct key_func {
  char key;
  char *func;
} key_funcs[] = {
  {'Q', "Quit"},
  {'S', "Save"},
  {'R', "Reload"},
  {'N', "New"},
  {'E', "Edit"},
  {'D', "Delete"},
  {'T', "Title"},
  {0, NULL}
};

void error(char *error) {
  error_string = error;
}

char *get_last_error() {
  return error_string;
}

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

void skip_whitespace(FILE *file) {
  int c;
  while ((c = fgetc(file)) != EOF && isspace(c)) {
  }
  ungetc(c, file);
}

void skip_line(FILE *file) {
  int c;
  while ((c = fgetc(file)) != EOF && c != '\n') {
  }
  ungetc(c, file);
}

char *increase_buffer(char *buffer, int oldsize, int newsize) {
  char *new = NULL;
  if (newsize < oldsize) {
    return NULL;
  }
  new = (char *)malloc(newsize);
  if (!new) {
    return NULL;
  }
  memcpy(new, buffer, oldsize);
  free(buffer);
  return new;
}

char read_char(FILE *file) {
  if (file) {
    return fgetc(file);
  }
  else {
    return getch();
  }
}

char *read_string(FILE *file) {
  int buffersize = 10;
  int c, i = 0;
  char *newbuffer = NULL;
  char *buffer = (char *)malloc(buffersize);
  while ((c = read_char(file)) != EOF && c != '\n') {
    buffer[i++] = c;
    if (i >= buffersize) {
      newbuffer = increase_buffer(buffer, buffersize, buffersize + buffersize);
      if (!newbuffer) {
        free(buffer);
        return NULL;
      }
      buffer = newbuffer;
      buffersize += buffersize;
    }
  }
  buffer[i] = 0;
  return buffer;
}

void load_next_task(FILE *file, TODOLIST *list) {
  char c;
  char *message = NULL;
  int done = 0;
  int priority = 0;
  skip_whitespace(file);
  c = fgetc(file);
  if (c != '[') {
    ungetc(c, file);
    skip_line(file);
    return;
  }
  c = fgetc(file);
  switch (c) {
    case 'X':
    case 'x':
      done = 1;
      break;
    case ' ':
      done = 0;
      break;
    default:
      ungetc(c, file);
      skip_line(file);
      return;
  }
  c = fgetc(file);
  if (c != ']') {
    ungetc(c, file);
    skip_line(file);
    return;
  }
  skip_whitespace(file);
  message = read_string(file);
  add_task(list, message, done, priority);
}

void delete_todolist(TODOLIST *todolist) {
  TASK *task = todolist->first;
  TASK *temp = NULL;
  while (task) {
    temp = task;
    task = task->next;
    free(temp->message);
    free(temp);
  }
  free(todolist->title);
  free(todolist);
}

int touch_file(char *filename) {
  FILE *file = fopen(filename, "w+");
  if (!file) {
    return 0;
  }
  fclose(file);
  return 1;
}

TODOLIST *load_todolist(char *filename) {
  TODOLIST *list = NULL;
  FILE *file = fopen(filename, "r");
  if (!file) {
    if (touch_file(filename)) {
      return load_todolist(filename);
    }
    else {
      error("Could not access file");
      return NULL;
    }
  }
  list = (TODOLIST *)malloc(sizeof(TODOLIST));
  if (!list) {
    fclose(file);
    error("Could not allocate memory");
    return NULL;
  }
  list->first = NULL;
  list->last = NULL;
  list->title = read_string(file);
  while (!feof(file)) {
    load_next_task(file, list);
  }
  return list;
}

int save_todolist(TODOLIST *todolist, char *filename) {
  TASK *task = NULL;
  FILE *file = fopen(filename, "w");
  if (!file) {
    error("Could not write to file");
    return 0;
  }
  fprintf(file, "%s\n", todolist->title);
  task = todolist->first;
  while (task) {
    fprintf(file, "[%c] %s\n",
        task->done ? 'X' : ' ', task->message);
    task = task->next;
  }
  fclose(file);
  return 1;
}

int print_multiline(int y, int x, char *str, int maxwidth) {
  int sc = 0, tc = 0, lc = 0, bc = 0;
  int splits = 0;
  char *temp = (char *)malloc(maxwidth + 1);
  while (str[sc]) {
    if (tc == 0 && isspace(str[sc])) {
      sc++;
    }
    else {
      temp[tc++] = str[sc++];
    }
    if (tc >= maxwidth || !str[sc]) {
      temp[tc] = 0;
      if (!isspace(str[sc]) && str[sc] != 0) {
        bc = sc;
        while (bc > lc) {
          if (isspace(str[bc])) {
            temp[bc - lc] = 0;
            sc = bc;
            break;
          }
          bc--;
        }
      }
      mvprintw(y++, x, "%s", temp);
      splits++;
      tc = 0;
      lc = sc;
    }
  }
  return splits;
}

char *get_input_edit(char *label, char *buffer) {
  int rows, cols, i, j, ch, bc = 0, cc;
  int lineoffset, linesize, buffersize;
  int bufferlines = 1, cursorpos = 0;
  char *newbuffer = NULL;
  getmaxyx(stdscr, rows, cols);
  lineoffset = strlen(label) + 1;
  linesize = cols - lineoffset;
  buffersize = linesize * bufferlines;
  if (!buffer) {
    buffer = (char *)malloc(buffersize);
  }
  else {
    bufferlines = strlen(buffer) / linesize + 1;
    buffersize = linesize * bufferlines;
    newbuffer = (char *)malloc(buffersize);
    if (!newbuffer) {
      error("Could not increase size of buffer");
      return NULL;
    }
    bc = strlen(buffer);
    memcpy(newbuffer, buffer, bc);
    buffer = newbuffer;
    cursorpos = bc;
  }
  if (!buffer) {
    error("Could not create buffer");
    return NULL;
  }
  attron(A_REVERSE);
  curs_set(2);
  while (1) {
    mvprintw(rows - 1 - bufferlines, 0, "%s:", label);
    cc = 0;
    for (i = 0; i < cols - lineoffset; i++, cc++) {
      if (cc < bc) {
        addch(buffer[cc]);
      }
      else {
        addch(' ');
      }
    }
    for (i = 1; i < bufferlines; i++) {
      for (j = 0; j < cols; j++) {
        if (j >= lineoffset) {
          if (cc < bc) {
            addch(buffer[cc++]);
            continue;
          }
          cc++;
        }
        addch(' ');
      }
    }
    move(rows - 1 - bufferlines + cursorpos / linesize, lineoffset + (cursorpos % linesize));
    refresh();
    ch = getch();
    if (ch == 10) { /* Enter */
      break;
    }
    if (ch == 4 || ch == 3 || ch == 27) { /* ^D , ^C or ESC */
      buffer[0] = 0;
      break;
    }
    switch (ch) {
      case KEY_LEFT:
        if (cursorpos > 0)
          cursorpos--;
        break;
      case KEY_RIGHT:
        if (cursorpos < bc)
          cursorpos++;
        break;
      case 262: /* home */
        cursorpos = 0;
        break;
      case 360: /* end */
        cursorpos = bc;
        break;
      case 8: /* backspace */
      case 127: /* also backspace */
        if (cursorpos > 0) {
          cursorpos--;
          memcpy(buffer + cursorpos, buffer + cursorpos + 1, bc - cursorpos - 1);
          if (bc > 0)
            bc--;
        }
        break;
      case 330: /* delete */
        if (cursorpos < bc) {
          memcpy(buffer + cursorpos, buffer + cursorpos + 1, bc - cursorpos - 1);
          if (bc > 0)
            bc--;
        }
        break;
      default:
        if (ch > 31 && ch < 127) {
          if (cursorpos == bc) {
            buffer[bc++] = ch;
          }
          else {
            memcpy(buffer + cursorpos + 1, buffer + cursorpos, bc - cursorpos);
            buffer[cursorpos] = ch;
            bc++;
          }
          if (bc >= buffersize) {
            bufferlines++;
            newbuffer = increase_buffer(buffer, buffersize, buffersize + linesize);
            if (!newbuffer) {
              free(buffer);
              error("Could not increase size of buffer");
              return NULL;
            }
            buffer = newbuffer;
            buffersize += linesize;
          }
          cursorpos++;
        }
        break;
    }
  }
  buffer[bc] = 0;
  curs_set(0);
  attroff(A_REVERSE);
  return buffer;
}

char *get_input(char *label) {
  return get_input_edit(label, NULL);
}

void print_bar(int status, int rows, int cols, int tasks) {
  int i;
  for (i = 0; key_funcs[i].key > 0; i++) {
    attron(A_REVERSE);
    mvprintw(rows - 1, i * 9, "%c", key_funcs[i].key);
    attroff(A_REVERSE);
    mvprintw(rows - 1, i * 9 + 2, "%s", key_funcs[i].func);
  }
  mvprintw(rows - 1, cols - strlen(status_str[status]) - 3, "%s  ", status_str[status]);
  mvprintw(0, cols - 16, "%10d %stask%s", 
      tasks,
      tasks == 1 ? " " : "",
      tasks == 1 ? "" : "s");
}

void fatal_error() {
  mvprintw(2, 4, "error: %s", get_last_error());
  refresh();
  getch();
  endwin();
  exit(1);
}

int main(int argc, char *argv[]) {
  char *filename = "todo.txt";
  char *input_text = NULL;
  int rows, cols, ch, y, highlight = 0, i = 0, status = 0,
      orows, ocols, top = 0, bottom = 0;
  TASK *task = NULL;
  TASK *selected = NULL;
  TODOLIST *todolist = NULL;

  initscr();
  raw();
  clear();
  curs_set(0);
  keypad(stdscr, TRUE);
  noecho();
  
  if (argc > 1) {
    filename = argv[1];
  }
  todolist = load_todolist(filename);

  mvprintw(0, 0, "ctodo %s", CTODO_VERSION);

  if (!todolist)
    fatal_error();

  while (1) {
    orows = rows;
    ocols = cols;
    getmaxyx(stdscr, rows, cols);
    if (orows != rows || ocols != cols) {
      clear();
    }
    mvprintw(0, 0, "ctodo %s", CTODO_VERSION);
    y = 2;
    y += print_multiline(y, 4, todolist->title, cols - 8) + 1;
    task = todolist->first;
    if (highlight >= i) highlight = i - 1;
    if (highlight < 0) highlight = 0;
    if (highlight < top) top = highlight;
    if (highlight > bottom) top += highlight - bottom;
    i = 0;
    selected = NULL;
    if (top != 0) {
      mvprintw(y - 1, 2, " * ");
    }
    while (task) {
      if (i >= top && y < rows - 3) {
        if (highlight == i) {
          attron(A_REVERSE);
          selected = task;
        }
        mvprintw(y, 2, "[%c]", task->done ? 'X' : ' ');
        y += print_multiline(y, 6, task->message, cols - 10);
        if (highlight == i) {
          attroff(A_REVERSE);
        }
        bottom = i;
      }
      i++;
      task = task->next;
    }
    if (bottom != i - 1) {
      mvprintw(y++, 2, " * ");
    }
    print_bar(status, rows, cols, i);
    refresh();
    ch = getch();

    switch (ch) {
      case 'k':
      case 'K':
      case KEY_UP:
        highlight--;
        clear();
        break;
      case 'j':
      case 'J':
      case KEY_DOWN:
        highlight++;
        clear();
        break;
      case 21: /* ^U */
      case 339: /* page up */
        highlight -= 5;
        clear();
        break;
      case 4: /* ^D */
      case 338: /* page down */
        highlight += 5;
        clear();
        break;
      case 262: /* home */
        highlight = 0;
        clear();
        break;
      case 360: /* end */
        highlight = i - 1;
        clear();
        break;
      case 'R':
      case 'r':
        i = 0;
        delete_todolist(todolist);
        todolist = load_todolist(filename);
        if (!todolist)
          fatal_error();
        status = 0;
        clear();
        break;
      case 'S':
      case 's':
        if (save_todolist(todolist, filename)) {
          status = 0;
        }
        else {
          status = 2;
        }
        clear();
        break;
      case 'D':
      case 'd':
      case '-':
      case 330: /* del */
        i--;
        if (selected) {
          delete_task(selected, todolist);
        }
        status = 1;
        clear();
        break;
      case 'N':
      case 'n':
      case '+':
      case 331: /* ins */
        input_text = get_input("New task");
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          add_task(todolist, input_text, 0, 0);
          status = 1;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 'E':
        if (!selected) {
          break;
        }
        input_text = get_input("Set task");
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          free(selected->message);
          selected->message = input_text;
          status = 1;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 'e':
        if (!selected) {
          break;
        }
        input_text = get_input_edit("Edit task", selected->message);
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          free(selected->message);
          selected->message = input_text;
          status = 1;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 'T':
        input_text = get_input("Set title");
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          free(todolist->title);
          todolist->title = input_text;
          status = 1;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 't':
        input_text = get_input_edit("Edit title", todolist->title);
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          free(todolist->title);
          todolist->title = input_text;
          status = 1;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case ' ':
      case 10: /* enter */
        selected->done ^= 1;
        status = 1;
        break;
      default:
        /* mvprintw(0, 0, "Key pressed: %d", ch); */
        break;
    }

    if (ch == 'q' || ch == 'Q') {
      if (save_todolist(todolist, filename) || status == 2) {
        break;
      }
      else {
        status = 2;
      }
    }
  }
  endwin();
  return 0;
}
