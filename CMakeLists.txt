CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT (atlantis C)

ENABLE_TESTING()

ADD_SUBDIRECTORY (cutest)
ADD_SUBDIRECTORY (tests)

if(MSVC)
  # Force to always compile with W4
  IF(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  ELSE()
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  ENDIF()
ELSEIF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unknown-pragmas -Wpointer-arith -Wno-char-subscripts -Wno-long-long")
    SET(CMAKE_C_FLAGS "-g")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
ELSE()
IF (MSVC)
	find_package (MSVC MODULE)
ELSE(MSVC)
    MESSAGE(STATUS "Unknown compiler ${CMAKE_C_COMPILER_ID}")
ENDIF(MSVC)
ENDIF()


FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)
CHECK_INCLUDE_FILES (stdbool.h HAVE_STDBOOL_H)
CHECK_SYMBOL_EXISTS (_Bool "stdbool.h" HAVE__BOOL)
CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)
CHECK_SYMBOL_EXISTS (_strlwr "string.h" HAVE__STRLWR)
CHECK_SYMBOL_EXISTS (_strcmpl "string.h" HAVE__STRCMPL)
CHECK_SYMBOL_EXISTS (_stricmp "string.h" HAVE__STRICMP)
CHECK_SYMBOL_EXISTS (_memicmp "string.h" HAVE__MEMICMP)
CHECK_SYMBOL_EXISTS (strcmpl "string.h" HAVE_STRCMPL)
CHECK_SYMBOL_EXISTS (stricmp "string.h" HAVE_STRICMP)
CHECK_SYMBOL_EXISTS (memicmp "string.h" HAVE_MEMICMP)
CHECK_SYMBOL_EXISTS (strlwr "string.h" HAVE_STRLWR)
CHECK_SYMBOL_EXISTS (mkdir "sys/stat.h" HAVE_SYS_STAT_MKDIR)
CHECK_SYMBOL_EXISTS (mkdir "direct.h" HAVE_DIRECT_MKDIR)
CHECK_SYMBOL_EXISTS (_mkdir "direct.h" HAVE_DIRECT__MKDIR)
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
                  DEPENDS rtl_tests CuTestTest atlantis
                  COMMENT "Running Tests...")

IF (MSVC)
	MSVC_CRT_SECURE_NO_WARNINGS (${PROJECT_NAME} atlantis rtl_tests)
ENDIF(MSVC)
