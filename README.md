[![Build Status](https://travis-ci.org/lmatthieu/sail.svg?branch=master)](https://travis-ci.org/lmatthieu/sail/)
[![Coverage Status](https://coveralls.io/repos/github/lmatthieu/sail/badge.png?branch=master)](https://coveralls.io/github/lmatthieu/sail?branch=master)

# README 

## Introduction

SAIL: A shallow/simple ai learning environment
This package is used for experimenting low energy and resources algorithms.

Actually, the SAIL module expose a simple API for experiments with vowpal 
wabbit models.

## Building

This package uses the CMake tool for building. CMake download automatically the 
dependencies:
- Vowpal wabbit
- Protocol buffers
- Redis: used only for unit testing
- RedisModulesSDK

For the Vowpal wabbit package, boost libraries must be available.


## Testing

You can create and use a conda environment for staring the unit tests.

``` bash
conda create -n sail python=2.7
pip install rmtest
make test ARGS="-V"
```

## Debugging

In the *build* directory.

- Build redis with noopt
```bash 
(cd redis/src/redis/ && make clean noopt)
```

- Start redis server
```bash
./redis/src/redis/src/redis-server --loadmodule src/libsail.so
```

- Get redis server process id
```bash
./redis/src/redis/src/redis-cli info | grep process_id
```

- Attach GDB
```bash
gdb ./redis/src/redis/src/redis-cli $process_id
```

## Online & Reinforcement learning (Work in progress)

The commands SAIL.REI.* try to implement generic online learning algorithms. 
This module is under construction and is not complete.

You can manipulate two kind of objects :
- Models: stored as vowpal wabbit models
- Examples: examples are values tracked during the interactions with the algorithm

