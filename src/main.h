/*
 * Tripcode Tester - A program for testing tripcodes.
 * Copyright (C) 2010-2013 Truls Edvard Stokke
 *
 * Tripcode Tester is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tripcode Tester is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tripcode Tester.  If not, see <http://www.gnu.org/licenses/>.
 */

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
