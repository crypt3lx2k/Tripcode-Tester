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

#include <ctype.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tripcode.h"

#include "config.h" /* pc, ps, pe, BOLD, ... */
#include "main.h" /* PRINT_ENO_AND_EXIT */

/* global conversion descriptor, if we are to add threading
   later we need to make this local to the threads.*/
static iconv_t cd;

static int input_is_shell;
static int output_is_file;

/* which stream we print the prompt to */
static FILE * prompt_file;

#if HAVE_ALL_CURSES
static int prompt_file_putchar (int c) {
  return putc(c, prompt_file);
}
#endif /* HAVE_ALL_CURSES */

static void print_result_color (const char * key, const char * tripcode) {
  if (!input_is_shell || output_is_file) {
    pc('#'); pe(BOLD); pe(GREEN); ps(key); pe(CLEAR); ps(" => ");
  }

  pc('!'); pe(BOLD); pe(RED); ps(tripcode); pe(CLEAR); pc('\n');
}

static void print_result (const char * key, const char * tripcode) {
  if (!output_is_file && max_colors >= 8)
    return print_result_color(key, tripcode);
  
  if (!input_is_shell || output_is_file)
    printf("#%s => ", key);
  printf("!%s\n", tripcode);
}

static void handle_input (char * io_buffer, char * buffer) {
  char * output;
  size_t len;

  /* ignore empty lines */
  if (io_buffer[0] == '\0')
    return;

  len = strlen(io_buffer);

  while (len &&
	 (io_buffer[len - 1] == '\n' ||
	  io_buffer[len - 1] == '\r'))
    io_buffer[--len] = '\0';

  /* cut the string short so that it will fit into the sjis
     buffer, only 8 characters will ultimately be used
     anyway. We might collapse 3 chars of UTF-8 into a
     single sjis character so here is the worst case. */
#define MAX_LENGTH (8*3)
  if (len > MAX_LENGTH) {
    len = MAX_LENGTH;

#define IS_UTF8_CONTINUATION_BYTE(c) \
    (!((((unsigned char) (c)) ^ 0x80) & 0xc0))

    /* if we are in the middle of a UTF-8 multibyte
       character we can't just cut off the middle or
       else iconv will choke when it reaches it. */
    while (len && IS_UTF8_CONTINUATION_BYTE(io_buffer[len]))
      io_buffer[len--] = '\0';

#undef IS_UTF8_CONTINUATION_BYTE

    io_buffer[len] = '\0';
  }
#undef MAX_LENGTH

  /* ignore lines that are only newlines */
  if (len == 0)
    return;

  output = tripcode(cd, io_buffer, len, buffer);

  if (output == NULL) {
    fputs("error when handling ", stderr);
    perror(io_buffer);
    return;
  }

  print_result(io_buffer, output);  
}

static void args_loop (int argc, char * argv[]) {
  int i;
  char buffer[14] = {0};

  for (i = 1; i < argc; i++)
    handle_input(argv[i], buffer);
}

static char * prompt (char * s, int size, FILE * stream) {
  if (input_is_shell) {
    pe(BLUE); fputs(">>> ", prompt_file); pe(CLEAR);
    fflush(prompt_file);
  }

  return fgets(s, size, stream);
}

static void fgets_loop (void) {
#define IO_BUFFER_SIZE 128
  char io_buffer[IO_BUFFER_SIZE];
  char buffer[14] = {0};

  while (prompt(io_buffer, IO_BUFFER_SIZE, stdin) != NULL)
    handle_input(io_buffer, buffer);

  if (input_is_shell)
    fputc('\n', prompt_file);

#undef IO_BUFFER_SIZE
}

#if HAVE_ALL_READLINE
static char * prompt_readline (void) {
  pe(BLUE); fputs(">>> ", prompt_file); pe(CLEAR);
  rl_already_prompted = 1;
  return readline(">>> ");
}

static void readline_loop (void) {
  char * io_buffer;
  char buffer[14] = {0};

  while ((io_buffer = prompt_readline()) != NULL) {
    handle_input(io_buffer, buffer);

# if HAVE_ALL_READLINE_HISTORY
    if (*io_buffer)
      add_history(io_buffer);
# endif /* HAVE_ALL_READLINE_HISTORY */

    free(io_buffer);
  }

  if (input_is_shell)
    fputc('\n', prompt_file);
}
#else
# define readline_loop() fgets_loop()
#endif /* HAVE_ALL_READLINE */

static void initialize (int argc, char * argv[]) {
  input_is_shell =  isatty(STDIN_FILENO) && argc == 1;
  output_is_file = !isatty(STDOUT_FILENO);

  prompt_file = output_is_file ? stderr : stdout;

#if HAVE_ALL_CURSES
  setupterm(NULL,
	    output_is_file ? STDERR_FILENO : STDOUT_FILENO,
	    NULL);
#endif /* HAVE_ALL_CURSES */

#if HAVE_ALL_READLINE
  rl_outstream = prompt_file;
#endif /* HAVE_ALL_READLINE */

  cd = iconv_open("SJIS//IGNORE", "UTF-8");

  if (cd == (iconv_t) -1)
    PRINT_ENO_AND_EXIT("iconv_open");
}

static void cleanup (void) {
  int error;

  error = iconv_close(cd);

  if (error == -1)
    PRINT_ENO_AND_EXIT("iconv_close");

#if HAVE_ALL_CURSES
  reset_shell_mode();
#endif /* HAVE_ALL_CURSES */
}

int main (int argc, char * argv[]) {
  initialize(argc, argv);

  if (argc > 1)
    args_loop(argc, argv);
  else if (input_is_shell)
    readline_loop();
  else
    fgets_loop();

  cleanup();

  exit(EXIT_SUCCESS);
}
