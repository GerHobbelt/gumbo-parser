## Gumbo - HTML parser library implemented in C99

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Ffxnoob%2Fgumbo-parser.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Ffxnoob%2Fgumbo-parser?ref=badge_shield)

Gumbo is an implementation of the HTML5 parsing algorithm implemented
as a pure C99 library with no outside dependencies. It's designed to serve
as a building block for other tools and libraries such as linters,
validators, templating languages, and refactoring and analysis tools.

This repository is a fork of [original GitHub repository](https://github.com/google/gumbo-parser)
which has been archived by the owner on Feb 16, 2023 and is no longer maintained.


Goals & features:

* Fully conformant with the [HTML5 spec](https://html.spec.whatwg.org/multipage).
* Robust and resilient to bad input.
* Simple API that can be easily wrapped by other languages.
* Support for source locations and pointers back to the original text.
* Support for fragment parsing.
* Relatively lightweight, with no outside dependencies.
* Passes all [html5lib tests](https://github.com/html5lib/html5lib-tests).
* Tested on over 2.5 billion pages from Google's index.
* Follows [Semantic Versioning](https://semver.org) scheme.

Non-goals:

* Execution speed.  Gumbo gains some of this by virtue of being written in
  C, but it is not an important consideration for the intended use-case, and
  was not a major design factor.
* Support for encodings other than UTF-8.  For the most part, client code
  can convert the input stream to UTF-8 text using another library before
  processing.
* Mutability.  Gumbo is intentionally designed to turn an HTML document into a
  parse tree, and free that parse tree all at once.  It's not designed to
  persistently store nodes or subtrees outside of the parse tree, or to perform
  arbitrary DOM mutations within your program.  If you need this functionality,
  we recommend translating the Gumbo parse tree into a mutable DOM
  representation more suited for the particular needs of your program before
  operating on it.

Building gumbo - Using vcpkg

You can download and install Gumbo using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    vcpkg install gumbo

The Gumbo port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

## Basic usage

```c
#include <gumbo.h>

int main(void) {
	GumboOutput* output = gumbo_parse("<h1>Hello, World!</h1>");
	// Do stuff with output->root
	gumbo_destroy_output(&kGumboDefaultOptions, output);
}
```

A variety of sample programs can be found in the [examples](https://codeberg.org/gumbo-parser/gumbo-parser/src/branch/master/examples) directory.
To build them, enable the `examples` build option during setup, e.g.:
```
meson setup builddir -Dexamples=true
```

## Learning more

* Building instructions: [doc/building.md](https://codeberg.org/gumbo-parser/gumbo-parser/src/branch/master/doc/building.md)
* Language bindings and other tools: [doc/bindings.md](https://codeberg.org/gumbo-parser/gumbo-parser/src/branch/master/doc/bindings.md)
* Contributing guide: [doc/contributing.md](https://codeberg.org/gumbo-parser/gumbo-parser/src/branch/master/doc/contributing.md)
* Debugging notes: [doc/debugging.md](https://codeberg.org/gumbo-parser/gumbo-parser/src/branch/master/doc/debugging.md)
* Maintenance directions: [doc/maintaining.md](https://codeberg.org/gumbo-parser/gumbo-parser/src/branch/master/doc/maintaining.md)
* Test suite guide: [doc/testing.md](https://codeberg.org/gumbo-parser/gumbo-parser/src/branch/master/doc/testing.md)

## Package availability

[![Packaging status](https://repology.org/badge/vertical-allrepos/gumbo-parser.svg?columns=4)](https://repology.org/project/gumbo-parser/versions)

A fork of the unmaintained original is available at [https://codeberg.org/gumbo-parser/gumbo-parser](https://codeberg.org/gumbo-parser/gumbo-parser).

## License

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Ffxnoob%2Fgumbo-parser.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Ffxnoob%2Fgumbo-parser?ref=badge_large)

