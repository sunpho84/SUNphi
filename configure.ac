AC_INIT([SUNphi],[1.0],[francesco.sanfilippo@infn.it])

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

# Search for OpenMP
AC_OPENMP
CFLAGS="$CXXFLAGS $OPENMP_CFLAGS"
CPPFLAGS="$CPPFLAGS $OPENMP_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $OPENMP_CXXFLAGS"

# Configure doxygen
DX_INIT_DOXYGEN($PACKAGE_NAME,[config/Doxyfile],[docs])
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_MAN_FEATURE(ON)
DX_HTML_FEATURE(ON)
AC_SUBST([DOXYGEN_OUTPUT_DIR],[docs])
 
# Check if we enanble MPI autosearch
AC_ARG_ENABLE(MPI-autosearch,
	AS_HELP_STRING([--enable-MPI-autosearch],[Enable MPI autosearch]),
	enable_MPI_autosearch="${enableval}",
	enable_MPI_autosearch="yes")
AC_MSG_RESULT([enabling MPI autosearch... $enable_MPI_autosearch])

# Search automatically MPI for c++
if test "$enable_MPI_autosearch" == "yes"
then
	AC_LANG_PUSH([C++])
	LX_FIND_MPI
	AC_LANG_POP([C++])
	AC_MSG_RESULT([with C++ MPI preprocessor options... $MPI_CXXFLAGS])
	AC_MSG_RESULT([with C++ MPI link options... $MPI_CXXLDFLAGS])
	AC_MSG_RESULT([with C++ MPI libs... $MPI_CXXLIBS])
	CPPFLAGS="$MPI_CXXFLAGS $CPPFLAGS"
	LIBS="$MPI_CXXLIBS $LIBS"
	LDFLAGS="$MPI_CXXLDFLAGS $LDFLAGS"
fi

# Configure output
AC_CONFIG_FILES(Makefile bin/Makefile config/Doxyfile)
 
AC_OUTPUT
