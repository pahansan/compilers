#!/bin/sh

DIR="folly"

REGEXP="[0-9]+[Xxb]?([']?[0-9A-Fa-f]+)*[Zz]?[UuLl]?[UuLl]?[UuLl]?[Zz]?"

for f in $(find $DIR -name "*.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Eo "$REGEXP"
done