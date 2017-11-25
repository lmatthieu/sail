# Vowpal wabbit dependency

message(STATUS "Configuring Vowpal Wabbit")
ExternalProject_Add(
        vowpal_wabbit
        PREFIX vowpal_wabbit
        INSTALL_DIR ${CMAKE_BINARY_DIR}
        DOWNLOAD_COMMAND test -d vowpal_wabbit/src/vowpal_wabbit/vowpalwabbit && echo "vowpal_wabbit already exists" || git clone --depth 1 https://github.com/JohnLangford/vowpal_wabbit.git
        #UPDATE_COMMAND git pull origin master
        CONFIGURE_COMMAND pwd && make
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE 1
)
set(VW_INC_DIR ${CMAKE_BINARY_DIR}/vowpal_wabbit/src/vowpal_wabbit/)
set(VW_LIB_DIR ${CMAKE_BINARY_DIR}/vowpal_wabbit/src/vowpal_wabbit/vowpalwabbit)
set(VW_BUILD_PATH ${CMAKE_BINARY_DIR}/vowpal_wabbit/src/vowpal_wabbit)
