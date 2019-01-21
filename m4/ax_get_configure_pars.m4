# Usage: AX_GET_CONFIGURE_PARS($*)

AC_DEFUN([AX_GET_CONFIGURE_PARS], [

# Include all config flags
config_flags="$*"
AC_DEFINE_UNQUOTED([CONFIG_FLAGS],["$config_flags"],[Flags passed to configure])

# Get configuration time
AC_MSG_CHECKING([configuration date and time])
CONFIG_TIME=`date`
AC_DEFINE_UNQUOTED([CONFIG_TIME], "$CONFIG_TIME", [time when configured])
AC_MSG_RESULT([$CONFIG_TIME])
])
