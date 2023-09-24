## Testing

To run the unit tests, you'll need to have [googletest][] downloaded and
unzipped.  The googletest maintainers recommend against using
`make install`; instead, symlink the root googletest directory to 'gtest'
inside gumbo's root directory, and then `make check`:

```
unzip gtest-1.6.0.zip
```
```
cd gumbo-*
```
```
ln -s ../gtest-1.6.0 gtest
```
```
make check
```

Gumbo's `make check` has code to automatically configure & build gtest and
then link in the library.

Debian and Fedora users can install libgtest with:

```
apt-get install libgtest-dev  # Debian/Ubuntu
```
```
yum install gtest-devel       # CentOS/Fedora
```

Note for Ubuntu users: libgtest-dev package only install source files.
You have to make libraries yourself using cmake:

```
sudo apt-get install cmake
```
```
cd /usr/src/gtest
```
```
sudo cmake CMakeLists.txt
```
```
sudo make
```
```
sudo cp *.a /usr/lib
```

The configure script will detect the presence of the library and use that
instead.

Note that you need to have super user privileges to execute these commands.
On most distros, you can prefix the commands above with `sudo` to execute
them as the super user.

Debian installs usually don't have `sudo` installed (Ubuntu however does.)
Switch users first with `su -`, then run `apt-get`.

[googletest]: https://github.com/google/googletest
