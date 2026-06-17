#include "allocator.h"

#include <cassert>
#include <iostream>

int main() {
    init_allocator();

    std::cout << "Block header size : " << sizeof(Block) << " bytes\n";
    std::cout << "Heap size         : " << HEAP_SIZE     << " bytes\n";

    // ── Initial state ─────────────────────────────────────────────────────────
    std::cout << "\n--- Initial state ---";
    print_heap();

    // ── Allocate three objects ────────────────────────────────────────────────
    int*    a = static_cast<int*>(my_alloc(15));    // rounds up to 16
    double* b = static_cast<double*>(my_alloc(32));
    char*   c = static_cast<char*>(my_alloc(100));  // rounds up to 104

    assert(a && b && c && "Allocation failed");

    *a = 42;
    *b = 3.14;
    *c = 'Z';

    std::cout << "--- After 3 allocations ---";
    print_heap();

    // ── Free middle block — coalesce should NOT fire (a is still live) ────────
    my_free(b);
    std::cout << "--- After freeing b (middle block) ---";
    print_heap();

    // ── Free first block — coalesces a+b into one ─────────────────────────────
    my_free(a);
    std::cout << "--- After freeing a (coalesce a+b expected) ---";
    print_heap();

    // ── Free last block — full coalesce back to one big block ─────────────────
    my_free(c);
    std::cout << "--- After freeing c (full coalesce expected) ---";
    print_heap();

    return 0;
}
