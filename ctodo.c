// ctodo
// Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
// Licensed under the MIT license.
// See the LICENSE file or http://opensource.org/licenses/MIT for more information.

#include <ncurses.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "task.h"
#include "file.h"
#include "sync.h"
#include "stream.h"
#include "error.h"

#define STATUS_SAVED "Saved"
#define STATUS_UNSAVED "Unsaved"

struct {
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

void print_message(char *format, ...) {
  int rows, cols, len, x;
  char *message;
  va_list va;
  va_start(va, format);
  message = string_vprintf(format, va);
  va_end(va);
  getmaxyx(stdscr, rows, cols);
  len = strlen(message);
  x = cols / 2 - (len + 4) / 2;
  move(rows - 2, 0);
  clrtoeol();
  attron(A_REVERSE);
  mvprintw(rows - 2, x, "[ %s ]", message);
  free(message);
  attroff(A_REVERSE);
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
      case 263: /* also backspace (in xterm?) */
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
            newbuffer = resize_buffer(buffer, buffersize, buffersize + linesize);
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

void print_bar(char *status, int rows, int cols, int tasks) {
  int i, x;
  for (i = 0; key_funcs[i].key > 0; i++) {
    attron(A_REVERSE);
    mvprintw(rows - 1, i * 9, "%c", key_funcs[i].key);
    attroff(A_REVERSE);
    mvprintw(rows - 1, i * 9 + 2, "%s", key_funcs[i].func);
  }
  attron(A_REVERSE);
  for (i = 0; i < cols; i++) {
    mvprintw(0, i, " ");
  }
  mvprintw(0, 2, "ctodo %s", CTODO_VERSION);
  mvprintw(0, cols - 18, "%10d %stask%s", 
      tasks,
      tasks == 1 ? " " : "",
      tasks == 1 ? "" : "s");
  x = cols / 2 - strlen(status) / 2;
  mvprintw(0, x, "%s", status);
  attroff(A_REVERSE);
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
  char *origin = NULL;
  char *status = STATUS_SAVED;
  int rows, cols, ch, y, highlight = 0, i = 0,
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

  if (get_option_bit(todolist, "autosync")) {
    origin = copy_option(todolist, "origin");
    if (origin) {
      mvprintw(2, 5, "Synchronizing...");
      refresh();
      // TODO: merge
      TODOLIST *new = pull_todolist(origin);
      if (new) {
        delete_todolist(todolist);
        todolist = new;
      }
      else {
        print_message("Synchronization failed: %s", get_last_error());
      }
    }
  }

  while (1) {
    orows = rows;
    ocols = cols;
    getmaxyx(stdscr, rows, cols);
    if (orows != rows || ocols != cols) {
      clear();
    }
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
        if (highlight < top)
          clear();
        break;
      case 'j':
      case 'J':
      case KEY_DOWN:
        highlight++;
        if (highlight > bottom)
          clear();
        break;
      case 21: /* ^U */
      case 339: /* page up */
        highlight -= 5;
        if (highlight < top)
          clear();
        break;
      case 4: /* ^D */
      case 338: /* page down */
        highlight += 5;
        if (highlight > bottom)
          clear();
        break;
      case 'g':
      case 262: /* home */
        highlight = 0;
        clear();
        break;
      case 'G':
      case 360: /* end */
        highlight = i - 1;
        clear();
        break;
      case 'z': // TODO
        if (origin) {
            print_message("Synchronizing...");
          refresh();
          if (!push_todolist(todolist, origin))
            print_message("Synchronization failed: %s", get_last_error());
          else
            print_message("Synchronized!");
        }
        break;
      case 'R':
      case 'r':
        i = 0;
        delete_todolist(todolist);
        todolist = load_todolist(filename);
        if (!todolist)
          print_message("Could not read file: %s", get_last_error());
        status = STATUS_SAVED;
        clear();
        break;
      case 'S':
      case 's':
        if (save_todolist(todolist, filename)) {
          status = STATUS_SAVED;
        }
        else {
          print_message("Could not save file: %s", get_last_error());
          status = STATUS_UNSAVED;
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
        status = STATUS_UNSAVED;
        clear();
        break;
      case 'i':
        input_text = get_input("Insert task");
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          if (selected) {
            insert_task(todolist, selected, input_text, 0, 0);
          }
          else {
            add_task(todolist, input_text, 0, 0);
          }
          status = STATUS_UNSAVED;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 'I':
        input_text = get_input("Insert task");
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          if (todolist->first) {
            insert_task(todolist, todolist->first, input_text, 0, 0);
          }
          else {
            add_task(todolist, input_text, 0, 0);
          }
          status = STATUS_UNSAVED;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 'a':
        input_text = get_input("Append task");
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          if (selected && selected->next) {
            insert_task(todolist, selected->next, input_text, 0, 0);
          }
          else {
            add_task(todolist, input_text, 0, 0);
          }
          status = STATUS_UNSAVED;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 'A':
      case 'N':
      case 'n':
      case '+':
      case 331: /* ins */
        input_text = get_input("Append task");
        if (!input_text)
          fatal_error();
        if (input_text[0]) {
          add_task(todolist, input_text, 0, 0);
          status = STATUS_UNSAVED;
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
          status = STATUS_UNSAVED;
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
          status = STATUS_UNSAVED;
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
          status = STATUS_UNSAVED;
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
          status = STATUS_UNSAVED;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case ' ':
      case 10: /* enter */
        if (!selected) {
          break;
        }
        selected->done ^= 1;
        status = STATUS_UNSAVED;
        break;
      default:
        if (isgraph(ch))
          print_message("Unbound key: %c", ch);
        else 
          print_message("Unbound key: (%d)", ch);
        /* mvprintw(0, 0, "Key pressed: %d", ch); */
        break;
    }

    if (ch == 'q' || ch == 'Q') {
      if (status == STATUS_SAVED || save_todolist(todolist, filename)) {
        break;
      }
      else {
        print_message("Could not save file: %s", get_last_error());
      }
    }
  }
  endwin();
  return 0;
}
