include_directories(${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

set(PROTOBUF_IMPORT_DIRS ${CMAKE_SOURCE_DIR}/third_party/protobuf/src)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS options.proto)

add_library(sailcodegen ${PROTO_SRCS} ${PROTO_HDRS})

ADD_EXECUTABLE(redis_codegen redis_codegen.cpp)
TARGET_LINK_LIBRARIES(redis_codegen
        sailcodegen
        ${PROTOBUF_LIBRARIES} ${Protobuf_PROTOC_LIBRARIES})