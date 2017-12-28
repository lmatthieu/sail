# Vowpal wabbit dependency

message(STATUS "Configuring Vowpal Wabbit")
SET(PNAME vowpal_wabbit)
ExternalProject_Add(
        ${PNAME}
        PREFIX ${PNAME}
        INSTALL_DIR ${CMAKE_BINARY_DIR}
        USES_TERMINAL_DOWNLOAD 1
        DOWNLOAD_COMMAND PNAME=${PNAME} ${CMAKE_SOURCE_DIR}/cmake-modules/cmd.sh ${PNAME}_download
        UPDATE_DISCONNECTED 1
        UPDATE_COMMAND git pull origin master
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE 1
)
set(VW_INC_DIR ${CMAKE_BINARY_DIR}/${PNAME}/src/vowpal_wabbit/)
set(VW_LIB_DIR ${CMAKE_BINARY_DIR}/${PNAME}/src/vowpal_wabbit/vowpalwabbit)
set(VW_BUILD_PATH ${CMAKE_BINARY_DIR}/${PNAME}/src/vowpal_wabbit)
