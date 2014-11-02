CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
if (WIN32)
FILE(TO_CMAKE_PATH "${CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH )
FILE(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH )
endif(WIN32)
PROJECT (atlantis C)

ENABLE_TESTING()

ADD_SUBDIRECTORY (cutest)

IF (MSVC)
    FIND_PACKAGE (MSVC MODULE)
    # Force to always compile with W4
    MSVC_SET_WARNING_LEVEL(4)
ELSEIF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
ELSE(MSVC)
    MESSAGE(STATUS "Unknown compiler ${CMAKE_C_COMPILER_ID}")
ENDIF(MSVC)

SET (CMAKE_POSITION_INDEPENDENT_CODE True)

ADD_SUBDIRECTORY (storage)
ADD_SUBDIRECTORY (quicklist)
ADD_SUBDIRECTORY (cJSON)
ADD_SUBDIRECTORY (crypto)

find_package (Lua)
IF(MSVC)
find_package (ToLua REQUIRED)
ELSE()
find_package (ToLua++)
ENDIF()

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)
CHECK_INCLUDE_FILES (stdbool.h HAVE_STDBOOL_H)
CHECK_SYMBOL_EXISTS (_Bool "stdbool.h" HAVE__BOOL)
CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)
CHECK_SYMBOL_EXISTS (strcasecmp "string.h" HAVE_STRCASECMP)
CHECK_SYMBOL_EXISTS (strncasecmp "string.h" HAVE_STRNCASECMP)
CHECK_SYMBOL_EXISTS (_strlwr "string.h" HAVE__STRLWR)
CHECK_SYMBOL_EXISTS (_strcmpl "string.h" HAVE__STRCMPL)
CHECK_SYMBOL_EXISTS (_strdup "string.h" HAVE__STRDUP)
CHECK_SYMBOL_EXISTS (_stricmp "string.h" HAVE__STRICMP)
CHECK_SYMBOL_EXISTS (_memicmp "string.h" HAVE__MEMICMP)
CHECK_SYMBOL_EXISTS (strcmpl "string.h" HAVE_STRCMPL)
CHECK_SYMBOL_EXISTS (strdup "string.h" HAVE_STRDUP)
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
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CUTEST_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${QUICKLIST_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${STORAGE_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CRYPTO_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CJSON_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${ATLANTIS_INCLUDE_DIR})

ADD_EXECUTABLE (test_rtl rtl.c rtl.h rtl.test.c)
TARGET_LINK_LIBRARIES (test_rtl ${CUTEST_LIBRARIES})
ADD_TEST (rtl test_rtl)

FILE (GLOB LIB_HDR rtl.h rtl.c bool.h atlantis.h report.h parser.h game.h
	combat.h battle.h building.h ship.h unit.h region.h faction.h keywords.h
    settings.h skills.h items.h json.h)
FILE (GLOB LIB_SRC atlantis.c report.c parser.c game.c combat.c battle.c
	building.c ship.c unit.c region.c faction.c settings.c json.c)

set (ATLANTIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "atlantis headers")
set (ATLANTIS_LIBRARIES atlantislib ${STORAGE_LIBRARIES} ${QUICKLIST_LIBRARIES} ${CRYPTO_LIBRARIES} ${CJSON_LIBRARIES} CACHE INTERNAL "atlantis libraries")
add_library (atlantislib STATIC ${BIND_SRC} ${LIB_SRC} ${LIB_HDR})
set_target_properties (atlantislib PROPERTIES POSITION_INDEPENDENT_CODE True)

ADD_EXECUTABLE (test_atlantis atlantis.test.c)
TARGET_LINK_LIBRARIES (test_atlantis ${CUTEST_LIBRARIES} ${ATLANTIS_LIBRARIES})
ADD_TEST (atlantis test_atlantis)
set_target_properties (test_atlantis PROPERTIES POSITION_INDEPENDENT_CODE False)

IF (LUA_FOUND)
INCLUDE_DIRECTORIES (${LUA_INCLUDE_DIR})
IF (TOLUA_FOUND)
INCLUDE_DIRECTORIES (${TOLUA_INCLUDE_DIR})
ENDIF(TOLUA_FOUND)

IF (TOLUA++_FOUND)
MACRO(TOLUA_BINDING PKGFILE FILES)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PKGFILE}.c
    DEPENDS ${FILES} ${PKGFILE}
    COMMAND ${TOLUA++_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_SOURCE_DIR}/${PKGFILE}.c ${PKGFILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
ENDMACRO(TOLUA_BINDING)
TOLUA_BINDING(bindings.pkg luabind.h atlantis.h faction.h region.h unit.h ship.h building.h game.h)
ENDIF()

SET(TOLUA_SRC
 tolua/lib/tolua_event.c
 tolua/lib/tolua_map.c
 tolua/lib/tolua_to.c
 tolua/lib/tolua_is.c  
 tolua/lib/tolua_push.c
)

set (BIND_SRC bindings.pkg.c luabind.c luabind.h)

if (NOT TOLUA_FOUND)
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/tolua/include)
set (BIND_SRC ${TOLUA_SRC} ${BIND_SRC})
endif ()

add_library (atlantisdll SHARED ${BIND_SRC})
target_link_libraries (atlantisdll ${LUA_LIBRARIES} storage) # ${ATLANTIS_LIBRARIES})
if (TOLUA_FOUND)
target_link_libraries (atlantisdll ${TOLUA_LIBRARY})
endif (TOLUA_FOUND)
set_target_properties(atlantisdll PROPERTIES POSITION_INDEPENDENT_CODE True)
set_target_properties(atlantisdll PROPERTIES OUTPUT_NAME atlantis)
set_target_properties(atlantisdll PROPERTIES PREFIX "")

ENDIF(LUA_FOUND)

ADD_EXECUTABLE (server server.c)
TARGET_LINK_LIBRARIES(server ${ATLANTIS_LIBRARIES})

ADD_EXECUTABLE (test_server server.test.c json.c json.h)
TARGET_LINK_LIBRARIES (test_server ${CUTEST_LIBRARIES} ${ATLANTIS_LIBRARIES})
ADD_TEST (server test_server)

ADD_EXECUTABLE (template template.c)
TARGET_LINK_LIBRARIES (template ${ATLANTIS_LIBRARIES})

IF (MSVC)
    MSVC_CRT_SECURE_NO_WARNINGS (server test_server)
    MSVC_CRT_SECURE_NO_WARNINGS (test_atlantis test_rtl atlantislib)
    MSVC_CRT_SECURE_NO_WARNINGS (template)
ENDIF (MSVC)

install(TARGETS server DESTINATION bin)
install(TARGETS atlantisdll DESTINATION lib)
install(DIRECTORY lua/ DESTINATION scripts FILES_MATCHING PATTERN "*.lua")
install(DIRECTORY conf/ DESTINATION etc FILES_MATCHING PATTERN "*.json")
