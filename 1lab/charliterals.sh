#!/bin/sh

DIR="."

REGEXP="(L|[Uu]8?)?\'[^\']*\'"

for f in $(find $DIR -name "test.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Po "$REGEXP"
done