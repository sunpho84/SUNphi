# usage: AX_ARG_WITH(package_name)

# introduce flags
AC_DEFUN([AX_ARG_WITH], [

AC_ARG_WITH($1,
	AS_HELP_STRING([--with-$1[=dir]], [Specify where to find $1]),
	with_$1="${withval}"
	CPPFLAGS="-I${with_$1}/include/ $CPPFLAGS"
	LDFLAGS="-L${with_$1}/lib/ $LDFLAGS",
	with_$1=no)
AC_MSG_RESULT(with $1 ... ${with_$1})
])
