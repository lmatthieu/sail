set(PROTOBUF_IMPORT_DIRS
        ${CMAKE_SOURCE_DIR}/src/codegen
        ${CMAKE_SOURCE_DIR}/third_party/protobuf/src)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS vowpal.proto)
protobuf_generate_python(PROTO_PY vowpal.proto)

MESSAGE(STATUS "Protocol buffers files generated: ${PROTO_SRCS} ${PROTO_HDRS}")

sail_generate_service_cpp(SRV_SRCS SRV_HDRS vowpal.proto)
MESSAGE(STATUS "Sail files generated: ${SRV_SRCS} ${SRV_HDRS}")

add_library(sailapi SHARED
        ${SRV_SRCS} ${SRV_HDRS}
        ${PROTO_SRCS} ${PROTO_HDRS}
        vowpalmodelimpl.h wrapper.h vowpalmodelimpl.cpp
        redismodule.c vowpal_service_impl.h vowpal_service_impl.cpp)

target_link_libraries(sailapi
        sailcodegen
        ${Protobuf_LIBRARIES} ${Protobuf_PROTOC_LIBRARY}
        vw boost_program_options z allreduce rmutil)
add_dependencies(sailapi redis_codegen)
