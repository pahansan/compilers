#!/bin/sh

DIR="folly"

REGEXP="(L|[Uu]8?)?\'[^\']*\'"

for f in $(find $DIR -name "*.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Po "$REGEXP"
done