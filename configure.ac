#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(s3fp, 0.1, wfchiang@cs.utah.edu)
AM_INIT_AUTOMAKE(s3fp, 0.1) 
AC_OUTPUT(
Makefile 
src/Makefile 
examples/Makefile 
examples/lib/Makefile 
examples/lib/div_geometric_primitives/Makefile 
examples/tests-fp-error/Makefile 
examples/tests-fp-error/balanced-reduction/Makefile
examples/tests-fp-error/imbalanced-reduction/Makefile
examples/tests-fp-error/kahan-sum/Makefile
examples/tests-div-detection/Makefile 
examples/tests-div-detection/pp_3x3/Makefile 
examples/tests-div-detection/pp_4x4/Makefile)

# AC_CONFIG_SRCDIR([scripts/utils/show_input_to_errors.cpp])
# AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_ENABLE_SHARED
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([float.h inttypes.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset sqrt])

AC_OUTPUT
