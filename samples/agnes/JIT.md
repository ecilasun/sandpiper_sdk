# Agnes JIT plan (NES CPU)

This note sketches a minimal JIT integration that preserves current timing and side‑effects.

## Goals
- Keep cycle accuracy and existing PPU/APU timing behavior.
- Preserve all memory‑mapped I/O side effects.
- Provide a safe fallback to the interpreter.

## Where to hook
- CPU fetch/decode/execute loop: [agnes.c](agnes.c#L984-L1037)
- Memory reads/writes: [agnes.c](agnes.c#L1039-L1116) and [agnes.c](agnes.c#L1125-L1167)
- Tick coordination: [agnes.c](agnes.c#L798-L840)

## Minimal JIT API (C)
Add a new module (e.g., `jit.c/.h`) with a tiny interface:

- `void jit_init(jit_t *jit, agnes_t *agnes);`
- `void jit_shutdown(jit_t *jit);`
- `bool jit_run_block(jit_t *jit, cpu_t *cpu, int *out_cycles);`
- `void jit_invalidate_all(jit_t *jit);`
- `void jit_invalidate_range(jit_t *jit, uint16_t start, uint16_t end);`

The `jit_run_block()` function:
- Returns `true` if it executed a compiled block.
- Writes the exact CPU cycles to `out_cycles`.
- Updates CPU registers and flags directly.
- Updates `cpu->pc` to the next instruction.

## Block format (conceptual)
A block is keyed by `(pc, jit_generation)` and stores:
- `entry_pc`
- `jit_generation`
- `exec_fn` (callable function pointer)
- Optional metadata: `max_cycles`, `estimated_icount`, or flags.

Each block should terminate on:
- Branch or jump
- `RTS/RTI/BRK`
- Interrupt boundary (optional mid‑block checks)
- Max instruction count (e.g., 32–128)

## Generation + invalidation
Add a `jit_generation` counter in `agnes_t` or `cpu_t`:
- Increment on mapper writes that change PRG bank mapping.
- Blocks compiled with a previous generation are discarded.

Invalidate by address when code is writable:
- When `cpu_write8` touches $6000+ (PRG‑RAM) or any known code‑backed RAM, call `jit_invalidate_range()`.
- For simplicity, start with `jit_invalidate_all()` for any write in $6000–$FFFF and later optimize.

## Memory access strategy
Inline **fast paths** inside compiled code for:
- Zero page and stack ($0000–$01FF)
- Internal RAM ($0000–$1FFF)

For all I/O or mapper regions, call the existing C functions:
- `cpu_read8` / `cpu_write8`

This preserves side effects for:
- PPU registers $2000–$3FFF
- APU registers $4000–$4017
- Controller reads $4016–$4017
- Mapper reads/writes $4020+

## Interrupts and stall
At the start of `jit_run_block()` (or each block):
- If `cpu->stall > 0`, return `false` so the interpreter handles it.
- If `cpu->interrupt != INTERRPUT_NONE`, return `false` so `cpu_tick()` handles it.

Optionally add a mid‑block check every N instructions for tighter timing.

## Cycle accounting
The JIT must reproduce the same cycle count as `cpu_tick`:
- Base cycles from the opcode table.
- Extra cycle on page‑cross if `page_cross_cycle` applies.
- Extra cycle from instruction implementation (if any).

## Integration point
In `cpu_tick` (top of function), add:
1. If `jit_run_block()` returns true, add cycles to `cpu->cycles` and return.
2. Otherwise, execute the existing interpreter path.

## Recommended first milestone
- Implement a “no‑codegen” JIT stub that caches decoded blocks and replays them using the existing per‑instruction handlers (still in C). This validates:
  - Block boundaries
  - Cycle accounting
  - Invalidation rules
- Then swap the replay with real machine code generation.

## Windows x64 codegen options
- AsmJit (recommended for C/C++ and x64)
- DynASM (compact, but more manual)

## Validation
- Run a ROM with interpreter and JIT, compare CPU state every N instructions.
- Verify frame checksum of the PPU output and APU sample counts.

---
If you want, I can add the minimal JIT scaffolding (`jit.h/.c`) and wire the hooks into `cpu_tick` + mapper writes.
