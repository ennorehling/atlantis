cmake_minimum_required(VERSION 2.6)
project (atlantis C)

FIND_PACKAGE(Lua)
FIND_PACKAGE(ToLua++)

ENABLE_TESTING()

INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR}/include)
INCLUDE_DIRECTORIES (${STORAGE_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${QUICKLIST_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CRYPTO_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CUTEST_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CJSON_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

IF (TOLUA++_FOUND)
MACRO(TOLUA_BINDING PKGFILE FILES)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${PKGFILE}.c
    DEPENDS ${FILES} ${PKGFILE}
    COMMAND ${TOLUA++_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/${PKGFILE}.c ${PKGFILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
ENDMACRO(TOLUA_BINDING)
                        
TOLUA_BINDING(atlantis.pkg atlantis.h)
ENDIF(TOLUA++_FOUND)

ADD_EXECUTABLE (test_rtl rtl.c rtl.h rtl.test.c)
TARGET_LINK_LIBRARIES (test_rtl ${CUTEST_LIBRARIES})
ADD_TEST (rtl test_rtl)

FILE (GLOB LIB_HDR rtl.h rtl.c bool.h atlantis.h report.h parser.h game.h
	combat.h battle.h building.h ship.h unit.h region.h faction.h keywords.h
    settings.h skills.h items.h)
FILE (GLOB LIB_SRC atlantis.c report.c parser.c game.c combat.c battle.c
	building.c ship.c unit.c region.c faction.c settings.c)

set (ATLANTIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "atlantis headers")
set (ATLANTIS_LIBRARIES atlantis ${STORAGE_LIBRARIES} ${QUICKLIST_LIBRARIES} ${CRYPTO_LIBRARIES} ${CJSON_LIBRARIES} CACHE INTERNAL "atlantis libraries")

IF (TOLUA++_FOUND)
set (BIND_SRC atlantis.pkg.c)
add_library (atlantis-lua SHARED ${BIND_SRC} ${LIB_SRC} ${LIB_HDR})

target_link_libraries (atlantis-lua ${TOLUA++_LIBRARIES} ${LUA_LIBRARIES} ${ATLANTIS_LIBRARIES})
set_target_properties(atlantis-lua PROPERTIES OUTPUT_NAME atlantis)
set_target_properties(atlantis-lua PROPERTIES PREFIX "")
ENDIF(TOLUA++_FOUND)

add_library (atlantis STATIC ${BIND_SRC} ${LIB_SRC} ${LIB_HDR})

ADD_EXECUTABLE (test_atlantis atlantis.test.c)
TARGET_LINK_LIBRARIES (test_atlantis ${CUTEST_LIBRARIES} ${ATLANTIS_LIBRARIES})
ADD_TEST (atlantis test_atlantis)

IF (MSVC)
    FIND_PACKAGE (MSVC MODULE)
    MSVC_CRT_SECURE_NO_WARNINGS (test_atlantis test_rtl atlantis)
ENDIF (MSVC)
