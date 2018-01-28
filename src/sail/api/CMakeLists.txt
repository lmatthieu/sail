
set(PROTOBUF_IMPORT_DIRS ${CMAKE_SOURCE_DIR}/src/codegen ${CMAKE_SOURCE_DIR}/third_party/protobuf/src)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS vowpal.proto)
protobuf_generate_python(PROTO_PY vowpal.proto)
SAIL_GENERATE_SERVICE_CPP(SRV_SRCS SRV_HDRS vowpal.proto)

add_library(sailapi STATIC ${PROTO_SRCS} ${PROTO_HDRS} ${SRV_SRCS} vowpalmodelimpl.h wrapper.h vowpalmodelimpl.cpp)

target_link_libraries(sailapi ${Protobuf_LIBRARIES})
add_dependencies(sailapi redis_codegen)
