#!/bin/sh

DIR="."

for f in $(find $DIR -name "test.cpp"); do
    echo "*** File $f"
    grep -Pzo '/\*[\s\S]*?\*/' $f
    echo ''
    grep -Eo "//.*" $f
done
