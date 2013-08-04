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

#include <errno.h>
#include <iconv.h>
#include <string.h>

#if HAVE_OPENSSL_DES_H
# include <openssl/des.h>
#else
/* try to declare a valid prototype for the
   function and then keep fingers crossed */
extern char * DES_fcrypt (const char * buf,
			  const char * salt,
			  char * ret);
#endif /* HAVE_OPENSSL_DES_H */

#include "tripcode.h"

static char salt_table [] =
  "................................"
  ".............../0123456789ABCDEF"
  "GABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
  "fabcdefghijklmnopqrstuvwxyz....."
  "................................"
  "................................"
  "................................"
  "................................";

size_t tripcode_html (const char * src, char * dest) {
  const char * s;
  size_t len;

  /* note that this macro mutates
     both dest __and__ len. */
#define STRCATINC(dest, s, len)			\
  do {						\
    memcpy(dest + len, s, sizeof(s) - 1);	\
    len += sizeof(s) - 1;			\
  } while (0)

  /*
   * These are supposed to be HTML names,
   * feel free to add new ones if you're
   * able to input them.
   */
  for (len = 0, s = src; len < 8 && *s != '\0'; s += 1) {
    switch (*s) {
    case '\"':
      STRCATINC(dest, "&quot;", len);
      break;
    case '&':
      STRCATINC(dest, "&amp;", len);
      break;
    case '<':
      STRCATINC(dest, "&lt;", len);
      break;
    case '>':
      STRCATINC(dest, "&gt;", len);
      break;
    default:
      dest[len++] = *s;
      break;
    }
  }

#undef STRCATINC

  dest[len] = '\0';
  return len;
}

size_t tripcode_sjis (iconv_t cd,
		      char * s,    size_t len,
		      char * dest, size_t size) {
  size_t e;
  size_t l = size;

  errno = 0;
  e = iconv(cd, &s, &len, &dest, &l);

  if (errno && e == (size_t) -1) {
    /* ignoring invalid multibyte sequences is
       congruent to the way tripcodes are actually
       handled. */
    if (errno != EILSEQ)
      return (size_t) -1;
  }

  dest[0] = '\0';
  return size - l;
}

char * tripcode_crypt (const char * s, size_t len, char * dest) {
  char salt[] = "HH";

  switch (len) {
  case 1:
    salt[1] = '.';
    break;
  default:
    salt[1] = salt_table[(unsigned char) s[2]];
  case 2:
    salt[0] = salt_table[(unsigned char) s[1]];
    break;
  }

  (void) DES_fcrypt(s, salt, dest);

  return dest + 3;
}

char * tripcode (iconv_t cd, char * input, size_t len, char * output) {
#define BUFFER_LENGTH 32
  char html[BUFFER_LENGTH] = {0};
  char sjis[BUFFER_LENGTH] = {0};

  len = tripcode_sjis(cd,
		      input, len,
		      sjis, BUFFER_LENGTH);

  if (len == (size_t) -1)
    return NULL;

  len = tripcode_html(sjis, html);
  return tripcode_crypt(sjis, len, output);
#undef BUFFER_LENGTH
}
