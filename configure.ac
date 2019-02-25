AC_INIT([SUNphi],[1.0],[francesco.sanfilippo@infn.it])

AX_GET_CONFIGURE_PARS($*)

AC_CONFIG_SRCDIR([include])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([1.0 no-define foreign subdir-objects])
AM_CONFIG_HEADER(include/config.hpp)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

AC_PROG_RANLIB

# Enable C and C++
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX(17,noext,mandatory)

# Activate all warnings
AX_CXXFLAGS_WARN_ALL

# Silent automake
AM_SILENT_RULES([yes])

# Search for pthread
AX_SUBPACKAGE(threads,pthread.h,pthread,pthread_getconcurrency,THREADS,autouse)

# Search automatically MPI for c++
AC_LANG_PUSH([C++])
LX_FIND_MPI
AC_LANG_POP([C++])
AC_MSG_RESULT([with C++ MPI preprocessor options... $MPI_CXXFLAGS])
AC_MSG_RESULT([with C++ MPI link options... $MPI_CXXLDFLAGS])
AC_MSG_RESULT([with C++ MPI libs... $MPI_CXXLIBS])

# Debug or not
AX_SIMPLE_ENABLE([debug],[no],[Enable debug mode (strip all optimization, add -O0 -g)])
if test "$enable_debug" == "yes"
then
  changequote({,})
  remove_flags='s|-O[0-9s]*||g;s|-g[0-9]*||g'

  CFLAGS=`echo "$CFLAGS"|sed -e $remove_flags`
  CPPFLAGS=`echo "$CPPFLAGS"|sed -e $remove_flags`
  CXXFLAGS=`echo "$CXXFLAGS"|sed -e $remove_flags`

  CPPFLAGS="$CPPFLAGS -g -O0"
  
  changequote([,])
  
  AC_DEFINE([DEBUG_MODE],1,[Enable debug mode])
fi

# Check if we enable MPI
AX_SIMPLE_ENABLE([MPI],[$have_CXX_mpi],[Enable MPI])
if test "$enable_MPI" == "yes"
then
	# Check if we have MPI
	if test "$have_CXX_mpi" == "yes"
	then
		CPPFLAGS="$MPI_CXXFLAGS $CPPFLAGS"
		LIBS="$MPI_CXXLIBS $LIBS"
		LDFLAGS="$MPI_CXXLDFLAGS $LDFLAGS"
		AC_DEFINE([USE_MPI],1,[Enable MPI])
	else
		AC_MSG_ERROR(["Unable to find MPI"])
	fi
fi

# Search for dwarf
if test "$enable_debug" == "yes"
then
	default_dwarf=autouse
else
	default_dwarf=
fi
AX_SUBPACKAGE(dwarf,libdwarf/dwarf.h,dwarf,dwarf_child,DWARF,$default_dwarf)

# yaml-cpp library
AX_SUBPACKAGE(yaml,yaml-cpp/yaml.h,yaml-cpp,YAML::GraphBuilderAdapter,YAML,mandatory)

# Configure doxygen
DX_INIT_DOXYGEN($PACKAGE_NAME,[config/Doxyfile],[docs])
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_MAN_FEATURE(ON)
DX_HTML_FEATURE(ON)
AC_SUBST([DOXYGEN_OUTPUT_DIR],[docs])
 
# Result of conf to screen
AC_MSG_RESULT([
--------------------------------------------------
Configuration summary for $PACKAGE_NAME ($VERSION)
--------------------------------------------------
CC                  : $CC
CPPFLAGS            : $CPPFLAGS
CXXFLAGS            : $CXXFLAGS
CXX                 : $CXX
---
LDFLAGS             : $LDFLAGS
LIBS                : $LIBS
---
Installation prefix : $prefix
$LIBRARY_RESULT
$SUMMARY_RESULT
--------------------------------------------------
Configuration OK
--------------------------------------------------
])

# Configure output
AC_CONFIG_FILES(Makefile
		bin/Makefile
		lib/Makefile
		config/Doxyfile)

AC_OUTPUT
