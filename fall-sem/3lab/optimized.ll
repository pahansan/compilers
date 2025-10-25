target triple = "x86_64-unknown-linux-gnu"

@.str = constant [17 x i8] c"gcd(12, 15) = 3\0A\00", align 1

define i32 @main() {
  call i32 @printf(ptr @.str)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...)
