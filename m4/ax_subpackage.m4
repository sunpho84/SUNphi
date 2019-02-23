# usage: AX_SUBPACKAGE(package_name,header,libraries,functions,conditional_name,[autouse,mandatory])

AC_DEFUN([AX_SUBPACKAGE], [

AX_ARG_WITH($1)

AX_SEARCH_HEADERS($1,$2)

AX_SEARCH_LIBS($1,$3,$4)

# check availability
if test "$$1_found_header" != "no"  -a "$$1_found_library" != "no"
then
	$1_found=yes
else
	$1_found=no
fi

if test "$6" != "mandatory"
then

	# set default value for activation
	if test "$6" == "autouse"
	then
	   helpstring="(automatically enabled if found)"
	   default=${$1_found}
	else
	   default=false
	fi

	AX_ARG_ENABLE($1,$default)

else
	enable_$1=yes
fi

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

	LIBS="$LIBS $$1_needed_libs"
	LIBRARY_RESULT="$LIBRARY_RESULT
$1                : $libs_to_link"

fi

AM_CONDITIONAL([USE_$5],[test "$enable_$1" == "yes" ],[true],[false])
])
