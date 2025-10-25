target triple = "x86_64-unknown-linux-gnu"

@.str = constant [18 x i8] c"gcd(12, 15) = %d\0A\00", align 1

define i32 @gcd(i32 %a, i32 %b) {
  %aPtr = alloca i32, align 4
  %bPtr = alloca i32, align 4
  %tPtr = alloca i32, align 4
  store i32 %a, ptr %aPtr, align 4
  store i32 %b, ptr %bPtr, align 4
  br label %loopHeader

loopHeader:
  %bToCompare = load i32, ptr %bPtr, align 4
  %cmpResult = icmp ne i32 %bToCompare, 0
  br i1 %cmpResult, label %loop, label %return

loop:
  %aInLoop = load i32, ptr %aPtr, align 4
  %bInLoop = load i32, ptr %bPtr, align 4
  %aModB = srem i32 %aInLoop, %bInLoop
  store i32 %aModB, ptr %tPtr, align 4
  store i32 %bInLoop, ptr %aPtr, align 4
  store i32 %aModB, ptr %bPtr, align 4
  br label %loopHeader

return:
  %aToReturn = load i32, ptr %aPtr, align 4
  ret i32 %aToReturn
}

define i32 @main() {
  %gcdResult = call i32 @gcd(i32 12, i32 15)
  call i32 @printf(ptr @.str, i32 %gcdResult)
  ret i32 0
}

declare i32 @printf(ptr, ...)
