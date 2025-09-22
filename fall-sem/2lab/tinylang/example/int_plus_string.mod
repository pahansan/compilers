MODULE Gcd;

(* Пример вычисления НОД *)

VAR x: INTEGER;

PROCEDURE GCD(a, b: INTEGER) : INTEGER;
VAR t: INTEGER;
BEGIN
  IF b = 0 THEN
    RETURN a;
  END;
  WHILE b # 0 DO
    t := a MOD b;
    t := 1 + "string";
    a := b;
    b := t;
  END;
  RETURN a;
END GCD;

END Gcd.
