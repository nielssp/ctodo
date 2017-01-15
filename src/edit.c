/* ctodo
 * Copyright (c) 2017 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

#include <ncurses.h>
#include <string.h>

#include "edit.h"
#include "error.h"
#include "stream.h"

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

char *get_input(char *prompt) {
  return get_input_edit(prompt, NULL);
}

char *get_input_edit(char *prompt, char *buffer) {
  int rows, cols, i, j, ch, bc = 0, cc;
  int lineoffset, linesize, buffersize;
  int bufferlines = 1, cursorpos = 0;
  char *newbuffer = NULL;
  getmaxyx(stdscr, rows, cols);
  print_commands(edit_commands, rows - 1, cols, 1);
  lineoffset = strlen(prompt) + 1;
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
    mvprintw(rows - 1 - bufferlines, 0, "%s:", prompt);
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
      case 2: /* ^B */
      case KEY_LEFT:
        if (cursorpos > 0)
          cursorpos--;
        break;
      case 6: /* ^F */
      case KEY_RIGHT:
        if (cursorpos < bc)
          cursorpos++;
        break;
      case 1: /* ^A */
      case 262: /* home */
        cursorpos = 0;
        break;
      case 5: /* ^E */
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
