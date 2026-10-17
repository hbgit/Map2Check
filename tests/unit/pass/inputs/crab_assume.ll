; What Clam leaves in the bitcode: invariants as calls to a nounwind
; verifier.assume(i1) (clam.py --crab-opt=add-invariants).
declare i32 @__VERIFIER_nondet_int()
declare void @verifier.assume(i1) #0

define i32 @main() {
entry:
  %x = call i32 @__VERIFIER_nondet_int()
  %c = icmp sgt i32 %x, 0
  call void @verifier.assume(i1 %c) #0
  %d = icmp slt i32 %x, 100
  call void @verifier.assume(i1 %d) #0
  ret i32 0
}

attributes #0 = { nounwind }
