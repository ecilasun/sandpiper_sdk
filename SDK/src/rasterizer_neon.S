/*
 * rasterizer_neon.S - ARMv7 NEON optimized rasterizer
 * 4x4 edge evaluation for triangle rasterization
 */

    .syntax unified
    .thumb
    .text
    .align 4

/*
 * void rasterizer_eval_4x4_neon_impl(
 *     int32_t e0, int32_t e1, int32_t e2,        r0-r2
 *     int32_t ddx0, int32_t ddx1, int32_t ddx2,  r3, [sp+0], [sp+4]
 *     int32_t ddy0, int32_t ddy1, int32_t ddy2,  [sp+8], [sp+12], [sp+16]
 *     uint16_t *mask_out                         [sp+20] - now just a single uint16_t
 * )
 */
    .globl rasterizer_eval_4x4_neon_impl
    .type rasterizer_eval_4x4_neon_impl, %function

rasterizer_eval_4x4_neon_impl:
    push {r4-r11, lr}               /* Save callee-saved regs: 9 regs × 4 = 36 bytes */
    vpush {d8-d15}                  /* Save NEON regs: 8 regs × 8 = 64 bytes */
                                    /* Total stack adjustment: 100 bytes */
    
    /* Get parameters from stack (adjusting for 100 byte frame) */
    ldr r4, [sp, #100]              /* ddx1 - was at sp[0], now sp[100] */
    ldr r5, [sp, #104]              /* ddx2 */
    ldr r6, [sp, #108]              /* ddy0 */
    ldr r7, [sp, #112]              /* ddy1 */
    ldr r8, [sp, #116]              /* ddy2 */
    ldr r9, [sp, #120]              /* mask_out pointer */
    
    /* Save e0, e1, e2, ddx0 in callee-saved regs that we just pushed */
    mov r10, r0                     /* r10 = e0 */
    mov r11, r1                     /* r11 = e1 */
    mov r12, r2                     /* r12 = e2 */
    /* r3 already has ddx0 */
    
    /* Set up NEON constants */
    vdup.s32 q0, r3                 /* q0 = [ddx0, ddx0, ddx0, ddx0] */
    vdup.s32 q1, r4                 /* q1 = [ddx1, ddx1, ddx1, ddx1] */
    vdup.s32 q2, r5                 /* q2 = [ddx2, ddx2, ddx2, ddx2] */
    vdup.s32 q3, r6                 /* q3 = [ddy0, ddy0, ddy0, ddy0] */
    vdup.s32 q4, r7                 /* q4 = [ddy1, ddy1, ddy1, ddy1] */
    vdup.s32 q5, r8                 /* q5 = [ddy2, ddy2, ddy2, ddy2] */
    
    /* Create X offset [0, 1, 2, 3] */
    movw r0, #0
    movw r1, #1
    movw r2, #2
    movw r3, #3
    vmov.s32 d12[0], r0
    vmov.s32 d12[1], r1
    vmov.s32 d13[0], r2
    vmov.s32 d13[1], r3             /* q6 = [0, 1, 2, 3] */
    
    vmul.s32 q7, q6, q0             /* q7 = x_offset for edge 0 */
    vmul.s32 q8, q6, q1             /* q8 = x_offset for edge 1 */
    vmul.s32 q9, q6, q2             /* q9 = x_offset for edge 2 */
    
    /* Accumulate mask bits in r0 (will build final 16-bit mask) */
    mov r0, #0
    mov r1, #0                      /* row counter */
    
.row_loop:
    cmp r1, #4
    bge .done
    
    /* Compute y offset for this row */
    mul r2, r1, r6                  /* r2 = row * ddy0 */
    mul r3, r1, r7                  /* r3 = row * ddy1 */
    mul r4, r1, r8                  /* r4 = row * ddy2 */
    
    /* Compute edge values: e[i] + x_offset[i] + y_offset */
    vdup.s32 q10, r10               /* q10 = [e0, e0, e0, e0] */
    vadd.s32 q10, q10, q7           /* Add x offsets */
    vdup.s32 q13, r2
    vadd.s32 q10, q10, q13          /* Add y offset */
    
    vdup.s32 q11, r11               /* q11 = [e1, e1, e1, e1] */
    vadd.s32 q11, q11, q8
    vdup.s32 q13, r3
    vadd.s32 q11, q11, q13
    
    vdup.s32 q12, r12               /* q12 = [e2, e2, e2, e2] */
    vadd.s32 q12, q12, q9
    vdup.s32 q13, r4
    vadd.s32 q12, q12, q13
    
    /* Test: e >= 0 for all three edges (inside triangle) */
    vcge.s32 q10, q10, #0           /* q10 (e0 >= 0) */
    vcge.s32 q11, q11, #0           /* q11 (e1 >= 0) */
    vcge.s32 q12, q12, #0           /* q12 (e2 >= 0) */
    
    /* AND them together */
    vand.u32 q10, q10, q11
    vand.u32 q10, q10, q12          /* q10 = final mask (4 × 32-bit) */
    
    /* Extract bits: shift right by 31 to get 0 or 1 for each pixel */
    vshr.u32 q10, q10, #31          /* q10 = [0/1, 0/1, 0/1, 0/1] as 32-bit */
    
    /* Extract scalar values and pack into bits */
    vmov.u32 r2, d20[0]             /* pixel 0 of row */
    vmov.u32 r3, d20[1]             /* pixel 1 of row */
    vmov.u32 r4, d21[0]             /* pixel 2 of row */
    vmov.u32 r5, d21[1]             /* pixel 3 of row */
    
    /* Pack bits: bit position = row * 4 + pixel_col */
    /* Calculate base bit position for this row (row * 4) using lsl */
    /* r1 = row, so row*4 = r1 << 2 */
    lsl r2, r2, r1, lsl #2          /* pixel 0 << (row * 4) */
    orr r0, r0, r2
    
    /* pixel 1: shift by row*4 + 1 */
    add r2, r1, r1, lsl #2          /* r2 = r1 + r1*4 = r1*5 (wrong!) */
    /* Actually, we need a temp for row*4. Since we can't nest shifts, calculate it first */
    lsl r14, r1, #2                 /* r14 = row * 4 */
    add r2, r14, #1                 /* r2 = row * 4 + 1 */
    lsl r3, r3, r2
    orr r0, r0, r3
    
    add r2, r14, #2                 /* row * 4 + 2 */
    lsl r4, r4, r2
    orr r0, r0, r4
    
    add r2, r14, #3                 /* row * 4 + 3 */
    lsl r5, r5, r2
    orr r0, r0, r5
    
    add r1, r1, #1
    b .row_loop
    
.done:
    /* Store final 16-bit mask */
    strh r0, [r9]
    
    vpop {d8-d15}
    pop {r4-r11, pc}

    .size rasterizer_eval_4x4_neon_impl, .-rasterizer_eval_4x4_neon_impl
    .section .note.GNU-stack,"",%progbits
