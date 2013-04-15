CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT (atlantis C)

ENABLE_TESTING()

ADD_SUBDIRECTORY (cutest)
ADD_SUBDIRECTORY (storage)
ADD_SUBDIRECTORY (crypto)
ADD_SUBDIRECTORY (tests)

IF (MSVC)
	find_package (MSVC MODULE)
    # Force to always compile with W4
    MSVC_SET_WARNING_LEVEL(4)
ELSEIF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unknown-pragmas -Wpointer-arith -Wno-char-subscripts -Wno-long-long")
    SET(CMAKE_C_FLAGS "-g")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
ELSE(MSVC)
    MESSAGE(STATUS "Unknown compiler ${CMAKE_C_COMPILER_ID}")
ENDIF(MSVC)

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

SET( ATLANTIS_SRC atlantis.c atlantis.h spells.h skills.h items.h unit.h unit.c region.h region.c faction.h faction.c keywords.h settings.h )
SET( RTL_SRC rtl.c rtl.h )
ADD_EXECUTABLE(atlantis main.c ${ATLANTIS_SRC} ${RTL_SRC} )
TARGET_LINK_LIBRARIES (atlantis ${CRYPTO_LIBRARIES} ${STORAGE_LIBRARIES} )

INCLUDE_DIRECTORIES (${CUTEST_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${STORAGE_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CRYPTO_INCLUDE_DIR})


ADD_EXECUTABLE(atlantis_tests atlantis.test.c ${ATLANTIS_SRC} ${RTL_SRC} )
TARGET_LINK_LIBRARIES (atlantis_tests ${STORAGE_LIBRARIES} ${CUTEST_LIBRARIES} ${CRYPTO_LIBRARIES} )
ADD_TEST (atlantis atlantis_tests)

ADD_EXECUTABLE(rtl_tests rtl.test.c ${RTL_SRC})
TARGET_LINK_LIBRARIES (rtl_tests ${CUTEST_LIBRARIES} )
ADD_TEST (rtl rtl_tests)

ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS rtl_tests atlantis_tests storage_tests CuTestTest atlantis
                  COMMENT "Running Tests...")

IF (MSVC)
    MSVC_CRT_SECURE_NO_WARNINGS (${PROJECT_NAME} atlantis atlantis_tests rtl_tests)
ENDIF (MSVC)
