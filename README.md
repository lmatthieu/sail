[![Build Status](https://travis-ci.org/lmatthieu/sail.svg?branch=master)](https://travis-ci.org/lmatthieu/sail/)

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
- Redis: used only for unit testing
- RedisModulesSSK

For the Vowpal wabbit package, boost libraries must be available.


## Testing

You can create and use a conda environment for staring the unit tests.

``` bash
conda create -n sail python=2.7
pip install rmtest
make test ARGS="-V"
```


