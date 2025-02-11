#!/bin/sh

DIR="folly"

REGEXP="0[Xx][0-9a-fA-F]+(['][0-9a-fA-F]+)*[Zz]?[UuLl]?[UuLl]?[UuLl]?[Zz]?"

for f in $(find $DIR -name "*.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Eo "$REGEXP"
done