/*
SPDX-License-Identifier: MIT
*/
#include "jit.h"

#include <string.h>
#include <stdlib.h>

typedef struct agnes agnes_t;

typedef enum {
    INTERRPUT_NONE = 0,
    INTERRUPT_NMI = 1,
    INTERRUPT_IRQ = 2
} cpu_interrupt_t;

typedef struct cpu {
    agnes_t *agnes;
    uint16_t pc;
    uint8_t sp;
    uint8_t acc;
    uint8_t x;
    uint8_t y;
    uint8_t flag_carry;
    uint8_t flag_zero;
    uint8_t flag_dis_interrupt;
    uint8_t flag_decimal;
    uint8_t flag_overflow;
    uint8_t flag_negative;
    uint32_t stall;
    uint64_t cycles;
    cpu_interrupt_t interrupt;
} cpu_t;

typedef enum {
    ADDR_MODE_NONE = 0,
    ADDR_MODE_ABSOLUTE,
    ADDR_MODE_ABSOLUTE_X,
    ADDR_MODE_ABSOLUTE_Y,
    ADDR_MODE_ACCUMULATOR,
    ADDR_MODE_IMMEDIATE,
    ADDR_MODE_IMPLIED,
    ADDR_MODE_IMPLIED_BRK,
    ADDR_MODE_INDIRECT,
    ADDR_MODE_INDIRECT_X,
    ADDR_MODE_INDIRECT_Y,
    ADDR_MODE_RELATIVE,
    ADDR_MODE_ZERO_PAGE,
    ADDR_MODE_ZERO_PAGE_X,
    ADDR_MODE_ZERO_PAGE_Y
} addr_mode_t;

typedef int (*instruction_op_fn)(cpu_t *cpu, uint16_t addr, addr_mode_t mode);

typedef struct {
    const char *name;
    uint8_t opcode;
    uint8_t cycles;
    bool page_cross_cycle;
    addr_mode_t mode;
    instruction_op_fn operation;
} instruction_t;

#ifndef AGNES_JIT_MAX_BLOCKS
#define AGNES_JIT_MAX_BLOCKS 256
#endif

#ifndef AGNES_JIT_MAX_INSNS
#define AGNES_JIT_MAX_INSNS 64
#endif

#ifndef AGNES_JIT_ENABLE
#define AGNES_JIT_ENABLE 0
#endif

typedef struct jit_insn {
    uint8_t opcode;
    uint8_t size;
    uint8_t cycles;
    bool page_cross_cycle;
    addr_mode_t mode;
    instruction_op_fn operation;
    bool terminator;
} jit_insn_t;

typedef struct jit_block {
    bool valid;
    uint16_t entry_pc;
    uint16_t end_pc;
    uint32_t generation;
    uint8_t insn_count;
    jit_insn_t insn[AGNES_JIT_MAX_INSNS];
} jit_block_t;

instruction_t* agnes_instruction_get(uint8_t opc);
uint8_t agnes_instruction_get_size(addr_mode_t mode);
uint16_t agnes_get_instruction_operand(cpu_t *cpu, addr_mode_t mode, bool *out_pages_differ);
uint8_t agnes_cpu_read8(cpu_t *cpu, uint16_t addr);

static bool jit_is_block_terminator(uint8_t opcode) {
    switch (opcode) {
        case 0x00: // BRK
        case 0x10: // BPL
        case 0x20: // JSR
        case 0x30: // BMI
        case 0x40: // RTI
        case 0x4c: // JMP abs
        case 0x50: // BVC
        case 0x60: // RTS
        case 0x6c: // JMP ind
        case 0x70: // BVS
        case 0x90: // BCC
        case 0xb0: // BCS
        case 0xd0: // BNE
        case 0xf0: // BEQ
            return true;
        default:
            return false;
    }
}

static jit_block_t *jit_pick_block_slot(jit_t *jit) {
    uint32_t slot = 0;
    if (jit->blocks_count > 0) {
        slot = jit->next_slot++ % jit->blocks_count;
    }
    return jit->blocks ? &jit->blocks[slot] : NULL;
}

static jit_block_t *jit_find_block(jit_t *jit, uint16_t pc) {
    if (!jit || !jit->blocks) {
        return NULL;
    }
    for (uint32_t i = 0; i < jit->blocks_count; i++) {
        jit_block_t *blk = &jit->blocks[i];
        if (blk->valid && blk->entry_pc == pc && blk->generation == jit->generation) {
            return blk;
        }
    }
    return NULL;
}

