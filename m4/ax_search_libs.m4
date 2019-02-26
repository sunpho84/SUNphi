# usage: AX_SEARCH_LIBS(package,libraries,functions)

# search for libraries
AC_DEFUN([AX_SEARCH_LIBS], [

# backup needed libs
AX_SUBPACKAGE_OLD_LIBS=$LIBS
libs_to_link=""
$1_found_library=yes

for function in $3
do
	if test "$1_found_library" != "no" -a "$2" != "" -a  "$function" != ""
	then
		echo $3|grep -q :: && matching=$?
		if test "$matching" == 0
		then
			AX_CXX_CHECK_LIB([$2],[$function],[$1_found_library=yes],[$1_found_library=no])
			LIBS="-lyaml-cpp $LIBS"
			libs_to_link="-lyaml-cpp $libs_to_link"
		else
			AC_SEARCH_LIBS([$function],[$2],[$1_found_library=yes],[$1_found_library=no])
			libs_to_link="$(eval echo \$ac_cv_search_$function) $libs_to_link"
		fi
	fi
done

# remove old
for old in $AX_SUBPACKAGE_OLD_LIBS
do
   LIBS=$(echo $LIBS|sed 's|'$old'||g')
done

# store needed libs
$1_needed_libs=$LIBS

# restore
LIBS=$AX_SUBPACKAGE_OLD_LIBS
])
