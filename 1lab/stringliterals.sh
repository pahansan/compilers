#!/bin/sh

DIR="folly"

REGEXP="([RL]|RL|[Uu]8?R?)?\"[^\"]*\""

for f in $(find $DIR -name "*.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Po "$REGEXP"
done