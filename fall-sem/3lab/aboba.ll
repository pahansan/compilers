; ModuleID = 'gcd.ll'
source_filename = "gcd.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = constant [18 x i8] c"gcd(12, 15) = %d\0A\00", align 1

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define i32 @gcd(i32 %a, i32 %b) local_unnamed_addr #0 {
  %cmpResult.not3 = icmp eq i32 %b, 0
  br i1 %cmpResult.not3, label %return, label %loop

loop:                                             ; preds = %0, %loop
  %aPtr.05 = phi i32 [ %bPtr.04, %loop ], [ %a, %0 ]
  %bPtr.04 = phi i32 [ %aModB, %loop ], [ %b, %0 ]
  %aModB = srem i32 %aPtr.05, %bPtr.04
  %cmpResult.not = icmp eq i32 %aModB, 0
  br i1 %cmpResult.not, label %return, label %loop

return:                                           ; preds = %loop, %0
  %aPtr.0.lcssa = phi i32 [ %a, %0 ], [ %bPtr.04, %loop ]
  ret i32 %aPtr.0.lcssa
}

; Function Attrs: nofree nounwind
define noundef i32 @main() local_unnamed_addr #1 {
  br label %loop.i

loop.i:                                           ; preds = %loop.i, %0
  %aPtr.05.i = phi i32 [ %bPtr.04.i, %loop.i ], [ 12, %0 ]
  %bPtr.04.i = phi i32 [ %aModB.i, %loop.i ], [ 15, %0 ]
  %aModB.i = urem i32 %aPtr.05.i, %bPtr.04.i
  %cmpResult.not.i = icmp eq i32 %aModB.i, 0
  br i1 %cmpResult.not.i, label %gcd.exit, label %loop.i

gcd.exit:                                         ; preds = %loop.i
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, i32 %bPtr.04.i)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #1

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree nounwind }
