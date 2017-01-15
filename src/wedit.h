/* ctodo
 * Copyright (c) 2017 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

/* Provides unicode readline-based line editor for editing tasks, title, etc.
 *
 * Is enabled by default since version 1.3. Can be disabled by running cmake
 * with -DREADLINE_ENABLE=OFF. */
#ifndef WEDIT_H
#define WEDIT_H

/* Key command description */
typedef struct {
  char *key;
  char *func;
} COMMAND;

/* Print a list of commands at the bottom of the screen. */
void print_commands(COMMAND *commands, int y, int width, int height);
/* Open an editor with the given prompt and return the result. */
char *get_input(const char *prompt);
/* Open an editor with the given prompt and content and return the result. */
char *get_input_edit(const char *prompt, const char *buffer);

#endif
