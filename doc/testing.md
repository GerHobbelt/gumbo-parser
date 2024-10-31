## Testing

Besides `autotools` you will need to have `cmake` installed.

Here's a summary of commands required to run tests:

```
cd gumbo-parser
./autogen.sh
./configure
rm -rf gtest
git clone --depth 1 https://github.com/google/googletest.git -b v1.15.2 gtest
make check
```

Gumbo's `make check` automatically builds gtest and then links in the library.

If you have gtest and its development files installed via package manager system
wide, then you can omit `git clone` command and run `make check` straightaway
not having directory `gtest` in project root.

Note for Ubuntu users: `libgtest-dev` package only install source files.
You have to build libraries yourself:

```
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```

## html5lib-tests

```
git submodule update --init
```
```
python3 python/gumbo/html5lib_adapter_test.py
```
