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

## Bitboards

Resources :
- https://essays.jwatzman.org/essays/chess-move-generation-with-magic-bitboards.html
- http://pradu.us/old/Nov27_2008/Buzz/research/magic/Bitboards.pdf
