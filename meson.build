project(
	'gumbo-parser',
	'c',
	version: '0.13.0',
	default_options: ['c_std=c99', 'cpp_std=c++14']
)

gumbo_src = files(
	'src/attribute.c',
	'src/char_ref.c',
	'src/error.c',
	'src/parser.c',
	'src/string_buffer.c',
	'src/string_piece.c',
	'src/tag.c',
	'src/tokenizer.c',
	'src/utf8.c',
	'src/util.c',
	'src/vector.c',
)

libgumbo = both_libraries('gumbo', gumbo_src, version: '3.0.0', install: true)

install_headers('src/gumbo.h', 'src/tag_enum.h')

pkg = import('pkgconfig')
pkg.generate(libgumbo, filebase: 'gumbo', name: 'Gumbo', description: 'A fully-compliant HTML5 parser.')

if get_option('tests')
	add_languages('cpp')

	# TODO: Fallback in case gtest isn't installed
	gtest_main = dependency('gtest_main')

	gumbo_test_src = files(
		'tests/attribute.cc',
		'tests/char_ref.cc',
		'tests/parser.cc',
		'tests/string_buffer.cc',
		'tests/string_piece.cc',
		'tests/tokenizer.cc',
		'tests/test_utils.cc',
		'tests/utf8.cc',
		'tests/vector.cc',
	)

	gumbo_test = executable(
		'gumbo_test',
		gumbo_test_src,
		link_with: [libgumbo],
		include_directories: ['src/'],
		dependencies: [gtest_main],
		install: false
	)

	test('gumbo_test', gumbo_test)
endif
