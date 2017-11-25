cmake_minimum_required(VERSION 3.5)
project(sail)

include(ExternalProject)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++11 -g")

include(cmake-modules/vowpal.cmake)
include(cmake-modules/redis.cmake)
include(cmake-modules/rmutil.cmake)

INCLUDE_DIRECTORIES(src ${VW_INC_DIR} ${RMUTIL_INCLUDE_PATH})
link_directories(${VW_LIB_DIR} ${RMUTIL_LIB_PATH})

add_subdirectory(src)

include(CTest)
enable_testing()
add_subdirectory(test)

