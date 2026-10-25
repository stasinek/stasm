## Multithreading

### What is shared

Every option you declare (`x86`, `x87`, `mmx`, `sse1`, `avx`, ...) gives you a static exchange area, e.g. `__stasm_x86`, `__stasm_x87`. The `save,x87`, `save,mmx`, `save,sse` and `save,avx` options also use static save areas (`__stasm_fnsaved_stack`, `__stasm_mnsaved_stack`, `__stasm_fxsaved_stack`, ...).

All of them are **global**: one copy for the whole program, shared by every thread and every function. If two threads use `__stasm_x86` at the same time, they overwrite each other's data.

A single-threaded program needs no locking at all.

### `__stasm()` does not lock anything

Locking is your job. You fill the exchange area in C *before* the block and read it *after* the block, so the lock must cover all three steps. The macro cannot see the C code around it.

```cpp
__TLOCK(x86)
    __stasm_x86[0] = 12;
    __stasm_x86[1] = 13;
    __stasm(x86,code,
        mov esi, __stasm_x86;
        mov eax,[esi+0];
        imul eax,[esi+4];
        mov [esi+8],eax;
    )
    int result = __stasm_x86[2];
__TUNLOCK(x86)
```

### Rules

1. **One lock per resource.** Use the same word you declared in `__stasm()`: `__TLOCK(x86)`, `__TLOCK(x87)`, `__TLOCK(mmx)`, `__TLOCK(sse1)`, `__TLOCK(avx)`. Blocks using different resources do not block each other.
2. **The lock covers the C write, the block and the C read.** Not just the block.
3. **Several resources: always lock them in the same order**, for example `x86 → x87 → mmx → sse1 → avx`, and unlock in reverse. Two threads locking in a different order will deadlock each other forever.
4. **No recursion while holding a lock.** If a function holding `__TLOCK(x86)` calls itself (or anything else that takes `__TLOCK(x86)`), the thread waits for itself forever.
5. **Never leave the locked region early.** A `return`, `break`, `goto` or C++ exception between `__TLOCK` and `__TUNLOCK` leaves the lock closed for good.
6. **`save,...` options use shared save areas too.** Lock the matching resource: `save,x87` → `__TLOCK(x87)`, `save,mmx` → `__TLOCK(mmx)`, `save,sse` → `__TLOCK(sse1)`, `save,avx` → `__TLOCK(avx)`.

### How the lock works

The lock is a spinlock built on a single atomic instruction, available in user mode (ring 3):

| CPU | Instruction |
|---|---|
| x86 / x86-64 | `XCHG` (implicit `LOCK`) |
| ARMv7 (Cortex-A, Cortex-M3/M4/M7) | `LDREX` / `STREX` + `DMB` |
| AArch64 | `LDAXR` / `STXR`, release with `STLR` |
| Cortex-M0/M0+ | disables interrupts (`CPSID I`), restores `PRIMASK` |

While waiting it executes `PAUSE` (`YIELD` on ARM) and gives up its time slice every 1000 spins (`stk::time::wait_us(0)`).

On bare-metal boards (no OS, no threads) the only "other thread" is an interrupt. Never take a lock inside an interrupt handler: if the interrupted code holds it, the handler waits forever.

### No locks at all: local variables

If you want recursion, nesting and threads without any locking, do not use the shared areas. Pass your own local C variables to the block as operands (`__stasmex`). Every call then has its own copy on the stack.

```cpp
int triple(int x){
    int y;
    __stasmex( mov eax, %[x]; imul eax, 3; mov %[y], eax; )
    __stasmex_o( [y] "=m" (y) )
    __stasmex_i( [x] "m" (x) )
    __stasmex_r( "eax", "cc" )
    __stasmex_end
    return y;
}
```

With GCC this requires `-masm=intel` on the command line. Without it GCC substitutes operands in AT&T syntax and the assembler rejects them (that was the root of bug reports 65897/65898). Clang behaves the same way.

### History

Earlier versions used a counting "semaphore" (`max--` / `++max`) to give up to `__STASM_MAX_PARARELL` threads their own slot. It was not atomic: two threads could take the same slot, and lost updates slowly leaked slots until every thread waited forever. It was also pointless, because the asm code reads a single global pointer (`__stasm_x86`) that every thread overwrote. It has been replaced by the per-resource spinlock described above.
