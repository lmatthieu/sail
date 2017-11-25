# REDIS
ExternalProject_Add(
        redis
        PREFIX redis
        INSTALL_DIR ${CMAKE_BINARY_DIR}
        DOWNLOAD_COMMAND test -d redis/src/redis/Makefile && echo "redis \
        directory already exists" || git clone --recursive https://github.com/antirez/redis.git
        UPDATE_COMMAND git submodule foreach git pull origin master
        CONFIGURE_COMMAND pwd && make
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE 1
        EXCLUDE_FROM_ALL 1
)
set(REDIS_SERVER_PATH "${CMAKE_BINARY_DIR}/redis/src/redis/src/redis-server")


