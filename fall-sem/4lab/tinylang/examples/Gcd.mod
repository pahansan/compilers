MODULE Gcd;

PROCEDURE GCD(a, b: INTEGER) : INTEGER;
VAR t: INTEGER;
VAR B: BOOLEAN;
VAR G: BOOLEAN;

BEGIN
  IF b = 0 THEN
    RETURN a;
  END;
  WHILE b # 0 DO
    t := a MOD b;
    a := b;
    b := t;
    B := b # 0;
    G := B # B;
  END;
  RETURN a;
END GCD;

END Gcd.
