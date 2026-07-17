#!/bin/bash
# Teste manual do WasmLifter
# Compila um programa C simples para WASM e depois lifta para LLVM IR

set -e

echo "=== Teste do WasmLifter ==="

# 1. Criar programa C de teste (sem stdio para evitar WASI)
cat > /tmp/wasm_lifter_test.c << 'EOF'
int add(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    int result = add(x, y);
    return result;
}
EOF

echo "1. Compilando C → WASM (wasm32-unknown-unknown, sem WASI)..."
/usr/bin/clang-16 --target=wasm32-unknown-unknown \
    -nostdlib -Wl,--no-entry \
    /tmp/wasm_lifter_test.c -o /tmp/wasm_lifter_test.wasm

echo "2. WASM gerado:"
ls -la /tmp/wasm_lifter_test.wasm
file /tmp/wasm_lifter_test.wasm

echo ""
echo "3. Lifting WASM → C (wasm2c)..."
wasm2c /tmp/wasm_lifter_test.wasm -o /tmp/wasm_lifter_test_wasm2c.c

echo "4. C gerado:"
ls -la /tmp/wasm_lifter_test_wasm2c.c
head -30 /tmp/wasm_lifter_test_wasm2c.c

echo ""
echo "5. Compilando C → LLVM IR (clang-16)..."
/usr/bin/clang-16 -S -emit-llvm \
    -I/usr/include \
    /tmp/wasm_lifter_test_wasm2c.c \
    -o /tmp/wasm_lifter_test_wasm2c.ll

echo "6. LLVM IR gerado:"
ls -la /tmp/wasm_lifter_test_wasm2c.ll
echo ""
echo "Funções no IR:"
grep "define" /tmp/wasm_lifter_test_wasm2c.ll | head -15

echo ""
echo "=== Teste concluído com sucesso! ==="
