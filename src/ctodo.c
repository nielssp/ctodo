/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

#include <ncurses.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>

#include "config.h"
#include "task.h"
#include "file.h"
#include "stream.h"
#include "error.h"

#ifdef READLINE_ENABLE
#include "wedit.h"
#else
#include "edit.h"
#endif

#ifdef SYNC_ENABLE
#include "sync.h"
#endif

#define STATUS_SAVED "Saved"
#define STATUS_UNSAVED "Unsaved"

COMMAND main_commands[] = {
  {"Q", "Quit"},
  {"S", "Save"},
  {"R", "Reload"},
  {"N", "New"},
  {"E", "Edit"},
  {"D", "Delete"},
  {"T", "Title"},
  {NULL, NULL}
};

int print_multiline(int top, int offset, const char *str, size_t max_width) {
  mbstate_t shift_state;
  wchar_t wc;
  size_t x = offset;
  size_t y = top;
  size_t wc_len;
  size_t n = strlen(str);
  memset(&shift_state, '\0', sizeof shift_state);
  for (size_t i = 0; i < n; i += wc_len) {
    wc_len = mbrtowc(&wc, str + i, MB_CUR_MAX, &shift_state);
    if (!wc_len || wc_len == (size_t)-1 || wc_len == (size_t)-2) {
      break;
    }
    mvprintw(y, x, "%C", wc);
    size_t new_x, new_y;
    getyx(stdscr, new_y, new_x);
    if (new_y > y) {
      y = new_y;
      if (new_x > 0) {
        move(new_y, 0);
        clrtoeol();
        mvprintw(y, offset, "%C", wc);
      }
      x = offset + new_x;
    }
    else if (new_x - offset > max_width) {
      move(y, x);
      clrtoeol();
      mvprintw(y + 1, offset, "%C", wc);
      getyx(stdscr, y, x);
    }
    else {
      x = new_x;
    }
  }
  if (x > offset || y == top) {
    return y - top + 1;
  }
  return y - top;
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
  if (x < 0) x = 0;
  move(rows - 2, 0);
  clrtoeol();
  attron(A_REVERSE);
  mvprintw(rows - 2, x, "[ %s ]", message);
  free(message);
  attroff(A_REVERSE);
}

void print_bar(char *status, int rows, int cols, int tasks) {
  int i, x;
  print_commands(main_commands, rows - 1, cols, 1);
  attron(A_REVERSE);
  for (i = 0; i < cols; i++) {
    mvprintw(0, i, " ");
  }
  mvprintw(0, 2, "ctodo %s", CTODO_VERSION);
  x = cols / 2 - strlen(status) / 2;
  mvprintw(0, x, "%s", status);
  mvprintw(0, cols - 18, "%10d %stask%s", 
      tasks,
      tasks == 1 ? " " : "",
      tasks == 1 ? "" : "s");
  attroff(A_REVERSE);
}

void fatal_error() {
  mvprintw(2, 4, "%s", get_last_error());
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
      orows, ocols, top = 0, bottom = 0, full = 0;
  TASK *task = NULL;
  TASK *selected = NULL;
  TODOLIST *todolist = NULL;
  char *file_version = NULL;

  setlocale(LC_ALL, "");
  initscr();
  clear();
  curs_set(0);
  noecho();

  if (argc > 1) {
    filename = argv[1];
  }
  todolist = load_todolist(filename);

  mvprintw(0, 0, "  ctodo %s", CTODO_VERSION);

  if (!todolist) {
    error("Could not open %s: %s", filename, get_last_error());
    fatal_error();
  }

  file_version = copy_option(todolist, "version");
  if (file_version) {
    /* TODO: compare versions */
    free(file_version);
  }
  else {
    set_option(todolist, "version", CTODO_VERSION);
  }

#ifdef SYNC_ENABLE
  if (get_option_bit(todolist, "autosync")) {
    origin = copy_option(todolist, "origin");
    if (origin) {
      getmaxyx(stdscr, rows, cols);
      mvprintw(2, 5, "Synchronizing tasks...");
      mvprintw(4, 5, "Downloading:");
      print_multiline(5, 5, origin, cols - 9);
      refresh();
      /* TODO: merge */
      TODOLIST *new = pull_todolist(origin);
      clear();
      if (new) {
        delete_todolist(todolist);
        todolist = new;
      }
      else {
        print_message("Synchronization failed: %s", get_last_error());
      }
    }
  }
#endif

  raw();
  keypad(stdscr, 1);
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
    full = y >= rows - 3;
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
      case 'm':
      case 337: /* S-Up */
        if (selected) {
          move_task_up(todolist, selected);
          highlight--;
          status = STATUS_UNSAVED;
          clear();
        }
        break;
      case 'M':
      case 336: /* S-Down */
        if (selected) {
          move_task_down(todolist, selected);
          highlight++;
          status = STATUS_UNSAVED;
          clear();
        }
        break;
#ifdef SYNC_ENABLE
      case 'z': /* TODO */
        if (origin) {
            print_message("Synchronizing tasks...");
          refresh();
          if (!push_todolist(todolist, origin))
            print_message("Synchronization failed: %s", get_last_error());
          else
            print_message("Synchronization complete!");
        }
        break;
#endif
      case 'R':
      case 'r':
        i = 0;
        TODOLIST *new = load_todolist(filename);
        if (!new) {
          print_message("Could not load %s: %s", filename, get_last_error());
        }
        else {
          delete_todolist(todolist);
          todolist = new;
          status = STATUS_SAVED;
          clear();
          print_message("Reloaded");
        }
        break;
      case 'S':
      case 's':
        if (save_todolist(todolist, filename)) {
          print_message("Saved");
          status = STATUS_SAVED;
        }
        else {
          print_message("Could not save %s: %s", filename, get_last_error());
          status = STATUS_UNSAVED;
        }
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
          i++;
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
          highlight = 0;
          i++;
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
          if (full && bottom == highlight && bottom < i) {
            top++;
          }
          bottom++;
          highlight++;
          i++;
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
          if (full && bottom < i) {
            top++;
          }
          highlight = i;
          bottom++;
          i++;
        }
        else {
          free(input_text);
        }
        clear();
        break;
      case 'c':
      case 'E':
        if (!selected) {
          break;
        }
        input_text = get_input("Change task");
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
        input_text = get_input("Change title");
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
        else if (ch < ' ')
          print_message("Unbound key: ^%c", '@' + ch);
        else 
          print_message("Unbound key: (%d)", ch);
        break;
    }

    if (ch == 'q' || ch == 'Q') {
      if (save_todolist(todolist, filename) || ch == 'Q') {
        break;
      }
      else {
        print_message("Could not save %s: %s", filename, get_last_error());
      }
    }
  }
  delete_todolist(todolist);
  endwin();
  return 0;
}
