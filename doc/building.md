## Building instructions

### Using meson

Setup the build directory:
```
meson setup build
```

Compile gumbo:
```
meson compile -C build
```

(Optional) Run the testsuite:
```
meson test -C build
```

Install gumbo:
```
meson install -C build
```

### Using autotools

To build and install the library, issue the standard UNIX incantation from
the root of the distribution:

```
./autogen.sh
```
```
./configure
```
```
make
```
```
sudo make install
```

### Using the gumbo library

Gumbo comes with full pkg-config support, so you can use the pkg-config to
print the flags needed to link your program against it:

```
pkg-config --cflags gumbo         # print compiler flags
```
```
pkg-config --libs gumbo           # print linker flags
```
```
pkg-config --cflags --libs gumbo  # print both
```

For example:

```
gcc my_program.c `pkg-config --cflags --libs gumbo`
```
