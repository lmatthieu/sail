# REDIS MODULES
ExternalProject_Add(
        redis_modules_sdk
        PREFIX redis_modules_sdk
        INSTALL_DIR ${CMAKE_BINARY_DIR}
        DOWNLOAD_COMMAND test -f rmutil/src/RedisModulesSDK/Makefile && echo
        "rmutil \
        aleady exists" || git clone https://github.com/lmatthieu/RedisModulesSDK.git redis_modules_sdk
        UPDATE_COMMAND git submodule foreach git pull origin master
        CONFIGURE_COMMAND make
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE 1
        EXCLUDE_FROM_ALL 1
)
set(RMUTIL_INCLUDE_PATH "${CMAKE_BINARY_DIR}/redis_modules_sdk/src")
set(RMUTIL_LIB_PATH "${CMAKE_BINARY_DIR}/redis_modules_sdk/src/redis_modules_sdk/rmutil")
