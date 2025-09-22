MODULE Gcd;

(* Пример вычисления НОД *)

VAR x: INTEGER;

PROCEDURE GCD(a, b: INTEGER) : INTEGER;
VAR x: INTEGER;
BEGIN
  IF b = 0 THEN
    RETURN a;
  END;
  WHILE b # 0 DO
    x := a MOD b;
    a := b;
    b := x;
  END;
  RETURN a;
END GCD;

END Gcd.
