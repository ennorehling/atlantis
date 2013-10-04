MACH=$(uname -m)
mkdir -p build-${MACH}-Debug
cd build-${MACH}-Debug
cmake -D CMAKE_MODULE_PATH=$(pwd)/../cmake/Modules ..
