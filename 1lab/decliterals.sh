#!/bin/sh

DIR="folly"

REGEXP="[0]|[1-9][0-9]*(['][0-9]+)*[Zz]?[UuLl]?[UuLl]?[UuLl]?[Zz]?"

for f in $(find $DIR -name "*.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Eo "$REGEXP"
done