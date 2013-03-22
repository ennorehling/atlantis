CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT (atlantis C)

ENABLE_TESTING()

ADD_SUBDIRECTORY (cutest)

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unknown-pragmas -Wpointer-arith -Wno-char-subscripts -Wno-long-long")
    SET(CMAKE_C_FLAGS "-g")
#    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -DHAVE__BOOL")
ELSE(CMAKE_COMPILER_IS_GNUCC)
    MESSAGE(STATUS "Unknown compiler ${CMAKE_C_COMPILER_ID}")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)
CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)
CHECK_SYMBOL_EXISTS (stricmp "string.h" HAVE_STRICMP)
CHECK_SYMBOL_EXISTS (strlwr "string.h" HAVE_STRLWR)
CHECK_SYMBOL_EXISTS (_stricmp "string.h" HAVE__STRICMP)
CONFIGURE_FILE (
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_BINARY_DIR}/include/config.h)
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR}/include)

ADD_EXECUTABLE(atlantis rtl.c rtl.h atlantis1.c)

INCLUDE_DIRECTORIES (${CUTEST_INCLUDE_DIR})

ADD_EXECUTABLE(rtl_tests rtl.c rtl.h rtl.test.c)
TARGET_LINK_LIBRARIES (rtl_tests cutest)

ADD_TEST (rtl rtl_tests)
ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS rtl_tests CuTestTest
                  COMMENT "Running Tests...")
