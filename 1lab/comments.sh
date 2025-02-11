#!/bin/sh

DIR="folly"

for f in $(find $DIR -name "*.cpp"); do
    echo "*** File $f"
    grep -Pzo '/\*[\s\S]*?\*/' $f
    echo ''
    grep -Eo "//.*" $f
done
