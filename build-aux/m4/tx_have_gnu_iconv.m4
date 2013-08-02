#
# SYNOPSIS
#
#   TX_HAVE_GNU_ICONV([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   This macro attempts to determine whether the GNU extensions suffixes
#   //IGNORE and //TRANSLIT are available for use in iconv_open.
#
#   On success this defines HAVE_GNU_ICONV to 1. The result of this macro is
#   cached in the tx_cv_have_gnu_iconv variable with either the value "yes"
#   (successful) or "no" (unsuccessful).
#
# LICENSE
#
#   Copyright (c) 2013 Truls Edvard Stokke
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
#

#serial 1

AC_DEFUN([TX_HAVE_GNU_ICONV], [
  SAVED_CPPFLAGS="$CPPFLAGS"
  SAVED_CFLAGS="$CFLAGS"
  SAVED_LDFLAGS="$LDFLAGS"
  SAVED_LIBS="$LIBS"

  AC_CACHE_CHECK([for gnu style iconv], [tx_cv_have_gnu_iconv], [
    AC_MSG_RESULT([(group)])
    AC_REQUIRE([AC_PROG_CC])
    AC_LANG_PUSH([C])
    AC_SEARCH_LIBS([iconv_open], [iconv])
    AC_CHECK_HEADER([iconv.h], [
      AC_TRY_RUN([
        #include <iconv.h>
        int main(void) {
          if (iconv_open("ASCII//IGNORE", "UTF-8") == (iconv_t) -1)
            return 1;
          if (iconv_open("ASCII//TRANSLIT", "UTF-8") == (iconv_t) -1)
            return 1;
          return 0;
        }],
        [tx_cv_have_gnu_iconv="yes"],
        [tx_cv_have_gnu_iconv="no"], [
          AC_MSG_WARN([cross-compiling, unable to determine gnu iconv support])
          tx_cv_have_gnu_iconv="no"
        ]
      )],
      [tx_cv_have_gnu_iconv="no"]
    )
    AC_LANG_POP([C])    
    AC_MSG_CHECKING([for gnu style iconv])
  ])

  CPPFLAGS="$SAVED_CPPFLAGS"
  CFLAGS="$SAVED_CFLAGS"
  LDFLAGS="$SAVED_LDFLAGS"
  LIBS="$SAVED_LIBS"

  if test "x$tx_cv_have_gnu_iconv" = "xyes" ; then
    AC_DEFINE([HAVE_GNU_ICONV], [1], [gnu iconv support])
  fi

  if test "x$tx_cv_have_gnu_iconv" = "xyes" ; then
    ifelse([$1], [], [:], [$1])
  else
    ifelse([$2], [], [:], [$2])
  fi
])
