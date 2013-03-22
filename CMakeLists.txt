cmake_minimum_required(VERSION 2.6)
project (atlantis C)

enable_testing()

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unknown-pragmas -Wpointer-arith -Wno-char-subscripts -Wno-long-long")
#    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -DHAVE__BOOL")
ELSE(CMAKE_COMPILER_IS_GNUCC)
    MESSAGE(STATUS "Unknown compiler ${CMAKE_C_COMPILER_ID}")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)
CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)
CHECK_SYMBOL_EXISTS (stricmp "string.h" HAVE_STRICMP)
CHECK_SYMBOL_EXISTS (_stricmp "string.h" HAVE__STRICMP)
CONFIGURE_FILE (
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_BINARY_DIR}/include/config.h)
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR}/include)

add_executable(atlantis atlantis1.c rtl.h)
