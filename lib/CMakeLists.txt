cmake_minimum_required(VERSION 2.6)
project (atlantis C)

ENABLE_TESTING()

INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR}/include)
INCLUDE_DIRECTORIES (${STORAGE_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CRYPTO_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CUTEST_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CJSON_INCLUDE_DIR})

ADD_EXECUTABLE (test_rtl rtl.c rtl.h rtl.test.c)
TARGET_LINK_LIBRARIES (test_rtl ${CUTEST_LIBRARIES})
ADD_TEST (rtl test_rtl)

FILE (GLOB LIB_HDR rtl.h rtl.c bool.h atlantis.h json.h building.h ship.h unit.h region.h faction.h keywords.h settings.h skills.h)
FILE (GLOB LIB_SRC atlantis.c json.c building.c ship.c unit.c region.c faction.c)

ADD_LIBRARY (atlantis ${LIB_SRC} ${LIB_HDR})

ADD_EXECUTABLE (test_atlantis atlantis.test.c)
TARGET_LINK_LIBRARIES (test_atlantis atlantis ${CUTEST_LIBRARIES} ${STORAGE_LIBRARIES} ${CRYPTO_LIBRARIES} ${CJSON_LIBRARIES})
ADD_TEST (atlantis test_atlantis)

set (ATLANTIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "atlantis headers")
set (ATLANTIS_LIBRARIES atlantis ${STORAGE_LIBRARIES} ${CRYPTO_LIBRARIES} ${CJSON_LIBRARIES} CACHE INTERNAL "atlantis libraries")

IF (MSVC)
    FIND_PACKAGE (MSVC MODULE)
    MSVC_CRT_SECURE_NO_WARNINGS (test_atlantis test_rtl atlantis)
ENDIF (MSVC)
