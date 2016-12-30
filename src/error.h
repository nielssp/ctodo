/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

/* Simple error handling. */
#ifndef ERROR_H
#define ERROR_H

/* Set an error message (see printf()). */
void error(const char *format, ...);
/* Whether an error has been set. */
int has_error();
/* Unset the error message. */
void unset_error();
/* Get the last error message. */
char *get_last_error();

#endif
