# Custom Memory Allocator in C++

A fixed-size heap allocator written from scratch in C++17, implementing
`malloc` / `free` semantics on top of a static byte array.

Built to deepen understanding of memory layout, pointer arithmetic, alignment,
and fragmentation — concepts central to low-latency systems programming.

---

## How It Works

### Memory Layout

```
[ heap array: 1 MB ]
  ┌────────────┬─────────────┬────────────┬─────────────┬──────────── ...
  │ Block hdr  │  user data  │ Block hdr  │  user data  │
  └────────────┴─────────────┴────────────┴─────────────┴──────────── ...
```

Every allocation is prefixed by a `Block` header containing:
- `size`  — usable bytes in this region
- `free`  — whether the region is available
- `next`  — pointer to the adjacent block (intrusive linked list)

`my_alloc` returns a pointer **past** the header; `my_free` subtracts one
`Block`-sized step to recover it.

### Alignment

All requested sizes are rounded up to the nearest 8-byte boundary before
scanning:

```cpp
(size + 7) & ~7
```

This ensures every returned pointer is correctly aligned for any fundamental
type (int, double, pointer).

### First-Fit Allocation

The allocator walks the free-list from the beginning and returns the **first**
block large enough to satisfy the request.  No best-fit search — intentionally
simple and O(n).

### Block Splitting

When a free block is significantly larger than the request, it is split:

```
Before split:
  [ Block | ........ 256 bytes free ........ ]

After my_alloc(16):
  [ Block | 16 bytes | Block | 232 bytes free ]
```

Splitting only happens when the remainder can hold a full `Block` header plus
at least 8 bytes of usable space; otherwise the entire block is returned
(internal fragmentation is accepted).

### Coalescing (Free-List Compaction)

After every `my_free`, a forward pass merges adjacent free blocks:

```
Before coalesce:
  [ Block free | Block free | Block used ]

After coalesce:
  [ Block free (merged) | Block used ]
```

A single forward pass is sufficient because coalescing runs on every free, so
at most two consecutive free blocks can ever exist.

---

## Build & Run

**Requirements:** g++ with C++17 support (GCC 7+ / Clang 5+)

```bash
git clone https://github.com/<your-username>/memory-allocator.git
cd memory-allocator
make
./allocator
```

Expected output (addresses will differ):

```
Block header size : 24 bytes
Heap size         : 1048576 bytes

--- Initial state ---
========== Heap State ==========
[0]  addr: 0x...  size: 1048552  free: yes
=================================

--- After 3 allocations ---
========== Heap State ==========
[0]  addr: 0x...  size: 16    free: no
[1]  addr: 0x...  size: 32    free: no
[2]  addr: 0x...  size: 104   free: no
[3]  addr: 0x...  size: ...   free: yes
=================================
...
--- After freeing c (full coalesce expected) ---
[0]  addr: 0x...  size: 1048552  free: yes
```

---

## Design Decisions & Trade-offs

| Decision | Rationale |
|---|---|
| Static byte array | Avoids `sbrk`/`mmap`; portable and easy to reason about |
| Intrusive linked list | No secondary data structure; headers live inside the heap itself |
| First-fit | Simple; good average-case fragmentation for short-lived allocations |
| Coalesce on every free | Prevents fragmentation buildup at the cost of O(n) on each free |
| 8-byte alignment | Sufficient for all primitive types on x86-64 |

---

## Known Limitations / Future Work

- **Thread safety** — no synchronisation; a real allocator would use per-thread
  arenas or lock-free structures (e.g. a Michael-Scott queue of free blocks)
- **Best-fit / segregated free-lists** — would reduce fragmentation at the cost
  of metadata overhead
- **Boundary tags** — adding a footer to each block enables O(1) backwards
  coalescing (currently O(n) forward-only)
- **Benchmarks** — `my_alloc` vs `malloc` latency comparison under mixed
  workloads

---

## Files

```
.
├── allocator.h      # Block struct + public API declarations
├── allocator.cpp    # Implementation (split, coalesce, alloc, free)
├── main.cpp         # Demo: allocate, free, observe coalescing
└── Makefile
```

---

## Concepts Demonstrated

- Manual memory management and pointer arithmetic
- Struct layout and alignment on x86-64
- Intrusive linked list design
- Fragmentation: internal (alignment padding) and external (free-list holes)
- Coalescing strategies
