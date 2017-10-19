AC_INIT([SUNphi],[1.0],[francesco.sanfilippo@infn.it])

AC_CONFIG_SRCDIR([include])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([1.0 no-define foreign subdir-objects])
AM_CONFIG_HEADER(include/config.hpp)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

AC_PROG_RANLIB

#enable C and C++
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX(14,noext,mandatory)
#activate all warnings
AX_CXXFLAGS_WARN_ALL

#silent automake
AM_SILENT_RULES([yes])

##search openMP
AC_OPENMP
CFLAGS="$CXXFLAGS $OPENMP_CFLAGS"
CPPFLAGS="$CPPFLAGS $OPENMP_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $OPENMP_CXXFLAGS"

#doxygen
DX_INIT_DOXYGEN($PACKAGE_NAME,[config/Doxyfile],[docs])
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_MAN_FEATURE(ON)
DX_HTML_FEATURE(ON)
AC_SUBST([DOXYGEN_OUTPUT_DIR],[docs])
 
#configure output
AC_CONFIG_FILES(Makefile bin/Makefile config/Doxyfile)
 
AC_OUTPUT
