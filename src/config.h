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

#ifndef CONFIG_H
#define CONFIG_H

/* Please don't look at this file, it's not pretty. */

#if HAVE_LIB_CURSES
# if HAVE_CURSES_H && HAVE_TERM_H
#  define HAVE_ALL_CURSES 1
#  include <curses.h>
#  include <term.h>
# endif /* HAVE_CURSES_H && HAVE_TERM_H */
#endif /* HAVE_LIB_CURSES */

#if HAVE_LIB_READLINE
# if HAVE_READLINE_READLINE_H
#  define HAVE_ALL_READLINE 1
#  include <readline/readline.h>
# else
extern char * readline (const char * prompt);
# endif /* HAVE_READLINE_READLINE_H */

# if HAVE_READLINE_HISTORY_H
#  define HAVE_ALL_READLINE_HISTORY 1
#  include <readline/history.h>
# else
extern void add_history (const char * line);
# endif /* HAVE_READLINE_HISTORY_H */
#endif /* HAVE_LIB_READLINE */

#if HAVE_ALL_CURSES
# define pc(c)  putchar((c))
# define ps(s)  fputs((s), stdout)
# define pe(s)  tputs((s), 1, prompt_file_putchar)
# define tpc(n) tparm(set_a_foreground, (n))
# define BOLD   tparm(enter_bold_mode)
# define CLEAR  tparm(exit_attribute_mode)
# define RED    tpc(1)
# define GREEN  tpc(2)
# define BLUE   tpc(4)
#else /* ! HAVE_ALL_CURSES */
# define pc(c)  putchar((c))
# define ps(s)  fputs((s), stdout)
# define pe(s)
# define max_colors 0
#endif /* HAVE_ALL_CURSES */

#endif /* CONFIG_H */
