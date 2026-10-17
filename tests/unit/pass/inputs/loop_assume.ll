; A loop in a program that already calls a nounwind __VERIFIER_assume (as a
; frontend, or clang on a definition without -fexceptions, can leave it).
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assume(i32) #0

define i32 @main() {
entry:
  %n = call i32 @__VERIFIER_nondet_int()
  %c = icmp slt i32 %n, 10
  %cz = zext i1 %c to i32
  call void @__VERIFIER_assume(i32 %cz) #0
  br label %loop

loop:
  %i = phi i32 [ 0, %entry ], [ %inc, %body ]
  %cmp = icmp slt i32 %i, %n
  br i1 %cmp, label %body, label %exit

body:
  %inc = add nsw i32 %i, 1
  br label %loop

exit:
  ret i32 %i
}

attributes #0 = { nounwind }
