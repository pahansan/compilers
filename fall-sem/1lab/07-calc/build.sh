#!/bin/bash

mkdir build
cd build
cmake ..
make

cd ..
cp -f build/src/calc tools/
