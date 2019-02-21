# usage: AX_SEARCH_HEADERS(package_name,headers)

#search for header
AC_DEFUN([AX_SEARCH_HEADERS], [

$1_found_headers="none needed"
for header in $2
do
	if test "$1_found_header" != "no"
	then
		AC_CHECK_HEADERS([$header],[$1_found_headers=yes],[$1_found_header=no])
	fi
done
])
