MODULE ExampleMod;

TYPE MyArray = ARRAY [10 + 4] OF INTEGER;
VAR v: MyArray;

PROCEDURE Main;
BEGIN
  v[1] := 1;
END Main;

END ExampleMod.
