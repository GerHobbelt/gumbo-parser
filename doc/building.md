## Building instructions

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
