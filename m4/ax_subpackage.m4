# usage: AX_SUBPACKAGE(package_name,header,library,function,conditional_name,autouse)

AC_DEFUN([AX_SUBPACKAGE], [

AX_ARG_WITH($1)

AX_SEARCH_HEADERS($1,$2)

#search for library
AX_SUBPACKAGE_OLD_LIBS=$LIBS
libs_to_link=""
$1_found_library=yes
for function in $4
do
	if test "$1_found_library" != "no" -a "$3" != "" -a  "$function" != ""
	then
		AC_SEARCH_LIBS([$function],[$3],[$1_found_library=yes],[$1_found_library=no])
		libs_to_link="$(eval echo \$ac_cv_search_$function) $libs_to_link"
	fi
done
NEW_LIBS=$LIBS
LIBS=$AX_SUBPACKAGE_OLD_LIBS

#check availability
if test "$$1_found_header" != "no"  -a "$$1_found_library" != "no"
then
	$1_found=yes
else
	$1_found=no
fi

#set default value
if test "$6" == "" -o "$6" == true
then
	auto=true
	errsuff="(automatically enabled if found)"
	default=${$1_found}
else
	auto=false
	default=false
fi

#introduce enable
AC_ARG_ENABLE($1,
	AS_HELP_STRING([--enable-$1],[Enable $1 ${errsuff}]),
	enable_$1="${enableval}",
	enable_$1="${default}")
AC_MSG_RESULT([enabling $1 ... ${enable_$1}])
SUMMARY_RESULT="$SUMMARY_RESULT
$1 enabled        : $enable_$1"

#check activability
if test "$enable_$1" == "yes"
then
	if test "$$1_found" == "no"
	then
		AC_MSG_ERROR(["Cannot enable $1, library not found, please provide/improve hint using --with-$1 flag"])
	fi

	if test "$5" != ""
	then
		AC_DEFINE([USE_$5],1,[Enable $1])
	fi

	LIBS=$NEW_LIBS
	LIBRARY_RESULT="$LIBRARY_RESULT
$1                : $libs_to_link"

fi

AM_CONDITIONAL([USE_$5],[test "$enable_$1" == "yes" ],[true],[false])
])
