MODULE Gcd;

(* Пример вычисления НОД *)

VAR x: INTEGER;

PROCEDURE GCD(a, b: INTEGER);
VAR t: INTEGER;
BEGIN
  IF b = 0 THEN
    RETURN a;
  END;
  WHILE b # 0 DO
    t := a MOD b;
    a := b;
    b := t;
  END;
  RETURN 10;
END GCD;

END Gcd.
