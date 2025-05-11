## Gumbo - A pure-C HTML5 parser

Gumbo is an implementation of the [HTML5 parsing algorithm][] implemented
as a pure C99 library with no outside dependencies. It's designed to serve
as a building block for other tools and libraries such as linters,
validators, templating languages, and refactoring and analysis tools.
This repository is a fork of [original GitHub repository][] which has been
archived by the owner on Feb 16, 2023 and is no longer maintained.

Goals & features:

* Fully conformant with the [HTML5 spec][].
* Robust and resilient to bad input.
* Simple API that can be easily wrapped by other languages.
* Support for source locations and pointers back to the original text.
* Support for fragment parsing.
* Relatively lightweight, with no outside dependencies.
* Passes all [html5lib tests][], including the template tag.
* Tested on over 2.5 billion pages from Google's index.

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

Wishlist (aka "We couldn't get these into the original release, but are
hoping to add them soon"):

* Full-featured error reporting.
* Additional performance improvements.
* DOM wrapper library/libraries (possibly within other language bindings)
* Query libraries, to extract information from parse trees using CSS or XPATH.

## Basic usage

```C
#include <gumbo.h>

int main() {
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

## A note on API/ABI compatibility

We'll make a best effort to preserve API compatibility between releases.
The initial release is a 0.9 (beta) release to solicit comments from early
adopters, but if no major problems are found with the API, a 1.0 release
will follow shortly, and the API of that should be considered stable.  If
changes are necessary, we follow [semantic versioning][].

We make no such guarantees about the ABI, and it's very likely that
subsequent versions may require a recompile of client code.  For this
reason, we recommend NOT using Gumbo data structures throughout a program,
and instead limiting them to a translation layer that picks out whatever
data is needed from the parse tree and then converts that to persistent
data structures more appropriate for the application.  The API is
structured to encourage this use, with a single delete function for the
whole parse tree, and is not designed with mutation in mind.

[HTML5 parsing algorithm]: http://www.whatwg.org/specs/web-apps/current-work/multipage/#auto-toc-12
[original GitHub repository]: https://github.com/google/gumbo-parser
[HTML5 spec]: http://www.whatwg.org/specs/web-apps/current-work/multipage/
[html5lib tests]: https://github.com/html5lib/html5lib-tests
[semantic versioning]: http://semver.org/
