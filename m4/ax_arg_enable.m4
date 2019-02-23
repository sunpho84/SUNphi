# usage: AX_ARG_ENABLE(feature,default)

# introduce enable
AC_DEFUN([AX_ARG_ENABLE], [
AC_ARG_ENABLE($1,
	AS_HELP_STRING([--enable-$1],[Enable $1 ${errsuff}]),
	enable_$1="${enableval}",
	enable_$1=$2)
AC_MSG_RESULT([enabling $1 ... ${enable_$1}])
SUMMARY_RESULT="$SUMMARY_RESULT
$1 enabled        : $enable_$1"
])
