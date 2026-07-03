/**
 * Copyright (C) 2026 Map2Check tool
 * SPDX-License-Identifier: GPL-2.0
 *
 * WasmRuntimeStubs — malloc-based wasm-rt runtime for KLEE compatibility
 *
 * The default WABT wasm-rt implementation uses mmap/munmap for memory
 * management, which KLEE models poorly. These stubs replace them with
 * calloc/free, making the lifted WASM bitcode analyzable by KLEE.
 *
 * This is the MVP (Approach B): bounds-check only, full memory tracking
 * with AllocationLog integration deferred to post-SBSeg work.
 */

#include "wasm-rt.h"

#include <stdlib.h>
#include <string.h>

/** Allocate WASM linear memory using calloc (KLEE-friendly). */
void wasm_rt_allocate_memory(wasm_rt_memory_t* mem,
                              uint64_t initial_pages,
                              uint64_t max_pages,
                              bool is64,
                              uint32_t page_size) {
    uint64_t byte_length = (uint64_t)page_size * initial_pages;
    mem->data = calloc(1, byte_length);
    mem->size = byte_length;
    mem->pages = initial_pages;
    mem->max_pages = max_pages;
    /* mem->is64 = is64; // field may not exist in all WABT versions */
}

/** Free WASM linear memory. */
void wasm_rt_free_memory(wasm_rt_memory_t* mem) {
    free(mem->data);
    mem->data = NULL;
    mem->size = 0;
}

/** Allocate funcref table (KLEE-friendly). */
void wasm_rt_allocate_funcref_table(wasm_rt_funcref_table_t* table,
                                     uint32_t elements,
                                     uint32_t max_elements) {
    table->data = calloc(elements, sizeof(void*));
    table->size = elements;
}

/** Free funcref table. */
void wasm_rt_free_funcref_table(wasm_rt_funcref_table_t* table) {
    free(table->data);
    table->data = NULL;
}

/* WASI stubs — minimal implementations for KLEE compatibility */

struct w2c_wasi__snapshot__preview1;

void w2c_wasi__snapshot__preview1_proc_exit(
    struct w2c_wasi__snapshot__preview1* instance, uint32_t code) {
    (void)instance; (void)code;
}

/** KLEE-friendly trap: route to map2check_error for witness generation. */
static int g_wasm_rt_initialized = 1;  // Always initialized for KLEE

bool wasm_rt_is_initialized(void) {
    return g_wasm_rt_initialized;
}

void wasm_rt_init(void) {
    g_wasm_rt_initialized = 1;
}

void wasm_rt_free(void) {
    g_wasm_rt_initialized = 0;
}

void wasm_rt_trap(wasm_rt_trap_t code) {
    (void)code;
    // KLEE compatible: use external call to signal error
    // map2check_error is provided by Map2Check runtime
    extern void map2check_error(void);
    map2check_error();
}
