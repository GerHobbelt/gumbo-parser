#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([gumbo], [0.10.1], [jonathan.d.tang@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/parser.c])
#AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile gumbo.pc])

# Checks for programs.
AC_PROG_CC_C99
AC_PROG_CXX
AC_DEFUN([GUMBO_CHECK_PROG],[dnl
AC_ARG_VAR([$1],[$3])
AC_CHECK_PROG([$1],[[$2]],[yes],[no])
AM_CONDITIONAL([$1_FOUND],[[[ "x${$1}" = 'xyes' ]]])])
GUMBO_CHECK_PROG([PYTHON],[python],[Path to python interpreter])
GUMBO_CHECK_PROG([RAGEL],[ragel],[Path to ragel state-machine compiler])
GUMBO_CHECK_PROG([GPERF],[gperf],[Path to gperf perfect-hash function generator])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_LIB([gtest_main],
             [main],
             AM_CONDITIONAL(HAVE_SHARED_LIBGTEST, [true]),
             AM_CONDITIONAL(HAVE_SHARED_LIBGTEST, [false]))

AC_ARG_ENABLE([examples],
              AS_HELP_STRING([--disable-examples], [Disable examples]))
AM_CONDITIONAL([HAVE_EXAMPLES], [test "x$enable_examples" != "xno"])

# Init Automake & libtool
AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
LT_INIT

AC_OUTPUT
