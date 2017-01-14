/* ctodo
 * Copyright (c) 2017 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

/* Provides line editor used when editing tasks, title, etc. */
#ifndef EDIT_H
#define EDIT_H

/* Key command description */
typedef struct {
  char *key;
  char *func;
} COMMAND;

/* Print a list of commands at the bottom of the screen. */
void print_commands(COMMAND *commands, int y, int width, int height);
/* Open an editor with the given prompt and return the result. */
char *get_input(char *prompt);
/* Open an editor with the given prompt and content and return the result. */
char *get_input_edit(char *prompt, char *buffer);

#endif
