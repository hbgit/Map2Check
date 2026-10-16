; The shape wasm2c gives a lifted module: an allocator returning an i32
; linear-memory offset, and accesses through a GEP into the memory's data.
; The allocator bodies are stand-ins, so the module links on its own.
define i32 @w2c_m_dlmalloc(ptr %instance, i32 %size) {
entry:
  ret i32 64
}

define void @w2c_m_dlfree(ptr %instance, i32 %offset) {
entry:
  ret void
}

define i32 @main() {
entry:
  %inst = alloca [64 x i8]
  %mem = alloca [1024 x i8]
  %off = call i32 @w2c_m_dlmalloc(ptr %inst, i32 16)
  %idx = zext i32 %off to i64
  %addr = getelementptr inbounds [1024 x i8], ptr %mem, i64 0, i64 %idx
  store i32 5, ptr %addr
  %v = load i32, ptr %addr
  call void @w2c_m_dlfree(ptr %inst, i32 %off)
  ret i32 %v
}
