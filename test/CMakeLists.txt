project(mlapi_test)

add_test(NAME sail_module COMMAND ${CMAKE_MAKE_PROGRAM} -C
        ${CMAKE_CURRENT_SOURCE_DIR}/sail)
set_property(TEST sail_module PROPERTY ENVIRONMENT
            "REDIS_MODULE_PATH=${CMAKE_BINARY_DIR}/src/libsaild.dylib;"
            "REDIS_SERVER_PATH=${REDIS_SERVER_PATH};"
            "VW_BUILD_PATH=${VW_BUILD_PATH};")
