should work on most linux distros, was tested on debian 12 and arch

will compile on windows too if sdl2 libraries are installed, was tested with mingw64

if your cmake is older than 3.25, lower the requirement in cmakelists, don't know what could go wrong

install libsdl2-dev through package manager

then run these commands from the folder:

mkdir build
cd build
cmake ..
make

after this, copy the textures folder into the build folder

run the executable
