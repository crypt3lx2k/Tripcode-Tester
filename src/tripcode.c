#include <errno.h>
#include <iconv.h>
#include <string.h>

#if HAVE_OPENSSL_DES_H
# include <openssl/des.h>
#else
extern char * DES_fcrypt (const char * buf,
			  const char * salt,
			  char * ret);
#endif

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

size_t transform_html (const char * src, char * dest) {
  const char * s;
  size_t len;

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

size_t transform_sjis (iconv_t cd,
		       char * s,    size_t len,
		       char * dest, size_t size) {
  size_t e;
  size_t l = size;

  errno = 0;
  e = iconv(cd, &s, &len, &dest, &l);

  if (errno || e == (size_t) -1)
    return (size_t) -1;

  return size - l;
}

void trip (const char * s, size_t len, char * dest) {
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
}
