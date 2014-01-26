#!/bin/sh
mkdir codeblocks
cd codeblocks
cmake -D CMAKE_MODULE_PATH=$(pwd)/../cmake/Modules -G "CodeBlocks - Unix Makefiles" ..
cd ..
