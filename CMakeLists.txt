cmake_minimum_required(VERSION 3.2)
project(sail)

include(ExternalProject)

option(CODE_COVERAGE "CODE_COVERAGE" OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

if(CODE_COVERAGE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -fno-common -std=c99 -g -O0 --coverage")
endif(CODE_COVERAGE)

include(cmake-modules/vowpal.cmake)
include(cmake-modules/redis.cmake)
include(cmake-modules/rmutil.cmake)
include(cmake-modules/protobuf.cmake)

INCLUDE_DIRECTORIES(src src/sail ${VW_INC_DIR} ${RMUTIL_INCLUDE_PATH})
link_directories(${VW_LIB_DIR} ${RMUTIL_LIB_PATH})

add_subdirectory(src)

include(CTest)
enable_testing()
add_subdirectory(test)

