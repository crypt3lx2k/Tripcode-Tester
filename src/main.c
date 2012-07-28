#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tripcode.h"

#define EXIT_ON_ERROR(function)			\
  do {						\
    perror(function);				\
    exit(EXIT_FAILURE);				\
  } while (0)

static iconv_t cd;

static int input_is_shell;
static int output_is_file;

static FILE * prompt_file;
static const char * output_fmt;

static void tripcode_driver (iconv_t cd,
			     char * input, size_t len,
			     char * output) {
  char sjis[32] = {0};
  char html[32] = {0};

  len = transform_sjis(cd, input, len, sjis, 32);

  if (len == (size_t) -1)
    EXIT_ON_ERROR("transform_sjis");

  len = transform_html(sjis, html);

  trip(html, len, output);
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
  if (input_is_shell)
    fprintf(prompt_file, "\e[02;34m>>>\e[0m ");

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
    EXIT_ON_ERROR("iconv_open");

  if (argc > 1)
    args_loop(argc, argv);
  else
    stdin_loop();

  error = iconv_close(cd);

  if (error == -1)
    EXIT_ON_ERROR("iconv_close");

  exit(EXIT_SUCCESS);
}