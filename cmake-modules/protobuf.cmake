##
## Protocol Buffers download and building script
##

set(PNAME protobuf)
ExternalProject_Add(
        ${PNAME}
        PREFIX ${PNAME}
        INSTALL_DIR ${CMAKE_BINARY_DIR}
        DOWNLOAD_COMMAND ${CMAKE_SOURCE_DIR}/cmake-modules/cmd.sh ${PNAME}_download
        UPDATE_COMMAND git submodule foreach git pull origin master
        UPDATE_DISCONNECTED 1
        CONFIGURE_COMMAND CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR} PNAME=${PNAME} ${CMAKE_SOURCE_DIR}/cmake-modules/cmd.sh ${PNAME}_configure
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        BUILD_IN_SOURCE 1
        EXCLUDE_FROM_ALL 1
)


SET(Protobuf_SRC_ROOT_FOLDER ${CMAKE_BINARY_DIR}/${PNAME}/src/${PNAME})
SET(PROTOBUF_PROTOC_EXECUTABLE ${CMAKE_BINARY_DIR}/${PNAME}/bin/protoc)
SET(Protobuf_LIBRARY ${CMAKE_BINARY_DIR}/${PNAME}/lib/libprotobuf.a)
SET(Protobuf_PROTOC_LIBRARY ${CMAKE_BINARY_DIR}/${PNAME}/lib/libprotoc.a)
SET(Protobuf_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${PNAME}/src/${PNAME}/src)

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
            -I ${CMAKE_BINARY_DIR}/protobuf/include
            -I ${CMAKE_CURRENT_SOURCE_DIR} ${ABS_FILE}
      DEPENDS ${ABS_FILE} ${PROTOBUF_PROTOC_EXECUTABLE}
      COMMENT "Running SAIL service compiler on ${FILE}"
      VERBATIM)
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

