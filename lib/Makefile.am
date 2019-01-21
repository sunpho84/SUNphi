
# Include the header directory
AM_CPPFLAGS= \
	-I$(top_srcdir)/include

# Build objects in subdirectories
AUTOMAKE_OPTIONS= \
	subdir-objects

# Target to build the library
lib_LIBRARIES= \
	$(top_builddir)/lib/libsunphi.a

# %D%/git_info.hpp: $(top_srcdir)/.git/HEAD $(top_srcdir)/.git/index $(top_srcdir)/config/get_git_info.sh Makefile
# 	@echo "Reading git version"
# 	@$(top_srcdir)/config/get_git_info.sh $(top_srcdir) > $@
# $(top_srcdir)/.git/HEAD:
# $(top_srcdir)/.git/index:

# BUILT_SOURCES=%D%/git_info.hpp

# Library sources
__top_builddir__lib_libsunphi_a_SOURCES= \
	%D%/Aliver.cpp

