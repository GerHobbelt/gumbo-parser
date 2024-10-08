# Googletest magic.  Gumbo relies on Googletest for its unittests, and
# Googletest (by design) does not provide a "make install" option.  Instead,
# we'll assume you copy (or symlink) the Googletest distribution into a 'gtest'
# directory inside the main library directory, and then provide rules to build
# it automatically.  This approach (and these rules) are copied from the
# protobuf distribution.

ACLOCAL_AMFLAGS = -I m4

if !HAVE_SHARED_LIBGTEST

# Build gtest before we build protobuf tests.  We don't add gtest to SUBDIRS
# because then "make check" would also build and run all of gtest's own tests,
# which takes a lot of time and is generally not useful to us.  Also, we don't
# want "make install" to recurse into gtest since we don't want to overwrite
# the installed version of gtest if there is one.
check-local: gtest/lib/libgtest.la gtest/lib/libgtest_main.la

gtest/lib/libgtest.la gtest/lib/libgtest_main.la: gtest/Makefile
	@echo "Making lib/libgtest.a lib/libgtest_main.a in gtest"
	@cd gtest && $(MAKE) $(AM_MAKEFLAGS) lib/libgtest.la lib/libgtest_main.la

gtest/Makefile: gtest/configure
	@cd gtest && ./configure

# We would like to clean gtest when "make clean" is invoked.  But we have to
# be careful because clean-local is also invoked during "make distclean", but
# "make distclean" already recurses into gtest because it's listed among the
# DIST_SUBDIRS.  distclean will delete gtest/Makefile, so if we then try to
# cd to the directory again and "make clean" it will fail.  So, check that the
# Makefile exists before recursing.
clean-local:
	@if test -e gtest/Makefile; then \
		echo "Making clean in gtest"; \
		cd gtest && $(MAKE) $(AM_MAKEFLAGS) clean; \
	fi

endif !HAVE_SHARED_LIBGTEST

if PYTHON_FOUND
if GPERF_FOUND
@srcdir@/src/tag_gperf.h: src/tag.in gentags.py genperf.py
	@echo GENPERF $<
	@python gentags.py $<
	@echo GPERF $<
	@gperf -LANSI-C --ignore-case -m200 $< |python genperf.py >@srcdir@/src/tag_gperf.h
endif
endif

if RAGEL_FOUND
@srcdir@/src/char_ref.c: src/char_ref.rl
	@echo RAGEL src/char_ref.rl
	@cd @srcdir@/src && ragel -F0 char_ref.rl > char_ref.c
endif

lib_LTLIBRARIES = libgumbo.la
libgumbo_la_CFLAGS = -Wall
libgumbo_la_LDFLAGS = -version-info 1:0:0 -no-undefined
libgumbo_la_SOURCES = \
				src/attribute.c \
				src/attribute.h \
				src/char_ref.c \
				src/char_ref.h \
				src/error.c \
				src/error.h \
				src/insertion_mode.h \
				src/parser.c \
				src/parser.h \
				src/string_buffer.c \
				src/string_buffer.h \
				src/string_piece.c \
				src/string_piece.h \
				src/tag.c \
				src/tag_enum.h \
				src/tag_gperf.h \
				src/tag_strings.h \
				src/tag_sizes.h \
				src/token_type.h \
				src/tokenizer.c \
				src/tokenizer.h \
				src/tokenizer_states.h \
				src/utf8.c \
				src/utf8.h \
				src/util.c \
				src/util.h \
				src/vector.c \
				src/vector.h \
                src/xpath.h \
                src/xpath.c
include_HEADERS = src/gumbo.h src/tag_enum.h src/vector.h src/xpath.h src/parser.h src/string_buffer.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gumbo.pc

check_PROGRAMS = gumbo_test
TESTS = gumbo_test
gumbo_test_CPPFLAGS = \
				-I"$(srcdir)/." \
				-I"$(srcdir)/src" \
				-I"$(srcdir)/gtest/include"
gumbo_test_SOURCES = \
				tests/attribute.cc \
				tests/char_ref.cc \
				tests/parser.cc \
				tests/string_buffer.cc \
				tests/string_piece.cc \
				tests/tokenizer.cc \
				tests/tag.cc \
				tests/test_utils.cc \
				tests/utf8.cc \
				tests/vector.cc \
				tests/error.cc
gumbo_test_DEPENDENCIES = libgumbo.la
gumbo_test_LDADD = libgumbo.la

if HAVE_SHARED_LIBGTEST
# FIXME(bnoordhuis) Should be configurable by the user.
gumbo_test_LDADD += -lgtest -lgtest_main
else
gumbo_test_DEPENDENCIES += check-local
gumbo_test_LDADD += gtest/lib/libgtest.la gtest/lib/libgtest_main.la
endif

if HAVE_EXAMPLES
noinst_PROGRAMS = clean_text find_links get_title positions_of_class benchmark serialize prettyprint print evalxpath
endif
LDADD = libgumbo.la
AM_CPPFLAGS = -I"$(srcdir)/src"
AM_CXXFLAGS = -std=c++11 -I"$(srcdir)/src"

clean_text_SOURCES = examples/clean_text.cc
find_links_SOURCES = examples/find_links.cc
get_title_SOURCES = examples/get_title.c
positions_of_class_SOURCES = examples/positions_of_class.cc
benchmark_SOURCES = benchmarks/benchmark.cc
serialize_SOURCES = examples/serialize.cc
prettyprint_SOURCES = examples/prettyprint.cc
print_SOURCES = tools/print.cc
evalxpath_SOURCES = examples/eval_xpath.cc
