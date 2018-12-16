ACLOCAL_AMFLAGS = -I m4

gentags: src/tag.in
	@python gentags.py $<
	@gperf -LANSI-C --ignore-case -m200 $< |python genperf.py >src/tag_gperf.h

lib_LTLIBRARIES = libgumbo.la
libgumbo_la_CFLAGS = -Wall
libgumbo_la_LDFLAGS = -version-info 1:0:0 -no-undefined
libgumbo_la_SOURCES = \
				src/ascii.c \
				src/ascii.h \
				src/attribute.c \
				src/attribute.h \
				src/char_ref.c \
				src/char_ref.h \
				src/error.c \
				src/error.h \
				src/foreign_attrs.c \
				src/insertion_mode.h \
				src/macros.h \
				src/parser.c \
				src/parser.h \
				src/replacement.h \
				src/string_buffer.c \
				src/string_buffer.h \
				src/string_piece.h \
				src/svg_attrs.c \
				src/svg_tags.c \
				src/tag.c \
				src/tag_lookup.c \
				src/tag_lookup.h \
				src/token_type.h \
				src/tokenizer.c \
				src/tokenizer.h \
				src/tokenizer_states.h \
				src/utf8.c \
				src/utf8.h \
				src/util.c \
				src/util.h \
				src/vector.c \
				src/vector.h
include_HEADERS = src/gumbo.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gumbo.pc

check_PROGRAMS = gumbo_test
TESTS = gumbo_test
gumbo_test_CPPFLAGS = \
				-I"$(srcdir)/." \
				-I"$(srcdir)/src"
gumbo_test_SOURCES = \
				tests/attribute.c \
				tests/char_ref.c \
				tests/parser.c \
				tests/string_buffer.c \
				tests/string_piece.c \
				tests/tokenizer.c \
				tests/test.c \
				tests/utf8.c \
				tests/vector.c \
				tests/main.c
gumbo_test_DEPENDENCIES = libgumbo.la
gumbo_test_LDADD = libgumbo.la

noinst_PROGRAMS = clean_text find_links get_title positions_of_class benchmark serialize prettyprint
LDADD = libgumbo.la
AM_CPPFLAGS = -I"$(srcdir)/src"

clean_text_SOURCES = examples/clean_text.cc
find_links_SOURCES = examples/find_links.cc
get_title_SOURCES = examples/get_title.c
positions_of_class_SOURCES = examples/positions_of_class.cc
benchmark_SOURCES = benchmarks/benchmark.c
serialize_SOURCES = examples/serialize.cc
prettyprint_SOURCES = examples/prettyprint.cc

