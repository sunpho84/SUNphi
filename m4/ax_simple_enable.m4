# usage: AX_SIMPLE_ENABLE(Functionality, Default value, Description)

AC_DEFUN([AX_SIMPLE_ENABLE], [

#introduce enable
AC_ARG_ENABLE($1,
	AS_HELP_STRING([--enable-$1],[$3]),
	enable_$1="${enableval}",
	enable_$1="$2")
AC_MSG_RESULT([enabling $1 ... ${enable_$1}])
SUMMARY_RESULT="$SUMMARY_RESULT
$1 enabled        : $enable_$1"
])
