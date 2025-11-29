#!/bin/sh

clang++ -S -emit-llvm ./main.cpp
./build/JIT main.ll
