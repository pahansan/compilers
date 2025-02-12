#!/bin/sh

DIR="."

REGEXP1="([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[bB]?[fFlL]?(16|32|64|128)?"
REGEXP2="([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[bB]?[fFlL]?(16|32|64|128)?"
REGEXP3="([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[bB]?[fFlL]?(16|32|64|128)?"
REGEXP4="0[Xx]([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[bB]?[fFlL]?(16|32|64|128)?"
REGEXP5="0[Xx]([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[bB]?[fFlL]?(16|32|64|128)?"
REGEXP6="0[Xx]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[bB]?[fFlL]?(16|32|64|128)?"

REGEXP="$REGEXP1|$REGEXP2|$REGEXP3|$REGEXP4|$REGEXP5|$REGEXP6"

for f in $(find $DIR -name "test.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Eow "$REGEXP"
done
