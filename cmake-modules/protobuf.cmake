##
## Protocol Buffers download and building script
##

set(PNAME protobuf)

option(protobuf_BUILD_SHARED_LIBS "" OFF)
option(protobuf_BUILD_TESTS "" OFF)
option(protobuf_BUILD_EXAMPLES "" OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/protobuf/cmake)

set(Protobuf_SRC_ROOT_FOLDER ${PROJECT_SOURCE_DIR}/third_party/protobuf)
set(PROTOBUF_PROTOC_EXECUTABLE ${PROJECT_BINARY_DIR}/third_party/protobuf/cmake/protoc)
set(Protobuf_LIBRARY ${CMAKE_BINARY_DIR}/third_party/protobuf/cmake/libprotobufd.a)
set(Protobuf_PROTOC_LIBRARY ${CMAKE_BINARY_DIR}/third_party/protobuf/cmake/libprotocd.a)
set(PROTOBUF_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/third_party/protobuf/src)

# Include the cmake default protobuf file
include(FindProtobuf)

# cmake function inspired from https://github.com/wastl/cmarmotta/blob/master/cmake/FindGRPC.cmake
function(SAIL_GENERATE_SERVICE_CPP SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: SAIL_GENERATE_SERVICE_CPP() need proto files in arguments")
    return()
  endif()

  set(${SRCS})
  set(${HDRS})

  foreach(FILE ${ARGN})
    get_filename_component(ABS_FILE ${FILE} ABSOLUTE)
    get_filename_component(FILE_STRIP ${FILE} NAME_WE)

    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FILE_STRIP}_service.cpp")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FILE_STRIP}_service.h")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FILE_STRIP}_service.cpp"
             "${CMAKE_CURRENT_BINARY_DIR}/${FILE_STRIP}_service.h"
      COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS  --plugin=protoc-gen-sail=${CMAKE_BINARY_DIR}/src/codegen/redis_codegen
            --sail_out=${CMAKE_CURRENT_BINARY_DIR}
            -I ${CMAKE_SOURCE_DIR}/src/codegen
            -I ${CMAKE_SOURCE_DIR}/third_party/protobuf/src
            -I ${CMAKE_CURRENT_SOURCE_DIR} ${ABS_FILE}
      DEPENDS ${ABS_FILE} ${PROTOBUF_PROTOC_EXECUTABLE}
      COMMENT "Running SAIL service compiler on ${FILE}"
      VERBATIM)
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

