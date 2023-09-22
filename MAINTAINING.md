## Bumping version

1. Change version numbers in `configure.ac` and `setup.py` files
3. Update `gumbo_version_info` in `configure.ac` according to [this](https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html)
4. Add changes log to `CHANGES.md` file

## Adding support for an HTML tag

1. Install [gpref](https://www.gnu.org/software/gperf)
2. Add tag name to `src/tag.in` file
3. Run `./autogen.sh`
4. Run `./configure`
5. Run `make gentags`
6. Increment tags count in `GumboTagEnumTest` test in `tests/tokenizer.cc` file
