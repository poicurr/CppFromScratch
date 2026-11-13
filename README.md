# CppFromScratch

[日本語版 README](README.ja.md)

Handwritten C++20 implementations of data structures, algorithms, and low-level utilities for practice and reference. The goal is to understand the implementation, not to replace production standard libraries.

## Requirements

- CMake 3.24+, Ninja, and a C++20 compiler.
- **Debug:** Clang or GCC with ASan + UBSan support. On Windows, use Clang with the corresponding sanitizer runtimes. MSVC alone does not support UBSan in this template.
- Catch2 v3 is fetched from source on sanitized builds. First configure requires internet access unless a local Catch2 checkout is supplied.
- **TSan:** Linux only; separate from ASan/UBSan.

## Build and test

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

```sh
cmake --preset release
cmake --build --preset release
ctest --preset release
```

## Quality checks

```sh
cmake --build --preset debug --target check
```

The `check` target builds tests, runs CTest, and runs the installed optional tools:

| Target | Tool | Purpose |
| --- | --- | --- |
| `format-check` | clang-format | Verify formatting without rewriting files |
| `clang-tidy` | clang-tidy | Static analysis |
| `cppcheck` | Cppcheck | Additional static checks |

On Windows with Clang and ASan, `cmake/AsanRuntime.cmake` copies the ASan DLL beside every project executable during normal builds, even when the executable itself is up to date. Linux needs no DLL staging; `-fsanitize` links the runtime normally. The Debug preset uses the non-debug MSVC DLL CRT (`/MD`) to avoid the observed Debug CRT/ASan failure, while retaining Debug symbols and sanitizer instrumentation.

Debug enables ASan and UBSan for both the project and Catch2 compiled from source. This prevents mismatched MSVC STL ASan annotations when linking tests. Release has no sanitizers. TSan is a separate Linux build:

```sh
cmake --preset tsan
cmake --build --preset tsan
ctest --preset tsan
```

## Exercise list

- [x] `LinkedList<T>` — Doubly linked list
- [x] `Stack<T>` — LIFO
- [x] `Queue<T>` — FIFO
- [x] `DynamicArray<T>` — Growable array
- [ ] `RingBuffer<T>` — Circular indexing
- [ ] `UniquePtr<T>` — Exclusive ownership and RAII
- [ ] `BinarySearchTree<T>` — Binary search tree
- [ ] `BinaryHeap<T>` — Heap invariants
- [ ] `HashMap<K, V>` — Hash collisions and rehashing
- [ ] `BitSet` — Packed bits
- [ ] `ObjectPool<T>` — Object reuse
- [ ] `ArenaAllocator` — Memory alignment and allocation
- [ ] `AVLTree<T>` — Balanced tree rotations
- [ ] `RedBlackTree<T>` — Red-black balancing rules
- [ ] `LRUCache<K, V>` — Hash map + doubly linked list
- [ ] `SparseSet<T>` — Sparse/dense indexing
- [ ] `SlotMap<T>` — Generational handles
- [ ] `SharedPtr<T>` — Shared ownership
- [ ] `Function<R(Args...)>` — Type erasure
- [ ] `LockFreeQueue<T>` — Atomics and memory ordering

## Add an exercise

Implement a header under `include/CppFromScratch/`, add test cases under `tests/`, and update `tests/CMakeLists.txt`.

No CI or old presets are included.

## License

MIT ([LICENSE](LICENSE)).
