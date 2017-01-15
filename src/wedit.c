/* ctodo
 * Copyright (c) 2017 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

#include <ncurses.h>
#include <readline/readline.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "wedit.h"
#include "error.h"
#include "stream.h"

int input_available = 0;
unsigned char next_input;
size_t prompt_l;
int old_lines = 0;
int stop = 0;
char *input_buffer = NULL;

COMMAND edit_commands[] = {
  {"^C", "Cancel"},
  {NULL, NULL}
};

void print_commands(COMMAND *commands, int y, int width, int height) {
  size_t kwidth;
  int i;
  int show = width / 11;
  int cwidth = width / show;
  for (i = 0; i < height; i++) {
    move(y + i, 0);
    clrtoeol();
  }
  for (i = 0; commands[i].key != NULL && i < show; i++) {
    attron(A_REVERSE);
    kwidth = strlen(commands[i].key);
    mvprintw(y + i % height, i / height * cwidth, "%s", commands[i].key);
    attroff(A_REVERSE);
    mvprintw(y + i % height, i / height * cwidth + kwidth, " %s", commands[i].func);
  }
}

char *get_input(const char *prompt) {
  return get_input_edit(prompt, NULL);
}

/* inspired by https://github.com/ulfalizer/readline-and-ncurses */

int has_input() {
  return input_available;
}

int pop_input(FILE *f) {
  input_available = 0;
  return next_input;
}

void push_input(char c) {
  next_input = c;
  input_available = 1;
  rl_callback_read_char();
}

size_t strnlen(const char *str, size_t max) {
  size_t i;
  for(i = 0; i < max && str[i]; i++);
  return i;
}

size_t display_width(const char *str, size_t n, size_t line_width, size_t cursor_point, size_t *cursor_pos) {
  mbstate_t shift_state;
  wchar_t wc;
  int cursor_set = 0;
  size_t wc_len;
  size_t width = prompt_l + 2;
  WINDOW *win = newwin(1, 10, 0, 0);
  memset(&shift_state, '\0', sizeof shift_state);
  for (size_t i = 0; i < n; i += wc_len) {
    if (!cursor_set && i >= cursor_point) {
      *cursor_pos = width;
      cursor_set = 1;
    }
    wc_len = mbrtowc(&wc, str + i, MB_CUR_MAX, &shift_state);
    switch (wc_len) {
      case (size_t)-1:
      case (size_t)-2:
        width += strnlen(str + i, n - i);
      case 0:
        delwin(win);
        if (!cursor_set) {
          *cursor_pos = width;
        }
        return width;
    }
    if (wc == '\t') {
      width += 8 - width % line_width % 8;
    }
    else if (iswcntrl(wc)) {
      width += 2;
    }
    else {
      /* finds the display width of the character by letting ncurses print it */
      mvwprintw(win, 0, 0, "%C", wc);
      int x, y;
      getyx(win, y, x);
      width += x;
      (void)y;
    }
  }
  delwin(win);
  if (!cursor_set) {
    *cursor_pos = width;
  }
  return width;
}

void show_buffer() {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  size_t pos;
  size_t buffer_size = display_width(rl_line_buffer, rl_end, cols, rl_point, &pos);
  int lines = buffer_size / cols + 1;
  if (lines < old_lines) {
    for (int i = lines; i < old_lines; i++) {
      move(rows - i - 2, 0);
      clrtoeol();
    }
  }
  old_lines = lines;
  int cursor_row = pos / cols;
  int cursor_col = pos % cols;
  int top = rows - lines - 1;
  print_commands(edit_commands, rows - 1, cols, 1);
  attron(A_REVERSE);
  for (int i = 0; i < lines; i++) {
    for (int j = 0; j < cols; j++) {
      mvprintw(top + i, j, " ");
    }
  }
  mvprintw(top, 0, "%s: %s", rl_display_prompt, rl_line_buffer);
  move(top + cursor_row, cursor_col);
  attroff(A_REVERSE);
}

void callback(char *line) {
  if (line == NULL) {
    input_buffer = malloc(1);
    input_buffer[0] = 0;
    stop = 1;
  }
  else {
    input_buffer = line;
    stop = 1;
  }
}

char *get_input_edit(const char *prompt, const char *buffer) {
  prompt_l = strlen(prompt);
  rl_bind_key('\t', rl_insert); /* disables completion */
  /* TODO: disable history as well? */
  rl_catch_signals = 0;
  rl_catch_sigwinch = 0;
  rl_deprep_term_function = NULL;
  rl_prep_term_function = NULL;
  rl_change_environment = 0;
  rl_getc_function = pop_input;
  rl_input_available_hook = has_input;
  rl_redisplay_function = show_buffer;
  rl_callback_handler_install(prompt, callback);

  if (buffer) {
    while (*buffer) {
      push_input(*(buffer++));
    }
  }

  curs_set(2);
  keypad(stdscr, 0);
  stop = 0;
  input_buffer = NULL;
  while (!stop) {
    int c = wgetch(stdscr);
    switch (c) {
      case KEY_RESIZE:
        show_buffer();
        break;
      case 3: /* ^C */
      case 4: /* ^D */
        input_buffer = malloc(1);
        input_buffer[0] = 0;
        stop = 1;
        break;
      case 27: /* ESC */
        /* TODO: the old editor would close on ESC, however when the keypad is
         * disabled (necessary when forwarding key presses to ncurses), an ESC
         * character preces any escape sequence (e.g. arrow keys etc.). */
      default:
        push_input(c);
    }
  }
  keypad(stdscr, 1);
  curs_set(0);
  rl_callback_handler_remove();
  return input_buffer;
}
