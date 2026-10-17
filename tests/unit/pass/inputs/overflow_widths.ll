; Signed i8/i16/i32/i64 arithmetic as the optimiser leaves it (clang -O0
; promotes char and short to int, so only IR reaches the narrow widths).
declare i32 @__VERIFIER_nondet_int()

define i8 @add8(i8 %a, i8 %b) {
entry:
  %r = add nsw i8 %a, %b
  ret i8 %r
}

define i16 @sub16(i16 %a, i16 %b) {
entry:
  %r = sub nsw i16 %a, %b
  ret i16 %r
}

define i32 @mul32(i32 %a, i32 %b) {
entry:
  %r = mul nsw i32 %a, %b
  ret i32 %r
}

define i64 @add64(i64 %a, i64 %b) {
entry:
  %r = add nsw i64 %a, %b
  ret i64 %r
}

define i32 @main() {
entry:
  %x = call i32 @__VERIFIER_nondet_int()
  %x8 = trunc i32 %x to i8
  %x16 = trunc i32 %x to i16
  %x64 = sext i32 %x to i64
  %r8 = call i8 @add8(i8 %x8, i8 1)
  %r16 = call i16 @sub16(i16 %x16, i16 1)
  %r32 = call i32 @mul32(i32 %x, i32 3)
  %r64 = call i64 @add64(i64 %x64, i64 1)
  ret i32 0
}
