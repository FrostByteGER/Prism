#!/bin/bash

rm -rf ./Distribution/*.so

if [ -n "$1" ]; then
    CONAN_PROFILE="--profile:build=$1"
else
    conan profile detect
fi


conan install . --build missing $CONAN_PROFILE --settings=build_type=Debug
conan install . --build missing $CONAN_PROFILE --settings=build_type=Release

