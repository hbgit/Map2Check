#!/bin/bash
set -e

JULIET_BASE="/workspace/test-comp2026/juliet/testcases"
SUPPORT="/workspace/test-comp2026/juliet/testcasesupport"
WASI_CLANG="/opt/wasi-sdk-33.0-x86_64-linux/bin/clang"
WASI_FLAGS="--target=wasm32-wasip1 -I${SUPPORT} -DINCLUDEMAIN"
OUTPUT="/workspace/test-comp2026/juliet/wasm_output"

mkdir -p "$OUTPUT"

echo "=== Compilando subset Juliet C -> WASM ==="

while IFS='|' read -r cwe_dir sub_dir base_name; do
    [[ -z "$cwe_dir" || "$cwe_dir" == \#* ]] && continue
    src="${JULIET_BASE}/${cwe_dir}/${sub_dir}/${base_name}.c"
    out="${OUTPUT}/${base_name}.wasm"
    
    if [ ! -f "$src" ]; then
        echo "SKIP $base_name (nao encontrado)"
        continue
    fi
    
    echo -n "Compilando: $base_name ... "
    ${WASI_CLANG} ${WASI_FLAGS} "$src" -o "$out" 2>/tmp/compile_err.txt
    EXIT=$?
    
    if [ $EXIT -eq 0 ] && [ -f "$out" ]; then
        echo "OK ($(ls -lh "$out" | awk '{print $5}'))"
    else
        echo "FAIL"
        head -2 /tmp/compile_err.txt 2>/dev/null
    fi
done << 'EOF'
CWE121_Stack_Based_Buffer_Overflow|s01|CWE121_Stack_Based_Buffer_Overflow__CWE129_fgets_01
CWE121_Stack_Based_Buffer_Overflow|s01|CWE121_Stack_Based_Buffer_Overflow__CWE129_large_01
CWE121_Stack_Based_Buffer_Overflow|s02|CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_memcpy_01
CWE122_Heap_Based_Buffer_Overflow|s05|CWE122_Heap_Based_Buffer_Overflow__CWE131_memcpy_01
CWE122_Heap_Based_Buffer_Overflow|s06|CWE122_Heap_Based_Buffer_Overflow__c_CWE129_fgets_01
CWE122_Heap_Based_Buffer_Overflow|s06|CWE122_Heap_Based_Buffer_Overflow__c_CWE129_large_01
CWE122_Heap_Based_Buffer_Overflow|s01|CWE122_Heap_Based_Buffer_Overflow__char_type_overrun_memcpy_01
CWE124_Buffer_Underwrite|s01|CWE124_Buffer_Underwrite__CWE839_fgets_01
CWE124_Buffer_Underwrite|s02|CWE124_Buffer_Underwrite__CWE839_negative_01
CWE124_Buffer_Underwrite|s01|CWE124_Buffer_Underwrite__char_alloca_cpy_01
CWE126_Buffer_Overread|s01|CWE126_Buffer_Overread__CWE129_fgets_01
CWE126_Buffer_Overread|s01|CWE126_Buffer_Overread__CWE129_large_01
CWE126_Buffer_Overread|s02|CWE126_Buffer_Overread__malloc_char_memcpy_01
CWE127_Buffer_Underread|s01|CWE127_Buffer_Underread__CWE839_fgets_01
CWE127_Buffer_Underread|s02|CWE127_Buffer_Underread__CWE839_negative_01
EOF

echo ""
echo "=== Criando casos manuais de buffer overflow ==="

# Caso 1: strcpy overflow
cat > /tmp/m2c_buf_strcpy.c << 'CEOF'
#include <string.h>
#include <assert.h>
int main() {
    char buf[10];
    strcpy(buf, "This string is way too long!");
    return 0;
}
CEOF

# Caso 2: memcpy overflow
cat > /tmp/m2c_buf_memcpy.c << 'CEOF'
#include <string.h>
#include <assert.h>
int main() {
    char dst[10];
    char src[100];
    memset(src, 'A', 100);
    memcpy(dst, src, 50);
    return 0;
}
CEOF

# Caso 3: array out-of-bounds
cat > /tmp/m2c_buf_oob.c << 'CEOF'
int main() {
    int arr[10];
    for (int i = 0; i <= 15; i++) {
        arr[i] = i;
    }
    return 0;
}
CEOF

for src in /tmp/m2c_buf_strcpy.c /tmp/m2c_buf_memcpy.c /tmp/m2c_buf_oob.c; do
    base=$(basename "$src" .c)
    out="${OUTPUT}/${base}.wasm"
    echo -n "Compilando: $base ... "
    ${WASI_CLANG} --target=wasm32-wasip1 "$src" -o "$out" 2>&1
    if [ -f "$out" ]; then
        echo "OK ($(ls -lh "$out" | awk '{print $5}'))"
    else
        echo "FAIL"
    fi
done

echo ""
echo "Total: $(ls ${OUTPUT}/*.wasm 2>/dev/null | wc -l) módulos WASM"
