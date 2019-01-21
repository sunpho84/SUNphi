
# Include the header directory
AM_CPPFLAGS= \
	-I$(top_srcdir)/include

# Build objects in subdirectories
AUTOMAKE_OPTIONS= \
	subdir-objects

# Include the library
LDADD= \
	$(top_builddir)/lib/libsunphi.a

# Init to null the list of program to ease the addition
bin_PROGRAMS=

bin_PROGRAMS+=main
main_SOURCES=main.cpp

bin_PROGRAMS+=checks
checks_SOURCES=checks.cpp
