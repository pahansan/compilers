#!/bin/sh

DIR="folly"

INT="[0-9]+[Xxb]?([']?[0-9A-Fa-f]+)*[Zz]?[UuLl]?[UuLl]?[UuLl]?[Zz]?"
STRING="([RL]|RL|[Uu]8?R?)?\"[^\"]*\""
CHAR="(L|[Uu]8?)?\'[^\']*\'"
FLOAT1="([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT2="([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT3="([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[fFlL]?"
FLOAT4="0[Xx]([']?[0-9]*)*[.]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT5="0[Xx]([']?[0-9]+)+[.]([pPeE]-?([']?[0-9]+)+)?[fFlL]?"
FLOAT6="0[Xx]([']?[0-9]+)+([pPeE]-?([']?[0-9]+)+)+[fFlL]?"
IDENT="_[a-z][A-Za-z_0-9]*"

REGEXP="($INT|$STRING|$CHAR|$FLOAT1|$FLOAT2|$FLOAT3|$FLOAT4|$FLOAT5|$FLOAT6)$IDENT"

for f in $(find $DIR -name "*.cpp"); do
    echo "*** File $f"
    sed -e '/\/\*/ { :b; /\*\//!{N; bb}; s|/\*.*\*/|| }' -e 's|//.*||' "$f" |
    grep -v '^#' |
    grep -Eow "$REGEXP"
done