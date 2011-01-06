/* 
 * Copyright (c) 2010 Truls Edvard Stokke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <iconv.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <openssl/des.h>

#define ARRSIZE 0x14

const char * restrict table =
  "................................"
  ".............../0123456789ABCDEF"
  "GABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
  "fabcdefghijklmnopqrstuvwxyz....."
  "................................"
  "................................"
  "................................"
  "................................";

struct timeval timebuf;

unsigned long long int trips = 0;
int shell, file;
FILE *prompt_file;
char *output_fmt;
iconv_t cd;

size_t
htmlify(dest, src)
     char * restrict dest;
     char * restrict src;
{
  char *sp;
  size_t len;

#define strcatinc(s)				\
  memcpy(dest + len, s, sizeof(s) - 1);		\
  len += sizeof(s) - 1;				\
  break;

  /*
   * These are supposed to be HTML names,
   * feel free to add new ones if you're
   * able to input them.
   */
  for (len = 0, sp = src; len < 8 && *sp; sp += 1) {
    switch (*sp) {
    case '\"':
      strcatinc("&quot;");
    case '&':
      strcatinc("&amp;");
    case '<':
      strcatinc("&lt;");
    case '>':
      strcatinc("&gt;");
    default:
      dest[len++] = *sp;
      break;
    }
  }

#undef strcatinc

  return len;
}

void
trip(tripcode, triplen, dest)
     char * restrict tripcode;
     size_t triplen;
     char * restrict dest;
{
  char * restrict sjis, * restrict html;
  char * restrict salt;
  size_t sjislen = ARRSIZE, htmllen;

  sjis = calloc(1, ARRSIZE);
  html = calloc(1,      14);
  salt = calloc(1,       3);

  memset(salt, 'H', 2);

  char *sp = sjis, *tp = tripcode;

  iconv(cd,
	&tp, &triplen,
	&sp, &sjislen);

  sjislen = ARRSIZE - sjislen;

  htmllen = htmlify(html, sjis);

  switch (htmllen) {
  case 1:
    salt[1] = '.';
    break;
  default:
    salt[1] = table[(unsigned char) html[2]];
  case 2:
    salt[0] = table[(unsigned char) html[1]];
    break;
  }

  DES_fcrypt(html, salt, dest);

  trips++;

  free(html);
  free(sjis);
  free(salt);
}

char *
prompt(s, size, stream)
     char *s;
     int size;
     FILE *stream;
{
  if (shell)
    fprintf(prompt_file, "\033[02;34m>>>\033[00m ");

  return fgets(s, size, stream);
}

void stdin_loop(void) {
  char * restrict buffer, * restrict ret;
  size_t blen;

  buffer = calloc(1, ARRSIZE);
  ret    = malloc(14);

  while (prompt(buffer, ARRSIZE, stdin)) {
    blen = strlen(buffer);
    buffer[--blen] = '\0';

    memset(ret, 0, 14);

    trip(buffer, blen, ret);

    if (shell && !file)
      printf(output_fmt, ret + 3);
    else
      printf(output_fmt, buffer, ret + 3);
  }

  if (shell)
    fputc('\n', prompt_file);

  free(buffer);
  free(ret);

  return;
}

void args_loop(int argc, char **argv) {
  char *t, * restrict ret;

  ret = malloc(14);

  while (--argc) {
    memset(ret, 0, 14);

    t = *++argv;
    trip(t, strlen(t), ret);

    printf(output_fmt, t, ret + 3);
  }

  free(ret);

  return;
}

void report(void) {
  if (trips < 1000)
    return;

  time_t start_secs, end_secs;
  suseconds_t start_msecs, end_msecs;
  start_secs = timebuf.tv_sec;
  start_msecs = timebuf.tv_usec;

  gettimeofday(&timebuf, NULL);
  end_secs = timebuf.tv_sec;
  end_msecs = timebuf.tv_usec;

  unsigned long seconds  =  end_secs - start_secs;
  unsigned long mseconds = end_msecs - start_msecs;

  fprintf(stderr,
	  "trips: %llu\n"
	  "time:  %lu.%06lu seconds\n"
	  "ratio: %lf trips/sec\n",
	  trips, seconds, mseconds, 
	  (double) trips / (seconds + (1e-6 * mseconds)));
  
  return;
}

void interrupted(int killval) {
  report();
  exit(killval);
}

int main(int argc, char **argv) {
  shell =  isatty( STDIN_FILENO);
  file  = !isatty(STDOUT_FILENO);

  prompt_file = file ? stderr : stdout;

  if (file)
    output_fmt = "#%s => !%s\n";
  else if (!shell || argc > 1)
    output_fmt = "#\033[01;32m%s\033[0m => !\033[01;31m%s\033[0m\n";
  else
    output_fmt = "!\033[01;31m%s\033[0m\n";

  cd = iconv_open("SJIS//IGNORE", "UTF-8");

  signal(SIGTERM, &interrupted);
  signal(SIGINT,  &interrupted);

  gettimeofday(&timebuf, NULL);

  if (argc > 1)
    args_loop(argc, argv);
  else
    stdin_loop();

  report();

  iconv_close(cd);
  exit(EXIT_SUCCESS);
}
