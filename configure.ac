# Process this file with autoconf to produce a configure script.


# Autoconf setup

AC_PREREQ([2.69])
AC_INIT([histogramr], [0.1.0], [torsten.scholak+histogramr@googlemail.com], [histogramr], [https://github.com/tscholak/histogramr])
AC_CONFIG_SRCDIR([src/data.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE


# Programs

dnl Checks for programs
AC_LANG(C)
AC_PROG_CC
AC_PROG_LN_S

dnl Checks for headers
AC_HEADER_STDC
AC_HEADER_MAJOR
AC_CHECK_HEADERS([stdio.h time.h sys/time.h math.h getopt.h limits.h])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

dnl Checks for library functions
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor gettimeofday strncasecmp strrchr strtol])
AC_CHECK_LIB([m],[log10])


# Compiler

dnl Preprocessor and compiler flags
AC_ARG_WITH(cppflags,
            AC_HELP_STRING([--with-cppflags=VAL],
                           [CPPFLAGS value (def: )]),
            [CPPFLAGS="$withval"],[CPPFLAGS=""])
AC_ARG_WITH(cflags,
            AC_HELP_STRING([--with-cflags=VAL],
                           [CFLAGS value (def: -Wall)]),
            [CFLAGS=$withval],[CFLAGS="-Wall"])
AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [Add -g to CFLAGS]),
              [CFLAGS="-g -O0 $CFLAGS"],[CFLAGS="-O3 $CFLAGS"])
AC_ARG_ENABLE(timing,
              AC_HELP_STRING([--enable-timing],
                             [Output timing information]),
              [AC_DEFINE(TIMING, 1, Output timing information)],)


# GNU Scientific Library

dnl GSL
AC_LANG_C
AC_ARG_WITH(gsllib, 
            AC_HELP_STRING([--with-gsllib=DIR],
                           [DIR=-LDIR, has libgsl and libgslcblas]),
            [gsllib="$withval/"],[gsllib=""])
if test $gsllib; then
  AC_MSG_RESULT(Checking user specified GSL)
  LDFLAGS="$LDFLAGS -L$gsllib"
  AC_CHECK_LIB(gsl, main,,
               AC_MSG_ERROR("libgsl not in location specified by --with-gsllib: $gsllib"),
               [-lm -lgslcblas -L$gsllib])
  AC_CHECK_LIB(gslcblas, main,,
               AC_MSG_ERROR("libgslcblas not in location specified by --with-gsllib: $gsllib"),
               [-lm -L$gsllib])
else
  AC_MSG_RESULT(Checking GSL)
  AC_CHECK_LIB(gsl, main,,
               AC_MSG_ERROR("libgsl not found; use --with-gsllib"),
               [-lm -lgslcblas])
  AC_CHECK_LIB(gslcblas, main,,
               AC_MSG_ERROR("libgslcblas not found; use --with-gsllib"),
               [-lm])
fi

AC_ARG_WITH(gslinc,
            AC_HELP_STRING([--with-gslinc=VAL],
                           [VAL=-IVAL, has gsl include files]),
            [gslinc="$withval/" CPPFLAGS="$CPPFLAGS -I$gslinc"],[gslinc=""])
AC_MSG_RESULT(Checking GSL headers)
AC_CHECK_HEADERS([gsl/gsl_errno.h gsl/gsl_math.h gsl/gsl_min.h],,
                 [if test $gslinc; then
                    AC_MSG_ERROR("gsl headers not in location specified by --with-gslinc")
                  elif test $gsllib; then
                    AC_CHECK_HEADER($gsllib/../include/gsl/gsl_math.h,
                                    [gslinc="$gsllib/../include/" CPPFLAGS="$CPPFLAGS -I$gslinc"],
                                    AC_MSG_ERROR("Can't find gsl includes in user specified location; use --with-gslinc"))
                  else
                    AC_MSG_ERROR("Can't find gsl includes; use --with-gslinc")
                  fi])


# HDF5 library

dnl HDF5
AC_LANG_C
AC_ARG_WITH(hdfinc,
            AC_HELP_STRING([--with-hdfinc=VAL],
                            [VAL=-IVAL, has HDF5 include files]), 
            [hdfinc="$withval/" CPPFLAGS="$CPPFLAGS -I$hdfinc"],[hdfinc=""])
AC_ARG_WITH(hdflib,
            AC_HELP_STRING([--with-hdflib=DIR],
                           [DIR=-LDIR, has libhdf5])],
            [hdflib="$withval/"],[hdflib=""])

dnl HDF5 header
AC_MSG_RESULT(Checking HDF5 headers)
AC_CHECK_HEADERS([hdf5.h],,
                 [if test $hdfinc; then
                    AC_MSG_ERROR("hdf5 headers not in location specified by --with-hdfinc")
                  elif test $hdflib; then
                    AC_CHECK_HEADER($hdflib/../include/hdf5.h,
                                    [hdfinc="$hdflib/../include/" CPPFLAGS="$CPPFLAGS -I$hdfinc"],
                                    AC_MSG_ERROR("Can't find HDF5 includes in user specified location; use --with-hdfinc"))
                  else
                    AC_MSG_ERROR("Can't find HDF5 includes; use --with-hdf5inc")
                  fi])

dnl HDF5 library
if test $hdflib; then
  AC_MSG_RESULT(Checking user specified HDF5)
  LDFLAGS="$LDFLAGS -L$hdflib"
  LIBS="$LIBS -lhdf5"
  AC_CHECK_LIB(hdf5,main,,
               AC_MSG_ERROR("libhdf5 not in location specified by --with-hdflib: $hdflib"),
               [-lhdf5 -L$hdflib])

else
  AC_MSG_RESULT(Checking HDF5)
  AC_CHECK_LIB(hdf5, main,,
               AC_MSG_ERROR("libhdf5 not found; use --with-hdflib"),)
fi


# Closing commands

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT