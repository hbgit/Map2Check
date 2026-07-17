#!/bin/bash
# Teste completo do pipeline Map2Check-WASM com versões atualizadas
# WABT 1.0.41 + wasi-sdk-33.0

set -e

echo "============================================================"
echo "Teste Pipeline Map2Check-WASM (Versões Atualizadas)"
echo "============================================================"
echo "WABT: $(wasm2c --version)"
echo "wasi-sdk: $(/opt/wasi-sdk-33.0-x86_64-linux/bin/clang --version | head -1)"
echo ""

WASM_RT_INCLUDE="/opt/wabt-1.0.41/include"

# Teste 1: Programa C simples com stdio
echo "=== Teste 1: C com stdio → WASM → LLVM IR ==="
cat > /tmp/test1.c << 'EOF'
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    int result = add(x, y);
    printf("Result: %d\n", result);
    return result;
}
EOF

/opt/wasi-sdk-33.0-x86_64-linux/bin/clang --target=wasm32-wasip1 \
    /tmp/test1.c -o /tmp/test1.wasm
wasm2c /tmp/test1.wasm -o /tmp/test1_wasm2c.c
/usr/bin/clang-16 -S -emit-llvm \
    -I${WASM_RT_INCLUDE} \
    -I/tmp \
    /tmp/test1_wasm2c.c -o /tmp/test1_wasm2c.ll

echo "✅ Teste 1 passou"
echo ""

# Teste 2: Programa com malloc/free (memory safety)
echo "=== Teste 2: C com malloc/free → WASM → LLVM IR ==="
cat > /tmp/test2.c << 'EOF'
#include <stdlib.h>

int main() {
    int* arr = (int*)malloc(10 * sizeof(int));
    if (!arr) return -1;
    
    for (int i = 0; i < 10; i++) {
        arr[i] = i * 2;
    }
    
    int result = arr[5];
    free(arr);
    return result;
}
EOF

/opt/wasi-sdk-33.0-x86_64-linux/bin/clang --target=wasm32-wasip1 \
    /tmp/test2.c -o /tmp/test2.wasm
wasm2c /tmp/test2.wasm -o /tmp/test2_wasm2c.c
/usr/bin/clang-16 -S -emit-llvm \
    -I${WASM_RT_INCLUDE} \
    -I/tmp \
    /tmp/test2_wasm2c.c -o /tmp/test2_wasm2c.ll

echo "✅ Teste 2 passou"
echo ""

# Teste 3: Buffer overflow intencional
echo "=== Teste 3: Buffer Overflow → WASM → LLVM IR ==="
cat > /tmp/test3.c << 'EOF'
#include <string.h>

int main() {
    char buffer[10];
    strcpy(buffer, "This is a very long string that overflows the buffer");
    return 0;
}
EOF

/opt/wasi-sdk-33.0-x86_64-linux/bin/clang --target=wasm32-wasip1 \
    /tmp/test3.c -o /tmp/test3.wasm
wasm2c /tmp/test3.wasm -o /tmp/test3_wasm2c.c
/usr/bin/clang-16 -S -emit-llvm \
    -I${WASM_RT_INCLUDE} \
    -I/tmp \
    /tmp/test3_wasm2c.c -o /tmp/test3_wasm2c.ll

echo "✅ Teste 3 passou"
echo ""

echo "============================================================"
echo "Todos os testes passaram! Pipeline WASM está funcional."
echo "============================================================"
