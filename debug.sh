#!/bin/bash
mkdir -p build
pushd build
    cmake -G "CodeLite - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
popd