#include "allocator.h"

#include <iostream>

// ── Internal state ────────────────────────────────────────────────────────────
static uint8_t heap[HEAP_SIZE];
static Block*  freelist = nullptr;

// ── Helpers ───────────────────────────────────────────────────────────────────

// Round size up to the nearest ALIGNMENT boundary.
static size_t align_up(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

// Split `block` into two: first has `size` usable bytes, remainder becomes a
// new free block.  Only called when the remainder is large enough to hold a
// Block header plus at least ALIGNMENT usable bytes.
static void split_block(Block* block, size_t size) {
    Block* new_block = reinterpret_cast<Block*>(
        reinterpret_cast<uint8_t*>(block + 1) + size
    );

    new_block->size  = block->size - size - sizeof(Block);
    new_block->free  = true;
    new_block->next  = block->next;

    block->next = new_block;
    block->size = size;
}

// Walk the free-list and merge adjacent free blocks into one.
// Only a single forward pass is needed because we always coalesce after every
// free, so at most two adjacent free blocks can exist at a time.
static void coalesce() {
    Block* curr = freelist;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(Block) + curr->next->size;
            curr->next  = curr->next->next;
            // do NOT advance: the merged block may now be adjacent to the next
        } else {
            curr = curr->next;
        }
    }
}

// ── Public API ────────────────────────────────────────────────────────────────

void init_allocator() {
    freelist        = reinterpret_cast<Block*>(heap);
    freelist->size  = HEAP_SIZE - sizeof(Block);
    freelist->free  = true;
    freelist->next  = nullptr;
}

void* my_alloc(size_t size) {
    if (size == 0) return nullptr;

    size = align_up(size);

    Block* curr = freelist;
    while (curr) {
        if (curr->free && curr->size >= size) {
            // Only split if the leftover can fit a header + a minimum payload.
            if (curr->size >= size + sizeof(Block) + ALIGNMENT) {
                split_block(curr, size);
            }
            curr->free = false;
            return static_cast<void*>(curr + 1);   // pointer past the header
        }
        curr = curr->next;
    }
    return nullptr;   // out of memory
}

void my_free(void* ptr) {
    if (!ptr) return;

    Block* block = static_cast<Block*>(ptr) - 1;
    block->free  = true;

    coalesce();
}

void print_heap() {
    Block* curr  = freelist;
    int    index = 0;

    std::cout << "\n========== Heap State ==========\n";
    while (curr) {
        std::cout << "[" << index++ << "]"
                  << "  addr: "  << static_cast<void*>(curr)
                  << "  size: "  << curr->size
                  << "  free: "  << (curr->free ? "yes" : "no")
                  << '\n';
        curr = curr->next;
    }
    std::cout << "=================================\n\n";
}
