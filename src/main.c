#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tripcode.h"

#include "main.h" /* PRINT_ENO_AND_EXIT */

/* global conversion descriptor, if we are to add threading
   later we need to make this local to the threads.*/
static iconv_t cd;

static int input_is_shell;
static int output_is_file;

/* which stream we print the prompt to */
static FILE * prompt_file;

/* the format that is used for the tripcodes */
static const char * output_fmt;

static void tripcode_driver (iconv_t cd,
			     char * input, size_t len,
			     char * output) {
#define BUFFER_LENGTH 32
  char sjis[BUFFER_LENGTH] = {0};
  char html[BUFFER_LENGTH] = {0};

  len = transform_sjis(cd, input, len, sjis, BUFFER_LENGTH);

  if (len == (size_t) -1)
    PRINT_ENO_AND_EXIT("transform_sjis");

  len = transform_html(sjis, html);

  trip(html, len, output);
#undef BUFFER_LENGTH
}

static void args_loop (int argc, char * argv[]) {
  int i;
  char tripcode[14] = {0};

  for (i = 1; i < argc; i++) {
    tripcode_driver(cd, argv[i], strlen(argv[i]), tripcode);

    printf(output_fmt, argv[i], tripcode + 3);
  }
}

static char * prompt (char * s, int size, FILE * stream) {
  if (input_is_shell) {
    fputs("\e[02;34m>>>\e[0m ", prompt_file);
    fflush(prompt_file);
  }

  return fgets(s, size, stream);
}

static void stdin_loop (void) {
#define IO_BUFFER_SIZE 128
  char io_buffer[IO_BUFFER_SIZE];
  char tripcode[14] = {0};

  while (prompt(io_buffer, IO_BUFFER_SIZE, stdin) != NULL) {
    size_t len;

    /* ignore empty lines */
    if (io_buffer[0] == '\0')
      continue;

    len = strlen(io_buffer);

    while (len &&
	   (io_buffer[len - 1] == '\n' ||
	    io_buffer[len - 1] == '\r'))
      io_buffer[--len] = '\0';

    /* ignore lines that are only newlines */
    if (len == 0)
      continue;

    /* cut the string short so that it will fit into the sjis
       buffer, only 8 characters will ultimately be used
       anyway. We might collapse 3 chars of UTF-8 into a
       single sjis character so here is the worst case. */
#define MAX_LENGTH (8*3)
    if (len > MAX_LENGTH) {
      io_buffer[MAX_LENGTH] = '\0';
      len = MAX_LENGTH;
    }
#undef MAX_LENGTH

    tripcode_driver(cd, io_buffer, len, tripcode);

    if (input_is_shell && !output_is_file)
      printf(output_fmt, tripcode + 3);
    else
      printf(output_fmt, io_buffer, tripcode + 3);
  }

  if (input_is_shell)
    fputc('\n', prompt_file);

#undef IO_BUFFER_SIZE
}

int main (int argc, char * argv[]) {
  int error;

  input_is_shell = isatty(STDIN_FILENO);
  output_is_file = !isatty(STDOUT_FILENO);

  prompt_file = output_is_file ? stderr : stdout;

  if (output_is_file)
    output_fmt = "#%s => !%s\n";
  else if (!input_is_shell || argc > 1)
    output_fmt = "#\e[01;32m%s\e[0m => !\e[01;31m%s\e[0m\n";
  else
    output_fmt = "!\e[01;31m%s\e[0m\n";

  cd = iconv_open("SJIS//IGNORE", "UTF-8");

  if (cd == (iconv_t) -1)
    PRINT_ENO_AND_EXIT("iconv_open");

  if (argc > 1)
    args_loop(argc, argv);
  else
    stdin_loop();

  error = iconv_close(cd);

  if (error == -1)
    PRINT_ENO_AND_EXIT("iconv_close");

  exit(EXIT_SUCCESS);
}
