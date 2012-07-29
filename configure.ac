#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([TripTester], [1.0.0], [trulses@gmail.com],, [https://github.com/crypt3lx2k/TripTester])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AC_SEARCH_LIBS([DES_fcrypt], [crypto ssl])

if test "$ac_cv_search_DES_fcrypt" = "no" ; then
   PKG_CHECK_MODULES([SSL], [openssl],, [AC_MSG_ERROR([can't find openssl installation])])
   LIBS="$SSL_LIBS $LIBS"
   AM_CFLAGS="$SSL_CFLAGS $AM_CFLAGS"
fi

# Checks for header files.
AC_CHECK_HEADERS([errno.h iconv.h stdio.h stdlib.h string.h unistd.h],, [AC_MSG_ERROR([POSIX standard headers not found])])
AC_CHECK_HEADERS([openssl/des.h],, [AC_MSG_WARN([openssl/des.h not found, will attempt to create valid prototype])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.

AC_OUTPUT(Makefile)
