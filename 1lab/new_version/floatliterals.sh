#!/bin/sh

DIR="."

REGEXP1="([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
REGEXP2="([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
REGEXP3="([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[fFlL]?"
REGEXP4="0[Xx]([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
REGEXP5="0[Xx]([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
REGEXP6="0[Xx]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[fFlL]?"

REGEXP="$REGEXP1|$REGEXP2|$REGEXP3|$REGEXP4|$REGEXP5|$REGEXP6"

for f in $(find $DIR -name "test.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Eow "$REGEXP"
done
