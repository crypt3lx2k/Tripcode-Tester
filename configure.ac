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
PKG_CHECK_MODULES([SSL], [openssl],, AC_MSG_ERROR(openssl not found))

# Checks for header files.
AC_CHECK_HEADERS([errno.h iconv.h stdio.h stdlib.h string.h unistd.h],, AC_MSG_ERROR(POSIX standard headers not found))
AC_CHECK_HEADERS([openssl/des.h],, AC_MSG_ERROR(Can't locate OpenSSL development files for des openssl/des.h))

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.

AC_OUTPUT(Makefile)
