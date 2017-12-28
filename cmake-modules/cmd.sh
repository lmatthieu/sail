#!/usr/bin/env bash

if [ "$1" == "vowpal_wabbit_download" ]; then
    if [ -d ${CMAKE_BINARY_DIR}/${PNAME}/src/${PNAME}/vowpalwabbit ]; then
        echo "${PNAME} already exists"
    else
        git clone --depth 1 https://github.com/JohnLangford/vowpal_wabbit.git
    fi
fi

if [ "$1" == "protobuf_download" ]; then
    if [ -d ${CMAKE_BINARY_DIR}/${PNAME}/src/${PNAME}/cmake ]; then
        echo "${PNAME} already exists"
    else
        git clone --recursive --branch v3.5.1.1 --depth 1 https://github.com/google/protobuf.git
    fi
fi

if [ "$1" == "protobuf_configure" ]; then
    if [ -f ${CMAKE_BINARY_DIR}/${PNAME}/src/${PNAME}/configure ]; then
        echo "${PNAME} already exists"
    else
        ./autogen.sh
    fi
    if [ -f ${CMAKE_BINARY_DIR}/${PNAME}/src/${PNAME}/Makefile ]; then
        echo "${PNAME} already exists"
    else
        ./configure --prefix ${CMAKE_BINARY_DIR}/${PNAME}
    fi
fi

