#pragma once

#include <cstddef>
#include <cstdint>

// ── Constants ────────────────────────────────────────────────────────────────
static constexpr size_t HEAP_SIZE = 1024 * 1024;   // 1 MB static heap
static constexpr size_t ALIGNMENT = 8;              // 8-byte alignment

// ── Block header (sits immediately before every allocation) ──────────────────
struct Block {
    size_t  size;   // usable bytes after this header
    bool    free;   // is this block available?
    Block*  next;   // next block in the intrusive free-list
};

// ── Public API ───────────────────────────────────────────────────────────────
void  init_allocator();         // must be called once before any alloc/free
void* my_alloc(size_t size);    // returns aligned pointer, nullptr on failure
void  my_free(void* ptr);       // marks block free and coalesces neighbours
void  print_heap();             // debug: walk and print every block
