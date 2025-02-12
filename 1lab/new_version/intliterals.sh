#!/bin/sh

DIR="."

INT="\b[0-9]+[Xxb]?([']?[0-9A-Fa-f]+)*[Zz]?[UuLl]?[UuLl]?[UuLl]?[Zz]?\b"

FLOAT1="([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT2="([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT3="([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[fFlL]?"
FLOAT4="0[Xx]([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT5="0[Xx]([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT6="0[Xx]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[fFlL]?"

FLOAT="$FLOAT1|$FLOAT2|$FLOAT3|$FLOAT4|$FLOAT5|$FLOAT6"

for f in $(find $DIR -name "test.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Ev "$FLOAT" |
    grep -Eo "$INT"
done