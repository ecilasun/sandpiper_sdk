/*
SPDX-License-Identifier: MIT
*/
#ifndef jit_h
#define jit_h

#include <stdbool.h>
#include <stdint.h>

#ifndef AGNES_INTERNAL
#define AGNES_INTERNAL
#endif

#ifndef AGNES_JIT_INVALIDATE_RAM
#define AGNES_JIT_INVALIDATE_RAM 0
#endif

typedef struct agnes agnes_t;
typedef struct cpu cpu_t;

typedef struct jit_insn jit_insn_t;
typedef struct jit_block jit_block_t;

typedef struct jit {
    agnes_t *agnes;
    uint32_t generation;
    bool enabled;

    uint32_t next_slot;
    jit_block_t *blocks;
    uint32_t blocks_count;
} jit_t;

AGNES_INTERNAL void jit_init(jit_t *jit, agnes_t *agnes);
AGNES_INTERNAL void jit_shutdown(jit_t *jit);
AGNES_INTERNAL bool jit_run_block(jit_t *jit, cpu_t *cpu, int *out_cycles);
AGNES_INTERNAL void jit_invalidate_all(jit_t *jit);
AGNES_INTERNAL void jit_invalidate_range(jit_t *jit, uint16_t start, uint16_t end);

#endif /* jit_h */
