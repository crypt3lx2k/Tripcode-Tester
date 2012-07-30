#ifndef MAIN_H
#define MAIN_H

#include <stdio.h> /* perror */
#include <stdlib.h> /* exit and family */

/**
 * PRINT_ENO_AND_EXIT
 *
 * Prints the string corresponding to errno and then exits.
 *
 * Note that this macro does not return.
 *
 * @param  (const char *) who is to blame for this calamity
 */
#define PRINT_ENO_AND_EXIT(sinner)		\
  do {						\
    perror(sinner);				\
    exit(EXIT_FAILURE);				\
  } while (0)

#endif /* MAIN_H */
