# Upsidedown Chess Bot

## Building
This assumes you are using some kind of uinx commandline.

Create the build directory and `cd` into it
```sh
mkdir build
cd build
```

Run cmake and use the `CMakeLists` file, from the directory tree source
```sh
cmake ..
```
This only really needs to happen once. This creates a makefile to build the code
with.

Run `make` to compile and link and then execute the executable.
```sh
make
./bot
```