static bool jit_compile_block(jit_t *jit, cpu_t *cpu, uint16_t pc, jit_block_t *out_blk) {
    if (!jit || !cpu || !out_blk) {
        return false;
    }
    memset(out_blk, 0, sizeof(*out_blk));
    out_blk->entry_pc = pc;
    out_blk->generation = jit->generation;

    uint16_t cur_pc = pc;
    for (uint32_t i = 0; i < AGNES_JIT_MAX_INSNS; i++) {
        uint8_t opcode = agnes_cpu_read8(cpu, cur_pc);
        instruction_t *ins = agnes_instruction_get(opcode);
        if (!ins || ins->operation == NULL) {
            out_blk->valid = false;
            return false;
        }

        uint8_t size = agnes_instruction_get_size(ins->mode);
        if (size == 0) {
            out_blk->valid = false;
            return false;
        }

        jit_insn_t *slot = &out_blk->insn[out_blk->insn_count++];
        slot->opcode = opcode;
        slot->size = size;
        slot->cycles = ins->cycles;
        slot->page_cross_cycle = ins->page_cross_cycle;
        slot->mode = ins->mode;
        slot->operation = ins->operation;
        slot->terminator = jit_is_block_terminator(opcode);

        cur_pc += size;
        if (slot->terminator) {
            break;
        }
    }

    out_blk->end_pc = cur_pc;
    out_blk->valid = out_blk->insn_count > 0;
    return out_blk->valid;
}

void jit_init(jit_t *jit, agnes_t *agnes) {
    if (!jit) {
        return;
    }
    memset(jit, 0, sizeof(*jit));
    jit->agnes = agnes;
    jit->enabled = AGNES_JIT_ENABLE ? true : false;
    jit->blocks_count = AGNES_JIT_MAX_BLOCKS;
    if (jit->blocks_count > 0) {
        jit->blocks = (jit_block_t *)malloc(sizeof(jit_block_t) * jit->blocks_count);
        if (jit->blocks) {
            memset(jit->blocks, 0, sizeof(jit_block_t) * jit->blocks_count);
        } else {
            jit->blocks_count = 0;
        }
    }
}

void jit_shutdown(jit_t *jit) {
    if (!jit) {
        return;
    }
    if (jit->blocks) {
        free(jit->blocks);
    }
    jit->blocks = NULL;
    jit->blocks_count = 0;
    jit->enabled = false;
    jit->generation = 0;
    jit->agnes = NULL;
}

bool jit_run_block(jit_t *jit, cpu_t *cpu, int *out_cycles) {
    if (!jit || !cpu || !out_cycles) {
        return false;
    }
    if (!jit->enabled) {
        return false;
    }
    if (cpu->stall > 0 || cpu->interrupt != INTERRPUT_NONE) {
        return false;
    }

    jit_block_t *blk = jit_find_block(jit, cpu->pc);
    if (!blk) {
        blk = jit_pick_block_slot(jit);
        if (!blk || !jit_compile_block(jit, cpu, cpu->pc, blk)) {
            return false;
        }
    }

    int cycles = 0;
    for (uint32_t i = 0; i < blk->insn_count; i++) {
        if (cpu->stall > 0 || cpu->interrupt != INTERRPUT_NONE) {
            break;
        }

        jit_insn_t *insn = &blk->insn[i];
        bool page_crossed = false;
        uint16_t addr = agnes_get_instruction_operand(cpu, insn->mode, &page_crossed);

        cpu->pc += insn->size;

        cycles += insn->cycles;
        cycles += insn->operation(cpu, addr, insn->mode);

        if (page_crossed && insn->page_cross_cycle) {
            cycles += 1;
        }

        if (insn->terminator) {
            break;
        }
    }

    *out_cycles = cycles;
    return cycles > 0;
}

void jit_invalidate_all(jit_t *jit) {
    if (!jit) {
        return;
    }
    jit->generation++;
}

void jit_invalidate_range(jit_t *jit, uint16_t start, uint16_t end) {
    (void)start;
    (void)end;
    if (!jit) {
        return;
    }
    jit->generation++;
}
