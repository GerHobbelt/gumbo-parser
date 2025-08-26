## Gumbo 0.13.2 (2025-08-26)

This is a maintenance release with no API/ABI changes.

* Add fuzz testing facilities using LLVM's libFuzzer
* Support Autotools builds in environments lacking a C++ compiler (#28)
* Fix compilation with the latest gtest which now requires C++17
* Introduce `default_library` option in Meson for more flexible builds (#12)

This release is fully compatible with the current [html5lib-tests](https://github.com/html5lib/html5lib-tests) (a9f4496)

## Gumbo 0.13.1 (2025-05-11)

This is a maintenance release with no API/ABI changes.

* Add Meson build system support (recommended build method)
* Add deprecation warning to Autotools configure script

This release is fully compatible with the current [html5lib-tests](https://github.com/html5lib/html5lib-tests) (a9f4496)

## Gumbo 0.13.0 (2025-01-04)

* Fix incorrect doctype matching
* Retroactively bump so version (should have been done in 0.12.2)
* This release is fully compatible with the current [html5lib-tests](https://github.com/html5lib/html5lib-tests)

## Gumbo 0.12.3 (2024-12-16)

* Make Python bindings compatible with Python 3.13 (#2)

## Gumbo 0.12.2 (2024-11-11)

* Add support for `<search>` tag
* Add special handling for `<hr>` tag in `<select>` context per [spec](https://github.com/whatwg/html/pull/9124)
* Fix potential segfault in `gumbo_caret_diagnostic_to_string`
* Fix pretty bad integer overflow in the numeric entity parsing code
* Fix foreign context handling according to the [spec](https://github.com/whatwg/html/pull/6455)
* Adjust token foreign attributes per [spec](https://html.spec.whatwg.org/multipage/parsing.html#adjust-foreign-attributes)
* Remove special handling for `<isindex>` tag per [spec](https://github.com/whatwg/html/pull/1095)
* Remove special handling for `<menuitem>` tag per [spec](https://github.com/whatwg/html/pull/2319)
* Update autoconf version requirements to 2.72
* Drop GYP build system support

## Gumbo 0.12.1 (2023-09-24)

* Fix empty the end of the tag `<form>` ([#372@gumbo-parser](https://github.com/google/gumbo-parser/pull/372))
* Correctly handle text in `<form>` elements ([73c5df2@nokogiri](https://github.com/sparklemotion/nokogiri/commit/73c5df2171dc5f10f3c378436b34618af1c279b6))
* Fix tag name filtering that could result in XSS ([#375@gumbo-parser](https://github.com/google/gumbo-parser/pull/375))
* Return `visualc/include/strings.h` file (see https://github.com/mesonbuild/wrapdb/pull/1198)
* Reorganize docs a little

## Gumbo 0.12.0 (2023-07-18)

* Add support for `<dialog>` tag
* Fix `TAGSET_INCLUDES` macro to work properly with multiple bit flags ([17ab1d2@lua-gumbo](https://github.com/craigbarnes/lua-gumbo/commit/17ab1d2b4c4742da78c0c8b3329f61a95744c895))
* Avoid potential crash for zero length tags ([01359a9@nokogiri](https://github.com/sparklemotion/nokogiri/commit/01359a951e6c43bd975dfdb1fa80c01d32cc77d2))
* Bump shared library version

## Gumbo 0.11.0 (2023-07-16)

* Add support for `<picture>` tag
* Make `genperf.py` script compatible with Python 3
* Change maintainer to Grigory Kirillov (txgk@bk.ru)

## Gumbo 0.10.1 (2015-04-30)

Same as 0.10.0, but with the version number bumped because the last version-number commit to v0.9.4 makes GitHub think that v0.9.4 is the latest version and so it's not highlighted on the webpage.

## Gumbo 0.10.0 (2015-04-30)

* Full support for `<template>` tag (kevinhendricks, nostrademons).
* Some fixes for `<rtc>`/`<rt>` handling (kevinhendricks, vmg).
* All html5lib-trunk tests pass now! (kevinhendricks, vmg, nostrademons)
* Support for fragment parsing (vmg)
* A couple additional example programs (kevinhendricks)
* Performance improvements totaling an estimated 30-40% total improvement (vmg, nostrademons).

## Gumbo 0.9.4 (2015-04-30)

* Additional Visual Studio fixes (lowjoel, nostrademons)
* Fixed some unused variable warnings.
* Fix for glibtoolize vs. libtoolize build errors on Mac.
* Fixed `CDATA` end tag handling.

## Gumbo 0.9.3 (2015-02-17)

* Bugfix for `&AElig;` entities (rgrove)
* Fix `CDATA` handling; `CDATA` sections now generate a `GUMBO_NODE_CDATA` node rather
than plain text.
* Fix `get_title example` to handle whitespace nodes (gsnedders)
* Visual Studio compilation fixes (fishioon)
* Take the namespace into account when determining whether a node matches a
certain tag (aroben)
* Replace the varargs tag functions with a tagset bytevector, for a 20-30%
speedup in overall parse time (kevinhendricks, vmg)
* Add MacOS X support to Travis CI, and fix the deployment/DLL issues this
uncovered (nostrademons, kevinhendricks, vmg)

## Gumbo 0.9.2 (2014-09-21)

* Performance improvements: Ragel-based char ref decoder and DFA-based UTF8
decoder, totaling speedups of up to 300%.
* Added benchmarking program and some sample data.
* Fixed a compiler error under Visual Studio.
* Fix an error in the ctypes bindings that could lead to memory corruption in
the Python bindings.
* Fix duplicate attributes when parsing `<isindex>` tags.
* Don't leave semicolons behind when consuming entity references (rgrove)
* Internally rename some functions in preparation for an amalgamation file
(jdeng)
* Add proper cflags for gyp builds (skabbes)

## Gumbo 0.9.1 (2014-08-07)

* First version listed on PyPi.
* Autotools files excluded from GitHub and generated via autogen.sh. (endgame)
* Numerous compiler warnings fixed. (bnoordhuis, craigbarnes)
* Google security audit passed.
* Gyp support (tfarina)
* Naming convention for structs changed to avoid C reserved words.
* Fix several integer and buffer overflows (Maxime2)
* Some Visual Studio compiler support (bugparty)
* Python3 compatibility for the ctypes bindings.

## Gumbo 0.9.0 (2013-08-13)

* Initial release open-sourced by Google.
