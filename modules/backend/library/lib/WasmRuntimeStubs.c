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

#include <stdint.h>
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
    mem->data_end = mem->data + byte_length;
    mem->page_size = page_size;
    mem->size = byte_length;
    mem->pages = initial_pages;
    mem->max_pages = max_pages;
    /* mem->is64 = is64; // field may not exist in all WABT versions */
}

/** Grow WASM linear memory by `pages` using realloc (KLEE-friendly).
 *  Returns the previous page count, or UINT32_MAX on failure,
 *  matching the wasm-rt.h contract. Called by lifted code whenever
 *  the module executes `memory.grow` (e.g. sbrk/malloc in WASI libc). */
uint64_t wasm_rt_grow_memory(wasm_rt_memory_t* mem, uint64_t pages) {
    uint64_t old_pages = mem->pages;
    uint64_t new_pages = old_pages + pages;
    if (new_pages < old_pages || new_pages > mem->max_pages) {
        return (uint64_t)UINT32_MAX;
    }
    if (pages == 0) {
        return old_pages;
    }
    uint32_t page_size = mem->page_size ? mem->page_size : 65536;
    uint64_t old_size = mem->size;
    uint64_t new_size = new_pages * (uint64_t)page_size;
    uint8_t* new_data = (uint8_t*)realloc(mem->data, new_size);
    if (!new_data) {
        return (uint64_t)UINT32_MAX;
    }
    memset(new_data + old_size, 0, new_size - old_size);
    mem->data = new_data;
    mem->data_end = new_data + new_size;
    mem->pages = new_pages;
    mem->size = new_size;
    return old_pages;
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
