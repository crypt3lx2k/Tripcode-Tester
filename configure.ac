#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([Tripcode Tester], [2.0.0], [trulses@gmail.com], [tripcode_tester], [https://github.com/crypt3lx2k/Tripcode-Tester])
AC_CONFIG_SRCDIR([src/main.c])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AC_SEARCH_LIBS([DES_fcrypt], [crypto ssl crypt openssl])

if test "$ac_cv_search_DES_fcrypt" = "no" ; then
   PKG_CHECK_MODULES([SSL], [openssl], [], [AC_MSG_ERROR([can't find openssl installation])])
   LIBS="$SSL_LIBS $LIBS"
   AM_CFLAGS="$SSL_CFLAGS $AM_CFLAGS"
fi

dnl keep this ordering between curses and readline
AC_SEARCH_LIBS([iconv_open], [iconv], [], [AC_MSG_ERROR([no iconv library found, will be unable to compile])])
AC_SEARCH_LIBS([tputs], [curses ncurses], [AC_DEFINE([HAVE_LIB_CURSES])], [AC_MSG_WARN([curses not found, colored output will not be available])])
AC_SEARCH_LIBS([readline], [readline], [AC_DEFINE([HAVE_LIB_READLINE])], [AC_MSG_WARN([readline not found, line editing will not be available])])

# Checks for header files.
AC_CHECK_HEADERS([errno.h iconv.h stddef.h stdio.h stdlib.h string.h unistd.h], [], [AC_MSG_ERROR([POSIX standard headers not found])])
AC_CHECK_HEADERS([openssl/des.h], [], [AC_MSG_WARN([openssl/des.h not found, will attempt to create valid prototype for DES_fcrypt])])

if test "$ac_cv_search_tputs" != "no" ; then
   AC_CHECK_HEADERS([curses.h term.h], [], [AC_MSG_WARN([curses header files not found, colored output will not be available])]) 
fi

if test "$ac_cv_search_readline" != "no" ; then
   AC_CHECK_HEADERS([readline/readline.h readline/history.h], [], [AC_MSG_WARN([readline header files not found])])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([iconv iconv_open iconv_close], [], [AC_MSG_ERROR([required iconv functions not found, will not be able to compile])])
TX_HAVE_GNU_ICONV([], [AC_MSG_ERROR([can't compile without GNU iconv support])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
